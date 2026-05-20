/**
 *  @file       SelfCommission.c
 *
 *  @brief      Debugging features
 *
 *  @details
 *
 *  @section
 *
 *  @copyright  Copyright 2013-$Date: 2016/01/11 12:30:13CET $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SelfCommissioning.h"
#include "SelfCommissioning_prv.h"
#include "ClrkPark.h"
#include "Pi.h"
#include "PWMModulation.h"
#include "MathCalc.h"
#include "IQMath.h"
#include "Filters.h"
#include "ObserverPmsm.h"
#include "Mci.h"
#include "Mcl.h"
#include "MotorSafetyMgr.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef struct
{
	// input
	float32 Vdc;                                    //!< [V]             DC-Bus Voltage
	ABC_COOR_SYST_F_TYPE Is_ABC;                    //!< [A]             Motor Phase Currents
	float32 Speed_Rot_Ref;                          //!< [rad/s]         Speed Reference

	// output
	ABC_COOR_SYST_F_TYPE  Vs_ABC;                   //!< [V]     motor phase reference voltage in abc system
	ABC_COOR_SYST_F_TYPE  Duty;                     //!< [%]     PWM Duties
	ABC_COOR_SYST_F_TYPE  Duty_bc;                  //!< [%]     PWM Duties after modulation and before compensation
	uint8 Lowers_On;                                //!< [n/a]   Turn on all lowers IGBTs
	float32 Speed_Rot_Est;                          //!< [rad/s]         Speed Reference
	float32 Torque;                           		//!< [Nm]    Motor Torque

} SELFCOMMISSIONING_IO_TYPE;



typedef enum SC_CONTROL_MODE
{
    SC_OPEN_LOOP  = 0,             // [ 0 ]
    SC_BEMF_OBSERVER = 1,              // [ 3 ]
    SC_CM_DUMMY  = 256
} SC_CONTROL_MODE_ENUM_TYPE;


typedef struct
{
	float32 IsD_Ref;
	float32 IsQ_Ref;
	float32 Speed_Rot_Ref_El_Abs;
	float32 Speed_Rot_Est_El_Abs;
	float32 Speed_Rot_Ref_Mech_Abs;
	float32 Speed_Rot_Est_Mech_Abs;
	DQ_COOR_SYST_F_TYPE Vs_DQ;
	DQ_COOR_SYST_F_TYPE Is_DQ;
	SC_CONTROL_MODE_ENUM_TYPE Control_Mode;
}FOC_BPM_QUANTITIES_TYPE;


SELFCOMMISSIONING_IO_TYPE Sc_IO_Data;
FOC_BPM_QUANTITIES_TYPE Sc_Data; //! Field oriented control main quantities (current/Voltage on D/Q axis, speed, dc-bus Voltage)

uint16 Sc_Master_Cmd_Force;  			// set the master commander speed command to mci
sint32 Sc_Master_Cmd_Speed;  			//mci speed command reference [rpm]
sint16 Sc_Master_Cmd_Acc;	  			//mci acceleration commnad reference [rpm/s]
sint16 Sc_Master_Cmd_Fdbk;  			//different from zero if there is a fault in mci command


#ifndef SQRT3_INV
    #define SQRT3_INV    	(float32) (1.0f / 1.7320508075688772935274463415059f)
#endif

#ifndef RPM_TO_RADS
    #define RPM_TO_RADS     0.10471975511965977461542144610932f
#endif


#ifndef INV_3
    #define INV_3          	(float32) (1.0f/3.0f)
#endif


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


// Foc Bpm parameters definition
typedef struct
{
    //---------------- HARDWARE CONFIGURATION PARAMETERS - begin ------
    uint16 IsrThr;          //!< ratio between fast control loop and slow control loop
    float32 Ts;


    float32 Pole_Pairs;
    float32 Rs;
    float32 Ld;
    float32 Lq;
    float32 Phim;           //!< Permanent Magnet Rotor Flux

 	//----------------- HARDWARE CONFIGURATION PARAMETERS - end -------

	//------------- MOTOR TABLE PARAMETERS  - begin ------
   // Startup
    float32 Iq_Open_Loop;           //!< Initial value of Speed regulator integral state
    float32 Speed_Close_Loop;      //!< Speed to switch from Injection to Bemf Observer control mode

}SETTING_FILE_PARAMS_TYPE;

uint16 Sc_Inverter_Compensation_Selector = SC_INVERTER_COMPENSATION;
uint16 Space_Vector_Mod_Selector = 0;


static ABC_COOR_SYST_F_TYPE	Sc_IsABC_Copy; //!< motor phase current in ABC reference used in FOC loop
static ABC_COOR_SYST_F_TYPE Sc_VsABC_bc;      //!< output pwm duties from space vector modulation
static ABC_COOR_SYST_F_TYPE Sc_VsABC;      //!< output pwm duties from space vector modulation
static ABC_COOR_SYST_F_TYPE Sc_VsABC_rec;      //!< output pwm duties from space vector modulation
static ALPHA_BETA_COOR_SYST_F_TYPE     Sc_VsAlphaBeta; //! motor phase Voltage in alpha-beta reference system
static ALPHA_BETA_COOR_SYST_F_TYPE     Sc_VsAlphaBeta_Rec; //! motor phase Voltage in alpha-beta reference system
static ALPHA_BETA_COOR_SYST_F_TYPE     Sc_IsAlphaBeta; //! motor phase current in alpha-beta reference system

// Pi controller parameters structures
static PI_CONTROLLER_F_TYPE Sc_Current_Controller_D; //!< Current-D PI parameters
static PI_CONTROLLER_F_TYPE Sc_Current_Controller_Q; //!< Current-Q PI parameters
static PI_CONTROLLER_F_TYPE Sc_Speed_Controller; //!< Speed PI parameters
static PI_CONTROLLER_F_TYPE Sc_Voltage_Controller; //!< Voltage PI parameters

static float32 Sc_V_Err_Voltage;
static float32 Sc_I_Err_Voltage;


static uint8 Sc_Sector; //sector of space vector modulation

// Injection & Observer
static BEMF_OBSERVER_PARAMS_F_TYPE          Sc_Bemf_Observer_Params;           //!< bemf observer parameters
static IIR1_F_TYPE                          Sc_IIR1_Filter_Dq_Obs;
static TRACKING_OBSERVER_PARAMS_F_TYPE    	Sc_Tracking_Observer_Params;       //!< tracking observer parameters



static SIN_COS_F_TYPE 	  Sc_Sin_Cos_Rotor_Position;  		//!< sin/cos of rotor-flux position used in current park trasformation
static SIN_COS_F_TYPE 	  Sc_Sin_Cos_Rotor_Position_Voltage;  //!< sin/cos of rotor-flux position used in Voltage inverse park trasformation

static float32 Sc_Position_Flux_Est;                 //!<  rotor-flux position
static float32 Sc_Position_Flux_Est_Voltage;		    //!<  rotor-flux position compensated for Voltage transformation

static float32   Sc_Speed_Flux_El_Est;                 	//!< estiamted rotor-flux speed

static SETTING_FILE_PARAMS_TYPE Sc_Prm; //!< setting file FOC parameters

// Counters
static uint16 Sc_Slow_Controloop_Cnt; //!< counter used to activate the slow-control loop (speed and Voltage control)
static uint16 Sc_Speed_Sign;


uint16 Sc_Injection_Flag; 			// High Sc_FrequencyInj injection is actived and foc control is by-passed
uint16 Sc_Injection_Flag_Old; 			// High Sc_FrequencyInj injection is actived and foc control is by-passed


//Voltage Injection Quantities and Parameters
SIN_COS_F_TYPE Sc_SinCosInj;                    // sinus and cosinus of the injected Voltage phase angle in  notation
ALPHA_BETA_COOR_SYST_F_TYPE Sc_VAphaBetaInj;    //!< injected Voltage in alpha-beta components
float32 Sc_FrequencyInj;                        //!< Sc_FrequencyInj of the injected Voltage [Hz]
float32 Sc_VoltageInj;                        	//!< Amplitude of the injected Voltage [V]
float32 Sc_VAmplCont;                           //!< Amplitude of the injected Voltage [V]
float32 Sc_Current_Inj_Ampl;
float32 Sc_PositionInj;                        //!< phase angle of the injected Voltage in Q2.30 notation
float32 Sc_PositionInjOld;
float32 Sc_PositionOpenLoop;
DQ_COOR_SYST_F_TYPE Is_DQ_Injection;
DQ_COOR_SYST_F_TYPE Vs_DQ_Injection;

//Currents max min amplitude variables
float32 Sc_Sc_IAmpl_Min_Tmp;
float32 Sc_IAmpl_Max_Tmp;
float32 Sc_IAmpl_K_1;
float32 Sc_IAmpl_K_2;
float32 Sc_IAmpl_Min;
float32 Sc_IAmpl_Max;
float32 Sc_VAmpl;
float32 Sc_IAmpl;
float32 Sc_Bemf_Ampl;

float32 Sc_VAmpl_Min_Tmp;
float32 Sc_VAmpl_Max_Tmp;
float32 Sc_IAmpl_K_2;
float32 Sc_VAmpl_Min;
float32 Sc_VAmpl_Max;



//Pwm Modulation Parameters
MATHCALC_LUT_EXT_F_TYPE   PwmModulationPrm;
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
void ScBpm__Initialize(void);
void ScBpm__PwmHandler(void);
static void Parameter_Initialize(void);
static void ScBpm__ResetFocState(void);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SelfCommission and its variables
 *
 *  @details    The selected debugging features are initialized
 *
 */
void SelfCommissioning__Initialize(void)
{
    ScBpm__Initialize();

    Sc_IO_Data.Is_ABC.A = 0.0f;
    Sc_IO_Data.Is_ABC.B = 0.0f;
    Sc_IO_Data.Is_ABC.C = 0.0f;
    Sc_IO_Data.Vdc      = 0.0f;

	Sc_Master_Cmd_Force = 0;
	Sc_Master_Cmd_Speed = 0;
	Sc_Master_Cmd_Acc = SC_DEFAULT_ACCEL;
	Sc_Master_Cmd_Fdbk = 0;

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Pwm handler for SelfCommissioning. Rate: every 62.5 us
 *
 */
void SelfCommissioning__RunningHandler(void)
{

    // Adc Reading
	Sc_IO_Data.Is_ABC.A =  Mcl_IO.Is_ABC.A;
	Sc_IO_Data.Is_ABC.B =  Mcl_IO.Is_ABC.B;
	Sc_IO_Data.Is_ABC.C =  Mcl_IO.Is_ABC.C;
	Sc_IO_Data.Vdc = Mcl_IO.Vdc;
	Sc_IO_Data.Speed_Rot_Ref  = Mcl_IO.Speed_Rot_Ref;


	//FOC LOOP
	ScBpm__PwmHandler();

	Mcl_IO.Duty.A = Sc_IO_Data.Duty.A;
	Mcl_IO.Duty.B = Sc_IO_Data.Duty.B;
	Mcl_IO.Duty.C = Sc_IO_Data.Duty.C;

}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Slow handler for SelfCommission. Rate: every 25 ms
 *
 */
void SelfCommissioning__25msHandler(void)
{

	if(Sc_Injection_Flag != Sc_Injection_Flag_Old)
	{
		if(Sc_Injection_Flag != 0)
		{
			Sc_Master_Cmd_Force = 1;
			Sc_Master_Cmd_Speed = SC_DEFAULT_SPEED;
		}
		else if(MotorSafetyMgr__DoesClassAHaveAccessToPwm() == TRUE)
		{
			Sc_Master_Cmd_Force = 1;
			Sc_Master_Cmd_Speed = 0;
		}
	}

	Sc_Injection_Flag_Old = Sc_Injection_Flag;



	if(Sc_Master_Cmd_Force == 1)
	{
		if(Sc_Master_Cmd_Speed != 0)
		{
			Sc_Master_Cmd_Fdbk = Mci__Run(MOTOR0, Sc_Master_Cmd_Speed, Sc_Master_Cmd_Acc);

			if(Sc_Master_Cmd_Fdbk == MCI_CMD_ACCEPTED)
			{
				Sc_Master_Cmd_Force = 0;
			}
		}
		else
		{
			Mci__Stop(MOTOR0, Sc_Master_Cmd_Acc);

			Sc_Master_Cmd_Force = 0;
		}

	}

	if(MotorSafetyMgr__DoesClassAHaveAccessToPwm() == FALSE)
	{
		ScBpm__ResetFocState();
	}

}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================








//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialization of Foc components parameters.
 *  @details    This routine initialise the structures of FOC components parameters (Pi regulators, Flux estimator..)
 *              with setting file values.
 *
 *  @param[in]
 *  @param[out]
 *  @return
 */
static void Parameter_Initialize(void)
{

	Sc_Prm.IsrThr = 32;
	Sc_Prm.Ts = SC_TS;

	Sc_Prm.Pole_Pairs = SC_POLE_PAIRS;
	Sc_Prm.Rs = SC_RS;
	Sc_Prm.Ld = SC_LD/1000.0f;
	Sc_Prm.Lq = SC_LQ/1000.0f;
	Sc_Prm.Phim = 0.0750f;


    //-------bemf observer params init  - begin ------
    Sc_Bemf_Observer_Params.Pi_D.Kp = 40.0f;
    Sc_Bemf_Observer_Params.Pi_D.Ki = 32000.0f*Sc_Prm.Ts;
    Sc_Bemf_Observer_Params.Pi_D.Upper_Limit = 500.0f;
    Sc_Bemf_Observer_Params.Pi_D.Lower_Limit = -500.0f;

    Sc_Bemf_Observer_Params.Pi_Q.Kp = 40.0f;
	Sc_Bemf_Observer_Params.Pi_Q.Ki = 32000.0f*Sc_Prm.Ts;
	Sc_Bemf_Observer_Params.Pi_Q.Upper_Limit = 500.0f;
	Sc_Bemf_Observer_Params.Pi_Q.Lower_Limit = -500.0f;

    Sc_Bemf_Observer_Params.I_Coeff = (Sc_Prm.Ld/(Sc_Prm.Ld+Sc_Prm.Ts*Sc_Prm.Rs));
    Sc_Bemf_Observer_Params.U_Coeff = (Sc_Prm.Ts/(Sc_Prm.Ld+Sc_Prm.Ts*Sc_Prm.Rs));
    Sc_Bemf_Observer_Params.WI_Coeff = (Sc_Prm.Ts*Sc_Prm.Lq/(Sc_Prm.Ld+Sc_Prm.Ts*Sc_Prm.Rs));
    Sc_Bemf_Observer_Params.E_Coeff = (Sc_Prm.Ts/(Sc_Prm.Ld+Sc_Prm.Ts*Sc_Prm.Rs));
    //-------bemf observer params init  - end ------


    //-------tracking observer params init  - begin ------
    Sc_Tracking_Observer_Params.Pi.Kp = SC_TRACKINGOBSERVER_KP;
    Sc_Tracking_Observer_Params.Pi.Ki = SC_TRACKINGOBSERVER_KI*Sc_Prm.Ts;
    Sc_Tracking_Observer_Params.Pi.Upper_Limit = 20000.0f;
    Sc_Tracking_Observer_Params.Pi.Lower_Limit = -20000.0f;
    Sc_Tracking_Observer_Params.Integ_Parameters.Ki = Sc_Prm.Ts;

    //-------IIR Filter ---------//
    Sc_IIR1_Filter_Dq_Obs.coeff.b0 = 0.026733f;
    Sc_IIR1_Filter_Dq_Obs.coeff.b1 = 0.026733f;
    Sc_IIR1_Filter_Dq_Obs.coeff.a1 = -0.946533f;
    Filters__IIR1InitF(&Sc_IIR1_Filter_Dq_Obs);
    //-------tracking observer params init  - end ------


    //-------D current control loop params init  - begin ------
    Sc_Current_Controller_D.Kp = SC_CURRENTCONTROLLER_KP;
    Sc_Current_Controller_D.Ki = SC_CURRENTCONTROLLER_KI*Sc_Prm.Ts;
    Sc_Current_Controller_D.Upper_Limit = 256.0f;
    Sc_Current_Controller_D.Lower_Limit = -256.0f;
    //-------D current control loop params init  - end ------

	//-------Q current control loop params init  - begin ------
	Sc_Current_Controller_Q.Kp = SC_CURRENTCONTROLLER_KP;
	Sc_Current_Controller_Q.Ki = SC_CURRENTCONTROLLER_KI*Sc_Prm.Ts;
	Sc_Current_Controller_Q.Upper_Limit = 256.0f;
	Sc_Current_Controller_Q.Lower_Limit = -256.0f;
	//-------Q current control loop params init  - end ------



    //-------Voltage control loop params init  - begin ------
    Sc_Voltage_Controller.Kp = 2.0f/1000.0f;
    Sc_Voltage_Controller.Ki = 8.0f/1000.0f*Sc_Prm.Ts*Sc_Prm.IsrThr;
    Sc_Voltage_Controller.Upper_Limit = 0.0f;
    Sc_Voltage_Controller.Lower_Limit = -5.0f;
    //-------Voltage control loop params init  - end ------


    //-------Speed control loop params init  - begin ------
    Sc_Speed_Controller.Kp = SC_SPEEDCONTROLLER_KP*(60.0f/(2.0f*PI))/1000.0f;
    Sc_Speed_Controller.Ki = SC_SPEEDCONTROLLER_KI*(60.0f/(2.0f*PI))*Sc_Prm.Ts*Sc_Prm.IsrThr/1000.0f;
    Sc_Speed_Controller.Upper_Limit = SC_SPEEDCONTROLLER_LIMIT;
    Sc_Speed_Controller.Lower_Limit = -SC_SPEEDCONTROLLER_LIMIT;
    //-------Speed control loop params init  - end ------


    //Pwm Parameters
    PwmModulationPrm.ptr_LUT      = Sc_Dutycycle_By_Current_LUT;
    PwmModulationPrm.sizeof_lut   = &sc_sizeof_inv_comp_lut;
    PwmModulationPrm.step_inv     = &sc_step_inv_pwm_comp;


    //OpenLoop
    Sc_Prm.Iq_Open_Loop = SC_OPENLOOP_CURRENT;
    Sc_Prm.Speed_Close_Loop = SC_CLOSELOOP_SPEED*RPM_TO_RADS*Sc_Prm.Pole_Pairs;

}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Reset all FOC quantities.
 *  @details    This routine reset all FOC quantities, it has to be called at every time the pwm is switched off (motor stop or free down ramp).
 *
 *
 *  @param[in]
 *  @param[out]
 *  @return
 */
void ScBpm__ResetFocState(void)
{

    // Pi States Reset
    Sc_Current_Controller_D.Integ_K_1 = 0;
    Sc_Current_Controller_Q.Integ_K_1 = 0;
    Sc_Speed_Controller.Integ_K_1 = 0;
    Sc_Voltage_Controller.Integ_K_1 = 0;


	// Foc quantities reset
    Sc_Data.Is_DQ.D = 0;
    Sc_Data.Is_DQ.Q = 0;
	Sc_Data.IsD_Ref = 0;
	Sc_Data.IsQ_Ref = 0;
    Sc_Data.Vs_DQ.D = 0;
	Sc_Data.Speed_Rot_Ref_El_Abs = 0;
	Sc_Data.Speed_Rot_Est_El_Abs = 0;
	Sc_Data.Speed_Rot_Ref_Mech_Abs = 0;
	Sc_Data.Speed_Rot_Est_Mech_Abs = 0;

    //Voltage quantities reset
	Sc_VsAlphaBeta.Alpha = 0;
	Sc_VsAlphaBeta.Beta = 0;


	// Bemf Observer
    Sc_Bemf_Observer_Params.Bemf_Est.D = 0;
    Sc_Bemf_Observer_Params.Bemf_Est.Q = 0;
    Sc_Bemf_Observer_Params.Current_Est.D = 0;
    Sc_Bemf_Observer_Params.Current_Est.Q = 0;
    Sc_Bemf_Observer_Params.Pi_D.Integ_K_1 = 0;
    Sc_Bemf_Observer_Params.Pi_Q.Integ_K_1 = 0;
    Sc_Bemf_Observer_Params.Theta_Flux_Error = 0;


    // Tracking Observer
    Sc_Tracking_Observer_Params.Pi.Integ_K_1  = 0;
    Sc_Tracking_Observer_Params.Omega_Est = 0;
    Sc_Tracking_Observer_Params.Theta_Est = 0;


    Sc_PositionOpenLoop = 0;

    // Duty reset
    Sc_VsABC.A = 0;
    Sc_VsABC.B = 0;
    Sc_VsABC.C = 0;


	// Foc Interface output reset
	Sc_IO_Data.Duty.A = 0;
	Sc_IO_Data.Duty.B = 0;
	Sc_IO_Data.Duty.C = 0;
	Sc_IO_Data.Torque = 0;
	Sc_Data.Control_Mode = SC_OPEN_LOOP;

	Sc_Slow_Controloop_Cnt = Sc_Prm.IsrThr;


	Sc_Sc_IAmpl_Min_Tmp  = 10.0f;
	Sc_IAmpl_Max_Tmp  = 0;
	Sc_IAmpl_Min      = 10.0f;
	Sc_IAmpl_Max      = 0;
	Sc_IAmpl          = 0;
	Sc_Bemf_Ampl      = 0;
}







//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Speed and Position estimation for a PMSM motor.
 *  @details   this routine estimates the position and the speed of rotor flux for a BPM motor, using high Sc_FrequencyInj injection for low speed and a
 *             bemf observer for high speed.
 *
 *
 *
 *  @param[in]
 *  @param[out]
 *  @param[in]
 *  @return
 */
void Observer(void)
{
    float32 filter_out;                      // output of IIR filter of position estimation error


    if(Sc_Data.Control_Mode != SC_BEMF_OBSERVER)
    {

        if(Sc_Data.Speed_Rot_Ref_El_Abs >= Sc_Prm.Speed_Close_Loop)
        {
            // Dq Observer state initialization
            Sc_Bemf_Observer_Params.Bemf_Est.D = 0;
            Sc_Bemf_Observer_Params.Bemf_Est.Q = -Sc_Speed_Flux_El_Est*Sc_Prm.Phim;
            Sc_Bemf_Observer_Params.Current_Est.D = Sc_Data.Is_DQ.D;
            Sc_Bemf_Observer_Params.Current_Est.Q = Sc_Data.Is_DQ.Q;
            Sc_Bemf_Observer_Params.Pi_D.Integ_K_1 = 0;
            Sc_Bemf_Observer_Params.Pi_Q.Integ_K_1 = Sc_Bemf_Observer_Params.Bemf_Est.Q;
            Sc_Bemf_Observer_Params.Theta_Flux_Error = 0;


            // Tracking Observer
            Sc_Tracking_Observer_Params.Pi.Integ_K_1  = Sc_Data.Speed_Rot_Ref_El_Abs;
            Sc_Tracking_Observer_Params.Omega_Est = Sc_Data.Speed_Rot_Ref_El_Abs;
            Sc_Tracking_Observer_Params.Theta_Est = Sc_PositionOpenLoop;

            // IIR Filter
            Filters__IIR1InitF(&Sc_IIR1_Filter_Dq_Obs);

            Sc_Data.Control_Mode = SC_BEMF_OBSERVER;
        }

        Sc_PositionOpenLoop = Sc_PositionOpenLoop + Sc_Data.Speed_Rot_Ref_El_Abs*Sc_Prm.Ts;

        if(Sc_PositionOpenLoop > PI)
		{
        	Sc_PositionOpenLoop -= (2.0f * PI);
		}

		if(Sc_PositionOpenLoop < (-PI))
		{
			Sc_PositionOpenLoop += (2.0f * PI);
		}

		Sc_Position_Flux_Est = Sc_PositionOpenLoop;
		Sc_Speed_Flux_El_Est = Sc_Data.Speed_Rot_Ref_El_Abs;


    }
    else  // Observer Control Mode
    {
    	ObserverPmsm__BemfObsDQF(&Sc_Data.Is_DQ, &Sc_Data.Vs_DQ, Sc_Speed_Flux_El_Est, &Sc_Bemf_Observer_Params);

        //Bemf amplitude calculation
        Sc_Bemf_Ampl = MathCalc__SqrtF((Sc_Bemf_Observer_Params.Bemf_Est.D * Sc_Bemf_Observer_Params.Bemf_Est.D)+ (Sc_Bemf_Observer_Params.Bemf_Est.Q*Sc_Bemf_Observer_Params.Bemf_Est.Q));


        // IIR Filter for output error
        filter_out = Filters__IIR1F(Sc_Bemf_Observer_Params.Theta_Flux_Error, &Sc_IIR1_Filter_Dq_Obs);

        Sc_Position_Flux_Est  = ObserverPmsm__TrackingObsF(filter_out, &Sc_Tracking_Observer_Params);
        Sc_Speed_Flux_El_Est  = Sc_Tracking_Observer_Params.Omega_Est;
    }


    Sc_Data.Speed_Rot_Est_El_Abs = Sc_Speed_Flux_El_Est;
    Sc_Data.Speed_Rot_Est_Mech_Abs = Sc_Data.Speed_Rot_Est_El_Abs/Sc_Prm.Pole_Pairs;

    // angle compensation
    Sc_Position_Flux_Est_Voltage = Sc_Position_Flux_Est +  Sc_Data.Speed_Rot_Est_El_Abs*Sc_Prm.Ts;

    // calculation of rotor flux position sinus and cosinus
    MathCalc__SinCosF(Sc_Position_Flux_Est,&Sc_Sin_Cos_Rotor_Position);
    MathCalc__SinCosF(Sc_Position_Flux_Est_Voltage,&Sc_Sin_Cos_Rotor_Position_Voltage);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Fast Control Loop initialisation.
 *  @details    In this routine are called all initialisation functions.
 *
 *
 *  @param[in]
 *  @param[out]
 *  @param[in]
 *  @return
 */
void ScBpm__Initialize(void)
{

	// initialize FOC and WM variables with Setting File values
	Parameter_Initialize();


	// reset FOC quantities
	ScBpm__ResetFocState(); //ResetFocState();

}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     FOC Control Loop for Bpm.
 *  @details   Field oriented control loop routine for Bpm:
 *             - d axis: rotor flux position
 *             - q axis: rotor flux quadrature axis
 *
 *             This routine is composed by three parts:
 *             - Input processing: tachometer signal processing
 *                                 motor speed reference absolute value
 *                                 motor phase currents swapping in order to work with a positive speed reference.
 *                                 dc Voltage filtering
 *
 *             - Slow Control Loop: slow dynamics controls (speed and Voltage) -> they generates current references
 *                                          - Iq reference is set by speed control
 *                                          - Id reference is set by Voltage control
 *                                  slow quantities calculation (torque, mean torque and vibration index)
 *                                  update of speed regulator gains/ fir filter taps/ flux estimator parameters
 *
 *             - Current Control Loop + Flux Position Estimation:                                        -
 *                                  flux estimator
 *                                  fast dynamics controls (currents) -> they generates Voltage references
 *                                  space vector modulation: -> it generates the duties
 *                                  duties swapping in accord to the speed sign.
 *
 *
 *  @param[in]
 *  @param[out]
 *  @param[in]
 *  @return
 */
void ScBpm__PwmHandler(void)
{

	float32 temp_common_mode;

    //---------------------------------------------------------------------------------------------------------------------
    // Input Processing - begin
    Sc_Data.Speed_Rot_Ref_Mech_Abs = MATHCALC__ABS(Sc_IO_Data.Speed_Rot_Ref);
    Sc_Data.Speed_Rot_Ref_El_Abs = Sc_Data.Speed_Rot_Ref_Mech_Abs*Sc_Prm.Pole_Pairs;

    if(Sc_IO_Data.Speed_Rot_Ref >= 0)
    {// positive speed reference

    	Sc_Speed_Sign = 0;
    }
    else
    {// negative speed reference

    	Sc_Speed_Sign = 1;
    }

    // Speed Sign Current SWAP -begin
    Sc_IsABC_Copy.B = Sc_IO_Data.Is_ABC.B;

    if (Sc_Speed_Sign != 0)
    {
        Sc_IsABC_Copy.A = Sc_IO_Data.Is_ABC.C;
        Sc_IsABC_Copy.C = Sc_IO_Data.Is_ABC.A;
    }
    else
    {
        Sc_IsABC_Copy.A = Sc_IO_Data.Is_ABC.A;
        Sc_IsABC_Copy.C = Sc_IO_Data.Is_ABC.C;
    }
    // Speed Sign Current SWAP -end


    // Input Processing - end
    //---------------------------------------------------------------------------------------------------------------------







    //---------------------------------------------------------------------------------------------------------------------
    // Slow Control Loop begin: Speed and Voltage Control - begin

    Sc_Slow_Controloop_Cnt++;

    if((Sc_Slow_Controloop_Cnt >= Sc_Prm.IsrThr)&&(Sc_Injection_Flag == 0))
    {
        Sc_Slow_Controloop_Cnt = 0;


        if (Sc_Data.Control_Mode == SC_OPEN_LOOP)
        {// Injection Alignment or Initial Rotor Position Detection

            Sc_Data.IsD_Ref = 0;

            if(Sc_Data.IsQ_Ref <= Sc_Prm.Iq_Open_Loop)
            {
            	Sc_Data.IsQ_Ref = Sc_Prm.Iq_Open_Loop + 0.04f;
            }

            Sc_Speed_Controller.Integ_K_1  = Sc_Prm.Iq_Open_Loop;
        }
        else
        {// Injection Startup or Bemf Observer

            // speed regulator execution
        	Sc_Speed_Controller.Err = Sc_Data.Speed_Rot_Ref_Mech_Abs - Sc_Data.Speed_Rot_Est_Mech_Abs;
        	Pi__CalcPiBackCalcF(&Sc_Speed_Controller);
            Sc_Data.IsQ_Ref = Sc_Speed_Controller.Out;
            Sc_Data.IsD_Ref = 0;

            //Flux Weakining
            // Voltage regulator execution
			Sc_V_Err_Voltage = Sc_Current_Controller_Q.Upper_Limit - Sc_Data.Vs_DQ.Q;
			Sc_I_Err_Voltage = (Sc_Data.IsQ_Ref - Sc_Data.Is_DQ.Q)*Sc_Prm.Rs;
			Sc_Voltage_Controller.Err = Sc_V_Err_Voltage - Sc_I_Err_Voltage;

			Pi__CalcPiBackCalcF(&Sc_Voltage_Controller);

			Sc_Data.IsD_Ref = Sc_Voltage_Controller.Out;
        }
    }
    // Slow Control Loop begin: Speed and Voltage Control - end
    //---------------------------------------------------------------------------------------------------------------------




    //---------------------------------------------------------------------------------------------------------------------
    // Current Control Loop + Flux Position Estimation - begin

    // Forward Clarke Transform - transforms ABC currents to Alpha/Beta coordinate system
    ClrkPark__DirectClarkeF(&Sc_IsABC_Copy,&Sc_IsAlphaBeta);


    // Forward Park Transform - transforms stator currents to DQ coordinate system
    ClrkPark__DirectParkF(&Sc_IsAlphaBeta, &Sc_Data.Is_DQ, &Sc_Sin_Cos_Rotor_Position);

    // Flux Speed and Position estimation
    Observer();


    // torque estimation
    Sc_IO_Data.Torque = (3.0f/2.0f*Sc_Prm.Pole_Pairs)*( \
    					Sc_Prm.Phim*Sc_Data.Is_DQ.Q  +\
    					(Sc_Prm.Ld-Sc_Prm.Lq)*Sc_Data.Is_DQ.D*Sc_Data.Is_DQ.Q);



    if(Sc_Injection_Flag)
    {// High Sc_FrequencyInj Injection is actived


    	/////////////////// Injection angle calculation ////////////
    	Sc_PositionInj = Sc_PositionInj + Sc_FrequencyInj*2.0f*PI*Sc_Prm.Ts;


		if(Sc_PositionInj > PI)
		{
			Sc_PositionInj -= (2.0f * PI);
		}

		if(Sc_PositionInj < (-PI))
		{
			Sc_PositionInj += (2.0f * PI);
		}

		MathCalc__SinCosF(Sc_PositionInj,&Sc_SinCosInj);
		/////////////////////////////////////////////////////////////


		if(Sc_Injection_Flag == 2)
		{
			/////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////
			///////// Voltage Injection----------------- -- start ///////

			Sc_VAphaBetaInj.Alpha = Sc_VoltageInj*Sc_SinCosInj.Cos;
			Sc_VAphaBetaInj.Beta = Sc_VoltageInj*Sc_SinCosInj.Sin;


			Sc_VsAlphaBeta.Alpha = Sc_VAmplCont;
			Sc_VsAlphaBeta.Beta  = 0;

			Sc_VsAlphaBeta.Alpha += Sc_VAphaBetaInj.Alpha;
			Sc_VsAlphaBeta.Beta  += Sc_VAphaBetaInj.Beta;
			/////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////
		}
		else if(Sc_Injection_Flag == 1)
		{
			/////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////
			///////// Corrent Injection----------------- -- start ///////
			ClrkPark__DirectParkF(&Sc_IsAlphaBeta, &Is_DQ_Injection, &Sc_SinCosInj);


			// D,Q-current regulators
			Sc_Current_Controller_D.Err = Sc_Current_Inj_Ampl - Is_DQ_Injection.D;
			Sc_Current_Controller_Q.Err = 0.0f - Is_DQ_Injection.Q;

			// D - current controller limit update
			Sc_Current_Controller_D.Upper_Limit = Sc_IO_Data.Vdc*SQRT3_INV;
			Sc_Current_Controller_D.Lower_Limit = -Sc_Current_Controller_D.Upper_Limit;

			// D - current regulator
			Pi__CalcPiBackCalcF(&Sc_Current_Controller_D);
			Vs_DQ_Injection.D = Sc_Current_Controller_D.Out;

			// Q - current controller limit update
			Sc_Current_Controller_Q.Upper_Limit = MathCalc__SqrtF((Sc_IO_Data.Vdc*Sc_IO_Data.Vdc)*(1.0f/3.0f) - (Vs_DQ_Injection.D*Vs_DQ_Injection.D)); // 2.30 = 17.15<<15
			Sc_Current_Controller_Q.Lower_Limit = -Sc_Current_Controller_Q.Upper_Limit;

			// Q - current regulator
			Pi__CalcPiBackCalcF(&Sc_Current_Controller_Q);
			Vs_DQ_Injection.Q = Sc_Current_Controller_Q.Out;

			// inverse Park transformation - transforms rotor Voltage D,Q --> Alpha,Beta
			ClrkPark__InverseParkF(&Vs_DQ_Injection, &Sc_VsAlphaBeta, &Sc_SinCosInj);

			/////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////
		}



        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        ///////// Voltage Reconstraction--------------- start ///////
        Sc_VsABC_rec.A = (Sc_VsABC_bc.A * Sc_IO_Data.Vdc);
        Sc_VsABC_rec.B = (Sc_VsABC_bc.B * Sc_IO_Data.Vdc);
        Sc_VsABC_rec.C = (Sc_VsABC_bc.C * Sc_IO_Data.Vdc);


        temp_common_mode = (Sc_VsABC_rec.A) + (Sc_VsABC_rec.B) + (Sc_VsABC_rec.C);
        temp_common_mode = temp_common_mode * INV_3 ;

        Sc_VsABC_rec.A -= temp_common_mode;
        Sc_VsABC_rec.B -= temp_common_mode;
        Sc_VsABC_rec.C -= temp_common_mode;

        ClrkPark__DirectClarkeF(&Sc_VsABC_rec, &Sc_VsAlphaBeta_Rec);


       //Voltage amplitude calculation
        Sc_VAmpl = MathCalc__SqrtF((Sc_VsAlphaBeta_Rec.Alpha*Sc_VsAlphaBeta_Rec.Alpha)+ (Sc_VsAlphaBeta_Rec.Beta*Sc_VsAlphaBeta_Rec.Beta));
        //////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////




        //Current amplitude calculation
        Sc_IAmpl = MathCalc__SqrtF((Sc_IsAlphaBeta.Alpha*Sc_IsAlphaBeta.Alpha) + (Sc_IsAlphaBeta.Beta*Sc_IsAlphaBeta.Beta));



        //Min Max Voltage amplitude calculation
		if(Sc_VAmpl_Max_Tmp < Sc_VAmpl)
		{
			Sc_VAmpl_Max_Tmp = Sc_VAmpl;
		}

		if(Sc_VAmpl_Min_Tmp > Sc_VAmpl)
		{
			Sc_VAmpl_Min_Tmp = Sc_VAmpl;

		}



        //Min Max Current amplitude calculation
        if(Sc_IAmpl_Max_Tmp < Sc_IAmpl)
        {
           Sc_IAmpl_Max_Tmp = Sc_IAmpl;
        }

        if(Sc_Sc_IAmpl_Min_Tmp > Sc_IAmpl)
        {
            Sc_Sc_IAmpl_Min_Tmp = Sc_IAmpl;

        }


        if((Sc_PositionInjOld<0)&&(Sc_PositionInj>0))
        {/* [ if different sign between speed reference and platform speed --> set inversion speed ] */


            Sc_IAmpl_Max = Sc_IAmpl_Max_Tmp;
            Sc_IAmpl_Max_Tmp = 0.0f;
            Sc_IAmpl_Min = Sc_Sc_IAmpl_Min_Tmp;

            Sc_Sc_IAmpl_Min_Tmp = 10.0f;


            Sc_VAmpl_Max = Sc_VAmpl_Max_Tmp;
			Sc_VAmpl_Max_Tmp = 0.0f;
			Sc_VAmpl_Min = Sc_VAmpl_Min_Tmp;

			Sc_VAmpl_Min_Tmp = 200.0f;
        }

        Sc_PositionInjOld = Sc_PositionInj;


    }
    else
    {// Injection Startup or Bemf Observer
        // D,Q-current regulators
    	Sc_Current_Controller_D.Err = Sc_Data.IsD_Ref - Sc_Data.Is_DQ.D;
    	Sc_Current_Controller_Q.Err = Sc_Data.IsQ_Ref - Sc_Data.Is_DQ.Q;

        // D - current controller limit update
        Sc_Current_Controller_D.Upper_Limit = Sc_IO_Data.Vdc*SQRT3_INV;
        Sc_Current_Controller_D.Lower_Limit = -Sc_Current_Controller_D.Upper_Limit;

        // D - current regulator
        Pi__CalcPiBackCalcF(&Sc_Current_Controller_D);
        Sc_Data.Vs_DQ.D = Sc_Current_Controller_D.Out;

        // Q - current controller limit update
        Sc_Current_Controller_Q.Upper_Limit = MathCalc__SqrtF((Sc_IO_Data.Vdc*Sc_IO_Data.Vdc)*(1.0f/3.0f) - (Sc_Data.Vs_DQ.D*Sc_Data.Vs_DQ.D)); // 2.30 = 17.15<<15
        Sc_Current_Controller_Q.Lower_Limit = -Sc_Current_Controller_Q.Upper_Limit;

        // Q - current regulator
        Pi__CalcPiBackCalcF(&Sc_Current_Controller_Q);
        Sc_Data.Vs_DQ.Q = Sc_Current_Controller_Q.Out;

        // inverse Park transformation - transforms rotor Voltage D,Q --> Alpha,Beta
        ClrkPark__InverseParkF(&Sc_Data.Vs_DQ, &Sc_VsAlphaBeta, &Sc_Sin_Cos_Rotor_Position_Voltage);
    }

	//ReCalculate Vd Vq if injection is enabled
    ClrkPark__DirectParkF( &Sc_VsAlphaBeta, &Sc_Data.Vs_DQ, &Sc_Sin_Cos_Rotor_Position_Voltage);


	// space vector modulation (including ripple compensation)
    Sc_Sector = PwmModulation__SpaceVectorModulationF(Sc_IO_Data.Vdc, &Sc_VsAlphaBeta, &Sc_VsABC_bc, FALSE);

	Sc_VsABC.A = Sc_VsABC_bc.A;
	Sc_VsABC.B = Sc_VsABC_bc.B;
	Sc_VsABC.C = Sc_VsABC_bc.C;


	// compensation strategy
	if(Sc_Inverter_Compensation_Selector == SC_INVERTER_LOSS_COMP)
	{
		PwmModulation__InverterLossCompF(&Sc_IO_Data.Is_ABC, &Sc_VsABC, &PwmModulationPrm);
	}
	else if(Sc_Inverter_Compensation_Selector == SC_DEADTIME_COMP)
	{
		PwmModulation__DeadtimeCompensationGradientF(&Sc_IO_Data.Is_ABC, &Sc_VsABC);
	}


    // Speed Sign Duties SWAP -begin
    Sc_IO_Data.Duty.B = Sc_VsABC.B;

    if (Sc_Speed_Sign != 0)
    {
        Sc_IO_Data.Duty.A = Sc_VsABC.C;
        Sc_IO_Data.Duty.C = Sc_VsABC.A;

        Sc_IO_Data.Speed_Rot_Est = -Sc_Data.Speed_Rot_Est_Mech_Abs;
    }
    else
    {
        Sc_IO_Data.Duty.A = Sc_VsABC.A;
        Sc_IO_Data.Duty.C = Sc_VsABC.C;

        Sc_IO_Data.Speed_Rot_Est = Sc_Data.Speed_Rot_Est_Mech_Abs;
    }
    // Speed Sign Duties SWAP -end

    // Current Control Loop + Flux Position Estimation - end
    //---------------------------------------------------------------------------------------------------------------------
}








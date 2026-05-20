/**
 *  @file       Mcl.c
 *  @brief      Motor Control Loop 3-phase motors
 *  @details    This module implements the motor oriented control loop for 3-phase motors (DD, BPM, CIM).
 *  @author     alessio.beato/luigi.fagnano
 *  $Header:
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
*/
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
//MCL macro-block inclusion
#include "Mcl.h"
#include "McMathCalc_macros.h"
#include "Filters.h"
#include "ClrkPark.h"
#include "Mci_prm.h"
#include "MotorSafetyMgr.h"

#if (THERMAL_MODEL_EXE == ENABLED)
#include "ThermalModel.h"
#endif
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef RPM_TO_RADS
    #define RPM_TO_RADS                 0.10471975511965977461542144610932f
#endif

#ifndef STOP_RAMP_DURING_INVERSION
    #define STOP_RAMP_DURING_INVERSION  DISABLED
#endif

#ifndef MAX_U16
    #define MAX_U16                     65535
#endif

#ifndef MAX_UINT32
    #define MAX_UINT32                  (uint32)((1ULL<<32)-1)
#endif

#define INV_3                           (float32)(1.0f/3.0f)
#define INV_10                          (float32)(1.0f/10.0f)
#define TWO_PI			                (float32)(6.283185307179586476925286766559f)


typedef enum
{
    STOP_COAST_DOWN_REQUESTED = 0,                      //!< 0 - Stop by coasting down requested
    STOP_REQUESTED,                                     //!< 1 - Stop with braking is requested
    STOP_LOWERS_ON_REQUESTED,                           //!< 2 - Stop with lowers on is requested
    RUNNING_MIN_SPEED_STOP_REACHED,                     //!< 3 - Stop requested and minimum speed was reached

    MANUAL_DC_VOLT_INJECTION,                           //!< 4 - inject DC voltage manually
    MANUAL_AC_VOLT_INJECTION,                           //!< 5 - inject AC voltage manually
    MANUAL_DC_CURRENT_INJECTION,                        //!< 6 - inject DC current manually
    MANUAL_AC_CURRENT_INJECTION,                        //!< 7 - inject AC current manually
} MCL_FLOW_MANAGER_TYPE;



typedef enum
{
    MCL_STARTING_NOT_DEFINED = 0,
    MCL_RESISTANCE_ESTIMATION,
    MCL_BUILD_MOTOR_FLUX,
    MCL_STARTING_FINISHED
} MCL_STARTING_STATE_TYPE;


MCL_CS_TYPE Mcl_Cs;                                         //!< MCL control specific variables

//Input Output Structures declarations ------ begin
MCL_IO_TYPE Mcl_IO; //!< MCL IO

//Input Processing IO
MCL_INPUT_PROC_IO_F_TYPE Mcl_Input_Proc_IO;

//Observer IO
static MCL_OBSERVER_IO_F_TYPE Mcl_Observer_IO;

//Speed Controller IO
static MCL_SPEED_CTRL_IO_F_TYPE Mcl_Speed_Ctrl_IO;

//DQ Reference Generator Controller IO
static MCL_DQ_REF_IO_F_TYPE Mcl_DQ_Ref_IO;

//DQ Controllers IO
MCL_DQ_CTRL_IO_F_TYPE Mcl_DQ_Ctrl_IO;

//Pwm IO
MCL_PWM_IO_F_TYPE Mcl_Pwm_IO;
////Input Output Structures declarations ------ end

MCL_PARAMS_DISPL1_TYPE*  Mcl_Params_Displ1;                 //!< MCL parameters displacement 1
static MCL_PARAMS_DISPL3_TYPE*  Mcl_Params_Displ3;                 //!< MCL parameters displacement 3
static MCL_PARAMS_DISPL4_TYPE*  Mcl_Params_Displ4;                 //!< MCL parameters displacement 4
static MCL_PARAMS_DISPL9_TYPE*  Mcl_Params_Displ9;                 //!< MCL parameters displacement 9
static MCL_PARAMS_DISPL10_TYPE* Mcl_Params_Displ10;                //!< MCL parameters displacement 10
static MCL_PARAMS_DISPL11_TYPE* Mcl_Params_Displ11;                //!< MCL parameters displacement 11
static MCL_PARAMS_DISPL13_TYPE* Mcl_Params_Displ13;                //!< MCL parameters displacement 11

/* additional structures of parameters */
MCL_INPUT_PROC_JOINT_PARAMS_TYPE MclInput_Params;
static MCLOBSERVER_JOINT_PARAMS_TYPE    MclObserver_Params;
static MCLSPEEDCTRL_JOINT_PARAMS_TYPE   MclSpeedCtrl_Params;
static MCLDQREF_JOINT_PARAMS_TYPE       MclDqRef_Params;
MCLPWM_JOINT_PARAMS_TYPE        MclPwm_Params;

MCL_QUANTITIES_TYPE Mcl_Quantities;                         //!< MCL quantities

static RES_EST_IO_TYPE Res_Est_IO;

static MCL_STARTING_STATE_TYPE Mcl_Starting_State;

static volatile MCL_FLOW_MANAGER_TYPE Mcl_Flow_Manager;

static float32 Manual_Inject_Level;                         //!< [A or V]- define the level used in the manual injection
static float32 Manual_Inject_Param;                         //!< [rad or Hz]- define the position of frequency of a given method
static float32 Manual_Pos_Reference;                        //!< [rad] - angle to calculate direct/inverse transformation

static float32 I1_Rms_Sum;
static float32 I2_Rms_Sum;
static float32 I3_Rms_Sum;
static float32 Mean_Speed_Sum;
static uint16 N_Samples;
static float32 N_Samples_inv;


static uint16 Over_Torque_Time_Counter;                    //!< [5ms ticks] time when the torque is too high
//! MCL_FLOW_MANAGER_TYPE controls the algorithm flow
#define MCL_SET_FLOW_MANAGER(mcl_flowcontrol)   BIT_SET(Mcl_Flow_Manager, (mcl_flowcontrol))
#define MCL_CLR_FLOW_MANAGER(mcl_flowcontrol)   BIT_CLR(Mcl_Flow_Manager, (mcl_flowcontrol))

//! Define multiples test cases in one shot because the compiler is optimizing and ignoring the BIT_TEST macro
#define MANUAL_VOLTAGE_INJECTION_FLAGS  (((MCL_FLOW_MANAGER_TYPE)1<<(MANUAL_DC_VOLT_INJECTION)) + ((MCL_FLOW_MANAGER_TYPE)1<<(MANUAL_AC_VOLT_INJECTION)))
#define MANUAL_CURRENT_INJECTION_FLAGS  (((MCL_FLOW_MANAGER_TYPE)1<<(MANUAL_DC_CURRENT_INJECTION)) + (((MCL_FLOW_MANAGER_TYPE)1<<(MANUAL_AC_CURRENT_INJECTION))))


//! Define the flags used to declare running finished.
#define RUNNING_FINISHED_FLOW_FLAGS     ((MCL_FLOW_MANAGER_TYPE)1<<STOP_LOWERS_ON_REQUESTED)  +   \
                                        ((MCL_FLOW_MANAGER_TYPE)1<<STOP_COAST_DOWN_REQUESTED) +   \
                                        ((MCL_FLOW_MANAGER_TYPE)1<<STOP_REQUESTED)


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static BOOL_TYPE Estimate_Stator_Resistance(void);
static void Calc_MeanSpeed_CurrentRMS(void);
static float32 Calc_Temperature_By_Resistance(void);
static float32 Calc_Resistance_By_Temperature(void);
#if (MCL__ELECTRICAL_POWER == ENABLED)
static void Calc_ElectricalPower(void);
#endif
#if (MCL__SHAFT_POWER == ENABLED)
static void Calc_ShaftPower(void);
#endif
static void Calc_TorqueForApplication(void);
static void TimeHandler(void);
static void Flags_Management(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Loop initialization.
 *  @details    In this routine are called all initialization functions.
 *
 *
 *  @param[in]     
 *  @param[out]     
 *  @param[in]      
 *  @return        
 */
void Mcl__Initialize(void)
{
    Mcl_Cs.flags.all                        = 0;
    Mcl_Cs.All_Devices_Off                  = &Mcl_IO.All_Devices_Off;
    Mcl_Cs.Lowers_On                        = &Mcl_IO.Lowers_On;
    Mcl_Cs.Speed_Rot_Ref_Abs                = &Mcl_Quantities.Speed_Rot_Ref_Abs;
    Mcl_Cs.Speed_Rotor_Observed             = &Mcl_Quantities.Speed_Rot;
    Mcl_Cs.Speed_Rotor_Observed_Mech        = &Mcl_Quantities.Speed_Rot_Mech;
    Mcl_Cs.Speed_Rotor_Observed_Mech_Abs    = &Mcl_Quantities.Speed_Rot_Mech_Abs;
    Mcl_Cs.Elapsed_Time_After_Starting      = &Mcl_Quantities.Elapsed_Time_After_Starting;
    Mcl_Cs.Id_stator_current                = &Mcl_Quantities.Id_stator_current;
    Mcl_Cs.Iq_stator_current                = &Mcl_Quantities.Iq_stator_current;
    Mcl_Cs.Id_Rotor_Current                 = &Mcl_Quantities.Id_rotor_current;
    Mcl_Cs.Torque                           = &Mcl_Quantities.Torque;
    Mcl_Cs.Stator_Resistance                = &Mcl_Quantities.Stator_Resistance;
    Mcl_Cs.Vs_Flux                          = &Mcl_Quantities.Vs_Flux;
    Mcl_Cs.Flux_Ref                         = &Mcl_Quantities.Flux_Ref;
    Mcl_Cs.Flux_Limit_By_Speed              = &Mcl_Quantities.Flux_Limit_By_Speed;
    Mcl_Cs.Stator_Flux_Mag                  = &Mcl_Quantities.Stator_Flux_Mag;
    Mcl_Cs.Load_Angle_Sin_Cos               = &Mcl_Quantities.Load_Angle_Sin_Cos;
    Mcl_Cs.Ldq_Ind                          = &Mcl_Quantities.Ldq_Ind;
    Mcl_Cs.Ldq_Cross_Ind                    = &Mcl_Quantities.Ldq_Cross_Ind;
    Mcl_Cs.Lqd_Cross_Ind                    = &Mcl_Quantities.Lqd_Cross_Ind;
    Mcl_Cs.Is_Abs                           = &Mcl_Quantities.Is_Abs;
    Mcl_Cs.DcInjection_Angle                = &Mcl_Quantities.DcInjection_Angle;
    Mcl_Cs.Rotor_Flux                       = &Mcl_Quantities.Rotor_Flux;
    Mcl_Cs.Delta_Speed_Rot_Ref              = &Mcl_Quantities.Delta_Speed_Rot_Ref;

    //////////////////////////////////////////////////////////////
	//----------- Input Processing Interface Set ------------------
	Mcl_Input_Proc_IO.Is_ABC                = &Mcl_IO.Is_ABC;
	Mcl_Input_Proc_IO.Vdc                   = &Mcl_IO.Vdc;
	Mcl_Input_Proc_IO.Duty                  = &Mcl_IO.Duty_bc;
	Mcl_Input_Proc_IO.Is_Alpha_Beta         = &Mcl_Quantities.Is_Alpha_Beta;
	Mcl_Input_Proc_IO.Vs_Max_Filt           = &Mcl_Quantities.Vs_Max_Filt;
	Mcl_Input_Proc_IO.Vs_Alpha_Beta_Rec     = &Mcl_Quantities.Vs_Alpha_Beta_Rec;
	Mcl_Input_Proc_IO.Vabc_Rec              = &Mcl_Quantities.Vabc_Rec;

	Mcl_Input_Proc_IO.Ctrl_Specific         = &Mcl_Cs;
	//-----------------------------------------------------------




	//////////////////////////////////////////////////////////////
	//------------------ Observer Interface Set -----------------
    Mcl_Observer_IO.Is_Alpha_Beta           = &Mcl_Quantities.Is_Alpha_Beta;
    Mcl_Observer_IO.Vs_Alpha_Beta           = &Mcl_Quantities.Vs_Alpha_Beta_Rec;
    Mcl_Observer_IO.Sin_Cos_Position_Flux   = &Mcl_Quantities.Sin_Cos_Position_Flux;
    Mcl_Observer_IO.Load_Angle              = &Mcl_Quantities.Load_Angle;
    Mcl_Observer_IO.Speed_Rotor_Observed    = &Mcl_Quantities.Speed_Rot;
    Mcl_Observer_IO.Vdc                     = &Mcl_IO.Vdc;
    Mcl_Observer_IO.Rotor_Position          = &Mcl_Quantities.Rotor_Position;
    Mcl_Observer_IO.Is_ABC                  = &Mcl_IO.Is_ABC;

    Mcl_Observer_IO.Ctrl_Specific           = &Mcl_Cs;
	//------------------------------------------------------------




	//////////////////////////////////////////////////////////////
	//------- Speed Controller Interface Set ---------------------
	Mcl_Speed_Ctrl_IO.Speed_Rot_Ref         = &Mcl_IO.Speed_Rot_Ref;
	Mcl_Speed_Ctrl_IO.Speed_Rot             = &Mcl_Quantities.Speed_Rot_Mech;
	Mcl_Speed_Ctrl_IO.Torque_Ref            = &Mcl_Quantities.Torque_Ref;

	Mcl_Speed_Ctrl_IO.Ctrl_Specific         = &Mcl_Cs;
	//------------------------------------------------------------




	//////////////////////////////////////////////////////////////
	//-------- DQ Reference Generator Interface set --------------

	Mcl_DQ_Ref_IO.Torque_Ref    = &Mcl_Quantities.Torque_Ref;
    Mcl_DQ_Ref_IO.Force_D_Ref   = &Mcl_Quantities.Force_Stator_Flux_Ref;
    Mcl_DQ_Ref_IO.Force_Q_Ref   = &Mcl_Quantities.Force_Torque_Ref;
    Mcl_DQ_Ref_IO.Vdc           = &Mcl_IO.Vdc;
	Mcl_DQ_Ref_IO.Q_Ref         = &Mcl_Quantities.Q_Torque_Ref;
	Mcl_DQ_Ref_IO.D_Ref         = &Mcl_Quantities.Flux_Ref;

	Mcl_DQ_Ref_IO.Ctrl_Specific = &Mcl_Cs;
	//------------------------------------------------------------




    //////////////////////////////////////////////////////////////
    //----------------- DQ controllers Interface set  ------------
    Mcl_DQ_Ctrl_IO.Q_Ref            = &Mcl_Quantities.Q_Torque_Ref;
    Mcl_DQ_Ctrl_IO.Q_Est            = &Mcl_Quantities.Torque;
    Mcl_DQ_Ctrl_IO.Vdc              = &Mcl_IO.Vdc;
    Mcl_DQ_Ctrl_IO.D_Ref            = &Mcl_Quantities.Flux_Ref;
    Mcl_DQ_Ctrl_IO.D_Est            = &Mcl_Quantities.Stator_Flux_Mag;
    Mcl_DQ_Ctrl_IO.Vs_DQ            = &Mcl_Quantities.Vs_DQ;
    Mcl_DQ_Ctrl_IO.Vs_Alpha_Beta    = &Mcl_Quantities.Vs_Alpha_Beta;
    Mcl_DQ_Ctrl_IO.Sin_Cos          = &Mcl_Quantities.Sin_Cos_Position_Flux;
    Mcl_DQ_Ctrl_IO.Force_Out_Volt   = &Mcl_Quantities.Force_Out_Volt;

    Mcl_DQ_Ctrl_IO.Ctrl_Specific    = &Mcl_Cs;
    //------------------------------------------------------------




    //////////////////////////////////////////////////////////////
    //-------- Pwm Interface Set -----------------------
    Mcl_Pwm_IO.Vs_Alpha_Beta        = &Mcl_Quantities.Vs_Alpha_Beta;
    Mcl_Pwm_IO.Duty                 = &Mcl_IO.Duty;
    Mcl_Pwm_IO.Duty_bc              = &Mcl_IO.Duty_bc;
    Mcl_Pwm_IO.Vdc                  = &Mcl_IO.Vdc;
    Mcl_Pwm_IO.Is_ABC               = &Mcl_IO.Is_ABC;

    Mcl_Pwm_IO.Ctrl_Specific        = &Mcl_Cs;
    //------------------------------------------------------------




    //////////////////////////////////////////////////////////////
    //-------- Estimation Parameters Interface Set -----------------------
    Res_Est_IO.Ctrl_Specific        = &Mcl_Cs;
    Res_Est_IO.level_injection_out  = &Mcl_Quantities.Level_Injection;
    Res_Est_IO.vs_voltage_abs       = &Mcl_Quantities.Vs_Voltage;
    //------------------------------------------------------------



	// Initialize and reset some variables
	Mcl__RunningReset();

	Mcl_Flow_Manager = (MCL_FLOW_MANAGER_TYPE)0;

#if (THERMAL_MODEL_EXE == ENABLED)
	ThermalModel__Initialize();
#endif
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Mcl Paramaterer Initialization.
 *
 *  @param      none
 *  @return     none
 */
void Mcl__ParamsInitialize(void)
{

    if(Mcl_Cs.flags.bit.params_ready == 1)
    {

        /* Prepare structures of joint parameters */
        MclInput_Params.InputProcPrm            = &Mcl_Params_Displ1->InputProcPrm;
        MclInput_Params.InputProcAddPrm         = &Mcl_Params_Displ13->InputProcAddPrm;

        MclObserver_Params.ObsPrm               = &Mcl_Params_Displ1->ObserverPrm;
        MclObserver_Params.ObsAddPrm            = &Mcl_Params_Displ13->ObserverAddPrm;
        MclObserver_Params.Ld_lut.ptr_LUT       = Mcl_Params_Displ3->Ld_Lut;
        MclObserver_Params.Ld_lut.sizeof_lut    = &Mcl_Params_Displ3->sizeof_Ld_Lq_Lut;
        MclObserver_Params.Ld_lut.step_inv      = &Mcl_Params_Displ3->step_inv_L_Lut;

        MclObserver_Params.Lq_lut.ptr_LUT       = Mcl_Params_Displ3->Lq_Lut;
        MclObserver_Params.Lq_lut.sizeof_lut    = &Mcl_Params_Displ3->sizeof_Ld_Lq_Lut;
        MclObserver_Params.Lq_lut.step_inv      = &Mcl_Params_Displ3->step_inv_L_Lut;

        MclObserver_Params.Ldq_lut.ptr_LUT      = Mcl_Params_Displ3->Ldq_Lut;
        MclObserver_Params.Ldq_lut.sizeof_lut   = &Mcl_Params_Displ3->sizeof_Ld_Lq_Lut;
        MclObserver_Params.Ldq_lut.step_inv     = &Mcl_Params_Displ3->step_inv_L_Lut;

        if (Mcl_Params_Displ11 != NULL)
        {
            MclObserver_Params.Rotor_Flux_lut.ptr_LUT    = Mcl_Params_Displ11->Flux_Lut;
            MclObserver_Params.Rotor_Flux_lut.sizeof_lut = &Mcl_Params_Displ11->sizeof_Flux_Lut;
            MclObserver_Params.Rotor_Flux_lut.step_inv   = &Mcl_Params_Displ11->step_inv_Flux_Lut;
        }
        else
        {
            MclObserver_Params.Rotor_Flux_lut.ptr_LUT = NULL; // the rotor flux lut is disabled
        }

        MclSpeedCtrl_Params.SpeedCtrlPrm        = &Mcl_Params_Displ4->SpeedCtrlPrm;
        MclSpeedCtrl_Params.SpeedCtrlAddPrm     = &Mcl_Params_Displ13->SpeedCtrlAddPrm;
        MclSpeedCtrl_Params.MTPVPrm.ptr_LUT     = Mcl_Params_Displ10->Tq_Limit_Lut;
        MclSpeedCtrl_Params.MTPVPrm.sizeof_lut  = &Mcl_Params_Displ10->sizeof_mtpv;
        MclSpeedCtrl_Params.MTPVPrm.step_inv    = &Mcl_Params_Displ10->step_inv_mtpv;

        MclDqRef_Params.DqRefPrm                = &Mcl_Params_Displ1->DQRefPrm;
        MclDqRef_Params.DqRefAddPrm             = &Mcl_Params_Displ13->DQRefAddPrm;
        MclDqRef_Params.MTPAPrm.ptr_LUT         = Mcl_Params_Displ9->Mtpa_Lut;
        MclDqRef_Params.MTPAPrm.sizeof_lut      = &Mcl_Params_Displ9->sizeof_mtpa_lut;
        MclDqRef_Params.MTPAPrm.step_inv        = &Mcl_Params_Displ9->step_inv_mtpa;

        MclPwm_Params.PwmPrm                    = &Mcl_Params_Displ1->PwmPrm;

        Mcl_Quantities.Stator_Resistance        = Mcl_Params_Displ1->ObserverPrm.Stator_Resistance;

        // Init DC Injection Angle
        Mcl_Quantities.DcInjection_Angle = 0.0f;


        ResistanceEstimation__Initialize(&Mcl_Params_Displ1->Res_Est_Param);
        SpeedRefGen__SetSpeedParams(Mcl_Params_Displ1->Mcl_SpeedRef_Params.SpeedRef_Default_Accel);

        //Input Processing Initialization
        MclInputProc__Initialize(&MclInput_Params);

        //Observer Initialization
        MclObserver__Initialize(&Mcl_Observer_IO, &MclObserver_Params);

        //Speed Controller Initialization
        MclSpeedCtrl__Initialize(&Mcl_Speed_Ctrl_IO,&MclSpeedCtrl_Params);

        //Dq reference generation Initialization
        MclDqRef__Initialize();

        //Dq controllers Initialization
        MclDqCtrl__Initialize(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->DQCtrlPrm);

        //Pwm Initialization
        MclPwm__Initialize(&Mcl_Pwm_IO, &MclPwm_Params);


        // Mean Irms and speed
        N_Samples_inv = 1.0f / (float32) Mcl_Params_Displ1->ResTempCalc.Samples_Number;

        Mcl_Cs.flags.bit.params_loaded = 1;

    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mcl events of 250us.
 *
 *  @param      none
 *  @return     none
 */
void Mcl__250usHandler(void)
{
    if(Mcl_Cs.flags.bit.params_loaded == 1)
    {
        Calc_MeanSpeed_CurrentRMS();
#if (MCL__ELECTRICAL_POWER == ENABLED)
        Calc_ElectricalPower();
#endif
        Calc_TorqueForApplication();
    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mcl events of 1ms.
 *
 *  @param      none
 *  @return     none
 */
void Mcl__1msHandler(void)
{
    if(Mcl_Cs.flags.bit.params_loaded == 1)
    {
        // update elapsed time after start
        TimeHandler();

        // Calculate the absolute value of the speed reference
        Mcl_Quantities.Speed_Rot_Ref_Abs = MATHCALC__ABS(Mcl_IO.Speed_Rot_Ref);
    	// Input Processing Handler
    	MclInputProc__1msHandler(&Mcl_Input_Proc_IO, &MclInput_Params);
    	// Observer Handler
        MclObserver__1msHandler(&Mcl_Observer_IO, &MclObserver_Params);
        MclSpeedCtrl__1msHandler(&Mcl_Speed_Ctrl_IO, &MclSpeedCtrl_Params);
        MclDqRef__1msHandler(&Mcl_DQ_Ref_IO, &MclDqRef_Params);
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mcl events of 5ms.
 *
 *  @param      none
 *  @return     none
 */
void Mcl__5msHandler(void)
{
    static uint16 local_debouncer;
    float32 temp;

    if(Mcl_Cs.flags.bit.params_loaded == 1)
    {
        // If requested to stop and the minimum speed is already reached...
        if(Mcl_Flow_Manager & RUNNING_FINISHED_FLOW_FLAGS)
        {
            if(Mcl_Flow_Manager & ((MCL_FLOW_MANAGER_TYPE)1<<STOP_REQUESTED))
            {
                if(Mcl_Quantities.Speed_Rot_Mech_Abs < Mcl_Params_Displ1->Running.Min_Speed_Before_Stopping)
                {
                    if(local_debouncer)
                    {
                        local_debouncer--;
                    }
                    else
                    {
                        MCL_CLR_FLOW_MANAGER(STOP_LOWERS_ON_REQUESTED);
                        MCL_CLR_FLOW_MANAGER(STOP_COAST_DOWN_REQUESTED);
                        MCL_CLR_FLOW_MANAGER(STOP_REQUESTED);

                        MCL_SET_FLOW_MANAGER(RUNNING_MIN_SPEED_STOP_REACHED);
                    }
                }
                else
                {
                    local_debouncer = Mcl_Params_Displ1->Running.Debounce_Off_Running_Finished;
                }
            }
            else if(Mcl_Flow_Manager & ((MCL_FLOW_MANAGER_TYPE)1<<STOP_COAST_DOWN_REQUESTED))
            {
                if( Mcl_IO.All_Devices_Off == FALSE )
                {
                    if(Mcl_Quantities.Speed_Rot_Mech_Abs < Mcl_Params_Displ1->Running.Min_Speed_Before_All_Devices_Off)
                    {
                        if(local_debouncer)
                        {
                            local_debouncer--;
                        }
                        else
                        {
                            local_debouncer = Mcl_Params_Displ1->Running.Time_All_Dev_Below_Min_Speed;

                            Mcl_IO.All_Devices_Off = TRUE;
                            Mcl_IO.Lowers_On = FALSE;
                        }
                    }
                    else
                    {
                        Mcl_IO.Lowers_On = TRUE;

                        local_debouncer = Mcl_Params_Displ1->Running.Debounce_Off_Running_Finished;
                    }
                }
                else
                {
                    if(local_debouncer)
                    {
                        local_debouncer--;
                    }
                    else
                    {

                        MCL_CLR_FLOW_MANAGER(STOP_LOWERS_ON_REQUESTED);
                        MCL_CLR_FLOW_MANAGER(STOP_COAST_DOWN_REQUESTED);
                        MCL_CLR_FLOW_MANAGER(STOP_REQUESTED);
                        MCL_SET_FLOW_MANAGER(RUNNING_MIN_SPEED_STOP_REACHED);

                    }
                }
            }
            else // default --> if(Mcl_Flow_Manager & ((MCL_FLOW_MANAGER_TYPE)1<<STOP_LOWERS_ON_REQUESTED))
            {
                Mcl_IO.Lowers_On = TRUE;

                if(Mcl_Quantities.Speed_Rot_Mech_Abs < Mcl_Params_Displ1->Running.Min_Speed_Before_Stopping)
                {
                    if(local_debouncer)
                    {
                        local_debouncer--;
                    }
                    else
                    {
                        MCL_CLR_FLOW_MANAGER(STOP_LOWERS_ON_REQUESTED);
                        MCL_CLR_FLOW_MANAGER(STOP_COAST_DOWN_REQUESTED);
                        MCL_CLR_FLOW_MANAGER(STOP_REQUESTED);
                        MCL_SET_FLOW_MANAGER(RUNNING_MIN_SPEED_STOP_REACHED);
                    }
                }
                else
                {
                    local_debouncer = Mcl_Params_Displ1->Running.Debounce_Off_Running_Finished;
                }
            }
        }
        else
        {
            local_debouncer = Mcl_Params_Displ1->Running.Debounce_Off_Running_Finished;
        }



        ////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////
        // Over torque detection

        // If motor is started...
        if((Mcl_Quantities.Elapsed_Time_After_Starting) && (MotorSafetyMgr__DoesClassAHaveAccessToPwm() == TRUE))
        {
            temp  = Mcl_IO.Is_ABC.A*Mcl_IO.Is_ABC.A;
            temp += Mcl_IO.Is_ABC.B*Mcl_IO.Is_ABC.B;
            temp += Mcl_IO.Is_ABC.C*Mcl_IO.Is_ABC.C;

            // If observed torque hit limit...
            if((temp > Mcl_Params_Displ1->Running.Locked_Rotor_Current_Threshold) || (Mcl_Cs.flags.bit.wait_pll_engagement == 1))
            {
                if(Over_Torque_Time_Counter < MAX_U16)
                {
                    Over_Torque_Time_Counter++;         // increment counter
                }
            }
            else
            {
                if(Over_Torque_Time_Counter)
                {
                    Over_Torque_Time_Counter--;         // decrement counter
                }
            }
        }
        else // else motor is stop
        {
            Over_Torque_Time_Counter = 0;               // reset counter
        }
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mcl events of 25ms.
 *
 *  @param      none
 *  @return     none
 */
void Mcl__25msHandler(void)
{
    float32 temp;

#if (THERMAL_MODEL_EXE == ENABLED)
    ThermalModel__Handler25ms();
#endif

    if(Mcl_Cs.flags.bit.params_loaded == 1)
    {
#if (THERMAL_MODEL_EXE == ENABLED)
        Mcl_Quantities.Stator_Temperature = ThermalModel__GetStatorTemperature();
#ifndef DISABLE_RESISTANCE_UPDATE  // Thermal model in open-loop if defined
        Mcl_Quantities.Stator_Resistance  = Calc_Resistance_By_Temperature();
#endif
#endif
        ResistanceEstimation__Handler25ms(&Mcl_Params_Displ1->Res_Est_Param);

        MclSpeedCtrl__25msHandler(&Mcl_Speed_Ctrl_IO, &MclSpeedCtrl_Params);
        MclDqCtrl__25msHandler(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->DQCtrlPrm);
        MclPwm__25msHandler(&Mcl_Pwm_IO);

        temp = Mcl_Params_Displ1->Mcl_SpeedRef_Params.SpeedRef_Min_Speed_Default_Accel * RPM_TO_RADS;
        if( (Mcl_Quantities.Speed_Rot_Ref_Abs >= temp) &&
            (Mcl_Quantities.Speed_Rot_Mech_Abs >= temp))
        {
            SpeedRefGen__ApplyDefaultAccel(FALSE);
        }

    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Starting Reset
 *
 *  @param      none
 *  @return     none
 */
void Mcl__StartingReset(void)
{
    MclPwm__ResetState(&Mcl_Pwm_IO, &MclPwm_Params);

    if(ResistanceEstimation__Requested())
    {
        ResistanceEstimation__Start(&Mcl_Params_Displ1->Res_Est_Param);

        Mcl_Starting_State = MCL_RESISTANCE_ESTIMATION;
    }
    else
    {
        Mcl_Starting_State = MCL_STARTING_FINISHED;
    }


    MCL_CLR_FLOW_MANAGER(RUNNING_MIN_SPEED_STOP_REACHED);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Starting Handler
 *
 *  @param      none
 *  @return     none
 */
void Mcl__StartingHandler(void)
{
    ////////////////////////////////////////////////
    // just two states:
    // - MCL_RESISTANCE_ESTIMATION
    // - MCL_STARTING_FINISHED
    ////////////////////////////////////////////////
    if(Mcl_Starting_State == MCL_RESISTANCE_ESTIMATION)
    {
        if(Estimate_Stator_Resistance())
        {
            Mcl_Starting_State = MCL_STARTING_FINISHED;
        }
    }
}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Reset all Mcl quantities.
 *  @details    This routine reset all Mcl quantities, it has to be called at every time the pwm is switched off (motor stop or free down ramp).
 *
 *
 *  @param[in]
 *  @param[out]
 *  @return
 */
void Mcl__RunningReset(void)
{
    Mcl_Quantities.Elapsed_Time_After_Starting = 0;

    Mcl_IO.All_Devices_Off = FALSE;
    Mcl_IO.Lowers_On = FALSE;

    Mcl_Cs.flags.bit.wrong_speed_direction = 0;
    Mcl_Cs.flags.bit.stopped_ramp = 0;


    if (Mcl_Cs.flags.bit.params_loaded == 1)
    {
        SpeedRefGen__SetSpeedParams(Mcl_Params_Displ1->Mcl_SpeedRef_Params.SpeedRef_Default_Accel);
        SpeedRefGen__ApplyDefaultAccel(TRUE);

        // reset all macro-blocks
        MclInputProc__ResetState();
        MclObserver__ResetState(&Mcl_Observer_IO, &MclObserver_Params);
        MclSpeedCtrl__ResetState(&Mcl_Speed_Ctrl_IO,&MclSpeedCtrl_Params);
        MclDqRef__ResetState(&MclDqRef_Params);
        MclDqCtrl__ResetState(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->DQCtrlPrm);
        MclPwm__ResetState(&Mcl_Pwm_IO, &MclPwm_Params);
    }

    Mcl_DQ_Ctrl_IO.Force_Out_Volt->D = 0.0f;
    Mcl_DQ_Ctrl_IO.Force_Out_Volt->Q = 0.0f;
}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Motor Control Loop for 3-Phase motors (DD,BPM,CIM).
 *  @details   Motor control loop routine for 3-Phase motors :
 *             - d axis: rotor flux position
 *             - q axis: rotor flux quadrature axis
 *
 *             This routine is composed by six parts:
 *             - Input processing: tachometer signal processing
 *                                 motor speed reference absolute value
 *                                 motor phase currents swapping in order to work with a positive speed reference.
 *                                 dc voltage filtering
 *
 *             - Observer:		   rotor flux speed estimation
 *             					   flux position estimation (FOC: rotor flux, DTC: stator flux)
 *
 *             - Speed Controller: speed PI execution
 *								   torque reference generation
 *
 *             - Dq Reference Gen: Mtpa
 *                                 Flux Weaking
 *             					   d-reference generation (FOC: current Id, DTC: flux reference)
 *             					   q reference generation (FOC: current Id, DTC: torque reference)
 *								   torque reference generation
 *
 *             - Dq Controller:   DQ PI execution (FOC: current PI, DTC: flux/torque PI)
 *								  alpha-beta voltage reference generation
 *
 *             - Pwm:   		  DC voltage compensation
 *             					  Deadtime compensation
 *             					  PWM Duties generation
 *  @param[in]     
 *  @param[out]     
 *  @param[in]
 *  @return        
 */
void Mcl__RunningHandler(void)
{

    Flags_Management();

    // Input Processing Handler
	MclInputProc__RunningHandler(&Mcl_Input_Proc_IO,&MclInput_Params);

	// Observer Handler
	MclObserver__RunningHandler(&Mcl_Observer_IO, &MclObserver_Params);

	// Speed Controller Handler
	MclSpeedCtrl__RunningHandler(&Mcl_Speed_Ctrl_IO, &MclSpeedCtrl_Params);

	// Dq reference generation Handler
	MclDqRef__RunningHandler(&Mcl_DQ_Ref_IO, &MclDqRef_Params);

	// Dq controllers Handler
	MclDqCtrl__RunningHandler(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->DQCtrlPrm);

	// Pwm Handler
	MclPwm__RunningHandler(&Mcl_Pwm_IO, &MclPwm_Params);
}
#ifdef PWM_DOUBLE_SLOT
void Mcl__2ndRunningHandler(void)
{
    // Dq controllers Handler
    MclDqCtrl__2ndRunningHandler(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->DQCtrlPrm);

    // Pwm Handler
    MclPwm__RunningHandler(&Mcl_Pwm_IO, &MclPwm_Params);
}
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Stopping Reset
 *
 *  @param      none
 *  @return     none
 */
void Mcl__StoppingReset(void)
{
    if (Mcl_Cs.flags.bit.params_loaded == 1)
    {
        MclSpeedCtrl__ResetState(&Mcl_Speed_Ctrl_IO,&MclSpeedCtrl_Params);
        MclObserver__ResetState(&Mcl_Observer_IO, &MclObserver_Params);
    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Stopping Handler
 *
 *  @param      none
 *  @return     none
 */
void Mcl__StoppingHandler(void)
{

    Mcl_Cs.flags.bit.braking_active = 0;
    Mcl_Quantities.Speed_Rot = 0.0f;
    Mcl_Quantities.Speed_Rot_Mech = 0.0f;
    Mcl_Quantities.Torque = 0.0f;
    Mcl_Quantities.Torque_Ref = 0.0f;
    Mcl_Quantities.Speed_Rot_Ref_Abs = 0.0f;
    Mcl_Quantities.Speed_Rot_Mech_Abs = 0.0f;
    Mcl_IO.Speed_Rot_Ref = 0.0f;

    Mcl_Cs.flags.bit.wait_pll_engagement = 0;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Manual Reset
 *
 *  @param      none
 *  @return     none
 */
void Mcl__ManualReset(void)
{
    if (Mcl_Cs.flags.bit.params_loaded == 1)
    {
        MclDqCtrl__ResetState(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->Manual_Injection);
    }

    Manual_Inject_Level = 0;
    Manual_Inject_Param = 0;
    Manual_Pos_Reference = 0;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Manual Handler
 *
 *  @param      none
 *  @return     none
 */
void Mcl__ManualHandler(void)
{
    float32 temp_level_target;
    float32 temp_level_rate;
    float32 temp_param_target;
    float32 temp_param_rate;
    DQ_COOR_SYST_F_TYPE manual_dq;

    Mcl_Quantities.Elapsed_Time_After_Starting = 0;

    // Input Processing Handler
    MclInputProc__RunningHandler(&Mcl_Input_Proc_IO,&MclInput_Params);

    // Observer Handler
    MclObserver__RunningHandler(&Mcl_Observer_IO, &MclObserver_Params);

    // Check if AC or DC voltage was selected
    if(Mcl_Flow_Manager & MANUAL_VOLTAGE_INJECTION_FLAGS)
    {

        // Variables and its meaning in MANUAL_AC_VOLT_INJECTION
        // Mcl_IO.Manual_Injection_Level            - define the voltage
        // Mcl_IO.Manual_Injection_Level_Rate       - define the voltage rate in volts/s
        // Mcl_IO.Manual_Injection_Param            - define the frequency
        // Mcl_IO.Manual_Injection_Param_Rate       - define the frequency rate in Hz/s

        // Variables and its meaning in MANUAL_DC_VOLT_INJECTION
        // Mcl_IO.Manual_Injection_Level            - define the voltage
        // Mcl_IO.Manual_Injection_Level_Rate       - define the voltage rate in volts/s
        // Mcl_IO.Manual_Injection_Param            - define position of the voltage vector in degrees
        // Mcl_IO.Manual_Injection_Param_Rate       - not used

        temp_level_target = Mcl_IO.Manual_Injection_Level/(32.0f);
        temp_level_rate  = Mcl_IO.Manual_Injection_Level_Rate/(32.0f);


        // Handle the voltage target increase
        if(Manual_Inject_Level < temp_level_target)
        {
            Manual_Inject_Level += (TSW*temp_level_rate);
        }
        else
        {
            Manual_Inject_Level = temp_level_target;
        }

        temp_level_target = Manual_Inject_Level;

        if(Mcl_Flow_Manager & ((MCL_FLOW_MANAGER_TYPE)1<<MANUAL_AC_VOLT_INJECTION))
        {
            // Handle the frequency
            temp_param_target = Mcl_IO.Manual_Injection_Param/32.0f;      // temp_param_target = (Mcl_IO.Manual_Injection_Param << 15)/(sint32)(32.0);
            temp_param_rate  = Mcl_IO.Manual_Injection_Param_Rate/32.0f;      // temp_param_rate  = (Mcl_IO.Manual_Injection_Param_Rate)/(sint32)(32.0);

            if(MATHCALC__ABS(Manual_Inject_Param) < MATHCALC__ABS(temp_param_target))
            {
                if(temp_param_target > 0)
                {
                    Manual_Inject_Param += TSW*temp_param_rate;
                }
                else
                {
                    Manual_Inject_Param -= TSW*temp_param_rate;
                }
            }
            else
            {
                Manual_Inject_Param = temp_param_target;
            }

            Manual_Pos_Reference += TSW*TWO_PI * Manual_Inject_Param;
            if(Manual_Pos_Reference > TWO_PI)
            {
                Manual_Pos_Reference = Manual_Pos_Reference - TWO_PI;
            }
        }
        else // MANUAL_DC_VOLT_INJECTION
        {
            Manual_Pos_Reference = (float32)(Mcl_IO.Manual_Injection_Param ) * (TWO_PI / (32.0f *360.0f));
        }


        // Set proper voltage
        if(!temp_level_target)
        {
            temp_level_target = 1;
        }

        MathCalc__SinCosF(Manual_Pos_Reference, Mcl_DQ_Ctrl_IO.Sin_Cos);

        Mcl_DQ_Ctrl_IO.Force_Out_Volt->D = temp_level_target;
        Mcl_DQ_Ctrl_IO.Force_Out_Volt->Q = 0.0f;

    }
    else if(Mcl_Flow_Manager & MANUAL_CURRENT_INJECTION_FLAGS)
    {

        // Variables and its meaning in MANUAL_AC_CURRENT_INJECTION
        // Mcl_IO.Manual_Injection_Level            - define the current level
        // Mcl_IO.Manual_Injection_Level_Rate       - define the current rate in volts/s
        // Mcl_IO.Manual_Injection_Param            - define the frequency
        // Mcl_IO.Manual_Injection_Param_Rate       - define the frequency rate in Hz/s

        // Variables and its meaning in MANUAL_DC_CURRENT_INJECTION
        // Mcl_IO.Manual_Injection_Level            - define the current level
        // Mcl_IO.Manual_Injection_Level_Rate       - define the current rate in volts/s
        // Mcl_IO.Manual_Injection_Param            - define position of the voltage vector in degrees
        // Mcl_IO.Manual_Injection_Param_Rate       - not used


        // Zero the forced voltage, this allows the controller to proper
        // generate the voltage references
        Mcl_DQ_Ctrl_IO.Force_Out_Volt->D = 0.0f;
        Mcl_DQ_Ctrl_IO.Force_Out_Volt->Q = 0.0f;


        temp_level_target = Mcl_IO.Manual_Injection_Level/(32.0f);
        temp_level_rate  = Mcl_IO.Manual_Injection_Level_Rate/(32.0f);


        // Handle the voltage target increase
        if(Manual_Inject_Level < temp_level_target)
        {
            Manual_Inject_Level += TSW*temp_level_rate;
        }
        else
        {
            Manual_Inject_Level = temp_level_target;
        }


        temp_level_target = Manual_Inject_Level;

        if(Mcl_Flow_Manager & ((MCL_FLOW_MANAGER_TYPE)1<<MANUAL_AC_CURRENT_INJECTION))
        {

            // Handle the frequency
            temp_param_target = Mcl_IO.Manual_Injection_Param/32.0f;      // temp_param_target = (Mcl_IO.Manual_Injection_Param << 15)/(sint32)(32.0);
            temp_param_rate  = Mcl_IO.Manual_Injection_Param_Rate/32.0f;      // temp_param_rate  = (Mcl_IO.Manual_Injection_Param_Rate)/(sint32)(32.0);

            if(MATHCALC__ABS(Manual_Inject_Param) < MATHCALC__ABS(temp_param_target))
            {
                if(temp_param_target > 0)
                {
                    Manual_Inject_Param += TSW*temp_param_rate;
                }
                else
                {
                    Manual_Inject_Param -= TSW*temp_param_rate;
                }
            }
            else
            {
                Manual_Inject_Param = temp_param_target;
            }

            Manual_Pos_Reference += TSW*TWO_PI*Manual_Inject_Param;
            if(Manual_Pos_Reference > TWO_PI)
            {
                Manual_Pos_Reference = Manual_Pos_Reference -TWO_PI;
            }

        }
        else // MANUAL_DC_CURRENT_INJECTION
        {
            Manual_Pos_Reference = (float32)(Mcl_IO.Manual_Injection_Param ) * (TWO_PI / (32.0f *360.0f));
        }

        MathCalc__SinCosF(Manual_Pos_Reference, Mcl_DQ_Ctrl_IO.Sin_Cos);

        manual_dq.ZeroSequence = 0;
        ClrkPark__DirectParkF(Mcl_Input_Proc_IO.Is_Alpha_Beta, &manual_dq, Mcl_DQ_Ctrl_IO.Sin_Cos);

        *Mcl_DQ_Ctrl_IO.D_Ref = temp_level_target;
        *Mcl_DQ_Ctrl_IO.D_Est = manual_dq.D;

        *Mcl_DQ_Ctrl_IO.Q_Ref = 0;
        *Mcl_DQ_Ctrl_IO.Q_Est = manual_dq.Q;

    }

    // Dq controllers Handler
    MclDqCtrl__OverrideRegGains(&Mcl_Params_Displ1->Manual_Injection);
    MclDqCtrl__RunningHandler(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->Manual_Injection);

    // Pwm Handler
    MclPwm__RunningHandler(&Mcl_Pwm_IO, &MclPwm_Params);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Mcl__ResetAll.
 *
 *  @param      none
 *  @return     none
 */
void Mcl__ResetAll(void)
{
    Mcl__RunningReset();
    Mcl__ManualReset();

    Mcl_Quantities.Speed_Rot = 0.0f;
    Mcl_Quantities.Speed_Rot_Mech = 0.0f;
    Mcl_Quantities.Torque = 0.0f;
    Mcl_Quantities.Torque_Ref = 0.0f;
    Mcl_Quantities.Speed_Rot_Ref_Abs = 0.0f;
    Mcl_Quantities.Speed_Rot_Mech_Abs = 0.0f;
    Mcl_IO.Speed_Rot_Ref = 0.0f;

    Mcl_Cs.flags.bit.wait_pll_engagement = 0;

    Mcl_Quantities.Electrical_Power = 0.0f;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Read any digital data from the MCL
 * @param input is index of the parameter. See the Mcl.h file
 * @return TRUE/FALSE according to the type of the passed parameter. The default return is FALSE.
 */
BOOL_TYPE Mcl__GetDigital(MCL_READ_DIGITAL_DATA_ENUM read_bit)
{
    BOOL_TYPE bit_value;

    bit_value = FALSE;


    switch(read_bit)
    {
        case MCL_READ_STARTING_FINISHED:
            if(Mcl_Starting_State == MCL_STARTING_FINISHED)
            {
                bit_value = TRUE;
            }
            break;

        case MCL_READ_RUNNING_FINISHED:
            if(Mcl_Flow_Manager & (1<<RUNNING_MIN_SPEED_STOP_REACHED))
            {
                bit_value = TRUE;
            }
            break;

        case MCL_READ_STOPPING_FINISHED:
            bit_value = TRUE;
            break;

        case MCL_READ_PARAMETERS_LOADED:
            if (Mcl_Cs.flags.bit.params_loaded == 1)
            {
                bit_value = TRUE;
            }
            else
            {
                bit_value = FALSE;
            }
            break;

        case MCL_READ_LOCKED_ROTOR_STARTING:
            if (Mcl_Cs.flags.bit.params_loaded == 1)
            {
                if(Mcl_Quantities.Elapsed_Time_After_Starting < Mcl_Params_Displ1->Running.Locked_Rotor_Starting_Or_Running)
                {
                    if(Over_Torque_Time_Counter > Mcl_Params_Displ1->Running.Locked_Rotor_Starting_Threshold)
                    {
                        Over_Torque_Time_Counter = 0;
                        bit_value = TRUE;
                    }
                }
            }
            break;

        case MCL_READ_LOCKED_ROTOR_RUNNING:
            if (Mcl_Cs.flags.bit.params_loaded == 1)
            {
                if(Mcl_Quantities.Elapsed_Time_After_Starting > Mcl_Params_Displ1->Running.Locked_Rotor_Starting_Or_Running)
                {
                    if(Over_Torque_Time_Counter > Mcl_Params_Displ1->Running.Locked_Rotor_Running_Threshold)
                    {
                        Over_Torque_Time_Counter = 0;
                        bit_value = TRUE;
                    }
                }
            }
            break;


        case MCL_READ_MOTOR_NOT_CONNECTED_ERROR:
            if(Mcl_Cs.flags.bit.resistance_est_error == 1)
            {
            	Mcl_Cs.flags.bit.resistance_est_error = 0;
                bit_value = TRUE;
            }
            else
            {
                bit_value = FALSE;
            }
            break;

        default:
            // Left blank intentionally
            break;
    }

    return bit_value;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Write a digital data to the MCL.
 * @param write_data is the index to the data. See the Mcl.h file.
 * @param value TRUE/FALSE according to the selected data
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
BOOL_TYPE Mcl__SetDigital(MCL_SET_DIGITAL_DATA_ENUM set_bit)
{
    BOOL_TYPE accepted;

    accepted = FALSE;

    switch(set_bit)
    {
        case MCL_WRITE_STOP_COAST_DOWN_REQ:
            MCL_SET_FLOW_MANAGER(STOP_COAST_DOWN_REQUESTED);

            accepted = TRUE;
            break;


        case MCL_WRITE_STOP_LOWERS_ON_REQ:
            MCL_SET_FLOW_MANAGER(STOP_LOWERS_ON_REQUESTED);

            accepted = TRUE;
            break;


        case MCL_WRITE_STOP_REQ:
            MCL_SET_FLOW_MANAGER(STOP_REQUESTED);

            accepted = TRUE;
            break;


        case MCL_WRITE_INJECTION_DC_VOLTAGE:
            MCL_SET_FLOW_MANAGER(MANUAL_DC_VOLT_INJECTION);

            accepted = TRUE;
            break;


        case MCL_WRITE_INJECTION_AC_VOLTAGE:
            MCL_SET_FLOW_MANAGER(MANUAL_AC_VOLT_INJECTION);

            accepted = TRUE;
            break;


        case MCL_WRITE_INJECTION_DC_CURRENT:
            MCL_SET_FLOW_MANAGER(MANUAL_DC_CURRENT_INJECTION);

            accepted = TRUE;
            break;


        case MCL_WRITE_INJECTION_AC_CURRENT:
            MCL_SET_FLOW_MANAGER(MANUAL_AC_CURRENT_INJECTION);

            accepted = TRUE;
            break;

        default:
            // Left blank intentionally
            break;
    }

    return accepted;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Reset a digital data to the MCL.
 * @param write_data is the index to the data. See the Mcl.h file.
 * @param value TRUE/FALSE according to the selected data
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
BOOL_TYPE Mcl__ResetDigital(MCL_SET_DIGITAL_DATA_ENUM reset_bit)
{
    BOOL_TYPE accepted;

    accepted = FALSE;

    switch(reset_bit)
    {
        case MCL_WRITE_ALL_INJECTION_METHOD:
            MCL_CLR_FLOW_MANAGER(MANUAL_DC_VOLT_INJECTION);
            MCL_CLR_FLOW_MANAGER(MANUAL_AC_VOLT_INJECTION);
            MCL_CLR_FLOW_MANAGER(MANUAL_DC_CURRENT_INJECTION);
            MCL_CLR_FLOW_MANAGER(MANUAL_AC_CURRENT_INJECTION);

            accepted = TRUE;
            break;


//        case MCL_WRITE_INJECTION_DC_CURRENT:
//            MCL_SET_FLOW_MANAGER(MANUAL_DC_CURRENT_INJECTION);
//            break;
//
//        case MCL_WRITE_INJECTION_AC_CURRENT:
//            MCL_SET_FLOW_MANAGER(MANUAL_AC_CURRENT_INJECTION);
//            break;

        default:
            // Left blank intentionally
            break;
    }

    return accepted;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Write a analog data to the MCL.
 * @param write_data is the index to the data. See the Mcl.h file.
 * @param value TRUE/FALSE according to the selected data
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
BOOL_TYPE Mcl__SetAnalog(MCL_AO_TYPE write_data, sint32 value)
{
    BOOL_TYPE accepted;

    accepted = FALSE;

    switch(write_data)
    {

        case MCL_AO_SPEED_LOOP_PROP_GAIN_S16:
            break;

        case MCL_AO_SPEED_LOOP_INT_GAIN_S16:
            break;

        case MCL_AO_SPEED_LOOP_GAIN_TABLE_INDEX:
            accepted = MclSpeedCtrl__SetSpeedGainIndex(value, &MclSpeedCtrl_Params);
            break;

        case MCL_AO_SPEED_FIR_TAPS_TABLE_INDEX:
            break;

        case MCL_AO_SOFTSTART_COUNTER_INDEX:
            break;

        case MCL_AO_SPEED_LIMIT_INDEX:
        	accepted = MclSpeedCtrl__SetSpeedRegLimit(value);
            break;

        case MCL_AO_ADD_DELTA_SPEED_REF_S16:
            accepted = MclSpeedCtrl__SetDeltaSpeedRef(value, &Mcl_Speed_Ctrl_IO);
            break;


        default:
            // Left blank intentionally
            break;
    }


    return accepted;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Write the address of mcl parameters.
 *  @details
 *
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */

BOOL_TYPE Mcl__SetParams(void*  mcl_params_address)
{
    BOOL_TYPE ret_val = FALSE;

    if (mcl_params_address != NULL)
    {
        // TODO: to be consistent with the MciParametersLoader
        Mcl_Params_Displ1 = (MCL_PARAMS_DISPL1_TYPE*) (*(addr_t*)mcl_params_address);
        Mcl_Params_Displ3 = (MCL_PARAMS_DISPL3_TYPE*) (*((addr_t*)mcl_params_address + 1));
        Mcl_Params_Displ4 = (MCL_PARAMS_DISPL4_TYPE*) (*((addr_t*)mcl_params_address + 2));
        Mcl_Params_Displ9 = (MCL_PARAMS_DISPL9_TYPE*) (*((addr_t*)mcl_params_address + 3));
        Mcl_Params_Displ10= (MCL_PARAMS_DISPL10_TYPE*)(*((addr_t*)mcl_params_address + 4));
        Mcl_Params_Displ11= (MCL_PARAMS_DISPL11_TYPE*)(*((addr_t*)mcl_params_address + 5));
        Mcl_Params_Displ13= (MCL_PARAMS_DISPL13_TYPE*)(*((addr_t*)mcl_params_address + 6));

        Mcl_Cs.flags.bit.params_ready = 1;

        Mcl__ParamsInitialize();

        ret_val = TRUE;
    }

    return ret_val;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
static BOOL_TYPE Estimate_Stator_Resistance(void)
{
    static uint8 switching_angle = 0;
    BOOL_TYPE estimation_finished;
    float32 temp_f32;
    float32 initial_position;

    estimation_finished = FALSE;

    Mcl_Quantities.Elapsed_Time_After_Starting = 0;

    // Input Processing Handler
    MclInputProc__RunningHandler(&Mcl_Input_Proc_IO,&MclInput_Params);

    // Observer Handler -- to check speed oscillation
    MclObserver__RunningHandler(&Mcl_Observer_IO, &MclObserver_Params);

    // Calculate the voltage magnitude applied to the motor
    // The voltage magnitude is calculated here and not in the MclInputProc because the
    // output voltage magnitude is only used here until now (it saves a sqrt call when controlling the motor)
    temp_f32  = (Mcl_Input_Proc_IO.Vs_Alpha_Beta_Rec->Alpha) * (Mcl_Input_Proc_IO.Vs_Alpha_Beta_Rec->Alpha);
    temp_f32 += (Mcl_Input_Proc_IO.Vs_Alpha_Beta_Rec->Beta)  * (Mcl_Input_Proc_IO.Vs_Alpha_Beta_Rec->Beta);
    Mcl_Quantities.Vs_Voltage = MC_SQRT_F(temp_f32);

    if(ResistanceEstimation__Calculate(&Res_Est_IO, &Mcl_Params_Displ1->Res_Est_Param))
    {
        Mcl_Quantities.Stator_Resistance  = ResistanceEstimation__GetResistance();
        Mcl_Quantities.Stator_Temperature = Calc_Temperature_By_Resistance();
#if (THERMAL_MODEL_EXE == ENABLED)
        ThermalModel__SetStatorTemperature(Mcl_Quantities.Stator_Temperature);
#endif
        initial_position = -(float32) switching_angle * RESISTANCE_EST_ALIGNMENT_STEP_VALUE;

        // initialize the initial angle of the observer
        Mcl_Quantities.DcInjection_Angle = initial_position;
        estimation_finished = TRUE;
        switching_angle++;
        if(switching_angle > (RESISTANCE_EST_ALIGNMENT_STEPS-1))
        {
            switching_angle = 0;
        }
    }
    else
    {
        // todo silvamc - Set initial angle from another module? e.g. angle tracking
        initial_position = -(float32) switching_angle * RESISTANCE_EST_ALIGNMENT_STEP_VALUE;
        // The voltage reference has to be different of zero, since the forced voltage has to be different of zero
        // in order to overwrite the DQ references.
        if(Mcl_Quantities.Level_Injection == 0.0f)
        {
            Mcl_Quantities.Level_Injection = 0.01f;
        }

        MathCalc__SinCosF(initial_position, Mcl_DQ_Ctrl_IO.Sin_Cos);

        Mcl_DQ_Ctrl_IO.Force_Out_Volt->D = Mcl_Quantities.Level_Injection;
        Mcl_DQ_Ctrl_IO.Force_Out_Volt->Q = 0.0f;
        Mcl_DQ_Ctrl_IO.Force_Out_Volt->ZeroSequence = 0.0f;

    }


    // Dq controllers Handler
    MclDqCtrl__RunningHandler(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->DQCtrlPrm);

    // Pwm Handler
    MclPwm__RunningHandler(&Mcl_Pwm_IO, &MclPwm_Params);


    return estimation_finished;
}


/**
 *  @brief      Calculate RMS values and mean speed value in 4khz handler (256ms time windows)
 *  @param[in]  Motor currents and rotor speed.
 *  @param[out] Mean RMS current of the three motor phasesa and the mean speed.
 *  @param[in]  SAMPLES_NUMBER: 10 suitable choice to have ~250ms of time window at 4Khz
 *  @return none
 */
static void Calc_MeanSpeed_CurrentRMS(void)
{

    I1_Rms_Sum += (Mcl_IO.Is_ABC.A * Mcl_IO.Is_ABC.A);
    I2_Rms_Sum += (Mcl_IO.Is_ABC.B * Mcl_IO.Is_ABC.B);
    I3_Rms_Sum += (Mcl_IO.Is_ABC.C * Mcl_IO.Is_ABC.C);
    Mean_Speed_Sum += Mcl_Quantities.Speed_Rot_Mech_Abs;

    N_Samples++;

    if(N_Samples >= Mcl_Params_Displ1->ResTempCalc.Samples_Number)
    {

        Mcl_Quantities.IsRMS = MC_SQRT_F((I1_Rms_Sum+I2_Rms_Sum+I3_Rms_Sum)*INV_3* N_Samples_inv);
        Mcl_Quantities.Mean_Speed = Mean_Speed_Sum * N_Samples_inv;

        N_Samples  = 0;
        I1_Rms_Sum = 0.0f;
        I2_Rms_Sum = 0.0f;
        I3_Rms_Sum = 0.0f;
        Mean_Speed_Sum = 0.0f;
    }
}




/**
 *  @brief      Calculating temperature from resistance
 *  @param[in]
 *  @param[out]
 *  @param[in]  Temperature coefficient and Resistance base temperature
 *  @return     Stator temperature in [C]
 */
static float32 Calc_Temperature_By_Resistance(void)
{
    float32 temp_1;
    float32 temp_2;

    // T = (R-Rs0)/(Rs0*alpha)  + T0

    temp_1 = Mcl_Quantities.Stator_Resistance - Mcl_Params_Displ1->ResTempCalc.Rs0;

    temp_2 = Mcl_Params_Displ1->ResTempCalc.Temp_Coeff * temp_1 * Mcl_Params_Displ1->ResTempCalc.Rs0_inv;
    temp_2 += Mcl_Params_Displ1->ResTempCalc.T0;

    return(temp_2);
}



/**
 *  @brief      Calculating resistance from temperature
 *  @param[in]  Temperature coefficient and Resistance base temperature
 *  @param[out]
 *  @param[in]
 *  @return Updated stator resistance in [Ohm]
 */
static float32 Calc_Resistance_By_Temperature(void)
{
    float32 temp_1;
    float32 temp_2;
    // Rs = Rs0*(1+1/alpha (T - T0))
    temp_1 = 1 + Mcl_Params_Displ1->ResTempCalc.Temp_Coeff_inv * (Mcl_Quantities.Stator_Temperature - Mcl_Params_Displ1->ResTempCalc.T0);
    temp_2 = Mcl_Params_Displ1->ResTempCalc.Rs0 * temp_1;

    return(temp_2);
}

#if (MCL__ELECTRICAL_POWER == ENABLED)
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
/**
 *  @brief      Calculate RMS values and mean speed value in 4khz handler (256ms time windows)
 *  @param[in]  Motor currents and rotor speed.
 *  @param[out] Mean RMS current of the three motor phasesa and the mean speed.
 *  @param[in]  SAMPLES_NUMBER: 10 suitable choice to have ~250ms of time window at 4Khz
 *  @return none
 */
static void Calc_ElectricalPower(void)
{
    float32 electrical_power;
    // instantaneous power
    electrical_power = 1.5f * (Mcl_Quantities.Vs_Alpha_Beta_Rec.Alpha * Mcl_Quantities.Is_Alpha_Beta.Alpha + Mcl_Quantities.Vs_Alpha_Beta_Rec.Beta * Mcl_Quantities.Is_Alpha_Beta.Beta);

    if ((Mcl_IO.All_Devices_Off == FALSE) && (MotorSafetyMgr__DoesClassAHaveAccessToPwm() == TRUE))
    {
        Mcl_Quantities.Electrical_Power = FILTERS__LOWPASSFILTER_F(Mcl_Quantities.Electrical_Power, MCL__EL_POWER_LPF, electrical_power);
    }
    else
    {
        Mcl_Quantities.Electrical_Power = 0.0f;
    }
}
#endif

#if (MCL__SHAFT_POWER == ENABLED)
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif

/**
 *  @brief      Calculate values and mean speed value in 4khz handler (256ms time windows)
 *  @param[in]  Eletromagnetic Torque and rotor speed.
 *  @param[out] Mechanical motor power without considering the rotational losses.
 *  @param[in]  SAMPLES_NUMBER: 10 suitable choice to have ~250ms of time window at 4Khz
 *  @return none
 */
void Calc_ShaftPower(void)
{
    float32 shaft_power;
    // Motor mechanical power output/Shaft Power =T*w
    shaft_power = (Mcl_Quantities.Torque * Mcl_Quantities.Speed_Rot_Mech_Abs);

    if ((Mcl_IO.All_Devices_Off == FALSE) && (MotorSafetyMgr__DoesClassAHaveAccessToPwm() == TRUE))
    {
       Mcl_Quantities.Shaft_Power = FILTERS__LOWPASSFILTER_F(Mcl_Quantities.Shaft_Power, MCL__ML_POWER_LPF, shaft_power);
    }
    else
    {
       Mcl_Quantities.Shaft_Power = 0.0f;
    }
}


#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif



static void TimeHandler(void)
{
    // Count time after every starting
    if( Mcl_Quantities.Elapsed_Time_After_Starting < MAX_UINT32)
    {
    	Mcl_Quantities.Elapsed_Time_After_Starting = Mcl_Quantities.Elapsed_Time_After_Starting + 1;
    }
}


#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
/**
 *  @brief      Calculate the torque value for the application layer according to the following convention:
 *              torque is positive when motoring and negative when braking whatever is the motor speed direction.
 *  @param[in]  Motor Torque (from Observer) and motor speed reference.
 *  @param[out] Torque for the application layer.
 *  @return none
 */
static void Calc_TorqueForApplication(void)
{
    // check the sign of the speed reference
    if (Mcl_IO.Speed_Rot_Ref<0)
    {   //adjust the sign of the torque if needed
        Mcl_Quantities.Torque_Application = -Mcl_Quantities.Torque;
    }
    else
    {// sign is correct
        Mcl_Quantities.Torque_Application = Mcl_Quantities.Torque;
    }

}

/**
 *  @brief      Management of some MCL flags during the running handler
 *  @return none
 */
static void Flags_Management(void)
{
    float32 target_speed;
    // logic to detect wrong speed direction
    target_speed   = SpeedRefGen__GetSpeedTarget();
    if ((target_speed * Mcl_Quantities.Speed_Rot) < 0)
    {
        Mcl_Cs.flags.bit.wrong_speed_direction = 1;
    }
    else
    {
        Mcl_Cs.flags.bit.wrong_speed_direction = 0;
    }


    if (Mcl_Cs.flags.bit.braking_active == 1)
    {
#if (STOP_RAMP_DURING_INVERSION == ENABLED)
        // stop the ramp if the reference is accelerating while in braking mode
        if (SpeedRefGen__GetStatus() == SPEED_REF_ACCELERATING)
        {
            SpeedRefGen__SetSpeedParams(0.0f);
            SpeedRefGen__ApplyDefaultAccel(TRUE);
            Mcl_Cs.flags.bit.stopped_ramp = 1;
        }
#endif
    }
    else
    {
#if (STOP_RAMP_DURING_INVERSION == ENABLED)
        // braking is ended: restart the ramp if it was stopped (default deceleration is 0)
        if (Mcl_Cs.flags.bit.stopped_ramp == 1)
        {
            SpeedRefGen__ApplyDefaultAccel(FALSE);
            Mcl_Cs.flags.bit.stopped_ramp = 0;
        }
#endif
        // logic to detect motor direction
        // update motor direction (if not braking)
        if (Mcl_IO.Speed_Rot_Ref>0)
        {
            Mcl_Cs.flags.bit.motor_direction = 1;
        }
        else if (Mcl_IO.Speed_Rot_Ref<0)
        {
            Mcl_Cs.flags.bit.motor_direction = 0;
        }
    }
}

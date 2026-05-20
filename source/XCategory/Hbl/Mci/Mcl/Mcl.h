/**
 *  @file       MclInputProc.h
 *
 *  @brief      Motor Control Loop 3-phase motors: Input Processing Macro Block.
 *
 *  @section
 *
 *  $Header:    MclInputProc.h
 *
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCL_H_
#define MCL_H_


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MclConfig.h"
#include "MathCalc.h"
#include "MclInputProc.h"
#include "MclObserver.h"
#include "MclSpeedCtrl.h"
#include "MclDqRef.h"
#include "MclDqCtrl.h"
#include "MclPwm.h"
#include "ResistanceEstimation.h"
#include "Braking.h"
#include "SpeedRefGen.h"
#include "MclCrossTypes.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef struct
{
    uint32 Forced_Flux_Plateau_After_Ramp;
    float32 Min_Current_Build_Flux;
    float32 Forced_Flux_Ref_After_Build_Flux;
    float32 Build_Flux_Increment;
} MCL_BUILD_PARAMS_TYPE;

typedef struct
{
    float32 SpeedRef_Default_Accel;                  //!< Default acceleration, used upon request, e.g. starting
    float32 SpeedRef_Min_Speed_Default_Accel;        //!< Minimum speed to request minimum acceleration
} MCL_SPEED_REF_PARAMS_TYPEDEF;

typedef struct
{
    float32 Rs0;           //!< [Ohm] Rs at T0.
    float32 Temp_Coeff;    //!<  \frac{1}{\alpha}
    float32 Rs0_inv;       //!< [Deg] - Base temperature coefficient
    float32 Temp_Coeff_inv;//!< \alpha
    float32 T0;            //!< [Deg] - Temperature T0


    sint32 Samples_Number;

} MCL_RESTEMP_PARAMS_TYPE;

typedef struct
{
    float32 Min_Speed_Before_All_Devices_Off;        //!< [rpm]    define the minimum speed to switch off all devices.
    float32 Min_Speed_Before_Stopping;               //!< [rpm]    define the minimum speed to declare motor stopped
    uint32 Debounce_Off_Running_Finished;            //!< [n/a]    debouncer to the minimum speed
    uint32 Time_All_Dev_Below_Min_Speed;             //!< [x 25ms] time to wait before declaring stop and below Min_Speed_Before_Stopping using all device off
    uint32 Locked_Rotor_Starting_Or_Running;         //!< [x Ts]   Time measured from starting to decide whether detected locked rotor at starting or running
    uint32 Locked_Rotor_Starting_Threshold;          //!< [x 5 ms] Starting debounce time to run with torque at its maximum
    uint32 Locked_Rotor_Running_Threshold;           //!< [x 5 ms] Running debounce time to run with torque at its maximum
    float32 Locked_Rotor_Current_Threshold;          //!< [A^2]    Squared current threhold to increment/decrement timer
} MCL_RUNNING_PARAMS_TYPE;

// Mcl parameters definition
typedef struct
{
    // Displacement 1 - Motor Params (DTC Variant)

    //! Input Processing Parameters
    MCL_INPUT_PROC_PARAMS_TYPE InputProcPrm;

    //! Observer Parameters
    MCL_OBSERVER_PARAMS_TYPE ObserverPrm;

    //! DQ Controllers Parameters
    MCL_DQ_CTRL_PARAMS_TYPE DQCtrlPrm;

    //! DQ Reference Generator Controller Parameters
    MCL_DQ_REF_PARAMS_TYPE DQRefPrm;

    //! Starting parameters
    MCL_RUNNING_PARAMS_TYPE Running;                //!< MCL parameters used while running motor

    //! Resistance estimation parameters
    RES_EST_PARAM_TYPE Res_Est_Param;               //!< MCL parameters used while estimating resistance of the motor

    //! Paramaters for temperature/resistance calculation
    MCL_RESTEMP_PARAMS_TYPE ResTempCalc;       //!< MCL parameters used for resistance/temperature calculation

    //! Speed Reference generator parameters
    MCL_SPEED_REF_PARAMS_TYPEDEF Mcl_SpeedRef_Params;

    //! Manual injection parameters
    MCL_DQ_CTRL_PARAMS_TYPE Manual_Injection;       //!< MCL parameters used while manually controlling voltage or current

    //! Pwm Parameters
    MCL_PWM_PARAMS_TYPE PwmPrm;
} MCL_PARAMS_DISPL1_TYPE;

typedef struct
{
    // Displacement 3 - Motor LUTs
    sint32  sizeof_Ld_Lq_Lut;
    float32 step_inv_L_Lut;
    //! Ld look-up table
    MATHCALC_LUT_F_TYPE Ld_Lut[8];
    //! Lq look-up table
    MATHCALC_LUT_F_TYPE Lq_Lut[8];
    //! Ldq look-up table
    MATHCALC_LUT_F_TYPE Ldq_Lut[8];
} MCL_PARAMS_DISPL3_TYPE;

typedef struct
{
    // Displacement 4 - Speed Controller

    //! Speed Controller Parameters
    MCL_SPEED_CTRL_PARAMS_TYPE SpeedCtrlPrm;
} MCL_PARAMS_DISPL4_TYPE;


typedef struct
{
    // Displacement 9 - MTPA LUT
    sint32  sizeof_mtpa_lut;
    float32 step_inv_mtpa;
    MATHCALC_LUT_F_TYPE Mtpa_Lut[16];
} MCL_PARAMS_DISPL9_TYPE;

typedef struct
{
    // Displacement 10 - MTPV LUT
    sint32 sizeof_mtpv;
    float32 step_inv_mtpv;
    MATHCALC_LUT_F_TYPE       Tq_Limit_Lut[16];
} MCL_PARAMS_DISPL10_TYPE;

typedef struct
{
    // Displacement 11 - Rotor Flux LUT
    sint32  sizeof_Flux_Lut;
    float32 step_inv_Flux_Lut;
    //! Rotor Flux look-up table
    MATHCALC_LUT_F_TYPE Flux_Lut[8];
} MCL_PARAMS_DISPL11_TYPE;


typedef struct
{
    // Displacement 13 - Motor Params (DTC Variant)

    //! Input Processing Parameters
    MCL_INPUT_PROC_ADDITIONAL_PARAMS_TYPE InputProcAddPrm;

    //! Observer Parameters
    MCL_OBSERVER_ADDITIONAL_PARAMS_TYPE ObserverAddPrm;
    
    
    //! Speed Controller Parameters
    MCL_SPEED_CTRL_ADDITIONAL_PARAMS_TYPE SpeedCtrlAddPrm;

    //! DQ Controllers Parameters
    //MCL_DQ_CTRL_PARAMS_TYPE DQCtrlAddPrm;

    //! DQ Reference Generator Controller Parameters
    MCL_DQ_REF_ADDITIONAL_PARAMS_TYPE DQRefAddPrm;
} MCL_PARAMS_DISPL13_TYPE;


typedef enum
{
    MCL_READ_STARTING_FINISHED,
    MCL_READ_RUNNING_FINISHED,
    MCL_READ_STOPPING_FINISHED,
    MCL_READ_PARAMETERS_LOADED,
	MCL_READ_LOCKED_ROTOR_STARTING,
	MCL_READ_LOCKED_ROTOR_RUNNING,
    MCL_READ_MOTOR_NOT_CONNECTED_ERROR,
} MCL_READ_DIGITAL_DATA_ENUM;


typedef enum
{
    MCL_WRITE_STOP_COAST_DOWN_REQ,                  //!< Request to stop motor using coast down (turn off all IGBTs)
    MCL_WRITE_STOP_LOWERS_ON_REQ,                   //!< Request to stop motor using lowers on (turn on only lower IGBTs)
    MCL_WRITE_STOP_REQ,                             //!< Request to stop motor using using controllers
    MCL_WRITE_INJECTION_DC_VOLTAGE,                 //!< Request to inject DC voltage
    MCL_WRITE_INJECTION_AC_VOLTAGE,                 //!< Request to inject AC voltage
    MCL_WRITE_INJECTION_DC_CURRENT,                 //!< Request to inject DC current
    MCL_WRITE_INJECTION_AC_CURRENT,                 //!< Request to inject AC current
    MCL_WRITE_ALL_INJECTION_METHOD,                 //!< Request to all methods of injection.
} MCL_SET_DIGITAL_DATA_ENUM;



typedef enum
{
    MCL_AO_SPEED_LOOP_PROP_GAIN_S16 = 0,            //!<  0 - Proportional speed loop gain - x65536 - (1<<16)
    MCL_AO_SPEED_LOOP_INT_GAIN_S16,                 //!<  1 - Integral speed loop gain - x65536 - (1<<16)
    MCL_AO_SPEED_LOOP_GAIN_TABLE_INDEX,             //!<  2 - Select index of speed loop gain table
    MCL_AO_SPEED_FIR_TAPS_TABLE_INDEX,              //!<  3 - Select index of speed fir taps

    MCL_AO_SOFTSTART_COUNTER_INDEX,                 //!<  4 - Select index of soft start counter
    MCL_AO_SPEED_LIMIT_INDEX,                       //!<  5 - Select index of speed controller limit
    MCL_AO_MCL_PARAMETERS_ADDRESS,                  //!<  6 - Set the address of Mcl parameters structure
    MCL_AO_ADD_DELTA_SPEED_REF_S16,                 //!<  7 - Add (force) a delta speed on the speed reference - x65536 - (1<<16)

    MCL_AO_NR_OF_AO                                 //!< Total of declared analog outputs
} MCL_AO_TYPE;




typedef struct
{
    // input
    float32 Vdc;                                     //!< [V]             DC-Bus Voltage
    ABC_COOR_SYST_F_TYPE Is_ABC;                     //!< [A]             Motor Phase Currents
    float32 Speed_Rot_Ref;                           //!< [rad/s]         Speed Reference
    sint32 Manual_Injection_Level;                   //!< [x32V or x32A]  define the peak level for manual injection according to the user request
    sint32 Manual_Injection_Level_Rate;              //!< [n/a x32]       Rate change of the level in level/s
    sint32 Manual_Injection_Param;                   //!< [n/a x32]       Depends on user request
    sint32 Manual_Injection_Param_Rate;              //!< [n/a x32]       Rate change of the parameter in parameter/s

    // output
    ABC_COOR_SYST_F_TYPE  Vs_ABC;                    //!< [V]     motor phase reference voltage in abc system
    ABC_COOR_SYST_F_TYPE  Duty;                      //!< [%]     PWM Duties
    ABC_COOR_SYST_F_TYPE  Duty_bc;                   //!< [%]     PWM Duties after modulation and before compensation
    uint8 Lowers_On;                                 //!< [n/a]   Turn on all lowers IGBTs
    uint8 All_Devices_Off;                           //!< [n/a]   Turn off all gates of IGBTs
    float32 Speed_Rot_Est;                           //!< [rad/s] Estimated Speed only for BPM
    float32 Torque;                                  //!< [Nm]    Estimated Torque
    float32 Position_Rot_Est;                        //!< [Rad]   Estimated Rotor Position
} MCL_IO_TYPE;



//! DTC parameters definition
typedef struct
{
	//! Mcl Input Processing Output
	ALPHA_BETA_COOR_SYST_F_TYPE Is_Alpha_Beta;       //!< [A]       motor phase current in alpha-beta reference system
	ALPHA_BETA_COOR_SYST_F_TYPE Vs_Alpha_Beta;       //!< [V]       motor phase voltage reconstructed in alpha-beta reference system
	DQ_COOR_SYST_F_TYPE Vs_DQ;                       //!< [V]       motor phase voltage in dq stator flux reference system
	float32 Is_Abs;                                  //!< [A]       motor stator current magnitude (modulus)
	float32 Vs_Max_Filt;                             //!< [V]       Mininum Dc bus voltage filtered
	ALPHA_BETA_COOR_SYST_F_TYPE Vs_Alpha_Beta_Rec;   //!< [V]       Motor phase voltage reconstructed in alpha-beta reference system
	ABC_COOR_SYST_F_TYPE Vabc_Rec;                   //!< [V]       Motor phase voltage reconstructed in abc system

	//! Mcl Observer variables
    // Mcl Observer Input
    DQ_COOR_SYST_F_TYPE Ldq_Ind;                     //!< [H]       Ld and Lq inductances
    float32 Ldq_Cross_Ind;                           //!< [H]       cross saturation from q do the d axis
    float32 Lqd_Cross_Ind;                           //!< [H]       cross saturation from d do the q axis

	float32 Stator_Flux_Mag;                         //!< [Vs]      stator flux magnitude
	float32 Load_Angle;                              //!< [rad]     load angle
	SIN_COS_F_TYPE Load_Angle_Sin_Cos;               //!< []        sin/cos of load angle
	float32 Stator_Temperature;                      //!< [degC]    temperature of motor stator windings

	//! Mcl speed controller variables
	float32 Speed_Rot_Ref_Abs;                       //!< [rad/s]   Absolute speed Reference mechanical
	float32 Speed_Rot;                               //!< [rad/s]   Speed measured/observed electrical
	float32 Speed_Rot_Mech_Abs;                      //!< [rad/s]   mechanical Absolute speed measured/observed
	float32 Speed_Rot_Mech;                          //!< [rad/s]   mechanical speed observed
	//! Mcl DQ Controller variables
	DQ_COOR_SYST_F_TYPE Force_Out_Volt;              //!< [V]       force dq output voltage

	SIN_COS_F_TYPE Sin_Cos_Position_Flux;            //!< []        sin/cos of rotor-flux position used in current park transformation
	float32 Torque_Ref;            	                 //!< [Nm]      Torque reference value
	float32 Force_Torque_Ref;            	         //!< [Nm]      Force torque reference value
	float32 Q_Torque_Ref;            	             //!< [Nm]      Torque reference value as output of DQ reference macroblock
	float32 Id_rotor_current;                        //!< [A]       rotor direct axis current
	float32 Id_stator_current;                       //!< [A]       rotor direct axis current
	float32 Iq_stator_current;                       //!< [A]       stator quadrature axis current
	float32 Iq_stator_current_abs;                   //!< [A]       absolute stator quadrature axis current
	float32 Torque;            	                     //!< [Nm]      Torque fed back value
	float32 Flux_Ref;            	                 //!< [Vs]      Flux reference value
	float32 Flux_Limit_By_Speed;                     //!< [Vs]      Flux limited by speed (field weakening operation)

	uint32 Elapsed_Time_After_Starting;              //!< [x Ts]    count time after starting
	float32 Force_Stator_Flux_Ref;                   //!< [Vs/rad]  force stator flux reference

	float32 Vs_Flux;                                 //!< [V]       Voltage available for flux weakening calculation
	float32 Rotor_Position;                          //!< [rad]     Rotor Position
	float32 Mean_Speed;                              //!< [rad/s]   Mean value speed calculated in 4Khz handler in a 250ms window.
	float32 IsRMS;                                   //!< [A]       RMS motor current

	float32 Stator_Resistance;                       //!< [ohm]     Resistance of stator windings
	float32 DcInjection_Angle;                       //!< [rad]     injected angle position during startup
	float32 Electrical_Power;                        //!< [W]       Motor Electrical power
	float32 Shaft_Power;                             //!< [W]       Motor shaft power

	float32 Torque_Application;                      //!< [Nm]      Torque for the application layer (sign corrected)
	float32 Rotor_Flux;                              //!< [Wb]      Rotor Flux
	float32 Delta_Speed_Rot_Ref;                     //!< [rad/s]   Delta rotor speed to be added to the reference value

	//! Resistance Estimation variables
	float32 Vs_Voltage;                              //!< [V]       Motor Voltage applied during resistance estimation phase.
	float32 Level_Injection;                         //!< [V]       Level of the voltage injection
} MCL_QUANTITIES_TYPE;

extern MCL_PARAMS_DISPL1_TYPE*  Mcl_Params_Displ1;                         //!< MCL parameters displacement 1
extern MCL_IO_TYPE Mcl_IO;
extern MCL_CS_TYPE Mcl_Cs;
extern MCL_QUANTITIES_TYPE Mcl_Quantities;
//Input Output Structures declarations ------ end

extern volatile sint32 Mcl_Thermal_Model_Resistance;
extern volatile sint32 Mcl_Estimated_Resistance;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Mcl__Initialize(void);
void Mcl__250usHandler(void);
void Mcl__1msHandler(void);
void Mcl__5msHandler(void);
void Mcl__25msHandler(void);
void Mcl__StartingReset(void);
void Mcl__StartingHandler(void);
void Mcl__RunningReset(void);
void Mcl__RunningHandler(void);
#ifdef PWM_DOUBLE_SLOT
void Mcl__2ndRunningHandler(void);
#endif
void Mcl__StoppingReset(void);
void Mcl__StoppingHandler(void);
void Mcl__ManualReset(void);
void Mcl__ManualHandler(void);

void Mcl__ResetAll(void);

BOOL_TYPE Mcl__SetParams(void*  mcl_params_address);
BOOL_TYPE Mcl__GetDigital(MCL_READ_DIGITAL_DATA_ENUM read_bit);
BOOL_TYPE Mcl__SetDigital(MCL_SET_DIGITAL_DATA_ENUM set_bit);
BOOL_TYPE Mcl__ResetDigital(MCL_SET_DIGITAL_DATA_ENUM reset_bit);
BOOL_TYPE Mcl__SetAnalog(MCL_AO_TYPE write_data, sint32 value);
#endif // MCL_H_

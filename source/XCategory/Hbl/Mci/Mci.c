/**
 *  @file       Mci.c
 *  @brief      Motor control interface
 *
 *  @details    Define the implementation of an API to control the motor, get its feedbacks, methods to set parameters.
 *              This is also a generic state machine enough to handle different motor control control techniques.
 *
    @startuml

    package "MCI2.0"  {
        InverterTemp :
        SpeedRefGen :
        SurgeRelay :
        Mathcalc :
        C_Extensions :

        package "Mcl" {
        package "MacroBlocks" {
                MclDqCtrl :
                MclDqRef :
                MclObserver :
                MclPwm :
                MclSpeedCtrl :
            }
        Mcl :
        Mcl ..> MacroBlocks
        }
    }

    InverterTemp -[hidden]- Mcl
    SpeedRefGen -[hidden]- Mcl
    SurgeRelay -[hidden]- Mcl
    Mathcalc -[hidden]- Mcl
    C_Extensions -[hidden]- Mcl
    @enduml
 *
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2017.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Mci.h"
#include "MotorSafetyMgr.h"
#include "Mcl.h"
#include "SpeedRefGen.h"

#if MCI_FVT_TESTS == ENABLED
    #include "MclFvt.h"
#endif

//!< Threshold to declare class B start up error in Seconds - If different, should be defined in a project specific file
#ifndef MCI_CLASS_B_STARTUP_THRES_SEC
    #define MCI_CLASS_B_STARTUP_THRES_SEC  10.0f
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//!< Post scaler to check error
#define MCI_CHECKERR_POSTSCALER     (uint8)(0.3f/0.005f)

//!< Threshold to declare initialization/power up error
#define MCI_INIT_ERROR_THRES        (uint16)(250.0f / 0.3f)

//!< Threshold to declare class B start up error - MCI_CLASS_B_STARTUP_THRES_SEC seconds by ticks of 0.3f (refer to MCI_CHECKERR_POSTSCALER)
#define MCI_CLASS_B_STARTUP_THRES   (uint16)(MCI_CLASS_B_STARTUP_THRES_SEC / 0.3f)

#ifndef MIN_SINT32
    #define MIN_SINT32  -2147483648L
#endif

#ifndef MAX_UINT16
    #define MAX_UINT16  65535L
#endif


#ifndef MAX_UINT32
    #define MAX_UINT32  4294967295UL
#endif



//! Define the behavior and algorithm flow control
typedef enum
{
    MCI_FORCE_LOWERS_ON,                            //!<  0 - Enabled all lowers IGBTs
    MCI_ALL_DEVICES_OFF,                            //!<  1 - Disable all 6 IGBTs
    MCI_REQUESTED_STOP,                             //!<  2 - Requested to stop
    MCI_REQUESTED_START,                            //!<  3 - Requested to start

    MCI_REQUESTED_MANUAL_DC_VOLTAGE,                //!<  4 - Requested to manually control the output DC voltage
    MCI_REQUESTED_MANUAL_AC_VOLTAGE,                //!<  5 - Requested to manually control the output AC voltage
    MCI_REQUESTED_MANUAL_DC_CURRENT,                //!<  6 - Requested to manually control the output DC current
    MCI_REQUESTED_MANUAL_AC_CURRENT,                //!<  7 - Requested to manually control the output AC current

    MCI_REQUESTED_FVT_MODE,                         //!<  8 - Requested to execute functional verification test (FVT)
} MCI_FLOWCONTROL_TYPE;


static MCI_PARAMS_TYPE* Mci_Parameters;
static BOOL_TYPE Mci_Parameters_Loaded;

//! Define the flags used to request manual injection
#define MCI_REQ_MANUAL_INJECTION_FLOW_FLAGS     ((MCI_FLOWCONTROL_TYPE)1<<MCI_REQUESTED_MANUAL_DC_VOLTAGE) +   \
                                                ((MCI_FLOWCONTROL_TYPE)1<<MCI_REQUESTED_MANUAL_AC_VOLTAGE) +   \
                                                ((MCI_FLOWCONTROL_TYPE)1<<MCI_REQUESTED_MANUAL_DC_CURRENT) +   \
                                                ((MCI_FLOWCONTROL_TYPE)1<<MCI_REQUESTED_MANUAL_AC_CURRENT)


MCI_CONTROL_STATE_TYPE Mci_Control_State;           //!< Define the states used in this controller

uint32 Mci_Error;                                   //!< [MCI_ERROR_TYPE] define all flags used in the mci management, use uint32
                                                    //!<                  rather MCI_ERROR_TYPE to prevent lint warnings.
uint32 Mci_Error_Holder;                            //!< [MCI_ERROR_TYPE] copy of the Mci_Error, use uint32 rather MCI_ERROR_TYPE
                                                    //!<                  to prevent lint warnings.
uint32 Mci_Flow_Control;                            //!< [MCI_FLOWCONTROL_TYPE] define some actions to control the algorithm flow
uint32 Mci_Status_Control;                          //!< [MCI_DI_TYPE] define some actions to control the algorithm state control
uint16 Mci_GP_Timer;                                //!< [seconds / MCI_CHECKERR_POSTSCALER] General Purpose timer.
                                                    //!< Timer to start up after POR. This timer also helps to filter out
                                                    //!< the tachometer signal and to perform the timeout during start up

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//! Converts a value into TRUE or FALSE; 0 is FALSE, TRUE otherwise.
#ifndef GET_TRUE_FALSE
    #define GET_TRUE_FALSE(value)           (BOOL_TYPE)(((value) == 0) ? (FALSE) : (TRUE))
#endif

//! MCI_SET_ERROR sets the flags data and its holder
#define MCI_SET_ERROR(mci_flags)                BIT_SET(Mci_Error_Holder, (mci_flags));                 \
                                                BIT_SET(Mci_Error_Holder, MCI_ERROR_ANY_ERROR_FOUND);   \
                                                BIT_SET(Mci_Error, (mci_flags))

//! MCI_CLR_ERROR resets only the flags data and does not touch the holder
#define MCI_CLR_ERROR(mci_flags)                BIT_CLR(Mci_Error, (mci_flags))

//! MCI_FLOWCONTROL_TYPE controls the algorithm flow
#define MCI_SET_FLOW_CTL(mci_flowcontrol)       BIT_SET(Mci_Flow_Control, (mci_flowcontrol))
#define MCI_CLR_FLOW_CTL(mci_flowcontrol)       BIT_CLR(Mci_Flow_Control, (mci_flowcontrol))

//! MCI_DI_TYPE controls the DI state of motor control
#define MCI_SET_STATUS_CTL(mci_statuscontrol)   BIT_SET(Mci_Status_Control, (mci_statuscontrol))
#define MCI_CLR_STATUS_CTL(mci_statuscontrol)   BIT_CLR(Mci_Status_Control, (mci_statuscontrol))


#include "Mci_prv.h"                                //! This include is made here in order to make some #define above visible in the prv file

//! Definition of some methods. They might be not defined in the Mci_prv.h file, however the macro is called here. So, define the macro as empty.
#ifndef MCI_STARTING_ENTRY
    #define MCI_STARTING_ENTRY()
#endif

#ifndef MCI_STARTING_EXIT
    #define MCI_STARTING_EXIT()
#endif

#ifndef MCI_RUNNING_ENTRY
    #define MCI_RUNNING_ENTRY()
#endif

#ifndef MCI_RUNNING_EXIT
    #define MCI_RUNNING_EXIT()
#endif

#ifndef MCI_STOPPING_ENTRY
    #define MCI_STOPPING_ENTRY()
#endif

#ifndef MCI_STOPPING_EXIT
    #define MCI_STOPPING_EXIT()
#endif

#ifndef MCI_IDLE_PWM
    #define MCI_IDLE_PWM()
#endif

#ifndef MCI_IDLE_LEAVE
    #define MCI_IDLE_LEAVE()    TRUE
#endif

#ifndef MCI_IDLE_EXIT
    #define MCI_IDLE_EXIT()
#endif

#ifndef MCI_MANUAL_INJECTION_EXIT
    #define MCI_MANUAL_INJECTION_EXIT()
#endif

#ifndef MCI_FAILED_PWM
    #define MCI_FAILED_PWM()
#endif

#ifndef MCI_FAILED_EXIT
    #define MCI_FAILED_EXIT()
#endif

#ifndef MCL_SET_STOPPING_COAST_DOWN
    #define MCL_SET_STOPPING_COAST_DOWN()
#endif

#ifndef MCL_SET_STOPPING_LOWERS_ON
    #define MCL_SET_STOPPING_LOWERS_ON()
#endif

#ifndef MCL_SET_STOPPING_REQUEST
    #define MCL_SET_STOPPING_REQUEST()
#endif

#ifndef MCI_STARTING_PWM_STEP2
    #define MCI_STARTING_PWM_STEP2()
#endif

#ifndef MCI_FVT_ENTRY
    #define MCI_FVT_ENTRY()
#endif

#ifndef MCI_FVT_EXIT
    #define MCI_FVT_EXIT()
#endif



//! If the post scaler is required.
#if (defined MCI_IDLE_2ND_PWM)             || \
    (defined MCI_STARTING_2ND_PWM)         || \
    (defined MCI_RUNNING_2ND_PWM)          || \
    (defined MCI_STOPPING_2ND_PWM)         || \
    (defined MCI_MANUAL_INJECTION_2ND_PWM) || \
    (defined MCI_FAILED_2ND_PWM)

    //! Define to use it.
    #define USE_HALF_PWM_FREQUENCY

#endif


/////////////////////// Callbacks; if they are not defined, they are declared as empty! - START
// These callbacks are usually defined in the Mci_prm.h file.
#ifndef MCI_INITIALIZE_CALLBACK
    #define MCI_INITIALIZE_CALLBACK()
#endif

#ifndef MCI_PWM_CALLBACK
    #define MCI_PWM_CALLBACK()
#endif

#ifndef MCI_END_PWM_CALLBACK
    #define MCI_END_PWM_CALLBACK()
#endif

#ifndef MCI_HALLS_CALLBACK
    #define MCI_HALLS_CALLBACK()
#endif

#ifndef MCI_250US_CALLBACK
    #define MCI_250US_CALLBACK()
#endif

#ifndef MCI_1MS_CALLBACK
    #define MCI_1MS_CALLBACK()
#endif

#ifndef MCI_5MS_CALLBACK
    #define MCI_5MS_CALLBACK()
#endif

#ifndef MCI_25MS_CALLBACK
    #define MCI_25MS_CALLBACK()
#endif


// Definition to connect surge relay module to the Mci.
#ifdef SURGE_RELAY_USED

    #include "SurgeRelay.h"

    #define SURGE_RELAY_INITIALIZE()        SurgeRelay__Initialize()
    #define SURGE_RELAY_25MS_HANDLER()      SurgeRelay__25msHandler()
    #define SURGE_RELAY_READY()             SurgeRelay__IsBusReady()

#else

    // Otherwise, declare the surge relay as empty module.
    #define SURGE_RELAY_INITIALIZE()
    #define SURGE_RELAY_25MS_HANDLER()
    #define SURGE_RELAY_READY()             TRUE

#endif // SURGE_RELAY_USED


// Definition to connect the inverter temperature module to the Mci.
#if defined MCI_MEASURE_INVERTER_TEMP

    #include "InverterTemp.h"

    #define INVERTER_TEMP_INITIALIZE()              InverterTemp__Initialize()
    #define INVERTER_TEMP_25MS_HANDLER()            InverterTemp__Handler()
    #define INVERTER_TEMP_READ_TEMP()

    #define INVERTER_TEMP_GET_TEMP_S16()            InverterTemp__GetInverterTemp_s16()
    #define INVERTER_TEMP_GET_TEMP_RAW_DATA()       InverterTemp__GetInverterRawTemp()


    #ifndef INVERTER_TEMP_SETTINGFILE_SUPPORT
    #define INVERTER_TEMP_SETTINGFILE_SUPPORT DISABLED
    #endif

#else

    // Otherwise, declare the inverter as empty module.
    #define INVERTER_TEMP_INITIALIZE()
    #define INVERTER_TEMP_25MS_HANDLER()
    #define INVERTER_TEMP_READ_TEMP()
    #define INVERTER_TEMP_SETTINGFILE_SUPPORT DISABLED
#endif // MCI_MEASURE_INVERTER_TEMP

#if (POWERMODULE_MANAGER_USED == ENABLED)
    #include "PowerModuleManager.h"

    #define POWERMODULEMANAGER__INITIALIZE()        PowerModuleManager__Initialize()
    #define POWERMODULEMANAGER__25MS_HANDLER()      PowerModuleManager__25msHandler()
#else
    #define POWERMODULE_MANAGER_USED    DISABLED
    #define POWERMODULEMANAGER__INITIALIZE()
    #define POWERMODULEMANAGER__25MS_HANDLER()
#endif // POWERMODULE_MANAGER_USED

#ifdef ENABLE_API220_CALLBACKS

    #include "APIMotorControl.h"

    #define API220_PUBLISHDIGITAL_CALLBACK(motor, write_data, value, ret_val)   APIMotorControl__PublishSetDigital((motor), (write_data), (value), (ret_val))
    #define API220_PUBLISHANALOG_CALLBACK(motor, write_data, value, ret_val)    APIMotorControl__PublishSetAnalog((motor), (write_data), (value), (ret_val))
    #define API220_RUN_CALLBACK(motor, speed_rpm, ramp_rpm_per_s, response)     APIMotorControl__PublishRun((motor), (speed_rpm), (ramp_rpm_per_s), (response))
	#define API220_MANUAL_INJECTION_CALLBACK(motor, 			\
											method, 			\
											level_x32, 			\
											level_rate_x32, 	\
											param_x32, 			\
											param_rate_x32, 	\
											response)							// todo Luigi needs to implement the API220 call back of manual injection
    #define API220_STOP_CALLBACK(motor, ramp_rpm_per_s, response)               APIMotorControl__PublishStop((motor), (ramp_rpm_per_s), (response))

#else

    #define API220_PUBLISHDIGITAL_CALLBACK(motor, write_data, value, ret_val)
    #define API220_PUBLISHANALOG_CALLBACK(motor, write_data, value, ret_val)
    #define API220_RUN_CALLBACK(motor, speed_rpm, ramp_rpm_per_s, response)
	#define API220_MANUAL_INJECTION_CALLBACK(motor, 			\
											method, 			\
											level_x32, 			\
											level_rate_x32, 	\
											param_x32, 			\
											param_rate_x32, 	\
											response)
	#define API220_STOP_CALLBACK(motor, ramp_rpm_per_s, response)

#endif


#ifndef API220_DEBUG_MODE
    #define API220_DEBUG_MODE   DISABLED
#endif


#define MCI_PARAMS_LOADED_CHECKS (3 + INVERTER_TEMP_SETTINGFILE_SUPPORT + POWERMODULE_MANAGER_USED)

/////////////////////// Callbacks; if they are not defined, they are declared as empty! - END


//////////////////////////////////////////////////////////////////////////////////////
// Floating point \ Fixed Point MCL management  -- usually defined in mci_prv.h     //
//                      if not defined, consider Fixed point notation               //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef MCL_SPEED_REF_GEN
    #define MCL_SPEED_REF_GEN()  (sint32)(SpeedRefGen__Generator() / BASE_SPEED_RADS * (float32)(1U<<IQ15))
#endif

#ifndef MCL_CURRENT
    #define MCL_CURRENT(arg)  (arg)
#endif

#ifndef MCL_VOLTAGE
    #define MCL_VOLTAGE(arg)  (arg)
#endif

#ifndef MCL_DUTY
    #define MCL_DUTY(arg) (arg)
#endif

#ifndef MCL_NOTATION_TYPE
    #define MCL_NOTATION_TYPE  sint32
#endif

#ifndef MCL__ABS
    #include "McMathCalc_macros.h"
    #define MCL__ABS(value)  MATHCALC__ABS(value)
#endif

//////////////////////////////////////////////////////////////////////////////////////




static void Initialize_Current_DCBus_Voltage(void);
static void Read_Current_DCBus_Voltage(sint32 ia_adc, sint32 ib_adc, sint32 ic_adc, sint32 dcbus_adc);
static void Drive_The_Motor(void);
static void Check_Errors_Pwm_Handler(void);
static void Check_Errors_5ms_Handler(void);
static void Update_Status(void);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Mci and its variables.
 *
 */
void Mci__Initialize(void)
{
    MCI_INITIALIZE_CALLBACK();

    SURGE_RELAY_INITIALIZE();
    INVERTER_TEMP_INITIALIZE();
    POWERMODULEMANAGER__INITIALIZE();

    Initialize_Current_DCBus_Voltage();
    Mcl__Initialize();
    SpeedRefGen__Initialize();

    // Initialize the errors that initializing procedures handles;
    // Other error are declared may be either not used, or not defined, or not specified
    // or it will require the motor be started to be checked.
    Mci_Error = MCI_POWER_UP_ERROR_TO_TEST;

    // No error shall be reported while starting up the inverter
    Mci_Error_Holder = 0;
    Mci_Flow_Control = 0;
    Mci_Status_Control = 0;
    Mci_GP_Timer = 0;

    Mci_Control_State = MCI_INITIALIZING;
    Mci_Parameters_Loaded = FALSE;

}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mci events every Pwm interrupt.
 *
    @startuml{MCI_Class_A_State_Machine.png}

        title Motor Control Interface - Main state machine

        [*] --> MCI_INITIALIZING

        MCI_INITIALIZING : Waits until data are reliable \nfrom either Class A and Class B
        MCI_INITIALIZING --> MCI_FAILED : [Initialization timer expired]
        MCI_INITIALIZING --> MCI_IDLE   : [No error found] AND\n[Class B in IDLE]


        MCI_IDLE : Motor is idling.
        MCI_IDLE : Entry / MCI_IDLE_ENTRY()
        MCI_IDLE : Do / MCI_IDLE_PWM() - MCI_IDLE_2ND_PWM()
        MCI_IDLE : Exit / MCI_IDLE_EXIT()
        MCI_IDLE --> MCI_CLASS_B_STARTING   : [User requested to run] OR\n[User requested to manual control]
        MCI_IDLE --> MCI_FAILED : [Any error found in either Class A or Class B]


        MCI_CLASS_B_STARTING : Waiting for Class B to be ready,\ne.g. perform plausibility check.
        MCI_CLASS_B_STARTING --> MCI_CLASS_B_STOPPING   : [User requested to stop]
        MCI_CLASS_B_STARTING --> MCI_MANUAL_CONTROL     : [User requested to manual control] AND\n[Class A granted access to PWM]
        MCI_CLASS_B_STARTING --> MCI_STARTING           : [User requested to run] AND\n[Class A granted access to PWM]
        MCI_CLASS_B_STARTING --> MCI_FAILED             : [Any error found in either Class A or Class B]


        MCI_CLASS_B_STOPPING : Waiting for Class B to be finish its checks,\ne.g. perform plausibility check.
        MCI_CLASS_B_STOPPING --> MCI_IDLE               : [Class B is IDLE]
        MCI_CLASS_B_STOPPING --> MCI_FAILED             : [Any error found in either Class A or Class B]


        MCI_FAILED : Fault found in either inverter or motor
        MCI_FAILED : Check and recover from failure
        MCI_FAILED --> MCI_IDLE   : [Class B in IDLE] AND\n[All errors are cleared]


        MCI_STARTING : Motor is performing activities before running control technique
        MCI_STARTING : Entry / MCI_STARTING_ENTRY()
        MCI_STARTING : Do / MCI_STARTING_PWM() - MCI_STARTING_2ND_PWM()
        MCI_STARTING : Exit / MCI_STARTING_EXIT()
        MCI_STARTING --> MCI_CLASS_B_STOPPING   : [User requested to stop]
        MCI_STARTING --> MCI_RUNNING            : [MCI_STARTING_GUARD_EXIT() == TRUE]
        MCI_STARTING --> MCI_FAILED             : [Any error found in either Class A or Class B]


        MCI_RUNNING : Motor is controlled by FOC/DTC, e.g. automatic control.
        MCI_RUNNING : Entry / MCI_RUNNING_ENTRY()
        MCI_RUNNING : Do / MCI_RUNNING_PWM() - MCI_RUNNING_2ND_PWM()
        MCI_RUNNING : Exit / MCI_RUNNING_EXIT()
        MCI_RUNNING --> MCI_STOPPING            : [MCI_RUNNING_GUARD_EXIT() == TRUE]
        MCI_RUNNING --> MCI_FAILED              : [Any error found in either Class A or Class B]


        MCI_MANUAL_CONTROL : Motor is under manual control.
        MCI_MANUAL_CONTROL : Entry / MCI_MANUAL_INJECTION_ENTRY()
        MCI_MANUAL_CONTROL : Do / MCI_MANUAL_INJECTION_PWM() - MCI_MANUAL_INJECTION_2ND_PWM()
        MCI_MANUAL_CONTROL : Exit / MCI_MANUAL_INJECTION_EXIT()
        MCI_MANUAL_CONTROL --> MCI_CLASS_B_STOPPING     : [User requested to stop]
        MCI_MANUAL_CONTROL --> MCI_FAILED               : [Any error found in either Class A or Class B]


        MCI_STOPPING : Motor is controlled by class A and it is stopping.
        MCI_STOPPING : Entry / MCI_STOPPING_ENTRY()
        MCI_STOPPING : Do / MCI_STOPPING_PWM() - MCI_STOPPING_2ND_PWM()
        MCI_STOPPING : Exit / MCI_STOPPING_EXIT()
        MCI_STOPPING --> MCI_CLASS_B_STOPPING   : [MCI_STOPPING_GUARD_EXIT() == TRUE]
        MCI_STOPPING --> MCI_FAILED             : [Any error found in either Class A or Class B]

    @enduml
 */
void Mci__PwmHandler(sint32 ia_adc, sint32 ib_adc, sint32 ic_adc, sint32 dcbus_adc)
{
#ifdef USE_HALF_PWM_FREQUENCY

    static unsigned char pwm_post_scaler = 0;

    pwm_post_scaler++;

#endif

    // Read and scales offseted values from class B
    Read_Current_DCBus_Voltage(ia_adc, ib_adc, ic_adc, dcbus_adc);

    // Callback running at pwm rate
    MCI_PWM_CALLBACK();

    // Check errors at pwm rate
    Check_Errors_Pwm_Handler();

    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    // If any error is found...
    if(Mci_Error)
    {
        // then set the state machine to error
        // if it is not under initialization.
        if((Mci_Control_State > MCI_INITIALIZING)|| \
           ((Mci_Control_State == MCI_INITIALIZING)&&(BIT_TEST(Mci_Error, MCI_ERROR_INITIALIZATION_FAILED))))
        {
            Mci_Control_State = MCI_FAILED;

            MCI_DEFAULT_ERROR_REACTION();
        }
    }


    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    if(Mci_Control_State == MCI_RUNNING)
    {

#if (defined MCI_RUNNING_2ND_PWM)

        if(pwm_post_scaler & 0x01)
        {
            MCI_RUNNING_PWM();

            if(MCI_RUNNING_GUARD_EXIT() == TRUE)
            {
                MCI_RUNNING_EXIT();
                MCI_STOPPING_ENTRY();
                Mci_Control_State = MCI_STOPPING;
            }
        }
        else
        {
            Mcl_IO.Speed_Rot_Ref = MCL_SPEED_REF_GEN();

            MCI_RUNNING_2ND_PWM();
        }

#else

        Mcl_IO.Speed_Rot_Ref = MCL_SPEED_REF_GEN();

        MCI_RUNNING_PWM();

        if(MCI_RUNNING_GUARD_EXIT() == TRUE)
        {
            MCI_RUNNING_EXIT();
            MCI_STOPPING_ENTRY();
            Mci_Control_State = MCI_STOPPING;
        }

#endif

        Drive_The_Motor();
    }



    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else if(Mci_Control_State == MCI_STARTING)
    {

#if (defined MCI_STARTING_2ND_PWM)

        if(pwm_post_scaler & 0x01)
        {
            MCI_STARTING_PWM();
        }
        else
        {
            MCI_STARTING_2ND_PWM();
        }

#else

        MCI_STARTING_PWM();

#endif

        Drive_The_Motor();

        if(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP))
        {
            MCI_STARTING_EXIT();
            Mci_Control_State = MCI_CLASS_B_STOPPING;
        }
        else if(MCI_STARTING_GUARD_EXIT() == TRUE)
        {
            MCI_STARTING_EXIT();
            MCI_RUNNING_ENTRY();
            Mci_Control_State = MCI_RUNNING;
        }

        // temporary fix - required to optimize "robust start"
        // issue found: drive to update pwm is required due to slot overflow
        if(MCI_STARTING_GUARD_EXIT() == FALSE)
        {
            MCI_STARTING_PWM_STEP2();
        }
    }



    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else if(Mci_Control_State == MCI_CLASS_B_STARTING)
    {
        MotorSafetyMgr__Prepare2StartMotor();

        if(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP))
        {
            Mci_Control_State = MCI_CLASS_B_STOPPING;
        }
        else if(MotorSafetyMgr__DoesClassAHaveAccessToPwm() == TRUE)
        {
            if(Mci_Flow_Control & MCI_REQ_MANUAL_INJECTION_FLOW_FLAGS)
            {
                MCI_MANUAL_INJECTION_ENTRY();
                Mci_Control_State = MCI_MANUAL_CONTROL;
            }
            else if(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_START))
            {
                SpeedRefGen__Reset();
                MCI_STARTING_ENTRY();
                Mci_Control_State = MCI_STARTING;
            }
#if defined MCI_FVT_PMW
            else if(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_FVT_MODE))
            {
                MCI_FVT_ENTRY();
                Mci_Control_State = MCI_FVT_MODE;
            }
#endif

            MCI_CLR_FLOW_CTL(MCI_REQUESTED_START);
        }
    }



    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else if(Mci_Control_State == MCI_STOPPING)
    {

#if defined MCI_STOPPING_2ND_PWM

        if(pwm_post_scaler & 0x01)
        {
            MCI_STOPPING_PWM();

            if(MCI_STOPPING_GUARD_EXIT())
            {
                MCI_STOPPING_EXIT();
                Mci_Control_State = MCI_CLASS_B_STOPPING;
            }
        }
        else
        {
             MCI_STOPPING_2ND_PWM();
        }

#else

        MCI_STOPPING_PWM();

        if(MCI_STOPPING_GUARD_EXIT())
        {
            MCI_STOPPING_EXIT();
            Mci_Control_State = MCI_CLASS_B_STOPPING;
        }

#endif

        Drive_The_Motor();

    }




    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else if(Mci_Control_State == MCI_CLASS_B_STOPPING)
    {
        MotorSafetyMgr__Prepare2StopMotor();

        if(MotorSafetyMgr__IsInIdle() == TRUE)
        {
            SpeedRefGen__Initialize();

            MCI_IDLE_ENTRY();

            Mci_Control_State = MCI_IDLE;
        }
    }




    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else if(Mci_Control_State == MCI_INITIALIZING)
    {
		// if no error found, then move to the next state
		if(!Mci_Error)
		{
			if(MotorSafetyMgr__IsInIdle() == TRUE)
			{
				Mci_Error_Holder = 0;

				MCI_IDLE_ENTRY();
				Mci_Control_State = MCI_IDLE;
			}
		}
    }

    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else if(Mci_Control_State == MCI_IDLE)
    {
        Mcl_IO.Speed_Rot_Ref = 0;

        MCI_CLR_FLOW_CTL(MCI_REQUESTED_STOP);
        MCI_CLR_FLOW_CTL(MCI_ALL_DEVICES_OFF);
        MCI_CLR_FLOW_CTL(MCI_FORCE_LOWERS_ON);

#if defined MCI_IDLE_2ND_PWM

        if(pwm_post_scaler & 0x01)
        {
            MCI_IDLE_PWM();
        }
        else
        {
            MCI_IDLE_2ND_PWM();
        }

#else

        MCI_IDLE_PWM();

#endif

        if(((BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_START))       ||
            (BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_FVT_MODE))    ||
            (Mci_Flow_Control & MCI_REQ_MANUAL_INJECTION_FLOW_FLAGS) ) &&
            (MCI_IDLE_LEAVE()))
        {
            MCI_IDLE_EXIT();
            Mci_GP_Timer = 0;
            Mci_Control_State = MCI_CLASS_B_STARTING;
        }
    }




    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else if(Mci_Control_State == MCI_DEBUGGING)
    {

    }

    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else if(Mci_Control_State == MCI_MANUAL_CONTROL)
    {

#if defined MCI_MANUAL_INJECTION_2ND_PWM

        if(pwm_post_scaler & 0x01)
        {
            MCI_MANUAL_INJECTION_PWM();
        }
        else
        {
            MCI_MANUAL_INJECTION_2ND_PWM();
        }

#else

        MCI_MANUAL_INJECTION_PWM();

#endif

        Drive_The_Motor();

        if(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP))
        {
            // Clear all requests.
            MCI_CLR_FLOW_CTL(MCI_REQUESTED_MANUAL_DC_VOLTAGE);
            MCI_CLR_FLOW_CTL(MCI_REQUESTED_MANUAL_AC_VOLTAGE);
            MCI_CLR_FLOW_CTL(MCI_REQUESTED_MANUAL_DC_CURRENT);
            MCI_CLR_FLOW_CTL(MCI_REQUESTED_MANUAL_AC_CURRENT);

            MCI_MANUAL_INJECTION_EXIT();
            Mci_Control_State = MCI_CLASS_B_STOPPING;
        }
    }

    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
#if defined MCI_FVT_PMW
    else if(Mci_Control_State == MCI_FVT_MODE)
    {

        #if defined MCI_FVT_2ND_PWM

            if(pwm_post_scaler & 0x01)
            {
                MCI_FVT_PMW();
            }
            else
            {
                MCI_FVT_2ND_PWM();
            }

        #else

                MCI_FVT_PMW();

        #endif

        Drive_The_Motor();

        if( (MCI_FVT_GUARD_EXIT() == TRUE) ||
            (BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP)))
        {
            if(!Mci_Error)
            {
                Mci_Error_Holder = 0;

                MCI_CLR_FLOW_CTL(MCI_REQUESTED_FVT_MODE);

                MCI_FVT_EXIT();
                Mci_Control_State = MCI_CLASS_B_STOPPING;
            }
        }
    }

#endif

    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    ////////////////////////////////////////////////
    else // MCI_FAILED
    {
#if defined MCI_FAILED_2ND_PWM

        if(pwm_post_scaler & 0x01)
        {
            MotorSafetyMgr__Prepare2StopMotor();

            MCI_FAILED_PWM();
        }
        else
        {
            MCI_FAILED_2ND_PWM();
        }

#else

        MotorSafetyMgr__Prepare2StopMotor();

        MCI_FAILED_PWM();

#endif

        if(MotorSafetyMgr__IsInIdle() == TRUE)
        {
        	// Ensure the initialization error flag will be reset
        	// in a later call of Check_Errors_5ms_Handler()
        	Mci_GP_Timer = 0;

            SpeedRefGen__Initialize();

            if(!Mci_Error)
            {
                MCI_CLR_FLOW_CTL(MCI_REQUESTED_START);
                MCI_CLR_FLOW_CTL(MCI_ALL_DEVICES_OFF);
                MCI_CLR_FLOW_CTL(MCI_REQUESTED_FVT_MODE);
                MCI_CLR_FLOW_CTL(MCI_REQUESTED_MANUAL_DC_VOLTAGE);
                MCI_CLR_FLOW_CTL(MCI_REQUESTED_MANUAL_AC_VOLTAGE);
                MCI_CLR_FLOW_CTL(MCI_REQUESTED_MANUAL_DC_CURRENT);
                MCI_CLR_FLOW_CTL(MCI_REQUESTED_MANUAL_AC_CURRENT);

                MCI_FAILED_EXIT();
                MCI_IDLE_ENTRY();
                Mci_Control_State = MCI_IDLE;
            }
        }
    }

    // Callback running at pwm rate, executed at end of pwm call
    MCI_END_PWM_CALLBACK();
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mci events every Hall interrupt.
 *
 *  @param
 *  @return
 */
void Mci__HallHandler(void)
{
    MCI_HALLS_CALLBACK();
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mci events of 250us.
 *
 *  @param
 *  @return
 */
void Mci__250usHandler(void)
{
    MCI_250US_CALLBACK();

    Mcl__250usHandler();
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mci events of 1ms.
 *
 *  @param      none
 *  @return     none
 */
void Mci__1msHandler(void)
{
    MCI_1MS_CALLBACK();

    Mcl__1msHandler();
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mci events of 5ms.
 *
 *  @param
 *  @return
 */
void Mci__5msHandler(void)
{
    MCI_5MS_CALLBACK();

    Check_Errors_5ms_Handler();
    Update_Status();

    Mcl__5msHandler();
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mci events of 25ms.
 *
 *  @param      none
 *  @return     none
 */
void Mci__25msHandler(void)
{
    MCI_25MS_CALLBACK();

    SURGE_RELAY_25MS_HANDLER();
    INVERTER_TEMP_25MS_HANDLER();
    POWERMODULEMANAGER__25MS_HANDLER();

    Mcl__25msHandler();
}




/**
 * Run the motor, this will start the motor to run from zero RPM until a given speed with a given RPM/s. And also,
 * if the motor is already running, the speed can be changed using this method.
 * @param motor selects which motor to be used (one motor application, uses only MOTOR0)
 * @param target_rpm is the next required speed
 * @param rpm_per_s is the acceleration to the next speed. This parameter is POSITIVE the Mci will take care of its
 * sign if a negative speed or deceleration is needed.
 * @return MCI_CMD_INDEX_OUT_RANGE     -7 - [T/F] - command was denied by Mci because selected motor does not exist
 * @return MCI_CMD_STOP_REQUESTED      -6 - [T/F] - command was denied by Mci because motor is stopping by user request
 * @return MCI_CMD_PARAM_NOT_LOADED    -5 - [T/F] - denied because parameters are not loaded
 * @return MCI_CMD_TIMEOUT             -4 - [T/F] - timeout due to long feedback time
 * @return MCI_CMD_RAMP_OUT_OF_RANGE   -3 - [T/F] - denied due to acceleration out of range
 * @return MCI_CMD_SPEED_OUT_OF_RANGE  -2 - [T/F] - denied due to speed out of range
 * @return MCI_CMD_DENIED              -1 - [T/F] - denied due reason not disclosed.
 * @return MCI_CMD_ACCEPTED             0 - [T/F] - accepted
 */
MCI_CMD_RESPONSE_TYPE Mci__Run(MOTOR_ENUM_TYPE motor, sint32 speed_rpm, sint32 ramp_rpm_per_s)
{
    MCI_CMD_RESPONSE_TYPE response;
    sint16 temp_s16;

    response = MCI_CMD_DENIED;

	#ifdef MAX_ACCELERATION_SAT
    if(ramp_rpm_per_s > MAX_ACCELERATION_SAT)
    {
        ramp_rpm_per_s = MAX_ACCELERATION_SAT;
    }
	#endif

    if(motor == MOTOR0)
    {
        if(BIT_TEST(Mci_Status_Control, MCI_DI_PARAMETERS_LOADED))
        {
            if(!(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP)))
            {
                temp_s16 = MATHCALC__ABS(speed_rpm);

                if( (temp_s16 <= Mci_Parameters->Max_Speed_Rpm) &&
                    (temp_s16 >= Mci_Parameters->Min_Speed_Rpm) )
                {
                    temp_s16 = MATHCALC__ABS(ramp_rpm_per_s);

                    if( (temp_s16 <= Mci_Parameters->Max_Rpm_Per_Sec_Accel) &&
                        (temp_s16 >=  Mci_Parameters->Min_Rpm_Per_Sec_Accel) )
                    {
                        if(Mci_Control_State == MCI_IDLE)
                        {
                            MCI_SET_FLOW_CTL(MCI_REQUESTED_START);

                            SpeedRefGen__SetNewSpeed(speed_rpm, ramp_rpm_per_s);

                            response = MCI_CMD_ACCEPTED;
                        }
                        else if(Mci_Control_State == MCI_RUNNING)
                        {
                            SpeedRefGen__SetNewSpeed(speed_rpm, ramp_rpm_per_s);

                            response = MCI_CMD_ACCEPTED;
                        }
                    }
                #if defined MCL_SET_RUNNING_COAST_DOWN
                    else if(temp_s16 == 0)
                    {
                        if(Mci_Control_State == MCI_RUNNING)
                        {
                            if(SpeedRefGen__SetNewSpeed(speed_rpm, 0) == SPEED_REF_NEG_STEP)
                            {
                                if(MCL_SET_RUNNING_COAST_DOWN(TRUE) == TRUE)
                                {
                                    response = MCI_CMD_ACCEPTED;
                                }
                            }
                        }
                    }
                #endif
                    else
                    {
                        response = MCI_CMD_RAMP_OUT_OF_RANGE;
                    }
                }
                else
                {
                    response = MCI_CMD_SPEED_OUT_OF_RANGE;
                }
            }
            else
            {
                response = MCI_CMD_STOP_REQUESTED;
            }
        }
        else
        {
            response = MCI_CMD_PARAM_NOT_LOADED;
        }
    }
    else
    {
		response = MCI_CMD_INDEX_OUT_RANGE;
    }

    API220_RUN_CALLBACK(motor, speed_rpm, ramp_rpm_per_s, response);

    return(response);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Manual injection - used to control manually voltage or current injection in the motor.
 *
 * @param   motor - motor indexer. Select what motor will be manually controlled
 * @param   method - select method to be controlled. Methods implemented are:
 *                         MCI_MANUAL_INJECTION_DC_VOLTAGE - DC voltage, requires voltage level and position
 *                         MCI_MANUAL_INJECTION_AC_VOLTAGE - AC voltage, requires voltage level and frequency
 *                         MCI_MANUAL_INJECTION_DC_CURRENT - DC current, requires current level [A], current rate [A/s] and position [degree]
 *                         MCI_MANUAL_INJECTION_AC_CURRENT - AC current, requires current level and frequency
 *
 * @param   level_x32 - targeted level x32;  [V] if selected method is voltage;
 *                                           [A] if method is current
 * @param   level_rate_x32 - level rate x32; [V/s] if selected method is voltage;
 *                                           [A/s] if method is current
 * @param   param_x32 - auxiliary parameter x32; [Hz] if selected method is AC voltage/current;
 *                                               [degree] if method is DC voltage/current
 * @param   param_rate_x32 - auxiliary parameter rate x32; [Hz/s] if selected method is AC voltage/current;
 *                                                         not used if method is DC voltage/current
 * @return MCI_CMD_INDEX_OUT_RANGE     -7 - [T/F] - command was denied by Mci because selected motor does not exist
 * @return MCI_CMD_STOP_REQUESTED      -6 - [T/F] - command was denied by Mci because motor is stopping by user request
 * @return MCI_CMD_PARAM_NOT_LOADED    -5 - [T/F] - denied because parameters are not loaded
 * @return MCI_CMD_TIMEOUT             -4 - [T/F] - timeout due to long feedback time
 * @return MCI_CMD_RAMP_OUT_OF_RANGE   -3 - [T/F] - denied due to acceleration out of range
 * @return MCI_CMD_SPEED_OUT_OF_RANGE  -2 - [T/F] - denied due to speed out of range
 * @return MCI_CMD_DENIED              -1 - [T/F] - denied due reason not disclosed.
 * @return MCI_CMD_ACCEPTED             0 - [T/F] - accepted
 *
 */
MCI_CMD_RESPONSE_TYPE Mci__Manual_Injection(MOTOR_ENUM_TYPE motor, MCI_INJECTION_TYPE method, sint32 level_x32, sint32 level_rate_x32, sint32 param_x32, sint32 param_rate_x32)
{
    MCI_CMD_RESPONSE_TYPE response;

    response = MCI_CMD_DENIED;

    if(motor == MOTOR0)
    {
        if(BIT_TEST(Mci_Status_Control, MCI_DI_PARAMETERS_LOADED))
        {
            if(!(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP)))
            {
                if((Mci_Control_State == MCI_IDLE)||(Mci_Control_State == MCI_MANUAL_CONTROL))
                {
                    Mcl_IO.Manual_Injection_Level = MATHCALC__ABS(level_x32);
                    Mcl_IO.Manual_Injection_Level_Rate = MATHCALC__ABS(level_rate_x32);

                    switch(method)
                    {
                        case MCI_MANUAL_INJECTION_DC_VOLTAGE:
                            Mcl_IO.Manual_Injection_Param = param_x32;
                            Mcl_IO.Manual_Injection_Param_Rate = 0;
                            Mcl__SetDigital(MCL_WRITE_INJECTION_DC_VOLTAGE);
                            MCI_SET_FLOW_CTL(MCI_REQUESTED_MANUAL_DC_VOLTAGE);
                            break;

                        case MCI_MANUAL_INJECTION_AC_VOLTAGE:
                            Mcl_IO.Manual_Injection_Param = param_x32;
                            Mcl_IO.Manual_Injection_Param_Rate = MATHCALC__ABS(param_rate_x32);
                            Mcl__SetDigital(MCL_WRITE_INJECTION_AC_VOLTAGE);
                            MCI_SET_FLOW_CTL(MCI_REQUESTED_MANUAL_AC_VOLTAGE);
                            break;

                        case MCI_MANUAL_INJECTION_DC_CURRENT:
                            Mcl_IO.Manual_Injection_Param = param_x32;
                            Mcl_IO.Manual_Injection_Param_Rate = 0;
                            Mcl__SetDigital(MCL_WRITE_INJECTION_DC_CURRENT);
                            MCI_SET_FLOW_CTL(MCI_REQUESTED_MANUAL_DC_CURRENT);
                            break;

                        case MCI_MANUAL_INJECTION_AC_CURRENT:
                            Mcl_IO.Manual_Injection_Param = param_x32;
                            Mcl_IO.Manual_Injection_Param_Rate = MATHCALC__ABS(param_rate_x32);
                            Mcl__SetDigital(MCL_WRITE_INJECTION_AC_CURRENT);
                            MCI_SET_FLOW_CTL(MCI_REQUESTED_MANUAL_AC_CURRENT);
                            break;

                        default:
                            Mcl_IO.Manual_Injection_Level = 0;
                            Mcl_IO.Manual_Injection_Level_Rate = 0;
                            Mcl_IO.Manual_Injection_Param = 0;
                            Mcl_IO.Manual_Injection_Param_Rate = 0;
                            break;
                    }
                    response = MCI_CMD_ACCEPTED;
                }
            }
            else
            {
                response = MCI_CMD_STOP_REQUESTED;
            }
        }
        else
        {
            response = MCI_CMD_PARAM_NOT_LOADED;
        }
    }
    else
    {
		response = MCI_CMD_INDEX_OUT_RANGE;
    }
    API220_MANUAL_INJECTION_CALLBACK(motor, method, level_x32, level_rate_x32, param_x32, param_rate_x32, response);

    return response;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Stop the motor regardless its speed with a desired rate. This command
 * is always accepted.
 * @param rpm_per_s - ramp down in rpm/s that the motor shall deaccelerates
 * @param ramp_rpm_per_s equal to zero means coasting stopping.
 * @param ramp_rpm_per_s higher than  MAX_RPM_PER_SEC_DECEL means lowers on mode stopping.
 * @param ramp_rpm_per_s lower than  MAX_RPM_PER_SEC_DECEL (but not zero) means all devices off stopping.
 * @return MCI_CMD_ACCEPTED             0 - [T/F] - accepted - always accepted
 */
MCI_CMD_RESPONSE_TYPE Mci__Stop(MOTOR_ENUM_TYPE motor, sint32 ramp_rpm_per_s)
{
    if(motor == MOTOR0)
    {
        MCI_CLR_FLOW_CTL(MCI_REQUESTED_START);

        if(BIT_TEST(Mci_Status_Control, MCI_DI_PARAMETERS_LOADED))
        {
            if(!(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP)))
            {
                if( (Mci_Control_State == MCI_RUNNING)          ||
                    (Mci_Control_State == MCI_STARTING)         ||
                    (Mci_Control_State == MCI_CLASS_B_STARTING)  )
                {
                    ramp_rpm_per_s = MATHCALC__ABS(ramp_rpm_per_s);

                    if(ramp_rpm_per_s == 0)
                    {
                        MCL_SET_STOPPING_COAST_DOWN();
                    }
                    else if(ramp_rpm_per_s > Mci_Parameters->Max_Rpm_Per_Sec_Decel)
                    {
                        ramp_rpm_per_s = Mci_Parameters->Max_Rpm_Per_Sec_Decel;

                        MCL_SET_STOPPING_LOWERS_ON();
                    }
                    else
                    {
                        MCL_SET_STOPPING_REQUEST();
                    }

                    MCI_SET_FLOW_CTL(MCI_REQUESTED_STOP);

                    SpeedRefGen__SetNewSpeed(0, ramp_rpm_per_s);
                }
                else if(Mci_Control_State == MCI_MANUAL_CONTROL)
                {
                    Mcl__ResetDigital(MCL_WRITE_ALL_INJECTION_METHOD);
                    MCI_SET_FLOW_CTL(MCI_REQUESTED_STOP);
                }
            }
        }
        else
        {
            MotorSafetyMgr__ForceStop();
        }
    }

    API220_STOP_CALLBACK(motor, ramp_rpm_per_s, MCI_CMD_ACCEPTED);

    return(MCI_CMD_ACCEPTED);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Read any digital error from the MCI
 * @param input is index of the parameter. See the Mci.h file
 * @return TRUE/FALSE according to the type of the passed parameter. The default return is TRUE.
 */
uint8 Mci__GetError(MOTOR_ENUM_TYPE motor, MCI_ERROR_TYPE read_error)
{
    uint8 ret_val;

    ret_val = FALSE;

    if(motor == MOTOR0)
    {
        if(Mci_Control_State > MCI_INITIALIZING)
        {
            switch(read_error)
           {
                case MCI_ERROR_ANY_ERROR_FOUND:                         // Any error found?
                     ret_val = GET_TRUE_FALSE(Mci_Error_Holder);
                     break;

                case MCI_ERROR_DCBUS_OVER_VOLTAGE:                      // DC bus link over voltage detected
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_DCBUS_OVER_VOLTAGE));
                    break;

                case MCI_ERROR_DCBUS_UNDER_VOLTAGE:                     // DC bus link under voltage detected
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_DCBUS_UNDER_VOLTAGE));
                    break;

                case MCI_ERROR_INVERTER_OVER_TEMP:                      // Module over temperature
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_INVERTER_OVER_TEMP));
                    break;

                case MCI_ERROR_CLASS_A_MOTOR_OVER_TEMP:                 // Class-A thermal model temperature exceeded the limit
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_CLASS_A_MOTOR_OVER_TEMP));
                    break;

                case MCI_ERROR_LOCKED_ROTOR_AT_STARTUP:                 // Rotor is locked by class A detection
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_LOCKED_ROTOR_AT_STARTUP));
                    break;

                case MCI_ERROR_LOCKED_ROTOR_IN_RUNNING:                 // Rotor is locked by class B detection
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_LOCKED_ROTOR_IN_RUNNING));
                    break;

                case MCI_ERROR_PHASE_LOST:                              // Phsae lost is detected.
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_PHASE_LOST));
                    break;

                case MCI_ERROR_INT_DISABLED:                            // Interrupt is disabled
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_INT_DISABLED));
                    break;

                case MCI_ERROR_OVER_LOAD:                               // Motor is overloaded!
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_OVER_LOAD));
                    break;

                case MCI_ERROR_SW_OVER_CURRENT:                         // Motor over current detected by logic/software
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_SW_OVER_CURRENT));
                    break;

                case MCI_ERROR_HW_OVER_CURRENT:                         // Motor over current detected by peripheral/hardware
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_HW_OVER_CURRENT));
                    break;

                case MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED:                // Speed check timeout
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED));
                    break;

                case MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED:    // Plausibility check for Input Capture failed
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED));
                    break;

                case MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED:    // Plausibility check for Input Capture maximum speed change failed
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED));
                    break;

                case MCI_ERROR_INITIALIZATION_FAILED:                   // Initialization is failed
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_INITIALIZATION_FAILED));
                    break;

                case MCI_ERROR_CLASS_B_FORCE2STOP:                      // Class B was forced to stop
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_CLASS_B_FORCE2STOP));
                        break;

                case MCI_ERROR_CURRENT_SENSOR_FAILED:                   // Error found in at least one current sensor
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_CURRENT_SENSOR_FAILED));
                        break;

                case MCI_ERROR_VOLTAGE_SENSOR_FAILED:                   // Error found in voltage sensor
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_VOLTAGE_SENSOR_FAILED));
                        break;

                case MCI_ERROR_PWM_SHUTDOWN_FAILED:                		// Error found in the hardware shut down
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_PWM_SHUTDOWN_FAILED));
                        break;

                case MCI_ERROR_OBSERVER_FAILURE:                        // Observer failure - lost observability
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_OBSERVER_FAILURE));
                        break;

                case MCI_ERROR_NOT_USED:                                // MCI_ERROR_NOT_USED
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_NOT_USED));
                        break;

                case MCI_ERROR_SURGE_RELAY_OPEN:                        // Surge relay open error
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Error_Holder, MCI_ERROR_SURGE_RELAY_OPEN));
                    break;

                default:
                    ret_val = TRUE;
                    break;
            }
        }
    }


    return(ret_val);

}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the error structure
 * @param motor =  index to the motor. See the Mci.h file
 * @return ret_val = Mci error holder variable value.
 */
uint32 Mci__GetErrorList(MOTOR_ENUM_TYPE motor)
{
    uint32 ret_val;
    ret_val = 0;

    if(motor == MOTOR0)
    {
        if(Mci_Control_State > MCI_INITIALIZING)
        {
            ret_val = Mci_Error_Holder;
        }
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Read any digital data from the MCI
 * @param input is index of the parameter. See the Mci.h file
 * @return TRUE/FALSE according to the type of the passed parameter. The default return is FALSE.
 */
uint8 Mci__GetDigital(MOTOR_ENUM_TYPE motor, MCI_DI_TYPE read_data)
{
    uint8 ret_val;

    ret_val = FALSE;

    if(motor == MOTOR0)
    {
        if(Mci_Control_State > MCI_INITIALIZING)
        {
            switch(read_data)
            {
                case MCI_DI_OVER_TORQUE:            //!<  0 - [T/F] Over torque detected
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Status_Control, MCI_DI_OVER_TORQUE));
                    break;

                case MCI_DI_MOTOR_STOP:             //!<  1 - [T/F] Is motor stopped?
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Status_Control, MCI_DI_MOTOR_STOP));
                    break;

                case MCI_DI_REACHED_TARGET_SPEED:   //!<  2 - [T/F] Target speed == reference speed
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Status_Control, MCI_DI_REACHED_TARGET_SPEED));
                    break;

                case MCI_DI_IDLE:                   //!<  3 - [T/F] Motor idle, if the speed is zero, no error found.
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Status_Control, MCI_DI_IDLE));
                    break;

                case MCI_DI_MOTOR_RUNNING:          //!<  4 - [T/F] Motor is running
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Status_Control, MCI_DI_MOTOR_RUNNING));
                    break;

                case MCI_DI_PARAMETERS_LOADED:      //!<  6 - [T/F] Read parameter loaded status
                    ret_val = GET_TRUE_FALSE(BIT_TEST(Mci_Status_Control, MCI_DI_PARAMETERS_LOADED));
                    break;

                default:
                    //
                    ret_val = FALSE;                // Return a non expected valued, just in case.
                    break;
            }
        }
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the digital structure
 * @param motor =  index to the motor. See the Mci.h file
 * @return ret_val = Mci status control variable value.
 */
uint32 Mci__GetDigitalList(MOTOR_ENUM_TYPE motor)
{
    uint32 ret_val;
    ret_val = 0;

    if(motor == MOTOR0)
    {
        if(Mci_Control_State > MCI_INITIALIZING)
        {
            ret_val = Mci_Status_Control;
        }
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Read analog data from the MCI
 * @param read_data index to the parameter. See the Mci.h file
 * @return the parameter value. The default return is zero. Not implemented functions returns -1
 */
sint32 Mci__GetAnalog(MOTOR_ENUM_TYPE motor, MCI_AI_TYPE read_data)
{
    sint32 ret_val;

    ret_val = MIN_SINT32;

    if(motor == MOTOR0)
    {
        switch(read_data)
        {

#if defined MCL_GET_TARGET_SPEED_RPM_S16
            case MCI_AI_TARGET_SPEED_RPM_S16:       //!<  0 - Target rotor shaft speed in RPM - x65536 - (1<<16)
                ret_val = MCL_GET_TARGET_SPEED_RPM_S16();
                break;
#endif

#if defined MCL_GET_SPEED_REFERENCE_RPM_S16
            case MCI_AI_SPEED_REFERENCE_RPM_S16:    //!<  1 - Speed reference in RPM - x65536 - (1<<16)
                ret_val = MCL_GET_SPEED_REFERENCE_RPM_S16();
                break;
#endif

#if defined MCL_GET_ACTUAL_SPEED_S16
            case MCI_AI_ACTUAL_SPEED_S16:           //!<  2 - Estimated rotor shaft speed in RPM - x65536 - (1<<16)
                ret_val = MCL_GET_ACTUAL_SPEED_S16();
                break;
#endif

#if defined MCL_GET_SPEED_ERROR_RPM_S16
            case MCI_AI_SPEED_ERROR_RPM_S16:        //!<  3 - Difference between estimated and reference speeds in RPM - x65536 - (1<<16)
                ret_val = MCL_GET_SPEED_ERROR_RPM_S16();
                break;
#endif

#if defined MCL_GET_MEAN_SPEED_S16
            case MCI_AI_MEAN_SPEED_S16:             //!<  4 - Mean rotor speed calculation - x65536 - (1<<16)
                ret_val = MCL_GET_MEAN_SPEED_S16();
                break;
#endif

#if defined MCL_GET_RMS_MOTOR_CURRENT_S16
            case MCI_AI_RMS_MOTOR_CURRENT_S16:      //!<  5 - RMS Motor current - x65536 - (1<<16)
                ret_val = MCL_GET_RMS_MOTOR_CURRENT_S16();
                break;
#endif

#if defined MCL_GET_ACTIVE_POWER_S16
            case MCI_AI_ACTIVE_POWER_S16:           //!<  6 - Measured active power in Watts - x65536 - (1<<16)
                ret_val = MCL_GET_ACTIVE_POWER_S16();
                break;
#endif

#if defined MCL_GET_SHAFT_POWER_S16
            case MCI_AI_SHAFT_POWER_S16:            //!<  7 - Estimated shaft output power in Watts from estimated torque - x65536 - (1<<16)
                ret_val = MCL_GET_SHAFT_POWER_S16();
                break;
#endif

#if defined MCL_GET_ROTOR_TEMP_S16
            case MCI_AI_ROTOR_TEMP_S16:             //!<  8 - Motor temperature in Celsius - x65536 - (1<<16)
                ret_val = MCL_GET_ROTOR_TEMP_S16();
                break;
#endif

#if defined MCL_GET_STATOR_TEMP_S16
            case MCI_AI_STATOR_TEMP_S16:            //!< 9 - Stator temperature calculated by stator resistance - x65536 - (1<<16)
                ret_val = MCL_GET_STATOR_TEMP_S16();
                break;
#endif

#if defined MCL_GET_LOAD_TORQUE_S16
            case MCI_AI_LOAD_TORQUE_S16:            //!< 10 - Read the load torque in Nm - x65536 - (1<<16)
                ret_val = MCL_GET_LOAD_TORQUE_S16();
                break;
#endif

#if defined MCL_GET_SHAFT_POSITION_DEG_S16
            case MCI_AI_SHAFT_POSITION_DEG_S16:     //!< 11 - Estimated rotor position in deg - x65536 - (1<<16)
                ret_val = MCL_GET_SHAFT_POSITION_DEG_S16();
                break;
#endif

#if defined MCL_GET_BUS_CURRENT_S16
            case MCI_AI_BUS_CURRENT_S16:            //!< 12 - Bus current in Amps - x65536 - (1<<16)
                ret_val = MCL_GET_BUS_CURRENT_S16();
                break;
#endif

#if defined MCL_GET_BUS_VOLTAGE_S16
            case MCI_AI_BUS_VOLTAGE_S16:            //!< 13 - Bus voltage in Volts - x65536 - (1<<16)
                ret_val = MCL_GET_BUS_VOLTAGE_S16();
                break;
#endif

#if defined INVERTER_TEMP_GET_TEMP_S16
            case MCI_AI_INVERTER_TEMP_S16:          //!< 14 - Inverter temperature in Celsius - x65536 - (1<<16)
                ret_val = INVERTER_TEMP_GET_TEMP_S16();
                break;
#endif

#if defined INVERTER_TEMP_GET_TEMP_RAW_DATA
            case MCI_AI_INVERTER_TEMP_RAW_DATA:     //!< 15 - Inverter temperature in raw data [ADC counts]
                ret_val = INVERTER_TEMP_GET_TEMP_RAW_DATA();
                break;
#endif

#if defined MCL_GET_VOLTAGE_PHASE_A_S16
            case MCI_AI_VOLTAGE_PHASE_A_S16:        //!< 16 - Motor phase A voltage in Volts - x65536 - (1<<16)
                ret_val = MCL_GET_VOLTAGE_PHASE_A_S16();
                break;
#endif

#if defined MCL_GET_VOLTAGE_PHASE_B_S16
            case MCI_AI_VOLTAGE_PHASE_B_S16:        //!< 17 - Motor phase B voltage in Volts - x65536 - (1<<16)
                ret_val = MCL_GET_VOLTAGE_PHASE_B_S16();
                break;
#endif

#if defined MCL_GET_VOLTAGE_PHASE_C_S16
            case MCI_AI_VOLTAGE_PHASE_C_S16:        //!< 18 - Motor phase C voltage in Volts - x65536 - (1<<16)
                ret_val = MCL_GET_VOLTAGE_PHASE_C_S16();
                break;
#endif

#if defined MCL_GET_SPEED_LOOP_GAIN_TABLE_INDEX
            case MCI_AI_SPEED_LOOP_GAIN_TABLE_INDEX://!< 19 - Select index of speed loop gain table
                ret_val = MCL_GET_SPEED_LOOP_GAIN_TABLE_INDEX();
                break;
#endif

#if defined MCL_GET_CURR_PHASE_A_S16
            case MCI_AI_CURR_PHASE_A_S16:           //!< 20 - Phase current A in A - x65536 - (1<<16)
                ret_val = MCL_GET_CURR_PHASE_A_S16();
                break;
#endif

#if defined MCL_GET_CURR_PHASE_B_S16
            case MCI_AI_CURR_PHASE_B_S16:           //!< 21 - Phase current B in A - x65536 - (1<<16)
                ret_val = MCL_GET_CURR_PHASE_B_S16();
                break;
#endif

#if defined MCL_GET_CURR_PHASE_C_S16
            case MCI_AI_CURR_PHASE_C_S16:           //!< 22 - Phase current C in A - x65536 - (1<<16)
                ret_val = MCL_GET_CURR_PHASE_C_S16();
                break;
#endif

#if defined MCL_GET_STATOR_RESISTANCE_S16
            case MCI_AI_STATOR_RESISTANCE_S16:      //!< 23 - Stator resistance in Ohm - x65536 - (1<<16)
                ret_val = MCL_GET_STATOR_RESISTANCE_S16();
                break;
#endif

            case MCI_AI_INTERNAL_MCI_STATE:         //!< 24 - Returns the internal Mci state
                ret_val = Mci_Control_State;
                break;

            default:
                ret_val = MIN_SINT32;               //!< Return a non expected valued, just in case.
                break;
        };
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Read parameters from the MCI
 * @param read_data index to the parameter. See the Mci.h file
 * @return the parameter value. The default return is zero. Not implemented functions returns -1
 */
sint32 Mci__GetParams(MOTOR_ENUM_TYPE motor, MCI_GET_PARAMETERS_TYPE read_data)
{
    sint32 ret_val;

    ret_val = MIN_SINT32;

    if((motor == MOTOR0)&&(Mci_Parameters_Loaded == TRUE))
    {
        switch(read_data)
        {
            case MCI_PARAMS_MAX_SPEED_RPM:              //!< 0 - Return the maximum allowed speed
                ret_val = Mci_Parameters->Max_Speed_Rpm;
                break;

            case MCI_PARAMS_MIN_SPEED_RPM:              //!< 1 - Returns the minimum allowed speed
                ret_val = Mci_Parameters->Min_Speed_Rpm;
                break;

            case MCI_PARAMS_MAX_RPM_PER_SEC_ACCEL:      //!< 2 - Returns the maximum allowed acceleration
                ret_val = Mci_Parameters->Max_Rpm_Per_Sec_Accel;
                break;

            case MCI_PARAMS_MAX_RPM_PER_SEC_DECEL:      //!< 3 - Returns the maximum allowed deceleration
                ret_val = Mci_Parameters->Max_Rpm_Per_Sec_Decel;
                break;

            case MCI_PARAMS_MIN_RPM_PER_SEC_ACCEL:      //!< 4 - Returns the minimum allowed acceleration
                ret_val = Mci_Parameters->Min_Rpm_Per_Sec_Accel;
                break;

            case MCI_PARAMS_MIN_RPM_PER_SEC_DECEL:      //!< 5 - Returns the minimum allowed deceleration
                ret_val = Mci_Parameters->Min_Rpm_Per_Sec_Decel;
                break;

            default:
                ret_val = MIN_SINT32;                   //!< Return a non expected valued, just in case.
                break;
        };
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Clear the error holder
 * @param motor =  index to the motor. See the Mci.h file
 * @return TRUE = All errors are clear.
 * @return FALSE = mci error state.
 */
uint8 Mci__ClearError(MOTOR_ENUM_TYPE motor)
{
    uint32 ret_val;
    uint32 temp_error;

    ret_val = FALSE;

    if(motor == MOTOR0)
    {
        // Check if the internal flag is already clear
        // before reseting the mirror one, prevent glitch on the bits
        temp_error = Mci_Error;                     // Use stack variable to prevent timings/glitch
        Mci_Error_Holder &= temp_error;

        if(temp_error)
        {
            BIT_SET(Mci_Error_Holder, MCI_ERROR_ANY_ERROR_FOUND);
        }
        else
        {
            ret_val = TRUE;
        }
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Write a digital data to the MCI.
 * @param write_data is the index to the data. See the Mci.h file.
 * @param value TRUE/FALSE according to the selected data
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
uint8 Mci__SetDigital(MOTOR_ENUM_TYPE motor, MCI_DO_TYPE write_data, uint8 value)
{
    uint8 ret_val;

    ret_val = FALSE;

    if(motor == MOTOR0)
    {
        switch(write_data)
        {

#if defined MCL_VA_WASH_STATE
            case MCI_DO_VA_WASH_STATE:   //!< 3 - [T/F] - TRUE = Agi / FALSE = Spin
                ret_val = MCL_VA_WASH_STATE(value);
                break;
#endif

            default:
                ret_val = FALSE;
                break;
        }
    }

    API220_PUBLISHDIGITAL_CALLBACK(motor, write_data, value, ret_val);

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Write an analog data to the MCI.
 * @param write_data is the index to the data. See Mci.h file
 * @param value is the value to the data
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
uint8 Mci__SetAnalog(MOTOR_ENUM_TYPE motor, MCI_AO_TYPE write_data, sint32 value)
{
    uint8 ret_val = FALSE;

    if(motor == MOTOR0)
    {
        switch(write_data)
        {

#if defined MCL_SET_SPEED_LOOP_PROP_GAIN_S16
            case MCI_AO_SPEED_LOOP_PROP_GAIN_S16:   //!<  0 - Proportional speed loop gain - x128 (1<<7)
                ret_val = MCL_SET_SPEED_LOOP_PROP_GAIN_S16(value);
                break;
#endif

#if defined MCL_SET_SPEED_LOOP_INT_GAIN_S16
            case MCI_AO_SPEED_LOOP_INT_GAIN_S16:    //!<  1 - Integral speed loop gain - x128 (1<<7)
                ret_val = MCL_SET_SPEED_LOOP_INT_GAIN_S16(value);
                break;
#endif

#if defined MCL_SET_SPEED_LOOP_GAIN_TABLE_INDEX
            case MCI_AO_SPEED_LOOP_GAIN_TABLE_INDEX://!<  2 - Select index of speed loop gain table
                ret_val = MCL_SET_SPEED_LOOP_GAIN_TABLE_INDEX(value);
                break;
#endif

#if defined MCL_SET_SPEED_FIR_TAPS_TABLE_INDEX
            case MCI_AO_SPEED_FIR_TAPS_TABLE_INDEX: //!<  3 - Select index of speed fir taps
                ret_val = MCL_SET_SPEED_FIR_TAPS_TABLE_INDEX(value);
                break;
#endif

#if defined MCL_SET_SOFTSTART_COUNTER_INDEX
            case MCI_AO_SOFTSTART_COUNTER_INDEX:    //!<  4 - Select index of soft start counter
                ret_val = MCL_SET_SOFTSTART_COUNTER_INDEX(value);
                break;
#endif

#if defined MCL_SET_SPEED_LIMIT_INDEX
            case MCI_AO_SPEED_LIMIT_INDEX:          //!<  5 - Select index of speed controller limit
                ret_val = MCL_SET_SPEED_LIMIT_INDEX(value);
                break;
#endif

#if defined MCL_SET_ADD_DELTA_SPEED_REF_S16
            case MCI_AO_ADD_DELTA_SPEED_REF_S16:    //!<  7 - Add (force) a delta speed on the speed reference - x65536 (1<<16)
                ret_val = MCL_SET_ADD_DELTA_SPEED_REF_S16(value);
                break;
#endif


            default:
                ret_val = FALSE;
                break;
        }
    }

//    API220_PUBLISHANALOG_CALLBACK(motor, write_data, value, ret_val);

    return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Write the addresses of Mcl parameters.
 *  @details
 *
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */

uint8 Mci__SetParams(void*  params_address)
{
    uint8 ret_val;

    Mci_Parameters = (MCI_PARAMS_TYPE*)(*((addr_t*)params_address + MCI_PARAMETERS_OFFSET));
    Mci_Parameters_Loaded = TRUE;

#if defined MCL_SET_MCL_PARAMETERS_ADDRESS
    ret_val = MCL_SET_MCL_PARAMETERS_ADDRESS(params_address);
#else
    ret_val = FALSE;
#endif
    return(ret_val);
}




#if MCI_FVT_TESTS == ENABLED

//---------------------------------------------------------------------------------------------------------------------
/**
 * Start FVT tests
 * @param  motor - directs the test to a specific motor or inverter
 * @return MCI_CMD_ACCEPTED             0 - [T/F] - accepted - always accepted
 */
MCI_CMD_RESPONSE_TYPE Mci__FvtStart(MOTOR_ENUM_TYPE motor)
{
    MCI_CMD_RESPONSE_TYPE response;

    response = MCI_CMD_DENIED;

    if(motor == MOTOR0)
    {
        if(BIT_TEST(Mci_Status_Control, MCI_DI_PARAMETERS_LOADED))
        {
            if(!(BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP)))
            {
                if(Mci_Control_State == MCI_IDLE)
                {
                    response = MCI_CMD_ACCEPTED;

                    MCI_FVT_ENTRY();
                    MCI_SET_FLOW_CTL(MCI_REQUESTED_FVT_MODE);
                }
            }
            else
            {
                response = MCI_CMD_STOP_REQUESTED;
            }
        }
        else
        {
            response = MCI_CMD_PARAM_NOT_LOADED;
        }
    }
    else
    {
        response = MCI_CMD_INDEX_OUT_RANGE;
    }

//    API220_MANUAL_INJECTION_CALLBACK(motor, method, level_x32, level_rate_x32, param_x32, param_rate_x32, response);

    return(response);
}










//---------------------------------------------------------------------------------------------------------------------
/**
 * Stop the FVT tests
 * @param  motor - directs the test to a specific motor or inverter
 * @return MCI_CMD_ACCEPTED             0 - [T/F] - accepted - always accepted
 */
MCI_CMD_RESPONSE_TYPE Mci__FvtStop(MOTOR_ENUM_TYPE motor)
{
    if(motor == MOTOR0)
    {
        if(BIT_TEST(Mci_Status_Control, MCI_DI_PARAMETERS_LOADED))
        {
            if( (Mci_Control_State == MCI_FVT_MODE)         ||
                (Mci_Control_State == MCI_STARTING)         ||
                (Mci_Control_State == MCI_CLASS_B_STARTING)  )
            {
                MCI_CLR_FLOW_CTL(MCI_REQUESTED_START);
                MCI_CLR_FLOW_CTL(MCI_REQUESTED_FVT_MODE);
                MCI_SET_FLOW_CTL(MCI_REQUESTED_STOP);
            }
        }
        else
        {
            MotorSafetyMgr__ForceStop();
        }
    }

//    API220_STOP_CALLBACK(motor, ramp_rpm_per_s, MCI_CMD_ACCEPTED);

    return(MCI_CMD_ACCEPTED);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Get FVT tests status
 * @param  motor - directs the test to a specific motor or inverter
 * @param  MCI_FVT_STATUS_TYPE             0 - [T/F] - MCI_FVT_STATUS_STARTED
 * @param  MCI_FVT_STATUS_TYPE             1 - [T/F] - MCI_FVT_STATUS_FINISHED
 * @return status                          [T/F] - refer to the requested status
 */
uint8 Mci__FvtGetStatus(MOTOR_ENUM_TYPE motor, MCI_FVT_STATUS_TYPE this_status)
{
    uint8 ret_val;

    ret_val = FALSE;

    if(motor == MOTOR0)
    {
        if(this_status < MCI_FVT_STATUS_TOTAL)
        {
            ret_val = MCI_FVT_GET_STATUS(this_status);
        }
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Get FVT results
 * @param  motor - request result of the test to a specific motor or inverter
 * @param  MCI_FVT_TEST_RESULT_TYPE - *fvt_data_result pointer to the data
 */
void Mci__FvtGetResult(MOTOR_ENUM_TYPE motor, MCI_FVT_TEST_RESULT_TYPE *fvt_data_result)
{
    if(motor == MOTOR0)
    {
        MCI_FVT_GET_RESULT(fvt_data_result);
    }
}
#endif


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Updates the pwm output according to the required pattern.
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Drive_The_Motor(void)
{
    sint32 dutyA;
    sint32 dutyB;
    sint32 dutyC;

    // LowersOn should have higher priority than All Devices Off
    if(BIT_TEST(Mci_Flow_Control, MCI_FORCE_LOWERS_ON))
    {
        MotorSafetyMgr__ConfigPwmPattern(OUTPUT_PATTERN_LOWERS_ON);
    }
    else if(BIT_TEST(Mci_Flow_Control, MCI_ALL_DEVICES_OFF))
    {
        MotorSafetyMgr__ConfigPwmPattern(OUTPUT_PATTERN_ALL_DEV_OFF);
    }
    else
    {
        // Check if there is any lower on or all device off commands
        if(Mcl_IO.Lowers_On + Mcl_IO.All_Devices_Off)
        {
            if(Mcl_IO.Lowers_On)
            {
                MotorSafetyMgr__ConfigPwmPattern(OUTPUT_PATTERN_LOWERS_ON);
            }
            else if(Mcl_IO.All_Devices_Off)
            {
                MotorSafetyMgr__ConfigPwmPattern(OUTPUT_PATTERN_ALL_DEV_OFF);
            }
        }
        else
        {
            dutyA = MCL_DUTY(Mcl_IO.Duty.A);
            dutyB = MCL_DUTY(Mcl_IO.Duty.B);
            dutyC = MCL_DUTY(Mcl_IO.Duty.C);
            MotorSafetyMgr__UpdatePwm(dutyA, dutyB, dutyC);
        }
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Check for all error used in the control. Hardware and peripheral may
 *          support to decrease the error polling rate.
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Check_Errors_Pwm_Handler(void)
{
    MCL_NOTATION_TYPE ia_abs;
    MCL_NOTATION_TYPE ib_abs;
    MCL_NOTATION_TYPE ic_abs;

    if(Mci_Control_State >= MCI_STARTING)
    {
        ia_abs = MCL__ABS(Mcl_IO.Is_ABC.A);
        ib_abs = MCL__ABS(Mcl_IO.Is_ABC.B);
        ic_abs = MCL__ABS(Mcl_IO.Is_ABC.C);

        if( (ia_abs >= MCI_MAXIMAL_MOTOR_CURRENT_ALLOWED)   ||
            (ib_abs >= MCI_MAXIMAL_MOTOR_CURRENT_ALLOWED)   ||
            (ic_abs >= MCI_MAXIMAL_MOTOR_CURRENT_ALLOWED) )
        {
            MCI_SET_ERROR(MCI_ERROR_SW_OVER_CURRENT);
        }
        else
        {
            MCI_CLR_ERROR(MCI_ERROR_SW_OVER_CURRENT);
        }
    }
    else
    {
        MCI_CLR_ERROR(MCI_ERROR_SW_OVER_CURRENT);
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Check for all error used in the control. Hardware and peripheral may
 *          support to decrease the error polling rate.
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Check_Errors_5ms_Handler(void)
{
	uint8 any_class_B_error_found;
    uint8 temp_u8;
    static uint8 time_postscaler = 0;

    //////////////////////////////////////////////////////////////////////////////////////////
    //// DC Bus voltage monitor
    if(!SURGE_RELAY_READY())
    {
        MCI_SET_ERROR(MCI_ERROR_SURGE_RELAY_OPEN);
    }
    else
    {
        MCI_CLR_ERROR(MCI_ERROR_SURGE_RELAY_OPEN);
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    // DC Bus (over) current monitor
    temp_u8 = MotorSafetyMgr__GetFault(SR_FAULT_OVER_CURRENT);

    if(temp_u8)
    {
        MCI_SET_ERROR(MCI_ERROR_HW_OVER_CURRENT);
    }
    else
    {
        MCI_CLR_ERROR(MCI_ERROR_HW_OVER_CURRENT);
    }

    // Time post scaler, reduce from 5 ms to MCI_CHECKERR_POSTSCALER
    if(time_postscaler)
    {
        time_postscaler--;
    }
    else
    {
        time_postscaler = MCI_CHECKERR_POSTSCALER;

        // Any error found
        any_class_B_error_found = MotorSafetyMgr__AnyFaultFound();
        if(any_class_B_error_found)
        {
            // Look for electronics failure, OR wise operation across the flags below
            temp_u8  = MotorSafetyMgr__GetFault(SR_FAULT_CURRENT_SENSOR_FAILED);
            if(temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_CURRENT_SENSOR_FAILED);
            }

            temp_u8 = MotorSafetyMgr__GetFault(SR_FAULT_VOLTAGE_SENSOR_FAILED);
            if(temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_VOLTAGE_SENSOR_FAILED);
            }

            temp_u8 = MotorSafetyMgr__GetFault(SR_FAULT_PWM_SHUTDOWN_FAILED);
            if(temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_PWM_SHUTDOWN_FAILED);
            }


            // Look for phase lost detection
            temp_u8  = MotorSafetyMgr__GetFault(SR_FAULT_PHASE_LOST);
            if(temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_PHASE_LOST);
            }

            //////////////////////////////////////////////////////////////////////////////////////////
            //// Class B check
            // Voltage is under the range from Class B
            temp_u8  = MotorSafetyMgr__GetFault(SR_FAULT_DCBUS_UNDER_VOLTAGE);
            if(temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_DCBUS_UNDER_VOLTAGE);
            }


            // Voltage is over the range from Class B
            temp_u8  = MotorSafetyMgr__GetFault(SR_FAULT_DCBUS_OVER_VOLTAGE);
            if(temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_DCBUS_OVER_VOLTAGE);
            }


            //// Check unlock door speed failed
            temp_u8 = MotorSafetyMgr__GetFault(SR_FAULT_UNLOCK_DOOR_SPEED_FAILED);
            if(temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED);
            }


            //// Check input capture plausibility faults
            #if defined MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED
                // Min Tach Fault
                temp_u8 = MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED();
                if(temp_u8)
                {
                	MCI_SET_ERROR(MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED);
                }
            #endif

            #if defined MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED
                // Max Speed change Fault
                temp_u8 = MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED();
                if(temp_u8)
                {
                	MCI_SET_ERROR(MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED);
                }
            #endif

            // Check for motor over heating
            temp_u8 = MotorSafetyMgr__GetFault(SR_FAULT_OVER_LOAD);
            if(temp_u8)
            {
               MCI_SET_ERROR(MCI_ERROR_OVER_LOAD);
            }
        }
        else
        {
            MCI_CLR_ERROR(MCI_ERROR_CURRENT_SENSOR_FAILED);
            MCI_CLR_ERROR(MCI_ERROR_VOLTAGE_SENSOR_FAILED);
            MCI_CLR_ERROR(MCI_ERROR_PWM_SHUTDOWN_FAILED);
            MCI_CLR_ERROR(MCI_ERROR_DCBUS_OVER_VOLTAGE);
            MCI_CLR_ERROR(MCI_ERROR_DCBUS_UNDER_VOLTAGE);
            MCI_CLR_ERROR(MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED);

            //// Check input capture plausibility faults
            #if defined MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED
                MCI_CLR_ERROR(MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED);
            #endif

            #if defined MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED
                MCI_CLR_ERROR(MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED);
            #endif

            MCI_CLR_ERROR(MCI_ERROR_OVER_LOAD);

            // Look for phase lost detection
#if defined MCL_GET_MOTOR_DISCONNECTED
            // Motor disconnection check
            temp_u8 = MCL_GET_MOTOR_DISCONNECTED();
#endif
            if(temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_PHASE_LOST);
            }
            else
            {
                MCI_CLR_ERROR(MCI_ERROR_PHASE_LOST);
            }
        }


        //////////////////////////////////////////
        ////// MCI_RUNNING
        if(Mci_Control_State == MCI_RUNNING)
        {
            temp_u8 = MotorSafetyMgr__DoesClassAHaveAccessToPwm();
            if(temp_u8 == FALSE)
            {
            	if(!any_class_B_error_found)
            	{
            		MCI_SET_ERROR(MCI_ERROR_CLASS_B_FORCE2STOP);
            	}
            }
        }
        //////////////////////////////////////////
        ////// MCI_STARTING
        else if(Mci_Control_State == MCI_STARTING)
        {
            temp_u8 = MotorSafetyMgr__DoesClassAHaveAccessToPwm();
            if(temp_u8 == FALSE)
            {
            	if(!any_class_B_error_found)
            	{
            		MCI_SET_ERROR(MCI_ERROR_CLASS_B_FORCE2STOP);
            	}
            }
        }
        //////////////////////////////////////////
        ////// MCI_INITIALIZING
        else if(Mci_Control_State == MCI_INITIALIZING)
        {
            if(Mci_GP_Timer < MCI_INIT_ERROR_THRES)
            {
                Mci_GP_Timer++;
            }
            else
            {
                MCI_SET_ERROR(MCI_ERROR_INITIALIZATION_FAILED);
            }
        }
        //////////////////////////////////////////
        ////// MCI_FAILED
        else if(Mci_Control_State == MCI_FAILED)
        {
            MCI_CLR_ERROR(MCI_ERROR_INITIALIZATION_FAILED);
            MCI_CLR_ERROR(MCI_ERROR_CLASS_B_FORCE2STOP);
        }
        //////////////////////////////////////////
        ////// MCI_CLASS_B_STARTING
        else if(Mci_Control_State == MCI_CLASS_B_STARTING)
        {
            if(Mci_GP_Timer < MCI_CLASS_B_STARTUP_THRES)
            {
                Mci_GP_Timer++;
            }
            else
            {
            	MCI_SET_ERROR(MCI_ERROR_CLASS_B_FORCE2STOP);
            }
        }

        // Check if pwm is accessible by class A, then
        if(Mci_Control_State >= MCI_STARTING)
        {
            temp_u8 = BIT_TEST(Mci_Status_Control, MCI_DI_PARAMETERS_LOADED);
            if(!temp_u8)
            {
                MCI_SET_ERROR(MCI_ERROR_INITIALIZATION_FAILED);
            }
        }

#if defined MCI_MEASURE_INVERTER_TEMP

        if(InverterTemp__AboveThreshold_s16((MODULE_TEMP_TO_STOP<<16)) == TRUE)
        {
            MCI_SET_ERROR(MCI_ERROR_INVERTER_OVER_TEMP);
        }
        if(InverterTemp__AboveThreshold_s16((MODULE_TEMP_TO_START<<16)) == FALSE)
        {
            MCI_CLR_ERROR(MCI_ERROR_INVERTER_OVER_TEMP);
        }

#endif


#if defined MCL_GET_LOCKED_ROTOR_AT_STARTUP

       // Locked rotor by algorithm - startup phase
       temp_u8 = MCL_GET_LOCKED_ROTOR_AT_STARTUP();
       if(temp_u8)
       {
           MCI_SET_ERROR(MCI_ERROR_LOCKED_ROTOR_AT_STARTUP);
       }
       else
       {
           MCI_CLR_ERROR(MCI_ERROR_LOCKED_ROTOR_AT_STARTUP);
       }

#endif


#if defined MCL_GET_LOCKED_ROTOR_IN_RUNNING

       // Locked rotor by algorithm - motor running
       temp_u8 = MCL_GET_LOCKED_ROTOR_IN_RUNNING();
       if(temp_u8)
       {
           MCI_SET_ERROR(MCI_ERROR_LOCKED_ROTOR_IN_RUNNING);
       }
       else
       {
           MCI_CLR_ERROR(MCI_ERROR_LOCKED_ROTOR_IN_RUNNING);
       }

#endif


#if defined MCL_GET_OBSERVER_FAILURE

       // Observer failure found
       temp_u8 = MCL_GET_OBSERVER_FAILURE();
       if(temp_u8)
       {
           MCI_SET_ERROR(MCI_ERROR_OBSERVER_FAILURE);
       }
       else
       {
           MCI_CLR_ERROR(MCI_ERROR_OBSERVER_FAILURE);
       }

#endif


#if defined MCL_CLASS_A_MOTOR_OVER_TEMP

       // Motor over temperature
       temp_u8 = MCL_CLASS_A_MOTOR_OVER_TEMP();
       if(temp_u8)
       {
           MCI_SET_ERROR(MCI_ERROR_CLASS_A_MOTOR_OVER_TEMP);
       }
       else
       {
           MCI_CLR_ERROR(MCI_ERROR_CLASS_A_MOTOR_OVER_TEMP);
       }

#endif


#if defined MCL_GET_INT_DISABLED

       // Motor over temperature
       temp_u8 = MCL_GET_INT_DISABLED();
       if(temp_u8)
       {
           MCI_SET_ERROR(MCI_ERROR_INT_DISABLED);
       }
       else
       {
           MCI_CLR_ERROR(MCI_ERROR_INT_DISABLED);
       }

#endif
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Check for all flow control that the algorithm uses.
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Update_Status(void)
{
    uint8 temp_u8;

    //////////////////////////////////////////////////////////////////////////////////////////
    //// Check the flag MCI_IS_MOTOR_STOPPED & MCI_MOTOR_IDLE
    // If the state is in IDLE
    if(Mci_Control_State == MCI_IDLE)
    {
        // Motor state is idle
        MCI_SET_STATUS_CTL(MCI_DI_IDLE);

        // Motor state is in IDLE, therefore the motor shall not be being drive anymore
        MCI_CLR_STATUS_CTL(MCI_DI_MOTOR_RUNNING);

        MCI_SET_STATUS_CTL(MCI_DI_MOTOR_STOP);

        MCI_CLR_STATUS_CTL(MCI_DI_REACHED_TARGET_SPEED);
    }
    else // otherwise
    {
        // the motor is not stopped
        MCI_CLR_STATUS_CTL(MCI_DI_MOTOR_STOP);

        // Motor state not idle
        MCI_CLR_STATUS_CTL(MCI_DI_IDLE);

        // If the main state is running
        if(Mci_Control_State == MCI_RUNNING)
        {
            // Set the motor running flag
            MCI_SET_STATUS_CTL(MCI_DI_MOTOR_RUNNING);

            if(SpeedRefGen__GetStatus() == SPEED_REF_AT_PLATEAU)
            {
                MCI_SET_STATUS_CTL(MCI_DI_REACHED_TARGET_SPEED);
            }
            else
            {
                MCI_CLR_STATUS_CTL(MCI_DI_REACHED_TARGET_SPEED);
            }

#if defined MCL_GET_OVER_TORQUE

            temp_u8 = MCL_GET_OVER_TORQUE();
            if(temp_u8)
            {
                MCI_SET_STATUS_CTL(MCI_DI_OVER_TORQUE);
            }
            else
            {
                MCI_CLR_STATUS_CTL(MCI_DI_OVER_TORQUE);
            }

#else

            MCI_CLR_STATUS_CTL(MCI_DI_OVER_TORQUE);

#endif
        }
        else
        {
                MCI_CLR_STATUS_CTL(MCI_DI_REACHED_TARGET_SPEED);
        }
    }


    // Check if parameters are loaded
    temp_u8  = MotorSafetyMgr__GetFlag(SR_FLAG_MC_PARAMETERS_LOADED);
    temp_u8 += MCL_GET_PARAMETERS_LOADED();
    temp_u8 += (uint8)Mci_Parameters_Loaded;
#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
    temp_u8 += (uint8) InverterTemp__GetParameterLoaded();
#endif
#if (POWERMODULE_MANAGER_USED == ENABLED)
    temp_u8 += (uint8) PowerModuleManager__GetParameterLoaded();
#endif
    if(temp_u8 == MCI_PARAMS_LOADED_CHECKS)
    {
        MCI_SET_STATUS_CTL(MCI_DI_PARAMETERS_LOADED);
    }
    else
    {
        MCI_CLR_STATUS_CTL(MCI_DI_PARAMETERS_LOADED);
    }


}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize the analog interface variables
 */

#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Initialize_Current_DCBus_Voltage(void)
{
    Mcl_IO.Is_ABC.A = (MCL_NOTATION_TYPE)0;
    Mcl_IO.Is_ABC.B = (MCL_NOTATION_TYPE)0;
    Mcl_IO.Is_ABC.C = (MCL_NOTATION_TYPE)0;
    Mcl_IO.Vdc      = (MCL_NOTATION_TYPE)0;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Convert data from peripherals into requested to the motor control
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Read_Current_DCBus_Voltage(sint32 ia_adc, sint32 ib_adc, sint32 ic_adc, sint32 dcbus_adc)
{
#ifndef MCI_CURRENT_SENSOR_GAIN_POSITIVE

    // Mcl interface Update
    Mcl_IO.Is_ABC.A = MCL_CURRENT(((-ia_adc) << 4));
    Mcl_IO.Is_ABC.B = MCL_CURRENT(((-ib_adc) << 4));
    Mcl_IO.Is_ABC.C = MCL_CURRENT(((-ic_adc) << 4));

#else

    // Mcl interface Update
    Mcl_IO.Is_ABC.A = MCL_CURRENT(((ia_adc) << 4));
    Mcl_IO.Is_ABC.B = MCL_CURRENT(((ib_adc) << 4));
    Mcl_IO.Is_ABC.C = MCL_CURRENT(((ic_adc) << 4));

#endif

    Mcl_IO.Vdc = MCL_VOLTAGE((dcbus_adc << 3));
}



#if(API220_DEBUG_MODE == ENABLED)
/*
 * @brief   Force error to emulate mci reaction. Used for debugging purposes
 *          of high level application, like cycle manager and other modules.
 * @param   emulate_this_error [MCI_ERROR_TYPE] - error index.
 */
void Mci__Emulate_Error(uint8 emulate_this_error)
{

    switch(emulate_this_error)
    {
        case MCI_ERROR_INITIALIZATION_FAILED:
            MCI_SET_ERROR(MCI_ERROR_INITIALIZATION_FAILED);
            break;

        case MCI_ERROR_DCBUS_OVER_VOLTAGE:
            MCI_SET_ERROR(MCI_ERROR_DCBUS_OVER_VOLTAGE);
            break;

        case MCI_ERROR_DCBUS_UNDER_VOLTAGE:
            MCI_SET_ERROR(MCI_ERROR_DCBUS_UNDER_VOLTAGE);
            break;

#if defined MCI_MEASURE_INVERTER_TEMP

        case MCI_ERROR_INVERTER_OVER_TEMP:
            MCI_SET_ERROR(MCI_ERROR_INVERTER_OVER_TEMP);
            break;

#endif

#if defined MCL_GET_LOCKED_ROTOR_AT_STARTUP

        case MCI_ERROR_LOCKED_ROTOR_AT_STARTUP:
            MCI_SET_ERROR(MCI_ERROR_LOCKED_ROTOR_AT_STARTUP);
            break;

#endif

#if defined MCL_GET_LOCKED_ROTOR_IN_RUNNING

        case MCI_ERROR_LOCKED_ROTOR_IN_RUNNING:
            MCI_SET_ERROR(MCI_ERROR_LOCKED_ROTOR_IN_RUNNING);
            break;

#endif

        case MCI_ERROR_OVER_LOAD:
            MCI_SET_ERROR(MCI_ERROR_OVER_LOAD);
            break;

        case MCI_ERROR_CURRENT_SENSOR_FAILED:
            MCI_SET_ERROR(MCI_ERROR_CURRENT_SENSOR_FAILED);
            break;

        case MCI_ERROR_VOLTAGE_SENSOR_FAILED:
            MCI_SET_ERROR(MCI_ERROR_VOLTAGE_SENSOR_FAILED);
            break;

        case MCI_ERROR_SW_OVER_CURRENT:
            MCI_SET_ERROR(MCI_ERROR_SW_OVER_CURRENT);
            break;

        case MCI_ERROR_HW_OVER_CURRENT:
            MCI_SET_ERROR(MCI_ERROR_HW_OVER_CURRENT);
            break;

        case MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED:
            MCI_SET_ERROR(MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED);
            break;

        case MCI_ERROR_PHASE_LOST:
            MCI_SET_ERROR(MCI_ERROR_PHASE_LOST);
            break;

#if defined MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED

        case MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED:
            MCI_SET_ERROR(MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED);
            break;

#endif

#if defined MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED

        case MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED:
            MCI_SET_ERROR(MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED);
            break;

#endif

#if defined MCL_GET_INT_DISABLED

        case MCI_ERROR_INT_DISABLED:
            MCI_SET_ERROR(MCI_ERROR_INT_DISABLED);
            break;

#endif

#if defined MCL_GET_STATOR_OVER_TEMP

        case MCI_ERROR_CLASS_A_MOTOR_OVER_TEMP:
            MCI_SET_ERROR(MCI_ERROR_CLASS_A_MOTOR_OVER_TEMP);
            break;

#endif

        case MCI_ERROR_CLASS_B_FORCE2STOP:
            MCI_SET_ERROR(MCI_ERROR_CLASS_B_FORCE2STOP);
            break;

        case MCI_ERROR_PWM_SHUTDOWN_FAILED:
            MCI_SET_ERROR(MCI_ERROR_PWM_SHUTDOWN_FAILED);
            break;

#if defined MCL_GET_OBSERVER_FAILURE

        case MCI_ERROR_OBSERVER_FAILURE:
            MCI_SET_ERROR(MCI_ERROR_OBSERVER_FAILURE);
            break;

#endif

        case MCI_ERROR_DCBUS_OUT_OF_RANGE:
            MCI_SET_ERROR(MCI_ERROR_DCBUS_OUT_OF_RANGE);
            break;

        case MCI_ERROR_SURGE_RELAY_OPEN:
        	MCI_SET_ERROR(MCI_ERROR_SURGE_RELAY_OPEN);
        	break;

        default:
            //
            break;
    }


}
#endif

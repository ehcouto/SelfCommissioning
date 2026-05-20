/**
 *  @file       Mci.h
 *  @brief      Motor control API
 *  @details    Define the basic commands to interface the application with the motor control
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2017.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef MCI_H_
    #define MCI_H_

#include "Mci_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Define the main states of this controller
typedef enum
{
    MCI_NOT_DEFINED = 0,                            //!< 0  - reserved 0 index - shall not be used
    MCI_FVT_MODE,                                   //!< 1  - Function Verification Test (FVT) Mode.
    MCI_INITIALIZING,                               //!< 2  - initializing state
    MCI_IDLE,                                       //!< 3  - idling
    MCI_CLASS_B_STARTING,                           //!< 4  - hardware starting, checking and power up
    MCI_CLASS_B_STOPPING,                           //!< 5  - stopping Class B, executes before idling
    MCI_FAILED,                                     //!< 6  - something has failed
    MCI_STARTING,                                   //!< 7  - starting the motor, executes before running
    MCI_RUNNING,                                    //!< 8  - running
    MCI_STOPPING,                                   //!< 9  - stopping the motor, executes before Class B stopping
    MCI_DEBUGGING,                                  //!< 10 - debugging, shall not be used for production
    MCI_MANUAL_CONTROL,                             //!< 11 - manual control of inverter bridge, used for AC or DC, voltage or current control
} MCI_CONTROL_STATE_TYPE;



//! Motor indexer
typedef enum
{
    MOTOR0 = 0,                                     //!< 0 - Motor index 0
    MOTOR1,                                         //!< 1 - Motor index 1
    MOTOR2,                                         //!< 2 - Motor index 2
    MOTOR3,                                         //!< 3 - Motor index 3
    MOTOR4                                          //!< 4 - Motor index 4
} MOTOR_ENUM_TYPE;


//! Digital inputs definition to application
typedef enum
{
    MCI_DI_OVER_TORQUE = 0,                         //!<  0 - [T/F] Over torque detected
    MCI_DI_MOTOR_STOP,                              //!<  1 - [T/F] Motor stop
    MCI_DI_REACHED_TARGET_SPEED,                    //!<  2 - [T/F] Target speed == reference speed
    MCI_DI_IDLE,                                    //!<  3 - [T/F] Motor idle, if the speed is zero, no error found.

    MCI_DI_MOTOR_RUNNING,                           //!<  4 - [T/F] Motor running
    MCI_DI_PARAMETERS_LOADED,                       //!<  6 - [T/F] Class A and B - SR_FLAG_MC_PARAMETERS_LOADED

    MCI_DI_NR_OF_DI                                 //!< Total declared of digital inputs
} MCI_DI_TYPE;




//! Error definition to application
typedef enum
{
    MCI_ERROR_ANY_ERROR_FOUND = 0,                  //!<  0 - [T/F] Any error found?
    MCI_ERROR_INITIALIZATION_FAILED,                //!<  1 - [T/F] Mci Initialization phase failed
    MCI_ERROR_DCBUS_OVER_VOLTAGE,                   //!<  2 - [T/F] DC bus link over voltage detected
    MCI_ERROR_DCBUS_UNDER_VOLTAGE,                  //!<  3 - [T/F] DC bus link under voltage detected

    MCI_ERROR_INVERTER_OVER_TEMP,                   //!<  4 - [T/F] Inverter module over temperature
	MCI_ERROR_LOCKED_ROTOR_AT_STARTUP,      	    //!<  5 - [T/F] Locked rotor detected at startup
	MCI_ERROR_LOCKED_ROTOR_IN_RUNNING,              //!<  6 - [T/F] Locked rotor detected in running
	MCI_ERROR_OVER_LOAD,                            //!<  7 - [T/F] Motor overload

    MCI_ERROR_CURRENT_SENSOR_FAILED,                //!<  8 - [T/F] From Class B - SR_FAULT_CURRENT_SENSOR_FAILED
    MCI_ERROR_VOLTAGE_SENSOR_FAILED,                //!<  9 - [T/F] From Class B - SR_FAULT_VOLTAGE_SENSOR_FAILED
    MCI_ERROR_SW_OVER_CURRENT,                      //!< 10 - [T/F] Motor over current detected by logic/software
    MCI_ERROR_HW_OVER_CURRENT,                      //!< 11 - [T/F] Motor over current detected by peripheral/hardware

    MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED,             //!< 12 - [T/F] From Class B - Safety Speed check for door unlock failed
    MCI_ERROR_PHASE_LOST,                           //!< 13 - [T/F] From Class B - Phase lost detected
    MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED, //!< 14 - [T/F] Safety Input Capture plausibility check failed
    MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED, //!< 15 - [T/F] Safety Input Capture plausibility check for maximum speed change failed

    MCI_ERROR_INT_DISABLED,                         //!< 16 - [T/F] Interrupt was disabled
    MCI_ERROR_CLASS_A_MOTOR_OVER_TEMP,              //!< 17 - [T/F] Class-A thermal model temperature exceeded the limit
    MCI_ERROR_CLASS_B_FORCE2STOP,                   //!< 18 - [T/F] Class B forced to stop without any Class B call
    MCI_ERROR_PWM_SHUTDOWN_FAILED,             		//!< 19 - [T/F] From Class B - SR_FAULT_PWM_SHUTDOWN_FAILED

    MCI_ERROR_OBSERVER_FAILURE,                     //!< 20 - [T/F] Observer failure - lost observability
    MCI_ERROR_NOT_USED,                             //!< 21 - [T/F] Not Used

	MCI_ERROR_SURGE_RELAY_OPEN,                     //!< 22 - [T/F] if set this bit if surge relay is open

    MCI_ERROR_NR_OF_DI                              //!< Total declared of digital inputs
} MCI_ERROR_TYPE;




//! Analog inputs to application
typedef enum
{
    MCI_AI_TARGET_SPEED_RPM_S16 = 0,                //!<  0 - Target rotor shaft speed in RPM - x65536 - (1<<16)
    MCI_AI_SPEED_REFERENCE_RPM_S16,                 //!<  1 - Speed reference in RPM - x65536 - (1<<16)
    MCI_AI_ACTUAL_SPEED_S16,                        //!<  2 - Estimated rotor shaft speed in RPM - x65536 - (1<<16)
    MCI_AI_SPEED_ERROR_RPM_S16,                     //!<  3 - Difference between estimated and reference speeds in RPM - x65536 - (1<<16)

    MCI_AI_MEAN_SPEED_S16,                          //!<  4 - Mean rotor speed calculation - x65536 - (1<<16)
    MCI_AI_RMS_MOTOR_CURRENT_S16,                   //!<  5 - RMS Motor current - x65536 - (1<<16)
    MCI_AI_ACTIVE_POWER_S16,                        //!<  6 - Measured active power in Watts - x65536 - (1<<16)
    MCI_AI_SHAFT_POWER_S16,                         //!<  7 - Estimated shaft output power in Watts from estimated torque - x65536 - (1<<16)

    MCI_AI_ROTOR_TEMP_S16,                          //!<  8 - Rotor temperature in Celsius - x65536 - (1<<16)
    MCI_AI_STATOR_TEMP_S16,                         //!<  9 - Stator temperature in Celsius - x65536 - (1<<16)
    MCI_AI_LOAD_TORQUE_S16,                         //!< 10 - Read the load torque in Nm - x65536 - (1<<16)
    MCI_AI_SHAFT_POSITION_DEG_S16,                  //!< 11 - Estimated rotor position in deg - x65536 - (1<<16)

    MCI_AI_BUS_CURRENT_S16,                         //!< 12 - Bus current in Amps - x65536 - (1<<16)
    MCI_AI_BUS_VOLTAGE_S16,                         //!< 13 - Bus voltage in Volts - x65536 - (1<<16)
    MCI_AI_INVERTER_TEMP_S16,                       //!< 14 - Inverter temperature in Celsius - x65536 - (1<<16)
    MCI_AI_INVERTER_TEMP_RAW_DATA,                  //!< 15 - Inverter temperature in raw data [ADC counts]

    MCI_AI_VOLTAGE_PHASE_A_S16,                     //!< 16 - Motor phase A voltage in Volts - x65536 - (1<<16)
    MCI_AI_VOLTAGE_PHASE_B_S16,                     //!< 17 - Motor phase B voltage in Volts - x65536 - (1<<16)
    MCI_AI_VOLTAGE_PHASE_C_S16,                     //!< 18 - Motor phase C voltage in Volts - x65536 - (1<<16)
    MCI_AI_SPEED_LOOP_GAIN_TABLE_INDEX,             //!< 19 - Select index of speed loop gain table

    MCI_AI_CURR_PHASE_A_S16,                        //!< 20 - Phase current A in A - x65536 - (1<<16)
    MCI_AI_CURR_PHASE_B_S16,                        //!< 21 - Phase current B in A - x65536 - (1<<16)
    MCI_AI_CURR_PHASE_C_S16,                        //!< 22 - Phase current C in A - x65536 - (1<<16)
    MCI_AI_STATOR_RESISTANCE_S16,                   //!< 23 - Stator resistance in Ohm - x65536 - (1<<16)

    MCI_AI_INTERNAL_MCI_STATE,                      //!< 24 - Returns the internal Mci state
    MCI_AI_NR_OF_AI                                 //!< Total declared of analog inputs
} MCI_AI_TYPE;



//! Mci Parameters
typedef enum
{
    MCI_PARAMS_MAX_SPEED_RPM = 0,                   //!< 0 - Returns the maximum allowed speed
    MCI_PARAMS_MIN_SPEED_RPM,                       //!< 1 - Returns the minimum allowed speed
    MCI_PARAMS_MAX_RPM_PER_SEC_ACCEL,               //!< 2 - Returns the maximum allowed acceleration
    MCI_PARAMS_MAX_RPM_PER_SEC_DECEL,               //!< 3 - Returns the maximum allowed deceleration

    MCI_PARAMS_MIN_RPM_PER_SEC_ACCEL,               //!< 4 - Returns the minimum allowed acceleration
    MCI_PARAMS_MIN_RPM_PER_SEC_DECEL,               //!< 5 - Returns the minimum allowed deceleration
    MCI_PARAMS_NR_OF_PARAMS                         //!< Total declared of mci parameters
} MCI_GET_PARAMETERS_TYPE;




//! Digital outputs from application
typedef enum
{
    MCI_DO_VA_WASH_STATE = 0,                       //!< 0 - [T/F] Set wash state for VA - TRUE = Agi / FALSE = Spin

    MCI_DO_NR_OF_DO                                 //!< Total of declared digital outputs
} MCI_DO_TYPE;




//! Analog outputs from application
typedef enum
{
    MCI_AO_SPEED_LOOP_PROP_GAIN_S16 = 0,            //!<  0 - Proportional speed loop gain - x65536 - (1<<16)
    MCI_AO_SPEED_LOOP_INT_GAIN_S16,                 //!<  1 - Integral speed loop gain - x65536 - (1<<16)
    MCI_AO_SPEED_LOOP_GAIN_TABLE_INDEX,             //!<  2 - Select index of speed loop gain table
    MCI_AO_SPEED_FIR_TAPS_TABLE_INDEX,              //!<  3 - Select index of speed fir taps

    MCI_AO_SOFTSTART_COUNTER_INDEX,                 //!<  4 - Select index of soft start counter
    MCI_AO_SPEED_LIMIT_INDEX,                       //!<  5 - Select index of speed controller limit
    MCI_AO_MCL_PARAMETERS_ADDRESS,                  //!<  6 - Set the address of Mcl parameters structure
    MCI_AO_ADD_DELTA_SPEED_REF_S16,                 //!<  7 - Add (force) a delta speed on the speed reference - x65536 - (1<<16)

    MCI_AO_NR_OF_AO                                 //!< Total of declared analog outputs
} MCI_AO_TYPE;




//! Manual control method of the inverter
typedef enum
{
    MCI_MANUAL_INJECTION_DC_VOLTAGE = 0,            //!<  0 - Inject DC voltage in the motor, requires voltage level and position
    MCI_MANUAL_INJECTION_AC_VOLTAGE,                //!<  1 - Inject AC voltage in the motor, requires voltage level and frequency
    MCI_MANUAL_INJECTION_DC_CURRENT,                //!<  2 - Inject DC current in the motor, requires current level [A], current rate [A/s] and position [degree]
    MCI_MANUAL_INJECTION_AC_CURRENT,                //!<  3 - Inject AC current in the motor, requires current level and frequency
} MCI_INJECTION_TYPE;




//! Response definition
typedef enum
{
    MCI_CMD_INDEX_OUT_RANGE         = -7,           //!< -7 - [T/F] - command was denied by Mci because selected motor does not exist
    MCI_CMD_STOP_REQUESTED          = -6,           //!< -6 - [T/F] - command was denied by Mci because motor is stopping by user request
    MCI_CMD_PARAM_NOT_LOADED        = -5,           //!< -5 - [T/F] - command was denied by Mci because parameters are not loaded

    MCI_CMD_TIMEOUT                 = -4,           //!< -4 - [T/F] - Command was timeout due to long feedback time
    MCI_CMD_RAMP_OUT_OF_RANGE       = -3,           //!< -3 - [T/F] - Command was denied by Mci due to acceleration out of range
    MCI_CMD_SPEED_OUT_OF_RANGE      = -2,           //!< -2 - [T/F] - Command was denied by Mci due to speed out of range
    MCI_CMD_DENIED                  = -1,           //!< -1 - [T/F] - Command was denied by Mci - reason not disclosed.

    MCI_CMD_ACCEPTED                =  0,           //!<  0 - [T/F] - Command was accepted by Mci
} MCI_CMD_RESPONSE_TYPE;



typedef struct
{
    sint32 Max_Speed_Rpm;                           //!< [rpm] Define the maximum allowed speed
    sint32 Min_Speed_Rpm;                           //!< [rpm] Define the minimum allowed speed
    sint32 Max_Rpm_Per_Sec_Accel;                   //!< [rpm/s] Maximum allowed acceleration
    sint32 Max_Rpm_Per_Sec_Decel;                   //!< [rpm/s] Maximum allowed deceleration
    sint32 Min_Rpm_Per_Sec_Accel;                   //!< [rpm/s] Minimum allowed acceleration
    sint32 Min_Rpm_Per_Sec_Decel;                   //!< [rpm/s] Minimum allowed deceleration
} MCI_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void Mci__Initialize(void);                         //!< Initialize Class A once after power up/reset
void Mci__PwmHandler(sint32 ia_adc, sint32 ib_adc, sint32 ic_adc, sint32 dcbus_adc); //!< Handler running at MCPwm rate
void Mci__HallHandler(void);                        //!< Handler running at Hall sensors rate
void Mci__250usHandler(void);                       //!< Handler running at 250 us (4 kHz) rate
void Mci__1msHandler(void);                         //!< Handler running at 1 ms (1 kHz) rate
void Mci__5msHandler(void);                         //!< Handler running at 5 ms (200 Hz) rate
void Mci__25msHandler(void);                        //!< Handler running at 25 ms (40 Hz) rate

//! Commands to the motor
MCI_CMD_RESPONSE_TYPE Mci__Run(MOTOR_ENUM_TYPE motor, sint32 speed_rpm, sint32 ramp_rpm_per_s);
MCI_CMD_RESPONSE_TYPE Mci__Manual_Injection(MOTOR_ENUM_TYPE motor, MCI_INJECTION_TYPE method, sint32 level_x32, sint32 level_rate_x32, sint32 param_x32, sint32 param_rate_x32);
MCI_CMD_RESPONSE_TYPE Mci__Stop(MOTOR_ENUM_TYPE motor, sint32 ramp_rpm_per_s);

//! Get and read ead data from the motor controller
uint8  Mci__GetError(MOTOR_ENUM_TYPE motor, MCI_ERROR_TYPE read_error);
uint32 Mci__GetErrorList(MOTOR_ENUM_TYPE motor);
uint8  Mci__GetDigital(MOTOR_ENUM_TYPE motor, MCI_DI_TYPE read_data);
uint32 Mci__GetDigitalList(MOTOR_ENUM_TYPE motor);
sint32 Mci__GetAnalog(MOTOR_ENUM_TYPE motor, MCI_AI_TYPE read_data);
sint32 Mci__GetParams(MOTOR_ENUM_TYPE motor, MCI_GET_PARAMETERS_TYPE read_data);

//! Set and write data to the motor controller
uint8 Mci__ClearError(MOTOR_ENUM_TYPE motor);
uint8 Mci__SetDigital(MOTOR_ENUM_TYPE motor, MCI_DO_TYPE write_data, uint8 value);
uint8 Mci__SetAnalog(MOTOR_ENUM_TYPE motor, MCI_AO_TYPE write_data, sint32 value);
uint8 Mci__SetParams(void*  params_address);

//! Commands to handle FVT
    //! Error during FVT
    typedef enum
    {
        MCI_FVT_ERROR_WRONG_OC = 0,                     //! bit 0 - OC happend when it was not supposed;
        MCI_FVT_ERROR_OC_TIMEOUT,                       //! bit 1 - Timed out when checking OC hardware;
        MCI_FVT_ERROR_DATA_OLD,                         //! bit 2 - Data is old, already read, may run again FVT?;
        MCI_FVT_ERROR_RESULT_NO_FVT_TEST,               //! bit 3 - Attempted to read data w/o ruuning FVT
    } MCI_FVT_ERROR_TYPE;

    //! Status during FVT
    typedef enum
    {
        MCI_FVT_STATUS_STARTED = 0,                     //! bit 0 - just started
        MCI_FVT_STATUS_FINISHED,                        //! bit 1 - finished

        MCI_FVT_STATUS_TOTAL
    } MCI_FVT_STATUS_TYPE;


    //! Struct that contains all statuses after running FVT
    typedef struct
    {
        //! Sensors test - DC current injection
        sint16 Sensors_Test_Avg_DC_Bus_Volt;            //!< [x64 V] Averaged dc bus voltage while injecting dc current
        sint16 Sensors_Test_Avg_Phase_Volt_A;           //!< [x64 V] Averaged phase A voltage while injection dc current
        sint16 Sensors_Test_Avg_Phase_Volt_B;           //!< [x64 V] Averaged phase B voltage while injection dc current
        sint16 Sensors_Test_Avg_Phase_Volt_C;           //!< [x64 V] Averaged phase C voltage while injection dc current
        sint16 Sensors_Test_Avg_Curr_A;                 //!< [x1024 A] Averaged phase A current while injeting current
        sint16 Sensors_Test_Avg_Curr_B;                 //!< [x1024 A] Averaged phase B current while injeting current
        sint16 Sensors_Test_Avg_Curr_C;                 //!< [x1024 A] Averaged phase C current while injeting current

        //! SurgeRelay test - Open and Closed
        sint16 Surge_Test_Avg_DC_Bus_Volt_Open;         //!< [x64 V] Averaged DC bus voltage while injection and relay open
        sint16 Surge_Test_Avg_DC_Bus_Volt_Close;        //!< [x64 V] Averaged DC bus voltage while injection and relay closed

        //! OC hardware test
        uint8 OC_Test_Found;                            //!< [T/F] True - OC hardware triggered, False - OC hardwere NOT triggered

        //! FVT test status
        MCI_FVT_ERROR_TYPE Error_List;                  //!< [MCI_FVT_ERROR_TYPE] True - Error found in bit x; False - no error found

    } MCI_FVT_TEST_RESULT_TYPE;

#if MCI_FVT_TESTS == ENABLED

    MCI_CMD_RESPONSE_TYPE Mci__FvtStart(MOTOR_ENUM_TYPE motor);
    MCI_CMD_RESPONSE_TYPE Mci__FvtStop(MOTOR_ENUM_TYPE motor);
    uint8 Mci__FvtGetStatus(MOTOR_ENUM_TYPE motor, MCI_FVT_STATUS_TYPE this_status);
    void Mci__FvtGetResult(MOTOR_ENUM_TYPE motor, MCI_FVT_TEST_RESULT_TYPE *fvt_data_result);

#endif



#endif // MCI_H_

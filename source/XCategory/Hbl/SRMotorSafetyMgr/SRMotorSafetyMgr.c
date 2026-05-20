/**
 *  @file       SRMotorSafetyMgr.c
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRMotorSafetyMgr.h"
#include "SRMotorSafetyMgr_prv.h"

// Standard Safety Relevant Class B libraries
#include "SRMicro.h"
#include "SRData.h"
#include "SRMCAtod.h"
#include "SRMCPwm.h"
#ifdef SR_CIM_MOTOR
#include "SRMCInputCapture.h"
#endif
#include "Crc16.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef SRDATA_CORRUPTION_EXCEPTION
    #define SRDATA_CORRUPTION_EXCEPTION(address)
#endif

/* Section identifiers for DATA corruption checks that are not using SRDATA */
#define DATA_CORRUPTION_SAFETY_PARAMS_SECTION   0
#define DATA_CORRUPTION_ANALOG_VAR_SECTION      1
#define DATA_CORRUPTION_PWM_VAR_SECTION         2
#define DATA_CORRUPTION_ISR_ENTER_SECTION       3
#define DATA_CORRUPTION_ISR_HANDLER_SECTION     4

//! Macros to update safety relevant data
//! Update SR variable and its negate
#define SR_MCU_MOTOR_UPDATE_VAR(var, value)             (var) = (value); (N##var) = ~((sint32)value)

#define SR_MCU_MOTOR_UPDATE_N_VAR(var)                  (N##var) = ~((sint32)var)



//! Macros to check safety relevant data
//! Check SR byte
#define SR_MCU_MOTOR_CHECK_BYTE(check,var)              {auto uint8 temp_u8;  temp_u8 = (uint8)(var); check += (temp_u8 !=  (uint8)(~(sint32)(N##var)));}
//! Check SR short
#define SR_MCU_MOTOR_CHECK_SHORT(check,var)             {auto uint16 temp_u16;  temp_u16 = (uint16)(var); check += (temp_u16 != (uint16)(~(sint32)(N##var)));}
//! Check SR long
#define SR_MCU_MOTOR_CHECK_LONG(check,var)              {auto uint32 temp_u32;  temp_u32 = (uint32)(var); check += (temp_u32 != (uint32)(~(sint32)(N##var)));}

// Check MACRO definitions
#define SR_DATA_MCU_CHECK_BYTE(var)                     SRData__CheckByte((uint8 *)&var, (uint8 *)&(N##var))
#define SR_DATA_MCU_CHECK_SHORT(var)                    SRData__CheckShort((uint16 *)&var, (uint16 *)&(N##var))
#define SR_DATA_MCU_CHECK_LONG(var)                     SRData__CheckLong((uint32 *)&var, (uint32 *)&(N##var))
#define SR_DATA_MCU_CHECK_FLOAT(var)                    SRData__CheckFloat((float *)&var, (float *)&(N##var))

#define SR_DATA_MCU_UPDATE_BYTE(var,value)              SRData__UpdateByte((uint8 *)&var, (uint8 *)&(N##var), (uint8)(value))
#define SR_DATA_MCU_UPDATE_SHORT(var,value)             SRData__UpdateShort((uint16 *)&var, (uint16 *)&(N##var), (uint16)(value))
#define SR_DATA_MCU_UPDATE_LONG(var,value)              SRData__UpdateLong((uint32 *)&var, (uint32 *)&(N##var), (uint32)(value))
#define SR_DATA_MCU_UPDATE_FLOAT(var,value)             SRData__UpdateFloat((float *)&var, (float *)&(N##var), (float)(value))




///////////////////////////////////////////////////////////////////////////////////////////////
// UNLOCK DOOR SPEED - define routine parameters based on the user parameters
#define SR_UNLOCKDOORSPEED_CURR_THRES_FIXP              (sint32)(SR_UNLOCKDOORSPEED_MIN_CURR_READ)
#define SR_UNLOCKDOORSPEED_DEBOUNCING_COUNTER           (uint32)(200.0f*SR_UNLOCKDOORSPEED_DEBOUNCING_TIME)
#define SR_UNLOCKDOORSPEED_FAIL_TIMER_COUNTER           (uint32)(200.0f*SR_UNLOCKDOORSPEED_FAIL_DETECTION_TIME)
// CIM Motor Handler defines
#define SR_UNLOCKDOORSPEED_TARGET_NOT_REACHED_TIMEOUT   (51200) // 256 seconds @ 200 Hz
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COUNTER THRESHOLD FOR CIM MOTOR MOVING HANDLER
// The Input Capture counter will be compared to this threshold in order to understand if the motor shall be considered
// moving or not.
#define SR_UNLOCKDOORSPEED_LOW_COUNTER_THR              (4428)  // 1270 rpm_M


///////////////////////////////////////////////////////////////////////////////////////////////
// POWER UP - define routine parameters for power up routine
#define SR_POWER_UP_DELAY_TIMER                         (sint16)(SR_POWER_UP_DELAY_TIME*1000.0/5.0)

///////////////////////////////////////////////////////////////////////////////////////////////
// SHUT DOWN PLAUSIBILITY - define routine parameters for checking the Shut Down Pin circuitry
#define SD_PIN_ZERO_TIME                                (uint32)(SD_PIN_ZERO_TIME_MS/1000.0f*(float32)SR_PWM_FREQUENCY_HZ)
#define SD_PIN_ONE_TIME                                 (uint32)(SD_PIN_ONE_TIME_MS/1000.f*(float32)SR_PWM_FREQUENCY_HZ)

///////////////////////////////////////////////////////////////////////////////////////////////
#define SR_MAX_LONG_VARIABLE                            4294967294L

#define ONE_THIRD                                       0.33333333333f
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RMS INTEGRATION TIMING - define the samples amount to meet the required time for rms calculation
#ifdef RMS_SAMPLING_TIME_MS
    #define RMS_SAMPLER_FREQUENCY                       4000.0f //! rms current sampler (1/RMS_SAMPLER_FREQUENCY = 250 us)
    #define RMS_COUNTER_SAMPLES_AT_TS                   (sint32)(((RMS_SAMPLING_TIME_MS * RMS_SAMPLER_FREQUENCY)/1000.0f)+1.0f) //
#endif

//! Maximum value used for period counter
#define SR_MOTOR_SAFETY_MNGR_MAX_PERIOD_COUNTER         1073741824L

#define OL_INIT_DEFAULT                                 35.0f

//! Define the states for the state machine handler
typedef enum
{
    SAFETY_MNGR_NOT_DEFINED         = 0,                //! Reserved 0 index - shall not be used
    SAFETY_MNGR_INITIALIZE          = 1,                //! Initialization for the state machine
    SAFETY_MNGR_IDLE                = 2,                //! The state machine is able to accept requests to run the pre run tests
    SAFETY_MNGR_PRERUN_TESTS        = 3,                //! All tests before running the motor has to be performed within this state
    SAFETY_MNGR_POSTRUN_TESTS       = 4,                //! Performs tests after running and before setting the unlock door speed flag
    SAFETY_MNGR_RUNNING             = 5,                //! This state is used when the motor is running
    SAFETY_MNGR_ERROR_FOUND         = 6,                //! If an error is found, the state machine shall stay in this ERROR state
    SAFETY_MNGR_UNLOCK_DOOR_CHECK   = 7,                //! Perform the plausibility check only once after IDLING for some time
} SAFETY_MNGR_TYPE;

//! Define for current offset verification
typedef enum
{
    MOTOR_ANALOG_CHECK_CURR_FAIL = 0,
    MOTOR_ANALOG_CHECK_VBUS_FAIL,
    MOTOR_ANALOG_CHECK_CORRECT,
    MOTOR_ANALOG_CHECK_SIZE
} MOTOR_ANALOG_CHECK_TYPE;


MOTOR_SAFETY_PARAMS_TYPE* SR_MotorSafety_Params;

static volatile SAFETY_MNGR_TYPE SR_SafetyMngr_State;

// For these variables, use the SR_MOTOR_FAULT_LIST_TYPE for the bit definition
static volatile uint16 SR_Motor_Fault;
static volatile uint16 SR_Motor_Fault_Holder;

// For these variables, use the SR_MOTOR_FLAGS_LIST_TYPE for the bit definition
static volatile uint16 SR_Motor_Flags;

static volatile sint16 SR_CurrentA;
static sint16 SR_CurrentOffsetA;
static volatile sint32 SR_CurrentOffsetHistA;

static volatile sint16 SR_CurrentB;
static sint16 SR_CurrentOffsetB;
static volatile sint32 SR_CurrentOffsetHistB;

static volatile sint16 SR_CurrentC;
static sint16 SR_CurrentOffsetC;
static volatile sint32 SR_CurrentOffsetHistC;

static sint16 SR_Raw_CurrentA;
static sint16 SR_Raw_CurrentB;
static sint16 SR_Raw_CurrentC;

static volatile uint16 SR_Sector;

static sint16 SR_DCBus;
static volatile sint16 SR_PowerUpTimer;

static volatile uint32 SR_ZeroSpeedDebouncer;
static volatile uint32 SR_ZeroSpeedWithCurrTimer;
static volatile sint32 SR_SquaredCurrentsSum;
static volatile uint32 SR_CalibrationCntr;

#ifdef SR_CIM_MOTOR
    // Motor Moving Handler CIM variables
    static volatile uint32 SR_VerifyTargetCounter;
#endif

static volatile sint32 SR_SquaredCurrentRmsA;
static volatile sint32 SR_SquaredCurrentRmsB;
static volatile sint32 SR_SquaredCurrentRmsC;
static volatile sint32 SR_SquaredCurrentSumA;
static volatile sint32 SR_SquaredCurrentSumB;
static volatile sint32 SR_SquaredCurrentSumC;
static volatile sint32 SR_Curr_Rms_Counter;


static volatile uint16 SR_Phase_Lost_Counter;
static volatile uint32 SR_Timeout_Failure_Retry;


static volatile uint16 SR_Overload_Counter;
static volatile float SR_Overload_Temp;

static float SR_Voltage_Frequency;                      //! [rpm] motor speed - use for class A - there is no RAM check
static float SR_Squared_Current;                        //! [A] squared current sum of all 3 currents - use for class A - there is no RAM check


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

static volatile SAFETY_MNGR_TYPE NSR_SafetyMngr_State;
static volatile uint16 NSR_Motor_Fault;
static volatile uint16 NSR_Motor_Fault_Holder;

static volatile uint16 NSR_Motor_Flags;

static volatile sint16 NSR_CurrentA;
static volatile sint16 NSR_CurrentOffsetA;
static volatile sint32 NSR_CurrentOffsetHistA;

static volatile sint16 NSR_CurrentB;
static volatile sint16 NSR_CurrentOffsetB;
static volatile sint32 NSR_CurrentOffsetHistB;

static volatile sint16 NSR_CurrentC;
static volatile sint16 NSR_CurrentOffsetC;
static volatile sint32 NSR_CurrentOffsetHistC;

static volatile sint16 NSR_Raw_CurrentA;
static volatile sint16 NSR_Raw_CurrentB;
static volatile sint16 NSR_Raw_CurrentC;

static volatile uint16 NSR_Sector;
static volatile sint16 NSR_DCBus;
static volatile sint16 NSR_PowerUpTimer;

static volatile uint32 NSR_ZeroSpeedDebouncer;
static volatile uint32 NSR_ZeroSpeedWithCurrTimer;
static volatile sint32 NSR_SquaredCurrentsSum;
static volatile uint32 NSR_CalibrationCntr;

#ifdef SR_CIM_MOTOR
    // Motor Moving Handler CIM variables
    static volatile uint32 NSR_VerifyTargetCounter;
#endif

static volatile sint32 NSR_SquaredCurrentRmsA;
static volatile sint32 NSR_SquaredCurrentRmsB;
static volatile sint32 NSR_SquaredCurrentRmsC;
static volatile sint32 NSR_SquaredCurrentSumA;
static volatile sint32 NSR_SquaredCurrentSumB;
static volatile sint32 NSR_SquaredCurrentSumC;
static volatile sint32 NSR_Curr_Rms_Counter;


static volatile uint16 NSR_Phase_Lost_Counter;
static volatile uint32 NSR_Timeout_Failure_Retry;

static volatile uint16 NSR_Overload_Counter;
static volatile float NSR_Overload_Temp;

static float NSR_Squared_Current;
static float NSR_Voltage_Frequency;

#ifdef SR_BPM_MOTOR

    const uint32 CHECK_UNLOCK_DOOR_SPEED_TIMEOUT_COUNTER    = (uint32)((CHECK_UNLOCK_DOOR_SPEED_TIMEOUT_TIME/1000.0f)/0.005f);
    const uint32 CHECK_UNLOCK_DOOR_SPEED_IDLING_BEFORE_CHECK_COUNT = (uint32)((CHECK_UNLOCK_DOOR_SPEED_IDLING_BEFORE_CHECK_TIME/1000.0f)/0.005f);
    const sint32 CHECK_UNLOCK_DOOR_MAX_VOLTAGE_COUNTS = (sint32)CHECK_UNLOCK_DOOR_MAX_VOLTAGE_DUTY;
    const sint32 CHECK_UNLOCK_DOOR_MAX_VOLTAGE_DUTY_STEP_COUNTS = (sint32)CHECK_UNLOCK_DOOR_MAX_VOLTAGE_DUTY_STEP;
    const sint16 CHECK_UNLOCK_DOOR_CURRENT_THRESHOLDS_COUNTS = (sint32)CHECK_UNLOCK_DOOR_CURRENT_THRESHOLDS;

    static sint32 SR_Avg_Fund_Period_High_Threshold;            //!<PARAMETER Average frequency HIGH threshold hysteresis
    static sint32 SR_Avg_Fund_Period_Low_Threshold;             //!<PARAMETER Average frequency LOW threshold hysteresis
    static uint8 SR_Avg_Fund_Period_Cycle_Total;                //!< PARAMETER Total cycles to calculate the average

    static sint32 SR_Avg_Threshold;                             //!< Instantaneous hysteresis
    static uint8 SR_Avg_Cross_Event;                            //!< Auxiliary variable to generate the cycles counter event
    static uint32 SR_Avg_Fund_Period_Time_Counter;              //!< Elapsed (accumulated) time
    static uint32 SR_Avg_Fund_Period_Cycle_Counter;             //!< Cycle counter
    static sint32 SR_Applied_Duty_A_Sum;                        //!< Applied duty cycle
    static sint32 SR_Applied_Duty_A_Sum_High;                   //!< Applied duty cycle
    static uint32 SR_Avg_Fund_Period;                           //!< Averaged speed from the fundamental output voltage

    static uint32 SR_Check_Unlock_Door_Speed_Timer;             //!< Timer to perform the unlock door speed check after idling for some time
    static sint32 SR_Check_Unlock_Door_Speed_Delta_Voltage;     //!< Delta voltage in pwm counts to control the output voltage of unlock speed check
    static uint8 SR_Avg_Fund_Ignored_First;                     //!< Variable used to ignore first startup cycle of average frequency calculation

    static sint32 NSR_Avg_Fund_Period_High_Threshold;           //!<PARAMETER Average frequency HIGH threshold hysteresis
    static sint32 NSR_Avg_Fund_Period_Low_Threshold;            //!<PARAMETER Average frequency LOW threshold hysteresis
    static uint8 NSR_Avg_Fund_Period_Cycle_Total;               //!< PARAMETER Total cycles to calculate the average

    static sint32 NSR_Avg_Threshold;                            //!< Instantaneous hysteresis
    static uint8 NSR_Avg_Cross_Event;                           //!< Auxiliary variable to generate the cycles counter event
    static uint32 NSR_Avg_Fund_Period_Time_Counter;             //!< Elapsed (accumulated) time
    static uint32 NSR_Avg_Fund_Period_Cycle_Counter;            //!< Cycle counter
    static sint32 NSR_Applied_Duty_A_Sum;                       //!< Applied duty cycle
    static sint32 NSR_Applied_Duty_A_Sum_High;                  //!< Applied duty cycle
    static uint32 NSR_Avg_Fund_Period;                          //!< Averaged speed from the fundamental output voltage
    static sint32 NSR_Check_Unlock_Door_Speed_Delta_Voltage;    //!< Delta voltage in pwm counts to control the output voltage of unlock speed check

    static uint32 NSR_Check_Unlock_Door_Speed_Timer;            //!< Timer to perform the unlock door speed check after idling for some time

    static uint8 NSR_Avg_Fund_Ignored_First;                    //!< Variable used to ignore first startup cycle of average frequency calculation
#endif

#ifdef SRMOTORSAFETYMGR_INTERNAL_ISR_CHECK
    static uint16 SR_MC_Isr_Enter_Cntr;
    static uint16 SR_MC_Isr_Exit_Cntr;
    static uint16 SR_250us_Isr_Enter_Cntr;
    static uint16 SR_250us_Isr_Exit_Cntr;
    static uint8  SR_Isr_Init_Flag;

    static uint16 NSR_MC_Isr_Enter_Cntr;
    static uint16 NSR_MC_Isr_Exit_Cntr;
    static uint16 NSR_250us_Isr_Enter_Cntr;
    static uint16 NSR_250us_Isr_Exit_Cntr;
    static uint8  NSR_Isr_Init_Flag;


    static void SrIsrMcEnterIsr(void);
    static void SrIsrMcExitIsr(void);
    static void SrIsr250usEnterIsr(void);
    static void SrIsrHandler(void);
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
uint8 CheckSafetyParameters(void);
static void CheckAnalogVariables4Failure(void);
static void CheckVariables4Failure_Pwm(void);
static void CheckVariables4Failure_250us(void);
static void CheckVariables4Failure_5ms(void);
static void CheckVariables4Failure_25ms(void);
static void AverageSensorsOffset(void);
static void CalculateRawAnalogData_250us(void);
static void CalculateRmsAnalogData(void);
static void PhaseLostHandler(void);
static void RcThermalUpdate(void);
static void InitOverLoadProtection(void);
static void OverLoadProtection(void);
static void PeripheralDiagnostic(void);
static void ClassAPwmAccess(void);
static void PowerUpSensors(void);
static MOTOR_ANALOG_CHECK_TYPE MotorAnalogCircuitCheck(void);
static uint8 CheckHwFaultStatus(void);
static void OverVoltageCheck(void);
static void OverVoltageProtection(void);
static void CheckCurrentsPlausibility(void);
static void Init_Avg_Frequency(void);
static void Calc_Avg_Frequency(void);
static void Check_Avg_Frequency(void);
static void SetFault(uint16 fault);
static void ClearFault(uint16 fault);

#ifdef SR_THIRD_CURRENT_RECONSTRUCTION
static void UpdateAdcConfig(void);
#endif


#ifdef SR_CIM_MOTOR

    static void InitMotorMovingHandlerCim(void);
    static void MotorMovingHandlerCim(void);
    static void InputCapturePlausibilityHandler(void);

    #define SR_MOTORSAFETY_MGR_POWERUP_ACTION()             SRMCPwm__TurnOffAllGates()
    #define SR_MOTORSAFETY_MGR_ERROR_REACTION()             SRMCPwm__TurnOffAllGates()
    #define SR_MOTORSAFETY_MGR_POSTRUN_ACTION()             SRMCPwm__TurnOffAllGates()
    #define SR_INITMOTORMOVINGHANDLER()                     InitMotorMovingHandlerCim()
    #define SR_MOTORMOVINGHANDLER()                         MotorMovingHandlerCim()
    #define SR_INPUTCAPTUREPLAUSIBILITYHANDLER()            InputCapturePlausibilityHandler()

#endif

#ifdef SR_BPM_MOTOR

    static void InitMotorMovingHandlerBpm(void);
    static void MotorMovingHandlerBpm(void);

    #define SR_INITMOTORMOVINGHANDLER()                     InitMotorMovingHandlerBpm()
    #define SR_MOTORMOVINGHANDLER()                         MotorMovingHandlerBpm()
    #define SR_INPUTCAPTUREPLAUSIBILITYHANDLER()

#endif

#ifndef SR_MOTORSAFETY_MGR_POWERUP_ACTION
    #define SR_MOTORSAFETY_MGR_POWERUP_ACTION()             SRMCPwm__TurnOnLowSideGates()
#endif
#ifndef SR_MOTORSAFETY_MGR_ERROR_REACTION
    #define SR_MOTORSAFETY_MGR_ERROR_REACTION()             SRMCPwm__TurnOnLowSideGates()
#endif
#ifndef SR_MOTORSAFETY_MGR_POSTRUN_ACTION
    #define SR_MOTORSAFETY_MGR_POSTRUN_ACTION()             SRMCPwm__TurnOnLowSideGates()
#endif

#ifndef SRMOTOR_RUNNING_PWM
    #define SRMOTOR_RUNNING_PWM()
#endif

#ifndef SR_MODULATION_LIMIT
    #define SR_MODULATION_LIMIT  1.0f
#endif


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize all protection submodule. This shall be a part of the whole class B protection.
 *              This shall be called once, when the processor is powering up or recovering from a reset.
 *
 *
 */
void SRMotorSafetyMgr__Initialize(void)
{
    uint16 temp;

    SRMOTORSAFETYMGR_FLOW_INITIALIZE_BEGIN();


    SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Fault,0);
    SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Fault_Holder,0);

    SR_Motor_Flags = 0;                             // Due to warm reset possibility, this variable needs to be manually zeroed.
    temp = SR_Motor_Flags;
    BIT_CLR(temp, SR_FLAG_CLASS_A_PWM_ACCESS);      // Deny the access to the pwm peripheral
    BIT_SET(temp, SR_FLAG_MICRO_POWERING_UP);       // Warn the protection handler that the micro is powering up...
    BIT_CLR(temp, SR_FLAG_UNLOCK_DOOR_SPEED_CHECK_REQUESTED); // Request to check unlock door speed after IDLE
    BIT_CLR(temp, SR_FLAG_MC_PARAMETERS_LOADED);    // Reset the motor control parameters loaded flag
    SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

    SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_INITIALIZE);   // Send the motor safety state machine to the initialize state
    SR_INITMOTORMOVINGHANDLER();                    // Initialize the zero speed handler


    // Initialize the over current (locked rotor and running overloaded) sub module
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentsSum,2*SR_POWERING_UP_CURRENTS_THRESHOLD); // Initialize to a higher value than the comparison threshold,
    // to prevent any wrong decision before finishing the powering up

    SR_DATA_MCU_UPDATE_SHORT(SR_Raw_CurrentA,0);            // Consider the raw phase current as zero
    SR_DATA_MCU_UPDATE_SHORT(SR_Raw_CurrentB,0);            // Consider the raw phase current as zero
    SR_DATA_MCU_UPDATE_SHORT(SR_Raw_CurrentC,0);            // Consider the raw phase current as zero
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentA,0);                // Consider the phase current as zero
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentB,0);                // Consider the phase current as zero
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentC,0);                // Consider the phase current as zero
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentOffsetA,0);          // Reset the offset information
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentOffsetB,0);          // Reset the offset information
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentOffsetC,0);          // Reset the offset information
    SR_DATA_MCU_UPDATE_SHORT(SR_Sector,0);
    SR_DATA_MCU_UPDATE_LONG(SR_CurrentOffsetHistA,0);       // Reset the offset history (filter) information
    SR_DATA_MCU_UPDATE_LONG(SR_CurrentOffsetHistB,0);       // Reset the offset history (filter) information
    SR_DATA_MCU_UPDATE_LONG(SR_CurrentOffsetHistC,0);       // Reset the offset history (filter) information
    SR_DATA_MCU_UPDATE_SHORT(SR_DCBus,0);                   // Consider as the DC voltage as zero
    SR_DATA_MCU_UPDATE_SHORT(SR_PowerUpTimer,0);            // Zero the power up timer
    SR_DATA_MCU_UPDATE_LONG(SR_CalibrationCntr,0);
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentRmsA,0);       // Reset the current A rms calculated value
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentRmsB,0);       // Reset the current B rms calculated value
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentRmsC,0);       // Reset the current C rms calculated value
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentSumA,0);       // Reset the current A rms accumulator
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentSumB,0);       // Reset the current B rms accumulator
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentSumC,0);       // Reset the current C rms accumulator
    SR_DATA_MCU_UPDATE_LONG(SR_Curr_Rms_Counter,0);
    SR_DATA_MCU_UPDATE_SHORT(SR_Phase_Lost_Counter,0);
    SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,0);    // Update the time out to zero


#ifdef SR_BPM_MOTOR

    SR_MCU_MOTOR_UPDATE_VAR(SR_Check_Unlock_Door_Speed_Timer, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Check_Unlock_Door_Speed_Delta_Voltage, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Avg_Fund_Period_Time_Counter, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Avg_Fund_Period_Cycle_Counter, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Applied_Duty_A_Sum, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Avg_Threshold, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Avg_Fund_Period_Low_Threshold, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Avg_Fund_Period_High_Threshold, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Avg_Cross_Event, 0);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Avg_Fund_Period_Cycle_Total, AVG_FREQUENCY_TOTAL_CYCLES);
    SR_MCU_MOTOR_UPDATE_VAR(SR_Avg_Fund_Period, SR_MOTOR_SAFETY_MNGR_MAX_PERIOD_COUNTER);

    SR_DATA_MCU_UPDATE_BYTE(SR_Avg_Fund_Ignored_First, FALSE);

    SR_DATA_MCU_UPDATE_FLOAT(SR_Voltage_Frequency, 0.0f);
    SR_DATA_MCU_UPDATE_FLOAT(SR_Overload_Temp,OL_INIT_DEFAULT); // Init the OverLoad temperature [default value]
    SR_DATA_MCU_UPDATE_FLOAT(SR_Squared_Current, 0.0f);

#endif

#ifdef SRMOTORSAFETYMGR_INTERNAL_ISR_CHECK
    SR_DATA_MCU_UPDATE_SHORT(SR_MC_Isr_Enter_Cntr,0);
    SR_DATA_MCU_UPDATE_SHORT(SR_MC_Isr_Exit_Cntr,0);
    SR_DATA_MCU_UPDATE_SHORT(SR_250us_Isr_Enter_Cntr,0);
    SR_DATA_MCU_UPDATE_SHORT(SR_250us_Isr_Exit_Cntr,0);
    SR_DATA_MCU_UPDATE_BYTE(SR_Isr_Init_Flag,0);
#endif

    // Initialize Class A module before framework start up
    INVOKE_CLASS_A_INIT_HANDLER();
    // Initialize the SR Class B MCAtod module - required to initialize the MC Atod before MC Pwm.
    SRMCAtod__Initialize();

    // Initialize the SR Class B Pwm module
    SRMCPwm__Initialize(SR_PWM_FREQUENCY_HZ, SR_PWM_DEADTIME_NS);
    SR_MOTORSAFETY_MGR_POWERUP_ACTION();
    SRMCPwm__StartCounter();


#ifdef SR_BPM_MOTOR

    Init_Avg_Frequency();

#endif

    // Initialize the SR Flow control module
    SRMOTORSAFETYMGR_FLOW_INITIALIZE_END();
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Class B MCPwm Handler (same rate as the MCPwm period)
 *
 */
void SRMotorSafetyMgr__MCPwmHandler(void)
{
#if (defined SRMOTOR_RUNNING_2ND_PWM)
    static unsigned char pwm_post_scaler = 0;
#endif

    register sint16 current_ia;
    register sint16 current_ib;
    register sint16 current_ic;
    register sint16 dc_bus;

    SRMOTORSAFETYMGR_ISRMONITOR_MCPWM_HANDLER_BEGIN();

    // Safety variables check
    CheckVariables4Failure_Pwm();

#ifdef SR_CIM_MOTOR
    // InputCapture PWM Handler
    SRMCInputCapture__PwmHandler();
#endif

#ifdef SR_THIRD_CURRENT_RECONSTRUCTION
    switch (SR_Sector)
    {
		case 1:
			SR_Raw_CurrentB = SRMCATOD__GETADC2_REC1();
			current_ib 		= SR_Raw_CurrentB - SR_CurrentOffsetB;   // Subtract zero offset

			SR_Raw_CurrentC = SRMCATOD__GETADC3_REC1();
			current_ic 		= SR_Raw_CurrentC - SR_CurrentOffsetC;   // Subtract zero offset

			// Reconstruct phase A
			current_ia 		= -current_ib - current_ic;
			SR_Raw_CurrentA = current_ia + SR_CurrentOffsetA;
			break;

		case 2:
			SR_Raw_CurrentA = SRMCATOD__GETADC1_REC2();
			current_ia 		= SR_Raw_CurrentA - SR_CurrentOffsetA;   // Subtract zero offset

			SR_Raw_CurrentC = SRMCATOD__GETADC3_REC2();
			current_ic 		= SR_Raw_CurrentC - SR_CurrentOffsetC;   // Subtract zero offset

			// Reconstruct phase B
			current_ib 		= -current_ia - current_ic;
			SR_Raw_CurrentB = current_ib + SR_CurrentOffsetB;
			break;

		case 3:
			SR_Raw_CurrentA = SRMCATOD__GETADC1_REC3();
			current_ia 		= SR_Raw_CurrentA - SR_CurrentOffsetA;   // Subtract zero offset

			SR_Raw_CurrentB = SRMCATOD__GETADC2_REC3();
			current_ib 		= SR_Raw_CurrentB - SR_CurrentOffsetB;   // Subtract zero offset

			// Reconstruct phase C
			current_ic 		= -current_ia - current_ib;
			SR_Raw_CurrentC = current_ic + SR_CurrentOffsetC;
			break;

		default:
		    // Adc calculations - no reconstruction
		    SR_Raw_CurrentA = SRMCATOD__GETADC1();              // Read the converted values...
		    current_ia = SR_Raw_CurrentA - SR_CurrentOffsetA;   // Subtract zero offset


		    SR_Raw_CurrentB = SRMCATOD__GETADC2();              // Read the converted values...
		    current_ib = SR_Raw_CurrentB - SR_CurrentOffsetB;   // Subtract zero offset

		    SR_Raw_CurrentC = SRMCATOD__GETADC3();              // Read the converted values...
		    current_ic = SR_Raw_CurrentC - SR_CurrentOffsetC;   // Subtract zero offset
			break;
    }

    SR_MCU_MOTOR_UPDATE_N_VAR(SR_Raw_CurrentA);
    SR_MCU_MOTOR_UPDATE_N_VAR(SR_Raw_CurrentB);
    SR_MCU_MOTOR_UPDATE_N_VAR(SR_Raw_CurrentC);



#else
    // Adc calculations
    SR_Raw_CurrentA = SRMCATOD__GETADC1();              // Read the converted values...
    current_ia = SR_Raw_CurrentA - SR_CurrentOffsetA;   // Subtract zero offset
    SR_MCU_MOTOR_UPDATE_N_VAR(SR_Raw_CurrentA);

    SR_Raw_CurrentB = SRMCATOD__GETADC2();              // Read the converted values...
    current_ib = SR_Raw_CurrentB - SR_CurrentOffsetB;   // Subtract zero offset
    SR_MCU_MOTOR_UPDATE_N_VAR(SR_Raw_CurrentB);

    SR_Raw_CurrentC = SRMCATOD__GETADC3();              // Read the converted values...
    current_ic = SR_Raw_CurrentC - SR_CurrentOffsetC;   // Subtract zero offset
    SR_MCU_MOTOR_UPDATE_N_VAR(SR_Raw_CurrentC);
#endif

    SR_DCBus = SRMCATOD__GETDCBUSVOLTAGE();             // Read the converted values...
    dc_bus = SR_DCBus;
    SR_MCU_MOTOR_UPDATE_N_VAR(SR_DCBus);


    // Motor control peripheral diagnostic handler
    PeripheralDiagnostic();
    
    // Class A Pwm Access Management
	ClassAPwmAccess();


    if(SR_SafetyMngr_State == SAFETY_MNGR_PRERUN_TESTS) // Ensure the data is valid during prerun checks
    {
        // Update Class A with this analog data
        current_ia = 0;
        current_ib = 0;
        current_ic = 0;
        dc_bus = VBUS_SR_NOMINAL_AD_COUNTS;
	}

    INVOKE_CLASS_A_MCPWM_HANDLER(current_ia, current_ib, current_ic, dc_bus);


#if (defined SRMOTOR_RUNNING_2ND_PWM)
    pwm_post_scaler++;
    if(pwm_post_scaler & 0x01)
    {
        SRMOTOR_RUNNING_PWM();

    }
    else
    {
        SRMOTOR_RUNNING_2ND_PWM();
    }

#else
    SRMOTOR_RUNNING_PWM();
#endif


    SRMOTORSAFETYMGR_ISRMONITOR_MCPWM_HANDLER_END();
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Perform the 250us (4kHz) handler.
 */
void SRMotorSafetyMgr__Handler250us(void)
{
    SRMOTORSAFETYMGR_250US_FLOW_HANDLER_BEGIN();

    // Check for variables integrity
    CheckVariables4Failure_250us();

    // Check for currents plausibility
    CheckCurrentsPlausibility();

    // Calculate the raw data
    CalculateRawAnalogData_250us();

    // Calculate the rms current used by class B thermal model
    CalculateRmsAnalogData();

    // Calculate the fundamental output frequency
    Calc_Avg_Frequency();

    // Invoke the pwm driver 250 handler
    SRMCPwm__Handler250us();

    SRMOTORSAFETYMGR_250US_FLOW_HANDLER_END();
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      General slot for call rate of 1ms.
 */
void SRMotorSafetyMgr__Handler1ms(void)
{
    SRMOTORSAFETYMGR_1MS_FLOW_HANDLER_BEGIN();


    SRMOTORSAFETYMGR_1MS_FLOW_HANDLER_END();
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Perform the protection/plausibility check when required. The outcome of
 *              this method shall control the multiplexer which will allow/deny access
 *              to the PWM driver.
 */
void SRMotorSafetyMgr__Handler5ms(void)
{
    sint32 temp;
    sint32 temp1;
    uint32 temp2;
#ifdef SR_BPM_MOTOR
    // Stacked variables to be used in the current reading
    sint32 temp_current_a;
    sint32 temp_current_b;
    sint32 temp_current_c;
#endif

    SRMOTORSAFETYMGR_5MS_FLOW_HANDLER_BEGIN();

    CheckVariables4Failure_5ms();

    OverVoltageCheck();

    /////////////////////////////////////
    // This block will square each phase current and then add all of them.
    temp = (sint32)SR_CurrentA;
    temp2  = (uint32)temp*temp;

    temp = (sint32)SR_CurrentB;
    temp2 += (uint32)temp*temp;

    temp = (sint32)SR_CurrentC;
    temp2 += (uint32)temp*temp;

    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentsSum,temp2);


    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Safety Fault Check
    temp = (sint32)SR_Motor_Fault;
    if(temp != 0)
    {
        // Fault detected -> Move the state machine in error
        // if it is not under initialization
        temp1 = (sint32)SR_SafetyMngr_State;
        if(temp1 > SAFETY_MNGR_INITIALIZE)
        {
            SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_ERROR_FOUND);    // Wait for zero speed flag...
        }
    }



    temp = (sint32)SR_SafetyMngr_State;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // This state is used when the motor is running
    if(temp == SAFETY_MNGR_RUNNING)
    {
        PhaseLostHandler();

        SR_INPUTCAPTUREPLAUSIBILITYHANDLER();

        Check_Avg_Frequency();

        // If the arrival here comes from the state where a fault in the sensor
        // was detected, then go to post run state.
        temp1 = CheckHwFaultStatus();

        if(temp1) // If any error has been found, then go to post run - the motor will not start!
        {
            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,ELECTRONIC_FAIL_TIMEOUT); //Load time out
        }
        else
        {
            temp = (sint32)SR_Motor_Flags;
            temp = BIT_TEST(temp, SR_FLAG_REQUESTED_TO_STOP);
            if(temp)
            {
                SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_POSTRUN_TESTS);
            }
        }
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// The state performs tests after running and before setting the unlock door speed flag,    ///
    /// the goal is check the harness and hardware before setting the unlock door speed flag     ///
    /// The motor in state is still considered as running, this state machine might go back to   ///
    /// the running state on a new Prepare2Start motor. The goal is to prevent a dead lock when  ///
    /// verifying any condition after running the motor.
    else if(temp == SAFETY_MNGR_POSTRUN_TESTS)
    {

        SR_MOTORSAFETY_MGR_POSTRUN_ACTION();

        SR_MOTORMOVINGHANDLER();                    // Invoke the Unlock Door Speed handler

        // if the sensors could not be checked, send
        // the machine to the waiting state, and keep 
        // seeing for a while if there is no current 
        // flowing through the shunt resistors.
#ifdef SR_BPM_MOTOR

        temp = (sint32)SR_Motor_Flags;
        temp = BIT_TEST(temp, SR_FLAG_UNLOCK_DOOR_SPEED_CHECK_REQUESTED);

#endif

#ifdef SR_CIM_MOTOR

        temp = (sint32)SR_Motor_Flags;
        temp = BIT_TEST(temp, SR_FLAG_UNLOCK_DOOR_SPEED);
#endif

        temp1 = (sint32)SR_Motor_Fault;
        temp1 = BIT_TEST(temp1, SR_FAULT_UNLOCK_DOOR_SPEED_FAILED);


        if(temp1)
        {
            // Unlock door speed failed
            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,TACHO_TIMEOUT); //Load time out
        }
        else if(temp)
        {
            // Unlock door speed found
            temp = (sint32)SR_Motor_Flags;
            BIT_CLR(temp, SR_FLAG_REQUESTED_TO_START);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

#ifdef SR_BPM_MOTOR
            // Idle for this amount of time before checking hardware integrity
            temp = (sint32)SR_Motor_Flags;
            temp = BIT_TEST(temp, SR_FLAG_FORCE_TO_STOP);
            if(temp)
            {
                // a force to stop was requested --> immediately check hardware integrity
                SR_DATA_MCU_UPDATE_LONG(SR_Check_Unlock_Door_Speed_Timer, 0);
            }
            else
            {   // standard stop request --> wait some time before checking hardware integrity
                SR_DATA_MCU_UPDATE_LONG(SR_Check_Unlock_Door_Speed_Timer, CHECK_UNLOCK_DOOR_SPEED_IDLING_BEFORE_CHECK_COUNT);
            }
#endif

            Init_Avg_Frequency();

            // Go idle
            SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_IDLE);
        }

    }


    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // All tests before running the motor has to be performed within this state
    else if(temp == SAFETY_MNGR_PRERUN_TESTS)
    {
        SRMCPwm__TurnOnLowSideGates();          // As in the idle state, the IGBTs are switched off
        // then the inverter bootstrap capacitors need to be
        // charged. The time is provided by the longest
        // peripheral integrity check, in this case, the
        // SRMCPwm Diagnostic handler.

        temp = (sint32)SR_Motor_Flags;
        temp = BIT_TEST(temp, SR_FLAG_REQUESTED_TO_START);
        if(temp)
        {
            // Clear the request to start bit.
            temp = (sint32)SR_Motor_Flags;
            BIT_CLR(temp, SR_FLAG_REQUESTED_TO_START);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

            // Clear the "prerun tests finished" bit.
            // This bit shall be set by finishing the plausibility check
            // of the hardware. The plausibility check tests are
            // implemented in the SRMotorSafetyMgr__MCPwmHandler()
            temp = (sint32)SR_Motor_Flags;
            BIT_CLR(temp, SR_FLAG_PRERUN_TESTS_FINISHED);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);


#ifdef SR_CIM_MOTOR
            // If the InputCapture is disabled, enable it
            if (SRMCInputCapture__GetStatus() != ENABLED)
            {
                SRMCInputCapture__EnableCapture();
            }

            SRMCInputCapture__InitDiagnostic();
#endif

            SRMCAtod__EnableDiagnostic(SR_MCATOD_PLAUSIBILITY_CHECK_THRESHOLD);
            SRMCPwm__EnableDiagnostic(SD_PIN_ZERO_TIME, SD_PIN_ONE_TIME);
        }

        // Check if the plausibility chech has been already performed
        temp = (sint32)SR_Motor_Flags;
        temp = BIT_TEST(temp, SR_FLAG_PRERUN_TESTS_FINISHED);
        if(temp)
        {

            SR_INITMOTORMOVINGHANDLER();       // Initialize the zero speed handler

            Init_Avg_Frequency();

            // Check if a request to stop has arrived when performing the pre run tests.
            // There are handler in conditional states, these flags prevents leaving
            // pre run state without accomplishing those tests.
            temp = (sint32)SR_Motor_Flags;
            temp = BIT_TEST(temp, SR_FLAG_REQUESTED_TO_STOP);
            if(temp)
            {
                SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_POSTRUN_TESTS);
            }
            else
            {
                // Go to running state and allow the pwm access for the class A software.
                SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_RUNNING);
            }

        }

        // If the arrival here comes from the state where a fault in the sensor
        // was detected, then go to error state.
        temp1 = CheckHwFaultStatus();

        if(temp1) // If any error has been found, then go to error - the motor will not start!
        {
            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,ELECTRONIC_FAIL_TIMEOUT); //Load time out
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // When the state machine is able to accept requests to run the pre run tests,
    // shall stay in this IDLE state
    else if(temp == SAFETY_MNGR_IDLE)
    {
        //! Turning both low and high side igbt off does not cause bee noise when motor stopped
        SRMCPwm__TurnOffAllGates();

        AverageSensorsOffset();                 // Average the offsets and update the analog data

        temp = (sint32)SR_Motor_Flags;
        temp = BIT_TEST(temp, SR_FLAG_REQUESTED_TO_START);
        if(temp)
        {
            // Clear the request to force stop
            temp = SR_Motor_Flags;
            BIT_CLR(temp, SR_FLAG_FORCE_TO_STOP);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

            // Cancel the request to check the flags
            temp = (uint32)SR_Motor_Flags;
            BIT_CLR(temp, SR_FLAG_UNLOCK_DOOR_SPEED_CHECK_REQUESTED);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

            SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_PRERUN_TESTS);

            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Fault_Holder,0);
        }

#ifdef SR_BPM_MOTOR

        SR_MOTOR_SAFETY_MGR__AVG_FREQUENCY_BELOW_THREHOLD();

        temp = (sint32)SR_Motor_Flags;
        temp = BIT_TEST(temp, SR_FLAG_UNLOCK_DOOR_SPEED_CHECK_REQUESTED);

        temp1 = (sint32)SR_Motor_Flags;
        temp1 = BIT_TEST(temp1, SR_FLAG_UNLOCK_DOOR_SPEED);

        if( (temp)    &&                            // if requested to check hardware integrity AND
            (!temp1) )                              // if unlock door speed was not yet checked...
        {
            // SR_Check_Unlock_Door_Speed_Timer is used to count the requested amount of idling time
            temp2 = SR_Check_Unlock_Door_Speed_Timer;
            if(temp2)
            {
                temp2--;
                SR_DATA_MCU_UPDATE_LONG(SR_Check_Unlock_Door_Speed_Timer, temp2);
            }
            else
            {
            #ifndef DISABLE_INVERTER_CHECK
                // Cancel the request to check the flags
                temp = (uint32)SR_Motor_Flags;
                BIT_CLR(temp, SR_FLAG_UNLOCK_DOOR_SPEED_CHECK_REQUESTED);
                SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

                // Load a timeout to detect hardware integrity
                SR_DATA_MCU_UPDATE_LONG(SR_Check_Unlock_Door_Speed_Timer, CHECK_UNLOCK_DOOR_SPEED_TIMEOUT_COUNTER);

                // Zero the delta voltage step
                SR_DATA_MCU_UPDATE_LONG(SR_Check_Unlock_Door_Speed_Delta_Voltage, 0);

                // Enable lowers on to charge the boot strap capacitors
                // the charging time is 5 ms.
                SRMCPwm__TurnOnLowSideGates();

                // Move state machine to perform the final check of the unlock door requirement
                SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State, SAFETY_MNGR_UNLOCK_DOOR_CHECK);
            #else
                // Reset the unlock door speed flag
                temp = SR_Motor_Flags;
                BIT_SET(temp, SR_FLAG_UNLOCK_DOOR_SPEED);
                SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

                Init_Avg_Frequency();

           #endif
            }
        }
#endif
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // after idling for some time, the integrity of the hardware shall be checked once,
    // once the check is over, the state machine shall get back to IDLE
    else if(temp == SAFETY_MNGR_UNLOCK_DOOR_CHECK)
    {
#ifdef SR_BPM_MOTOR

        // SR_Check_Unlock_Door_Speed_Timer is used as timeout for the algorithm
        temp2 = SR_Check_Unlock_Door_Speed_Timer;
        if(temp2)
        {
            temp2--;
            SR_DATA_MCU_UPDATE_LONG(SR_Check_Unlock_Door_Speed_Timer, temp2);

            // Inject some voltage and look for current to check hardware integrity
            temp1 = SR_Check_Unlock_Door_Speed_Delta_Voltage;
            if(temp1 < CHECK_UNLOCK_DOOR_MAX_VOLTAGE_COUNTS)
            {
                temp1 += CHECK_UNLOCK_DOOR_MAX_VOLTAGE_DUTY_STEP_COUNTS;
                SR_DATA_MCU_UPDATE_LONG(SR_Check_Unlock_Door_Speed_Delta_Voltage, temp1);
            }

            SRMCPwm__LoadAllPwmValues((16384L + temp1), 16384L, (16384L - temp1));

            // Load currents to the stack
            temp_current_a = SR_CurrentA;
            temp_current_b = SR_CurrentB;
            temp_current_c = SR_CurrentC;

            // One current will be positive, other negative and the third one shall be zero;
            // the zero one has a larger zone since it shall prevent hardware noise
            if( (temp_current_a <    -CHECK_UNLOCK_DOOR_CURRENT_THRESHOLDS_COUNTS)  &&
                (temp_current_b > -(2*CHECK_UNLOCK_DOOR_CURRENT_THRESHOLDS_COUNTS)) &&
                (temp_current_b <  (2*CHECK_UNLOCK_DOOR_CURRENT_THRESHOLDS_COUNTS)) &&
                (temp_current_c >     CHECK_UNLOCK_DOOR_CURRENT_THRESHOLDS_COUNTS)  )
            {
                // Reset the unlock door speed flag
                temp = SR_Motor_Flags;
                BIT_SET(temp, SR_FLAG_UNLOCK_DOOR_SPEED);
                SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

                Init_Avg_Frequency();

                SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_IDLE);
            }
        }
        else // unlock door speed check time is over!
        {
            // Unable to detect current on time, so, it has failed
            SetFault(SR_FAULT_UNLOCK_DOOR_SPEED_FAILED);

            Init_Avg_Frequency();

            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry, TACHO_TIMEOUT); //Load time out
        }

#endif

#ifdef SR_CIM_MOTOR

        // State required only for BPM, if the sw falls here, the
        // state variable is corrupted.
        SRDATA_CORRUPTION_EXCEPTION(10);

#endif
    }




    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize the variables for this state machine and waiting until the power up is performed
    else if(temp == SAFETY_MNGR_INITIALIZE)
    {

        AverageSensorsOffset();                 // Average the offsets and update the analog data

        PowerUpSensors();                       // Check and set a flag accordingly if the power up condition is already finished

        // Check if the powering up stage has been completed
        temp = (sint32)SR_Motor_Flags;
        temp1 = BIT_TEST(temp, SR_FLAG_MICRO_POWERING_UP);

        if(!temp1)
        {
            SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_POSTRUN_TESTS);
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // If an error is found, the state machine shall stay in this ERROR state
    else if(temp == SAFETY_MNGR_ERROR_FOUND)
    {

        temp2 = (uint32)SR_Motor_Fault;
        temp2  = BIT_TEST(temp2, SR_FAULT_OVER_CURRENT);
        if(!temp2) // If no overcurrent failure has been found, then apply the pwm error reaction, otherwise turn off Pwm
        {
            OverVoltageProtection();
        }
        else
        {
            SRMCPwm__TurnOffAllGates();
        }

        temp2 = (uint32)SR_Motor_Fault;
        temp2  = BIT_TEST(temp2, SR_FAULT_OVER_LOAD);

        if(!temp2)
        {
            // no overload condition
            temp2 = (uint32)SR_Timeout_Failure_Retry;

            if(!temp2)//timeout to zero
            {
                SR_DATA_MCU_UPDATE_SHORT(SR_Phase_Lost_Counter,0);

                // Clear all Sr Fault except the overload one
                SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Fault,0);

                // Go postrun test to restart the motor
                SR_INITMOTORMOVINGHANDLER();
                SR_DATA_MCU_UPDATE_BYTE(SR_SafetyMngr_State,SAFETY_MNGR_POSTRUN_TESTS);

            }
            else //reduce time
            {
                temp2--;
                SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,temp2);
            }
        }
    }


    SRMOTORSAFETYMGR_5MS_FLOW_HANDLER_END();
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Perform the motor over load verification
 */
void SRMotorSafetyMgr__SlowHandler(void)
{
    uint16 temp;
    uint16 temp2;

    SRMOTORSAFETYMGR_25MS_FLOW_HANDLER_BEGIN();

    SR_DATA_MCU_CHECK_BYTE(SR_SafetyMngr_State);

    temp = (uint16)SR_SafetyMngr_State;

    temp2 = SR_Motor_Flags;
    temp2 = BIT_TEST(temp2, SR_FLAG_MC_PARAMETERS_LOADED);

    if( (temp > SAFETY_MNGR_INITIALIZE) &&          // overload temperature initialized
        (temp2) )                                   // and ensure parameters are loaded
    {
        CheckSafetyParameters();

        CheckVariables4Failure_25ms();

        OverLoadProtection();
    }

    SRMOTORSAFETYMGR_25MS_FLOW_HANDLER_END();
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Return if there is any exception on the motor control
 *  @return     TRUE (fault found) or FALSE (no fault found)
 */
uint8 SRMotorSafetyMgr__AnyFaultFound(void)
{
    uint8 response;
    uint16 temp;

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault);

    response = TRUE;

    temp = SR_Motor_Fault;

    if(!temp)
    {
        response = FALSE;
    }

    return(response);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get if a single fault has happened.
 *  @Param      request_this_fault gives which fault should be verified.
 *              Check in the definition of SR_MOTOR_FAULT_LIST_TYPE
 *  @return     TRUE: given fault found or FALSE: given fault not found
 */
uint8 SRMotorSafetyMgr__GetFault(SR_MOTOR_FAULT_LIST_TYPE request_this_fault)
{
    uint8 response = TRUE;
    uint16 temp;

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault);

    temp = SR_Motor_Fault;
    temp = BIT_TEST(temp, (SR_MOTOR_FAULT_LIST_TYPE)request_this_fault);

    // Return the status of the tested fault.
    if(temp)
    {
        response = TRUE;
    }
    else
    {
        response = FALSE;
    }

    return(response);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Return if there any error below the MCI.
 * @return  FALSE - No error detected or the error are already recovered
 *          TRUE - There is a error somewhere.
 */
SR_MOTOR_FAULT_LIST_TYPE SRMotorSafetyMgr__GetListedError(void)
{
    uint16 temp;
    SR_MOTOR_FAULT_LIST_TYPE response;

    response = (SR_MOTOR_FAULT_LIST_TYPE)0xFF;     // By default, set all error in the flag.

    temp = (uint16)SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault);
    if(temp == SRDATA_OK)                       // If the fault variable is intact...
    {
        response = (SR_MOTOR_FAULT_LIST_TYPE) SR_Motor_Fault;              // update the feedback variable
    }

    return(response);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get if a single fault has happened and kept in the holder register
 *
 *  @Param      request_this_fault gives which fault should be verified.
 *              Check in the definition of SR_MOTOR_FAULT_LIST_TYPE
 *  @return     TRUE: given fault found or FALSE: given fault not found
 */
uint8 SRMotorSafetyMgr__GetFaultHolder(SR_MOTOR_FAULT_LIST_TYPE request_this_fault)
{
    uint8 response;
    uint16 temp_u16_holder;

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault_Holder);

    temp_u16_holder = SR_Motor_Fault_Holder;
    temp_u16_holder = BIT_TEST(temp_u16_holder, (SR_MOTOR_FAULT_LIST_TYPE)request_this_fault);

    response = TRUE;                                // By default, error is true.
    if(temp_u16_holder == FALSE)                    // check if correlated bit is reset
    {
        response = FALSE;                           // then return error is false.
    }

    return(response);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get if a single fault has happened.
 *  @Param      request_this_fault gives which fault should be verified.
 *              Check in the definition of SR_MOTOR_FLAGS_LIST_TYPE
 *  @return     TRUE: given fault found or FALSE: given fault not found
 */
uint8 SRMotorSafetyMgr__GetFlag(SR_MOTOR_FLAGS_LIST_TYPE request_this_flag)
{
    uint16 temp;
    uint8 response = FALSE;

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);

    temp = SR_Motor_Flags;
    temp = BIT_TEST(temp, (SR_MOTOR_FLAGS_LIST_TYPE)request_this_flag);

    // Return the status of the tested flag.
    if(temp)
    {
        response = TRUE;
    }
    else
    {
        response = FALSE;
    }

    return(response);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Feeds back if the state machine is in IDLe state.
 *              being controller by a Class A software.
 *  @param      none
 *  @return     TRUE: is in IDLE / FALSE: is at any other state
 */
uint8 SRMotorSafetyMgr__IsInIdle(void)
{
    uint8 temp;
    uint16 temp16;

    SR_DATA_MCU_CHECK_BYTE(SR_SafetyMngr_State);
    SR_DATA_MCU_CHECK_BYTE(SR_Motor_Flags);

    temp = (uint8)SR_SafetyMngr_State;
    temp16 = SR_Motor_Flags;

    if((temp == SAFETY_MNGR_IDLE) &&
       (BIT_TEST(temp16, SR_FLAG_REQUESTED_TO_START) == 0))
    {
        temp = TRUE;
    }
    else
    {
        temp = FALSE;
    }

    return(temp);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Warn the Safety Manager the motor is going to start - this routine shall
 * trigger the class b plausibility check BEFORE starting the motor.
 * @return  TRUE accepted request
 *          FALSE denied request
 */
uint8 SRMotorSafetyMgr__Prepare2StartMotor(void)
{
    uint16 temp;
    uint16 temp2;

    SR_DATA_MCU_CHECK_BYTE(SR_SafetyMngr_State);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);

    temp2 = SR_Motor_Flags;
    temp2 = BIT_TEST(temp2, SR_FLAG_MC_PARAMETERS_LOADED);

    temp = (uint16)SR_SafetyMngr_State;
    if( (temp == SAFETY_MNGR_IDLE)  &&
        (temp2) )
    {
        temp = SR_Motor_Flags;
        BIT_SET(temp, SR_FLAG_REQUESTED_TO_START);
        SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

        temp = SR_Motor_Flags;
        BIT_CLR(temp, SR_FLAG_REQUESTED_TO_STOP);
        SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

        temp = TRUE;
    }
    else
    {
        temp = FALSE;
    }


    return((uint8)temp);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Warn the Safety Manager the motor is going to stop soon - this routine shall
 * trigger the class b zero speed measurements.
 * @return  TRUE accepted request
 *          FALSE denied request
 */
uint8 SRMotorSafetyMgr__Prepare2StopMotor(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint16 temp;
    uint16 temp_flags;

    SR_DATA_MCU_CHECK_BYTE(SR_SafetyMngr_State);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);

    temp  = (uint16)SR_SafetyMngr_State;

    if( (temp == SAFETY_MNGR_RUNNING) ||
        (temp == SAFETY_MNGR_PRERUN_TESTS))
    {
        // From this moment on, the class B will
        // take over on the control of the pwm access

        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        // Set the request to stop
        temp_flags = SR_Motor_Flags;
        BIT_SET(temp_flags, SR_FLAG_REQUESTED_TO_STOP);
        SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp_flags);

        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

        temp = TRUE;
    }
    else
    {
        temp = FALSE;
    }

    return((uint8)temp);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Force the motor to stop in a safe way, regardless if the motor is
 *              being controller by a Class A software.
 *  @param      none
 *  @return     TRUE: accepted / FALSE: not accepted
 */
uint8 SRMotorSafetyMgr__ForceStop(void)
{
    uint16 temp;

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);

    // Set the request to force stop
    temp = SR_Motor_Flags;
    BIT_SET(temp, SR_FLAG_FORCE_TO_STOP);
    SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

    temp = SRMotorSafetyMgr__Prepare2StopMotor();

    return((uint8)temp);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Update Pwm variables - class A part
 *  @param      pwm values
 *
 */
void SRMotorSafetyMgr__UpdatePwm(sint32 duty_a, sint32 duty_b, sint32 duty_c)
{
    uint16 temp;

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);

    temp = SR_Motor_Flags;
    temp = BIT_TEST(temp, SR_FLAG_CLASS_A_PWM_ACCESS);
    if(temp) // if the class A has access to the pwm peripheral
    {
        SRMCPWM__LOADALLPWMVALUES(duty_a, duty_b, duty_c);
    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set protection active flag and output pattern. If a pattern which is not defined
 *              is passed as parameter, it will force the state machine to error state.
 *  @param      OUTPUT_PATTERN_TYPE pattern
 *  @return     TRUE: request accepted / FALSE: request denied
 */
uint8 SRMotorSafetyMgr__ConfigPwmPattern(OUTPUT_PATTERN_TYPE pattern)
{
    uint16 temp;

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);

    temp = SR_Motor_Flags;
    temp = BIT_TEST(temp, SR_FLAG_CLASS_A_PWM_ACCESS);
    if(temp)    // if the class A has access to the pwm peripheral
    {
        // The write the pattern asked by the class A to the pwm peripheral
        if(pattern == OUTPUT_PATTERN_LOWERS_ON)
        {
            temp = TRUE;

            SRMCPwm__TurnOnLowSideGates();
        }
        else if(pattern == OUTPUT_PATTERN_ALL_DEV_OFF)
        {
            temp = TRUE;
            //! Turning only low side igbt off helps on the brownout management. Turn high side off does not help.
            SRMCPwm__TurnOffAllGates();
        }
    }

    return(temp);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Variable PWM Frequency: update the PWM frequency.

 * @param[in]   pwm_period_perc The percentage of the nominal frequency\period we want to apply in the next pwm cycle.
 */
void SRMotorSafetyMgr__UpdatePwmPeriod(uint32 pwm_period_perc)
{
    SRMCPwm__UpdatePeriod(pwm_period_perc);
}

//---------------------------------------------------------------------------------------------------------------------
float SRMotorSafetyMgr__SR_SquaredCurrentRmsA()
{
    float data;

    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsA);
    data = (SR_SquaredCurrentRmsA);
    return(data);
}

//---------------------------------------------------------------------------------------------------------------------
float SRMotorSafetyMgr__SR_SquaredCurrentRmsB()
{
    float data;

    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsB);
    data = (SR_SquaredCurrentRmsB);
    return(data);
}

//---------------------------------------------------------------------------------------------------------------------
float SRMotorSafetyMgr__SR_SquaredCurrentRmsC()
{
    float data;

    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsC);
    data = SR_SquaredCurrentRmsC;
    return(data);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Write the address of SRMotorSafety parameters.
 *  @details
 *
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */

BOOL_TYPE SRMotorSafetyMgr__SetParams(void *params_address)
{
    BOOL_TYPE ret_val = FALSE;
    uint16 temp;

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);

    if (params_address != NULL)
    {
        SR_MotorSafety_Params = (MOTOR_SAFETY_PARAMS_TYPE *)params_address;

        temp = CheckSafetyParameters();
        if(temp == SRDATA_OK)
        {
            temp = SR_Motor_Flags;
            BIT_SET(temp, SR_FLAG_MC_PARAMETERS_LOADED);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

            InitOverLoadProtection();

            ret_val = TRUE;
        }
        else
        {
            temp = SR_Motor_Flags;
            BIT_CLR(temp, SR_FLAG_MC_PARAMETERS_LOADED);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);
            ret_val = FALSE;
        }
    }
    return ret_val;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the calculated fundamental frequency of voltage
 *  @param      none
 *  @return     [Hz] float - Class A interface - fundamental voltage
 */
float SRMotorSafetyMgr__GetVoltageFrequency(void)
{
    SR_DATA_MCU_CHECK_FLOAT(SR_Voltage_Frequency);

    return(SR_Voltage_Frequency);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the squared currents
 *  @param      none
 *  @return     [A] float - Class A interface - squared current sum of all 3 currents
 */
float SRMotorSafetyMgr__GetSquaredCurrents(void)
{
    SR_DATA_MCU_CHECK_FLOAT(SR_Squared_Current);

    return(SR_Squared_Current);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the class B motor temperature (thermal model)
 *  @param      none
 *  @return     [deg C] float - Class A interface - motor temperature - calculated from thermal model
 */
float SRMotorSafetyMgr__GetMotorTemperature(void)
{
    SR_DATA_MCU_CHECK_FLOAT(SR_Overload_Temp);

    return(SR_Overload_Temp);
}




//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 *  @brief      Handler the unlock door speed (or rotor not moving) detection sub module. This shall be a part of the
 *              whole class B protection. It uses two flags, one is to detect when the speed is really zero and,
 *              when there is no movement in at the motor shaft. For safety issues, the zero speed detection
 *              shall be used as flag for lid opening. There is also a flag which warns that the unlock door speed
 *              was not able to be detected, this is not a fault and will not prevent the motor to start over.
 *              This error in the unlock door speed flag shall be used in the door lock routine.
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
uint8 CheckSafetyParameters(void)
{
    sint16 check = SRDATA_OK;
    uint16 crc;

    crc = Crc16__RangeProcess((uint8 *)&(SR_MotorSafety_Params->OverLoad_Temp_Init),0xFFFF,(SAFETY_MOTOR_PARAMETERS_NUM-2)); //lint !e928

    if( crc !=  SR_MotorSafety_Params->SrCrc)
    {
        SRDATA_CORRUPTION_EXCEPTION(DATA_CORRUPTION_SAFETY_PARAMS_SECTION);
        check = 1;
    }

    return( (uint8)check);
}




/**
 *  @brief      It tests the integrity of each variable used in this Class B module. It performs an OR
 *              operation between all used variable.
 *  @return     TRUE: at least one variable is corrupted
 *              FALSE: all variables are ok (not corrupted)
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void CheckAnalogVariables4Failure(void)
{
    sint16 check = SRDATA_OK;

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    SR_MCU_MOTOR_CHECK_SHORT(check,SR_CurrentA);
    SR_MCU_MOTOR_CHECK_SHORT(check,SR_CurrentB);
    SR_MCU_MOTOR_CHECK_SHORT(check,SR_CurrentC);
    SR_MCU_MOTOR_CHECK_SHORT(check,SR_CurrentOffsetA);
    SR_MCU_MOTOR_CHECK_SHORT(check,SR_CurrentOffsetB);
    SR_MCU_MOTOR_CHECK_SHORT(check,SR_CurrentOffsetC);
    SR_MCU_MOTOR_CHECK_SHORT(check,SR_DCBus);

    if(check)
    {
        SRDATA_CORRUPTION_EXCEPTION(DATA_CORRUPTION_ANALOG_VAR_SECTION);
    }

    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
}






/**
 *  @brief      It tests the integrity of each variable used in this Class B module. It performs an OR
 *              operation between all used variable.
 *  @return     TRUE: at least one variable is corrupted
 *              FALSE: all variables are ok (not corrupted)
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void CheckVariables4Failure_Pwm(void)
{
    sint16 check = SRDATA_OK;

    SR_MCU_MOTOR_CHECK_BYTE(check, SR_SafetyMngr_State);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_Motor_Flags);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_Motor_Fault);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_Raw_CurrentA);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_Raw_CurrentB);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_Raw_CurrentC);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_DCBus);
#ifdef SR_THIRD_CURRENT_RECONSTRUCTION
    SR_MCU_MOTOR_CHECK_SHORT(check,SR_Sector);
#endif

    if(check)
    {
        SRDATA_CORRUPTION_EXCEPTION(DATA_CORRUPTION_PWM_VAR_SECTION);
    }
}


/**
 *  @brief      It tests the integrity of each variable used in this Class B module. It performs an OR
 *              operation between all used variable.
 *  @return     TRUE: at least one variable is corrupted
 *              FALSE: all variables are ok (not corrupted)
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void CheckVariables4Failure_250us(void)
{
    SR_DATA_MCU_CHECK_BYTE(SR_SafetyMngr_State);

    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault_Holder);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentA);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentB);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentC);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentOffsetA);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentOffsetB);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentOffsetC);
    SR_DATA_MCU_CHECK_SHORT(SR_DCBus);
    SR_DATA_MCU_CHECK_SHORT(SR_Raw_CurrentA);
    SR_DATA_MCU_CHECK_SHORT(SR_Raw_CurrentB);
    SR_DATA_MCU_CHECK_SHORT(SR_Raw_CurrentC);

    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsA);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsB);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsC);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentSumA);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentSumB);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentSumC);
    SR_DATA_MCU_CHECK_LONG(SR_Curr_Rms_Counter);
}


/**
 *  @brief      It tests the integrity of each variable used in this Class B module. It performs an OR
 *              operation between all used variable.
 *  @return     TRUE: at least one variable is corrupted
 *              FALSE: all variables are ok (not corrupted)
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void CheckVariables4Failure_5ms(void)
{
    SR_DATA_MCU_CHECK_BYTE(SR_SafetyMngr_State);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault_Holder);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsA);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsB);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsC);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentsSum);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentA);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentB);
    SR_DATA_MCU_CHECK_SHORT(SR_CurrentC);
    SR_DATA_MCU_CHECK_SHORT(SR_Phase_Lost_Counter);
    SR_DATA_MCU_CHECK_LONG(SR_CalibrationCntr);
    SR_DATA_MCU_CHECK_LONG(SR_Timeout_Failure_Retry);

#ifdef SR_CIM_MOTOR
    SR_DATA_MCU_CHECK_LONG(SR_VerifyTargetCounter);
#else
    SR_DATA_MCU_CHECK_LONG(SR_ZeroSpeedDebouncer);
    SR_DATA_MCU_CHECK_LONG(SR_ZeroSpeedWithCurrTimer);
    SR_DATA_MCU_CHECK_LONG(SR_Check_Unlock_Door_Speed_Timer);
    SR_DATA_MCU_CHECK_LONG(SR_Check_Unlock_Door_Speed_Delta_Voltage);
#endif

}



/**
 *  @brief      It tests the integrity of each variable used in this Class B module. It performs an OR
 *              operation between all used variable.
 *  @return     TRUE: at least one variable is corrupted
 *              FALSE: all variables are ok (not corrupted)
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void CheckVariables4Failure_25ms(void)
{
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Fault_Holder);
    SR_DATA_MCU_CHECK_SHORT(SR_Motor_Flags);
    SR_DATA_MCU_CHECK_SHORT(SR_Overload_Counter);
    SR_DATA_MCU_CHECK_FLOAT(SR_Overload_Temp);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsA);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsB);
    SR_DATA_MCU_CHECK_LONG(SR_SquaredCurrentRmsC);
}




/**
 *  @brief      Calculate the difference between the instantaneous adc value with the
 *              average (if applicable) and save this values in SR variables. rate: 250 us
 *  @return     none
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void CalculateRawAnalogData_250us(void)
{
    sint16 raw_ia;
    sint16 raw_ib;
    sint16 raw_ic;

    raw_ia = SR_Raw_CurrentA - SR_CurrentOffsetA;   // Subtract zero offset
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentA, raw_ia);

    raw_ib = SR_Raw_CurrentB - SR_CurrentOffsetB;   // Subtract zero offset
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentB, raw_ib);

    raw_ic = SR_Raw_CurrentC - SR_CurrentOffsetC;   // Subtract zero offset
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentC, raw_ic);
}

/**
 *  @brief      Checks for motor currents plausibility. If one of the three currents
 *              is found out of the reading range, a fault is triggered.
 *  @return     none
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void CheckCurrentsPlausibility(void)
{
    if(SR_SafetyMngr_State == SAFETY_MNGR_RUNNING)
    {
        if((SR_Raw_CurrentA < SR_CURRENT_PLAUSIBILITY_LOW_LIM) ||
           (SR_Raw_CurrentA > SR_CURRENT_PLAUSIBILITY_HIGH_LIM))
        {
            SetFault(SR_FAULT_CURRENT_SENSOR_FAILED);
            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry, ELECTRONIC_FAIL_TIMEOUT); //Load time out
        }

        if((SR_Raw_CurrentB < SR_CURRENT_PLAUSIBILITY_LOW_LIM) ||
           (SR_Raw_CurrentB > SR_CURRENT_PLAUSIBILITY_HIGH_LIM))
        {
            SetFault(SR_FAULT_CURRENT_SENSOR_FAILED);
            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry, ELECTRONIC_FAIL_TIMEOUT); //Load time out
        }

        if((SR_Raw_CurrentC < SR_CURRENT_PLAUSIBILITY_LOW_LIM) ||
           (SR_Raw_CurrentC > SR_CURRENT_PLAUSIBILITY_HIGH_LIM))
        {
            SetFault(SR_FAULT_CURRENT_SENSOR_FAILED);
            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry, ELECTRONIC_FAIL_TIMEOUT); //Load time out
        }
    }
}


/**
 *  @brief      Calculate the rms for each phase current
 *  @return     none
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void CalculateRmsAnalogData(void)
{
    sint32 temp_s32_1;
    sint32 temp_s32_2;
    sint32 temp_s32_3;
    sint32 temp_counter_s32;

    /* This block calculate the rms for each phase current*/
    temp_s32_1 = (sint32)SR_CurrentA;
    temp_s32_1 = SR_SquaredCurrentSumA + ((temp_s32_1 * temp_s32_1)>>7);   //desired output format 17.15, input current are 21.11 format, 21.11*21.11 = 10.22 -> 17.15 = 10.22>>7

    temp_s32_2 = (sint32)SR_CurrentB;
    temp_s32_2 = SR_SquaredCurrentSumB + ((temp_s32_2 * temp_s32_2)>>7);   //desired output format 17.15, input current are 21.11 format, 21.11*21.11 = 10.22 -> 17.15 = 10.22>>7

    temp_s32_3 = (sint32)SR_CurrentC;
    temp_s32_3 = SR_SquaredCurrentSumC + ((temp_s32_3 * temp_s32_3)>>7);   //desired output format 17.15, input current are 21.11 format, 21.11*21.11 = 10.22 -> 17.15 = 10.22>>7

    temp_counter_s32 = SR_Curr_Rms_Counter;
    temp_counter_s32++;

    if(temp_counter_s32 > RMS_COUNTER_SAMPLES_AT_TS) // Time elapsed - RMS_SAMPLING_TIME_MS
    {
        /* Average of squared current */
        temp_s32_1 = temp_s32_1 / temp_counter_s32;
        SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentRmsA, temp_s32_1);

        temp_s32_2 = temp_s32_2 / temp_counter_s32;
        SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentRmsB, temp_s32_2);

        temp_s32_3 = temp_s32_3 / temp_counter_s32;
        SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentRmsC, temp_s32_3);

        // Reset of the sum terms
        temp_counter_s32 = 0;
        temp_s32_1 = 0;
        temp_s32_2 = 0;
        temp_s32_3 = 0;
    }

    //Update Sr Data
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentSumA, temp_s32_1);
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentSumB, temp_s32_2);
    SR_DATA_MCU_UPDATE_LONG(SR_SquaredCurrentSumC, temp_s32_3);
    SR_DATA_MCU_UPDATE_LONG(SR_Curr_Rms_Counter, temp_counter_s32);

}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handler of the over current detection (locked rotor and over load).
 *              It uses one single counter for two different periods. The time is counted when some shaft movement
 *              is detected. The sum of the squared current is used in order to compare to a threshold. A debouncer is
 *              implemented in order filter out some noise due to some current threshold crossings. This
 *              current threshold is a sum of the squared value of the currents, it also considers the worst case
 *              for noise in the current sensors. This debouncer implements a counting up and down
 *              according to the comparison to this zero speed current threshold; the debounce is not zeroed after
 *              crossing down the threshold because the "history" of some previous values shall be considered in the
 *              future decisions. There is also a said starting time, which it is used todetect either a locked rotor
 *              or overload. No speed information is used in this method, only sensed current.
 *
 *  @param      none
 *  @return     none.
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void PhaseLostHandler(void)
{
    uint32 tmp32;
    uint16 temp;


    //** phase lost management
    if(SR_SquaredCurrentRmsB < CURRENT_LOW_THLD_A)
    {
        if( (SR_SquaredCurrentRmsC > CURRENT_HIGH_THLD_A) &&
        (SR_SquaredCurrentRmsA > CURRENT_HIGH_THLD_A))// &&
            //(MATHCALC__ABS( SR_SquaredCurrentRmsA - SR_SquaredCurrentRmsC ) )< CURRENT_NOISE)
        {
            tmp32 = (uint32)SR_Motor_Flags;
            BIT_SET(tmp32, SR_FLAG_DETECTION_PHASE_LOST);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,tmp32);
        }
    }
    else if(SR_SquaredCurrentRmsC < CURRENT_LOW_THLD_A)
    {
        if( (SR_SquaredCurrentRmsB > CURRENT_HIGH_THLD_A) &&
            (SR_SquaredCurrentRmsA > CURRENT_HIGH_THLD_A) )//&&
            //(MATHCALC__ABS( SR_SquaredCurrentRmsA - SR_SquaredCurrentRmsB ) )< CURRENT_NOISE)
        {
            tmp32 = (uint32)SR_Motor_Flags;
            BIT_SET(tmp32, SR_FLAG_DETECTION_PHASE_LOST);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,tmp32);
        }
    }
    else if(SR_SquaredCurrentRmsA < CURRENT_LOW_THLD_A)
    {
        if( (SR_SquaredCurrentRmsC > CURRENT_HIGH_THLD_A) &&
            (SR_SquaredCurrentRmsB > CURRENT_HIGH_THLD_A) )//&&
            //(MATHCALC__ABS( SR_SquaredCurrentRmsB - SR_SquaredCurrentRmsC ) )< CURRENT_NOISE)
        {
            tmp32 = (uint32)SR_Motor_Flags;
            BIT_SET(tmp32, SR_FLAG_DETECTION_PHASE_LOST);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,tmp32);
        }
    }
    else
    {
        tmp32 = (uint32)SR_Motor_Flags;
        BIT_CLR(tmp32, SR_FLAG_DETECTION_PHASE_LOST);
        SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,tmp32);
    }

    temp = SR_Motor_Flags;
    temp = BIT_TEST(temp, SR_FLAG_DETECTION_PHASE_LOST);

    if(temp)
    {
        SR_Phase_Lost_Counter++;
        if(SR_Phase_Lost_Counter > PHASE_LOST_COUNTER_THLD)
        {
            SR_Phase_Lost_Counter = PHASE_LOST_COUNTER_THLD;

            SetFault(SR_FAULT_PHASE_LOST);

            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,PHASE_LOST_TIMEOUT); //Load time out
        }
    }
    else
    {
        if(SR_Phase_Lost_Counter>0)
        {
            SR_Phase_Lost_Counter--;
        }
    }

    SR_DATA_MCU_UPDATE_SHORT(SR_Phase_Lost_Counter,SR_Phase_Lost_Counter);

}


/**
 *  @brief      Simplified RC motor thermal model.
 *
 *  @return     none
 */
void RcThermalUpdate(void)
{
    float gt;                                       //overall conductance from motor to environment
    float psteel;                                   //steel heat loss dissipation in the motor
    float qjoule;                                   //Winding joule loss dissipation in the motor
    float qdiss;                                    //overall heat dissipation in the motor (qjoule+qsteel)
    float qnet;                                     //net heat accumulated in the motor Capacitance (qdiss-gt(twind-tamb))
    float dTdt;                                     //time derivative of the temperature
    float temp_delta;                               //difference between motor temperature and enviroment temperature
    float squared_current_f;                        //motor squared current in floating point
    float squared_current_pu;                       //motor squared current in fixed point
    float motor_speed_f;                            //motor speed in floating point


    // delta temperature calculation
    temp_delta = SR_Overload_Temp - SR_MotorSafety_Params->Rc_Thermal.Tamb;


    // Calculation of Motor Thermal Model Input -> Motor current RMS (Ia^2 + Ib^2 + Ic^2)
    squared_current_pu = SR_SquaredCurrentRmsA;
    squared_current_pu += SR_SquaredCurrentRmsB;
    squared_current_pu += SR_SquaredCurrentRmsC;

    // motor square current format transformation: from fixed point to floating
    squared_current_f = SRMC__IQ2REALF(squared_current_pu, BASE_CURRENT*BASE_CURRENT*ONE_THIRD, 15);

#ifdef SR_BPM_MOTOR

    // Check variable integrity
    SR_DATA_MCU_CHECK_BYTE(SR_Avg_Fund_Period_Cycle_Total);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period);

    // motor speed format transformation: from fixed point to floating
    motor_speed_f = (RMS_SAMPLER_FREQUENCY * SR_Avg_Fund_Period_Cycle_Total)/(1.0f + SR_Avg_Fund_Period);  // +1 to prevent zero division

#else //SR_CIM_MOTOR

    motor_speed_f = 0.0f;

#endif

    ///////////////////////Losses being generated in the Motor - Steel + Joulse Losses/////////////////
    //Steel Losses
    psteel = SR_MotorSafety_Params->Rc_Thermal.A1 * motor_speed_f + SR_MotorSafety_Params->Rc_Thermal.A2 * (motor_speed_f*motor_speed_f);

    //Joule losses    - considering variation of resitance with temperature
    qjoule=3*SR_MotorSafety_Params->Rc_Thermal.Rwind*(1.0f+SR_MotorSafety_Params->Rc_Thermal.Alfa_Winding*(temp_delta))*squared_current_f;

    /////////////////////////////heat being dissipated by the motor to the air///////////////////////////////
    //Overall thermal conductance from motor mass to the environment
    if (motor_speed_f > SR_MotorSafety_Params->Rc_Thermal.SSpeed)
    {
        gt = SR_MotorSafety_Params->Rc_Thermal.Gspin; //Spinning  SSpeed is the threshold to switch
    }
    else
    {
        gt = SR_MotorSafety_Params->Rc_Thermal.Gtumb1*(temp_delta) + SR_MotorSafety_Params->Rc_Thermal.Gtumb;  //Tumble
    }


    //Dissipated Heat
    qdiss =  gt*(temp_delta);

    //Net heat stored inside the motor - that generated the temperature variation
    qnet =  qjoule + psteel - qdiss;

    //Temperature derivative with time
    dTdt= qnet*SR_MotorSafety_Params->Rc_Thermal.Cmot_Inv;

    //Update motor temperature
    SR_Overload_Temp = SR_Overload_Temp + dTdt*(SR_MotorSafety_Params->Rc_Thermal.Dt);

    // Check and update variables to communicate to other modules.
    SR_DATA_MCU_CHECK_FLOAT(SR_Squared_Current);
    SR_DATA_MCU_CHECK_FLOAT(SR_Voltage_Frequency);

    SR_DATA_MCU_UPDATE_FLOAT(SR_Squared_Current, squared_current_f);
    SR_DATA_MCU_UPDATE_FLOAT(SR_Voltage_Frequency, motor_speed_f);
}




/**
 *  @brief      Motor overload Protection based on a simplified motor thermal model.
 *
 *  @return     none
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void InitOverLoadProtection(void)
{
    SR_DATA_MCU_UPDATE_SHORT(SR_Overload_Counter,0);             // Reset the overload counter
    SR_DATA_MCU_UPDATE_FLOAT(SR_Overload_Temp,(float)SR_MotorSafety_Params->OverLoad_Temp_Init); // Init the overload temperature

    SR_DATA_MCU_UPDATE_FLOAT(SR_Squared_Current, 0.0f);
    SR_DATA_MCU_UPDATE_FLOAT(SR_Voltage_Frequency, 0.0f);
}




/**
 *  @brief      Motor OverLoad Protection based on a simplified motor thermal model.
 *
 *  @return     none
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void OverLoadProtection(void)
{
    sint16 temp;
    uint16 temp1;

    SR_Overload_Counter++;

    if(SR_Overload_Counter >= ONE_SEC_TICK)
    {
        RcThermalUpdate();

        SR_Overload_Counter = 0;
    }

    temp = (sint16)SR_Overload_Temp;             // Conversion from float to int

    temp1 = SR_Motor_Fault;
    temp1 = BIT_TEST(temp1, SR_FAULT_OVER_LOAD);
    if(temp1) // If overload condition detected
    {
        if(temp < SR_MotorSafety_Params->OverLoad_Temp_Low)
        {
            // Motor temperature is lower than the lower threshold  -> Clear the overload fault
            ClearFault(SR_FAULT_OVER_LOAD);
        }
    }
    else
    {
        if(temp > SR_MotorSafety_Params->OverLoad_Temp_High)
        {
            // Motor temperature is higher than the maximum allowed  -> Set the overload fault
            SetFault(SR_FAULT_OVER_LOAD);
        }
    }


    SR_DATA_MCU_UPDATE_SHORT(SR_Overload_Counter,SR_Overload_Counter);
    SR_DATA_MCU_UPDATE_FLOAT(SR_Overload_Temp,SR_Overload_Temp);
}




/**
 *  @brief      Performs the diagnostic of motor control peripherals (Atod and Pwm)
 *  @return     none
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void PeripheralDiagnostic(void)
{
    uint32 temp;
    uint16 temp_flags;

    if(SR_SafetyMngr_State == SAFETY_MNGR_PRERUN_TESTS)
    {
        // Perform the Shut Down pin plausibility check
        SRMCPwm__DiagnosticHandler();

        // Perfom the ADCs plausibility check
        SRMCAtod__DiagnosticHandler();

        // Read flag and fault to temp variables.
        temp_flags = SR_Motor_Flags;

        // Handle the feedbacks

        // AtoD module
        temp = SRMCAtod__GetDiagnosticFeedback();
        if (temp == SR_MCATOD_ERROR_PERIPHERAL_SAFE)
        {
            BIT_SET(temp_flags, SR_FLAG_ADC_PLAUSIB_CHECK_OK);
        }
        else if(temp == SR_MCATOD_ERROR_PERIPHERAL_NOT_SAFE)
        {
            SetFault(SR_FAULT_CURRENT_SENSOR_FAILED);

            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,ELECTRONIC_FAIL_TIMEOUT); //Load time out
        }

        // PWM module
        temp = SRMCPwm__GetDiagnosticFeedback();
        if (temp == SR_MCPWM_ERROR_PERIPHERAL_SAFE)
        {
            BIT_SET(temp_flags, SR_FLAG_SHUTDOWN_PLAUSIB_CHECK_OK);
        }
        else if (temp == SR_MCPWM_ERROR_PERIPHERAL_NOT_SAFE)
        {
            SetFault(SR_FAULT_PWM_SHUTDOWN_FAILED);

            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,ELECTRONIC_FAIL_TIMEOUT); //Load time out
        }

        // If there is nothing wrong in the plausibility chech of the sensors...
        if( (BIT_TEST(temp_flags, SR_FLAG_ADC_PLAUSIB_CHECK_OK) != 0)       &&
            (BIT_TEST(temp_flags, SR_FLAG_SHUTDOWN_PLAUSIB_CHECK_OK) != 0)  )
        {

            // Check if the signals from the hardware are within the
            // acceptable levels.
            temp = MotorAnalogCircuitCheck();

            if (temp == MOTOR_ANALOG_CHECK_CURR_FAIL)
            {
                SetFault(SR_FAULT_CURRENT_SENSOR_FAILED);

                SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,ELECTRONIC_FAIL_TIMEOUT); //Load time out
            }
            else if (temp == MOTOR_ANALOG_CHECK_VBUS_FAIL)
            {
                SetFault(SR_FAULT_VOLTAGE_SENSOR_FAILED);

                SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,ELECTRONIC_FAIL_TIMEOUT); //Load time out
            }
            else // no hardware circuit error was found so far.
            {
                BIT_SET(temp_flags, SR_FLAG_PRERUN_TESTS_FINISHED);     // Warn the initial tests were performed
            }

            // Reset plausibility checks status - RICUPL
            BIT_CLR(temp_flags, SR_FLAG_ADC_PLAUSIB_CHECK_OK);
            BIT_CLR(temp_flags, SR_FLAG_SHUTDOWN_PLAUSIB_CHECK_OK);
        }

        SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags, temp_flags);
    }
    else if (SR_SafetyMngr_State == SAFETY_MNGR_POSTRUN_TESTS)
    {
        // Perform the Shut Down pin plausibility check
        SRMCPwm__DiagnosticHandler();
    }

}



/**
 *  @brief      Manage the  Class A Pwm Access
 *  @return     none
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void ClassAPwmAccess(void)
{
    // Class A Pwm Access management
    if (SR_SafetyMngr_State == SAFETY_MNGR_RUNNING)
    {
        if(SR_Motor_Fault == 0 && (BIT_TEST(SR_Motor_Flags, SR_FLAG_REQUESTED_TO_STOP) == 0))
        {// No safety fault and request to stop are detected

            // Allow the pwm access by Class A software,
            // the Class A shall start accessing the pwm driver from this moment...
            BIT_SET(SR_Motor_Flags, SR_FLAG_CLASS_A_PWM_ACCESS);
            SR_MCU_MOTOR_UPDATE_N_VAR(SR_Motor_Flags);

        }
        else
        {
            // Class A software is not allowed to access the peripheral
            BIT_CLR(SR_Motor_Flags, SR_FLAG_CLASS_A_PWM_ACCESS);
            SR_MCU_MOTOR_UPDATE_N_VAR(SR_Motor_Flags);

            // The state machine is still in running and a fault is detected,
            // apply the pwm error reaction before the 5 ms handler action
            SR_MOTORSAFETY_MGR_ERROR_REACTION();
        }

    }
    else
    {
        // Class A software is not allowed to access the peripheral
        BIT_CLR(SR_Motor_Flags, SR_FLAG_CLASS_A_PWM_ACCESS);
        SR_MCU_MOTOR_UPDATE_N_VAR(SR_Motor_Flags);
    }
}


/**
 *  @brief      Test if the power up condition for the MCU have already reached.
 *              If the sum of the square of each current is smaller than the following
 *              threshold, then the powering up is done in the current sensoring point
 *              of view. The trick is: when the micro is powering up, the current offset
 *              will take long time to stabilize, due to a low pass filter with a very
 *              low cut off frequency. As for starting, then a current will be reading
 *              (there is no current flowing the shunt resistors), so, when the offset
 *              filter is stabilized, then the read current (i1, i2, i3) will be very
 *              close to zero. It is not zero, since there is some noise in those sensors.
 *  @return     TRUE: power up is already finished
 *              FALSE: power up it not completed yet.
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void PowerUpSensors(void)
{
    sint16 temp;
    uint32 temp1;
    uint32 temp2;

    SR_DATA_MCU_CHECK_SHORT(SR_PowerUpTimer);

    // Just count up a dummy timer, only used
    // to prevent any hardware noise from the hardware power-up
    temp = SR_PowerUpTimer;
    if(temp < SR_POWER_UP_DELAY_TIMER)
    {
        temp++;
        SR_DATA_MCU_UPDATE_SHORT(SR_PowerUpTimer,temp);
    }
    else
    {
        // If the sum of the square current has converged to zero
        // then declare the power up is finished.
        temp1 = SR_SquaredCurrentsSum;
        temp2 = SR_CalibrationCntr;
        if(temp1 < SR_POWERING_UP_CURRENTS_THRESHOLD)
        {
            temp2++;
            SR_DATA_MCU_UPDATE_LONG(SR_CalibrationCntr,temp2);
            if (temp2 > SR_CALIBRATION_CNTR_THRESHOLD)
            {
                SR_DATA_MCU_UPDATE_LONG(SR_CalibrationCntr,0);
                // The currents readings are stabilized, so, transitory is finished!
                temp = (uint16)SR_Motor_Flags;
                BIT_CLR(temp, SR_FLAG_MICRO_POWERING_UP);
                SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,(uint16)temp);
            }
        }
    }

}



/**
 *  @brief      Calculate the average of the sensors offsets
 *  @return     none
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void AverageSensorsOffset(void)
{
    sint32 tempS32_1;
    sint32 tempS32_2;


    CheckAnalogVariables4Failure();
    SR_DATA_MCU_CHECK_LONG(SR_CurrentOffsetHistA);
    SR_DATA_MCU_CHECK_LONG(SR_CurrentOffsetHistB);
    SR_DATA_MCU_CHECK_LONG(SR_CurrentOffsetHistC);


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    tempS32_1 = SR_CurrentOffsetHistA;

    //tempS32_2 = SRMCATOD__GETADC1();                  // Read the converted values...
    tempS32_2 = SR_CurrentA + SR_CurrentOffsetA;

    tempS32_2 -= (tempS32_1 >> 16);                 // Subtract zero offset
    // this make 400ms time constant for 16kHz sampling frequency, i.e. reach final value in 6 x 400ms
    tempS32_1 += tempS32_2 * AVERAGE_CURR_SENSOR_OFFSET_LPF_FACTOR;

    SR_DATA_MCU_UPDATE_LONG(SR_CurrentOffsetHistA,tempS32_1);
    tempS32_1 = tempS32_1 >> 16;
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentOffsetA,(sint16)tempS32_1);


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    tempS32_1 = SR_CurrentOffsetHistB;

    //tempS32_2 = SRMCATOD__GETADC2();                  // Read the converted values...
    tempS32_2 = SR_CurrentB + SR_CurrentOffsetB;

    tempS32_2 -= (tempS32_1 >> 16);                 // Subtract zero offset
    // this make 400ms time constant for 16kHz sampling frequency, i.e. reach final value in 6 x 400ms
    tempS32_1 += tempS32_2 * AVERAGE_CURR_SENSOR_OFFSET_LPF_FACTOR; 
    
    SR_DATA_MCU_UPDATE_LONG(SR_CurrentOffsetHistB,tempS32_1);
    tempS32_1 = tempS32_1 >> 16;
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentOffsetB,(sint16)tempS32_1);


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    tempS32_1 = SR_CurrentOffsetHistC;

    //tempS32_2 = SRMCATOD__GETADC3();                  // Read the converted values...
    tempS32_2 = SR_CurrentC + SR_CurrentOffsetC;

    tempS32_2 -= (tempS32_1 >> 16);                 // Subtract zero offset
    // this make 400ms time constant for 16kHz sampling frequency, i.e. reach final value in 6 x 400ms
    tempS32_1 += tempS32_2 * AVERAGE_CURR_SENSOR_OFFSET_LPF_FACTOR;
    
    SR_DATA_MCU_UPDATE_LONG(SR_CurrentOffsetHistC,tempS32_1);
    tempS32_1 = tempS32_1 >> 16;
    SR_DATA_MCU_UPDATE_SHORT(SR_CurrentOffsetC,(sint16)tempS32_1);
}




/**
 *  @brief      Get the feed about the shut down pin and feeds back
 *              in the case of a failure.
 *  @return     TRUE: fault found; FALSE: nothing wrong
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
uint8 CheckHwFaultStatus(void)
{
    uint8 temp = FALSE;                     // Return variable
    uint8 temp1;                            // temporary variable for SD pin feedback

    temp1 = SRMCPwm__GetHwFaultStatus();            // Gets SD pin status
    if(temp1 == TRUE)                               // In case HW fault detected
    {
        SetFault(SR_FAULT_OVER_CURRENT);

        temp = TRUE;                                // Updates flags in classB and returns failure detected
    }
    return(temp);                                   // Returns check response
}




/**
 *  @brief      Calculate the average of the sensors offsets
 *  @return     none
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
MOTOR_ANALOG_CHECK_TYPE MotorAnalogCircuitCheck(void)
{
    MOTOR_ANALOG_CHECK_TYPE response;
    static sint32 delta_curr_a;
    static sint32 delta_curr_b;
    static sint32 delta_curr_c;
    static sint16 vbus;

    //Check of offset for current of phase A
    delta_curr_a = SR_CurrentOffsetA - CURR_OFFSET_REF;
    if(delta_curr_a<0)
    {
        delta_curr_a = -delta_curr_a;
    }

    //Check of offset for current of phase B
    delta_curr_b = SR_CurrentOffsetB - CURR_OFFSET_REF;
    if(delta_curr_b<0)
    {
        delta_curr_b = -delta_curr_b;
    }

    //Check of offset for current of phase C
    delta_curr_c = SR_CurrentOffsetC - CURR_OFFSET_REF;
    if(delta_curr_c<0)
    {
        delta_curr_c = -delta_curr_c;
    }

    //Check for consistency of vbus reading
    vbus = SR_DCBus;

    if( (delta_curr_a > CURR_OFFSET_THD) ||
        (delta_curr_b > CURR_OFFSET_THD) ||
        (delta_curr_c > CURR_OFFSET_THD))
    {
        response = MOTOR_ANALOG_CHECK_CURR_FAIL;
    }
    else if( (vbus > VBUS_SR_HIGH_LIM) ||
             (vbus < VBUS_SR_LOW_LIM)  )
    {
        response = MOTOR_ANALOG_CHECK_VBUS_FAIL;
    }
    else
    {
        response = MOTOR_ANALOG_CHECK_CORRECT;
    }

    return(response);
}




/**
 *  @brief      Evaluate if the voltage is within allowed range
 *  @return     none
 */
void OverVoltageCheck(void)
{
    sint16 vbus;
    uint16 temp;

    temp = (uint16)SR_SafetyMngr_State;
    if((temp != SAFETY_MNGR_PRERUN_TESTS)&&(temp != SAFETY_MNGR_ERROR_FOUND)) //DC-Bus adc value is not reliable during adc diagnostic execution
    {
        //Check for consistency of vbus reading
        SR_DATA_MCU_CHECK_SHORT(SR_DCBus);
        vbus = SR_DCBus;

        // Powering up stage is completed -> adc signals are reliable
        if(vbus < VBUS_SR_LOW_THR_AD_COUNTS)
        {
            SetFault(SR_FAULT_DCBUS_UNDER_VOLTAGE);

            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,VOLTAGE_ERROR_TIMEOUT); //Load time out
        }
        else if(vbus > VBUS_SR_HIGH_THR_AD_COUNTS)
        {
            SetFault(SR_FAULT_DCBUS_OVER_VOLTAGE);

            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,VOLTAGE_ERROR_TIMEOUT); //Load time out
        }
    }
}




/**
 *  @brief      In the case the dc bus voltage is outside safe levels, take some
 *              action to keep the voltage within safe level.
 *  @oaram      none
 *  @return     none
 */
void OverVoltageProtection(void)
{
    sint16 vbus;
    uint16 temp;

    temp = SR_Motor_Fault;
    temp  = BIT_TEST(temp, SR_FAULT_DCBUS_UNDER_VOLTAGE);
    temp += BIT_TEST(temp, SR_FAULT_DCBUS_OVER_VOLTAGE);

    if(temp)
    {
        //Check for consistency of vbus reading
        SR_DATA_MCU_CHECK_SHORT(SR_DCBus);
        vbus = SR_DCBus;

        // Powering up stage is completed -> adc signals are reliable
        // Dc Voltage out of range fault detected
        if(vbus > VBUS_SR_HIGH_THR_AD_COUNTS)
        {
            SR_MOTORSAFETY_MGR_ERROR_REACTION();
        }
        else if(vbus < VBUS_SR_LOW_THR_AD_COUNTS)
        {
            SRMCPwm__TurnOffAllGates();
        }
    }
    else
    {
        SR_MOTORSAFETY_MGR_ERROR_REACTION();
    }
}

#ifdef SR_CIM_MOTOR

#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void InitMotorMovingHandlerCim(void)
{
    uint16 temp;

    //! To be safe start the motor warning that it is moving.

    // Reset the zero speed flag, i.e., the motor is moving
    temp = SR_Motor_Flags;
    BIT_CLR(temp, SR_FLAG_UNLOCK_DOOR_SPEED);
    SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

    // Set the rotor moving flag, i.e., the motor is moving
    ClearFault(SR_FAULT_UNLOCK_DOOR_SPEED_FAILED);

    // Reset the Tacho zero speed check counter
    SR_DATA_MCU_UPDATE_LONG(SR_VerifyTargetCounter,0);


}

/**
 *  @brief      Handler the unlock door speed (or rotor not moving) detection sub module. This shall be a part of the
 *              whole class B protection. It uses two flags, one is to detect when the speed is really zero and,
 *              when there is no movement in at the motor shaft. For safety issues, the zero speed detection
 *              shall be used as flag for lid opening. There is also a flag which warns that the unlock door speed
 *              was not able to be detected, this is not a fault and will not prevent the motor to start over.
 *              This error in the unlock door speed flag shall be used in the door lock routine.
 */
void MotorMovingHandlerCim(void)
{
    uint32 temp;
#ifndef SKIP_MOTOR_MOVING_CHECK
    if (SRMCInputCapture__GetMovementStatus(SR_UNLOCKDOORSPEED_LOW_COUNTER_THR))
    {
        // motor is moving
        // update the state

        // Clear the unlock door speed flag.
        temp = (uint32)SR_Motor_Flags;
        BIT_CLR(temp, SR_FLAG_UNLOCK_DOOR_SPEED);
        SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,(uint16)temp);

        temp = SR_VerifyTargetCounter;
        if (temp < SR_UNLOCKDOORSPEED_TARGET_NOT_REACHED_TIMEOUT)
        {
            // wait state
            temp++;
            SR_DATA_MCU_UPDATE_LONG(SR_VerifyTargetCounter,temp);
        }
        else
        {
            // timeout state
            // Set the flag that a failure has happened while detecting the unlock door speed flag
            SetFault(SR_FAULT_UNLOCK_DOOR_SPEED_FAILED);
        }
    }
    else
#endif
    {
        // motor is stopped

        // Set the unlock door speed flag.
        temp = (uint32)SR_Motor_Flags;
        BIT_SET(temp, SR_FLAG_UNLOCK_DOOR_SPEED);
        SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,(uint16)temp);
    }
}


#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif


#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void InputCapturePlausibilityHandler(void)
{
    uint16 temp;

    temp = SRMCInputCapture__GetDiagnosticFeedback();

    if (temp == SR_IC_MIN_TACH_ERROR)
    {
        // Set the plausibility fault
        SetFault(SR_FAULT_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED);

        SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,TACHO_TIMEOUT); //Load time out
    }
    else if (temp == SR_IC_MAX_SPEED_ERROR)
    {
        SetFault(SR_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED);

        SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,TACHO_TIMEOUT); //Load time out
    }
    else if ((temp == SR_IC_TEST_NOT_INITIALIZED) || (temp == SR_IC_ERROR_PERIPHERAL_NOT_SAFE))
    {
        // todo: which fault report?
        // Set the plausibility fault
        SetFault(SR_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED);

        SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,TACHO_TIMEOUT); //Load time out
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the latest captured value of the InputCapture driver.
 *  @param      none
 *  @return     Last value of the InputCapture driver. Zero if the IC driver is disabled
 */
unsigned int SRMotorSafetyMgr__GetInputCaptureCounterValue(void)
{
    unsigned int res = 0;
    if (SRMCInputCapture__GetStatus() == INPUTCAPTURE_ENABLED)
    {
        res = SRMCInputCapture__GetCounterValue();
    }

    return res;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get if a Input Capture match event occurred.
 *  @param      none
 *  @return     TRUE: match event occurred; FALSE: no match event occurred.
 */
uint8 SRMotorSafetyMgr__GetInputCaptureMatchEvent(void)
{
    uint8 res = 0;

    if (SRMCInputCapture__GetStatus() == INPUTCAPTURE_ENABLED)
    {
        res = SRMCInputCapture__GetMatchEvent();
    }

    return res;
}
#endif




#ifdef SR_BPM_MOTOR
/**
 *  @brief      Initializes the variable of the unlock door speed algorithm.
 *  @param      None
 *  @return     None
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void InitMotorMovingHandlerBpm(void)
{
    uint16 temp;

    //! To be safe start the motor warning that it is moving.

    // Reset the zero speed flag, i.e., the motor is moving
    temp = SR_Motor_Flags;
    BIT_CLR(temp, SR_FLAG_UNLOCK_DOOR_SPEED);
    SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp);

    SR_DATA_MCU_UPDATE_LONG(SR_ZeroSpeedWithCurrTimer,0);

    SR_DATA_MCU_UPDATE_LONG(SR_ZeroSpeedDebouncer,0);                           // Initialize reseting the zero speed debouncer
}




/**
 *  @brief      Handles the the unlock door speed algorithm.
 *  @details    Call rate: every 5 ms
 *  @param      None
 *  @return     None
 *
 */
void MotorMovingHandlerBpm(void)
{
    uint32 temp;
    uint32 temp1;
    uint32 temp_a;
    uint32 temp_b;

#ifndef SKIP_MOTOR_MOVING_CHECK
    CheckAnalogVariables4Failure(); // Check for corrupted data

    // Count up the timeout to stop the motor
    temp = SR_ZeroSpeedWithCurrTimer;
    if(temp < SR_MAX_LONG_VARIABLE)
    {
        temp++;
        SR_DATA_MCU_UPDATE_LONG(SR_ZeroSpeedWithCurrTimer,temp);
    }

    temp1 = SR_SquaredCurrentsSum;

    //// The block below implements the unlock door speed debouncer
    // if the sum of the squared currents is close to zero
    // and the pwm is performing the "lowers on"
    temp = SR_ZeroSpeedDebouncer;
    if((temp1 < SR_UNLOCKDOORSPEED_CURR_THRES_FIXP)&&(temp < SR_UNLOCKDOORSPEED_DEBOUNCING_COUNTER)) // SRMCpwm driver is changed.
    {   // ZeroSpeedDebouncer does not already reached the desired value
        // If no exception in the hardware has been read (shut down input)
        temp = (uint32)CheckHwFaultStatus();
        if(!temp)
        {
            // Increment the "debouncer" timer
            temp = SR_ZeroSpeedDebouncer;
            if(temp < SR_MAX_LONG_VARIABLE)
            {
                temp++;
                SR_DATA_MCU_UPDATE_LONG(SR_ZeroSpeedDebouncer,temp);
            }

        }
        else // else a hardware exception has happened
        {
            // then zero the "debouncer" timer.
            SR_DATA_MCU_UPDATE_LONG(SR_ZeroSpeedDebouncer,0);

            // Hw faults are detected -> wait 60 s before turn-on again PWM
            SR_DATA_MCU_UPDATE_LONG(SR_Timeout_Failure_Retry,HW_FAULT_POSTRUN_TIMEOUT); //Load time out
        }
    }

    // If the time is longer enough and very short compared to the zero speed detection
    temp = SR_ZeroSpeedDebouncer;

    // Debounce the time for "unlock door speed" flag
    // The "debouncer" time is over, then...
    if(temp >= SR_UNLOCKDOORSPEED_DEBOUNCING_COUNTER)
    {
        // Trigger the shut down pin before setting the unlock door speed flags.
        // Start the Shut down plausibility check
        SRMCPwm__EnableDiagnostic(SD_PIN_ZERO_TIME, SD_PIN_ONE_TIME);

        // Read the SRMCPwm diagnosis feedback
        temp = SRMCPwm__GetDiagnosticFeedback();

        // If the pwm peripheral is unsafe...
        if(temp == SR_MCPWM_ERROR_PERIPHERAL_NOT_SAFE)
        {
            // Set the flag warning the shut down circuit is malfunctioning.
            SetFault(SR_FAULT_PWM_SHUTDOWN_FAILED);

        }
        else if(temp == SR_MCPWM_ERROR_PERIPHERAL_SAFE) // if the shut down is safe
        {
            // Found zero speed with zero vector (1° test passed - zero motor currents), set the unlock door speed check request to execute 2° test (not zero motor currents)
            temp1 = (uint32)SR_Motor_Flags;
            BIT_SET(temp1, SR_FLAG_UNLOCK_DOOR_SPEED_CHECK_REQUESTED);
            SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp1);
        }
    }

    // Get all faults that prevents the unlock door speed be read properly
    temp_a  = SR_Motor_Fault;
    temp_b  = BIT_TEST(temp_a, SR_FAULT_CURRENT_SENSOR_FAILED);
    temp_b += BIT_TEST(temp_a, SR_FAULT_VOLTAGE_SENSOR_FAILED);
    temp_b += BIT_TEST(temp_a, SR_FAULT_PWM_SHUTDOWN_FAILED);

    // If the timeout is expired.
    temp = SR_ZeroSpeedWithCurrTimer;
    if( (temp >= SR_UNLOCKDOORSPEED_FAIL_TIMER_COUNTER) ||
        (temp_b) )
    {
        // Set the flag that a failure has happened while detecting the unlock door speed flag
        SetFault(SR_FAULT_UNLOCK_DOOR_SPEED_FAILED);
    }
#else
    // Mock for BPM
    // motor is stopped
    // Clear the flag that the shut down was corrupted from previous run
    ClearFault(SR_FAULT_PWM_SHUTDOWN_FAILED);

    // Set the zero speed flag.
    temp1 = (uint32)SR_Motor_Flags;
    BIT_SET(temp1, SR_FLAG_UNLOCK_DOOR_SPEED_CHECK_REQUESTED);
    SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Flags,temp1);

    // Clear the flag  that a failure has happened while detection the zero speed
    ClearFault(SR_FAULT_UNLOCK_DOOR_SPEED_FAILED);
#endif
}

#endif






/**
 *  @brief      Initialize the variables used by the frequency measurement module.
 *  @details    Called every motor control state machine starting up
 *  @param      None
 *  @return     None
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Init_Avg_Frequency(void)
{
#ifdef SR_BPM_MOTOR

    sint32 temp_s32;

    SR_DATA_MCU_CHECK_BYTE(SR_Avg_Cross_Event);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period_High_Threshold);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period_Low_Threshold);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Threshold);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period_Time_Counter);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period_Cycle_Counter);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period);
    SR_DATA_MCU_CHECK_LONG(SR_Applied_Duty_A_Sum);
    SR_DATA_MCU_CHECK_BYTE(SR_Avg_Fund_Ignored_First);

    temp_s32 = SRMCPWM__GETMAXPWM();
    temp_s32 = (sint32)(SR_MODULATION_LIMIT * (float)temp_s32);
    temp_s32 = temp_s32>>1;                         //!< Half value of the maximum duty, e.g. 0.5 pu
    SR_Avg_Fund_Period_High_Threshold = (sint32)(((100.0f + AVG_THRESHOLD_HT)/100.0f) * temp_s32);  //!< High threshold hysteresis value;
    SR_Avg_Fund_Period_Low_Threshold =  (sint32)(((100.0f - AVG_THRESHOLD_HT)/100.0f) * temp_s32);  //!< Low threshold hysteresis value

    SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period_High_Threshold, SR_Avg_Fund_Period_High_Threshold);
    SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period_Low_Threshold, SR_Avg_Fund_Period_Low_Threshold);
    SR_DATA_MCU_UPDATE_LONG(SR_Avg_Threshold, SR_Avg_Fund_Period_Low_Threshold);

    SR_DATA_MCU_UPDATE_BYTE(SR_Avg_Cross_Event, FALSE);
    SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period_Time_Counter, 0);
    SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period_Cycle_Counter, SR_MOTOR_SAFETY_MNGR_MAX_PERIOD_COUNTER);
    SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period, SR_MOTOR_SAFETY_MNGR_MAX_PERIOD_COUNTER);
    SR_DATA_MCU_UPDATE_LONG(SR_Applied_Duty_A_Sum, 0);
    SR_DATA_MCU_UPDATE_LONG(SR_Applied_Duty_A_Sum_High, 0);

    SR_DATA_MCU_UPDATE_BYTE(SR_Avg_Fund_Ignored_First, FALSE);

#else // SR_CIM_MOTOR


#endif
}




/**
 *  @brief      Measure the avereage frequency
 *  @details    Call rate every motor control rate (pwm rate)
 *  @param      None
 *  @return     None
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Calc_Avg_Frequency(void)
{
#ifdef SR_BPM_MOTOR

    uint32 temp_u32;
    sint32 temp_s32;
    sint32 temp_s32_1;
    sint32 temp_s32_low;
    sint32 temp_s32_high;

    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period_Time_Counter);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period_Cycle_Counter);
    SR_DATA_MCU_CHECK_LONG(SR_Applied_Duty_A_Sum);
    SR_DATA_MCU_CHECK_LONG(SR_Applied_Duty_A_Sum_High);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Threshold);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period_Low_Threshold);
    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period_High_Threshold);
    SR_DATA_MCU_CHECK_BYTE(SR_Avg_Cross_Event);
    SR_DATA_MCU_CHECK_BYTE(SR_Avg_Fund_Period_Cycle_Total);
    SR_DATA_MCU_CHECK_BYTE(SR_Avg_Fund_Ignored_First);

    temp_u32 = SR_Avg_Fund_Period_Time_Counter;
    if(temp_u32 < SR_MOTOR_SAFETY_MNGR_MAX_PERIOD_COUNTER)
    {
        temp_u32++;
        SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period_Time_Counter, temp_u32);
    }

    temp_s32 = SRMCPWM__GET_PWM_A();

    temp_s32_1 = SR_Applied_Duty_A_Sum;
    SRMC__FILTER_CALC(temp_s32, temp_s32_1, SRMOTOR_SAFETY_AVG_CALC_COFF_FREQ_LOW_SPD);
    SR_DATA_MCU_UPDATE_LONG(SR_Applied_Duty_A_Sum, temp_s32_1);
    temp_s32_low = SRMC__FILTER_GET_FILTERED_VAR(temp_s32_1, SRMOTOR_SAFETY_AVG_CALC_COFF_FREQ_LOW_SPD);

    temp_s32_1 = SR_Applied_Duty_A_Sum_High;
    SRMC__FILTER_CALC(temp_s32, temp_s32_1, SRMOTOR_SAFETY_AVG_CALC_COFF_FREQ_HI_SPD);
    SR_DATA_MCU_UPDATE_LONG(SR_Applied_Duty_A_Sum_High, temp_s32_1);
    temp_s32_high = SRMC__FILTER_GET_FILTERED_VAR(temp_s32_1, SRMOTOR_SAFETY_AVG_CALC_COFF_FREQ_HI_SPD);

    // Select which filtered value will be used in the threshold, there is a small glitch
    // when jumping from filter to other one but the error is small.
    temp_s32 = SR_Avg_Fund_Period;
    if(temp_s32 < SRMOTOR_SAFETY_AVG_HIST_LOW_TO_HIGH_THRES)
    {
        temp_s32 = temp_s32_high;
    }
    else
    {
        temp_s32 = temp_s32_low;
    }

    // Look for complete cycles and count the time accordingly.
    if(temp_s32 > SR_Avg_Threshold)
    {
        SR_DATA_MCU_UPDATE_LONG(SR_Avg_Threshold, SR_Avg_Fund_Period_Low_Threshold);

        if(SR_Avg_Cross_Event == TRUE)
        {
            SR_DATA_MCU_UPDATE_BYTE(SR_Avg_Cross_Event, FALSE);

            temp_u32 = SR_Avg_Fund_Period_Cycle_Counter;
            temp_u32++;
            SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period_Cycle_Counter, temp_u32);

            if(SR_Avg_Fund_Period_Cycle_Counter >= SR_Avg_Fund_Period_Cycle_Total)
            {
            	if(SR_Avg_Fund_Ignored_First == TRUE)
            	{
            		SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period, SR_Avg_Fund_Period_Time_Counter);
            	}
            	else
            	{
            		SR_DATA_MCU_UPDATE_BYTE(SR_Avg_Fund_Ignored_First, TRUE);
            	}

                SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period_Cycle_Counter, 0);
                SR_DATA_MCU_UPDATE_LONG(SR_Avg_Fund_Period_Time_Counter, 0);
            }
        }
    }
    else if(temp_s32 < SR_Avg_Threshold)
    {
        SR_DATA_MCU_UPDATE_LONG(SR_Avg_Threshold, SR_Avg_Fund_Period_High_Threshold);
        SR_DATA_MCU_UPDATE_BYTE(SR_Avg_Cross_Event, TRUE);
    }

#endif
}




/**
 *  @brief      Compare the averaged speed to a threshold.
 *  @details    Called every 5 ms
 *  @param      None
 *  @return     None - calls back the Speed Monitor with TRUE (speed above) or FALSE (speed below) threshold.
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void Check_Avg_Frequency(void)
{
#ifdef SR_BPM_MOTOR

    uint16 temp;

    SR_DATA_MCU_CHECK_LONG(SR_Avg_Fund_Period);


    temp = SR_Motor_Flags;
    temp = BIT_TEST(temp, SR_FLAG_MC_PARAMETERS_LOADED);

    if(temp) // ensure parameters are loaded
    {

        if(SR_Avg_Fund_Period < SR_MotorSafety_Params->Avg_Fund_Period_Threshold_1)
        {
            // Call back warning the frequency is above the threshold set
            SR_MOTOR_SAFETY_MGR__AVG_FREQUENCY_ABOVE_THREHOLD();
        }
        else
        {
            // Call back warning the frequency is below the threshold set
            SR_MOTOR_SAFETY_MGR__AVG_FREQUENCY_BELOW_THREHOLD();
        }
    }
#else //SR_CIM_MOTOR

    // To be implemented.

#endif
}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Update Pwm variables - shall be only used by class B part
 *  @param      pwm values
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif

#ifdef SR_THIRD_CURRENT_RECONSTRUCTION
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Update ADC configuration according to duty-cycle pattern
 *  @details    The ADC is configured so that the two currents corresponding to the
 *              shortest duties are sampled on the first trigger event
 *  @param
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
void UpdateAdcConfig()
{
    sint32 temp_s32;
    uint16 index;   
    sint32 b;
    sint32 c;
    if(SR_SafetyMngr_State != SAFETY_MNGR_PRERUN_TESTS)  // in prerun we must not touch the ADC configuration
    {
        // Reconfigure ADC connections according to sector for third current reconstruction strategy
        if (SR_SafetyMngr_State != SAFETY_MNGR_RUNNING)
        {   // read all currents for correct offset update
            index = 0;
        }
        else
        {
            // calculate the index of the current corresponding to the longest duty (SR_sector)
            b = SRMCPWM__GET_PWM_B();
            c = SRMCPWM__GET_PWM_C();

            temp_s32 = SRMCPWM__GET_PWM_A();
            index = 1;

            if (b > temp_s32)
            {
                temp_s32 = b;
                index = 2;
            }

            if (c > temp_s32)
            {
                index = 3;
            }
        }

        SR_Sector = index;
        SR_MCU_MOTOR_UPDATE_N_VAR(SR_Sector);

        // and reconfigure ADC accordingly
        switch (SR_Sector)
        {
            case 1:
                SRMCATOD__SETADC2_REC1();
                SRMCATOD__SETADC3_REC1();
                break;

            case 2:
                SRMCATOD__SETADC1_REC2();
                SRMCATOD__SETADC3_REC2();
                break;

            case 3:
                SRMCATOD__SETADC1_REC3();
                SRMCATOD__SETADC2_REC3();
                break;

            default:
                SRMCATOD__SETADC1_DEFAULT();
                SRMCATOD__SETADC2_DEFAULT();
                SRMCATOD__SETADC3_DEFAULT();
                SRMCATOD__SETDCBUSVOLTAGE_DEFAULT();
                break;
        }
    }
}
#endif



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set a bit fault in the fault and fault holder registers and deny access from class A.
 *  @param[in]  error bit - refer to the SR_MOTOR_FAULT_LIST_TYPE for the bit definition
 *
 */
void SetFault(uint16 fault)
{
	uint16 temp_u16;

	temp_u16 = SR_Motor_Fault;
	BIT_SET(temp_u16, fault);
	SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Fault, temp_u16);

	temp_u16 = SR_Motor_Fault_Holder;
	BIT_SET(temp_u16, fault);
	SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Fault_Holder, temp_u16);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Only clears the bit fault in the fault register.
 *  @param[in]  error bit - refer to the SR_MOTOR_FAULT_LIST_TYPE for the bit definition
 *
 */
void ClearFault(uint16 fault)
{
	uint16 temp_fault;
	temp_fault = SR_Motor_Fault;
	BIT_CLR(temp_fault, (fault));
	SR_DATA_MCU_UPDATE_SHORT(SR_Motor_Fault, temp_fault);
}


#ifdef SRMOTORSAFETYMGR_INTERNAL_ISR_CHECK
//----------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      On entering Motor Control interrupt.
 *
 */
static void SrIsrMcEnterIsr(void)
{
    sint16 check = SRDATA_OK;

    SR_MCU_MOTOR_CHECK_SHORT(check, SR_MC_Isr_Enter_Cntr);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_MC_Isr_Exit_Cntr);
    if(check)
    {
        SRDATA_CORRUPTION_EXCEPTION(DATA_CORRUPTION_ISR_ENTER_SECTION);
    }

    if (SR_MC_Isr_Enter_Cntr > SR_MC_ISRMONITOR_MAX)
    {
        SRMOTORSAFETYMGR_ISR_FLOW_FAIL_OVERRUN();
    }
    else
    {
        SR_MC_Isr_Enter_Cntr += 1;
        SR_MCU_MOTOR_UPDATE_N_VAR(SR_MC_Isr_Enter_Cntr);
    }
}

//----------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      On exiting Motor Control interrupt.
 *
 */
static void SrIsrMcExitIsr(void)
{
    SR_MC_Isr_Exit_Cntr += 1;
    SR_MCU_MOTOR_UPDATE_N_VAR(SR_MC_Isr_Exit_Cntr);
}


//----------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      On entering 250us interrupt.
 *
 */
static void SrIsr250usEnterIsr(void)
{
    SR_DATA_MCU_CHECK_SHORT(SR_250us_Isr_Enter_Cntr);
    SR_DATA_MCU_CHECK_SHORT(SR_250us_Isr_Exit_Cntr);
    if (SR_250us_Isr_Enter_Cntr > SR_250US_ISRMONITOR_MAX)
    {
        SRMOTORSAFETYMGR_ISR_FLOW_FAIL_OVERRUN();
    }
    else
    {
        SR_DATA_MCU_UPDATE_SHORT(SR_250us_Isr_Enter_Cntr, (SR_250us_Isr_Enter_Cntr+1));
    }
}


//----------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handler for ISR Internal checks.
 *
 */
static void SrIsrHandler(void)
{
    sint16 check = SRDATA_OK;

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    SR_MCU_MOTOR_CHECK_BYTE(check, SR_Isr_Init_Flag);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_MC_Isr_Enter_Cntr);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_MC_Isr_Exit_Cntr);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_250us_Isr_Enter_Cntr);
    SR_MCU_MOTOR_CHECK_SHORT(check, SR_250us_Isr_Exit_Cntr);

    if(check)
    {
        SRDATA_CORRUPTION_EXCEPTION(DATA_CORRUPTION_ISR_HANDLER_SECTION);
    }

    if (SR_Isr_Init_Flag == 1)
    {   // Isr check already initialized
        // Catch an Enter/Exit mismatch - at this point they must be equal!
        if (SR_MC_Isr_Enter_Cntr != SR_MC_Isr_Exit_Cntr)
        {
            SRMOTORSAFETYMGR_ISR_FLOW_FAIL_SEQUENCE(SR_MC_Isr_Enter_Cntr,SR_MC_Isr_Exit_Cntr);
        }

        if (SR_250us_Isr_Enter_Cntr != SR_250us_Isr_Exit_Cntr)
        {
            SRMOTORSAFETYMGR_ISR_FLOW_FAIL_SEQUENCE(SR_250us_Isr_Enter_Cntr,SR_250us_Isr_Exit_Cntr);
        }

        // Catch interrupts less than lower threshold set
        if (SR_MC_Isr_Enter_Cntr < SR_MC_SRISRMONITOR_MIN)
        {
            SRMOTORSAFETYMGR_ISR_FLOW_FAIL_TIMEOUT();
        }

        if (SR_250us_Isr_Enter_Cntr < SR_250US_SRISRMONITOR_MIN)
        {
            SRMOTORSAFETYMGR_ISR_FLOW_FAIL_TIMEOUT();
        }

        // Everything is going OK - reset all counters for next check
        SR_DATA_MCU_UPDATE_SHORT(SR_MC_Isr_Enter_Cntr,0);
        SR_DATA_MCU_UPDATE_SHORT(SR_MC_Isr_Exit_Cntr,0);
        SR_DATA_MCU_UPDATE_SHORT(SR_250us_Isr_Enter_Cntr,0);
        SR_DATA_MCU_UPDATE_SHORT(SR_250us_Isr_Exit_Cntr,0);
    }
    else
    {  // First time entering the ISR check --> skip the check to get the correct synchronization
        SR_DATA_MCU_UPDATE_SHORT(SR_MC_Isr_Enter_Cntr,0);
        SR_DATA_MCU_UPDATE_SHORT(SR_MC_Isr_Exit_Cntr,0);
        SR_DATA_MCU_UPDATE_SHORT(SR_250us_Isr_Enter_Cntr,0);
        SR_DATA_MCU_UPDATE_SHORT(SR_250us_Isr_Exit_Cntr,0);
        SR_DATA_MCU_UPDATE_BYTE(SR_Isr_Init_Flag, 1); // start executing the check
    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    //------- End ISR Protected Code ----------------------------------------------------------------------------------
}

#endif

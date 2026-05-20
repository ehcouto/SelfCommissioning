/**
 *  @defgroup   CLASS_B
 *  @file       SRMCPwm.c
 *  @brief      Motor Control PWM driver
 *
 *  @details    Motor control PWM initializer, control functions, and ISR
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
*/

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRMCPwm.h"
#include "SRMicro.h"
#include "SRData.h"
#include "SRException.h"
#include "Comparator.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#ifndef SR_PWM_ENABLE_IRQ
    #define SR_PWM_ENABLE_IRQ                         0
#endif

#ifndef SR_PWM_DELAY_ENABLE
    #define SR_PWM_DELAY_ENABLE                       0
#endif

// Configuration constants
#define MILLION         1000000.0f
#define BILLION         1000000000.0f

typedef enum
{
    PWM_MOTOR_DRIVER_IDLE,
    PWM_MOTOR_DRIVER_STARTED,
    PWM_MOTOR_DRIVER_DEVICES_OFF,
    PWM_MOTOR_DRIVER_ACTIVE,
    PWM_MOTOR_DRIVER_LOWERS_ON,
    PWM_MOTOR_DRIVER_UPPERS_ON
} PWM_DRIVE_STATE_TYPE;

static uint16 MaxPwm;
static uint16 PwmFrequency;
static uint16 DeadTimeNanosec;
static uint16 NominalPwmPeriod;


//-------------------------------------- Safety Relevant Variables ------------------------------------------------

// All variables declared in this module are considered critical (SR) and have one negated copy shadow (NSR).
// All variables are also declared as static, since its visibility must be blocked at compiling time.

static SRMCPWM_DIAG_STATE_TYPE SR_MCPwm_Diag_State;     //!< Contains the current state where the state machine is located and running specific tasks.
static uint32 SR_MCPwm_Zero_Timeout;             //!< Contains time to wait when the overcurrent comparator is configured with the low threshold (SR_PWM_FAULT_0_CMP_LEV_TEST). Once the time is over, the peripheral fault state is read, and the active polarity is expected.
static uint32 SR_MCPwm_One_Timeout;              //!< Contains time to wait when the overcurrent comparator is configured with the high, normal threshold (SR_PWM_FAULT_0_CMP_LEV). Once the time is over, the peripheral fault state is read, and the default polarity is expected.
static SRMCPWM_DIAG_ERROR_TYPE SR_MCPwm_Errors;              //!< At each state of the handler, the state machine searches for a failure in the hardware, once the fault is found, then a bit is set in this variable. The error are listed in bitwise method. The bit description is listed in the SRMCPWM_DIAG_ERROR_TYPE definition.
static uint32 SR_MCPwm_Pulse_Active_Level;   //!< Contains the longest pulse measurement, when the shut down pin is read as active. This variable is used in the 250us polling.
static uint32 SR_MCPwm_Pulse_Default_Level;  //!< Contains the time that the shut down pin is considered inactive while running the SRMCPwm diagnostic state machine handler. This variable is used in the PWM period polling.


static SRMCPWM_DIAG_STATE_TYPE NSR_MCPwm_Diag_State;
static uint32 NSR_MCPwm_Zero_Timeout;
static uint32 NSR_MCPwm_One_Timeout;
static SRMCPWM_DIAG_ERROR_TYPE NSR_MCPwm_Errors;
static uint32 NSR_MCPwm_Pulse_Active_Level;
static uint32 NSR_MCPwm_Pulse_Default_Level;


#if(SR_PWM_DELAY_ENABLE == 1)
static uint16 SR_Pwm_A_1;
static uint16 SR_Pwm_B_1;
static uint16 SR_Pwm_C_1;

static uint16 NSR_Pwm_A_1;
static uint16 NSR_Pwm_B_1;
static uint16 NSR_Pwm_C_1;
#endif


//-------------------------------------- Definitions ------------------------------------------------

#define PWM_OUTPUT_OFF()			{FTM0.OUTMASK |= (FTM_OUTMASK_CH5OM_MASK | FTM_OUTMASK_CH4OM_MASK |FTM_OUTMASK_CH3OM_MASK | FTM_OUTMASK_CH2OM_MASK |FTM_OUTMASK_CH1OM_MASK | FTM_OUTMASK_CH0OM_MASK);}	// all outputs disabled (at inactive level)
#define PWM_OUTPUT_ON()				{FTM0.OUTMASK &= ~(FTM_OUTMASK_CH5OM_MASK | FTM_OUTMASK_CH4OM_MASK |FTM_OUTMASK_CH3OM_MASK | FTM_OUTMASK_CH2OM_MASK |FTM_OUTMASK_CH1OM_MASK | FTM_OUTMASK_CH0OM_MASK);}	// all outputs enable (at active level)
#define PWM_TIMER_START()			{FTM0.MODE |= FTM_MODE_WPDIS_MASK; FTM0.SC |= FTM_SC_CLKS(1);FTM0.FMS |= FTM_FMS_WPEN_MASK;} //Not functional
#define PWM_TIMER_STOP()			{FTM0.MODE |= FTM_MODE_WPDIS_MASK; FTM0.SC |= FTM_SC_CLKS(0);FTM0.FMS |= FTM_FMS_WPEN_MASK;} //Not functional

//---------------------------------------- Macros -----------------------------------------------------

#if(1)
// CHECK Macro defs
//lint -emacro( 928, SR_MCPWM_CHECK_DIAG_STATE )
#define SR_MCPWM_CHECK_DIAG_STATE()                         SRData__CheckByte((uint8 *)&SR_MCPwm_Diag_State, (uint8 *)&NSR_MCPwm_Diag_State)
//lint -emacro( 929, SR_MCPWM_CHECK_ZERO_TIMEOUT )
#define SR_MCPWM_CHECK_ZERO_TIMEOUT()                       SRData__CheckLong((uint32 *)&SR_MCPwm_Zero_Timeout, (uint32 *)&NSR_MCPwm_Zero_Timeout)
//lint -emacro( 929, SR_MCPWM_CHECK_ONE_TIMEOUT )
#define SR_MCPWM_CHECK_ONE_TIMEOUT()                        SRData__CheckLong((uint32 *)&SR_MCPwm_One_Timeout, (uint32 *)&NSR_MCPwm_One_Timeout)
//lint -emacro( 928, SR_MCPWM_CHECK_ERRORS )
#define SR_MCPWM_CHECK_ERRORS()                             SRData__CheckByte((uint8 *)&SR_MCPwm_Errors, (uint8 *)&NSR_MCPwm_Errors)
//lint -emacro( 929, SR_MCPWM_CHECK_MAX_PULSE_DURATION )
#define SR_MCPWM_CHECK_MAX_PULSE_DURATION()                 SRData__CheckShort((uint16 *)&SR_MCPwm_Max_Pulse_Duration, (uint16 *)&NSR_MCPwm_Max_Pulse_Duration)
//lint -emacro( 929, SR_MCPWM_CHECK_TMP_PULSE_DURATION )
#define SR_MCPWM_CHECK_TMP_PULSE_DURATION()                 SRData__CheckLong((uint32 *)&SR_MCPwm_Tmp_Pulse_Duration, (uint32 *)&NSR_MCPwm_Tmp_Pulse_Duration)
//lint -emacro( 929, SR_MCPWM_CHECK_ACTIVE_PULSE_DURATION )
#define SR_MCPWM_CHECK_ACTIVE_PULSE_DURATION()              SRData__CheckLong((uint32 *)&SR_MCPwm_Pulse_Active_Level, (uint32 *)&NSR_MCPwm_Pulse_Active_Level)
//lint -emacro( 929, SR_MCPWM_CHECK_DEFAULT_PULSE_DURATION )
#define SR_MCPWM_CHECK_DEFAULT_PULSE_DURATION()             SRData__CheckLong((uint32 *)&SR_MCPwm_Pulse_Default_Level, (uint32 *)&NSR_MCPwm_Pulse_Default_Level)

#if(SR_PWM_DELAY_ENABLE == 1)
//lint -emacro( 929, SR_MCPWM_CHECK_DUTY_A_DELAYED )
#define SR_MCPWM_CHECK_DUTY_A_DELAYED()                     SRData__CheckShort((uint16 *)&SR_Pwm_A_1, (uint16 *)&NSR_Pwm_A_1)
//lint -emacro( 929, SR_MCPWM_CHECK_DUTY_B_DELAYED )
#define SR_MCPWM_CHECK_DUTY_B_DELAYED()                     SRData__CheckShort((uint16 *)&SR_Pwm_B_1, (uint16 *)&NSR_Pwm_B_1)
//lint -emacro( 929, SR_MCPWM_CHECK_DUTY_C_DELAYED )
#define SR_MCPWM_CHECK_DUTY_C_DELAYED()                     SRData__CheckShort((uint16 *)&SR_Pwm_C_1, (uint16 *)&NSR_Pwm_C_1)
#endif

// UPDATE Macro defs
//lint -emacro( 928, SR_MCPWM_UPDATE_DIAG_STATE )
#define SR_MCPWM_UPDATE_DIAG_STATE(value)                   SRData__UpdateByte((uint8 *)&SR_MCPwm_Diag_State, (uint8 *)&NSR_MCPwm_Diag_State, (uint8)(value))
//lint -emacro( 929, SR_MCPWM_UPDATE_ZERO_TIMEOUT )
#define SR_MCPWM_UPDATE_ZERO_TIMEOUT(value)                 SRData__UpdateLong((uint32 *)&SR_MCPwm_Zero_Timeout, (uint32 *)&NSR_MCPwm_Zero_Timeout, (uint32)(value))
//lint -emacro( 929, SR_MCPWM_UPDATE_ONE_TIMEOUT )
#define SR_MCPWM_UPDATE_ONE_TIMEOUT(value)                  SRData__UpdateLong((uint32 *)&SR_MCPwm_One_Timeout, (uint32 *)&NSR_MCPwm_One_Timeout, (uint32)(value))
//lint -emacro( 928, SR_MCPWM_UPDATE_ERRORS )
#define SR_MCPWM_UPDATE_ERRORS(value)                       SRData__UpdateByte((uint8 *)&SR_MCPwm_Errors, (uint8 *)&NSR_MCPwm_Errors, (uint8)(value))
//lint -emacro( 929, SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION )
#define SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(value)        SRData__UpdateLong((uint32 *)&SR_MCPwm_Pulse_Active_Level, (uint32 *)&NSR_MCPwm_Pulse_Active_Level, (uint32)(value))
//lint -emacro( 929, SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION )
#define SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(value)       SRData__UpdateLong((uint32 *)&SR_MCPwm_Pulse_Default_Level, (uint32 *)&NSR_MCPwm_Pulse_Default_Level, (uint32)(value))

#if(SR_PWM_DELAY_ENABLE == 1)
//lint -emacro( 929, SR_MCPWM_UPDATE_DUTY_A_DELAYED )
#define SR_MCPWM_UPDATE_DUTY_A_DELAYED(value)               SRData__UpdateShort((uint16 *)&SR_Pwm_A_1, (uint16 *)&NSR_Pwm_A_1, (uint16)(value))
//lint -emacro( 929, SR_MCPWM_UPDATE_DUTY_B_DELAYED )
#define SR_MCPWM_UPDATE_DUTY_B_DELAYED(value)               SRData__UpdateShort((uint16 *)&SR_Pwm_B_1, (uint16 *)&NSR_Pwm_B_1, (uint16)(value))
//lint -emacro( 929, SR_MCPWM_UPDATE_DUTY_C_DELAYED )
#define SR_MCPWM_UPDATE_DUTY_C_DELAYED(value)               SRData__UpdateShort((uint16 *)&SR_Pwm_C_1, (uint16 *)&NSR_Pwm_C_1, (uint16)(value))
#endif

#else
// CHECK Macro defs
#define SR_MCPWM_CHECK_DIAG_STATE()                         SRDATA_OK // SR_MCPwm_Diag_State
#define SR_MCPWM_CHECK_ZERO_TIMEOUT()                       SRDATA_OK // SR_MCPwm_Zero_Timeout
#define SR_MCPWM_CHECK_ONE_TIMEOUT()                        SRDATA_OK // SR_MCPwm_One_Timeout
#define SR_MCPWM_CHECK_ERRORS()                             SRDATA_OK // SR_MCPwm_Errors
#define SR_MCPWM_CHECK_MAX_PULSE_DURATION()                 SRDATA_OK // SR_MCPwm_Max_Pulse_Duration
#define SR_MCPWM_CHECK_TMP_PULSE_DURATION()                 SRDATA_OK // SR_MCPwm_Tmp_Pulse_Duration
#define SR_MCPWM_CHECK_ACTIVE_PULSE_DURATION()              SRDATA_OK // SR_MCPwm_Pulse_Active_Level
#define SR_MCPWM_CHECK_DEFAULT_PULSE_DURATION()             SRDATA_OK // SR_MCPwm_Pulse_Active_Level

// UPDATE Macro defs
#define SR_MCPWM_UPDATE_DIAG_STATE(value)                   SR_MCPwm_Diag_State = (value)
#define SR_MCPWM_UPDATE_ZERO_TIMEOUT(value)                 SR_MCPwm_Zero_Timeout = (value)
#define SR_MCPWM_UPDATE_ONE_TIMEOUT(value)                  SR_MCPwm_One_Timeout = (value)
#define SR_MCPWM_UPDATE_ERRORS(value)                       SR_MCPwm_Errors = (value)
#define SR_MCPWM_UPDATE_MAX_PULSE_DURATION(value)           SR_MCPwm_Max_Pulse_Duration = (value)
#define SR_MCPWM_UPDATE_TMP_PULSE_DURATION(value)           SR_MCPwm_Tmp_Pulse_Duration = (value)
#define SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(value)        SR_MCPwm_Pulse_Active_Level = (value)
#define SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(value)       SR_MCPwm_Pulse_Default_Level = (value)

#endif

//-------------------------------------- PRIVATE Function Prototypes ------------------------------------------------
static BOOL_TYPE GetFaultState(void);
static BOOL_TYPE GetFaultFlagState(void);
static void ClearFaultFlag(void);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


/*
 * @brief       Initialize the inverter outputs before booting the class B - enable all lowers switches
 * @details     According to the memory size, the microcontroller may take long time to power up, in the case of
 *              a reset and according to the physics of the motor, some side effects shall happen to the
 *              system. E.g., very high DC bus voltage and breaks the electronic. Therefore, it requires
 *              to disconnect motor from the dc bus capacitors by enabled lowers switches.
 */
void SRMCPwm__Initialize_Before_Boot(void)
{
    SIM.SCGC5 |= (SIM_SCGC5_PORTA_MASK |            // Connect the GPIOA device to the Core bus
                 SIM_SCGC5_PORTB_MASK  |            // Connect the GPIOB device to the Core bus
                 SIM_SCGC5_PORTC_MASK  |            // Connect the GPIOC device to the Core bus
                 SIM_SCGC5_PORTD_MASK  |            // Connect the GPIOD device to the Core bus
                 SIM_SCGC5_PORTE_MASK);             // Connect the GPIOE device to the Core bus




    // Initialize the output state
#if SR_PWM_HIGH_CHANNELS_ACTIVE_LEVEL == 1 // High level activated

    #if (SR_PWM__REMAP == 0)

        // Zero all upper switches
        GPIOC.PCOR = (1<<0);
        GPIOC.PCOR = (1<<2);
        GPIOC.PCOR = (1<<4);

    #elif(SR_PWM__REMAP == 1)

        // Zero all upper switches
        GPIOD.PCOR = (1<<0);
        GPIOD.PCOR = (1<<2);
        GPIOD.PCOR = (1<<4);

    #endif

#elif SR_PWM_HIGH_CHANNELS_ACTIVE_LEVEL == 0 // Low level activated

    #if (SR_PWM__REMAP == 0)

        // One all upper switches
        GPIOC.PSOR = (1<<0);
        GPIOC.PSOR = (1<<2);
        GPIOC.PSOR = (1<<4);

    #elif(SR_PWM__REMAP == 1)

        // One all upper switches
        GPIOD.PSOR = (1<<0);
        GPIOD.PSOR = (1<<2);
        GPIOD.PSOR = (1<<4);

    #endif

#else

    #error High power switch driver polarity not defined

#endif


#if SR_PWM_LOW_CHANNELS_ACTIVE_LEVEL == 1 // High level activated

    #if(SR_PWM__REMAP == 0)

        GPIOC.PSOR = (1<<1);
        GPIOC.PSOR = (1<<3);
        GPIOC.PSOR = (1<<5);

    #elif(SR_PWM__REMAP == 1)

        GPIOD.PSOR = (1<<1);
        GPIOD.PSOR = (1<<3);
        GPIOD.PSOR = (1<<5);

    #endif

#elif SR_PWM_LOW_CHANNELS_ACTIVE_LEVEL == 0 // Low level activated

    #if(SR_PWM__REMAP == 0)

        GPIOC.PCOR = (1<<1);
        GPIOC.PCOR = (1<<3);
        GPIOC.PCOR = (1<<5);

    #elif(SR_PWM__REMAP == 1)

        GPIOD.PCOR = (1<<1);
        GPIOD.PCOR = (1<<3);
        GPIOD.PCOR = (1<<5);

    #endif

#else

    #error High power switch driver polarity not defined

#endif




    // Initilize the gpio as output push pull
#if(SR_PWM__REMAP == 0)

    GPIOC.PDDR |= (1 << 0);
    PCIPORTC.PCR[0] = (PORT_PCR_MUX(1));

    GPIOC.PDDR |= (1 << 1);
    PCIPORTC.PCR[1] = (PORT_PCR_MUX(1));

    GPIOC.PDDR |= (1 << 2);
    PCIPORTC.PCR[2] = (PORT_PCR_MUX(1));

    GPIOC.PDDR |= (1 << 3);
    PCIPORTC.PCR[3] = (PORT_PCR_MUX(1));

    GPIOC.PDDR |= (1 << 4);
    PCIPORTC.PCR[4] = (PORT_PCR_MUX(1));

    GPIOC.PDDR |= (1 << 5);
    PCIPORTC.PCR[5] = (PORT_PCR_MUX(1));

#elif(SR_PWM__REMAP == 1)

    GPIOD.PDDR |= (1 << 0);
    PCIPORTD.PCR[0] = (PORT_PCR_MUX(1));

    GPIOD.PDDR |= (1 << 1);
    PCIPORTD.PCR[1] = (PORT_PCR_MUX(1));

    GPIOD.PDDR |= (1 << 2);
    PCIPORTD.PCR[2] = (PORT_PCR_MUX(1));

    GPIOD.PDDR |= (1 << 3);
    PCIPORTD.PCR[3] = (PORT_PCR_MUX(1));

    GPIOD.PDDR |= (1 << 4);
    PCIPORTD.PCR[4] = (PORT_PCR_MUX(1));

    GPIOD.PDDR |= (1 << 5);
    PCIPORTD.PCR[5] = (PORT_PCR_MUX(1));

#endif




    // Refresh the initialized output state
#if SR_PWM_HIGH_CHANNELS_ACTIVE_LEVEL == 1 // High level activated

    #if (SR_PWM__REMAP == 0)

        // Zero all upper switches
        GPIOC.PCOR = (1<<0);
        GPIOC.PCOR = (1<<2);
        GPIOC.PCOR = (1<<4);

    #elif(SR_PWM__REMAP == 1)

        // Zero all upper switches
        GPIOD.PCOR = (1<<0);
        GPIOD.PCOR = (1<<2);
        GPIOD.PCOR = (1<<4);

    #endif

#elif SR_PWM_HIGH_CHANNELS_ACTIVE_LEVEL == 0 // Low level activated

    #if (SR_PWM__REMAP == 0)

        // One all upper switches
        GPIOC.PSOR = (1<<0);
        GPIOC.PSOR = (1<<2);
        GPIOC.PSOR = (1<<4);

    #elif(SR_PWM__REMAP == 1)

        // One all upper switches
        GPIOD.PSOR = (1<<0);
        GPIOD.PSOR = (1<<2);
        GPIOD.PSOR = (1<<4);

    #endif

#else

    #error High power switch driver polarity not defined

#endif


#if SR_PWM_LOW_CHANNELS_ACTIVE_LEVEL == 1 // High level activated

    #if(SR_PWM__REMAP == 0)

        GPIOC.PSOR = (1<<1);
        GPIOC.PSOR = (1<<3);
        GPIOC.PSOR = (1<<5);

    #elif(SR_PWM__REMAP == 1)

        GPIOD.PSOR = (1<<1);
        GPIOD.PSOR = (1<<3);
        GPIOD.PSOR = (1<<5);

    #endif

#elif SR_PWM_LOW_CHANNELS_ACTIVE_LEVEL == 0 // Low level activated

    #if(SR_PWM__REMAP == 0)

        GPIOC.PCOR = (1<<1);
        GPIOC.PCOR = (1<<3);
        GPIOC.PCOR = (1<<5);

    #elif(SR_PWM__REMAP == 1)

        GPIOD.PCOR = (1<<1);
        GPIOD.PCOR = (1<<3);
        GPIOD.PCOR = (1<<5);

    #endif

#else

    #error High power switch driver polarity not defined

#endif
}




/**
 *  @brief       It Initializes the module MCPwm and its variables.
 *               FTM0 is selected for PWM outputs
 *
 *  @param[in]   pwm_frequency defines the PWM frequency in Hz.
 *  @param[in]   dead_time_ns defines the dead time inserted between complementary channels. This is defined in nanoseconds.
 */
void SRMCPwm__Initialize(uint16 pwm_frequency, uint16 dead_time_ns)
{
    uint16 deadtime;
	uint16 deadtime_prescaler;
    uint16 pwm_period;                               // Local variable for calculation
    uint16 fault_enable_mask = 0;
 

    SIM.SCGC6 |= SIM_SCGC6_FTM0_MASK;	// Enables the clock for FTM0

//------------------  Pwm Period & Deadtime Calculation --------------------------
    pwm_period = (uint16)(Micro__GetClock()->BUSCLK_Frequency / pwm_frequency);// PWM period in clock events

    //FTM0 deadtime counter it's only 5 bits long, there is a 2-bit pre-scaler that can be set:
    //Calculating the pre-scaler value:
    deadtime_prescaler = (uint16)(((dead_time_ns*(Micro__GetClock()->BUSCLK_Frequency/MILLION))/63.0f)/(uint32)1000.0f);
    //The pre-scaler is found by getting the Count of leading zeros in a 16 bit integer, IAR supports __clz(x) if needed
    //FTM0 does not have a divided per 2 pre-scaler, so if we need 1, 3 divides the clock by 16
    if (deadtime_prescaler < 4 && deadtime_prescaler > 0)
    {
        deadtime_prescaler = 2;
    }
    else if (deadtime_prescaler >= 4)
    {
        deadtime_prescaler = 4;
    }
    else
    {
        deadtime_prescaler = 0;
    }


    deadtime = ((uint16)((dead_time_ns*(Micro__GetClock()->BUSCLK_Frequency/MILLION))/1000.0f)/(1<<deadtime_prescaler));//1<<deadtime_prescaler);

    if (deadtime > ((1<<6)-1))
    {
        deadtime = ((1<<6)-1);
    }

    if (deadtime_prescaler < 4)
    {
        deadtime |= (deadtime_prescaler<<6);
    }
    else //The pre-scalar value of 3 is a 16 divider on the clock
    {
        deadtime |= (3<<6);
    }

    PwmFrequency = pwm_frequency;
    DeadTimeNanosec = dead_time_ns;
    MaxPwm = pwm_period>>1;
    NominalPwmPeriod = pwm_period;




//------------------------- Port configuration to force lowers on in case of break ---------------------
    //Config_MCPwm_Gpio();

//------------------  FTM0 Initialization --------------------------

	MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();	//
	MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();		// Disables interruptions while configuring
	
	// Disable all channels outputs using the OUTPUT MASK feature.
	// However, the output pins are still driven as GPIO since the
	// channel mode is set to FTM channel disabled after RESET
	PWM_OUTPUT_OFF();
	
	FTM0.MODE |= FTM_MODE_WPDIS_MASK; 			// Disable write protection for certain registers
    FTM0.MODE |= FTM_MODE_FTMEN_MASK; 			// Enables the counter
    //FTM0.MODE |= FTM_MODE_FAULTM_MASK;			// Fault control is enabled for all channels, and the selected mode is the automatic fault clearing.
    FTM0.MODE |= FTM_MODE_FAULTM(2);          // Fault control is enabled for all channels, and the selected mode is the manual fault clearing.

//------------------  FTM0 Counter configuration --------------------------
//Set the counter, edge aligned PWM, center-aligned is forced by the way the Duty cycles are set

	FTM0.MOD = (uint16)((pwm_period>>1)-1);		// Set Modulo register
    FTM0.CNTIN = (uint32) (-(sint32)FTM0.MOD-1); 					// Set initial counting value

#ifdef PWM_LOAD_AT_MOD
    FTM0.SYNC = FTM_SYNC_CNTMAX_MASK;  	// Pwm loading when FTM counter reaches its maximum value (MOD register).
#endif
#ifdef PWM_LOAD_AT_CNTIN
    FTM0.SYNC = FTM_SYNC_CNTMIN_MASK; 	// Pwm loading when FTM counter reaches its minimum value (CNTIN register).
#endif

    FTM0.SYNCONF |= FTM_SYNC_REINIT_MASK; //Synchronize CNTIN with PWM in legacy pwm sync mode


    // for center aligned PWM using combine mode:
	//	COMBINE = 1 - combine mode set
	//	COMP = 1 - complementary PWM set
	//	DTEN = 1 - deadtime enabled
	//	SYNCEN = 1 - PWM update synchronization enabled
	//	FAULTEN = 1 - fault control enabled

    FTM0.COMBINE =  FTM_COMBINE_FAULTEN0_MASK | FTM_COMBINE_SYNCEN0_MASK | FTM_COMBINE_DTEN0_MASK
                | FTM_COMBINE_COMP0_MASK    | FTM_COMBINE_COMBINE0_MASK
                | FTM_COMBINE_FAULTEN1_MASK | FTM_COMBINE_SYNCEN1_MASK | FTM_COMBINE_DTEN1_MASK
                | FTM_COMBINE_COMP1_MASK    | FTM_COMBINE_COMBINE1_MASK
                | FTM_COMBINE_FAULTEN2_MASK | FTM_COMBINE_SYNCEN2_MASK | FTM_COMBINE_DTEN2_MASK
                | FTM_COMBINE_COMP2_MASK    | FTM_COMBINE_COMBINE2_MASK;



//------------------------- Fault Input configuration ------------------------
#if (SR_PWM_FAULT_0 == ENABLED)
    fault_enable_mask |= FTM_FLTCTRL_FAULT0EN_MASK;

	#if (SR_PWM_FAULT_0_SRC == CMP)
	SIM.SOPT4 |= SIM_SOPT4_FTM0FLT0_MASK;	// Configures the source of FTM0 fault 0 to comparator CMP0 output
	Comparator__Config(SR_COMPARATOR_CHANNEL, SR_COMPARATOR_INVERTED_INPUT, SR_COMPARATOR_NONINVERTED_INPUT, SR_COMPARATOR_MODE, SR_PWM_FAULT_0_CMP_LEV);	 // Conifigures the comparator 0
	#else
	SIM.SOPT4 &= ~SIM_SOPT4_FTM0FLT0_MASK;
	#endif
#endif

#if (SR_PWM_FAULT_1 == ENABLED)
	fault_enable_mask |= FTM_FLTCTRL_FAULT1EN_MASK;

	#if (SR_PWM_FAULT_1_SRC == CMP)
	SIM.SOPT4 |= SIM_SOPT4_FTM0FLT1_MASK;  	// Configures the source of FTM0 fault 1 to comparator CMP0 output
	Comparator__Config(SR_COMPARATOR_CHANNEL, SR_COMPARATOR_INVERTED_INPUT, SR_COMPARATOR_NONINVERTED_INPUT, SR_COMPARATOR_MODE, SR_PWM_FAULT_0_CMP_LEV);  // Conifigures the comparator 1
	#else
	SIM.SOPT4 &= ~SIM_SOPT4_FTM0FLT1_MASK;
	#endif
#endif

#if (SR_PWM_FAULT_2 == ENABLED)
	fault_enable_mask |= FTM_FLTCTRL_FAULT2EN_MASK;
#endif

#if (SR_PWM_FAULT_3 == ENABLED)
	fault_enable_mask |= FTM_FLTCTRL_FAULT3EN_MASK;
#endif

	FTM0.FLTCTRL |= fault_enable_mask; 		// Enables the fault inputs



//------------------------- FMT0 Output Polarity configuration ------------------------
	//POL Register set the level of the channel in case of Fault
	//This is considering channel 0,2 and 4 as the high channels
	FTM0.CONTROLS[0].CnSC |= FTM_CnSC_ELSB_MASK ;
	FTM0.CONTROLS[1].CnSC |= FTM_CnSC_ELSB_MASK ;
	FTM0.CONTROLS[2].CnSC |= FTM_CnSC_ELSB_MASK ;
	FTM0.CONTROLS[3].CnSC |= FTM_CnSC_ELSB_MASK ;
	FTM0.CONTROLS[4].CnSC |= FTM_CnSC_ELSB_MASK ;
	FTM0.CONTROLS[5].CnSC |= FTM_CnSC_ELSB_MASK ;

#if (SR_PWM_HIGH_CHANNELS_ACTIVE_LEVEL == SR_PWM_LOW_CHANNELS_ACTIVE_LEVEL)
 #if (SR_PWM_HIGH_CHANNELS_ACTIVE_LEVEL == 1)
    FTM0.POL |= 0;									// The channel polarity is active high.
 #else
    FTM0.POL |= FTM_POL_POL0_MASK | FTM_POL_POL2_MASK | FTM_POL_POL4_MASK | FTM_POL_POL1_MASK | FTM_POL_POL3_MASK | FTM_POL_POL5_MASK;
 #endif
#else
 #if (SR_PWM_HIGH_CHANNELS_ACTIVE_LEVEL == 1)
    FTM0.POL |= FTM_POL_POL1_MASK | FTM_POL_POL3_MASK | FTM_POL_POL5_MASK;
 #else
    FTM0.POL |= FTM_POL_POL0_MASK | FTM_POL_POL2_MASK | FTM_POL_POL4_MASK;
 #endif
#endif

	FTM0.DEADTIME = deadtime;	 					// Configures the deadtime prescaler factor and deadtime value

	SRMCPWM__LOADALLPWMVALUES(SRMCPWM__GET_LOWERS_ON_PWM(),SRMCPWM__GET_LOWERS_ON_PWM(),SRMCPWM__GET_LOWERS_ON_PWM()); // Initial setting of value registers to 0 % of duty cycle

	FTM0.EXTTRIG |= FTM_EXTTRIG_INITTRIGEN_MASK;	// Enables the generation of the trigger when the FTM counter is equal to the CNTIN register, used for starting of the PDB counter
	FTM0.MODE |= FTM_MODE_INIT_MASK;				// Initializes the channels
	FTM0.SC |= FTM_SC_CLKS(1);						// Set system clock as source for FTM0 (CLKS[1:0] = 01)

	FTM0.CONF |= FTM_CONF_BDMMODE(1);               // When in debug mode, channels outputs are forced to their safe value according to POLn bit
//------------------------- Port for FTM0 configuration ---------------------
// Note: Clock for Port C anc D enabled in sysinit.c already
#if (SR_PWM__REMAP == 0)
	PCIPORTC.PCR[1] = PORT_PCR_MUX(4); 				// FTM0 CH0,PTC1
	PCIPORTC.PCR[2] = PORT_PCR_MUX(4); 				// FTM0 CH1,PTC2
	PCIPORTC.PCR[5] = PORT_PCR_MUX(7); 				// FTM0 CH2,PTC5
	PCIPORTC.PCR[4] = PORT_PCR_MUX(4); 				// FTM0 CH3,PTC4
	PCIPORTD.PCR[4] = PORT_PCR_MUX(4); 				// FTM0 CH4,PTD4
	PCIPORTD.PCR[5] = PORT_PCR_MUX(4); 				// FTM0 CH5,PTD5

	PCIPORTA.PCR[19] = PORT_PCR_MUX(2); 			// Fault input FTM0_FLT0
#elif(SR_PWM__REMAP == 1)
	PCIPORTD.PCR[0]  = PORT_PCR_MUX(4); 			// FTM0 CH0  GWHGD
	PCIPORTD.PCR[1]  = PORT_PCR_MUX(4);             // FTM0 CH1  GWLGD
	PCIPORTD.PCR[2]  = PORT_PCR_MUX(4);             // FTM0 CH2  GVHGD
	PCIPORTD.PCR[3]  = PORT_PCR_MUX(4);             // FTM0 CH3  GVLGD
	PCIPORTD.PCR[4]  = PORT_PCR_MUX(4);             // FTM0 CH4  GUHGD
	PCIPORTD.PCR[5]  = PORT_PCR_MUX(4);             // FTM0 CH5  GULGD
#endif



//------------------------------ Enables Output ------------------------------
#if (SR_PWM_ENABLE_IRQ == 1)
	FTM0.SC |= FTM_SC_TOIE_MASK;					// Enables Counter Overflow interrupt
	Micro__NVICEnableIRQ(FTM0_IRQ_CHANNEL,0,0);	   	// Enables FTM0 Interrupt
#endif
	FTM0.FMS |= FTM_FMS_WPEN_MASK;				   	// Protect the Registers - Write Protection Enabled

#if(SR_PWM_INIT_STATE == ENABLED)
	PWM_OUTPUT_ON();                                // Enables PWM output
#endif

	MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();		// Enables back interruptions


//------------------------ Initialize the Class B variables ---------------------------
    SR_MCPWM_UPDATE_ZERO_TIMEOUT(0);

    SR_MCPWM_UPDATE_ONE_TIMEOUT(0);

    SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(0);
    SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(0);

    SR_MCPWM_UPDATE_ERRORS(SR_MCPWM_ERROR_PERIPHERAL_NOT_SAFE);

    SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_WAITING_TO_CHECK);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the max PWM counts configured in the peripheral.
 *
 * @remark      This method uses a shadowed variable, which was set during the initialization of this module.
 *
 * @return      Max PWM in timer counts.
 */
uint16 SRMCPwm__GetMaxPwm(void)
{
    return(MaxPwm);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       ISR for the PWM.
 *
 * @detail      This is configured to happen at the end of each PWM period.
 */
INTERRUPT_DIRECTIVE void SRMCPwm__PwmISR(void)
{
#if(SR_PWM_DELAY_ENABLE == 1)
    uint32 temp;

    temp = SR_MCPWM_CHECK_DUTY_A_DELAYED();    // Checks state data
    temp += SR_MCPWM_CHECK_DUTY_B_DELAYED();   // Checks variable data
    temp += SR_MCPWM_CHECK_DUTY_C_DELAYED();   // Checks variable data

    if(temp == SRDATA_OK)                          // If data is not corrupted
    {
        // phase U
        FTM0.CONTROLS[SR_PWM_U_HIGH].CnV    = (sint16)SR_Pwm_A_1;
        FTM0.CONTROLS[SR_PWM_U_LOW].CnV     = -(sint16)SR_Pwm_A_1;

        // phase V
        FTM0.CONTROLS[SR_PWM_V_HIGH].CnV    = (sint16)SR_Pwm_B_1;
        FTM0.CONTROLS[SR_PWM_V_LOW].CnV     = -(sint16)SR_Pwm_B_1;

        // phase V
        FTM0.CONTROLS[SR_PWM_W_HIGH].CnV    = (sint16)SR_Pwm_C_1;
        FTM0.CONTROLS[SR_PWM_W_LOW].CnV     = -(sint16)SR_Pwm_C_1;

        FTM0.PWMLOAD = FTM_PWMLOAD_LDOK_MASK;


        FTM0.SC &= ~FTM_SC_TOF_MASK; //Clear the interrupt flag
    }
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Turn ON ALL LOW GATES.
 *
 * @remark      This is related to the driver input and NOT to the PWM output.
 */
void SRMCPwm__TurnOnLowSideGates(void)
{
	SRMCPWM__LOADALLPWMVALUES(0, 0, 0);
    PWM_OUTPUT_ON();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Turn OFF ALL LOW GATES.
 *
 * @remark      This is related to the driver input and NOT to the PWM output.
 */
void SRMCPwm__TurnOffLowSideGates(void)
{
	SRMCPwm__DisablePwmOutput(SR_MCPWM_GATE_ALL_LO);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Turn ON ALL HIGH GATES.
 *
 * @remark      This is related to the driver input and NOT to the PWM output.
 */
void SRMCPwm__TurnOnHighSideGates(void)
{
	SRMCPWM__LOADALLPWMVALUES(32768 , 32768 , 32768);

	PWM_OUTPUT_ON();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief        Turn OFF ALL HIGH GATES.
 *
 * @details      This is related to the driver input and NOT to the PWM output.
 *
 */
void SRMCPwm__TurnOffHighSideGates(void)
{
	SRMCPwm__DisablePwmOutput(SR_MCPWM_GATE_ALL_HI);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Turn OFF all the gates.
 *
 * @details     This method is created and reconfigure on fly the Gpio output, because
 *              the LOCK bits can be used and the OCx/OCxN will not be able to be
 *              reconfigured on fly.
 */
void SRMCPwm__TurnOffAllGates(void)
{
	PWM_OUTPUT_OFF();

    if (GetFaultFlagState() != 0)
    {
        ClearFaultFlag();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Load the user PWM duty to the respective registers.
 *
 * @details     All parameters are given in PWM counts.
 *
 * @param[in] pwm_a
 * @param[in] pwm_b
 * @param[in] pwm_c
 */
void SRMCPwm__LoadAllPwmValues(uint16 pwm_a, uint16 pwm_b, uint16 pwm_c)
{

    pwm_a = (uint16)(((sint32)(MaxPwm) * (sint32)(pwm_a))>>15);
    pwm_b = (uint16)(((sint32)(MaxPwm) * (sint32)(pwm_b))>>15);
    pwm_c = (uint16)(((sint32)(MaxPwm) * (sint32)(pwm_c))>>15);

#if(SR_PWM_DELAY_ENABLE == 0)
    // phase U
    FTM0.CONTROLS[SR_PWM_U_HIGH].CnV 	= (sint16)pwm_a;
    FTM0.CONTROLS[SR_PWM_U_LOW].CnV 	= -(sint16)pwm_a;

    // phase V
	FTM0.CONTROLS[SR_PWM_V_HIGH].CnV 	= (sint16)pwm_b;
	FTM0.CONTROLS[SR_PWM_V_LOW].CnV 	= -(sint16)pwm_b;

	// phase V
    FTM0.CONTROLS[SR_PWM_W_HIGH].CnV 	= (sint16)pwm_c;
    FTM0.CONTROLS[SR_PWM_W_LOW].CnV 	= -(sint16)pwm_c;

	FTM0.PWMLOAD = FTM_PWMLOAD_LDOK_MASK;
#else

	SR_MCPWM_UPDATE_DUTY_A_DELAYED(pwm_a);
	SR_MCPWM_UPDATE_DUTY_B_DELAYED(pwm_b);
	SR_MCPWM_UPDATE_DUTY_C_DELAYED(pwm_c);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Start the PWM timer counter.
 *
 */
void SRMCPwm__StartCounter(void)
{
	PWM_TIMER_START();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Stop the timer counter.
 *
 * @remark      This method also turns off the PWM ISR because the timer will be stopped.
 */
void SRMCPwm__StopCounter(void)
{
	PWM_TIMER_STOP();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Enable the PWM output according the parameter gate_name.
 *              It connects the PWM output to the internal PWM peripheral.
 *
 * @param[in]   gate_name Selects the gate to be connected.
 */
void SRMCPwm__EnablePwmOutput(SRMCPWM_GATE_NAME_TYPE gate_name)
{
	switch(gate_name)
	{
	    case SR_MCPWM_GATE_A_HI:
	    	FTM0.OUTMASK &= ~(FTM_OUTMASK_CH4OM_MASK);
	        break;
	    case SR_MCPWM_GATE_B_HI:
	    	FTM0.OUTMASK &= ~(FTM_OUTMASK_CH2OM_MASK);
	        break;
	    case SR_MCPWM_GATE_C_HI:
	    	FTM0.OUTMASK &= ~(FTM_OUTMASK_CH0OM_MASK);
	        break;
	    case SR_MCPWM_GATE_ALL_HI:
	    	FTM0.OUTMASK &= ~(FTM_OUTMASK_CH0OM_MASK | FTM_OUTMASK_CH2OM_MASK |FTM_OUTMASK_CH4OM_MASK);
	        break;
	    case SR_MCPWM_GATE_A_LO:
	    	FTM0.OUTMASK &= ~(FTM_OUTMASK_CH5OM_MASK);
	        break;
	    case SR_MCPWM_GATE_B_LO:
	    	FTM0.OUTMASK &= ~(FTM_OUTMASK_CH3OM_MASK);
	        break;
	    case SR_MCPWM_GATE_C_LO:
	    	FTM0.OUTMASK &= ~(FTM_OUTMASK_CH1OM_MASK);
	        break;
	    case SR_MCPWM_GATE_ALL_LO:
	    	FTM0.OUTMASK &= ~(FTM_OUTMASK_CH1OM_MASK | FTM_OUTMASK_CH3OM_MASK |FTM_OUTMASK_CH5OM_MASK);
	        break;
	    case SR_MCPWM_GATE_ALL_6GATES:
	    	PWM_OUTPUT_ON()
			break;
	    default:
	        break;
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Disables PWM output according the parameter gate_name.
 *              It disconnects the PWM output to the internal PWM peripheral.
 *
 * @param[in]   gate_name Selects the gate to be disconnected.
 */
void SRMCPwm__DisablePwmOutput(SRMCPWM_GATE_NAME_TYPE gate_name)
{
	switch(gate_name)
	{
	    case SR_MCPWM_GATE_A_HI:
	    	FTM0.OUTMASK |= (FTM_OUTMASK_CH4OM_MASK);;
	        break;
	    case SR_MCPWM_GATE_B_HI:
	    	FTM0.OUTMASK |= (FTM_OUTMASK_CH2OM_MASK);
	        break;
	    case SR_MCPWM_GATE_C_HI:
	    	FTM0.OUTMASK |= (FTM_OUTMASK_CH0OM_MASK);
	        break;
	    case SR_MCPWM_GATE_ALL_HI:
	    	FTM0.OUTMASK |= (FTM_OUTMASK_CH0OM_MASK | FTM_OUTMASK_CH2OM_MASK |FTM_OUTMASK_CH4OM_MASK);
	        break;
	    case SR_MCPWM_GATE_A_LO:
	    	FTM0.OUTMASK |= (FTM_OUTMASK_CH5OM_MASK);
	        break;
	    case SR_MCPWM_GATE_B_LO:
	    	FTM0.OUTMASK |= (FTM_OUTMASK_CH3OM_MASK);
	        break;
	    case SR_MCPWM_GATE_C_LO:
	    	FTM0.OUTMASK |= (FTM_OUTMASK_CH1OM_MASK);
	        break;
	    case SR_MCPWM_GATE_ALL_LO:
	    	FTM0.OUTMASK |= (FTM_OUTMASK_CH1OM_MASK | FTM_OUTMASK_CH3OM_MASK |FTM_OUTMASK_CH5OM_MASK);
	        break;
	    case SR_MCPWM_GATE_ALL_6GATES:
	    	PWM_OUTPUT_OFF();
			break;
	    default:
	        break;
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the specified PWM frequency.
 *
 * @return      Frequency in Hertz, decimal and no fractional number.
 */
uint16 SRMCPwm__GetPwmFrequencyHz(void)
{
	return (PwmFrequency);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the configured dead time in the PWM peripheral.
 *
 * @return      Dead time in nanoseconds, decimal and no fractional number.
 */
uint16 SRMCPwm__GetDeadtimeNanosec(void)
{
	return (DeadTimeNanosec);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the Fault Status of the peripheral.
 *
 * @details     This function returns over current or over temperature error from driver module.
 *
 * @return
 */
uint8 SRMCPwm__GetHwFaultStatus(void)
{
    uint8 temp = FALSE;                     // Starts with no fault reported
    uint32 temp1;

    temp1 = SR_MCPWM_CHECK_DIAG_STATE();            // Checks state data

    if(temp1 == SRDATA_OK)                          // If data is not corrupted
    {
        temp1 = SR_MCPwm_Diag_State;

        if( (temp1 == SR_MCPWM_FEEDBACK_UPDATED) ||
            (temp1 == SR_MCPWM_WAITING_TO_CHECK) )  // Checks if it already finished a check or not started
        {
          
            if(GetFaultFlagState() != 0)
            {
                temp = TRUE;                        // Failure detected

            }
        }
    }
    return(temp);                                   // Returns status
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 *
 */
void SRMCPwm__Handler250us(void)
{



}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Triggers the handler to start, if the handler is available.
 *
 * @param[in]   timeout_to_zero Counts the time that the micro will force the signal to zero logic
 * @param[in]   timeout_to_one Counts the time that the micro will force the signal to one logic
 */
void SRMCPwm__EnableDiagnostic(uint32 timeout_to_zero, uint32 timeout_to_one)
{
    uint32 temp;

    temp = SR_MCPWM_CHECK_ZERO_TIMEOUT();
    temp += SR_MCPWM_CHECK_ONE_TIMEOUT();
    temp += SR_MCPWM_CHECK_DIAG_STATE();
    temp += SR_MCPWM_CHECK_ERRORS();

    if(temp == SRDATA_OK)
    {
        temp = SR_MCPwm_Diag_State;

        if(temp == SR_MCPWM_WAITING_TO_CHECK)
        {
            SR_MCPWM_UPDATE_ZERO_TIMEOUT(timeout_to_zero);
            SR_MCPWM_UPDATE_ONE_TIMEOUT(timeout_to_one);
            SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_CHECK_SIGNAL_POL);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Peripheral Diagnostic handler
 *
 * @details     Manipulates the shut down input in order to perform
 *              the plausibility checks.
 */
void SRMCPwm__DiagnosticHandler(void)
{
    uint32 temp;
    uint32 temp1;
    uint32 temp2;

    temp = SR_MCPWM_CHECK_DIAG_STATE();             // Checks state integrity state
    if(temp == SRDATA_OK)                           // Continues check if OK, in case of corruption a reset will be forced
    {
        temp = SR_MCPwm_Diag_State;                 // Saves state into temp

        if(temp == SR_MCPWM_WAITING_TO_CHECK)       // Keeps here waiting for an update, but is always checking state
        {
            temp  = SR_MCPWM_CHECK_ACTIVE_PULSE_DURATION();
            temp += SR_MCPWM_CHECK_DEFAULT_PULSE_DURATION();
            
            if(temp == SRDATA_OK)                           // Continues check if OK, in case of corruption a reset will be forced
            {
                SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(0);
                SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(0);
            }
        }

        // Check if the shut down is INACTIVE.
        else if(temp == SR_MCPWM_CHECK_SIGNAL_POL)  // If was commanded to check SD pin, starts by reading signals
        {
            temp = SR_MCPWM_CHECK_ERRORS();
            temp += SR_MCPWM_CHECK_ACTIVE_PULSE_DURATION();
            temp += SR_MCPWM_CHECK_DEFAULT_PULSE_DURATION();
            
            if(temp == SRDATA_OK)                   // Checks data integrity
            {
//                Gpio__PinConfig(PORT7,0, INPUT_PULLUP);
            	Comparator__SetThr(SR_COMPARATOR_CHANNEL, SR_PWM_FAULT_0_CMP_LEV);

                temp1 = SR_MCPwm_Pulse_Active_Level;
                temp2 = SR_MCPwm_Pulse_Default_Level;
//                temp = Gpio__PinRead(PORT7,0);
                temp = GetFaultState();

                if(temp == SR_MCPWM_SHUTDOWN_ACTIVE_POLARITY)
                {
                    temp1 = SR_MCPwm_Pulse_Active_Level;
                    temp1++;
                    SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(temp1);
                }
                else
                {
                    temp2 = SR_MCPwm_Pulse_Default_Level;
                    temp2++;
                    SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(temp2);
                }
#ifdef FORCE_SHUTDOWN_FAILURE_ALREADY_ACTIVE
                temp1 = SR_MCPWM_DIAG_SAMPLES_4_DECISION;
#endif

                if((temp1 + temp2) >= SR_MCPWM_DIAG_TOTAL_SAMPLES_NR)
                {                                   // after 2.5ms sampling
                    if (temp1 >= SR_MCPWM_DIAG_SAMPLES_4_DECISION)
                    {                               // 60% samples showing SD active
                        // The shut down circuit is already activated!
                        temp = SR_MCPwm_Errors;     // Read errors
                        BIT_SET(temp, SR_MCPWM_ERROR_SHUTDOWN_ALREADY_ON);
                        SR_MCPWM_UPDATE_ERRORS((SRMCPWM_DIAG_ERROR_TYPE)temp);
                        SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_FEEDBACK_UPDATED);
                    }
                    else                            // SD pin is not active
                    {
                        // The shut down circuit is already deactivated!
                        temp = SR_MCPwm_Errors;     // We are reading and clearing an error which was not set (really needed???)
                        BIT_CLR(temp, SR_MCPWM_ERROR_SHUTDOWN_ALREADY_ON);
                        SR_MCPWM_UPDATE_ERRORS((SRMCPWM_DIAG_ERROR_TYPE)temp);
                        // There are specific feedacks for errors, but we don't report them to application, is it also needed
  //                      Gpio__PinConfig(PORT7,0, OUTPUT_OPEN_DRAIN);
  //                      Gpio__PinWrite(PORT7,0, SR_MCPWM_SHUTDOWN_ACTIVE_POLARITY);
                        Comparator__SetThr(SR_COMPARATOR_CHANNEL, SR_PWM_FAULT_0_CMP_LEV_TEST);

                        temp1 = 0;
                        temp2 = 0;
                        SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(temp1);
                        SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(temp2);
                        // Commands output to force disabling SD pin

                        SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_CHECK_STUCK_AT_1);
                                                // Gets to the state to detect if it's stuck at 1
                    }                               

                }                               // Gets final state
            }
        }
        else if(temp == SR_MCPWM_CHECK_STUCK_AT_1)
        {
            temp  = SR_MCPWM_CHECK_ZERO_TIMEOUT();
            temp += SR_MCPWM_CHECK_ERRORS();        // Checks timers, error flags and flow flags
            temp += SR_MCPWM_CHECK_ACTIVE_PULSE_DURATION();
            temp += SR_MCPWM_CHECK_DEFAULT_PULSE_DURATION();
            
            if(temp == SRDATA_OK)                   // In case data wasn't corrupted
            {
                temp = SR_MCPwm_Zero_Timeout;
                if(temp)
                {
                    temp--;                         // Decrements and updates timer
                    SR_MCPWM_UPDATE_ZERO_TIMEOUT(temp);
                }
                else                                // After a while (2.5ms forcing null)
                {
 //                   Gpio__PinConfig(PORT7,0, INPUT_PULLUP);
 //               	Comparator__SetThr(SR_COMPARATOR_CHANNEL, SR_PWM_FAULT_0_CMP_LEV);

                    temp1 = SR_MCPwm_Pulse_Active_Level;
                    temp2 = SR_MCPwm_Pulse_Default_Level;

  //                  temp = Gpio__PinRead(PORT7,0);
                    temp = GetFaultState();

                    if(temp == SR_MCPWM_SHUTDOWN_ACTIVE_POLARITY)
                    {
                        temp1 = SR_MCPwm_Pulse_Active_Level;
                        temp1++;
                        SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(temp1);
                    }
                    else
                    {
                        temp2 = SR_MCPwm_Pulse_Default_Level;
                        temp2++;
                        SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(temp2);
                    }
#ifdef FORCE_SHUTDOWN_FAILURE_STUCK_AT_1
                    temp1 = 0;
#endif
                    if((temp1 + temp2) >= SR_MCPWM_DIAG_TOTAL_SAMPLES_NR)
                    {                                   // after 2.5ms sampling
                        if (temp1 >= SR_MCPWM_DIAG_SAMPLES_4_DECISION)
                        {                               // 60% samples showing SD active
                            // The register is NOT stuck at 1!
                            temp = SR_MCPwm_Errors;
                            BIT_CLR(temp, SR_MCPWM_ERROR_REGISTER_STUCK_AT_1);
                            SR_MCPWM_UPDATE_ERRORS((SRMCPWM_DIAG_ERROR_TYPE)temp);
                            Comparator__SetThr(SR_COMPARATOR_CHANNEL, SR_PWM_FAULT_0_CMP_LEV);
                            temp1 = 0;
                            temp2 = 0;
                            SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(temp1);
                            SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(temp2);

                            SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_CHECK_STUCK_AT_0);
                                                       // Changes state to check if it's stuck at zero (I don't know if it is redundant, since first check was checking if not zero)
                        }                               
                        else
                        {
                            // The register IS stuck at 1!
                            temp = SR_MCPwm_Errors;
                            BIT_SET(temp, SR_MCPWM_ERROR_REGISTER_STUCK_AT_1);
                            SR_MCPWM_UPDATE_ERRORS((SRMCPWM_DIAG_ERROR_TYPE)temp);
                                                        // Updates error

                            SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_FEEDBACK_UPDATED);
                        }                               // Ends check since error was detected
                    }
                }
            }
        }
        else if(temp == SR_MCPWM_CHECK_STUCK_AT_0)  // Checks if not stuck at zero
        {
            temp  = SR_MCPWM_CHECK_ONE_TIMEOUT();
            temp += SR_MCPWM_CHECK_ERRORS();
            temp += SR_MCPWM_CHECK_ACTIVE_PULSE_DURATION();
            temp += SR_MCPWM_CHECK_DEFAULT_PULSE_DURATION();

            if(temp == SRDATA_OK)                   // Since data is not corrupted (I don't know if all these tests are really needed, in case of corruption it will force reset
            {
                temp = SR_MCPwm_One_Timeout;
                if(temp)
                {
                    temp--;
                    SR_MCPWM_UPDATE_ONE_TIMEOUT(temp);
                }                                   // Waits 20ms to release condition
                else
                {
                    temp1 = SR_MCPwm_Pulse_Active_Level;
                    temp2 = SR_MCPwm_Pulse_Default_Level;
//                    temp = Gpio__PinRead(PORT7,0);
                    temp = GetFaultState();

                    if(temp == SR_MCPWM_SHUTDOWN_ACTIVE_POLARITY)
                    {
                        temp1 = SR_MCPwm_Pulse_Active_Level;
                        temp1++;
                        SR_MCPWM_UPDATE_ACTIVE_PULSE_DURATION(temp1);
                    }
                    else
                    {
                        temp2 = SR_MCPwm_Pulse_Default_Level;
                        temp2++;
                        SR_MCPWM_UPDATE_DEFAULT_PULSE_DURATION(temp2);
                    }

#ifdef FORCE_SHUTDOWN_FAILURE_STUCK_AT_0
                    temp2 = 0;
#endif
                    if((temp1 + temp2) >= SR_MCPWM_DIAG_TOTAL_SAMPLES_NR)
                    {                                   // after 2.5ms sampling
                        if (temp2 >= SR_MCPWM_DIAG_SAMPLES_4_DECISION)
                        {                               // 60% samples showing SD active
                            // The register is NOT stuck at 0!
                            temp = SR_MCPwm_Errors; // It's not checking to read here
                            BIT_CLR(temp, SR_MCPWM_ERROR_REGISTER_STUCK_AT_0);
                            BIT_SET(temp, SR_MCPWM_ERROR_PERIPHERAL_SAFE);
                            SR_MCPWM_UPDATE_ERRORS((SRMCPWM_DIAG_ERROR_TYPE)temp);

                            SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_FEEDBACK_UPDATED);
                            // Updates state machine
                            if (GetFaultFlagState() != 0)
                            {
                                ClearFaultFlag();
                            }
                        }
                        else                        // In case of failure
                        {
                            // The register IS stuck at 1!
                            temp = SR_MCPwm_Errors;
                            BIT_SET(temp, SR_MCPWM_ERROR_REGISTER_STUCK_AT_0);
                            SR_MCPWM_UPDATE_ERRORS((SRMCPWM_DIAG_ERROR_TYPE)temp);
                            // Reports the error
                            SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_FEEDBACK_UPDATED);
                        }
                    }
                }
            }
        }
        else if(temp == SR_MCPWM_FEEDBACK_UPDATED)
        {
            // Stay here until the feedback is read by the caller.
        }
        else
        {
            // Should never reach this region.
            // Just in case, should force to a safe condition here
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the feedback of the plausibility check
 *
 * @details     Gets the plausibility check result, when the state machine is fully executed.
 *              As soon as the state machine reaches its final state, it will update the feedback
 *              which will be read by this method, once the state machine has reached its final state,
 *              and once this method is called, then it will trigger the state machine to go back to
 *              its very state and reset the integrity feedback of the circuit.
 *              This action prevents two reading in sequence of a safe hardware, i.e., once it gets
 *              the feedback, the state machine must be ran again to update the status.
 *
 * @return      The plausibility check result.
 */
SRMCPWM_DIAG_ERROR_TYPE SRMCPwm__GetDiagnosticFeedback(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint32 temp;
    uint32 temp1;
    SRMCPWM_DIAG_ERROR_TYPE current_status;

    SR_MCPWM_CHECK_DIAG_STATE();
    SR_MCPWM_CHECK_ERRORS();

    temp = SR_MCPwm_Diag_State;

    if(temp == SR_MCPWM_FEEDBACK_UPDATED)
    {
       MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
       current_status = SR_MCPWM_ERROR_PERIPHERAL_NOT_SAFE;

       temp  = SR_MCPwm_Errors;
       temp1  = BIT_TEST(temp, SR_MCPWM_ERROR_SHUTDOWN_ALREADY_ON);
       temp1 += BIT_TEST(temp, SR_MCPWM_ERROR_REGISTER_STUCK_AT_1);
       temp1 += BIT_TEST(temp, SR_MCPWM_ERROR_REGISTER_STUCK_AT_0);

       if(!temp1)
       {
           current_status = SR_MCPWM_ERROR_PERIPHERAL_SAFE;
       }

       SR_MCPWM_UPDATE_DIAG_STATE(SR_MCPWM_WAITING_TO_CHECK);
       MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }
    else if(temp == SR_MCPWM_WAITING_TO_CHECK)
    {
        current_status = SR_MCPWM_ERROR_WAITING_START_TRIGGER;
    }
    else
    {
        current_status = SR_MCPWM_ERROR_CHECKING;
    }


    return(current_status);                                 // Returns status
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Variable PWM Frequency: update the PWM period
 *
 * @details     It updates the pwm switching period according to the percentage of the nominal value.
 *
 * @param[in]   pwm_period_perc The percentage of the nominal period we want to apply in the next pwm cycle.
 *              pwm_period perc is expressed in 17.15 fixed point notation (base value is 100% --> 1)
 */
void SRMCPwm__UpdatePeriod(uint32 pwm_period_perc)
{
    uint16 pwm_period;
    uint16 mod;

    FTM0.MODE |= FTM_MODE_WPDIS_MASK;           // Disable write protection for certain registers
    // calculate new pwm_period
    pwm_period = (uint16)(((uint32)NominalPwmPeriod * pwm_period_perc) >> 15);

    // Update MaxPwm value
    MaxPwm = pwm_period>>1;


    // Update Timer modulus register and initialized value for CNT
    mod = (uint16)((pwm_period>>1)-1);
    FTM0.MOD = mod;
    FTM0.CNTIN = (uint32) (-(sint32)mod-1);                    // Set initial counting value

    FTM0.FMS |= FTM_FMS_WPEN_MASK;                  // Protect the Registers - Write Protection Enabled
}


//=====================================================================================================================
//-------------------------------------- Private Functions -------------------------------------------------------------
//=====================================================================================================================

/**
 * @brief       Get the fault state
 *
 * @return      TRUE, if there is a fault, FALSE otherwise
 *
 */
static BOOL_TYPE GetFaultState(void)
{
	BOOL_TYPE response;
	uint16 temp;

	temp = FTM0.FMS&FTM_FMS_FAULTIN_MASK;

	if(temp != 0)
	{
		response = TRUE;
	}
	else
	{
		response = FALSE;
	}

	return (response);
}

/**
 * @brief       Get the fault flag state
 *
 * @return      TRUE, if there is a fault, FALSE otherwise
 *
 */
static BOOL_TYPE GetFaultFlagState(void)
{
	BOOL_TYPE response;
	uint16 temp;

	temp = FTM0.FMS&FTM_FMS_FAULTF_MASK;

	if(temp != 0)
	{
		response = TRUE;
	}
	else
	{
		response = FALSE;
	}

	return (response);
}

/**
 * @brief       Clear the fault flag.
 *
 *
 */
static void ClearFaultFlag(void)
{

	FTM0.FMS &= ~(FTM_FMS_FAULTF_MASK|FTM_FMS_FAULTF3_MASK|FTM_FMS_FAULTF2_MASK|FTM_FMS_FAULTF1_MASK|FTM_FMS_FAULTF0_MASK);
}



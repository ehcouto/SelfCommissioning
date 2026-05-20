/**
 *  @defgroup   CLASS_B
 *  @file       SRMCInputCapture.c
 *  @brief      This module treats the Input Capture peripheral for motor control
 *
 *  @details    InputCapture implementation for KV3x Controllers
 *
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRMCInputCapture.h"
#include "SRMCInputCapture_prm.h"
#include "SRMCInputCapture_prv.h"
#include "C_Extensions.h"
#include "Micro_defs.h"
#include "MathCalc.h"
#include "SRData.h"

#include "mci_prm.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#ifndef MAX_U32
    #define MAX_U32     4294967295UL                //!< Maximum of uint32
#endif

#ifndef MAX_U16
    #define MAX_U16     65535                       //!< Maximum of unsigned short
#endif


#if (INPUTCAPTURE_MODULE == 0)
	#define IC_CLK_EN_MASK SIM_SCGC6_FTM0_MASK
	#define IC_BASE_REG IC_MODULE_0
#endif
#if (INPUTCAPTURE_MODULE == 1)
	#define IC_CLK_EN_MASK SIM_SCGC6_FTM1_MASK
	#define IC_BASE_REG IC_MODULE_1
#endif
#if (INPUTCAPTURE_MODULE == 2)
	#define IC_CLK_EN_MASK SIM_SCGC6_FTM2_MASK
	#define IC_BASE_REG IC_MODULE_2
#endif


//! Plausibility check for the minimum IC counter value
#define SR_PLAUSIBILITY_MIN_COUNTER_VALUE       284  // Same as OmegaScale

//! Plausibility check for the maximum IC counter difference
#define SR_PLAUSIBILITY_MAX_DIFFERENCE_CONST    2283 // DeltaOmega = 2463 rpm_M -> OmegaScale * BaseSpeed / (DeltaOmega)

//! Maximum number of maximum speed change difference errors before declaring the fault
#define SR_MAX_SPEED_CHANGE_CNT_THR             5000

//! Threshold for speed region discrimination
#define SR_HIGH_SPEED_REGION_THR                5623 // 1000 rpm_M

//! Maximum number of minimum IC counter events before declaring the fault
#define SR_MIN_TACH_CNT_THR                     5000

//! Fictitious ramp duration
#define SR_FICT_RAMP_DOWN_COUNTER               50000 // 250 seconds @ 5 ms

//! Undertorque pulse threshold
#define SR_UNDERTORQUE_PULSE_THR                200

//-------------------------------------- Safety Relevant Variables ------------------------------------------------

//! [n/a] total of overflow captured between two captures
static uint32 SR_IC_OverFlow_Total;

//! [n/a] total of overflow captured between two captures
static uint32 SR_IC_OverFlow_Total_k1;

//! [n/a] counts the input capture overflow; temporary variable
static uint32 SR_IC_OverFlow_Counter;

//! TRUE a capture event has happened once; No event otherwise
static uint8 SR_Capture_Event;

//! Match event counter
static uint32 SR_Match_Event_Counter;

//! Plausibility counter for maximum detectable speed
static uint32 SR_Plausibility_Min_Counter;

//! Plausibility counter for maximum speed variation
static uint32 SR_Plausibility_Max_Counter;

//! Fictitious ramp counter
static uint32 SR_Fict_Ramp_Counter;

//! High speed region status
static BOOL_TYPE SR_High_Speed_Region;

//! Pulse counter for undertorque management
static uint32 SR_Pulse_Counter;

//! External request to reset the SR_Pulse_Counter
static uint8 SR_Reset_Pulse_Flag;

//! Diagnostic handler status
static SRMCINPUTCAPTURE_DIAG_ERROR_TYPE SR_Diagnostic_Status;

static uint32 NSR_IC_OverFlow_Total;
static uint32 NSR_IC_OverFlow_Total_k1;
static uint32 NSR_IC_OverFlow_Counter;
static uint8 NSR_Capture_Event;
static uint32 NSR_Match_Event_Counter;
static uint32 NSR_Plausibility_Min_Counter;
static uint32 NSR_Plausibility_Max_Counter;
static uint32 NSR_Fict_Ramp_Counter;
static BOOL_TYPE NSR_High_Speed_Region;
static uint32 NSR_Pulse_Counter;
static uint8 NSR_Reset_Pulse_Flag;
static SRMCINPUTCAPTURE_DIAG_ERROR_TYPE NSR_Diagnostic_Status;

//---------------------------------------- Macros -----------------------------------------------------

#if(1)
// CHECK Macro defs
//lint -emacro( 929, SR_MCIC_CHECK_OVERFLOW_TOTAL )
#define SR_MCIC_CHECK_OVERFLOW_TOTAL()                     SRData__CheckLong((unsigned long *)&SR_IC_OverFlow_Total, (unsigned long *)&NSR_IC_OverFlow_Total)
//lint -emacro( 929, SR_MCIC_CHECK_OVERFLOW_TOTAL_K1 )
#define SR_MCIC_CHECK_OVERFLOW_TOTAL_K1()                  SRData__CheckLong((unsigned long *)&SR_IC_OverFlow_Total_k1, (unsigned long *)&NSR_IC_OverFlow_Total_k1)
//lint -emacro( 929, SR_MCIC_CHECK_OVERFLOW_CNTR )
#define SR_MCIC_CHECK_OVERFLOW_CNTR()                      SRData__CheckLong((unsigned long *)&SR_IC_OverFlow_Counter, (unsigned long *)&NSR_IC_OverFlow_Counter)
//lint -emacro( 926, SR_MCIC_CHECK_CAPTURE_EVENT )
#define SR_MCIC_CHECK_CAPTURE_EVENT()                      SRData__CheckByte((unsigned char *)&SR_Capture_Event, (unsigned char *)&NSR_Capture_Event)
//lint -emacro( 929, SR_MCIC_CHECK_MATCH_EVENT_CNTR )
#define SR_MCIC_CHECK_MATCH_EVENT_CNTR()                   SRData__CheckLong((unsigned long *)&SR_Match_Event_Counter, (unsigned long *)&NSR_Match_Event_Counter)
//lint -emacro( 929, SR_MCIC_CHECK_PLAUSIBILITY_MIN_CNTR )
#define SR_MCIC_CHECK_PLAUSIBILITY_MIN_CNTR()              SRData__CheckLong((unsigned long *)&SR_Plausibility_Min_Counter, (unsigned long *)&NSR_Plausibility_Min_Counter)
//lint -emacro( 929, SR_MCIC_CHECK_PLAUSIBILITY_MAX_CNTR )
#define SR_MCIC_CHECK_PLAUSIBILITY_MAX_CNTR()              SRData__CheckLong((unsigned long *)&SR_Plausibility_Max_Counter, (unsigned long *)&NSR_Plausibility_Max_Counter)
//lint -emacro( 929, SR_MCIC_CHECK_FICT_RAMP_CNTR )
#define SR_MCIC_CHECK_FICT_RAMP_CNTR()                     SRData__CheckLong((unsigned long *)&SR_Fict_Ramp_Counter, (unsigned long *)&NSR_Fict_Ramp_Counter)
//lint -emacro( 928, SR_MCIC_CHECK_HIGH_SPEED_REGION )
#define SR_MCIC_CHECK_HIGH_SPEED_REGION()                  SRData__CheckByte((unsigned char *)&SR_High_Speed_Region, (unsigned char *)&NSR_High_Speed_Region)
//lint -emacro( 929, SR_MCIC_CHECK_PULSE_CNTR )
#define SR_MCIC_CHECK_PULSE_CNTR()                         SRData__CheckLong((unsigned long *)&SR_Pulse_Counter, (unsigned long *)&NSR_Pulse_Counter)
//lint -emacro( 926, SR_MCIC_CHECK_RESET_PULSE_FLAG )
#define SR_MCIC_CHECK_RESET_PULSE_FLAG()                   SRData__CheckByte((unsigned char *)&SR_Reset_Pulse_Flag, (unsigned char *)&NSR_Reset_Pulse_Flag)
//lint -emacro( 928, SR_MCIC_CHECK_DIAGNOSTIC_STATUS )
#define SR_MCIC_CHECK_DIAGNOSTIC_STATUS()                  SRData__CheckByte((unsigned char *)&SR_Diagnostic_Status, (unsigned char *)&NSR_Diagnostic_Status)

// UPDATE Macro defs
//lint -emacro( 929, SR_MCIC_UPDATE_OVERFLOW_TOTAL )
#define SR_MCIC_UPDATE_OVERFLOW_TOTAL(value)               SRData__UpdateLong((unsigned long *)&SR_IC_OverFlow_Total, (unsigned long *)&NSR_IC_OverFlow_Total, (unsigned long)(value))
//lint -emacro( 929, SR_MCIC_UPDATE_OVERFLOW_TOTAL_K1 )
#define SR_MCIC_UPDATE_OVERFLOW_TOTAL_K1(value)            SRData__UpdateLong((unsigned long *)&SR_IC_OverFlow_Total_k1, (unsigned long *)&NSR_IC_OverFlow_Total_k1, (unsigned long)(value))
//lint -emacro( 929, SR_MCIC_UPDATE_OVERFLOW_CNTR )
#define SR_MCIC_UPDATE_OVERFLOW_CNTR(value)                SRData__UpdateLong((unsigned long *)&SR_IC_OverFlow_Counter, (unsigned long *)&NSR_IC_OverFlow_Counter, (unsigned long)(value))
//lint -emacro( 926, SR_MCIC_UPDATE_CAPTURE_EVENT )
#define SR_MCIC_UPDATE_CAPTURE_EVENT(value)                SRData__UpdateByte((unsigned char *)&SR_Capture_Event, (unsigned char *)&NSR_Capture_Event, (unsigned char)(value))
//lint -emacro( 929, SR_MCIC_UPDATE_MATCH_EVENT_CNTR )
#define SR_MCIC_UPDATE_MATCH_EVENT_CNTR(value)             SRData__UpdateLong((unsigned long *)&SR_Match_Event_Counter, (unsigned long *)&NSR_Match_Event_Counter, (unsigned long)(value))
//lint -emacro( 929, SR_MCIC_UPDATE_PLAUSIBILITY_MIN_CNTR )
#define SR_MCIC_UPDATE_PLAUSIBILITY_MIN_CNTR(value)        SRData__UpdateLong((unsigned long *)&SR_Plausibility_Min_Counter, (unsigned long *)&NSR_Plausibility_Min_Counter, (unsigned long)(value))
//lint -emacro( 929, SR_MCIC_UPDATE_PLAUSIBILITY_MAX_CNTR )
#define SR_MCIC_UPDATE_PLAUSIBILITY_MAX_CNTR(value)        SRData__UpdateLong((unsigned long *)&SR_Plausibility_Max_Counter, (unsigned long *)&NSR_Plausibility_Max_Counter, (unsigned long)(value))
//lint -emacro( 929, SR_MCIC_UPDATE_FICT_RAMP_CNTR )
#define SR_MCIC_UPDATE_FICT_RAMP_CNTR(value)               SRData__UpdateLong((unsigned long *)&SR_Fict_Ramp_Counter, (unsigned long *)&NSR_Fict_Ramp_Counter, (unsigned long)(value))
//lint -emacro( 928, SR_MCIC_UPDATE_HIGH_SPEED_REGION )
#define SR_MCIC_UPDATE_HIGH_SPEED_REGION(value)            SRData__UpdateByte((unsigned char *)&SR_High_Speed_Region, (unsigned char *)&NSR_High_Speed_Region, (unsigned char)(value))
//lint -emacro( 929, SR_MCIC_UPDATE_PULSE_CNTR )
#define SR_MCIC_UPDATE_PULSE_CNTR(value)                   SRData__UpdateLong((unsigned long *)&SR_Pulse_Counter, (unsigned long *)&NSR_Pulse_Counter, (unsigned long)(value))
//lint -emacro( 926, SR_MCIC_UPDATE_RESET_PULSE_FLAG )
#define SR_MCIC_UPDATE_RESET_PULSE_FLAG(value)             SRData__UpdateByte((unsigned char *)&SR_Reset_Pulse_Flag, (unsigned char *)&NSR_Reset_Pulse_Flag, (unsigned char)(value))
//lint -emacro( 928, SR_MCIC_UPDATE_DIAGNOSTIC_STATUS )
#define SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(value)            SRData__UpdateByte((unsigned char *)&SR_Diagnostic_Status, (unsigned char *)&NSR_Diagnostic_Status, (unsigned char)(value))


#else
// CHECK Macro defs
#define SR_MCIC_CHECK_OVERFLOW_TOTAL()                     SRDATA_OK
#define SR_MCIC_CHECK_OVERFLOW_TOTAL_K1()                  SRDATA_OK
#define SR_MCIC_CHECK_OVERFLOW_CNTR()                      SRDATA_OK
#define SR_MCIC_CHECK_CAPTURE_EVENT()                      SRDATA_OK
#define SR_MCIC_CHECK_MATCH_EVENT_CNTR()                   SRDATA_OK
#define SR_MCIC_CHECK_PLAUSIBILITY_MIN_CNTR()              SRDATA_OK
#define SR_MCIC_CHECK_PLAUSIBILITY_MAX_CNTR()              SRDATA_OK
#define SR_MCIC_CHECK_FICT_RAMP_CNTR()                     SRDATA_OK
#define SR_MCIC_CHECK_HIGH_SPEED_REGION()                  SRDATA_OK
#define SR_MCIC_CHECK_PULSE_CNTR()                         SRDATA_OK
#define SR_MCIC_CHECK_RESET_PULSE_FLAG()                   SRDATA_OK
#define SR_MCIC_CHECK_DIAGNOSTIC_STATUS()                  SRDATA_OK

// UPDATE Macro defs
#define SR_MCIC_UPDATE_OVERFLOW_TOTAL(value)               SR_IC_OverFlow_Total = (value)
#define SR_MCIC_UPDATE_OVERFLOW_TOTAL_K1(value)            SR_IC_OverFlow_Total_k1 = (value)
#define SR_MCIC_UPDATE_OVERFLOW_CNTR(value)                SR_IC_OverFlow_Counter = (value)
#define SR_MCIC_UPDATE_CAPTURE_EVENT(value)                SR_Capture_Event = (value)
#define SR_MCIC_UPDATE_MATCH_EVENT_CNTR(value)             SR_Match_Event_Counter = (value)
#define SR_MCIC_UPDATE_PLAUSIBILITY_MIN_CNTR(value)        SR_Plausibility_Min_Counter = (value)
#define SR_MCIC_UPDATE_PLAUSIBILITY_MAX_CNTR(value)        SR_Plausibility_Max_Counter = (value)
#define SR_MCIC_UPDATE_FICT_RAMP_CNTR(value)               SR_Fict_Ramp_Counter = (value)
#define SR_MCIC_UPDATE_HIGH_SPEED_REGION(value)            SR_High_Speed_Region = (value)
#define SR_MCIC_UPDATE_PULSE_CNTR(value)                   SR_Pulse_Counter = (value)
#define SR_MCIC_UPDATE_RESET_PULSE_FLAG(value)             SR_Reset_Pulse_Flag = (value)
#define SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(value)            SR_Diagnostic_Status = (value)
#endif
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static inline BOOL_TYPE IsPeripheralClockEnabled(void);
static inline void DiagnosticHandler(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRMCInputCapture and its variables
 *
 */
void SRMCInputCapture__Initialize(void)
{
    SRMCINPUTCAPTURE_FLOW_INITIALIZE_BEGIN();
	/* enable clock to the selected module */
	SIM.SCGC6 |= IC_CLK_EN_MASK;

	IC_BASE_REG.MODE |= FTM_MODE_WPDIS_MASK; /* Disable write protection for certain registers */
	IC_BASE_REG.MODE |= FTM_MODE_FTMEN_MASK; /* Enable the counter */

	/* Settings up FTM SC register for clock setup */
	IC_BASE_REG.CNTIN = 0x0;
	IC_BASE_REG.MOD = 0xFFFF;

	IC_BASE_REG.SC |= FTM_SC_CLKS(1); /* Set bus clock as source for FTM0 (CLKS[1:0] = 01) */

	IC_BASE_REG.SC |= FTM_SC_PS(INPUT_CAPTURE_PS);  /* Set prescaler */
	IC_BASE_REG.CONTROLS[INPUTCAPTURE_CHANNEL].CnSC |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK | FTM_CnSC_ICRST_MASK);   /* Input capture on both edges, reset CNT on capture */

	// todo: this needs to be configurable, but it is chip specific
#if (INPUTCAPTURE_MODULE == 1)
	PCIPORTD.PCR[7]  = PORT_PCR_MUX(5); /* Setting PTD7 as FTM1 input pin */
#else
    #error "Please set the proper input pin for the selected IC peripheral!"
#endif
	// Initialize the data to be handled while running input capture
	// Zero the variables used by the input capture algorithm
	SR_MCIC_UPDATE_OVERFLOW_CNTR(0);
	SR_MCIC_UPDATE_OVERFLOW_TOTAL(MAX_U32);
	SR_MCIC_UPDATE_OVERFLOW_TOTAL_K1(MAX_U32);
	SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(SR_IC_TEST_NOT_INITIALIZED);
    SR_MCIC_UPDATE_MATCH_EVENT_CNTR(0);
    SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(SR_IC_TEST_NOT_PERFORMED);
    SR_MCIC_UPDATE_PLAUSIBILITY_MIN_CNTR(0);
    SR_MCIC_UPDATE_PLAUSIBILITY_MAX_CNTR(0);
    SR_MCIC_UPDATE_FICT_RAMP_CNTR(SR_FICT_RAMP_DOWN_COUNTER);
    SR_MCIC_UPDATE_MATCH_EVENT_CNTR(0);
    // Undertorque management
    SR_MCIC_UPDATE_PULSE_CNTR(0);
    SR_MCIC_UPDATE_HIGH_SPEED_REGION(FALSE);
    SR_MCIC_UPDATE_RESET_PULSE_FLAG(FALSE);
	// No capture event has happened so far
	SR_MCIC_UPDATE_CAPTURE_EVENT(FALSE);
	SERVICE_WATCHDOG();
	SRMCINPUTCAPTURE_FLOW_INITIALIZE_END();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Enable Input Capture peripheral.
 *
 * @details     This method will initialize the internal variables for the driver and
 *              will enable the timer counter.
 *
 * @remarks 	SRMCInputCapture__Initialize needs to be called first
 */
PASS_FAIL_TYPE SRMCInputCapture__EnableCapture(void)
{
    uint32 temp;
	PASS_FAIL_TYPE ret_val = FAIL;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    temp = SR_MCIC_CHECK_OVERFLOW_CNTR();
    temp += SR_MCIC_CHECK_MATCH_EVENT_CNTR();
    if(temp == SRDATA_OK)
    {
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        // Access the register only if the clock for the module is enabled
        if (IsPeripheralClockEnabled())
        {
            FTM1.MODE |= FTM_MODE_FTMEN_MASK;  /* Enable the counter */

            // Resets the input capture over flow counter
            SR_MCIC_UPDATE_OVERFLOW_CNTR(0);
            // Reset the Match Events counter
            SR_MCIC_UPDATE_MATCH_EVENT_CNTR(0);
            // Reset the plausibility counters
            SRMCInputCapture__InitDiagnostic();

            ret_val = PASS;
        }
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }

	return ret_val;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Disable Input Capture peripheral.
 *
 */
PASS_FAIL_TYPE SRMCInputCapture__DisableCapture(void)
{
    uint32 temp;
	PASS_FAIL_TYPE ret_val = FAIL;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    temp = SR_MCIC_CHECK_OVERFLOW_TOTAL();
    temp += SR_MCIC_CHECK_OVERFLOW_TOTAL_K1();
    if(temp == SRDATA_OK)
    {
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        // Access the register only if the clock for the module is enabled
        if (IsPeripheralClockEnabled())
        {
            IC_BASE_REG.MODE &= ~FTM_MODE_FTMEN_MASK;       /* Disable the counter */

            // Just set the maximum time when the peripheral is not enabled
            SR_MCIC_UPDATE_OVERFLOW_TOTAL(MAX_U32);
            SR_MCIC_UPDATE_OVERFLOW_TOTAL_K1(MAX_U32);
            ret_val = PASS;
        }
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }

	return ret_val;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the number of InputCapture counts
 * @details
 *
 * @return      Number of counts
 */
uint32 SRMCInputCapture__GetCounterValue(void)
{
    uint32 temp;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint32 ret_val = 0;
    uint32 temp_overflow_counter;
    temp_overflow_counter = 0;

    temp = SR_MCIC_CHECK_OVERFLOW_TOTAL();
    if(temp == SRDATA_OK)
    {
        // Get if peripheral is enabled
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        if (IsPeripheralClockEnabled())
        {
            if (IC_BASE_REG.MODE & FTM_MODE_FTMEN_MASK)
            {
                temp_overflow_counter = SR_IC_OverFlow_Total;
                temp_overflow_counter++;

                // with peripheral enabled, calculate the required time
                ret_val = temp_overflow_counter;
            }
        }
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }

	// Will return 0 in case of disabled peripheral
    return(ret_val);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the status of the input capture.
 *
 * @details
 *
 * @return      INPUTCAPTURE_ENABLED - input capture is enabled;\n
 *              INPUTCAPTURE_DISABLED - if the number of IC overflows has exceeded the INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED
 *              parameter
*/
INPUTCAPTURE_STATUS_TYPE SRMCInputCapture__GetStatus(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint8 temp;
    uint32 temp2;
    INPUTCAPTURE_STATUS_TYPE current_ic_status = INPUTCAPTURE_NOT_CONFIGURED;

    temp2 = SR_MCIC_CHECK_OVERFLOW_CNTR();
    if(temp2 == SRDATA_OK)
    {
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        if (IsPeripheralClockEnabled())
        {
            temp = FTM1.MODE & FTM_MODE_FTMEN_MASK;
            temp2 = SR_IC_OverFlow_Counter;
            if(temp2 < INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED)
            {
                if(temp)
                {
                    current_ic_status = INPUTCAPTURE_ENABLED;
                }
                else
                {
                    current_ic_status = INPUTCAPTURE_DISABLED;
                }
            }
            else
            {
                current_ic_status = INPUTCAPTURE_UNDEFINED;
            }
        }
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }

    return(current_ic_status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get match event from an input capture.
 * @details     When an input capture event just happened, then it may be read by means of this function. The
 *              flag is set when it happens and shall be set until it is read, e.g., only by reading this event
 *              shall reset the flag.
 * @return      TRUE: input capture match event happened: FALSE - no events happened.
 */
uint8 SRMCInputCapture__GetMatchEvent(void)
{
    uint32 temp;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint8 event;

    event = FALSE;

    temp = SR_MCIC_CHECK_CAPTURE_EVENT();
    if(temp == SRDATA_OK)
    {
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        event = SR_Capture_Event;
        SR_MCIC_UPDATE_CAPTURE_EVENT(FALSE);
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }
    return(event);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the Movement Status of the Input Capture
 * @details		This method relies on a counter threshold and on the number of overflows to decide
 *              if the motor shall be considered moving or not. \n
 * 				If the number of overflow events is lower than the INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED
 * 				parameter, the motor is always considered moving. \n
 * 				Otherwise, the IC counter is analyzed. If it is below the low threshold,
 *              a fictitious ramp to 0 is implemented, by waiting a certain time before declaring
 *              that the motor is stopped. Otherwise, if the counter is above the low threshold, the motor
 *              is considered directly stopped.\n
 *
 * @remarks     This function shall be called only once per 5 ms.\n
 *              The fictitious ramp can be disabled by putting the lo_thr to 0.
 *
 * @param[in]	thr The threshold, corresponding to high speed (i.e. 127 rpm_D)
 *
 * @return      TRUE, if there is movement
 * 				FALSE, if there is no movement
 */
BOOL_TYPE SRMCInputCapture__GetMovementStatus(uint32 thr)
{
	uint32 temp, temp2, temp3;
    BOOL_TYPE ret_val = TRUE;

    temp = SR_MCIC_CHECK_OVERFLOW_TOTAL();
    temp += SR_MCIC_CHECK_OVERFLOW_CNTR();
    temp += SR_MCIC_CHECK_FICT_RAMP_CNTR();
    if(temp == SRDATA_OK)
    {
        temp = SR_IC_OverFlow_Total;
        temp2 = SR_IC_OverFlow_Counter;
        temp3 = SR_Fict_Ramp_Counter;

        if (temp2 >= INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED)
        {
            // IC_OverFlow_Counter == INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED
            if (temp < thr)
            {
                // fictitious ramp, dependent on the value of the counter
                // ramp to start yet?
                if (temp3)
                {
                    // fictitious ramp, motor is considered moving
                    temp3--;
                    SR_MCIC_UPDATE_FICT_RAMP_CNTR(temp3);
                    ret_val = TRUE;
                }
                else
                {
                    // Reset IC Overflow counter to avoid another fictius ramp
                    SR_MCIC_UPDATE_OVERFLOW_TOTAL(MAX_U32);
                    SR_MCIC_UPDATE_OVERFLOW_TOTAL_K1(MAX_U32);

                    // ramp end, motor is considered stopped
                    ret_val = FALSE;
                }
            }
            else
            {
                // motor is stopped
                ret_val = FALSE;
            }
        }
        // IC_OverFlow_Counter < INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED
        else
        {
            ret_val = TRUE;
        }
    }

    return ret_val;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the current Match Events Counter
 *
 * @details     This method returns the number of match events since the InputCapture has been enabled
 * @return      The number of the Match Events since the InputCapture has been enabled
 */
uint32 SRMCInputCapture__GetMatchEventCounter(void)
{
    uint32 temp, res;

    temp = SR_MCIC_CHECK_MATCH_EVENT_CNTR();
    if(temp == SRDATA_OK)
    {
        res = SR_Match_Event_Counter;
    }
    else
    {
        res = 0;
    }

    return res;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Input Capture PWM handler
 * @details     Captures an input capture over flow and count it. The counted overflow is used in the calculation
 *              of the signal frequency. It also implements a debouncer while starting the input capture peripheral,
 *              such debouncer is used to prevent glitches from the readings.
 */
void SRMCInputCapture__PwmHandler(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint32 input_timer;
    uint32 temp, temp_u32;
    uint8 input_capture_enabled;

    temp = SR_MCIC_CHECK_OVERFLOW_TOTAL();
    temp += SR_MCIC_CHECK_OVERFLOW_CNTR();
    temp += SR_MCIC_CHECK_CAPTURE_EVENT();
    temp += SR_MCIC_CHECK_MATCH_EVENT_CNTR();
    temp += SR_MCIC_CHECK_PULSE_CNTR();
    temp += SR_MCIC_CHECK_RESET_PULSE_FLAG();
    if(temp == SRDATA_OK)
    {
        // Atomic block for this variable access.
        // Check if the peripheral is activated
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        if (IsPeripheralClockEnabled())
        {
            input_capture_enabled = IC_BASE_REG.MODE & FTM_MODE_FTMEN_MASK;

            if(input_capture_enabled)
            {
                if(IC_BASE_REG.SC & FTM_SC_TOF_MASK) // If an overflow has happened in the input capture
                {
                    // Clear the flag
                    IC_BASE_REG.SC &= ~FTM_SC_TOF_MASK;

                    // Count the overflow event
                    temp_u32 = SR_IC_OverFlow_Counter;
                    if(temp_u32 < INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED)
                    {
                        temp_u32++;
                        SR_MCIC_UPDATE_OVERFLOW_CNTR(temp_u32);
                    }
                }

                // If the input capture event has been triggered
                if(IC_BASE_REG.CONTROLS[INPUTCAPTURE_CHANNEL].CnSC & FTM_CnSC_CHF_MASK)
                {
                    /* Clear the Input Capture flag */
                    IC_BASE_REG.CONTROLS[INPUTCAPTURE_CHANNEL].CnSC &= ~FTM_CnSC_CHF_MASK;

                    // Set the event in the capture event
                    SR_MCIC_UPDATE_CAPTURE_EVENT(TRUE);
                    // Increment the Match Event Counter
                    temp_u32 = SR_Match_Event_Counter;
                    temp_u32++;
                    SR_MCIC_UPDATE_MATCH_EVENT_CNTR(temp_u32);

                    // Save the input time in a variable
                    input_timer = IC_BASE_REG.CONTROLS[INPUTCAPTURE_CHANNEL].CnV;
                    // Division by zero prevention
                    input_timer++;

                    // Save the counted overflow events in the stack
                    temp_u32 = SR_IC_OverFlow_Counter;

                    // Compute the total equivalent input capture
                    temp_u32  = ((uint32)temp_u32) << 16; // 16 means a multiplication by 65536
                    temp_u32 += input_timer;

                    // Atomic block for this variable write access.

                    SR_MCIC_UPDATE_OVERFLOW_TOTAL(temp_u32);

                    // Perform plausibility checks on the acquired value
                    DiagnosticHandler();

                    // Zero the overflow counter
                    SR_MCIC_UPDATE_OVERFLOW_CNTR(0);
                }

                // Undertorque Management
                // Handle the counter reset request from the SR module
                if (SR_Reset_Pulse_Flag)
                {
                    SR_MCIC_UPDATE_PULSE_CNTR(0);
                    SR_MCIC_UPDATE_RESET_PULSE_FLAG(FALSE);
                }
            }
        }
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the Diagnostic Feedback
 * @details     Get the self diagnostic status. The DiagnosticHandler is called whenever a InputCapture
 *              event is detected.
 *
 * @return      The current diagnostic status. See the @ref SRMCInputCapture.h for the definition.
 */
SRMCINPUTCAPTURE_DIAG_ERROR_TYPE SRMCInputCapture__GetDiagnosticFeedback(void)
{
    uint32 temp;
    SRMCINPUTCAPTURE_DIAG_ERROR_TYPE res;

    temp = SR_MCIC_CHECK_DIAGNOSTIC_STATUS();
    if(temp == SRDATA_OK)
    {
        res = (SR_Diagnostic_Status);
    }
    else
    {
        res = SR_IC_ERROR_PERIPHERAL_NOT_SAFE;
    }
    return res;						// Returns status
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Initialize the Diagnostic variables
 *
 * @details     Reset all the plausibility checks variables
 *
 */
PASS_FAIL_TYPE SRMCInputCapture__InitDiagnostic(void)
{
    uint32 temp;
    PASS_FAIL_TYPE res = FAIL;

    temp = SR_MCIC_CHECK_DIAGNOSTIC_STATUS();
    temp += SR_MCIC_CHECK_PLAUSIBILITY_MIN_CNTR();
    temp += SR_MCIC_CHECK_PLAUSIBILITY_MAX_CNTR();
    temp += SR_MCIC_CHECK_FICT_RAMP_CNTR();
    temp += SR_MCIC_CHECK_MATCH_EVENT_CNTR();
    temp += SR_MCIC_CHECK_HIGH_SPEED_REGION();
    temp += SR_MCIC_CHECK_PULSE_CNTR();
    temp += SR_MCIC_CHECK_RESET_PULSE_FLAG();
    if(temp == SRDATA_OK)
    {
        SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(SR_IC_TEST_NOT_PERFORMED);
        SR_MCIC_UPDATE_PLAUSIBILITY_MIN_CNTR(0);
        SR_MCIC_UPDATE_PLAUSIBILITY_MAX_CNTR(0);
        SR_MCIC_UPDATE_FICT_RAMP_CNTR(SR_FICT_RAMP_DOWN_COUNTER);
        SR_MCIC_UPDATE_MATCH_EVENT_CNTR(0);
        // Undertorque management
        SR_MCIC_UPDATE_PULSE_CNTR(0);
        SR_MCIC_UPDATE_HIGH_SPEED_REGION(FALSE);
        SR_MCIC_UPDATE_RESET_PULSE_FLAG(FALSE);
    }
    else
    {
        res = FAIL;
    }
    return res;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the Undertorque Counter
 *
 * @return      The current Undertorque Counter.
 */
uint32 SRMCInputCapture__GetUndertorqueCounter(void)
{
    uint32 temp, res;

    temp = SR_MCIC_CHECK_PULSE_CNTR();
    if(temp == SRDATA_OK)
    {
        res = SR_Pulse_Counter;
    }
    else
    {
        res = 0;
    }
    return res;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Request a reset of the undertorque counter
 *
 * @return      TRUE if the request has been accepted
 */
BOOL_TYPE SRMCInputCapture__ResetUndertorqueCounter(void)
{
    uint32 temp;
    BOOL_TYPE res;

    temp = SR_MCIC_CHECK_RESET_PULSE_FLAG();
    if(temp == SRDATA_OK)
    {
        SR_MCIC_UPDATE_RESET_PULSE_FLAG(TRUE);
        res = TRUE;
    }
    else
    {
        res = FALSE;
    }

    return res;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get Speed Region
 *
 * @return      TRUE if in High Speed, FALSE otherwise.
 */
BOOL_TYPE SRMCInputCapture__GetSpeedRegion(void)
{
    uint32 temp;
    BOOL_TYPE res;

    temp = SR_MCIC_CHECK_HIGH_SPEED_REGION();
    if(temp == SRDATA_OK)
    {
        res = SR_High_Speed_Region;
    }
    else
    {
        res = TRUE;
    }
    return res;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 * @brief       Input Capture Diagnostic Handler
 * @details     Plausibility checks for the Input Capture and Undertorque Management.
 *
 */
static inline void DiagnosticHandler(void)
{
    sint32 difference;
    sint64 threshold;
    uint32 temp;

    temp = SR_MCIC_CHECK_OVERFLOW_TOTAL();
    temp += SR_MCIC_CHECK_OVERFLOW_TOTAL_K1();
    temp += SR_MCIC_CHECK_PLAUSIBILITY_MIN_CNTR();
    temp += SR_MCIC_CHECK_PLAUSIBILITY_MAX_CNTR();
    temp += SR_MCIC_CHECK_HIGH_SPEED_REGION();
    temp += SR_MCIC_CHECK_PULSE_CNTR();
    temp += SR_MCIC_CHECK_DIAGNOSTIC_STATUS();
    if(temp == SRDATA_OK)
    {
        if (SR_IC_OverFlow_Counter < INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED)
        {
            difference = SR_IC_OverFlow_Total - SR_IC_OverFlow_Total_k1;
            difference = MATHCALC__ABS(difference) * SR_PLAUSIBILITY_MAX_DIFFERENCE_CONST;
            threshold = SR_IC_OverFlow_Total * SR_IC_OverFlow_Total_k1;

            if(SR_IC_OverFlow_Total < SR_PLAUSIBILITY_MIN_COUNTER_VALUE)
            {
                // increment the relevant counter
                temp = SR_Plausibility_Min_Counter;
                temp++;
                SR_MCIC_UPDATE_PLAUSIBILITY_MIN_CNTR(temp);
                // discard the acquired value
                // take the previous one
                SR_MCIC_UPDATE_OVERFLOW_TOTAL(SR_IC_OverFlow_Total_k1);
            }
            else if (difference > threshold)
            {
                // increment the relevant counter
                temp = SR_Plausibility_Max_Counter;
                temp++;
                SR_MCIC_UPDATE_PLAUSIBILITY_MAX_CNTR(temp);
                // discard the acquired value
                // take the previous one
                SR_MCIC_UPDATE_OVERFLOW_TOTAL(SR_IC_OverFlow_Total_k1);
            }
            else
            {
                // Plausibility checks passed

                // Undertorque Management
                // check the speed region
                if (SR_IC_OverFlow_Total < SR_HIGH_SPEED_REGION_THR)
                {
                    // High speed region
                    SR_MCIC_UPDATE_HIGH_SPEED_REGION(TRUE);
                }
                else
                {
                    // Low speed region
                    SR_MCIC_UPDATE_HIGH_SPEED_REGION(FALSE);
                }

                // increment pulse counter
                temp = SR_Pulse_Counter;
                temp++;
                SR_MCIC_UPDATE_PULSE_CNTR(temp);
            }

            // Move the diagnostic handler state
            if (SR_Plausibility_Min_Counter > SR_MIN_TACH_CNT_THR)
            {
                SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(SR_IC_MIN_TACH_ERROR);
            }
            else if (SR_Plausibility_Max_Counter > SR_MAX_SPEED_CHANGE_CNT_THR)
            {
                SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(SR_IC_MAX_SPEED_ERROR);
            }
            else
            {
                SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(SR_IC_PERIPHERAL_SAFE);
            }
        }

        // store the current counter
        SR_MCIC_UPDATE_OVERFLOW_TOTAL_K1(SR_IC_OverFlow_Total);
    }
    else
    {
        SR_MCIC_UPDATE_DIAGNOSTIC_STATUS(SR_IC_ERROR_PERIPHERAL_NOT_SAFE);
    }
}

static inline BOOL_TYPE IsPeripheralClockEnabled(void)
{
	BOOL_TYPE ret_val = FALSE;
	if ((SIM.SCGC6 & IC_CLK_EN_MASK) == IC_CLK_EN_MASK)
	{
		ret_val = TRUE;
	}

	return ret_val;
}

/**
 *  @defgroup   CLASS_B
 *  @file       SRMCAtod.c
 *  @brief      This module treats analog conversions for motor control
 *
 *  @details    Analog conversions are triggered by PDB timer event in order minimize ringing noise.
 *              Trigger for all 3 currents and DC bus voltage occur at the same time
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRMCAtod.h"
#include "SRMicro.h"
#include "SRData.h"
#include "SRException.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (SRMCATOD_ENABLE_ADDITIONAL_READING == 1)
typedef struct
{
    SRMCATOD_CONVERTER_TYPE Engine;
    SRMCATOD_CHANNEL_DEF Channel;
    BOOL_TYPE Use_Mux;
    ADC_BASIC_DEF* Base_Reg;
} SRMCATOD_MANUAL_CONFIG_TYPE;
#endif
//-------------------------------------- Safety Relevant Variables ----------------------------------------------------
static SRMCATOD_DIAG_STATE_TYPE SR_State;
static SRMCATOD_DIAG_ERROR_TYPE SR_Fault_Holder;
static sint16 SR_Threshold;
#if((SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 1)||(SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 2))
static uint16  SR_Eos_Isr_Counter;
#endif

static SRMCATOD_DIAG_STATE_TYPE NSR_State;
static SRMCATOD_DIAG_ERROR_TYPE NSR_Fault_Holder;
static sint16 NSR_Threshold;
#if((SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 1)||(SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 2))
static uint16 NSR_Eos_Isr_Counter;
#endif

#if (SRMCATOD_ENABLE_ADDITIONAL_READING == 1)
static sint16 Last_Manual_Sample;
static SRMCATOD_MANUAL_CONFIG_TYPE Manual_Cfg_Hdlr;
#endif

//! FTM clock counts distance between Counter and Modulo used to check for stuck manual conversion condition [FTM counts]
#define CONVERSION_TIME_THR     10

//---------------------------------------- Macros -----------------------------------------------------
#ifndef SRMCATOD_MANUAL_CONVERSION_EXCEPTION
    #define SRMCATOD_MANUAL_CONVERSION_EXCEPTION()      SRException__Queue(EXCEPTION_ATOD_STUCK, 0 , 0)
#endif

#if(1)
// CHECK Macro defs
//lint -emacro( 928, SR_MCATOD_CHECK_STATE )
#define SR_MCATOD_CHECK_STATE()                         SRData__CheckByte((uint8 *)&SR_State, (uint8 *)&NSR_State)
//lint -emacro( 928, SR_MCATOD_CHECK_FAULT_HOLDER )
#define SR_MCATOD_CHECK_FAULT_HOLDER()                  SRData__CheckByte((uint8 *)&SR_Fault_Holder, (uint8 *)&NSR_Fault_Holder)
//lint -emacro( 929, SR_MCATOD_CHECK_THRESHOLD )
#define SR_MCATOD_CHECK_THRESHOLD()                     SRData__CheckShort((uint16 *)&SR_Threshold, (uint16 *)&NSR_Threshold)
//lint -emacro( 928, SR_MCATOD_CHECK_EOS_ISR_CNT )
#define SR_MCATOD_CHECK_EOS_ISR_CNT()                   SRData__CheckShort((uint16 *)&SR_Eos_Isr_Counter, (uint16 *)&NSR_Eos_Isr_Counter)


// UPDATE Macro defs
//lint -emacro( 928, SR_MCATOD_UPDATE_STATE )
#define SR_MCATOD_UPDATE_STATE(value)                   SRData__UpdateByte((uint8 *)&SR_State, (uint8 *)&NSR_State, (uint8)(value))
//lint -emacro( 928, SR_MCATOD_UPDATE_FAULT_HOLDER )
#define SR_MCATOD_UPDATE_FAULT_HOLDER(value)            SRData__UpdateByte((uint8 *)&SR_Fault_Holder, (uint8 *)&NSR_Fault_Holder, (uint8)(value))
//lint -emacro( 929, SR_MCATOD_UPDATE_THRESHOLD )
#define SR_MCATOD_UPDATE_THRESHOLD(value)               SRData__UpdateShort((uint16 *)&SR_Threshold, (uint16 *)&NSR_Threshold, (uint16)(value))
//lint -emacro( 928, SR_MCATOD_UPDATE_EOS_ISR_CNT )
#define SR_MCATOD_UPDATE_EOS_ISR_CNT(value)             SRData__UpdateShort((uint16 *)&SR_Eos_Isr_Counter, (uint16 *)&NSR_Eos_Isr_Counter, (uint16)(value))


#else
// CHECK Macro defs
#define SR_MCATOD_CHECK_STATE()                         SRDATA_OK
#define SR_MCATOD_CHECK_FAULT_HOLDER()                  SRDATA_OK
#define SR_MCATOD_CHECK_THRESHOLD()                     SRDATA_OK

// UPDATE Macro defs
#define SR_MCATOD_UPDATE_STATE(value)                   SR_State = (value)
#define SR_MCATOD_UPDATE_FAULT_HOLDER(value)            SR_Fault_Holder = (value)
#define SR_MCATOD_UPDATE_THRESHOLD(value)               SR_Threshold = (value)

#endif
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void SRInitializeAD0(void);
static void SRInitializeAD1(void);
static void SRCalibrateAD0(void);
static void SRCalibrateAD1(void);
static void SRInitializePDB0(void);
static void DisableEndOfConversionInteruption(SRMCATOD_CONVERTER_TYPE engine);
static void EnableEndOfConversionInteruption(SRMCATOD_CONVERTER_TYPE engine);
static void SREnableSelfDiagMode(SRMCATOD_CONVERTER_TYPE engine, SRMCATOD_DIAG_CONFIG_TYPE mode);
static SRMCATOD_DIAG_ERROR_TYPE IsConverterSafe(SRMCATOD_CONVERTER_TYPE engine,sint16 thresold, SRMCATOD_DIAG_CONFIG_TYPE mode);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MCAtod peripheral and its variables
 *
 *  @details    ADC0, ADC1 and PDB0 peripherals are initialized.
 *
 *  @remarks    Only the IRQ of ADC1 channel 1 is enabled.
 *
 */
void SRMCAtod__Initialize(void)
{
	DISABLE_INTERRUPTS();                           // Disables interruptions while configuring

	SRInitializeAD0();                           	// Configures AD0 with PDB0 trigger for 2 channels
	SRInitializeAD1();								// Configures AD1 with PDB0 trigger for 2 channels
	SRInitializePDB0();								// Configures PDB0 triggered by FTM0

    // Enable the 1V reference for plausibility checks
    PMC.REGSC.BIT.BGEN = 1;
    PMC.REGSC.BIT.BGBE = 1;
    // Initialize the Safety Relevant variables
    SR_MCATOD_UPDATE_STATE(SR_MCATOD_WAITING_TO_CHECK);
    SR_MCATOD_UPDATE_THRESHOLD(0);
    SR_MCATOD_UPDATE_FAULT_HOLDER(SR_MCATOD_ERROR_TEST_NOT_PERFORMED);

#if((SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 1)||(SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 2))
    SR_MCATOD_UPDATE_EOS_ISR_CNT(0);
#endif
#if (SRMCATOD_ENABLE_ADDITIONAL_READING == 1)
    Manual_Cfg_Hdlr.Engine = (SRMCATOD_CONVERTER_TYPE) SRMCATOD_ADDITIONAL_READING_ADC_ENGINE;
    Manual_Cfg_Hdlr.Channel = (SRMCATOD_CHANNEL_DEF) SRMCATOD_ADDITIONAL_READING_CHANNEL;
    Manual_Cfg_Hdlr.Use_Mux = (BOOL_TYPE) SRMCATOD_ADDITIONAL_READING_MUX_SELECT;

#if SRMCATOD_ADDITIONAL_READING_ADC_ENGINE == 1
    Manual_Cfg_Hdlr.Base_Reg = &ADC1;
#else
    Manual_Cfg_Hdlr.Base_Reg = &ADC0;
#endif

    Last_Manual_Sample = SRMCATOD_ADDITIONAL_READING_INITIAL_VALUE;
#endif
    ENABLE_INTERRUPTS();                            // Enables back interruptions
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      End of engine 0 conversion.
 *
 *  @details    This is supposed to be simultaneous to the end of engine 1, since they are triggered together.
 *
 */
INTERRUPT_DIRECTIVE void SRMCAtod__ISR(void)
{
    #if((SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 1)||(SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 2))
	uint16 temp;
	SR_MCATOD_CHECK_EOS_ISR_CNT();
	temp = SR_Eos_Isr_Counter;
    temp++;
    SR_MCATOD_UPDATE_EOS_ISR_CNT(temp);
    #endif

	if((ADC0.ADC_SC1[0] & ADC_SC1_COCO_MASK) == ADC_SC1_COCO_MASK)
	{
		ADC0.ADC_SC1[0] &= ~ADC_SC1_COCO_MASK;
	}
	if((ADC0.ADC_SC1[1] & ADC_SC1_COCO_MASK) == ADC_SC1_COCO_MASK)
	{
		ADC0.ADC_SC1[1] &= ~ADC_SC1_COCO_MASK;
	}
	if((ADC1.ADC_SC1[0] & ADC_SC1_COCO_MASK) == ADC_SC1_COCO_MASK)
	{
		ADC1.ADC_SC1[0] &= ~ADC_SC1_COCO_MASK;
	}
	if((ADC1.ADC_SC1[1] & ADC_SC1_COCO_MASK) == ADC_SC1_COCO_MASK)
	{
		ADC1.ADC_SC1[1] &= ~ADC_SC1_COCO_MASK;
		
        #if(SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 1)
		if (temp & 1)//
        #elif(SRMCATOD_EOC_ISR_HANDLER_PRESCALER == 2)
		if (temp - ((temp >> 2) << 2) == 0) // (Counter % 4) == 0
        #endif
		{
			SRMCATOD_EOC_ISR_HANDLER();
	 	}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      PDB interrupt service routine
 *
 */
INTERRUPT_DIRECTIVE void SRMCPDB__ISR(void)
{
    PDB.SC &= ~PDB_SC_PDBIF_MASK ;  				// clear interrupt mask

    PDB.SC |= PDB_SC_LDOK_MASK;                 	// LDOK needed for update of delay values

	if ((PDB.CH[0].S & PDB_S_ERR_MASK) ||
		(PDB.CH[1].S & PDB_S_ERR_MASK))
	{
		PDB.SC &= ~PDB_SC_PDBEN_MASK;  				// disable PDB
		PDB.CH[0].S &= ~PDB_S_CF_MASK;
		PDB.CH[0].S &= ~PDB_S_ERR_MASK;   			// reset error CH0
		PDB.CH[1].S &= ~PDB_S_CF_MASK;
		PDB.CH[1].S &= ~PDB_S_ERR_MASK;  			// reset error CH1
		PDB.SC |= PDB_SC_PDBEN_MASK;  				// Enable PDB
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      ADC Diagnostic handler
 *
 *  @details    This function is a handler used to check 12 bits AD registers and converters status.
 *              May be used to test stuck at register failure and conversion errors. Checks all converters with all possible fixed values.
 *              This handler has no time dependency, may be used on handlers at 16KHz, 250us, 1ms 5ms or 25ms.
 *              Use functions to start tests and get feedbacks.
 *
 */
void SRMCAtod__DiagnosticHandler(void)
{
    uint8 temp;
    
    SR_MCATOD_CHECK_STATE();
    SR_MCATOD_CHECK_FAULT_HOLDER();
    SR_MCATOD_CHECK_THRESHOLD();

	switch (SR_State)							    // State machine triggered externally
	{
		case SR_MCATOD_WAITING_TO_CHECK:					// Waits until a verification is requested

			break;

		case SR_MCATOD_START_EVAL:							// Configures peripheral converters to test in respect to internal voltage reference

            SREnableSelfDiagMode(SR_AD_ENGINE_0,SR_MCATOD_SELF_TEST_VREFH0_SCALE_0);
            SREnableSelfDiagMode(SR_AD_ENGINE_1,SR_MCATOD_SELF_TEST_VREFH0_SCALE_0);

            SR_MCATOD_UPDATE_FAULT_HOLDER(SR_MCATOD_ERROR_TEST_NOT_PERFORMED);
                                                    // Initializes feedback to update test is not finished
            SR_MCATOD_UPDATE_STATE(SR_MCATOD_CHECK_STUCK_AT_1);
                                                // Gets to next state to get feedbacks from this test
			break;

		case SR_MCATOD_CHECK_STUCK_AT_1:					// Checks if all bits are null above certain threshold

			SREnableSelfDiagMode(SR_AD_ENGINE_0,SR_MCATOD_SELF_TEST_DISABLED);
			SREnableSelfDiagMode(SR_AD_ENGINE_1,SR_MCATOD_SELF_TEST_DISABLED);
													// Disables self diagnostic
			temp = (uint8) IsConverterSafe(SR_AD_ENGINE_0,SR_Threshold, SR_MCATOD_SELF_TEST_VREFH0_SCALE_0);
			SR_MCATOD_UPDATE_FAULT_HOLDER(temp);    // Gets converter status

			if (SR_Fault_Holder == SR_MCATOD_ERROR_PERIPHERAL_SAFE)	// If previous converter feedback is SAFE, checks next converter
			{
			    temp = (uint8) IsConverterSafe(SR_AD_ENGINE_1,SR_Threshold, SR_MCATOD_SELF_TEST_VREFH0_SCALE_0);
			    SR_MCATOD_UPDATE_FAULT_HOLDER(temp);    // Gets converter status

			    if(SR_Fault_Holder == SR_MCATOD_ERROR_PERIPHERAL_SAFE)
				{									// If this feedback is also safe, checks in respect to next internal voltage level
                    SREnableSelfDiagMode(SR_AD_ENGINE_0,SR_MCATOD_SELF_TEST_VREFH0_SCALE_1);
                    SREnableSelfDiagMode(SR_AD_ENGINE_1,SR_MCATOD_SELF_TEST_VREFH0_SCALE_1);
                                                    // Configures peripheral converters to test in respect to internal voltage reference
                    SR_MCATOD_UPDATE_STATE(SR_MCATOD_CHECK_STUCK_AT_0);
                                                    // Gets to next test state since this was successfully tested
				}
				else								// Any type of error moves directly to feedback update to publish status
				{
                    SR_MCATOD_UPDATE_STATE(SR_MCATOD_FEEDBACK_UPDATED);
				}
			}
			else									// Any type of error moves directly to feedback update to publish status
			{
                SR_MCATOD_UPDATE_STATE(SR_MCATOD_FEEDBACK_UPDATED);
			}
			break;

		case SR_MCATOD_CHECK_STUCK_AT_0:					// Checks if all bits are set under certain threshold

			SREnableSelfDiagMode(SR_AD_ENGINE_0,SR_MCATOD_SELF_TEST_DISABLED);
			SREnableSelfDiagMode(SR_AD_ENGINE_1,SR_MCATOD_SELF_TEST_DISABLED);
													// Disables self diagnostic
			temp = (uint8) IsConverterSafe(SR_AD_ENGINE_0,SR_Threshold, SR_MCATOD_SELF_TEST_VREFH0_SCALE_1);
			SR_MCATOD_UPDATE_FAULT_HOLDER(temp);    // Gets converter status

			if (SR_Fault_Holder == SR_MCATOD_ERROR_PERIPHERAL_SAFE)	// If previous converter feedback is SAFE, checks next converter
			{
			    temp = (uint8) IsConverterSafe(SR_AD_ENGINE_1,SR_Threshold, SR_MCATOD_SELF_TEST_VREFH0_SCALE_1);
			    SR_MCATOD_UPDATE_FAULT_HOLDER(temp);    // Gets converter status

			    if(SR_Fault_Holder == SR_MCATOD_ERROR_PERIPHERAL_SAFE)
				{									// If this feedback is also safe, checks in respect to next internal voltage level
                    SREnableSelfDiagMode(SR_AD_ENGINE_0,SR_MCATOD_SELF_TEST_VREFH0_SCALE_0_5);
                    SREnableSelfDiagMode(SR_AD_ENGINE_1,SR_MCATOD_SELF_TEST_VREFH0_SCALE_0_5);
                                                    // Configures peripheral converters to test in respect to internal voltage reference
                    SR_MCATOD_UPDATE_STATE(SR_MCATOD_CHECK_CONVERTER_ERROR);
				}									// Gets to next test state since this was successfully tested
				else								// Any type of error moves directly to feedback update to publish status
				{
                    SR_MCATOD_UPDATE_STATE(SR_MCATOD_FEEDBACK_UPDATED);
				}
			}
			else									// Any type of error moves directly to feedback update to publish status
			{
                SR_MCATOD_UPDATE_STATE(SR_MCATOD_FEEDBACK_UPDATED);
			}
			break;

		case SR_MCATOD_CHECK_CONVERTER_ERROR:				// Checks if converter values are good under certain threshold

			SREnableSelfDiagMode(SR_AD_ENGINE_0,SR_MCATOD_SELF_TEST_DISABLED);
			SREnableSelfDiagMode(SR_AD_ENGINE_1,SR_MCATOD_SELF_TEST_DISABLED);
													// Disables self diagnostic
			temp = (uint8) IsConverterSafe(SR_AD_ENGINE_0,SR_Threshold, SR_MCATOD_SELF_TEST_VREFH0_SCALE_0_5);
			SR_MCATOD_UPDATE_FAULT_HOLDER(temp);    // Gets converter status

			if (SR_Fault_Holder == SR_MCATOD_ERROR_PERIPHERAL_SAFE)	// If previous converter feedback is SAFE, checks next converter
			{
                temp = (uint8) IsConverterSafe(SR_AD_ENGINE_1,SR_Threshold, SR_MCATOD_SELF_TEST_VREFH0_SCALE_0_5);
                SR_MCATOD_UPDATE_FAULT_HOLDER(temp);    // Gets converter status
                SR_MCATOD_UPDATE_STATE(SR_MCATOD_FEEDBACK_UPDATED);
                                                    // Gets converter feedback and update for publishing
			}
			else									// Any type of error moves directly to feedback update to publish status
			{
                SR_MCATOD_UPDATE_STATE(SR_MCATOD_FEEDBACK_UPDATED);
			}
			break;

		case SR_MCATOD_FEEDBACK_UPDATED:					// Stays here until upper level application gets feedback

			break;

		default:									// default state returns to wait command to check
			SREnableSelfDiagMode(SR_AD_ENGINE_0,SR_MCATOD_SELF_TEST_DISABLED);
			SREnableSelfDiagMode(SR_AD_ENGINE_1,SR_MCATOD_SELF_TEST_DISABLED);
													// Disables self diagnostic
			SR_MCATOD_UPDATE_STATE(SR_MCATOD_WAITING_TO_CHECK);
			break;
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Enable AtoD Diagnostic features.
 *
 *  @details    This function is a handler used to start checking 12 bits AD registers and converters status.
 *              Only restarts if feedback was updated or it's waiting for a check request.
 *
 *  @param      commanded_threshold Threshold used for the correct level detection
 *
 */
void SRMCAtod__EnableDiagnostic(sint16 commanded_threshold)
{

    SR_MCATOD_CHECK_STATE();
    SR_MCATOD_CHECK_THRESHOLD();

	if ((SR_State == SR_MCATOD_WAITING_TO_CHECK)||(SR_State == SR_MCATOD_FEEDBACK_UPDATED))
	{
		SR_MCATOD_UPDATE_THRESHOLD(commanded_threshold);
	    SR_MCATOD_UPDATE_STATE(SR_MCATOD_START_EVAL);						// Gets to next state to get feedbacks from this test
	}												// Defines new threshold for this verification
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get diagnostic status.
 *
 *  @details    This function gets converters status.
 *
 *  @return     Returns if all converters tests result: TEST_NOT_PERFORMED, TEST_NOT_INITIALIZED, REGISTER_BIT_STUCK_AT_0,
 *              REGISTER_BIT_STUCK_AT_1, CONVERTER_ERROR or PERIPHERAL_SAFE
 */
SRMCATOD_DIAG_ERROR_TYPE SRMCAtod__GetDiagnosticFeedback(void)
{
	SRMCATOD_DIAG_ERROR_TYPE current_status;
    uint32 temp;

	SR_MCATOD_CHECK_STATE();
	SR_MCATOD_CHECK_FAULT_HOLDER();

	if (SR_State == SR_MCATOD_FEEDBACK_UPDATED)				// Checks if test is concluded
	{
	    temp  = SR_Fault_Holder;

        if((temp == SR_MCATOD_ERROR_REGISTER_BIT_STUCK_AT_0) ||
           (temp == SR_MCATOD_ERROR_REGISTER_BIT_STUCK_AT_1) ||
           (temp == SR_MCATOD_ERROR_CONVERTER_ERROR) )
        {
            current_status = SR_MCATOD_ERROR_PERIPHERAL_NOT_SAFE;
        }
        else
        {
            current_status = SR_Fault_Holder;
        }

		SR_MCATOD_UPDATE_FAULT_HOLDER(SR_MCATOD_ERROR_TEST_NOT_INITIALIZED);
		                                                // Returns test feedback and reinitializes for next test waiting to be initialized
	}
	else if (SR_State == SR_MCATOD_WAITING_TO_CHECK)			// In case test was never performed
	{
		current_status = SR_MCATOD_ERROR_TEST_NOT_INITIALIZED;	// Returns it's waiting to be initialized
	}
	else											// In case test is still on going
	{
		current_status = SR_MCATOD_ERROR_TEST_NOT_PERFORMED;		// Returns test not performed yet
	}
	return (current_status);						// Returns status
}

#if (SRMCATOD_ENABLE_ADDITIONAL_READING == 1)
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Convert a sample value with a manual request.
 *
 *  @details    This function starts a new conversion using a SW trigger and returns when the conversion has finished.
 *
 *  @remark     The engine and channel to use need to be configured in the prm file and / or calling the
 *              SRMCAtod__ConfigureManualReading function
 *
 *  @remark     Call this function with caution!
 *              This function stops the Motor Control framework until the requested value is converted.
 *              If not called properly, this may cause the application to malfunction or even damage the board!
 *
 */
void SRMCAtod__ConvertManualSample(void)
{
    uint8 prev_mux_bit;
    uint8 prev_channel;

    // store the configuration
    prev_mux_bit = (Manual_Cfg_Hdlr.Base_Reg->ADC_CFG2 & ADC_CFG2_MUXSEL_MASK) >> ADC_CFG2_MUXSEL_SHIFT;
    prev_channel = (Manual_Cfg_Hdlr.Base_Reg->ADC_SC1[0] & ADC_SC1_ADCH_MASK);

    // Disable ADC EOS interrupt
    ADC1.ADC_SC1[1] &= ~ADC_SC1_AIEN_MASK;
    // Disable PDB module
    PDB.SC &= ~PDB_SC_PDBEN_MASK;
    // hardware trigger disabled
    Manual_Cfg_Hdlr.Base_Reg->ADC_SC2 &= ~ADC_SC2_ADTRG_MASK;

    // configure to the new channel & start the conversion
    if (Manual_Cfg_Hdlr.Use_Mux)
    {
        Manual_Cfg_Hdlr.Base_Reg->ADC_CFG2 |= ADC_CFG2_MUXSEL_MASK;
    }
    else
    {
        Manual_Cfg_Hdlr.Base_Reg->ADC_CFG2 &= ~ADC_CFG2_MUXSEL_MASK;
    }
    Manual_Cfg_Hdlr.Base_Reg->ADC_SC1[0] = (Manual_Cfg_Hdlr.Base_Reg->ADC_SC1[0] & 0x60) | Manual_Cfg_Hdlr.Channel;

    // wait for the conversion complete
    while ((Manual_Cfg_Hdlr.Base_Reg->ADC_SC1[0] & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK)
    {
        // If too close to the reload event
        if ((FTM0.MOD - FTM0.CNT) < CONVERSION_TIME_THR)
        {
            // Conversion is taking too much time
            SRMCATOD_MANUAL_CONVERSION_EXCEPTION();
        }
    }
    // read the result
    Last_Manual_Sample = (signed short int)(Manual_Cfg_Hdlr.Base_Reg->ADC_R[0]);

    // restore previous configuration
    // enable HW trigger
    Manual_Cfg_Hdlr.Base_Reg->ADC_SC2 |= ADC_SC2_ADTRG_MASK;
    // reconfigure the channel to default value
    Manual_Cfg_Hdlr.Base_Reg->ADC_CFG2 |= (prev_mux_bit << ADC_CFG2_MUXSEL_SHIFT);
    Manual_Cfg_Hdlr.Base_Reg->ADC_SC1[0] = (Manual_Cfg_Hdlr.Base_Reg->ADC_SC1[0] & 0x60) | prev_channel;

    // Enable PDB module
    PDB.SC |= PDB_SC_PDBEN_MASK;
    // Enable ADC EOS interrupt
    ADC1.ADC_SC1[1] |= ADC_SC1_AIEN_MASK;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the last manually converted value.
 *
 *  @remark     Call the SRMCAtod__ConvertManualSample function first to ensure an updated value
 *
 *  @return     The converted value, in AD Counts.
 */
sint16 SRMCAtod__GetLastManualSample(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    sint16 ret = 0;

    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
    ret = Last_Manual_Sample;
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

    return ret;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Configure the manual reading operation
 *
 *  @param      engine      ADC engine to use for the manual reading
 *              channel     The channel number, according to the SRMCATOD_CHANNEL_DEF type
 *              use_mux     MUX select setting for the selected additional channel
 *
 *  @return
 */
void SRMCAtod__ConfigureManualReading(SRMCATOD_CONVERTER_TYPE engine, SRMCATOD_CHANNEL_DEF channel, BOOL_TYPE use_mux)
{
    Manual_Cfg_Hdlr.Engine = engine;
    Manual_Cfg_Hdlr.Channel = channel;
    Manual_Cfg_Hdlr.Use_Mux = use_mux;

    if (engine == SR_AD_ENGINE_0)
    {
        Manual_Cfg_Hdlr.Base_Reg = &ADC0;
    }
    else if (engine == SR_AD_ENGINE_1)
    {
        Manual_Cfg_Hdlr.Base_Reg = &ADC1;
    }
    else
    {
        // intentionally left blank
    }
}
#endif
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 *  @brief      Initialization of ADC0
 *
 *  @details    This function configures ADC0 to sample 2 channels triggered by PDB0 delay in respect to FTM0 event.
 *
 */
static void SRInitializeAD0(void)
{
    //------------------------------  AD0 Configuration ------------------------------
	DisableEndOfConversionInteruption(SR_AD_ENGINE_0);
	// System clock enabling
	SIM.SCGC6 |= SIM_SCGC6_ADC0_MASK;				// Enables ADC0 clock
	// ADC0 Self calibration
	SRCalibrateAD0();
	// setting prescaler to get ADCK = bus clock, 12 bits resolution, short sample time and normal operation mode
	ADC0.ADC_CFG1 = ADC_CFG1_ADIV(SRMCATOD_ADC0_CLOCK_DIVIDE_SEL)
					| ADC_CFG1_ADICLK(SRMCATOD_ADC0_CLK_INPUT)
					| ADC_CFG1_MODE(SRMCATOD_ADC0_RES_MODE);

	ADC0.ADC_CFG2 = ADC_CFG2_ADHSC_MASK;			// High speed configuration
	// Configure ports as analog input
	// First and Second samples
    #if(SRMCATOD_ADC0_MUX_SET == 1)
        #if(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 4)
            PCIPORTC.PCR[SRMCATOD_ADC0_FIRST_SAMPLE_CHN - 2] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #elif(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 5)
            PCIPORTD.PCR[SRMCATOD_ADC0_FIRST_SAMPLE_CHN - 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #elif((SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 6) || (SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 7))
            PCIPORTD.PCR[SRMCATOD_ADC0_FIRST_SAMPLE_CHN - 1] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #endif

        #if(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 4)
            PCIPORTC.PCR[SRMCATOD_ADC0_SECOND_SAMPLE_CHN - 2] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #elif(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 5)
            PCIPORTD.PCR[SRMCATOD_ADC0_SECOND_SAMPLE_CHN - 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #elif((SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 6) || (SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 7))
            PCIPORTD.PCR[SRMCATOD_ADC0_SECOND_SAMPLE_CHN - 1] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #endif
    #else
        #if ((SRMCATOD_ADC0_FIRST_SAMPLE_CHN >= 4) && (SRMCATOD_ADC0_FIRST_SAMPLE_CHN < 8))
            PCIPORTE.PCR[SRMCATOD_ADC0_FIRST_SAMPLE_CHN + 12] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #endif
        #if ((SRMCATOD_ADC0_SECOND_SAMPLE_CHN >= 4) && (SRMCATOD_ADC0_SECOND_SAMPLE_CHN < 8))
            PCIPORTE.PCR[SRMCATOD_ADC0_SECOND_SAMPLE_CHN + 12] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #endif
    #endif
    #if((SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 8) || (SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 9))
        PCIPORTB.PCR[SRMCATOD_ADC0_FIRST_SAMPLE_CHN - 8] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif((SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 12) || (SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 13))
        PCIPORTB.PCR[SRMCATOD_ADC0_FIRST_SAMPLE_CHN - 10] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif((SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 14) || (SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 15))
        PCIPORTC.PCR[SRMCATOD_ADC0_FIRST_SAMPLE_CHN - 14] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif((SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 17) || (SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 18))
        PCIPORTE.PCR[SRMCATOD_ADC0_FIRST_SAMPLE_CHN + 7] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #endif
    #if((SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 8) || (SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 9))
        PCIPORTB.PCR[SRMCATOD_ADC0_SECOND_SAMPLE_CHN - 8] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif((SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 12) || (SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 13))
        PCIPORTB.PCR[SRMCATOD_ADC0_SECOND_SAMPLE_CHN - 10] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif((SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 14) || (SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 15))
        PCIPORTC.PCR[SRMCATOD_ADC0_SECOND_SAMPLE_CHN - 14] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif((SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 17) || (SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 18))
        PCIPORTE.PCR[SRMCATOD_ADC0_SECOND_SAMPLE_CHN + 7] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #endif
    // Additional sample
    #if(SRMCATOD_ENABLE_ADDITIONAL_READING == 1)
        #if(SRMCATOD_ADDITIONAL_READING_ADC_ENGINE == 0)
            #if(SRMCATOD_ADDITIONAL_READING_MUX_SELECT == 1)
                #if(SRMCATOD_ADDITIONAL_READING_CHANNEL == 4)
                    PCIPORTC.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL - 2] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
                #elif(SRMCATOD_ADDITIONAL_READING_CHANNEL == 5)
                    PCIPORTD.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL - 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
                #elif((SRMCATOD_ADDITIONAL_READING_CHANNEL == 6) || (SRMCATOD_ADDITIONAL_READING_CHANNEL == 7))
                    PCIPORTD.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL - 1] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
                #endif
            #else
                #if ((SRMCATOD_ADDITIONAL_READING_CHANNEL >= 4) && (SRMCATOD_ADDITIONAL_READING_CHANNEL < 8))
                    PCIPORTE.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL + 12] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
                #endif
                #if ((SRMCATOD_ADDITIONAL_READING_CHANNEL >= 4) && (SRMCATOD_ADDITIONAL_READING_CHANNEL < 8))
                    PCIPORTE.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL + 12] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
                #endif
            #endif
            #if((SRMCATOD_ADDITIONAL_READING_CHANNEL == 8) || (SRMCATOD_ADDITIONAL_READING_CHANNEL == 9))
                PCIPORTB.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL - 8] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
            #elif((SRMCATOD_ADDITIONAL_READING_CHANNEL == 12) || (SRMCATOD_ADDITIONAL_READING_CHANNEL == 13))
                PCIPORTB.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL - 10] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
            #elif((SRMCATOD_ADDITIONAL_READING_CHANNEL == 14) || (SRMCATOD_ADDITIONAL_READING_CHANNEL == 15))
                PCIPORTC.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL - 14] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
            #elif((SRMCATOD_ADDITIONAL_READING_CHANNEL == 17) || (SRMCATOD_ADDITIONAL_READING_CHANNEL == 18))
                PCIPORTE.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL + 7] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
            #endif
        #endif
    #endif
    // Channel MUX configuration
    #if (SRMCATOD_ADC0_MUX_SET == 1)
    ADC0.ADC_CFG2 |= ADC_CFG2_MUXSEL_MASK;
    #endif
	// hardware trigger enabled
	ADC0.ADC_SC2 |= ADC_SC2_ADTRG_MASK;
	// hardware averaging disabled and calibration bit cleared (allows next calibration to be called next reset)
	ADC0.ADC_SC3 = 0;

	// start conversion of channels and in this application, disables ADC0 interrupts
	// ADC0 end of scan interrupt is disabled. Only ADC1 Interrupt is used
	ADC0.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC0_FIRST_SAMPLE_CHN);
	ADC0.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC0_SECOND_SAMPLE_CHN);
}

/**
 *  @brief      ADC self calibration procedure start
 *
 *  @details    Recommended on AN3949 to operate with ADC clock between 2MHz and 4MHz, in high speed mode and 32 samples averaged.
 *              Calibration should be run with both VDDA and VREFH at or above 3V.
 *              The input channel, conversion mode setting, compare function values, resolution, and differential and single-ended
 *              settings have no effect on the calibration result.
 *              Software trigger selected: ADCx_SC2 bit ADTRG must be cleared before calibration.
 *              Disable interruptions during calibration.
 *              Calibration must be performed at least every reset.
 *              For single ended calibrations, minus side gain register can be skipped.
 *              Calibration may take as long as 15,000 ADCK cycles + 100 bus cycles
 *
 */
static void SRCalibrateAD0(void)
{
    uint16 calib;
	// setting the clock to 7 MHz (ADIV and ADICLK bits), long sample time and single-ended 12-bit conversion (MODE bits);
	ADC0.ADC_CFG1 = ADC_CFG1_ADIV(SRMCATOD_ADC0_CAL_CLOCK_DIVIDE_SEL)
					| ADC_CFG1_MODE(SRMCATOD_ADC0_RES_MODE)
					| ADC_CFG1_ADICLK(SRMCATOD_ADC0_CLK_INPUT)
					| ADC_CFG1_ADLSMP_MASK;

	ADC0.ADC_CFG2 = ADC_CFG2_ADHSC_MASK;			// High speed configuration with longest sampling time(20 extra ADCK cycles, 24 total)

	ADC0.ADC_SC2 = 0;								// Software trigger selected

	// HW averaging enabled, 32 samples averaged, one shot conversion mode
	ADC0.ADC_SC3 = ADC_SC3_AVGE_MASK
					| ADC_SC3_AVGS(SRMCATOD_ADC1_CAL_HW_AVG_SEL);

	// starting the calibration of ADC0
	ADC0.ADC_SC3 |= ADC_SC3_CAL_MASK;
	do
	{
		// waits until the calibration completes
		if(( ADC0.ADC_SC3 & ADC_SC3_CALF_MASK ) == ADC_SC3_CALF_MASK)
		{
			Micro__Trap();						    // traps in here in case calibration failed, waiting for the watchdog (must be enabled before calling this configuration)
		}
		SERVICE_WATCHDOG();
		
	} while (((ADC0.ADC_SC1[0] & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK));

	calib = 0;										//16 bit RAM initialized
	calib += ADC0.ADC_CLP0;
	calib += ADC0.ADC_CLP1;
	calib += ADC0.ADC_CLP2;
	calib += ADC0.ADC_CLP3;
	calib += ADC0.ADC_CLP4;
	calib += ADC0.ADC_CLPS;							// Calibration results added
	calib = calib >> 1;								// Divided by 2
	calib = calib | 0x8000; 						// set the MSB (2's complement)
	ADC0.ADC_PG = calib; 							// Plus side gain register updated
	// Minus side gain register updated (not required for single ended calibrations)
	calib = 0;										//16 bit RAM initialized again, just in case
	calib += ADC0.ADC_CLM0;
	calib += ADC0.ADC_CLM1;
	calib += ADC0.ADC_CLM2;
	calib += ADC0.ADC_CLM3;
	calib += ADC0.ADC_CLM4;
	calib += ADC0.ADC_CLMS;                         // Calibration results added
	calib = calib >> 1;                             // Divided by 2
	calib = calib | 0x8000;                         // set the MSB (2's complement)
	ADC0.ADC_MG = calib;                            // Minus side gain register updated
}

/**
 *  @brief      Initialization of ADC1
 *
 *  @details    This function configures ADC1 to sample 2 channels triggered by PDB0 delay in respect to FTM0 event.
 *
 */
static void SRInitializeAD1(void)
{
	Micro__NVICDisableIRQ(ADC1_IRQ_CHANNEL);		// Disables ADC1 interruption during configuration
	// System clock enabling
	SIM.SCGC6 |= SIM_SCGC6_ADC1_MASK;				// Enables ADC1 clock
	SRCalibrateAD1();

	ADC1.ADC_CFG2 = ADC_CFG2_ADHSC_MASK;			// High speed configuration
	// Configure ports as analog input
	// First and Second samples
    #if(SRMCATOD_ADC1_MUX_SET == 1)
        #if((SRMCATOD_ADC1_FIRST_SAMPLE_CHN >= 4) && (SRMCATOD_ADC1_FIRST_SAMPLE_CHN < 8))
            PCIPORTC.PCR[SRMCATOD_ADC1_FIRST_SAMPLE_CHN + 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #endif

        #if((SRMCATOD_ADC1_SECOND_SAMPLE_CHN >= 4) && (SRMCATOD_ADC1_SECOND_SAMPLE_CHN < 8))
            PCIPORTC.PCR[SRMCATOD_ADC1_SECOND_SAMPLE_CHN + 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #endif
    #else
        #if ((SRMCATOD_ADC1_FIRST_SAMPLE_CHN >= 4) && (SRMCATOD_ADC1_FIRST_SAMPLE_CHN < 8))
            PCIPORTE.PCR[SRMCATOD_ADC1_FIRST_SAMPLE_CHN -4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #endif
        #if ((SRMCATOD_ADC1_SECOND_SAMPLE_CHN >= 4) && (SRMCATOD_ADC1_SECOND_SAMPLE_CHN < 8))
            PCIPORTE.PCR[SRMCATOD_ADC1_SECOND_SAMPLE_CHN -4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
        #endif
    #endif
    #if((SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 14) || (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 15))
        PCIPORTB.PCR[SRMCATOD_ADC1_FIRST_SAMPLE_CHN - 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif(SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 17)
        PCIPORTA.PCR[SRMCATOD_ADC1_FIRST_SAMPLE_CHN] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #endif
    #if((SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 8) || (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 9))
        PCIPORTB.PCR[SRMCATOD_ADC1_SECOND_SAMPLE_CHN - 8] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif((SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 14) || (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 15))
        PCIPORTB.PCR[SRMCATOD_ADC1_FIRST_SAMPLE_CHN - 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #elif(SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 17)
        PCIPORTA.PCR[SRMCATOD_ADC1_SECOND_SAMPLE_CHN] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
    #endif
    // Additional sample
    #if(SRMCATOD_ENABLE_ADDITIONAL_READING == 1)
        #if(SRMCATOD_ADDITIONAL_READING_ADC_ENGINE == 1)
            #if(SRMCATOD_ADDITIONAL_READING_MUX_SELECT == 1)
                #if((SRMCATOD_ADDITIONAL_READING_CHANNEL >= 4) && (SRMCATOD_ADDITIONAL_READING_CHANNEL < 8))
                    PCIPORTC.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL + 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
                #endif
            #else
                #if ((SRMCATOD_ADDITIONAL_READING_CHANNEL >= 4) && (SRMCATOD_ADDITIONAL_READING_CHANNEL < 8))
                    PCIPORTE.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL -4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
                #endif
            #endif
            #if((SRMCATOD_ADDITIONAL_READING_CHANNEL == 14) || (SRMCATOD_ADDITIONAL_READING_CHANNEL == 15))
                PCIPORTB.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL - 4] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
            #elif(SRMCATOD_ADDITIONAL_READING_CHANNEL == 17)
                PCIPORTA.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
            #elif((SRMCATOD_ADDITIONAL_READING_CHANNEL == 8) || (SRMCATOD_ADDITIONAL_READING_CHANNEL == 9))
                PCIPORTB.PCR[SRMCATOD_ADDITIONAL_READING_CHANNEL - 8] = PORT_PCR_MUX(SRMCATOD_ANALOG_INPUT_CONFIG);
            #endif
        #endif
    #endif
    // Channel MUX configuration
#if (SRMCATOD_ADC1_MUX_SET == 1)
    ADC1.ADC_CFG2 |= ADC_CFG2_MUXSEL_MASK;
#endif
    // setting prescaler to get ADCK = bus clock, 12 bits resolution, short sample time and normal operation mode
    ADC1.ADC_CFG1 = ADC_CFG1_ADIV(SRMCATOD_ADC1_CLOCK_DIVIDE_SEL)
    				| ADC_CFG1_ADICLK(SRMCATOD_ADC1_CLK_INPUT)
					| ADC_CFG1_MODE(SRMCATOD_ADC1_RES_MODE);

    // hardware trigger enabled
	ADC1.ADC_SC2 |= ADC_SC2_ADTRG_MASK;
    // hardware averaging disabled and calibration bit cleared (allows next calibration to be called next reset)
    ADC1.ADC_SC3 = 0;

    // start conversion of channels and in this application, disables ADC interrupts
    ADC1.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC1_FIRST_SAMPLE_CHN);
    ADC1.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC1_SECOND_SAMPLE_CHN)|ADC_SC1_AIEN_MASK;
	EnableEndOfConversionInteruption(SR_AD_ENGINE_1);
}

/**
 *  @brief      ADC self calibration procedure start
 *
 *  @details    Recommended on AN3949 to operate with ADC clock between 2MHz and 4MHz, in high speed mode and 32 samples averaged.
 *              Calibration should be run with both VDDA and VREFH at or above 3V.
 *              The input channel, conversion mode setting, compare function values, resolution, and differential and single-ended
 *              settings have no effect on the calibration result.
 *              Software trigger selected: ADCx_SC2 bit ADTRG must be cleared before calibration.
 *              Disable interruptions during calibration.
 *              Calibration must be performed at least every reset.
 *              For single ended calibrations, minus side gain register can be skipped.
 *              Calibration may take as long as 15,000 ADCK cycles + 100 bus cycles
 *
 */
static void SRCalibrateAD1(void)
{
    uint16 calib;
	// setting the clock to 7 MHz (ADIV and ADICLK bits), long sample time and single-ended 12-bit conversion (MODE bits);
	ADC1.ADC_CFG1 = ADC_CFG1_ADIV(SRMCATOD_ADC1_CAL_CLOCK_DIVIDE_SEL)
					| ADC_CFG1_MODE(SRMCATOD_ADC1_RES_MODE)
					| ADC_CFG1_ADICLK(SRMCATOD_ADC1_CLK_INPUT)
					| ADC_CFG1_ADLSMP_MASK;

	ADC1.ADC_CFG2 = ADC_CFG2_ADHSC_MASK;            // High speed configuration with longest sampling time(20 extra ADCK cycles, 24 total)

	ADC1.ADC_SC2 = 0;                               // Software trigger selected

	// HW averaging enabled, 32 samples averaged, one shot conversion mode
	ADC1.ADC_SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(SRMCATOD_ADC1_CAL_HW_AVG_SEL);// | ADC_SC3_ADCO_MASK;

	// starting the calibration of ADC1
	ADC1.ADC_SC3 |= ADC_SC3_CAL_MASK;

	do
	{
		// waits until the calibration completes
		if ((ADC1.ADC_SC3 & ADC_SC3_CALF_MASK ) == ADC_SC3_CALF_MASK)
		{
			Micro__Trap();								// traps in here in case calibration failed, waiting for the watchdog (must be enabled before calling this configuration)
		}
		SERVICE_WATCHDOG();
		
	} while (((ADC1.ADC_SC1[0] & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK));

	calib = 0;										//16 bit RAM initialized
	calib += ADC1.ADC_CLP0;
	calib += ADC1.ADC_CLP1;
	calib += ADC1.ADC_CLP2;
	calib += ADC1.ADC_CLP3;
	calib += ADC1.ADC_CLP4;
	calib += ADC1.ADC_CLPS;							// Calibration results added
	calib = calib >> 1;								// Divided by 2
	calib = calib | 0x8000; 						// set the MSB (2's complement)
	ADC1.ADC_PG = calib; 							// Plus side gain register updated
	// Minus side gain register updated (not required for single ended calibrations)
	calib = 0;										//16 bit RAM initialized again, just in case
	calib += ADC1.ADC_CLM0;
	calib += ADC1.ADC_CLM1;
	calib += ADC1.ADC_CLM2;
	calib += ADC1.ADC_CLM3;
	calib += ADC1.ADC_CLM4;
	calib += ADC1.ADC_CLMS;                         // Calibration results added
	calib = calib >> 1;                             // Divided by 2
	calib = calib | 0x8000;                         // set the MSB (2's complement)
	ADC1.ADC_MG = calib;                            // Minus side gain register updated
}

/**
 *  @brief      PDB0 peripheral initialization
 *
 */
static void SRInitializePDB0(void)
{
    // PDB configuration
	SIM.SCGC6 |= SIM_SCGC6_PDB_MASK;				// Enables PDB clock

	Micro__NVICDisableIRQ(PDB_IRQ_CHANNEL);

	PDB.MOD = SRMCATOD_PDB_MODULUS_RELOAD & 0xFFFF;			// Specifies counter duration. When the counter reaches this value, it will be reset back to zero (16 bits  read/write).

	PDB.CH[0].DLY[0] = SRMCATOD_PDB_DELAY_FIRST_CONVERSION;	// Defines a delay to avoid ringing noise in respect to center aligned PWM reload trigger event
	PDB.CH[1].DLY[0] = SRMCATOD_PDB_DELAY_FIRST_CONVERSION; // Defines a delay to avoid ringing noise in respect to center aligned PWM reload trigger event


	PDB.CH[0].C1 = PDB_C1_BB(SRMCATOD_PDB_PRE_TRIGGER_CHN_1) // back-to-back mode enabled
			| PDB_C1_TOS(SRMCATOD_PDB_PRE_TRIGGER_CHN_0)
			| PDB_C1_EN(SRMCATOD_PDB_PRE_TRIGGER_CHN_0)
			| PDB_C1_TOS(SRMCATOD_PDB_PRE_TRIGGER_CHN_1)
			| PDB_C1_EN(SRMCATOD_PDB_PRE_TRIGGER_CHN_1);

	PDB.CH[1].C1 = PDB_C1_BB(SRMCATOD_PDB_PRE_TRIGGER_CHN_1) // back-to-back mode enabled
			| PDB_C1_TOS(SRMCATOD_PDB_PRE_TRIGGER_CHN_0)
			| PDB_C1_EN(SRMCATOD_PDB_PRE_TRIGGER_CHN_0)
			| PDB_C1_TOS(SRMCATOD_PDB_PRE_TRIGGER_CHN_1)
			| PDB_C1_EN(SRMCATOD_PDB_PRE_TRIGGER_CHN_1);

	PDB.SC = 		PDB_SC_LDMOD(SRMCATOD_PDB_LD_MOD_SETTING)	// Configures PDB to load after trigger event and LDOK set
			| PDB_SC_PRESCALER(SRMCATOD_PDB_PRESCALER_SETTING)
			| PDB_SC_TRGSEL(SRMCATOD_PDB_TRIGGER_SEL)
			| PDB_SC_MULT(SRMCATOD_PDB_MULT_PRESCALER)
			| PDB_SC_PDBEIE_MASK					// Enables interruption error
			| PDB_SC_PDBEN_MASK;       				// PDB enabled

	PDB.SC |= PDB_SC_LDOK_MASK;						// PDB load OK

	Micro__NVICEnableIRQ(PDB_IRQ_CHANNEL, SRMCATOD_INTERRUPT_PDB0_PREEMPTION_PRIORITY,SRMCATOD_PDB0_INTERRUPT_SUB_PRIORITY);
	/* switch the trigger of the ADC to PDB */
	SIM.SOPT7 = 0;
}


/**
 *  @brief      This function enables conversion interruption for selected engine.
 *
 *  @param[in]  engine Selected engine may be AD_ENGINE_0 or AD_ENGINE_1
 *
 */
static void EnableEndOfConversionInteruption(SRMCATOD_CONVERTER_TYPE engine)
{
    if (engine == SR_AD_ENGINE_0)
    {
    	Micro__NVICEnableIRQ(ADC0_IRQ_CHANNEL, SRMCATOD_INTERRUPT_ADC_PREEMPTION_PRIORITY,SRMCATOD_ADC_INTERRUPT_SUB_PRIORITY);
    }
    else if (engine == SR_AD_ENGINE_1)
    {
    	Micro__NVICEnableIRQ(ADC1_IRQ_CHANNEL, SRMCATOD_INTERRUPT_ADC_PREEMPTION_PRIORITY,SRMCATOD_ADC_INTERRUPT_SUB_PRIORITY);
    }
}

/**
 *  @brief      This function disables conversion interruption for selected engine.
 *
 *  @param      engine Selected engine may be AD_ENGINE_0 or AD_ENGINE_1
 *
 */
static void DisableEndOfConversionInteruption(SRMCATOD_CONVERTER_TYPE engine)
{
    if (engine == SR_AD_ENGINE_0)
    {
    	Micro__NVICDisableIRQ(ADC0_IRQ_CHANNEL);			// Disables ADC0 interruption during configuration
    }
    else if (engine == SR_AD_ENGINE_1)
    {
    	Micro__NVICDisableIRQ(ADC1_IRQ_CHANNEL);			// Disables ADC0 interruption during configuration
    }
}


/**
 *  @brief      Enable self diagnostic mode
 *
 *  @details    This function is used to enable/disable self check hardware for converter and register integrity.
 *              Drives to VREFH0, (VREFH0 x 1/2) and (VREFH0 x 0)
 *
 *  @param[in]  engine Defines which converter will be configured: AD_ENGINE_0 or AD_ENGINE_1
 *  @param[in]  mode Defines configuration mode:
 *              SELF_TEST_DISABLED,SELF_TEST_VREFH0_SCALE_0,SELF_TEST_VREFH0_SCALE_0_5,SELF_TEST_VREFH0_SCALE_1, AUTOMATED_SELF_TEST
 *
 */
static void SREnableSelfDiagMode(SRMCATOD_CONVERTER_TYPE engine, SRMCATOD_DIAG_CONFIG_TYPE mode)
{
	if (engine == SR_AD_ENGINE_0)                  	// Configuration for S12AD0
	{
		switch (mode)                            	// Mode for self check
		{
			case SR_MCATOD_SELF_TEST_DISABLED:

				ADC0.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC0_FIRST_SAMPLE_CHN);
				ADC0.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC0_SECOND_SAMPLE_CHN);
				break;

			case SR_MCATOD_SELF_TEST_VREFH0_SCALE_0:
				ADC0.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC_VREF_LOW);
				ADC0.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC_VREF_LOW);

				break;

			case SR_MCATOD_SELF_TEST_VREFH0_SCALE_0_5:

				ADC0.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC0_VREF_INTERMEDIATE);
				ADC0.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC0_VREF_INTERMEDIATE);
				break;

			case SR_MCATOD_SELF_TEST_VREFH0_SCALE_1:

				ADC0.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC_VREF_HIGH);
				ADC0.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC_VREF_HIGH);
				break;

			case SR_MCATOD_AUTOMATED_SELF_TEST:

				SRInitializeAD0();					// Performs self calibration routine and initializes converter configuration
				break;

			default:                                // In case it's not properly defined

				SRInitializeAD0();					// Initializes converter configuration
				break;
		}
	}                                               // Uploads mirror data
	else if (engine == SR_AD_ENGINE_1)             	// Configuration for S12AD0
	{
		switch (mode)                               // Mode for self check
		{
			case SR_MCATOD_SELF_TEST_DISABLED:

			    ADC1.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC1_FIRST_SAMPLE_CHN);
			    ADC1.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC1_SECOND_SAMPLE_CHN) | ADC_SC1_AIEN_MASK;
				break;

			case SR_MCATOD_SELF_TEST_VREFH0_SCALE_0:

				ADC1.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC_VREF_LOW);
				ADC1.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC_VREF_LOW) | ADC_SC1_AIEN_MASK;
				break;

			case SR_MCATOD_SELF_TEST_VREFH0_SCALE_0_5:

				ADC1.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC1_VREF_INTERMEDIATE);
				ADC1.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC1_VREF_INTERMEDIATE) | ADC_SC1_AIEN_MASK;
				break;

			case SR_MCATOD_SELF_TEST_VREFH0_SCALE_1:

				ADC1.ADC_SC1[0] = ADC_SC1_ADCH(SRMCATOD_ADC_VREF_HIGH);
				ADC1.ADC_SC1[1] = ADC_SC1_ADCH(SRMCATOD_ADC_VREF_HIGH) | ADC_SC1_AIEN_MASK;
				break;

			case SR_MCATOD_AUTOMATED_SELF_TEST:

				SRInitializeAD1();					// Performs self calibration routine and initializes converter configuration
				break;

			default:                                // In case it's not properly defined

				SRInitializeAD1();					// Performs self calibration routine and initializes converter configuration
				break;
		}
    }
}

/**
 *  @brief      Detect if the peripheral is in safe status
 *
 *  @details    This function is used to check register and converter status.
 *              May be used to test stuck at register failure and conversion errors.
 *
 *  @param[in]  engine defines which engine will be tested
 *  @param[in]  threshold allows adjustable error for 1/2 VREFH0 conversion
 *  @param[in]  mode The test mode that we need to check:
 *              SELF_TEST_DISABLED,SELF_TEST_VREFH0_SCALE_0,SELF_TEST_VREFH0_SCALE_0_5,SELF_TEST_VREFH0_SCALE_1, AUTOMATED_SELF_TEST
 *
 *  @return     returns if peripheral test result TEST_NOT_PERFORMED, REGISTER_BIT_STUCK_AT_0,
 *              REGISTER_BIT_STUCK_AT_1, CONVERTER_ERROR or PERIPHERAL_SAFE
 */
static SRMCATOD_DIAG_ERROR_TYPE IsConverterSafe(SRMCATOD_CONVERTER_TYPE engine,sint16 local_threshold, SRMCATOD_DIAG_CONFIG_TYPE mode)
{
    SRMCATOD_DIAG_ERROR_TYPE status;                // returns test result
    sint16 temp;                        	// Gets converted result for check
    sint16 temp_a;                        // Gets converted result for check
    sint16 temp_b;                        // Gets converted result for check

    temp_a = 0;
    temp_b = 0;

    if (engine == SR_AD_ENGINE_0)                   // Configuration for testing ADC0
    {
        // Get data
    	temp_a = SRMCATOD__ADC0_RA();
    	temp_b = SRMCATOD__ADC0_RB();
    }
    else if (engine == SR_AD_ENGINE_1)              // Configuration for testing ADC1
    {
        // Get data
    	temp_a = SRMCATOD__ADC1_RA();
    	temp_b = SRMCATOD__ADC1_RB();
    }
    switch (mode)                                   // Now it's all set for checking data according to test mode
    {
        case SR_MCATOD_SELF_TEST_DISABLED:                 // If it wasn't tested
            status = SR_MCATOD_ERROR_TEST_NOT_PERFORMED;         // It's not possible to return peripheral is safe
            break;

        case SR_MCATOD_SELF_TEST_VREFH0_SCALE_0:           // If connected to minimum reference
            status = SR_MCATOD_ERROR_REGISTER_BIT_STUCK_AT_1;    // Possible fault is bit stuck at 1
            temp = temp_a | temp_b;

            if (temp <= ((sint16)(SR_MCATOD_MINIMUM_12BITS_AD_VALUE) + local_threshold))
            {                                       // Expected data on register is found
                status = SR_MCATOD_ERROR_PERIPHERAL_SAFE;        // Peripheral doesn't show this failure mode
            }
            break;

        case SR_MCATOD_SELF_TEST_VREFH0_SCALE_0_5:         // If connected to half maximum value
            status = SR_MCATOD_ERROR_CONVERTER_ERROR;            // Possible fault is erroneous conversion
            temp = (temp_a + temp_b) >> 1;

            if ((temp >= ((sint16)(SR_MCATOD_HALF_RANGE_12BITS_AD_VALUE) - local_threshold)) &&
                (temp <= ((sint16)(SR_MCATOD_HALF_RANGE_12BITS_AD_VALUE) + local_threshold)))
            {                                       // Expected data on register is found
                status = SR_MCATOD_ERROR_PERIPHERAL_SAFE;        // Peripheral doesn't show this failure mode
            }
            break;

        case SR_MCATOD_SELF_TEST_VREFH0_SCALE_1:           // If connected to maximum value
            status = SR_MCATOD_ERROR_REGISTER_BIT_STUCK_AT_0;    // Possible fault is bit stuck at 0
            temp = temp_a & temp_b;

            if (temp >= ((sint16)(SR_MCATOD_MAXIMUM_12BITS_AD_VALUE) - local_threshold))
            {                                       // Expected data on register is found
                status = SR_MCATOD_ERROR_PERIPHERAL_SAFE;        // Peripheral doesn't show this failure mode
            }
            break;

        default:
            status = SR_MCATOD_ERROR_TEST_NOT_PERFORMED;         // It's not possible to return peripheral is safe
            break;
    }

    return (status);                                // Returns status
}

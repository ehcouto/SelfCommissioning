/**
 *  @defgroup   CLASS_B
 *  @file       SRMCAtod_prm.h
 *  @brief      Adjustable parameters for motor control conversions
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMCATOD_PRM_H_
	#define SRMCATOD_PRM_H_
#include "SRMC_defs.h"
#include "SRMotorSafetyMgr.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    SRMCATOD_CH0,                  // 00 - ADCx_DP0
    SRMCATOD_CH1,                  // 01 - ADCx_DP1
    SRMCATOD_CH2,                  // 02 - ADC0_DP2 (Reserved only for ADC1)
    SRMCATOD_CH3,                  // 03 - ADCx_DP3
    SRMCATOD_CH4,                  // 04 - ADCx_SE4a or ADCx_SE4b (depending on MUXSEL)
    SRMCATOD_CH5,                  // 05 - ADCx_SE5a or ADCx_SE5b (depending on MUXSEL)
    SRMCATOD_CH6,                  // 06 - ADCx_SE6a or ADCx_SE6b (depending on MUXSEL)
    SRMCATOD_CH7,                  // 07 - ADCx_SE7a or ADCx_SE7b (depending on MUXSEL)
    SRMCATOD_CH8,                  // 08 - ADCx_SE8
    SRMCATOD_CH9,                  // 09 - ADCx_SE9
	SRMCATOD_RESERVED_0,           // 10 - Reserved.
	SRMCATOD_RESERVED_1,           // 11 - Reserved.
    SRMCATOD_CH12,                 // 12 - ADC0_SE12 (Reserved only for ADC1)
    SRMCATOD_CH13,                 // 13 - ADC0_SE13 (Reserved only for ADC1)
    SRMCATOD_CH14,                 // 14 - ADCx_SE14
    SRMCATOD_CH15,                 // 15 - ADCx_SE15
    SRMCATOD_CH16,                 // 16 - Reserved.
    SRMCATOD_CH17,                 // 17 - ADCx_SE17
    SRMCATOD_CH18,                 // 18 - ADCx_SE18
    SRMCATOD_CH19,                 // 19 - ADC0_DM0
    SRMCATOD_CH20,                 // 20 - ADC0_DM1
    SRMCATOD_CH21,                 // 21 - Reserved.
    SRMCATOD_CH22,                 // 22 - Reserved.
    SRMCATOD_CH23,                 // 23 - ADC0_SE23
    SRMCATOD_RESERVED_2,           // 24 - Reserved.
    SRMCATOD_RESERVED_3,           // 25 - Reserved.
    SRMCATOD_CH26,				   // 26 - Temp Sensor (single-ended)
    SRMCATOD_CH27,                 // 27 - Bandgap (single-ended)
    SRMCATOD_RESERVED_4,           // 28 - Reserved.
    SRMCATOD_CH29,                 // 29 - VREFSH
	SRMCATOD_CH30,                 // 30 - VREFSL
	SRMCATOD_MOD_DISABLED          // 31 - Module disabled
} SRMCATOD_CHANNEL_DEF;

#define SRMCATOD_ADC0_MUX_SET                   0
#define SRMCATOD_ADC1_MUX_SET                   0

#if(WINDY_STRIP_BOARD == 1)
#define SRMCATOD_ADC0_FIRST_SAMPLE_CHN			12	//SRMCATOD_CH12   //current C
#define SRMCATOD_ADC0_SECOND_SAMPLE_CHN	   		4	//SRMCATOD_CH4    // Vdc
#define SRMCATOD_ADC1_FIRST_SAMPLE_CHN			8	//SRMCATOD_CH8    // Current A
#define SRMCATOD_ADC1_SECOND_SAMPLE_CHN			9	//SRMCATOD_CH9   // current B
#elif(WINDY_INTERNATION_BOARD == 1)
#define SRMCATOD_ADC0_FIRST_SAMPLE_CHN          12 //SRMCATOD_CH12   // current C
#define SRMCATOD_ADC0_SECOND_SAMPLE_CHN         9  //SRMCATOD_CH9    // current B
#define SRMCATOD_ADC1_FIRST_SAMPLE_CHN          8  //SRMCATOD_CH8    // Current A
#define SRMCATOD_ADC1_SECOND_SAMPLE_CHN         4  //SRMCATOD_CH4    // Vdc
#else
#error "Need to define a board"
#endif

#define SRMCATOD_ADC_VREF_HIGH					29	//SRMCATOD_CH29
#define SRMCATOD_ADC_VREF_LOW					30	//SRMCATOD_CH30
#define SRMCATOD_ADC0_VREF_INTERMEDIATE			27	//SRMCATOD_CH27 // Bandgap reference 1.0V
#define SRMCATOD_ADC1_VREF_INTERMEDIATE         27	//SRMCATOD_CH27 // Bandgap reference 1.0V

/** SRMCATOD_ENABLE_ADDITIONAL_READING - Enable the additional reading:
 *
 * @param  1   – Enabled
 *         0   – Disabled
*/

#if(WINDY_STRIP_BOARD == 1)
#define SRMCATOD_ENABLE_ADDITIONAL_READING      0
#elif(WINDY_INTERNATION_BOARD == 1)
#define SRMCATOD_ENABLE_ADDITIONAL_READING      1
#endif

#if (SRMCATOD_ENABLE_ADDITIONAL_READING == 1)

/** SRMCATOD_ADDITIONAL_READING_ADC_ENGINE - ADC engine to use for the manual reading:
 *
 * @param  0   – select ADC0 engine
 *         1   – select ADC1 engine
*/
#define SRMCATOD_ADDITIONAL_READING_ADC_ENGINE  0

/** SRMCATOD_ADDITIONAL_READING_MUX_SELECT - MUX select setting for the selected additional channel
 *
 * @param  0   – MUX select disabled
 *         1   – MUX select enabled
*/
#define SRMCATOD_ADDITIONAL_READING_MUX_SELECT  1

/** SRMCATOD_ADDITIONAL_READING_CHANNEL - ADC engine to use for the manual reading:
 *
 * @param  The channel number, according to the SRMCATOD_CHANNEL_DEF type
*/
#if(WINDY_STRIP_BOARD == 1)
#define SRMCATOD_ADDITIONAL_READING_CHANNEL  7  //SRMCATOD_CH7    // Temperature

#define SRMCATOD_ADDITIONAL_READING_INITIAL_VALUE  300
#elif(WINDY_INTERNATION_BOARD == 1)
#define SRMCATOD_ADDITIONAL_READING_CHANNEL  4  //SRMCATOD_CH7    // Temperature new NTC channel

#define SRMCATOD_ADDITIONAL_READING_INITIAL_VALUE 0
#endif

#endif

// Checks if reserved or out of range channels were configured by mistake
#if((SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 10) || \
	(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 11) || \
	(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 16) || \
	(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 21) || \
	(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 22) || \
	(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 24) || \
	(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 25) || \
	(SRMCATOD_ADC0_FIRST_SAMPLE_CHN == 28) || \
	(SRMCATOD_ADC0_FIRST_SAMPLE_CHN > 31) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 10) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 11) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 16) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 21) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 22) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 24) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 25) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN == 28) || \
	(SRMCATOD_ADC0_SECOND_SAMPLE_CHN > 31) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 2) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 10) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 11) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 12) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 13) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 16) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 21) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 22) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 24) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 25) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN == 28) || \
    (SRMCATOD_ADC1_FIRST_SAMPLE_CHN > 31) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 2) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 10) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 11) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 12) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 13) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 16) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 21) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 22) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 24) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 25) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN == 28) || \
    (SRMCATOD_ADC1_SECOND_SAMPLE_CHN > 31))
	#error:"Invalid or reserved channel defined, not available in this device"
#endif

#define SRMCATOD_ANALOG_INPUT_CONFIG			0			// Analog input port configuration

//=====================================================================================================================
/** ADC_CLOCK_DIVIDE_SEL - Clock Divide Select:
 *
 * Selects the divide ratio used by the ADC to generate the internal clock ADCK.
 *     	00 The divide ratio is 1 and the clock rate is input clock.
 *     	01 The divide ratio is 2 and the clock rate is (input clock)/2.
 *     	10 The divide ratio is 4 and the clock rate is (input clock)/4.
 *     	11 The divide ratio is 8 and the clock rate is (input clock)/8.
*/
#if(WINDY_STRIP_BOARD == 1)
#define SRMCATOD_ADC0_CLOCK_DIVIDE_SEL				1
#define SRMCATOD_ADC1_CLOCK_DIVIDE_SEL				1
#elif(WINDY_INTERNATION_BOARD == 1)
#define SRMCATOD_ADC0_CLOCK_DIVIDE_SEL				2
#define SRMCATOD_ADC1_CLOCK_DIVIDE_SEL				2
#endif
/// Specific calibration mode configuration
#define SRMCATOD_ADC0_CAL_CLOCK_DIVIDE_SEL		    3
#define SRMCATOD_ADC1_CAL_CLOCK_DIVIDE_SEL			3

//=====================================================================================================================
/** ADC_RES_MODE - Clock Divide Select:
 *
 *
 * Selects the ADC resolution mode.
 * 		00 When DIFF=0:It is single-ended 8-bit conversion; when DIFF=1, it is differential 9-bit conversion with
 * 2's complement output.
 * 		01 When DIFF=0:It is single-ended 12-bit conversion ; when DIFF=1, it is differential 13-bit conversion
 * with 2's complement output.
 * 		10 When DIFF=0:It is single-ended 10-bit conversion. ; when DIFF=1, it is differential 11-bit conversion
 * with 2's complement output
 * 		11 When DIFF=0:It is single-ended 16-bit conversion..; when DIFF=1, it is differential 16-bit conversion
 * with 2's complement output
*/
#define SRMCATOD_ADC0_RES_MODE		    			1
#define SRMCATOD_ADC1_RES_MODE						1

//=====================================================================================================================
/** ADC_CLK_INPUT - Input Clock Select:
 *
 * Selects the input clock source to generate the internal clock, ADCK. Note that when the ADACK clock
 * source is selected, it is not required to be active prior to conversion start. When it is selected and it is not
 * active prior to a conversion start, when CFG2[ADACKEN]=0, the asynchronous clock is activated at the
 * start of a conversion and deactivated when conversions are terminated. In this case, there is an
 * associated clock startup delay each time the clock source is re-activated.
 * 		00 Bus clock
 * 		01 Alternate clock 2 (ALTCLK2)
 * 		10 Alternate clock (ALTCLK)
 * 		11 Asynchronous clock (ADACK)
*/
#define SRMCATOD_ADC0_CLK_INPUT		    			0
#define SRMCATOD_ADC1_CLK_INPUT						0

//=====================================================================================================================
/** ADC_HW_AVG_SEL - Hardware Average Select:
 *
 * Determines how many ADC conversions will be averaged to create the ADC average result.
 * 00 4 samples averaged.
 * 01 8 samples averaged.
 * 10 16 samples averaged.
 * 11 32 samples averaged.
*/
#define SRMCATOD_ADC0_HW_AVG_SEL						0
#define SRMCATOD_ADC1_HW_AVG_SEL						0
// Specific calibration mode configuration
#define SRMCATOD_ADC0_CAL_HW_AVG_SEL				    3
#define SRMCATOD_ADC1_CAL_HW_AVG_SEL				    3

//=====================================================================================================================
// Programmable delay block
#define SRMCATOD_PDB_MODULUS_RELOAD					0xFFFF	// Specifies counter duration. When the counter reaches this value, it will be reset back to zero.
#define SRMCATOD_PDB_INT_SERVICE_DELAY				0x02C0	// Delay in respect to center aligned PWM event to serve interruption (after all AD conversions are complete)

#if(WINDY_STRIP_BOARD == 1)
/********* PDB DELAY FOR THIRD CURRENT RECONSTRUCTION STRATEGY - Windy Strip/Full boards ***********/
#define SRMCATOD_PDB_DELAY_FIRST_CONVERSION		    0x0014	// Defines a delay to avoid ringing noise in respect to center aligned PWM reload trigger event
/***************************************************************************************************/
/********* PDB DELAY FOR STANDARD CURRENT READING STRATEGY *****************************************/
//#define SRMCATOD_PDB_DELAY_FIRST_CONVERSION         0x0000  // Defines a delay to avoid ringing noise in respect to center aligned PWM reload trigger event
/***************************************************************************************************/
#elif(WINDY_INTERNATION_BOARD == 1)
#define SRMCATOD_PDB_DELAY_FIRST_CONVERSION		    0x0000	// Defines a delay to avoid ringing noise in respect to center aligned PWM reload trigger event
#endif

#define SRMCATOD_PDB_DELAY_SECOND_CONVERSION		0x006C	// Defines a delay to wait after first conversion was complete, in respect to center aligned PWM reload trigger event

//=====================================================================================================================
/** PDB - Load Mode selection:
 * Selects the mode to load the MOD, IDLY, CHnDLYm, INTx, and POyDLY registers, after 1 is written to LDOK.
 *
 * 		00 The internal registers are loaded with the values from their buffers immediately after 1 is written to
 * LDOK.
 * 		01 The internal registers are loaded with the values from their buffers when the PDB counter reaches
 * the MOD register value after 1 is written to LDOK.
 * 		10 The internal registers are loaded with the values from their buffers when a trigger input event is
 * detected after 1 is written to LDOK.
 * 		11 The internal registers are loaded with the values from their buffers when either the PDB counter
 * reaches the MOD register value or a trigger input event is detected, after 1 is written to LDOK.
*/
#define SRMCATOD_PDB_LD_MOD_SETTING						0x02

//=====================================================================================================================
/** PDB - Prescaler Divider Selection:
 *
 * 		000 Counting uses the peripheral clock divided by multiplication factor selected by MULT.
 * 		001 Counting uses the peripheral clock divided by twice of the multiplication factor selected by MULT.
 * 		010 Counting uses the peripheral clock divided by four times of the multiplication factor selected by
 * MULT.
 * 		011 Counting uses the peripheral clock divided by eight times of the multiplication factor selected by
 * MULT.
 * 		100 Counting uses the peripheral clock divided by 16 times of the multiplication factor selected by
 * MULT.
 * 		101 Counting uses the peripheral clock divided by 32 times of the multiplication factor selected by
 * MULT.
 * 		110 Counting uses the peripheral clock divided by 64 times of the multiplication factor selected by
 * MULT.
 * 		111 Counting uses the peripheral clock divided by 128 times of the multiplication factor selected by
 * MULT.
*/
#define SRMCATOD_PDB_PRESCALER_SETTING					0x00

//=====================================================================================================================
/** PDB - Trigger Input Source Select
 * Selects the trigger input source for the PDB. The trigger input source can be internal or external (EXTRG
 * pin), or the software trigger. Refer to chip configuration details for the actual PDB input trigger
 * connections.
 * 0000 Trigger-In 0 is External Trigger selected.
 * 0001 Trigger-In 1 is CMP 0 selected.
 * 0010 Trigger-In 2 is CMP 1 selected.
 * 0011 Trigger-In 3 is reserved.
 * 0100 Trigger-In 4 is PIT Ch 0 Output selected.
 * 0101 Trigger-In 5 is PIT Ch 1 Output selected.
 * 0110 Trigger-In 6 is PIT Ch 2 Output selected.
 * 0111 Trigger-In 7 is PIT Ch 3 Output selected.
 * 1000 Trigger-In 8 is FTM0 initialization trigger and channel triggers programmed in the FTM external trigger register (EXTTRIG) selected.
 * 1001 Trigger-In 9 is FTM1 initialization trigger and channel triggers programmed in the FTM external trigger register (EXTTRIG) selected.
 * 1010 Trigger-In 10 isFTM2 initialization trigger and channel triggers programmed in the FTM external trigger register (EXTTRIG) selected.
 * 1011 Trigger-In 11 is FTM3 initialization trigger and channel triggers programmed in the FTM external trigger register (EXTTRIG) selected.  selected.
 * 1100 Trigger-In 12 is reserved.
 * 1101 Trigger-In 13 is reserved.
 * 1110 Trigger-In 14 is LPTMR Output selected.
 * 1111 Software trigger is selected.
*/
#define SRMCATOD_PDB_TRIGGER_SEL							0x08

//=====================================================================================================================
/** PDB - Multiplication Factor Select for Prescaler
 * Selects the multiplication factor of the prescaler divider for the counter clock.
 * 00 Multiplication factor is 1.
 * 01 Multiplication factor is 10.
 * 10 Multiplication factor is 20.
 * 11 Multiplication factor is 40.
*/
#define SRMCATOD_PDB_MULT_PRESCALER						0x00

// For this application, 2 PDB channels are available
#define SRMCATOD_PDB_PRE_TRIGGER_CHN_0				    0x01
#define SRMCATOD_PDB_PRE_TRIGGER_CHN_1				    0x02

#define SRMCATOD_INTERRUPT_PDB0_PREEMPTION_PRIORITY		0
#define SRMCATOD_PDB0_INTERRUPT_SUB_PRIORITY			0

#define SRMCATOD_INTERRUPT_ADC_PREEMPTION_PRIORITY		0
#define SRMCATOD_ADC_INTERRUPT_SUB_PRIORITY				0


//=====================================================================================================================
/** EOS ISR HANDLER -  Prescaler Divider Selection:
 * 0 EOS ISR Handler Frequency is PWM frequency.
 * 1 EOS ISR Handler Frequency is PWM frequency/2.
 * 2 EOS ISR Handler Frequency is PWM frequency/4.
*/
#define SRMCATOD_EOC_ISR_HANDLER_PRESCALER              0

//=====================================================================================================================
/**
 * Check on schematics where are the currents and DC bus voltage pins in order to perform proper assignments here.
 * Use ADC0__RA, ADC0__RB, ADC1__RA and ADC1__RB as channels for motor readings
 */
#if(WINDY_STRIP_BOARD == 1)
#define SRMCATOD_I1_DATA_REG()              SRMCATOD__ADC1_RA()			// Recommended to read currents first to make reconstruction
#define SRMCATOD_I2_DATA_REG()            	SRMCATOD__ADC1_RB()
#define SRMCATOD_I3_DATA_REG()            	SRMCATOD__ADC0_RA()			// Recommended to read currents first to make reconstruction
#define SRMCATOD_HVDC_BUS_DATA_REG()      	SRMCATOD__ADC0_RB()

//--- additional defines for third current reconstruction strategy
// reading
#define SRMCATOD_I2_DATA_REG_REC1()         SRMCATOD__ADC1_RA() // Current B on ADC1
#define SRMCATOD_I3_DATA_REG_REC1()         SRMCATOD__ADC0_RA() // Current C on ADC0
#define SRMCATOD_I1_DATA_REG_REC2()         SRMCATOD__ADC1_RA() // Current A on ADC1
#define SRMCATOD_I3_DATA_REG_REC2()         SRMCATOD__ADC0_RA() // Current C on ADC0
#define SRMCATOD_I1_DATA_REG_REC3()         SRMCATOD__ADC1_RA() // Current A on ADC1
#define SRMCATOD_I2_DATA_REG_REC3()         SRMCATOD__ADC0_RA() // Current B on ADC0

//configuring
#define SRMCATOD_I2_ADC_CONF_REC1()         SRMCATOD__SET_ADC1_SAMPLE_CHN(0, 9)  // Current B on ADC1
#define SRMCATOD_I3_ADC_CONF_REC1()         SRMCATOD__SET_ADC0_SAMPLE_CHN(0, 12) // Current C on ADC0
#define SRMCATOD_I1_ADC_CONF_REC2()         SRMCATOD__SET_ADC1_SAMPLE_CHN(0, 8)  // Current A on ADC1
#define SRMCATOD_I3_ADC_CONF_REC2()         SRMCATOD__SET_ADC0_SAMPLE_CHN(0, 12) // Current C on ADC0
#define SRMCATOD_I1_ADC_CONF_REC3()         SRMCATOD__SET_ADC1_SAMPLE_CHN(0, 8)  // Current A on ADC1
#define SRMCATOD_I2_ADC_CONF_REC3()         SRMCATOD__SET_ADC0_SAMPLE_CHN(0, 9)  // Current B on ADC0
//default
#define SRMCATOD_I1_ADC_CONF_DEF()          SRMCATOD__SET_ADC1_SAMPLE_CHN(0, 8)  // Current A on ADC1
#define SRMCATOD_I2_ADC_CONF_DEF()
#define SRMCATOD_I3_ADC_CONF_DEF()          SRMCATOD__SET_ADC0_SAMPLE_CHN(0, 12) // Current C on ADC0
#define SRMCATOD_DCBUS_ADC_CONF_DEF()
#elif(WINDY_INTERNATION_BOARD == 1)
#define SRMCATOD_I1_DATA_REG()              SRMCATOD__ADC1_RA()         // Recommended to read currents first to make reconstruction
#define SRMCATOD_I2_DATA_REG()              SRMCATOD__ADC0_RB()
#define SRMCATOD_I3_DATA_REG()              SRMCATOD__ADC0_RA()         // Recommended to read currents first to make reconstruction
#define SRMCATOD_HVDC_BUS_DATA_REG()        SRMCATOD__ADC1_RB()
#endif


#define SRMCATOD_EOC_ISR_HANDLER()          SRMotorSafetyMgr__MCPwmHandler()

#endif // SRMCATOD_PRM_H_

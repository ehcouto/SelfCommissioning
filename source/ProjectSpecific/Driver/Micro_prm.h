/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      Contains some configurable part of the code for SRMicro GDM
 *
 *  $Header: SRMicro_prm.h 1.2 2015/08/21 13:52:08EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/08/21 13:52:08EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef MICRO_PRM_H
    #define MICRO_PRM_H
#include "C_Extensions.h"
#include "uc.h"
#include "SRMC_defs.h"


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Defines the Enabling or disabling of the Safety relevent test features
 *    @details-
 *
 *    Below are the tests to be performed during boot and run-time -
 *    CPU Register test,
 *    Stack Test
 *    RAM Test
 *    Flash Test
 *    Watchdog Test
 *
 *    @param  - ENABLED(1)    - Which Enables the ClassB Test. ClassB software shall enable this macro
 *              DISABLED(0)   – Which Disables the ClassB Test. ClassA software can disables this macro
 *
 *    @note -   "#define MICRO_SR_FEATURE_TEST                   ENABLED"
 *              "#define MICRO_SR_FEATURE_TEST                   DISABLED"
 */
#define MICRO_SR_FEATURE_TEST                               ENABLED

//=====================================================================================================================
/**
 *  @brief  -  Defines whether to use the External/Internal Oscillator as Master Clock
 *
 *  @param  -  ENABLED   – Uses the HSE as the Master clock source
 *             DISABLED  – Uses the HSI as the Master clock source
 *
 *  @example -  #define MICRO_EXTERNAL_OSCILATOR    ENABLED  //Uses HSE as Master Clock source
 *              #define MICRO_EXTERNAL_OSCILATOR    DISABLED //Uses HSI as Master Clock source
 *  @note - For PLL mode i.e PBE or PEE mode External Crystal is required.
 */
//=====================================================================================================================
#define MICRO_EXTERNAL_OSCILATOR                            ENABLED

//=====================================================================================================================
/**
 *  @brief  -  Following Macro Defines micro is using FLL or PLL clock as Master Clock
 *
 *  @param  - ENABLED   – Uses the FLL as clock
 *            DISABLED  – Uses the PLL as a clock
 *
 *  @example -  #define MICRO_CLOCK_MODE_USED_FLL    ENABLED  //Uses FLL as Master Clock source
 *              #define MICRO_CLOCK_MODE_USED_FLL    DISABLED //Uses PLL as Master Clock source
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define MICRO_CLOCK_MODE_USED_FLL							ENABLED
#elif(WINDY_INTERNATION_BOARD == 1)
#define MICRO_CLOCK_MODE_USED_FLL							DISABLED
#else
#error "Need to define a board"
#endif

//=====================================================================================================================
/**
 *  @brief  -  Following Macro Defines High Gain Oscillator Select
 *
 *  @param  - 0 - configure crystal oscillator low power operation.
 *            1 - configure crystal oscillator high-gain operation.
 *
 *  @example -  #define MICRO_OSC_HGO    0 //low power operation
 *              #define MICRO_OSC_HGO    1 //high-gain operation
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define MICRO_OSC_HGO										1
#elif(WINDY_INTERNATION_BOARD == 1)
#define MICRO_OSC_HGO										0
#endif

//=====================================================================================================================
/**
 *  @brief  -  Following Macro configures the oscillator load.
 *
 *  @param  -   For MICRO_OSC_SC2P - Oscillator 2 pF Capacitor Load Configure
 *  			0 Disable the selection.
 *				1 Add 2 pF capacitor to the oscillator load.
 *
 *				For MICRO_OSC_SC4P - Oscillator 4 pF Capacitor Load Configure
 *  			0 Disable the selection.
 *				1 Add 4 pF capacitor to the oscillator load.
 *
 *				For MICRO_OSC_SC8P - Oscillator 8 pF Capacitor Load Configure
 *  			0 Disable the selection.
 *				1 Add 4 pF capacitor to the oscillator load.
 *
 *				For MICRO_OSC_SC16P - Oscillator 16 pF Capacitor Load Configure
 *  			0 Disable the selection.
 *				1 Add 16 pF capacitor to the oscillator load.
 *
 *  @example -  #define MICRO_OSC_SC2P    		0 //Disable the selection.
 *              #define MICRO_OSC_SC2P    		1 //Add 2 pF capacitor to the oscillator load.
 */
//=====================================================================================================================


#if(WINDY_STRIP_BOARD == 1)
#define MICRO_OSC_SC2P					0
#define MICRO_OSC_SC4P					0
#define MICRO_OSC_SC8P					1
#define MICRO_OSC_SC16P					1
#elif(WINDY_INTERNATION_BOARD == 1)
#define MICRO_OSC_SC2P					0 
#define MICRO_OSC_SC4P					0
#define MICRO_OSC_SC8P					0 
#define MICRO_OSC_SC16P					0
#endif
//=====================================================================================================================
/**
 *    @brief  - Defines the HSE(High Speed External) Clock frequency, which ranges from 32Khz to 40Khz and 3MHz to 32MHz
 *    @details-1. The configuration value for the HSE clock frequency should be within this range.
 *    			  Crystal Oscillator in two ranges
 *                For  - 32Khz to 40Khz
 *                For  - 3Mhz to 32Mhz
 *
 *             2. Micro in FEE, FBE, PEE or PBE mode requires an external crystal oscillator.
 *                FEE - FLL engaged external refernce clock,    FBE - FLL bypassed external refernce clock
 *                PEE - PLL engaged external refernce clock,    PBE - PLL bypassed external refernce clock
 *
 *             3. This macro must be defined even if the system uses an internal oscillator as system clock
 *
 *    @param   -3000000 -to- 32000000
 *    @note    -"#define HSE_VALUE   8000000"    // If the External Clock is of 8MHz
 *              "#define HSE_VALUE   32000000"   // If the External Clock is of 32MHz
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define HSE_VALUE                 							32768
#elif(WINDY_INTERNATION_BOARD == 1)
#define HSE_VALUE                 							8000000
#endif
//=====================================================================================================================
/**
 *    @brief  - Defines the Micro High Speed Run Mode.
 *    @details- The following macro switches Micro from Run mode to High Speed Run Mode
 *    			The MCU can be run at a faster frequency compared with RUN mode and the internal supply is fully regulated.
 *              In this state, the MCU is able to operate at a faster frequency compared to normal RUN mode
 *               -------------------------------------
 *              |For High Speed RUN Mode:             |
 *              |   Core/Syetem clock       - 120Mhz  |
 *              |   Bus clock               - 60Mhz   |
 *              |   FlexBus clock           - 30Mhz   |
 *              |   Flash clock             - 24Mhz   |
 *               --------------------------------------
 *    @note
 *    While in this mode, the following restrictions must be adhered to:
 *      • The maximum allowable change in frequency of the system, bus, flash or core clocks is restricted to x2.
 *      • Before exiting HSRUN mode, clock frequencies should be reduced back down to those acceptable in RUN mode.
 *      • Stop mode entry is not supported from HSRUN.
 *      • Modifications to clock gating control bits are prohibited.
 *      • Flash programming/erasing is not allowed.
 *
 *    @note    -"#define MICRO_HIGHSPEEDRUN_MODE                ENABLED"    // Micro is in High Speed Run State
 *              "#define MICRO_HIGHSPEEDRUN_MODE                DISABLED"   // Micro is in normal Run State
 */
//=====================================================================================================================
#define MICRO_HIGHSPEEDRUN_MODE                                 ENABLED

//=====================================================================================================================
/**
 * @brief  - This macro defines the FLL frequency mode for External reference Oscillator
 *           FEE mode (FLL Engaged External reference - ICS supplies a clock derived from the FLL which is controlled by an external reference clock source )
 *           FBE mode (FLL Bypassed External reference (i.e HSE_VALUE) - ICS supplies a clock derived from the external reference clock source )
 *
 * @details-
 *
 *      -----------------------------------------------------------------------           -----------------------------------------------------------------------
 *     |              For FEE Mode - FLL Engaged External referene             |         |                  For FBE Mode - FLL Bypasse External reference        |
 *     |-----------------------------------------------------------------------|         |-----------------------------------------------------------------------|
 *     | For example: Configures the Micro for the below clock frequencies     |         | For example: Configures the Micro for the below clock frequencies     |
 *     |             72Mhz is the required system/core frequency and           |         |             8Mhz is the required system/core frequency and            |
 *     |             36Mhz bus frequency.                                      |         |             8Mhz bus frequency.                                       |
 *     |             18Mhz Flex bus frequency                                  |         |             4Mhz Flex bus frequency                                   |
 *     |              9Mhz Flash frequency                                     |         |              2Mhz Flash frequency                                     |
 *     |          ** Consider 8Mhz is external reference crystal clock         |         |          ** Consider 8Mhz is external reference crystal clock         |
 *     |                                                                       |         |                                                                       |
 *     | Step0: Enabled following macro to select Exterenal ref crystal clock  |         | Step0: Enabled following macro to select Exterenal ref crystal clock  |
 *     |        #define MICRO_EXTERNAL_OSCILATOR           ENABLED             |         |        #define MICRO_EXTERNAL_OSCILATOR           ENABLED             |
 *     |                                                                       |         |                                                                       |
 *     | Step1: Enabled following macro to use FLL for clock generation        |         | Step1: Enabled following macro to use FLL for clock generation        |
 *     |        #define MICRO_CLOCK_MODE_USED_FLL          ENABLED             |         |        #define MICRO_CLOCK_MODE_USED_FLL          ENABLED             |
 *     |                                                                       |         |                                                                       |
 *     | Step2: Defines the HSE(High Speed External) Clock frequency.          |         | Step2: Defines the HSE(High Speed External) Clock frequency.          |
 *     |        #define HSE_VALUE                 		   8000000             |         |        #define HSE_VALUE                 		  8000000            |
 *     |                                                                       |         |                                                                       |
 *     | Step3: Enabled following macro to configured FEE mode for micro clock |         | Step3: Disabled following macro to configured FBE mode for micro clock|
 *     |        #define ICS_CLK_MODE_FEE                   ENABLED             |         |        #define ICS_CLK_MODE_FEE                   DISABLED            |
 *     |                                                                       |         |                                                                       |
 *     | Step4: Using DCO_RANGE macro select DCO frequency range.              |         | Step4: Using FRDIV macro (FLL External Reference Divider)             |
 *     |        #define DCO_RANGE    2  //Mid high range frequency 72Mhz       |         |        set to 3'b011, or divide-by-256 because 8 MHz / 256 = 31.25Khz |
 *     |                                                                       |         |        set BDIV = 1   (i.e divide by 2)                               |
 *     | Step5: Calculate value for the FLL External Reference Divider         |         |        which is in the 31.25 kHz to 39.0625 kHz range required by FLL |
 *     |        #define FRDIV                             3                    |         |                                                                       |
 *     |        Using FRDIV macro (FLL External Reference Divider)             |         | Step5: SIM_OUTDIV1 macro used to divide value for core/system clock   |
 *     |        set to 3'b011, or divide-by-256 because 8 MHz / 256 = 31.25Khz |         |        coming from MCGOUTCLK                                          |
 *     |        set BDIV = 1   (i.e divide by 2)                               |         |        #define SIM_OUTDIV1      0  (core clock = MCGOUTCLK)           |
 *     |        which is in the 31.25 kHz to 39.0625 kHz range required by FLL |         |                                                                       |
 *     |                                                                       |         | Step6: SIM_OUTDIV2 macro can used to divide value for bus clock       |
 *     | Step6: SIM_OUTDIV1 macro used to divide value for core/system clock   |         |        #define SIM_OUTDIV2      0  (bus clock = MCGOUTCLK)            |
 *     |        coming from MCGOUTCLK                                          |         |                                                                       |
 *     |        #define SIM_OUTDIV1      0  (core clock = MCGOUTCLK)           |         | Step7: SIM_OUTDIV3 macro can used to divide value for Flexbus clock   |
 *     |                                                                       |         |        coming from MCGOUTCLK                                          |
 *     | Step7: SIM_OUTDIV2 macro can used to divide value for bus clock       |         |        #define SIM_OUTDIV3      1  (core clock = MCGOUTCLK)           |
 *     |        #define SIM_OUTDIV2      1  (bus clock = MCGOUTCLK)            |         |                                                                       |
 *     |                                                                       |         | Step8: SIM_OUTDIV4 macro can used to divide value for Flash clock     |
 *     | Step8: SIM_OUTDIV3 macro can used to divide value for Flexbus clock   |         |        coming from MCGOUTCLK                                          |
 *     |        coming from MCGOUTCLK                                          |         |        #define SIM_OUTDIV4      2  (bus clock = MCGOUTCLK)            |
 *     |        #define SIM_OUTDIV3      2  (core clock = MCGOUTCLK)           |         |                                                                       |
 *     |                                                                       |          ------------------------------------------------------------------------
 *     | Step9: SIM_OUTDIV4 macro can used to divide value for Flash clock     |
 *     |        coming from MCGOUTCLK                                          |
 *     |        #define SIM_OUTDIV4      3  (bus clock = MCGOUTCLK)            |
 *     |                                                                       |
 *      ------------------------------------------------------------------------
 *
 *         ---------------------------------------------------------       ---------------------------------------------------------
 *  @note |For RUN Mode:                                            |     |For High Speed RUN Mode:                                 |
 *        |   Core/Syetem clock                           - 80Mhz   |     |   Core/Syetem clock                           - 120Mhz  |
 *        |   Bus clock                                   - 40Mhz   |     |   Bus clock                                   - 60Mhz   |
 *        |   FlexBus clock  should not be greater than   - 20Mhz   |     |   FlexBus clock  should not be greater than   - 30Mhz   |
 *        |   Flash clock should not be greater than      - 26.67Mhz|     |   Flash clock should not be greater than      - 24Mhz   |
 *         ---------------------------------------------------------       ---------------------------------------------------------
 *
 *  @note Oscillator clock, FlexBus and Flash clock can be confirmed by using following macro from this file
 *           #define CLKOUTSEL
 *
 * @param  - ENABLED   – FLL as FEE mode (FLL Engaged External - ICS supplies a clock derived from the FLL which is controlled by an external reference clock source )
 *           DISABLED  – FLL as FBE mode (FLL Bypassed External - ICS supplies a clock derived from the external reference clock source )
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define ICS_CLK_MODE_FEE 				 					ENABLED				//FLL Engaged External
#elif(WINDY_INTERNATION_BOARD == 1)
#define ICS_CLK_MODE_FEE 				 					DISABLED			
#endif

//=====================================================================================================================
/**
 * @brief FLL External Reference Divider
 * @details FRDIV comes in picture only for FEE mode (FLL Engaged External)
 *          i.e if ICS_CLK_MODE_FEE macro is  ENABLED
 *          Default set FRDIV = 0
 *
 * Selects the amount to divide down the external reference clock for the FLL. The resulting frequency must
 * be in the range 31.25 kHz to 39.0625 kHz (This is required when FLL/DCO is the clock source for
 * MCGOUTCLK . In FBE mode, it is not required to meet this range, but it is recommended in the cases
 * when trying to enter a FLL mode from FBE).
 *
 *  -------------------------------------------------------------------------------------------------------------------------
 * |FRDIV |                                       RANGE or OSCSEL                                                            |
 * |------|------------------------------------------------------------------------------------------------------------------|
 * | 000  |  If RANGE = 0 or OSCSEL=1 ,    Divide Factor is 1; 		    for all other RANGE values,  Divide Factor is 32.    |
 * | 001  |  If RANGE = 0 or OSCSEL=1 ,    Divide Factor is 2; 		    for all other RANGE values,  Divide Factor is 64.    |
 * | 010  |  If RANGE = 0 or OSCSEL=1 ,    Divide Factor is 4; 		    for all other RANGE values,  Divide Factor is 128.   |
 * | 011  |  If RANGE = 0 or OSCSEL=1 ,    Divide Factor is 8; 		    for all other RANGE values,  Divide Factor is 256.   |
 * | 100  |  If RANGE = 0 or OSCSEL=1 ,    Divide Factor is 16; 		for all other RANGE values,  Divide Factor is 512.   |
 * | 101  |  If RANGE = 0 or OSCSEL=1 ,    Divide Factor is 32; 		for all other RANGE values,  Divide Factor is 1024.  |
 * | 110  |  If RANGE = 0 or OSCSEL=1 ,    Divide Factor is 64; 		for all other RANGE values,  Divide Factor is 1280.  |
 * | 111  |  If RANGE = 0 or OSCSEL=1 ,    Divide Factor is 128; 		for all other RANGE values,  Divide Factor is 1536.  |
 *  --------------------------------------------------------------------------------------------------------------------------
 */
//=====================================================================================================================
#define FRDIV_DEF												0

//=====================================================================================================================
/**
 * @brief  - This macro defines the PLL frequency mode for External reference Oscillator
 *           PEE mode (PLL Engaged External reference - ICS supplies a clock derived from the FLL which is controlled by an external reference clock source )
 *           PBE mode (PLL Bypassed External reference (i.e HSE_VALUE) - ICS supplies a clock derived from the external reference clock source )
 *
 * @details-
 *
 * Case 0 - PEE Mode: PLL Engaged External reference
 *          PLL used as PEE mode (PLL engaged external) to boost frequency for the selected external reference clock source.
 *          #define ICS_CLK_MODE_PEE    	ENABLED ---------- For MKV03x_FAMILY
 *
 *                 -----------------------------------------------------------------------
 *                |                  For MKV03x_FAMILY                                    |
 *                |-----------------------------------------------------------------------|
 *                | For example: Configures the Micro for the below clock frequencies     |
 *                |             100Mhz is the required system/core frequency and          |
 *                |             50Mhz bus frequency.                                      |
 *                |             12Mhz Flex bus frequency                                  |
 *                |              25Mhz Flash frequency                                    |
 *                |          ** Consider 8Mhz is external reference crystal clock         |
 *                |                                                                       |
 *                | Step0: Enabled following macro to select Exterenal ref crystal clock  |
 *                |        #define MICRO_EXTERNAL_OSCILATOR           ENABLED             |
 *                |                                                                       |
 *                | Step1: Enabled following macro to use FLL for clock generation        |
 *                |        #define MICRO_CLOCK_MODE_USED_FLL          DISABLED            |
 *                |                                                                       |
 *                | Step2: Disabled following macro to configured FEE mode for micro clock|
 *                |        #define ICS_CLK_MODE_FEE                   ENABLED             |
 *                |                                                                       |
 *                | Step2: Disabled following macro to used external crystal as           |
 *                |        #define ICS_CLK_MODE_PEE                   ENABLED             |
 *                |                                                                       |
 *                | Step3:Assigned value for the required PLL frequency to following macro|
 *                |        #define MICRO_HSE_VALUE_PLL_CLK           100000000            |
 *                |                                                                       |
 *                | Step4: Calculate value for the PLL External Reference Divider         |
 *                |        #define PRDIV                             3                    |
 *                |        Divide down the external reference clock for the PLL.          |
 *                |        The resulting frequency must be in the range of 2 MHz to 4 MHz.|
 *                |                                                                       |
 *                |        In this case 8Mhz is external refernce crystal clock           |
 *                |        frequency = [(External reference clock for the PLL) /(PRDIV)]  |
 *                |        frequency of 8MHz/4 = 2 MHz.                                   |
 *                |                                                                       |
 *                | Step5: Calculate value for VCO 0 Multipler                            |
 *                |        #define VDIV0                             26                   |
 *                |        Now, with PRDIV of divide-by-2, and C6[VDIV] of multiply-by-24,|
 *                |        MCGOUTCLK = [(Crystalclock/PRDIV0) * VDIV0] = 100Mhz           |
 *                |        MCGOUTCLK = [(8 MHz / 2) * 50] = 100 MHz.                      |
 *                |        C6[VDIV0] set to '26', i.e 50                                  |
 *                |                                                                       |
 *                | Step6: SIM_OUTDIV1 macro used to divide value for core/system clock   |
 *                |        coming from MCGOUTCLK                                          |
 *                |        #define SIM_OUTDIV1      0  (core clock = MCGOUTCLK)           |
 *                |                                                                       |
 *                | Step7: SIM_OUTDIV2 macro can used to divide value for bus clock       |
 *                |        #define SIM_OUTDIV2      1  (bus clock = MCGOUTCLK)            |
 *                |                                                                       |
 *                | Step8: SIM_OUTDIV3 macro can used to divide value for Flexbus clock   |
 *                |        coming from MCGOUTCLK                                          |
 *                |        #define SIM_OUTDIV3      4  (core clock = MCGOUTCLK)           |
 *                |                                                                       |
 *                | Step9: SIM_OUTDIV4 macro can used to divide value for Flash clock     |
 *                |        coming from MCGOUTCLK                                          |
 *                |        #define SIM_OUTDIV4      3  (bus clock = MCGOUTCLK)            |
 *                |                                                                       |
 *                | @note FlexBus clock frequency should not be greater than 20Mhz        |
 *                | @note Flash clock frequency should not be greater than 26.67Mhz       |
 *                |                                                                       |
 *                | @note Oscillator clock, FlexBus and Flash clock                       |
 *                |       can be confirmed by using following macro from this file        |
 *                |       #define CLKOUTSEL                                               |
 *                 ------------------------------------------------------------------------
 *
 * Case 1 - ICS_CLK_MODE_PEE    	DISABLED  ---------- For MKV03x_FAMILY
 *
 *           PLL used as PBE mode (PLL bypassed external) for the selected external reference clock source.
 *
 * @note If using an external clock source less than 2 MHz, the MCG must not be configured for any of the PLL modes (PEE and PBE).
 */
//=====================================================================================================================
#define ICS_CLK_MODE_PEE									ENABLED				//PLL Engaged External

//=====================================================================================================================
/**
 *    @brief  - Defines the HSE PLL(High Speed External) Clock frequency with PLL bypassed, which ranges from 48MHz to 120MHz
 *    @details-1. The configuration value for the HSE PLL clock frequency should be within this range.
 *
 *             2. ICS_CLK_MODE_PEE macro is ENABLED
 *                PLL used as PEE mode (PLL engaged external) to boost frequency for the selected external reference clock source.
 *
 *             3. ICS_CLK_MODE_FEE macro is  DISABLED
 *                PLL used as PBE mode (PLL bypassed external) for the selected external reference clock source.
 *
 *             4. Micro in PEE or PBE mode requires an external crystal oscillator.
 *
 *             5. This macro must be defined even if the system uses an internal oscillator as system clock
 *
 *    @param   -48000000 -to- 120000000
 *    @note    -"#define MICRO_HSE_VALUE_PLL_CLK   48000000"    // If the External Clock is of 48MHz
 *              "#define MICRO_HSE_VALUE_PLL_CLK   120000000"   // If the External Clock is of 120MHz
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define MICRO_HSE_VALUE_PLL_CLK                 							100000000
#elif(WINDY_INTERNATION_BOARD == 1)
#define MICRO_HSE_VALUE_PLL_CLK                 							120000000
#endif

//=====================================================================================================================
/**
 * @brief PLL External Reference Divider
 * @details Configure C5[PRDIV0] to generate correct PLL reference frequency.
 *          PRDIV comes in picture only for PEE mode and PBE mode (PLL Engaged External)
 *          Default set PRDIV = 0
 *          For example 8Mhz is external refernce crystal clock
 *          C5[PRDIV] set to C5'b00001, or divide-by-2 resulting in a pll reference
 *          frequency of 8MHz/4 = 2 MHz.
 *
 * Selects the amount to divide down the external reference clock for the PLL. The resulting frequency must
 * be in the range of 2 MHz to 4 MHz. After the PLL is enabled (by setting either PLLCLKEN 0 or PLLS), the
 * PRDIV 0 value must not be changed when LOCK0 is zero.
 *  --------------------------   --------------------------   --------------------------   --------------------------
 * | PRDIV0  | Divide Factor  | | PRDIV0  | Divide Factor  | | PRDIV0  | Divide Factor  | | PRDIV0  | Divide Factor  |
 * |---------|----------------| |---------|----------------| |---------|----------------| |---------|----------------|
 * | 00000   |       1        | | 01000   |       9        | | 10000   |       17       | | 11000   |       25       |
 * | 00001   |       2        | | 01001   |       10       | | 10001   |       18       | | 11001   |       26       |
 * | 00010   |       3        | | 01010   |       11       | | 10010   |       19       | | 11010   |       27       |
 * | 00011   |       4        | | 01011   |       12       | | 10011   |       20       | | 11011   |       28       |
 * | 00100   |       5        | | 01100   |       13       | | 10100   |       21       | | 11100   |       29       |
 * | 00101   |       6        | | 01101   |       14       | | 10101   |       22       | | 11101   |       30       |
 * | 00110   |       7        | | 01110   |       15       | | 10110   |       23       | | 11110   |       31       |
 * | 00111   |       8        | | 01111   |       16       | | 10111   |       24       | | 11111   |       32       |
 *  ---------------------------  ---------------------------  ---------------------------  ---------------------------
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define PRDIV												3
#elif(WINDY_INTERNATION_BOARD == 1)
#define PRDIV												1
#endif
//=====================================================================================================================
/**
 * @brief - Macro defines VCO 0 Divider
 * @details Configure C6[VDIV0] to generate correct PLL reference frequency.
 *          PRDIV comes in picture only for PEE mode and PBE mode (PLL Engaged External)
 *          Default set VDIV0 = 0
 *          For example 8Mhz is external refernce crystal clock and to generate MCGOUTCLK frequency = 80Mhz
 *
 *          Now, with PRDIV of divide-by-2, and C6[VDIV] of multiply-by-24,
 *          MCGOURCLK = [(Crystalclocl/PRDIV0) * VDIV0] = 80Mhz
 *          MCGOUTCLK = [(8 MHz / 2) * 40] = 80 MHz.
 *          C6[VDIV0] set to C6'b10000, i.e 40
 *
 * Selects the amount to divide the VCO output of the PLL. The VDIV 0 bits establish the multiplication factor
 * (M) applied to the reference clock frequency. After the PLL is enabled (by setting either PLLCLKEN 0 or
 * PLLS), the VDIV 0 value must not be changed when LOCK 0 is zero.
 *  --------------------------   --------------------------   --------------------------   --------------------------
 * | VDIV0   |Multiply Factor | | VDIV0   |Multiply Factor | | VDIV0   |Multiply Factor | | VDIV0   |Multiply Factor |
 * |---------|----------------| |---------|----------------| |---------|----------------| |---------|----------------|
 * |    0    |       24       | |   8     |       32       | |   16    |       40       | |   24    |       48       |
 * |    1    |       25       | |   9     |       33       | |   17    |       41       | |   25    |       49       |
 * |    2    |       26       | |   10    |       34       | |   18    |       42       | |   26    |       50       |
 * |    3    |       27       | |   11    |       35       | |   19    |       43       | |   27    |       51       |
 * |    4    |       28       | |   12    |       36       | |   20    |       44       | |   28    |       52       |
 * |    5    |       29       | |   13    |       37       | |   21    |       45       | |   29    |       53       |
 * |    6    |       30       | |   14    |       38       | |   22    |       46       | |   30    |       54       |
 * |    7    |       31       | |   15    |       39       | |   23    |       47       | |   31    |       55       |
 *  --------------------------   --------------------------   --------------------------   ---------------------------
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define VDIV0_DEF												26
#elif(WINDY_INTERNATION_BOARD == 1)
#define VDIV0_DEF												6
#endif
//=====================================================================================================================
/**
 *  @brief  - Defines the mode of FLL frequency for Internal reference Oscillator
 *  @details
 *
 *    -----------------------------------------------------------------------        ---------------------------------------------------------------------
 *   |                 For FEI Mode Frequency Engaged Internal               |      |                For FBI Frequency Bypass Internal                    |
 *   |-----------------------------------------------------------------------|      |---------------------------------------------------------------------|
 *   | For example: Configures the Micro for the below clock frequencies     |      | For example:                                                        |
 *   |             72Mhz is the required system/core frequency and           |      |             4Mhz is Internal reference clock                        |
 *   |             72Mhz bus frequency.                                      |      |             4Mhz is the required system/core frequency and          |
 *   |             12Mhz Flex bus                                            |      |             2Mhz bus frequency.                                     |
 *   |             16Mhz Flash Frequency                                     |      |             1Mhz Flash Frequency                                    |
 *   |          ** Consider 8Mhz is external reference crystal clock         |      |                                                                     |
 *   |                                                                       |      |                                                                     |
 *   | Step0: Enabled following macro to select Exterenal ref crystal clock  |      | Step0: Enabled following macro                                      |
 *   |        #define MICRO_EXTERNAL_OSCILATOR           DISABLED            |      |        #define MICRO_EXTERNAL_OSCILATOR            DISABLED         |
 *   |                                                                       |      |                                                                     |
 *   | Step2: Enabled following macro to used external crystal as            |      | Step1: Disabled following macro to used external crystal as         |
 *   |        #define ICS_CLK_MODE_FEI                   ENABLED             |      |        #define ICS_CLK_MODE_FEI                   DISABLED          |
 *   |                                                                       |      |                                                                     |
 *   | Step3: Using DCO_RANGE macro select DCO frequency range.              |      | Step3: Select Internal reference clock using IRCS_CLOCK macro       |
 *   |        #define DCO_RANGE        2  //High range frequency 96Mhz       |      |        #define IRCS_CLOCK       0  //Slow internal reference clock  |
 *   |                                                                       |      |        #define IRCS_CLOCK       1  //Fast internal reference clock  |
 *   | Step4: SIM_OUTDIV1 macro used to divide value for core/system clock   |      |                                                                     |
 *   |        coming from MCGOUTCLK                                          |      | Step4: If Fast internal ref clock is selected                       |
 *   |        #define SIM_OUTDIV1      0                                     |      |        Using FCRDIV macro Selects the amount to divide down the fast|
 *   |                                                                       |      |        internal reference clock                                     |
 *   | Step5: SIM_OUTDIV2 macro can used to divide value for bus clock       |      |        #define  FCRDIV          0  //Divide Factor is 1             |
 *   |        #define SIM_OUTDIV2      0                                     |      |                                                                     |
 *   |                                                                       |      | Step4: SIM_OUTDIV1 macro used to divide value for core/system clock |
 *   | Step6: SIM_OUTDIV3 macro can used to divide value for Flexbus clock   |      |        coming from MCGOUTCLK                                        |
 *   |        coming from MCGOUTCLK                                          |      |        #define SIM_OUTDIV1      0                                   |
 *   |        #define SIM_OUTDIV3      6                                     |      |                                                                     |
 *   |                                                                       |      | Step5: SIM_OUTDIV2 macro can used to divide value for bus clock     |
 *   | Step7: SIM_OUTDIV4 macro can used to divide value for Flash clock     |      |        #define SIM_OUTDIV2      1                                   |
 *   |        coming from MCGOUTCLK                                          |      |                                                                     |
 *   |        #define SIM_OUTDIV4      4                                     |      | Step6: SIM_OUTDIV3 macro can used to divide value for Flexbus clock |
 *   |                                                                       |      |        coming from MCGOUTCLK                                        |
 *    ----------------------------------------------------------------------        |        #define SIM_OUTDIV3      1                                   |
 *                                                                                  |                                                                     |
 *                                                                                  | Step7: SIM_OUTDIV4 macro can used to divide value for Flash clock   |
 *                                                                                  |        coming from MCGOUTCLK                                        |
 *                                                                                  |        #define SIM_OUTDIV4      1                                   |
 *                                                                                  |                                                                     |
 *                                                                                   ---------------------------------------------------------------------
 *
 *  @param  - ENABLED   – Uses the FLL as FEI mode (FLL Engaged Internal mode )
 *            DISABLED  – Uses the FLL as FBI mode (FLL Bypassed Internal mode )
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define ICS_CLK_MODE_FEI                					ENABLED
#elif(WINDY_INTERNATION_BOARD == 1)
#define ICS_CLK_MODE_FEI                					DISABLED
#endif
//=====================================================================================================================
/**
 * @brief Macro defines Internal Reference Clock Select
 * @details This macro Selects between the fast or slow internal reference clock source.
 *
 * #define IRCS_CLOCK		0 //Slow internal reference clock selected. (i.e 32Khz)
 * #define IRCS_CLOCK		1 //Fast internal reference clock selected. (i.e 4Mhz)
 */
//=====================================================================================================================
#define IRCS_CLOCK												1

//=====================================================================================================================
/**
 * @brief FLL Fast Clock Internal Reference Divider
 * @details Selects the amount to divide down the fast internal reference clock. The resulting frequency will be in the
 *          range 31.25 kHz to 4 MHz (Note: Changing the divider when the Fast IRC is enabled is not supported).
 * FCRDIV comes in picture only for FEI mode (FLL Engaged Internal when the Fast internal reference clock selected)
 *          i.e if ICS_CLK_MODE_FEI macro is  DISABLED
 *          Default set FCRDIV = 1
 *
 * FCRDIV          RANGE or OSCSEL
 *  000           Divide Factor is 1
 *  001           Divide Factor is 2.
 *  010           Divide Factor is 4.
 *  011           Divide Factor is 8.
 *  100           Divide Factor is 16
 *  101           Divide Factor is 32
 *  110           Divide Factor is 64
 *  111           Divide Factor is 128.
 */
//=====================================================================================================================
#define FCRDIV_CLOCK                                                0

//=====================================================================================================================
/**
 * @brief Defines DCO Range Select when in Internal Clock mode
 *
 *<pre>
 *      
 *      ---------------------------------------------------------
 *      |   DRST_DRS    |   DMX32   |     DCO Range             |
 *      |---------------|-----------|---------------------------|
 *      |     00        |     1     |     24Mhz  low range      |
 *      |---------------|-----------|---------------------------|
 *      |     01        |     1     |     48Mhz  Mid-high       |
 *      |---------------|-----------|---------------------------|
 *      |     02        |     1     |     72Mhz  Mid-high range |
 *      |---------------|-----------|---------------------------|
 *      |     03        |     1     |     96Mhz  High range     |
 *      |-------------------------------------------------------|</pre>
 */
//=====================================================================================================================
#define DCO_RANGE_INT                     1

//=====================================================================================================================
/**
 * @brief Defines DCO Range Select
 * @details- It gives possibility to set IWDG timeout from 1millisecond to 65 seconds with reference to internal 1Khz clock.
 * @param  - Acceptable values: From 0x0000 to 0xFFFF.
 *
 *<pre>
 *      Below table explains about the window watchdog timeout value for the clock source at 1MHz -
 *      ---------------------------------------------------------
 *      |   DRST_DRS    |   DMX32   | 	  DCO Range	     	    |
 *      |---------------|-----------|---------------------------|
 *      |     00        |     1     |     24Mhz  low range      |
 *      |---------------|-----------|---------------------------|
 *      |     01        |     1     |     48Mhz  Mid-high       |
 *      |---------------|-----------|---------------------------|
 *      |     02        |     1     |     72Mhz  Mid-high range |
 *      |---------------|-----------|---------------------------|
 *      |     03        |     1     |     96Mhz  High range     |
 *      |-------------------------------------------------------|</pre>
 */
//=====================================================================================================================
#define DCO_RANGE                        3

//=====================================================================================================================
/**
 * @brief This macro sets the divide value for the Core/System (OUTDIV1)clock from MCGOUTCLK.
 *        SIM_CLDDIV register - Clock 1 Output divider value
 * @details This macro sets the divide value for the core/system clock.
 *          0000 Divide-by-1.
 *          0001 Divide-by-2.
 *          0010 Divide-by-3.
 *          0011 Divide-by-4.
 *          0100 Divide-by-5.
 *          0101 Divide-by-6.
 *          0110 Divide-by-7.
 *          0111 Divide-by-8.
 *          1000 Divide-by-9.
 *          1001 Divide-by-10.
 *          1010 Divide-by-11.
 *          1011 Divide-by-12.
 *          1100 Divide-by-13.
 *          1101 Divide-by-14.
 *          1110 Divide-by-15.
 *          1111 Divide-by-16.
 *
 * @note - Do not set any other value other than above defined values.
 * @note - Carefully configure the SIM_OUTDIV1 and SIM_OUTDIV2 to avoid bus clock frequency high than 24Mhz
 *
 * @note - This Macro #define SIM_OUTDIV1 is used -------- For MKE06_FAMILY/MKE04_FAMILY
 */
//=====================================================================================================================
#define SIM_OUTDIV1                       0//1

//=====================================================================================================================
/**
 * @brief This macro This field sets the divide value for the BUS (OUTDIV2) clock from MCGOUTCLK.
 *
 * @details This macro sets the divide value for the bus, follow MCGOUTCLK.
 *          0000 Divide-by-1.
 *          0001 Divide-by-2.
 *          0010 Divide-by-3.
 *          0011 Divide-by-4.
 *          0100 Divide-by-5.
 *          0101 Divide-by-6.
 *          0110 Divide-by-7.
 *          0111 Divide-by-8.
 *          1000 Divide-by-9.
 *          1001 Divide-by-10.
 *          1010 Divide-by-11.
 *          1011 Divide-by-12.
 *          1100 Divide-by-13.
 *          1101 Divide-by-14.
 *          1110 Divide-by-15.
 *          1111 Divide-by-16.
 */
//=====================================================================================================================
#define SIM_OUTDIV2                       1

//=====================================================================================================================
/**
 * @brief This macro This field sets the divide value for the FlexBus (OUTDIV3) clock (external pin FB_CLK) from MCGOUTCLK.
 *        Clock 2 Output divider value
 * @details This macro sets the divide value for the FlexBus Clock, follow MCGOUTCLK.
 *          0000 Divide-by-1.
 *          0001 Divide-by-2.				//Default value
 *          0010 Divide-by-3.
 *          0011 Divide-by-4.
 *          0100 Divide-by-5.
 *          0101 Divide-by-6.
 *          0110 Divide-by-7.
 *          0111 Divide-by-8.
 *          1000 Divide-by-9.
 *          1001 Divide-by-10.
 *          1010 Divide-by-11.
 *          1011 Divide-by-12.
 *          1100 Divide-by-13.
 *          1101 Divide-by-14.
 *          1110 Divide-by-15.
 *          1111 Divide-by-16.
 *
 * @note - At the end of reset, it is loaded with either 0001 or 1111 depending on FTF_FOPT[LPBOOT].
 * @note - FlexBus clock should not be greater than 20Mhz  in normal mode.
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define SIM_OUTDIV3                       6
#elif(WINDY_INTERNATION_BOARD == 1)
#define SIM_OUTDIV3                       3
#endif
//=====================================================================================================================
/**
 * @brief This macro This field sets the divide value for the FLASH (OUTDIV4) clock from MCGOUTCLK.
 *        Clock 2 Output divider value
 * @details This macro sets the divide value for the Flash Clock, follow MCGOUTCLK.
 *          0000 Divide-by-1.
 *          0001 Divide-by-2.
 *          0010 Divide-by-3.
 *          0011 Divide-by-4.
 *          0100 Divide-by-5.
 *          0101 Divide-by-6.
 *          0110 Divide-by-7.
 *          0111 Divide-by-8.
 *          1000 Divide-by-9.
 *          1001 Divide-by-10.
 *          1010 Divide-by-11.
 *          1011 Divide-by-12.
 *          1100 Divide-by-13.
 *          1101 Divide-by-14.
 *          1110 Divide-by-15.
 *          1111 Divide-by-16.
 *
 * @note - At the end of reset, it is loaded with either 0001 or 1111 depending on FTF_FOPT[LPBOOT].
 * @note - Flash clock should not be greater than 26.67Mhz  in normal mode.
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define SIM_OUTDIV4                      3
#elif(WINDY_INTERNATION_BOARD == 1)
#define SIM_OUTDIV4                      4
#endif
//=====================================================================================================================
/**
 * @brief This setting is used only when the microcontroller is in PROGRAMMING mode, because Flash operation cannot be
 * 		  done at high speed run mode.
 * 		  This macro sets the divide value for the Core/System (OUTDIV1)clock from MCGOUTCLK.
 *        SIM_CLDDIV register - Clock 1 Output divider value
 * @details This macro sets the divide value for the core/system clock.
 *          0000 Divide-by-1.
 *          0001 Divide-by-2.
 *          0010 Divide-by-3.
 *          0011 Divide-by-4.
 *          0100 Divide-by-5.
 *          0101 Divide-by-6.
 *          0110 Divide-by-7.
 *          0111 Divide-by-8.
 *          1000 Divide-by-9.
 *          1001 Divide-by-10.
 *          1010 Divide-by-11.
 *          1011 Divide-by-12.
 *          1100 Divide-by-13.
 *          1101 Divide-by-14.
 *          1110 Divide-by-15.
 *          1111 Divide-by-16.
 *
 * @note - Do not set any other value other than above defined values.
 * @note - Carefully configure the SIM_OUTDIV1 and SIM_OUTDIV2 to avoid bus clock frequency high than 24Mhz
 *
 * @note - This Macro #define SIM_OUTDIV1 is used -------- For MKE06_FAMILY/MKE04_FAMILY
 */
//=====================================================================================================================
#define LOWSPEED_SIM_OUTDIV1                       0

//=====================================================================================================================
/**
 * @brief This setting is used only when the microcontroller is in PROGRAMMING mode, because Flash operation cannot be
 * 		  done at high speed run mode.
 *        This macro This field sets the divide value for the BUS (OUTDIV2) clock from MCGOUTCLK.
 *
 * @details This macro sets the divide value for the bus, follow MCGOUTCLK.
 *          0000 Divide-by-1.
 *          0001 Divide-by-2.
 *          0010 Divide-by-3.
 *          0011 Divide-by-4.
 *          0100 Divide-by-5.
 *          0101 Divide-by-6.
 *          0110 Divide-by-7.
 *          0111 Divide-by-8.
 *          1000 Divide-by-9.
 *          1001 Divide-by-10.
 *          1010 Divide-by-11.
 *          1011 Divide-by-12.
 *          1100 Divide-by-13.
 *          1101 Divide-by-14.
 *          1110 Divide-by-15.
 *          1111 Divide-by-16.
 */
//=====================================================================================================================
#define LOWSPEED_SIM_OUTDIV2                       0

//=====================================================================================================================
/**
 * @brief This setting is used only when the microcontroller is in PROGRAMMING mode, because Flash operation cannot be
 * 		  done at high speed run mode.
 *        This macro This field sets the divide value for the FlexBus (OUTDIV3) clock (external pin FB_CLK) from MCGOUTCLK.
 *        Clock 2 Output divider value
 * @details This macro sets the divide value for the FlexBus Clock, follow MCGOUTCLK.
 *          0000 Divide-by-1.
 *          0001 Divide-by-2.               //Default value
 *          0010 Divide-by-3.
 *          0011 Divide-by-4.
 *          0100 Divide-by-5.
 *          0101 Divide-by-6.
 *          0110 Divide-by-7.
 *          0111 Divide-by-8.
 *          1000 Divide-by-9.
 *          1001 Divide-by-10.
 *          1010 Divide-by-11.
 *          1011 Divide-by-12.
 *          1100 Divide-by-13.
 *          1101 Divide-by-14.
 *          1110 Divide-by-15.
 *          1111 Divide-by-16.
 *
 * @note - At the end of reset, it is loaded with either 0001 or 1111 depending on FTF_FOPT[LPBOOT].
 * @note - FlexBus clock should not be greater than 20Mhz  in normal mode.
 */
//=====================================================================================================================
#define LOWSPEED_SIM_OUTDIV3                       6

//=====================================================================================================================
/**
 * @brief This setting is used only when the microcontroller is in PROGRAMMING mode, because Flash operation cannot be
 * 		  done at high speed run mode.
 *        This macro This field sets the divide value for the FLASH (OUTDIV4) clock from MCGOUTCLK.
 *        Clock 2 Output divider value
 * @details This macro sets the divide value for the Flash Clock, follow MCGOUTCLK.
 *          0000 Divide-by-1.
 *          0001 Divide-by-2.
 *          0010 Divide-by-3.
 *          0011 Divide-by-4.
 *          0100 Divide-by-5.
 *          0101 Divide-by-6.
 *          0110 Divide-by-7.
 *          0111 Divide-by-8.
 *          1000 Divide-by-9.
 *          1001 Divide-by-10.
 *          1010 Divide-by-11.
 *          1011 Divide-by-12.
 *          1100 Divide-by-13.
 *          1101 Divide-by-14.
 *          1110 Divide-by-15.
 *          1111 Divide-by-16.
 *
 * @note - At the end of reset, it is loaded with either 0001 or 1111 depending on FTF_FOPT[LPBOOT].
 * @note - Flash clock should not be greater than 26.67Mhz  in normal mode.
 */
//=====================================================================================================================
#define LOWSPEED_SIM_OUTDIV4                      3

//=====================================================================================================================
/**
 * @brief Macro Clock out frequency this macro output the selected clock frequncy on the PortC Pin 3.
 * @details To check clock frequency enable following CLOCK_OUT macro and check slected clock frequency on PTC3 pin.
 * @note    -"#define CLOCK_OUT          ENABLED"  //clock o/p frequency enable  on PTC3
 *           "#define CLOCK_OUT          DISABLED" //clock o/p frequency disable on PTC3
 */
//=====================================================================================================================
#define CLOCK_OUT                        DISABLED

//=====================================================================================================================
/**
 * @brief Macro define to selects the clock to output on the CLKOUT pin.
 * @details To check clock frequency assigned the following value to check selected clock frequency on PORTC Pin3.
 * @note    -"#define CLKOUTSEL          2"  //Flash clock o/p frequency enable  on PTC3
 *           "#define CLKOUTSEL          6"  //Oscillator clock o/p frequency enable on PTC3
 *
 *           0 FlexBus CLKOUT
 *           1 Reserved
 *           2 Flash Clock
 *           3 LPO clock (1Khz)
 *           4 MCGIRCLK
 *           5 Reserved
 *           6 OSCERCLK0
 *           7 IRC 48Mhz Clock
 */
//=====================================================================================================================
#define CLKOUTSEL         2

//=====================================================================================================================
/**
 * @brief  -This Macro define OSC CLock divider clock
 * @details - ERCLK prescaler. These two bits are used to divide the ERCLK output. The un-divided ERCLK output is not affected by these two bits.
 *
 * @note    -"#define MICRO_ERPS_OSCERCLK          0"  //The divisor ratio is 1.
 *           "#define MICRO_ERPS_OSCERCLK          1"  //The divisor ratio is 2.
 *           "#define MICRO_ERPS_OSCERCLK          2"  //The divisor ratio is 4.
 *           "#define MICRO_ERPS_OSCERCLK          3"  //The divisor ratio is 8.
 *
 * @param  - 0,1,2 or 3
 *
 */
//=====================================================================================================================
#define MICRO_ERPS_OSCERCLK           0

//=====================================================================================================================
/**
 * @brief  -This define enables or disables the debug mode.
 * @details -
 * @note    -"#define DEBUG_MODE          ENABLED"  //Debug mode ON  - Watchdog enabled in debug mode
 *           "#define DEBUG_MODE          DISABLED" //Debug mode OFF - Watchdog disabled in debug mode
 */
//=====================================================================================================================
#define DEBUG_MODE  		             DISABLED

//=====================================================================================================================
/**
 *  @brief  -  Defines the Enabling or disabling of the Independent Watchdog.
 *  @details-  This macro should be Enabled for the production code, can be disabled only for debugging purpose
 *             The Independent watchdog is used in watchdog test in CLASSB software.
 *
 *             Note -The Independent watchdog feature should be enabled if the SRMICRO_WATCHDOG_BOOT_TEST is enabled
 *
 *  @param  - ENABLED(1)    - Which Enables the Independent watchdog
 *            DISABLED(0)   – Which Disables the Independent watchdog
 *
 *  @note    -  "#define MICRO_IWDG_FEATURE    ENABLED" // Enables the watchdog
 *              "#define MICRO_IWDG_FEATURE    DISABLED"// Disables the watchdog
 */
//=====================================================================================================================
#define MICRO_WATCHDOG_FEATURE              ENABLED

//=====================================================================================================================
/**
 * @brief   -Defines the programmable prescaler divider feeding the counter clock.
 * @details - This write once bit enables a fixed 256 pre-scaling of watchdog counter reference clock
 *
 * @param  - 1: 256 prescalar enabled.
 *           0: 256 prescalar disabled.
 */
//=====================================================================================================================
#define WDG_PRESCALER					0

//=====================================================================================================================
/**
 * @brief Defines Watchdog reload value.
 * @details- It gives possibility to set IWDG timeout from 1millisecond to 65 seconds with reference to internal 1Khz clock.
 * @param  - Acceptable values: From 0x0000 to 0xFFFF.
 *
 * @note   - For example - Set watchdog timeout for 1 second.
 *           by deafult 1Khz clock source is set for watchdog timeout.
 *           T = 1/f = 1/1000
 *             = 1 second
 *           Load this "1000" count in the IWDG_RELAOD_VALUE = 1000 (0x03E8 in hex)
 *<pre>
 *      Below table explains about the window watchdog timeout value for the clock source at 1MHz -
 *      ---------------------------------------------------------
 *      |   Watchdog    | Prescaler | 					        |
 *      |    Timeout    | divider   |    IWDG_RELAOD_VALUE      |
 *      |---------------|-----------|---------------------------|
 *      |     50ms      |     0     |            50             |
 *      |---------------|-----------|---------------------------|
 *      |    100ms      |     0     |            100            |
 *      |---------------|-----------|---------------------------|
 *      |    200ms      |     0     |            200            |
 *      |---------------|-----------|---------------------------|
 *      |    300ms      |     0     |            300            |
 *      |-------------------------------------------------------|
 *      |    1Sec       |     0     |            1000           |
 *        -------------------------------------------------------</pre>
 */
//=====================================================================================================================
#define IWDG_RELOAD_VALUE					500 // 500 ms @ 1 KHz

//=====================================================================================================================
/**
 *  @brief  - This macro enables or disables the low Voltage monitor feature.
 *  @details- Its interrupt driven
 *            If LOW_VOLTAGE_MONITOR_FEATURE is enabled then user should-
 *            assign the user application function which will be called when the low voltage is detected as follows
 *            in the SRInterrupt_prm.h file-
 *            #define PVD_IRQ()                       APplLvdService()
 *
 *            The moment the low voltage is detected the APplLvdService()(application function) is called and
 *            here the application can decide what action to be taken for the low voltage
 *
 *  @param  - ENABLED(1)   – Enables the low voltage monitor feature
 *            DISABLED(0)  – Disables the low voltage monitor feature
 *
 *  @note    - This enables the low voltage monitor feature
 *             "#define LOW_VOLTAGE_MONITOR_FEATURE     ENABLED"
 *
 *             This disables the low voltage monitor feature
 *             "#define LOW_VOLTAGE_MONITOR_FEATURE     DISABLED"
 */
//=====================================================================================================================
#define LOW_VOLTAGE_MONITOR_FEATURE        ENABLED

//=====================================================================================================================
/**
 * @brief   - Defines the Supply voltage of Micro controller.
 * @details - This macro defines the actual supply voltage to determine voltage threshold detected by the power voltage detector.
 * @param   - 0/1
 * @note    - "#define LOW_VOLTAGE_SUPPLY         0" //Supply voltage is 3.3V
 *            "#define LOW_VOLTAGE_SUPPLY         1" //Supply voltage is 5.0V
 */
//=====================================================================================================================
#define LOW_VOLTAGE_SUPPLY		         0

//=====================================================================================================================
/**
 *  @brief    This macro defines the Low-Voltage Detect
 *            i.e Selects the LVD trip point voltage (V LVD ).
 *  @param  - 0/1
 *
 *  @note    - #define LVD_LEVEL    00  - Low trip point selected  (V LVD = V LVDL ) [1.60V typically refer Datasheet]
 *             #define LVD_LEVEL    01  - High trip point selected (V LVD = V LVDH ) [2.56V typically refer Datasheet]
 *
 *</pre>
 */
//=====================================================================================================================
#define LVD_LEVEL				1

//=====================================================================================================================
/**
 *  @brief    This macro defines Low-Voltage Warning Voltage Select
 *            i.e Selects the low-voltage warning (LVW) trip point voltage.
 *
 *  @param  - 0/1/2/3
 *<pre>
 *      For Low Range (LVDV = 0) : Falling low-voltage detect threshold.
 *      if Low trip point selected
 *      #define LVD_LEVEL				0
 *      Below table explains Low-voltage warning thresholds - low range
 *       ---------------------------------------------------------------------------------------
 *      |LVW Trip Point Voltage| Voltage level(typ)|                                            |
 *      |----------------------|-------------------|--------------------------------------------|
 *      |          0           |        1.80V      | Low trip point selected (VLVW = VLVW1L)    |
 *      |----------------------|-------------------|--------------------------------------------|
 *      |          1           |        1.90V      | Mid 1 trip point selected (VLVW = VLVW2L)  |
 *      |----------------------|-------------------|--------------------------------------------|
 *      |          2           |        2.00V      | Mid 2 trip point selected (VLVW = VLVW3L)  |
 *      |----------------------|-------------------|--------------------------------------------|
 *      |          3           |        2.10V      | High trip point selected (VLVW = VLVW4L)   |
 *       ------------------------------------------------------------------------------------ </pre>
 *
 *
 *      For High Range (LVDV = 1) : Falling low-voltage detect threshold.
 *      if Low trip point selected
 *      #define LVD_LEVEL				1
 *      Below table explains Low-voltage warning thresholds - High range
 *       ---------------------------------------------------------------------------------------
 *      |LVW Trip Point Voltage| Volage level (typ)|                                            |
 *      |----------------------|-------------------|--------------------------------------------|
 *      |          0           |       2.70V       | Low trip point selected (VLVW = VLVW1L)    |
 *      |----------------------|-------------------|--------------------------------------------|
 *      |          1           |       2.80V       | Mid 1 trip point selected (VLVW = VLVW2L)  |
 *      |----------------------|-------------------|--------------------------------------------|
 *      |          2           |       2.90V       | Mid 2 trip point selected (VLVW = VLVW3L)  |
 *      |----------------------|-------------------|--------------------------------------------|
 *      |          3           |       3.00V       | High trip point selected (VLVW = VLVW4L)   |
 *       ---------------------------------------------------------------------------------------- </pre>
 *
 *</pre>
 */
//=====================================================================================================================
#define LVW_LEVEL				1

//=====================================================================================================================
/**
 *  @brief  - This macro Selects the low Voltage detect action
 *  @details- > This macro defines that, what action need to taken for the Low Voltage
 *            > If LOW_VOLTAGE_DETECT_ACTION is ENABLED then LVD system Request a hardware interrupt when LVWF = 1.
 *              If LOW_VOLTAGE_DETECT_ACTION is DISABLED then LVD system Hardware interrupt is disabled (use polling).
 *              Poll this Warning flag and the mome
 *
 *             The moment the low voltage Warning flag is detected
 *             here the application can decide what action to be taken for the low voltage
 *
 *            if(PMC.LVDSC1.BIT.LVDF == 1 )
 *	             {
 *		            Micro__ClearLvdIsrRegisters();
 *		            ""
 * 		            ""
 *	             }
 *
 *  @param  - ENABLED(1)   – Action on LVD --> RESET
 *            DISABLED(0)  – Action on LVD --> POLLING
 *
 *  @note    -"#define LOW_VOLTAGE_DETECT_ACTION     ENABLED"
 *            "#define LOW_VOLTAGE_DETECT_ACTION     DISABLED"
 *            LVDRE - Low voltage detect reset enable - This filed can b written only one time after resets
 *            additional writes are ignored
 */
//====================================================================================================================
#define        LOW_VOLTAGE_DETECT_ACTION            DISABLED

//=====================================================================================================================
/**
 *  @brief  - This macro Selects the low Voltage warning detection detect action Low-voltage warning event detected
 *  @details- > This macro defines that, what action need to taken for the Low Voltage warning
 *            > If LOW_VOLTAGE_WARNING_ACTION is ENABLED then LVD system Request a hardware interrupt when LVWF = 1.
 *              If LOW_VOLTAGE_WARNING_ACTION is DISABLED then LVD system Hardware interrupt is disabled (use polling).
 *              Poll this Warning flag.
 *
 *             The moment the low voltage Warning flag is detected
 *             here the application can decide what action to be taken for the low voltage
 *
 *            if(PMC.LVDSC2.BIT.LVWF == 1 )
 *	             {
 *		            Micro__ClearLvdIsrRegisters();
 *		            ""
 * 		            ""
 *	             }
 *
 *  @param  - ENABLED(1)   – Action on LVD --> RESET
 *            DISABLED(0)  – Action on LVD --> POLLING
 *
 *  @note    -"#define LOW_VOLTAGE_WARNING_ACTION     ENABLED"
 *            "#define LOW_VOLTAGE_WARNING_ACTION     DISABLED"
 *            LVDRE - Low voltage detect reset enable - This filed can b written only one time after resets
 *            additional writes are ignored
 */
//====================================================================================================================
#define        LOW_VOLTAGE_WARNING_ACTION            DISABLED

//=====================================================================================================================
/**
 *    @brief   -Defines the clock speed when the Micro__SetLowSpeedMode() API is called.
 *    @details -In this case if the external clock is ON it’s turned OFF and the internal oscillator is used as
 *              the system clock.
 *              The internal oscillator clock is of 8MHz.
 *              Adjust the ICS_TRIM_VALUE    to set long Timerinterrupt of 50ms when the crystal is running at low frequency
 *              check this Timerinterrupt on C.R.O by toggling a pin
 *    @param  - 4
 *
 *
 *    @example -  "#define LOW_SPEED_CLOCK       4"  //2.5Mhz ~ 3.125Mhz - Divides selected clock by 16
 */
//=====================================================================================================================
#define LOW_SPEED_CLOCK       4

//=====================================================================================================================
/**
 *  @details : This macro gives configuration if user want to use LOW SPEED FEATURE or not
 *
 *  @note : #define  SRMICRO_STOP_MODE           ENABLED      == Peripheral will be disconnected from the clock Bus to save Power.
 *          #define  SRMICRO_STOP_MODE           DISABLED     == Peripheral will be function with Clock Bus connected to it with normal frequency
 */
//=====================================================================================================================
#define MICRO_STOP_MODE									DISABLED

//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  -  This enables the SRPowerMgr module features
 *    @details -
 *
 *  @param  - ENABLED(1)   – Enables the SRPowerMgr initialization
 *            DISABLED(0)  – Disables the SRPowerMgr features
 *
 */
#define MICRO_SRPOWERMGR_FEATURE						DISABLED


//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  -  This enables the Low Power Timer peripheral
 *    @details
 *
 *  @param  - ENABLED(1)   – Enables the LPTMR peripheral
 *            DISABLED(0)  – Disables the LPTMR peripheral
 */
#define MICRO_LPTIMER_EXTCLOCKSOURCE                    ENABLED

//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  -  Select the value of the preemption priority for the LPTMR interrupt
 *    @details
 *
 *  @param  - 1
 *
 */
#define LPTIMER0_PREEMPTION_PRIORITY                    2

//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  -  Select the value of the preemption sub-priority for the LPTMR interrupt
 *    @details
 *
 *  @param  - 0
 *
 */
#define LPTIMER0_SUB_PRIORITY                           0

//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  -  The value of the LPTMR compare threshold is selected here
 *    @details
 *
 *  @param  - 200
 *
 */
#define MICRO_LPTIMER_COMPARE_VAL                       200






#endif

/**
 *  @file
 *
 *  @defgroup   CLASS_B
 *
 *  @brief      This module implements the standard API (Application Programming Interface) for
 *              Freescale Kintes Motor control series MKV03x Micro module.
 *
 *  @details    This module provides the interface to the application to configure the controller and to perform the
 *              CLASSB safety relevant controller tests.
 *
 *              Configuring the Controller such as-
 *              Initializing microcontroller bus clock, configuring clock for microcontroller,
 *              Configures the controller to work either in slow, normal or sleep mode of operation,
 *              servicing Window and Independent watchdog,
 *              enabling/disabling or store/restoring of the interrupts,
 *              Providing FLASH size ,RAM size and ID of microcontroller,
 *              provision to detect Low voltage.
 *
 *              CLASS B safety-relevant controller test during boot as well as run time such as -
 *              CPU Register Test,
 *              Watchdog test,
 *              RAM test,
 *              Flash test
 *              stack test
 *
 *  @section    Applicable_Documents
 *              Requirement ID - 50717 - https://rm.whirlpool.oncloudone.com/rm/web#action=com.ibm.rdm.web.pages.showArtifact&artifactURI=https%3A%2F%2Frm.whirlpool.oncloudone.com%2Frm%2Fresources%2F_SuetES5PEearGv06AJXz5w&vvc.configuration=https%3A%2F%2Frm.whirlpool.oncloudone.com%2Frm%2Fcm%2Fstream%2F_qZzlsCEkEea1esD_TUzLUg&componentURI=https%3A%2F%2Frm.whirlpool.oncloudone.com%2Frm%2Frm-projects%2F_qYFucSEkEea1esD_TUzLUg%2Fcomponents%2F_qZvUQCEkEea1esD_TUzLUg
 *              RQM Test Plan - https://qm.whirlpool.oncloudone.com/qm/web/console/Nucleus%20(Quality%20Management)#action=com.ibm.rqm.planning.home.actionDispatcher&subAction=viewTestSuite&id=6634
 *
 *  $Header: Micro.c $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "uc.h"
#include "Micro.h"
#include "Micro_prv.h"
#include "SRSystemConfig.h"
#include "SRException.h"
#include "Crc8.h"
#include "string.h"


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Defines the Enabling or disabling of the Safety relevant test features
 *    @details-
 *
 *    Below are the tests to be performed during boot and run-time -
 *    CPU Register test,
 *    Stack Test
 *    RAM Test(at boot time)
 *    Flash Test
 *    Watchdog Test
 *
 *    @param  - ENABLED(1)    - Which Enables the ClassB Test. ClassB software shall enable this macro
 *              DISABLED(0)   – Which Disables the ClassB Test. ClassA software can disables this macro
 *
 *    @note -   "#define MICRO_SR_FEATURE_TEST                   ENABLED"
 *              "#define MICRO_SR_FEATURE_TEST                   DISABLED"
 */
#if (CLASSB_FEATURE == ENABLED)
    #ifndef MICRO_SR_FEATURE_TEST
        #define MICRO_SR_FEATURE_TEST                               ENABLED
    #endif
#else
    #ifndef MICRO_SR_FEATURE_TEST
        #define MICRO_SR_FEATURE_TEST                               DISABLED
    #endif
#endif

#if (MICRO_SR_FEATURE_TEST   == ENABLED)
    #include "SRData.h"
#endif

//---------------------------------------------------------------------------------------------------------------------
//Memory alignment access protection
//Mis aligned access across 0x2000000 boundary are not supported in the ARM Cortex Architecture
#pragma location=0x1FFFFFFC
static __root uint32 Ram0_Page_End_Protection;

#pragma location=0x20000000
static __root uint32 Ram1_Page_Start_Protection;

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Option Bit/ Fuse Bit /Flash Configuration Fields
 *    @details- This configurations are programmed at the location of 0x400 to till 0x40F.
 *              The contents are loaded at the boot time by the micro internal code into the appropriate registers
 *
 *              Some of the Configurations are forced to default configurations
 *
 */
PACKED typedef struct
{
    /**
     * @brief If the project decided to secure the flash memory access then they shall define the Access Key
     */
    uint8 BackDoor_Access_Key[8];

    /**
     * @brief Define which program flash regions are protected from program and erase operations.
     * Protected flash regions cannot have their content changed; that is, these regions cannot be programmed and cannot be erased.
     * Unprotected regions can be changed by program and erase operations.
     *
     * This feature is disabled as we do not have a requirement or use case to protect the selective Flash regions from program and erase operations
     */
    uint32 Flash_Protection_Regions;

    /**
     * @brief - Flash Security
     *
     * KEYEN - Back door key access is Enabled or Disabled
     * Bit 7-6          : 11 Backdoor key access disabled
     *                  : 10 Backdoor key access enabled
     *
     * Mass Erase Enable
     * Bit 5-4          : 11 Mass erase is enabled
     *
     * Factory Security Level Access Code Granted
     * Bit 3-2          : 11 Factory access granted
     *
     * Flash Security
     * Bit 1-0          : 10 MCU security status is unsecure
     *
     */
    uint8 Security;

    /**
     * @brief Allows the user to customize the operation of the MCU at boot time.
     *
     * Boot From Flash Memory
     * Bit 7 BOOTSRC_SEL    - 0
     * Bit 1 BOOTPIN_OPT    - 1
     *
     *
     * Below are reserved and written as 1 by default
     * Bit 6 - 4            - 1
     *
     *
     * RESET PIN Configuration - RESET_b pin is dedicated. The port is configured with pullup enabled, open drain, passive filter enabled.
     * Bit 3 RESET_PIN_CFG  - 1
     *
     *
     * Disable the NMI interrupts generated by the dedicated pin -
     * The associated pin continues to default to NMI_b pin controls with internal pullup enabled. When NMI_b pin function is
     * disabled, it cannot be used as a source for low-power mode wake-up.
     * If the NMI function is not required, either for an interrupt or wake up source, it is
     * recommended that the NMI function be disabled by clearing NMI_DIS.
     * Bit 2 NMI_DIS        - 0
     *
     *
     * LPBOOT - Normal boot: Core and system clock divider (DIVCORE) is 0x0 (divide by 1).
     * Bit 0                - 1
     *
     */
    uint8 Boot_options;

    /**
     * @brief The FEPROT register defines which EEPROM regions of the FlexRAM are protected against program and erase operations.
     * Protected EEPROM regions cannot have their content changed by writing to it.
     * Unprotected regions can be changed by writing to the FlexRAM.
     *
     * This feature is disabled as we do not have a requirement or use case to protect the EEPROM regions from program and erase operations
     */
    uint8 EEPROM_Protection;

    /**
     * @brief Defines which data flash regions are protected against program and erase operations.
     * Protected Flash regions cannot have their content changed; that is, these regions cannot be programmed and cannot be erased.
     * Unprotected regions can be changed by both program and erase operations.
     *
     * This feature is disabled as we do not have a requirement or use case to protect the Data Flash regions from program and erase operations
     */
    uint8 DataFlash_Protection;
} MICRO_FLASH_CONFIGURATION_TYPE;


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Enable or Disable the Flash security by back door key access
 *    @details- If this feature is enabled then the micro can be programmed or read the Flash memory only by providing the correct access key
 *
 *    The Access key is defined in the MICRO_FLASH_CONFIGURATION_BACKDOOR_ACCESS_KEY macro
 *
 *    By default disabling this feature
 */
#ifndef MICRO_FLASH_SECUTIRY_BY_BACK_DOOR_KEY_ACCESS_FEATURE
    #define MICRO_FLASH_SECUTIRY_BY_BACK_DOOR_KEY_ACCESS_FEATURE        DISABLED
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Flash Access key to secure the Flash read
 *    @details- The key shall be of 8 bytes only
 *              The feature is disabled if all the bits of the key are either 0 or 1
 *
 *              Default value will be as below which disables the feature
 *              #define MICRO_FLASH_CONFIGURATION_BACKDOOR_ACCESS_KEY       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
 *
 */
#if (MICRO_FLASH_SECUTIRY_BY_BACK_DOOR_KEY_ACCESS_FEATURE == ENABLED)
    #ifndef MICRO_FLASH_CONFIGURATION_BACKDOOR_ACCESS_KEY
        #error : "User shall define the MICRO_FLASH_CONFIGURATION_BACKDOOR_ACCESS_KEY if the MICRO_FLASH_SECUTIRY_BY_BACK_DOOR_KEY_ACCESS_FEATURE is ENABLED";
    #endif
#endif

#ifndef MICRO_FLASH_CONFIGURATION_BACKDOOR_ACCESS_KEY
    #define MICRO_FLASH_CONFIGURATION_BACKDOOR_ACCESS_KEY       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#endif

//!  Allows all the Flash regions to be re-programmed
#define MICRO_FLASH_CONFIGURATION_FLASH_PROTECTION_REGIONS      ((uint32)0xFFFFFFFF)

//!  Allows Data Flash regions to be re-programmed
#define MICRO_FLASH_CONFIGURATION_DATAFLASH_PROTECTION          ((uint8)0xFF)

//!  Allows EEPROM regions to be re-programmed
#define MICRO_FLASH_CONFIGURATION_EEPROM_PROTECTION             ((uint8)0xFF)

//! Boot From Flash Memory,
//! Dedicated Reset Pin can not be used for alternate purpose
//! Disable NMI interrupt generation from the pin
//! Normal Boot
#define MICRO_FLASH_CONFIGURATION_BOOT_OPTION                   ((uint8)0x7B)

#if (MICRO_FLASH_SECUTIRY_BY_BACK_DOOR_KEY_ACCESS_FEATURE == DISABLED)
    #define MICRO_FLASH_SECURITY                                ((uint8)0xFE)
#else
    #define MICRO_FLASH_SECURITY                                ((uint8)0xBE)
#endif

#define FLASHCONFIG_START                                       0x400

#ifdef SET_FLASH_OPTION_CONFIGURATION
    #define CONFIGURE_FLASH_OPTION
#endif

#ifdef CONFIGURE_FLASH_OPTION
    #pragma location = FLASHCONFIG_START
    __root static const MICRO_FLASH_CONFIGURATION_TYPE MICRO_FLASH_CONFIGURATION =
    {   /* BackDoor_Access_Key          */      MICRO_FLASH_CONFIGURATION_BACKDOOR_ACCESS_KEY,
        /* Flash_Protection_Regions     */      MICRO_FLASH_CONFIGURATION_FLASH_PROTECTION_REGIONS,
        /* Security                     */      MICRO_FLASH_SECURITY,
        /* Boot_options                 */      MICRO_FLASH_CONFIGURATION_BOOT_OPTION,
        /* EEPROM_Protection            */      MICRO_FLASH_CONFIGURATION_EEPROM_PROTECTION,
        /* DataFlash_Protection         */      MICRO_FLASH_CONFIGURATION_DATAFLASH_PROTECTION,
    };
#endif

//=====================================================================================================================
// The following MACROS handle generation of the register offset and byte masks
#define IRQ_BIT_SHIFT(IRQn)                         ( (((uint32)(IRQn)       )    &  0x03) * 8 )
#define IRQ_SHP_IDX(IRQn)                           (((((uint32)(IRQn) & 0x0F)-8) >>    2)     )
#define IRQ_IP_IDX(IRQn)                            (  ((uint32)(IRQn)            >>    2)     )

#ifndef MICRO_STOP_MODE
    #define MICRO_STOP_MODE                         DISABLED
#endif

#define U_ID      ((uint16 *)0x1FFFF7AC)

#define MICRO_SCB_CPACR_ENABLE_CP10_CP11_CO_PROCESSOR       ((uint32)0x00F00000)


//*********************Watchodog macro's ******************************************************************
// Watchdog
#ifndef MICRO_WATCHDOG_FEATURE
    #define MICRO_WATCHDOG_FEATURE    					ENABLED
#endif

#ifndef MICRO_WATCHDOG_DEBUG_MODE
	#define MICRO_WATCHDOG_DEBUG_MODE				DISABLED
#endif

#define WDOG_Unlock()                               {WDOG.UNLOCK = 0xC520; WDOG.UNLOCK = 0xD928;}    // Write 0xC520 to the unlock register: Key1
																						 // Followed by 0xD928 to complete the unlock: Key2
//********************* End of Watchodog macro's ************************************************************
typedef enum
{
	CLKS_FLL_SELECT = 0,							//Output of FLL is selected FLL Engaged for internal and external Clock
	CLKS_HSI_SELECT = 1,							//Internal reference Clock is selected
	CLKS_HSE_SELECT = 2,							//External reference Clock is selected
	CLKS_PLL_SELECT = 3,							//Output of PLL is selected
}CLOCK_MODE;										//Clock mode Status


#define OUTPUT_CLOCK_OF_FLL_OR_PLL			0
#define INTERNAL_REF_CLOCK					1
#define EXTERNAL_REF_CLOCK					2

//=====================================================================================================================
//Compiler directive
//=====================================================================================================================

//Check HSE frequency range
#if((HSE_VALUE < 3000) || (HSE_VALUE > 32000000))
	#error : "High Speed External Clock frequency value should be in between 3Khz to 32Mhz range in the Micro_prm.h file"
#endif

//High gain oscillator - Controls the OSC mode of operations (
#ifndef MICRO_OSC_HGO
	#define MICRO_OSC_HGO							0			//0 - configure crystal oscillator low power operation.
#endif		

#ifndef MICRO_HIGHSPEEDRUN_MODE
	#define MICRO_HIGHSPEEDRUN_MODE          		DISABLED
#endif													//1 - configure crystal oscillator high-gain operation.

//The LPTimer can measure the cycle of the frequency-divided LPO clock used for the independent watchdog timer (IWDT)
//in the MKV03x. By this function, the main clock oscillation frequency errors can be detected.
#ifndef MICRO_LPTIMER_EXTCLOCKSOURCE
	#define MICRO_LPTIMER_EXTCLOCKSOURCE			DISABLED
#endif

#ifndef MICRO_LPTIMER_COMPARE_VAL
	#define MICRO_LPTIMER_COMPARE_VAL				200
#endif

//SIM_OUTDIV1 Clock 1 output divider value : divide value for the core/system clock from MCGOUTCLK.
#if(SIM_OUTDIV1 == 0)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV0
#elif(SIM_OUTDIV1 == 1)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV1
#elif(SIM_OUTDIV1 == 2)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV2
#elif(SIM_OUTDIV1 == 3)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV3
#elif(SIM_OUTDIV1 == 4)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV4
#elif(SIM_OUTDIV1 == 5)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV5
#elif(SIM_OUTDIV1 == 6)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV6
#elif(SIM_OUTDIV1 == 7)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV7
#elif(SIM_OUTDIV1 == 8)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV8
#elif(SIM_OUTDIV1 == 9)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV9
#elif(SIM_OUTDIV1 == 10)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV10
#elif(SIM_OUTDIV1 == 11)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV11
#elif(SIM_OUTDIV1 == 12)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV12
#elif(SIM_OUTDIV1 == 13)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV13
#elif(SIM_OUTDIV1 == 14)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV14
#elif(SIM_OUTDIV1 == 15)
	#define SIM_OUTDIVIDER1                          SIM_PRE_OUTDIV1_DIV15
#else
	#error : "SIM OUTDIV1 macro value should be either 0,1..... 15 in the Micro_prm.h file"
#endif


//SIM_OUTDIV2 Clock2  output divider value : divide value for the Bus clock from MCGOUTCLK.
#if(SIM_OUTDIV2 == 0)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV0
#elif(SIM_OUTDIV2 == 1)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV1
#elif(SIM_OUTDIV2 == 2)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV2
#elif(SIM_OUTDIV2 == 3)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV3
#elif(SIM_OUTDIV2 == 4)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV4
#elif(SIM_OUTDIV2 == 5)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV5
#elif(SIM_OUTDIV2 == 6)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV6
#elif(SIM_OUTDIV2 == 7)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV7
#elif(SIM_OUTDIV2 == 8)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV8
#elif(SIM_OUTDIV2 == 9)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV9
#elif(SIM_OUTDIV2 == 10)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV10
#elif(SIM_OUTDIV2 == 11)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV11
#elif(SIM_OUTDIV2 == 12)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV12
#elif(SIM_OUTDIV2 == 13)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV13
#elif(SIM_OUTDIV2 == 14)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV14
#elif(SIM_OUTDIV2 == 15)
	#define SIM_OUTDIVIDER2                          SIM_PRE_OUTDIV2_DIV15
#else
	#error : "SIM OUTDIV2 macro value should be either 0,1..... 15  in the Micro_prm.h file"
#endif


//SIM_OUTDIV1 Clock 3 output divider value : divide value for the FlexBus clock from MCGOUTCLK.
#if(SIM_OUTDIV3 == 0)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV0
#elif(SIM_OUTDIV3 == 1)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV1
#elif(SIM_OUTDIV3 == 2)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV2
#elif(SIM_OUTDIV3 == 3)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV3
#elif(SIM_OUTDIV3 == 4)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV4
#elif(SIM_OUTDIV3 == 5)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV5
#elif(SIM_OUTDIV3 == 6)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV6
#elif(SIM_OUTDIV3 == 7)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV7
#elif(SIM_OUTDIV3 == 8)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV8
#elif(SIM_OUTDIV3 == 9)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV9
#elif(SIM_OUTDIV3 == 10)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV10
#elif(SIM_OUTDIV3 == 11)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV11
#elif(SIM_OUTDIV3 == 12)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV12
#elif(SIM_OUTDIV3 == 13)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV13
#elif(SIM_OUTDIV3 == 14)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV14
#elif(SIM_OUTDIV3 == 15)
	#define SIM_OUTDIVIDER3                          SIM_PRE_OUTDIV3_DIV15
#else
	#error : "SIM OUTDIV3 macro value should be either 0,1..... 15  in the Micro_prm.h file"
#endif


//SIM_OUTDIV1 Clock 1 output divider value : divide value for the Flash clock from MCGOUTCLK.
#if(SIM_OUTDIV4 == 0)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV0
#elif(SIM_OUTDIV4 == 1)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV1
#elif(SIM_OUTDIV4 == 2)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV2
#elif(SIM_OUTDIV4 == 3)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV3
#elif(SIM_OUTDIV4 == 4)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV4
#elif(SIM_OUTDIV4 == 5)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV5
#elif(SIM_OUTDIV4 == 6)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV6
#elif(SIM_OUTDIV4 == 7)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV7
#elif(SIM_OUTDIV4 == 8)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV8
#elif(SIM_OUTDIV4 == 9)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV9
#elif(SIM_OUTDIV4 == 10)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV10
#elif(SIM_OUTDIV4 == 11)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV11
#elif(SIM_OUTDIV4 == 12)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV12
#elif(SIM_OUTDIV4 == 13)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV13
#elif(SIM_OUTDIV4 == 14)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV14
#elif(SIM_OUTDIV4 == 15)
	#define SIM_OUTDIVIDER4                          SIM_PRE_OUTDIV4_DIV15
#else
	#error : "SIM OUTDIV4 macro value should be either 0,1..... 15  in the Micro_prm.h file"
#endif

#if(DCO_RANGE == 0)
	#define DCO_FREQUENCY								24000000
#elif (DCO_RANGE == 1)
	#define DCO_FREQUENCY								48000000
#elif (DCO_RANGE == 2)
	#define DCO_FREQUENCY								72000000
#elif (DCO_RANGE == 3)
	#define DCO_FREQUENCY								96000000
#else
	#error : "DCO_FREQUENCY macro value should be either 0,1,2 or 3  in the Micro_prm.h file"
#endif

#if(DCO_RANGE_INT == 0)
    #define DCO_FREQUENCY_INT                               24000000
#elif (DCO_RANGE_INT == 1)
    #define DCO_FREQUENCY_INT                               48000000
#elif (DCO_RANGE_INT == 2)
    #define DCO_FREQUENCY_INT                               72000000
#elif (DCO_RANGE_INT == 3)
    #define DCO_FREQUENCY_INT                               96000000
#else
    #error : "DCO_FREQUENCY_INT macro value should be either 0,1,2 or 3  in the Micro_prm.h file"
#endif

// Low voltage detector
#ifndef LOW_VOLTAGE_MONITOR_FEATURE
    #define LOW_VOLTAGE_MONITOR_FEATURE        DISABLED
#endif

#if (LOW_VOLTAGE_MONITOR_FEATURE == ENABLED)
    #ifndef LOW_VOLTAGE_LEVEL
 //       #error:"LOW_VOLTAGE_LEVEL macro must be defined in the Micro_prm.h file"
    #else
        #if LOW_VOLTAGE_LEVEL > 7
            #error:"LOW_VOLTAGE_LEVEL macro value must be within 0 to 3  and should be defined in the Micro_prm.h file"
        #endif
    #endif
#ifndef CLOCK_OUT
	#define CLOCK_OUT 				DISABLED
#endif
    // PVD(Programmable Voltage detector) interrupt priority
    #ifndef PVD_PREEMPTION_PRIORITY
        #define PVD_PREEMPTION_PRIORITY     3  //The PVD(Programmable Voltage detector) is in interrupt group 0 having highest priority
    #elif (PVD_PREEMPTION_PRIORITY > 3)
        #error:"PVD_PREEMPTION_PRIORITY macro value must be either 0,1,2,3"
    #endif

	#ifndef PVD_PREEMPTION_SUB_PRIORITY
		#define PVD_PREEMPTION_SUB_PRIORITY     3  //The PVD(Programmable Voltage detector) is in interrupt group 0 having highest priority
	#elif (PVD_PREEMPTION_SUB_PRIORITY > 3)
		#error:"PVD_PREEMPTION_PRIORITY macro value must be either 0,1,2,3"
	#endif
#endif


static const uint8 MICRO_ID[] = {'M','K','V','3','x'};
static const MICRO_ID_TYPE MICRO_CPU_IDENTIFICATION = {(uint8 *)MICRO_ID,5};

#define MICRO_FLASH_SIZE()                  (5822)
#define MICRO_RAM_SIZE()                    (16)
#define MICRO_CPU_ID()                      ((MICRO_ID_TYPE * )(&MICRO_CPU_IDENTIFICATION))
#define MICRO_BOOT_SEQUENCE_1               3
#define MICRO_BOOT_SEQUENCE_2               5
#define MICRO_BOOT_SEQUENCE_3               11

//---------------------------------------------------------------------------------------------------------------------
/**
 * List all the generic structures, variable, private function prototype
 */
#ifndef MICRO_SRPOWERMGR_FEATURE
    #define MICRO_SRPOWERMGR_FEATURE    DISABLED
#endif

#if (MICRO_SRPOWERMGR_FEATURE == ENABLED)
    #include "SRPowerMgr.h"
#endif

#define MICRO_RESETINFO_STRING_SIZE             44

PACKED typedef struct
{
    uint8 Header[MICRO_RESETINFO_STRING_SIZE];
    MICRO_LAST_RESET_TYPE Reset;
    MICRO_LAST_RESET_TYPE Actual_Reset;
    MICRO_RESET_MODE_TYPE Mode;
    MICRO_WARM_RESET_ACTION_TYPE Warm_Reset_Actions;
    BOOL_TYPE Force_Cold_Reset;
    uint8 Warm_Reset_Count;
    //This variable shall always be at the end of the structure
    uint8 Crc8_Data_Integrator;
}MICRO_RESET_INFO_HEADER_TYPE;

//! Holds the Clock status
static MICRO_CLOCK_DEF  Micro_ClocksStatus;

const uint8 MICRO_RESETINFO_HEADER[MICRO_RESETINFO_STRING_SIZE] =      {"Dream is not the thing you see in sleep but"};
const uint8 MICRO_RESETINFO_FOOTER[MICRO_RESETINFO_STRING_SIZE] =      {"it is that thing that doesn't let you sleep"};

#pragma location = ".RESET_INFO_HEADER"
static NO_INIT MICRO_RESET_INFO_HEADER_TYPE Micro_ResetInfo;

#pragma location = ".RESET_INFO_FOOTER"
static NO_INIT uint8 Micro_ResetInfo_Footer[MICRO_RESETINFO_STRING_SIZE];

//=====================================================================================================================
//! ClassB Related
//=====================================================================================================================

#define MICRO_RESETINFO_CLASSB_DATA_FROM      ((uint8 *)&Micro_ResetInfo.Reset)
#define MICRO_RESETINFO_CLASSB_DATA_SIZE      (sizeof(Micro_ResetInfo) - MICRO_RESETINFO_STRING_SIZE - 1)       // -1 to exclude the Crc location

//! Stack test patterns
#define STACK_TEST_PATTERN                    0x55
#define STACK_TEST_PATTERN_INV                0xAA
#define STACK_TEST_BKD                        0x00
#define STACK_TEST_BKD_INV                    0xFF

//"Whirlpool100Year"
#define STACK_TEST_PATTERN1                 0x72696857
#define STACK_TEST_PATTERN2                 0x6F6F706C
#define STACK_TEST_PATTERN3                 0x3030316C
#define STACK_TEST_PATTERN4                 0x72616559

//"EES Organization"
#define STACK_TEST_PATTERN5                 0x20534545
#define STACK_TEST_PATTERN6                 0x6167724F
#define STACK_TEST_PATTERN7                 0x617A696E
#define STACK_TEST_PATTERN8                 0x6E6F6974


#ifndef MICRO_CALLBACK_BEFORE_WATCHDOG_INIT
    #define MICRO_CALLBACK_BEFORE_WATCHDOG_INIT()
#endif

#define MICRO_BOOT_SEQUENCE_1               3
#define MICRO_BOOT_SEQUENCE_2               5
#define MICRO_BOOT_SEQUENCE_3               11

#define RUN_TIME_STACK_TEST_TRUE              0x00000080
#define RUN_TIME_STACK_TEST_COUNT_MASK        0x0000007F
#define RUN_TIME_STACK_TEST_PROP_MASK         0x000000FF

#ifndef MICRO_WARM_RESET_COUNTER_LIMIT
    #define MICRO_WARM_RESET_COUNTER_LIMIT  20
#endif

// Below symbols are exported from the linker file (config.icf)

#pragma section = "STACK_UNDERFLOW_SIGNATURE"
#pragma section = "STACK_OVERFLOW_SIGNATURE"

extern uint32  __ICFEDIT_region_RAM_start__;
extern uint32  __ICFEDIT_region_RAM_end__;

#define MICRO_RAM_START_ADDRESS         ((uint32)&__ICFEDIT_region_RAM_start__)
#define MICRO_RAM_END_ADDRESS           ((uint32)&__ICFEDIT_region_RAM_end__)

#define MICRO_STACK_TEST_START_ADDRESS         ((uint32)__section_begin("STACK_OVERFLOW_SIGNATURE"))
#define MICRO_STACK_TEST_END_ADDRESS           ((uint32)__section_begin("STACK_UNDERFLOW_SIGNATURE"))

extern uint32  MICRO_FLASH_START;
extern uint32  MICRO_FLASH_END;

#define MICRO_FLASH_START_ADDRESS   ((uint32 *)&MICRO_FLASH_START)
#define MICRO_FLASH_END_ADDRESS     ((uint32 *)&MICRO_FLASH_END)

#ifndef MICRO_SR_FEATURE_TEST
    #define MICRO_SR_FEATURE_TEST  DISABLED
#elif (MICRO_SR_FEATURE_TEST == ENABLED)

    /**
     * Ram Periodic Test
     */
#ifndef MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST
//When 4 words are processed per call then time taken to detect the fault is 25.6 seconds for 1KB Stack memory.
        #define MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST  4
    #elif (MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST > 127)
        #error :"MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST macro value should not be more than 127 bytes per call"
    #endif

    // Below symbols are exported from the linker file (config.icf)
uint32 *Run_Time_Stack_Pointer;
uint32 *Run_Time_Stack_Pointer_Inv;
    /**
     * Flash Periodic Test
     *
     * The time taken for 1 word test is 5.9us were the system clock is 72MHz by the Inbuilt tool.
     * The time taken for 10 word test is 56.9us were the system clock is 72MHz by the Inbuilt tool.
     */
    #ifndef MICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST
        #define MICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST  4
    #elif (MICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST > 255)
        #error :"MICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST macro value should not be more than 255 bytes per call"
    #endif

    // crc GDM should be included if the Flash test is enabled.
    #ifndef CRC_H
        #include "Crc16.h"
    #endif

    // symbol MICRO_FLASH_CRC created by the IELF tool, this holds the calculated CRC value on the flash memory.
    extern uint16 MICRO_FLASH_CRC;

    static uint16 Micro_Flash_Crc;
    static uint32 * Micro_Flash_Crc_Pt;

    // Below symbols are exported from the linker file (config.icf)

    #define MICRO_CRC_INITIAL           0x0000
#endif

extern uint32 __ICFEDIT_INTERRUPT_TABLE_start__;
#define NVIC_VECTTAB_FLASH           ((uint32)&__ICFEDIT_INTERRUPT_TABLE_start__)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void MicroGetClocksStatus(void);
static void MicroHWInit(void);
static void MicroInitClk(void);
static void MicroInitClkLowSpeed(void);
static MICRO_LAST_RESET_TYPE MicroReadActualReset(void);
static void MicroWatchdogDisable(void);
#if (MICRO_SR_FEATURE_TEST == ENABLED)
	static void MicroBootWatchdogTest(void);
#endif

static void MicroResetHandling(void);
static void MicroClearResetData(void);
static void MicroSetRamSignature(void);
static BOOL_TYPE MicroIsResetSignatureValid(void);
static void MicroValidateResetInfoDataCrc(void);
static void MicroUpdateResetInfoDataCrc(void);

#if (MICRO_SR_FEATURE_TEST == ENABLED)
    static void MicroBootFlashTest(void);
    static void MicroFailSafe(void);
#endif
// After boot test __iar_program_start which calls the void main(void)
extern void __iar_program_start( void );

//Create a lib_init() function, for the C initialization. This function will copy the initial values for
//static and global initialized variables from ROM to RAM, and initialize zero-initialized data to 0
#ifndef RAM_DATA_INITIALIZE
    extern void __iar_data_init3(void);
    #define RAM_DATA_INITIALIZE()   __iar_data_init3()
#endif

extern void main(void);

#define MICRO_BOOTTEST_WDTINIT()                      Micro__ConfigureWDT()
#define MICRO_BOOTTEST_MICROHWINIT()                  MicroHWInit()
#define MICRO_BOOTTEST_RAMTEST()                      __BootTimeRamTest()
#define MICRO_BOOTTEST_REGTEST()                      __StartupRegisterTest()
#define MICRO_BOOTTEST_FLASHTEST()                    MicroBootFlashTest()
#define MICRO_BOOTTEST_WDTEST()                       MicroBootWatchdogTest()
#define MICRO_BOOTTEST_CLKINIT()                      MicroInitClkLowSpeed()
#define MICRO_RUNTIME_REGISTER_TEST()                 __RunRegisterTest()

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the Micro module
 *    @details  This method is used to initialize clock, Nested Vectored Interrupt (NVIC) and
 *              window watchdog configuration. It should be called prior to any other application method is called
 *              since it initializes micro module.
 *
 *              The Gpio module must be re-initialized if the SRMicro__Initialize() is called in the middle
 *              of the Application code.
 */
void Micro__Initialize(void)
{
	uint8 index;

	#if (MICRO_SR_FEATURE_TEST == ENABLED)
		uint32 * pt_stack_overflow_test;
		uint32 * pt_stack_underflow_test;
	#endif

    SR_MICRO_INITIALIZE_FLOW_BEGIN()
	index = 0;

//	MicroWatchdogDisable();								//Disable Watchdog          -- For every reset By default Watchdog is enabled

	Micro__ConfigureWDT();

    MicroHWInit();

	RCM.SSRS0 |= RCM_SSRS0_SWDOG_MASK;							//Clear Watchdog Timeout flag

	// Initialize the interrupt vectors
    NVIC.Disable[0] = 0xFFFFFFFF;
    NVIC.Clear[0] = 0xFFFFFFFF;

    //Initializes all priorities to 0
    for(index = 0; index <= NVIC_INTERRUPT_PRIORITY_ARRAY; index++)
    {
        NVIC.Priority[index] = NVIC_INTERRUPT_PRIORITY_CLEAR;
    }

    // Priority groups configuration - 4 groups, 64 subgroups
    SCB.AIRCR = AIRC_VECTKEY_MASK | (0x5 << SCB_AIRCR_PRIGROUP_POS);

	#if (CLOCK_OUT == ENABLED)									//Clock out is checked on PTC3(ALT5)
		{
			SIM.SOPT2 = SIM_SOPT2_CLKOUTSEL(CLKOUTSEL);			//(Pin11 of J6 connector on TERboard of KV3x)
			SIM.SCGC5 |= SIM_SCGC5_PORTC_MASK;					//Enable Port C clock
			PCIPORTC.PCR[3] = PORT_PCR_MUX(5);					// Set Pin C3 to ALT 5 clkout function
		}
	#endif

	// Division by zero shall be trapped
	SCB.CCR |= 0x10;
	
    //Set the Vector Table base address
    SCB.VTOR = NVIC_VECTTAB_FLASH;

    //Initializes the system clock
    MicroInitClkLowSpeed();

    // This method read the current system clock configuration and update Micro_ClocksStatus
    MicroGetClocksStatus();

    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        //Initialize the flash periodic test variables.
        Micro_Flash_Crc = MICRO_CRC_INITIAL;
        Micro_Flash_Crc_Pt = MICRO_FLASH_START_ADDRESS;

	    register uint32 * pt_stack_test;
	    //Initialize the Ram periodic test variables.
	    SRData__UpdateLong((uint32 PACKED *)&Run_Time_Stack_Pointer, (uint32 PACKED *)&Run_Time_Stack_Pointer_Inv,MICRO_STACK_TEST_START_ADDRESS);

        pt_stack_test = (uint32*)MICRO_STACK_TEST_END_ADDRESS;
        pt_stack_test[3] = STACK_TEST_PATTERN4;
        pt_stack_test[2] = STACK_TEST_PATTERN3;
        pt_stack_test[1] = STACK_TEST_PATTERN2;
        pt_stack_test[0] = STACK_TEST_PATTERN1;

        pt_stack_test = (uint32*)MICRO_STACK_TEST_START_ADDRESS;
        pt_stack_test[0] = STACK_TEST_PATTERN5;
        pt_stack_test[1] = STACK_TEST_PATTERN6;
        pt_stack_test[2] = STACK_TEST_PATTERN7;
        pt_stack_test[3] = STACK_TEST_PATTERN8;
	#endif

    Micro__ServiceWatchdog();

    #if(MICRO_LPTIMER_EXTCLOCKSOURCE == ENABLED)

        SIM.SCGC5 |= SIM_SCGC5_LPTMR_MASK;									//Enable LPTMR module clock
        /* Configure LPTMR */
        LPTMR0.CSR = 0;
        LPTMR0.CMR = LPTMR_CMR_COMPARE(MICRO_LPTIMER_COMPARE_VAL); 			//Set the compare value to the number of ms to delay
        LPTMR0.PSR = LPTMR_PSR_PCS(0x1) | LPTMR_PSR_PBYP_MASK; 				//Use LPO clock and bypass prescale
        LPTMR0.CSR |= LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK;				//Start the timer
        NVIC.Clear[1]|=0x04000000;											//Clear pending IRQ before enabling this IRQ
        Micro__NVICEnableIRQ(LOW_POWER_TIMER_IRQ_CHANNEL, LPTIMER0_PREEMPTION_PRIORITY, LPTIMER0_SUB_PRIORITY);

    #endif
    SR_MICRO_INITIALIZE__FLOW_END()
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method makes the controller to work at low speed
 *    @details  The clock speed in the low power mode is depend on the clock source
 *
 *    Note - 1. The low speed clock should not be set less than 4MHz to run the Wide and time dependent Application.
 *           2. For the time precision application its recommend to use the External clock source during the low speed.
 */
void Micro__SetLowSpeedMode(void)
{
	uint8 temp_reg;
	uint16 i;
	MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    //Save and Disable the Interrupts
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
	#if (MICRO_WATCHDOG_FEATURE == ENABLED)
		SERVICE_WATCHDOG();
	#endif

		/* Select the desired IRC */
		MCG.MCG_C2.BYTE|= MCG_C2_IRCS_MASK;

		temp_reg = MCG.MCG_C1.BYTE;
		temp_reg &= ~MCG_C1_CLKS_MASK;							//clear CLKS
		temp_reg |= MCG_C1_CLKS(1);								//select IRC as the MCG clock sourse
		MCG.MCG_C1.BYTE = 1<< MCG_C1_CLKS_SHIFT;

		SERVICE_WATCHDOG();
		/* wait until internal reference switches to requested irc. */
		for (i = 0; i < 2000; i++)
			{
				if (!(MCG.MCG_S.BYTE & MCG_S_IRCST_MASK))
					break;										//jump out early if IRCST clears before loop finishes
			}

		SERVICE_WATCHDOG();
		/* Wait for clock status bits to update */
		for (i = 0; i < 2000; i++)
		{
			if (((MCG.MCG_S.BYTE & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x1)
				break;											// jump out early if CLKST shows IRC slected before loop finishes
		}

		MCG.MCG_C1.BIT.IRCLKEN = TRUE;
		MCG.MCG_SC.BIT.FCRDIV = 0;
		MCG.MCG_C6.BYTE = 0<<MCG_C6_PLLS_SHIFT;   				//FLL is selected

		SERVICE_WATCHDOG();
		MicroGetClocksStatus();
		
	    //Restore Interrupts
	    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method configured the Low Voltage Detection if the feature is available.
 * @image    html SRMicro_LVD.jpg
 */
void Micro__ConfigureLvd(void)
{
	#if (LOW_VOLTAGE_MONITOR_FEATURE == ENABLED)

		PMC.LVDSC1.BIT.LVDACK = 1;                      //LVWF must be clear by writing LVWACK first

		//LVD interrupt operation
		#if(LOW_VOLTAGE_DETECT_ACTION == ENABLED)
			PMC.LVDSC1.BIT.LVDRE = TRUE;                //Forces an MCU reset when an enabled LVD event occurs
			PMC.LVDSC1.BIT.LVDIE = TRUE;                //Configure interrupt i.e Low-Voltage Detect Interrupt Enable

		#else
			PMC.LVDSC1.BIT.LVDRE = FALSE;               //LVD events do not generate hardware resets
			PMC.LVDSC1.BIT.LVDIE = FALSE;               //Configure the interrupt i.e Low-voltage warning Interrupt Disable.

		#endif

		//Configure the threshold voltage level
		PMC.LVDSC1.BYTE |= PMC_LVDSC1_LVDV(LVD_LEVEL);             //Selects Low-Voltage Detect Voltage Select

		//Low-voltage warning (LVW) interrupt operation
		PMC.LVDSC2.BIT.LVWACK = 1;                      //LVWF must be clear by writing LVWACK first

		#if(LOW_VOLTAGE_WARNING_ACTION == ENABLED)
			PMC.LVDSC1.BIT.LVWIE = TRUE;                //Enables hardware interrupt requests for LVWF.

		#else
			PMC.LVDSC2.BIT.LVWIE = FALSE;               //Enables hardware interrupt requests for LVWF.

		#endif



		//Configure the threshold voltage level
		PMC.LVDSC2.BYTE |= PMC_LVDSC2_LVWV(LVW_LEVEL);             //Selects Low-Voltage Warning Voltage Select
//
//		//Enable the voltage detector - This filed can be written only one time after resets additional writes are ignored
//		PMC.SPMSC1.BIT.LVDE = TRUE;			            //Enabled LVD logic
//
//		PMC.SPMSC1.BIT.LVDSE = FALSE;

		Micro__NVICEnableIRQ(LVD_LVW_IRQ_CHANNEL,PVD_PREEMPTION_PRIORITY,PVD_PREEMPTION_SUB_PRIORITY);

	#else

//		PMC.SPMSC1.BIT.LVDE = FALSE;			        //Disable the voltage detector
                                                        //This filed can be written only one time after resets additional writes are ignored
	#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method should be called from the Low Voltage detection interrupt handle which is application specific.
 *        This function only clear the register related to the interruption if need.
 *        This is not the handler for the interrupt, this code should be managed by the Application specific code which knows
 *        what must be done during the LVD ISR.
 */
void Micro__ClearLvdIsrRegisters(void)
{
    #if (LOW_VOLTAGE_MONITOR_FEATURE == ENABLED)
            PMC.LVDSC2.BIT.LVWACK = 1;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the current status of the LVD.
 *
 *    @return   MICRO_LVD_STATUS_TYPE :
 *               MICRO_LVD_DISABLED              - If the LVD is disabled
 *               MICRO_LVD_ENABLED_NOT_DETECTING - The LVD is enabled and low voltage threshold condition not happened
 *               MICRO_LVD_ENABLED_DETECTING     - The LVD is enabled and low voltage threshold condition is occured
 */
MICRO_LVD_STATUS_TYPE  Micro__GetLvdStatus(void)
{
    MICRO_LVD_STATUS_TYPE lvdreturn;

    lvdreturn = MICRO_LVD_DISABLED;

    #if (LOW_VOLTAGE_MONITOR_FEATURE == ENABLED)
        if(PMC.LVDSC1.BIT.LVDRE == TRUE)
        {
            lvdreturn = MICRO_LVD_ENABLED_DETECTING;
        }
        else
        {
            lvdreturn = MICRO_LVD_ENABLED_NOT_DETECTING;
        }
    #endif

    return (lvdreturn);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This function enables and sets priority for a device specific interrupt in the NVIC interrupt controller.
 *    @details
 *    @param	irqchnl		: Defines the IRQ Channel number should be from the "IRQ_CHANNEL_TYPE" enum list.
 *    @param	priority	: Should range from 0 to 3.
 *    						  0 is the Highest priority and 3 is the lowest priority.
 *
 *    Note: The priority cannot be set for every core interrupt.
 */
void Micro__NVICEnableIRQ(IRQ_CHANNEL_TYPE irqchnl, uint8 premption_priority, uint8 sub_priority)
{
    uint32 tmppriority;
    uint32 tmppre;
    uint32 tmpsub;

    tmppriority = 0x00;
    tmppre = 0x00;
    tmpsub = 0x0F;

    //if (irqchnl != INVALID_IRQ_CHANNEL)
    {
    //Compute the Corresponding IRQ Priority --------------------------------
    tmppriority = (0x700 - ((SCB.AIRCR) & (uint32)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (uint32)(premption_priority << tmppre);
    tmppriority |=  sub_priority & tmpsub;
    tmppriority = tmppriority << 0x04;

	if(irqchnl < 0)
	{
	    SCB.SHP[((uint32)(irqchnl) & 0xF)-4] = tmppriority;
	}
	else
	{
	    NVIC.Priority[(uint32)(irqchnl)] = tmppriority;

	    // Enable the Selected IRQ Channels --------------------------------------
        NVIC.Enable[(uint32)((uint32)irqchnl) >> 5] = (uint32)(1 << ((uint32)((uint32)irqchnl) & (uint32)0x1F));
	}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    Disable External Interrupt
 *    @details	This function disables a device specific interrupt in the NVIC interrupt controller.
 *    			The interrupt number cannot be a negative value.
 *
 *    @param	irqchnl		: Defines the IRQ Channel number should be from the "IRQ_CHANNEL_TYPE" enum list.
 */
void Micro__NVICDisableIRQ(IRQ_CHANNEL_TYPE irqchnl)
{
    if(irqchnl > 0)
    {
		// Disable the Selected IRQ Channels 
	   	NVIC.Disable[((uint32)(irqchnl) >> 5)] = (1 << ((uint32)(irqchnl) & 0x1F));
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    Handler for a Hard Fault exception.
 *    @details
 *    @remarks  Currently only division by zero exception is handled.
 */
INTERRUPT_DIRECTIVE void Micro__HardfaultHdlr(void)
{
    if ((SCB.HFSR & (1 << 30)) != 0)
    {
        if ((SCB.CFSR & (1UL << 25)) != 0)
        {
            //SRException__Queue(EXCEPTION_DIVISION_BY_ZERO, 0, 0);
        }
    }
    SYSTEM_RESET();
}


//------------------------------------------------------------------------------------------------------------------------------
/**
*    @brief    This method is used for Core/System clock oscillation frequency errors can be detected.
*    @details  This ISR function measure the cycle of the frequency-divided LPO clock used for the independent watchdog timer (IWDT)
*              in the MKV03x. By this function, the main Core/System clock oscillation frequency errors can be detected.
*              This needs to be configured in the SRInterruptVector_prm.h & SRClockMonitor_prv.h file as below
*
*              #define LOW_POWER_TIMER                             Micro__LPTimerInterrupt
*
*              It defines the maximum number of TimerInterrupt counts above the expected is acceptable for the external clock source base
*              #define SRCLOCKMONITOR_CLK_TICKS_ABOVE       5
*
*              It defines the minimum number of TimerInterrupt counts below the expected is acceptable for the external clock source base
*              #define SRCLOCKMONITOR_CLK_TICKS_BELOW       5
*
*              example: for by default LPTIMER = 1Khz  & Prescaler is disabled  = 1msec
*              #define MICRO_LPTIMER_COMPARE_VAL    50 (i.e 50msec )
*              For every 50 msec LPTimer will generate an interrupt so compare this with SR Clockmonitor
*              to test Core/System clock by keeping min and max limits
*
*/
INTERRUPT_DIRECTIVE void Micro__LPTimerInterrupt(void)
{
    #if(MICRO_LPTIMER_EXTCLOCKSOURCE == ENABLED)

        /* write 1 to TCF to clear the LPT timer compare flag */
        LPTMR0.CSR |= LPTMR_CSR_TCF_MASK;
        LPTMR0.CSR = (LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK | LPTMR_CSR_TCF_MASK);

        SRMICRO_EXCEPTION_CONTINUOUS_LPTIMER_HANDLER();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * List all the generic API's used across different micro
 */

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs the Boot test during the system boot.
 *    @details  This method is the first function to be executed in the system and from this function the code
 *              goes to the main function.
 *
 *              This method is responsible for:
 *              CPU Register Boot Test,
 *              Watchdog Boot test,
 *              RAM MarchC test,
 *              Flash Boot test
 *              and writing stack test pattern at the boundaries of stack memory.
 *              After this it calls the system startup routine.
 *
 *
 *              Note-1. The Independent watchdog refresh time should be big enough to perform the boot test
 *                   2. The boot test should not be performed when the application is running.
 */
#pragma optimize=none
INTERRUPT_DIRECTIVE void Micro__BootTest(void)
{
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
		register uint8 sr_boot_test_counter = 0;       //Variable used for testing whether the flow is correct or incorrect
        register uint8 result;
        register uint32 counter;
        register uint32 mask;
    #endif

    //Byte 1 - save_reset_status
    //Byte 0 - save_reset_sync
    register uint32 save_reset_value;

    MICRO_BOOTTEST_MICROHWINIT();
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : Full register test
        //
        //Its mandatory to test all the CPU register test
        //-----------------------------------------------------------------------------------------------------------------
        if (FALSE == MICRO_BOOTTEST_REGTEST())
        {
            //This is the routine where the application will end up if a test fails during the boot time.
            while(1)
            {
               #ifdef MICRO_SR_BOOTTEST_FAILURE_TASK
                   SERVICE_WATCHDOG();
                   MICRO_SR_BOOTTEST_FAILURE_TASK();
               #else
                    SYSTEM_RESET();
               #endif
            }
        }
    #endif

    //This function realizes the reset mode
    MicroResetHandling();
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        sr_boot_test_counter++;
    #endif

    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        RAM_DATA_INITIALIZE();
    }

    #if (MICRO_SRPOWERMGR_FEATURE == ENABLED)
        #if (MICRO_SR_FEATURE_TEST == ENABLED)
            Micro_Flow = MICRO_FLOW_SRPOWERMGR_INIT;
            N_Micro_Flow = (uint8)MICRO_SRDATA__COMPLEMENT(MICRO_FLOW_SRPOWERMGR_INIT);
        #endif
        SRPowerMgr__Initialize();
    #endif

    MICRO_CALLBACK_BEFORE_WATCHDOG_INIT();

    //Initialize the watchdog
    MICRO_BOOTTEST_WDTINIT();
	#if (MICRO_SR_FEATURE_TEST == ENABLED)
		sr_boot_test_counter++;
	#endif

    //Initializes the system clock
    MICRO_BOOTTEST_CLKINIT();
	#if (MICRO_SR_FEATURE_TEST == ENABLED)
		sr_boot_test_counter++;
        MicroValidateResetInfoDataCrc();
        if ((Micro_ResetInfo.Warm_Reset_Actions & MICRO_WARM_RESET_ACTION_SKIP_CLASSB_TEST) == 0)
        {

            if (sr_boot_test_counter != MICRO_BOOT_SEQUENCE_1)
            {
              MicroFailSafe();
            }

            //Skip storing the reset reason as this is the intended reset for test
            Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_SKIP_WATCHDOG_TEST);

            //-----------------------------------------------------------------------------------------------------------------
            // CLASSB Test : Watchdog Test
            //
            //It's mandatory to use the Independent watchdog for the CLASSB test
            //-----------------------------------------------------------------------------------------------------------------
            MICRO_BOOTTEST_WDTEST();
            sr_boot_test_counter++;
            SERVICE_WATCHDOG();

            //-----------------------------------------------------------------------------------------------------------------
            // CLASSB Test : Flash CRC test
            //
            //This test is used to validate the non-volatile memory consistency
            //-----------------------------------------------------------------------------------------------------------------
            MICRO_BOOTTEST_FLASHTEST();
            sr_boot_test_counter++;
            SERVICE_WATCHDOG();

            //Skip the ClassB test during warm reset
            Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_SKIP_CLASSB_TEST);
        }
        else
        {
            sr_boot_test_counter += 2;
        }

        if (sr_boot_test_counter != MICRO_BOOT_SEQUENCE_2)
        {
          MicroFailSafe();
        }
    #endif

    MicroValidateResetInfoDataCrc();
    if (Micro_ResetInfo.Mode == MICRO_RESET_MODE_COLD)
    {
        MicroValidateResetInfoDataCrc();

        //Save reset status on Auto register as March C test will screw the all ram memory content
        save_reset_value    =   (*(uint8 *)(&Micro_ResetInfo.Actual_Reset));
        save_reset_value   |=   ((*(uint8 *)(&Micro_ResetInfo.Reset)) << 8);

        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : RAM MARCH C Test and Bit coupling test
        //
        // This test is used to validate the volatile memory by using the MARCH C test and bit coupling test
        //-----------------------------------------------------------------------------------------------------------------
        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : RAM MARCH C Test
        //
        //This test is used to validate the volatile memory by using the MARCH C test
        //-----------------------------------------------------------------------------------------------------------------
        #if (MICRO_SR_FEATURE_TEST == ENABLED)
            SERVICE_WATCHDOG();

            result = TRUE;

            //=>W0
            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter++)
            {
                *((uint8 *)counter) = 0;
            }
                sr_boot_test_counter++;
            //=>(R0, W1)
            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter++)
            {
                if (*((uint8 *)counter) != 0)
                {
                    result = FALSE;
                }
                *((uint8 *)counter) = 0xFF;
            }
                sr_boot_test_counter++;
            SERVICE_WATCHDOG();

            //=>(R1, W0)
            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter++)
            {
                if (*((uint8 *)counter) != 0xFF)
                {
                    result = FALSE;
                }
                *((uint8 *)counter) = 0x0;
            }
                sr_boot_test_counter++;
            //<=(R0, W1)
            for (counter = MICRO_RAM_END_ADDRESS; counter >= MICRO_RAM_START_ADDRESS; counter--)
            {
                if (*((uint8 *)counter) != 0)
                {
                    result = FALSE;
                }
                *((uint8 *)counter) = 0xFF;
            }
                sr_boot_test_counter++;

            SERVICE_WATCHDOG();

            //<=(R1, W0)
            for (counter = MICRO_RAM_END_ADDRESS; counter >= MICRO_RAM_START_ADDRESS; counter--)
            {
                if (*((uint8 *)counter) != 0xFF)
                {
                    result = FALSE;
                }
                *((uint8 *)counter) = 0x0;
            }
                sr_boot_test_counter++;

            //=>R0
            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter++)
            {
                if (*((uint8 *)counter) != 0)
                {
                    result = FALSE;
                }
            }
                sr_boot_test_counter++;
            if (result == FALSE)
            {
                //If the test fails
                MicroFailSafe();
            }

            SERVICE_WATCHDOG();

            for (counter = MICRO_RAM_START_ADDRESS; counter <= MICRO_RAM_END_ADDRESS; counter += 4)
            {
                SERVICE_WATCHDOG();

                result = *((uint32 *) counter);

                for (mask = 1; mask > 0; mask = (mask << 1))
                {
                    *((uint32 *) counter) = mask;
                    if (*((uint32 *) counter) != mask)
                    {
                        MicroFailSafe();

                    }
                }
                *((uint32 *) counter) = result;
            }

            //refresh the watch dog
            SERVICE_WATCHDOG();

        if (sr_boot_test_counter != MICRO_BOOT_SEQUENCE_3)
        {
            MicroFailSafe();
        }
#endif

        //restore the reset status
        MicroClearResetData();
        (*(uint8 *)(&Micro_ResetInfo.Reset))        = (save_reset_value >> 8) & 0xFF;
        (*(uint8 *)(&Micro_ResetInfo.Actual_Reset)) = save_reset_value & 0xFF;

        MicroUpdateResetInfoDataCrc();

        Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_SKIP_CLASSB_TEST);

        // Go to compiler startup
        __iar_program_start();
    }
    else
    {
        main();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs volatile memory (CPU register, Stack) test periodically.
 *    @details  This method is responsible for:
 *              CPU Registers Test,
 *              Stack memory complete test
 *              Stack overflow and underflow test.
 *
 *              MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST configuration macro defined in the SRMicro_prv.h file
 *              Defines how many bytes will be processed during each interaction of the Continuous stack memory Test.
 *
 *
 *              The time taken for complete stack memory test is calculated below-
 *              FULL RAM TEST (in ms) = (Stack Size/ MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST) *
 *                                       Call rate(of Micro__RamTest())
 *
 *             For example-
 *              Stack memory  size is 2048bytes,  number of bytes processed in each call are 4 and the call rate is 100ms then,
 *              FULL  Stack memory(in ms) =  (2048/4)*100 ms
 *                                        =  51200 ms
 *
 *              Along with this it provides the flow control log and exception control for each tests.
 *
 *              This API is been called from ApplienceMgr Thread at every 100ms
 */
#pragma optimize=none
void Micro__SRRamTest(void)
{
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        //Maximum 4 variables can be assigned as register type. More than that could cause problem.
        //Used to point the Ram location and its adjacent location  on which DC coupling RAM test is performed
	   register uint8 *stack_test_ptr;
	   register uint8 *stack_next_location_ptr;
	   register uint8 *stack_previous_location_ptr;

        //Used to hold the data of a RAM memory location on which the RAM test is conducted
        //Byte3 - ram_next_location_data
        //Byte2 - ram_current_location_data
        //Byte1 - ram_previous_location_data
        //Byte 0 - BIT7      - result - test result
        //       - BIT6-BIT0 - counter, number of bytes processed
	    register uint32 stack_test_data;

         register uint32 * pt_stack_test;

        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : CPU run time Test
        //
        //Tests the R0-R7 registers
        //If any error is found an exception will be thrown.
        //-----------------------------------------------------------------------------------------------------------------
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        if (FALSE == MICRO_RUNTIME_REGISTER_TEST())
        {
            //If the test fails throw test fail exception
            MICRO_SR_EXCEPTION_RUNTIME_CPU_REGISTER();
        }

        //Restore Interrupts
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : RAM periodic Test
        //
        // This test is used to validate the volatile memory
        //-----------------------------------------------------------------------------------------------------------------

        SR_RAMTEST_FLOW_BEGIN()

        //Refresh the watchdog
        Micro__ServiceWatchdog();

        // Check if ram pointer is not corrupted
        if(SRData__CheckLong((uint32 PACKED *)&Run_Time_Stack_Pointer,(uint32 PACKED *)&Run_Time_Stack_Pointer_Inv)== SRDATA_OK)
        {
            //Disable the interrupt during test
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

            stack_test_data       = RUN_TIME_STACK_TEST_TRUE;

            //assign the ram location to be tested to the register variable
            stack_test_ptr = (uint8 *)Run_Time_Stack_Pointer;
            stack_next_location_ptr = (uint8 *)stack_test_ptr+1;
            stack_previous_location_ptr = (uint8 *)(stack_test_ptr - 1);

            for(stack_test_data &= ~RUN_TIME_STACK_TEST_COUNT_MASK; (stack_test_data & RUN_TIME_STACK_TEST_COUNT_MASK) < MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST; stack_test_data++)
            {
                //Check for the RAM boundaries
                if ( stack_test_ptr == (uint8 *)MICRO_STACK_TEST_START_ADDRESS)
                {
                    stack_next_location_ptr = (uint8 *)(stack_test_ptr + 1);
                    stack_previous_location_ptr = (uint8 *)(stack_test_ptr + 2);
                }

                if ( stack_test_ptr == (uint8 *)MICRO_STACK_TEST_END_ADDRESS)
                {
                    stack_next_location_ptr = (uint8 *)(stack_test_ptr - 2);
                    stack_previous_location_ptr = (uint8 *)(stack_test_ptr - 1);
                }

                //store the data value into registers on which RAM test is done
                stack_test_data = ((*stack_test_ptr) << 16)  | (((*stack_next_location_ptr) << 24)) | ((*stack_previous_location_ptr) << 8) | (stack_test_data & RUN_TIME_STACK_TEST_PROP_MASK);

                //========================================================================
                // Actual engine of calculation

                //Write the test location with 0x55 value and the neighbors by 0
                *(stack_test_ptr) = STACK_TEST_PATTERN;
                *(stack_next_location_ptr) = STACK_TEST_BKD;
                *(stack_previous_location_ptr) = STACK_TEST_BKD;

                //Check if the Ram test location is changed because of the neighborhood pattern
                if (*(stack_test_ptr) != STACK_TEST_PATTERN)
                {
                    stack_test_data  &= ~RUN_TIME_STACK_TEST_TRUE;
                }

                //change the neighborhood location value to 0xff and do not change the Ram test value
                *(stack_next_location_ptr) = STACK_TEST_BKD_INV;
                *(stack_previous_location_ptr) = STACK_TEST_BKD_INV;

                //Check if the Ram test location is changed because of the neighborhood pattern
                if (*(stack_test_ptr) != STACK_TEST_PATTERN)
                {
                    stack_test_data  &= ~RUN_TIME_STACK_TEST_TRUE;
                }

                //Write the test location with 0xAA value and the neighbors by 0
                *(stack_test_ptr) = STACK_TEST_PATTERN_INV;
                *(stack_next_location_ptr) = STACK_TEST_BKD;
                *(stack_previous_location_ptr) = STACK_TEST_BKD;

                //Check if the Ram test location is changed because of the neighborhood pattern
                if (*(stack_test_ptr) != STACK_TEST_PATTERN_INV)
                {
                    stack_test_data  &= ~RUN_TIME_STACK_TEST_TRUE;
                }

                //change the neighborhood location value to 0xff and do not change the Ram test value
                *(stack_next_location_ptr) = STACK_TEST_BKD_INV;
                *(stack_previous_location_ptr) = STACK_TEST_BKD_INV;

                //Check if the Ram test location is changed because of the neighborhood pattern
                if (*(stack_test_ptr) != STACK_TEST_PATTERN_INV)
                {
                    stack_test_data  &= ~RUN_TIME_STACK_TEST_TRUE;
                }

                //Restore the value
                *stack_next_location_ptr = (stack_test_data >> 24) & 0xFF;
                *stack_previous_location_ptr = (stack_test_data >> 8) & 0xFF;
                *stack_test_ptr = (stack_test_data >> 16) & 0xFF;;

               //Is the RAM test reached the end of the RAM memory
                if ( stack_test_ptr == (uint8 *)MICRO_STACK_TEST_END_ADDRESS)
                {
                    stack_test_ptr = (uint8 *)MICRO_STACK_TEST_START_ADDRESS;
                }
                else
                {
                    //or else go to the next memory location
                    stack_test_ptr++;
                    stack_next_location_ptr = (uint8 *)(stack_test_ptr + 1);
                    stack_previous_location_ptr = (uint8 *)(stack_test_ptr - 1);
                }
            }//End of for loop for RAM_CONTINUOUS_TEST

            //Restore the RAM test pointer
            Run_Time_Stack_Pointer = (uint32 *)stack_test_ptr;
            Run_Time_Stack_Pointer_Inv = ((uint32 *)~((uint32)Run_Time_Stack_Pointer));

            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }//End of if loop

        if ((stack_test_data & RUN_TIME_STACK_TEST_TRUE) == 0)
          {
              //If the test fails throw test fail exception
            MICRO_SR_EXCEPTION_CONTINUOUS_RAM(Run_Time_Stack_Pointer);
          }

        //-----------------------------------------------------------------------------------------------------------------
        // CLASSB Test : Stack Underflow and overflow test
        //
        //This is used to validate the Stack operation
        //-----------------------------------------------------------------------------------------------------------------
        //check Stack underflow pattern

        #define SREXCEPTION_DATA1               ((uint32)1)     //1 represents that the exception is from Micro module
        #define SREXCEPTION_DATA0_OVERFLOW      ((uint32)0)     //0 represents that the exception is because of the stack overflow
        #define SREXCEPTION_DATA0_UNDERFLOW     ((uint32)1)     //1 represents that the exception is because of the stack underflow

        pt_stack_test = (uint32*)MICRO_STACK_TEST_END_ADDRESS;

        if ((pt_stack_test[0] != STACK_TEST_PATTERN1) ||
            (pt_stack_test[1] != STACK_TEST_PATTERN2) ||
            (pt_stack_test[2] != STACK_TEST_PATTERN3) ||
            (pt_stack_test[3] != STACK_TEST_PATTERN4))
        {
            //If the test fails throw test fail exception
            MICRO_SR_EXCEPTION_STACK_UNDERFLOW();
        }

        //check Stack overflow pattern
        pt_stack_test = (uint32*)MICRO_STACK_TEST_START_ADDRESS;

        if ((pt_stack_test[0] != STACK_TEST_PATTERN5) ||
            (pt_stack_test[1] != STACK_TEST_PATTERN6) ||
            (pt_stack_test[2] != STACK_TEST_PATTERN7) ||
            (pt_stack_test[3] != STACK_TEST_PATTERN8))
        {
            //If the test fails throw test fail exception
            MICRO_SR_EXCEPTION_STACK_OVERFLOW();
        }
        SR_RAMTEST_FLOW_END()
     #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method makes the controller to work at Normal speed or resumes the controller from Low Speed mode
 *    @details  This method provides a way to switch from slow to normal mode of operation
 */
void Micro__SetNormalSpeedMode(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint32 i;

    //Save and Disable the Interrupts
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    MicroInitClk();
    MicroGetClocksStatus();

    #if(MICRO_HIGHSPEEDRUN_MODE == ENABLED)
    {

        /* The PMPROT register may have already been written by init code
         If so then this next write is not done.
         PMPROT is write once after RESET
         this write-once bit allows the MCU to enter High Speed Run Mode (HSRUN)*/
        SMC.PMPROT.BIT.AHSRUN = 1;

        /* Set the RUNM bits to 0b011 for HSRUN mode */
        SMC.PMCTRL = SMC_PMCTRL_RUNM(3);

        /* Wait for HSRUN regulator mode to be confirmed */
        for (i = 0; i < 10000; i++)
        { /* check that the value of SMC_PMSTAT is not 0x80 once it is 0x80, we can stop checking */
            if (SMC.PMSTAT != 0x80)
            {
                // 0x01 Current mode is normal run
                // 0x80 Current mode is high speed run
            }
            else
                break;
        }
    }
    #endif

    //Restore Interrupts
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method puts the micro in WAIT mode
 *    @details  The CPU clock is stopped, reducing the power consumption of the microcontroller.
 *              When an internal or external interrupt request occurs,
 *              the CPU wakes-up from Wait mode and resumes processing
 */
void Micro__GoSleep(void)
{
    Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_LOWPOWER);
    Micro__ForceReset(MICRO_RESET_MODE_WARM);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method refreshes the watchdog
 * @details This method should be called periodically to refresh the WWDG (window watchdog)
 *          and/or IWDG (Independent watchdog) depending on the configuration.
 *
 *          Can be used to resolve processor malfunctions due to hardware or software failures.
 *          Watchdog should be refreshed at every 5ms
 */
void Micro__ServiceWatchdog(void)
{
	#if (MICRO_WATCHDOG_FEATURE == ENABLED)
		SERVICE_WATCHDOG();
	#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It reset the micro when a not handled interruption occurs
 *    @details  This method is used to reset the micro when an interrupt appears which was not handled
 */
INTERRUPT_DIRECTIVE void Micro__Trap(void)
{
    SYSTEM_RESET();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method provides information about the clocks
 *    @details  This method provides information about clocks such as
 *              System clock frequency (SYSCLK_Frequency),
 *              AHB Clock frequency (HCLK_Frequency),
 *              APB1 Clock frequency( PCLK1_Frequency),
 *              APB2 Clock frequency( PCLK2_Frequency),
 *              And HSE(High speed External) initialization status.
 *
 *    @return   MICRO_CLOCK_DEF* : pointer to the clock registers structure
 */
MICRO_CLOCK_DEF * Micro__GetClock(void)
{
    return (&Micro_ClocksStatus);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method adds the delay to the execution
 *    @details  Adds the delay by executing the NOP(No Operation)
 *    @param    delay : Defines the number times the NOP opcode is executed
 */
void Micro__DelayNumNops(uint16 delay)
{
    uint16 count;

    for(count =0; count < delay; count++)
    {
        NOP();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method provides a reason for last system reset.
 *    @details  This method is used to provides information about the source from where reset to system occurs
 *    @return   MICRO_LAST_RESET_TYPE* : pointer to system reset status register structure
 *                                      Reset by Debugger,
 *                                      Reset by Low Voltage Detector (LVD),
 *                                      Reset by Clock failure,
 *                                      Reset by EMC,
 *                                      Reset by illegal opcode execution,
 *                                      Reset by watchdog,
 *                                      Reset by External,
 *                                      And Power on reset.
 */
MICRO_LAST_RESET_TYPE * Micro__GetLastResetstatus(void)
{
    MicroValidateResetInfoDataCrc();
    return (&(Micro_ResetInfo.Actual_Reset));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API assert the reset
 *
 * @param reset_mode
 *
 */
void Micro__ForceReset(MICRO_RESET_MODE_TYPE reset_mode)
{
    if (reset_mode == MICRO_RESET_MODE_WARM)
    {
        //warm reset
        SYSTEM_RESET();
    }
    else
    {
        //Cold reset
        //Corrupt the signature
        Micro_ResetInfo.Header[0] = 0;
        Micro_ResetInfo_Footer[0] = 0;

        SYSTEM_RESET();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API reads the reset mode
 * @return MICRO_RESET_MODE_TYPE:
 *
 */
MICRO_RESET_MODE_TYPE Micro__GetResetMode(void)
{
    MicroValidateResetInfoDataCrc();
    return(Micro_ResetInfo.Mode);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to Enable the warm reset action if the next reset is warn reset type
 * @param warm_reset_action
 */
#pragma inline=forced
void Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_TYPE warm_reset_action)
{
    MicroSetRamSignature();
    MicroValidateResetInfoDataCrc();
    Micro_ResetInfo.Warm_Reset_Actions |= warm_reset_action;
    MicroUpdateResetInfoDataCrc();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to Disable the warm reset action if the next reset is warn reset type
 * @param warm_reset_action
 */
#pragma inline=forced
void Micro__DisableWarmResetAction(MICRO_WARM_RESET_ACTION_TYPE warm_reset_action)
{
    MicroSetRamSignature();
    MicroValidateResetInfoDataCrc();
    Micro_ResetInfo.Warm_Reset_Actions &= ~warm_reset_action;
    MicroUpdateResetInfoDataCrc();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API is used to read the warm reset actions set
 * @return MICRO_WARM_RESET_ACTION_TYPE
 */
MICRO_WARM_RESET_ACTION_TYPE Micro__GetWarmResetAction(void)
{
    if (MicroIsResetSignatureValid() == TRUE)
    {
        MicroValidateResetInfoDataCrc();
        return(Micro_ResetInfo.Warm_Reset_Actions);
    }
    else
    {
        return((MICRO_WARM_RESET_ACTION_TYPE)0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Returns the Warm Reset Count
 * @details - We keep the count on how many times the Warm reset type occurred in the micro ranging from 0 to 20.
 * 			  After 20th Warm reset type we are forcing the cold reset which does clears the RAM memory
 *
 * @return uint8 : Returns the Current Warm reset counts
 * 	@retval
 */
uint8 Micro__GetWarmResetCount(void)
{
    if (MicroIsResetSignatureValid() == TRUE)
    {
        MicroValidateResetInfoDataCrc();
        return(Micro_ResetInfo.Warm_Reset_Count);
    }
    else
    {
        return(0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs non-volatile memory (FLASH) test periodically.
 *    @details  This method is responsible for checking the FLASH memory consistency.
 *              Run time Flash Test which calculates CRC couple bytes each time and keeps on continuing until it
 *              reaches the FLASH memory end and compare the calculated CRC against a CRC value stored
 *              in FLASH memory.
 *
 *              INBUILT_FLASH_CRC_CALCULATOR macro defines the CRC calculation engine either Inbuilt or External.
 *
 *              MICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST configuration macro defined in the SRMicro_prv.h file
 *              Defines how many words will be processed each interaction of the Continuous Flash Test.
 *
 *              The time taken for complete FLASH test is calculated below-
 *              For the inbuilt CRC tool(IELF) -
 *              FULL FLASH TEST (in ms) =
 *                 (((FLASH Size in words)-4)/ MICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST) * Call rate(of Micro__FlashTest())
 *
 *              If the external CRC tool(IELF) is used-
 *              FULL FLASH TEST (in ms) =
 *                  (((FLASH Size in bytes)-4)/ MICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST) * Call rate(of Micro__FlashTest())
 *
 *              In the calculation FLASH Size-4 because the last 4- bytes of the flash memory is used to
 *              hold the calculated CRC value on which the Flash test is not performed.
 *
 *
 *              For example
 *              Flash size is 32768 in words,  number of bytes processed in each call are 4 and the call rate is 5ms
 *              then time taken by the inbuilt CRC calculator is-
 *              FULL FLASH TEST (in ms) =  ((32768-4) /4)  * 5 ms
 *                                      = 2047.75ms
 *
 *              Along with this it provides the flow control log and exception control for each tests.
 *              Note- Should be called at every 5ms or 25ms slot.
 */
#pragma optimize=none
void Micro__SRFlashTest(void)
{
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        uint8 count;

        SR_FLASHTEST_FLOW_BEGIN()
        for (count=0; count< MICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST;count++)
        {
            //Calculate the CRC16 CCIT
            Micro_Flash_Crc = Crc16__ConstRangeProcess((uint8 *)Micro_Flash_Crc_Pt,Micro_Flash_Crc,4);

            Micro_Flash_Crc_Pt++;

            //If the test reached the end of memory
            if (Micro_Flash_Crc_Pt >= (uint32 *)MICRO_FLASH_END_ADDRESS)
            {
                //compare the calculated CRC against a CRC value stored in FLASH memory by the IELF tool.
                if ((uint16)MICRO_FLASH_CRC !=  Micro_Flash_Crc)
                {
                    //If the test fails throw test fail exception
                    MICRO_SR_EXCEPTION_CONTINUOUS_FLASH(Micro_Flash_Crc, (uint16)MICRO_FLASH_CRC);
                }

                Micro_Flash_Crc = MICRO_CRC_INITIAL;
                Micro_Flash_Crc_Pt = (uint32 *)MICRO_FLASH_START_ADDRESS;
            }
        }
    #endif
        SR_FLASHTEST_FLOW_END()
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method returns the Flash size of microcontroller
 *    @return   uint16 : Returns the flash size
 */
uint32 Micro__GetFlashSize(void)
{
    return (MICRO_FLASH_SIZE());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method returns the RAM size of microcontroller.
 *    @return   uint32 : It returns the ram size.
 */
uint32 Micro__GetRamSize(void)
{
    return (MICRO_RAM_SIZE());
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method returns the revision of the microcontrollers core.
 *  @return     uint8 : Revision of the Microcontroller Core
 */
MICRO_ID_TYPE * Micro__GetCoreID(void)
{
    return (MICRO_CPU_ID());
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes system clock.
 *    @details  It uses the PLL as the system clock.
 *              This internal function is called from the Micro__Initialize() API.
 */
static void MicroInitClk(void)
{
	uint16 i;
	uint8 temp_reg;
	//uint8 hgo_val;
	uint32 temp2_reg;
	#if(ICS_CLK_MODE_FEI == DISABLED)
		uint8 irc_select;
		irc_select = 0;
	#endif

	temp2_reg = 0;
	temp_reg = 0;

	/*
	 * Ramp up the system clock, Set the system dividers
	 * NOTE: The PLL init will not configure the system clock dividers, so they must be configured appropriately before calling the PLL
	 * init function to ensure that clocks remain in valid ranges.
	 */
	if(PMC.REGSC.BYTE & PMC_REGSC_ACKISO_MASK)
	{
		PMC.REGSC.BYTE |= PMC_REGSC_ACKISO_MASK;
	}

	temp2_reg = SIM.CLKDIV1;
	temp2_reg &= ~(SIM_CLKDIV1_OUTDIV1_MASK | SIM_CLKDIV1_OUTDIV2_MASK | SIM_CLKDIV1_OUTDIV3_MASK | SIM_CLKDIV1_OUTDIV4_MASK);
								// Core/system						// Busclk							// FlexBus						//Flash
	temp2_reg |= (0 | SIM_CLKDIV1_OUTDIV1(SIM_OUTDIV1) | SIM_CLKDIV1_OUTDIV2(SIM_OUTDIV2) | SIM_CLKDIV1_OUTDIV3(SIM_OUTDIV3) | SIM_CLKDIV1_OUTDIV4(SIM_OUTDIV4));
	SIM.CLKDIV1 = temp2_reg;

    #if ((MICRO_EXTERNAL_OSCILATOR == ENABLED))
        {
            //hgo_val = 1; 			//Configured crystal oscilllator for high gain operation.

        #if(MICRO_CLOCK_MODE_USED_FLL == ENABLED)

            #if(ICS_CLK_MODE_FEE == ENABLED)
                    {
                        //FEE: For FLL Bypassed External Reference Clock
                        //1 External reference clock is enabled and Configures the oscillator load.
                        OSC.OSC_CR |= (OSC_CR_ERCLKEN_MASK) | (MICRO_OSC_SC2P << OSC_CR_SC2P_SHIFT) | (MICRO_OSC_SC4P << OSC_CR_SC4P_SHIFT) | \
                                            (MICRO_OSC_SC8P << OSC_CR_SC8P_SHIFT) | (MICRO_OSC_SC16P << OSC_CR_SC16P_SHIFT);


                        /*Configure the MCG_C2 register : the RANGE value is determined by the external frequency.
                         * Since the RANGE parameter affects the FRDIV divide value it still needs to be set correctly even if the oscillator is not being used */
                        MCG.MCG_C2.BYTE = 0;
                        temp_reg = MCG.MCG_C2.BYTE;
                        temp_reg &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK	| MCG_C2_EREFS0_MASK);	//clear fields before writing new values

                        if (HSE_VALUE <= 40000)
                        {
                            temp_reg |= (MCG_C2_RANGE0(0) | (MICRO_OSC_HGO << MCG_C2_HGO0_SHIFT) | (1 << MCG_C2_EREFS0_SHIFT));
                        }
                        else if (HSE_VALUE <= 8000000)
                        {
                            temp_reg |= (MCG_C2_RANGE0(1) | (MICRO_OSC_HGO << MCG_C2_HGO0_SHIFT) | (1 << MCG_C2_EREFS0_SHIFT));
                        }
                        else
                        {
                            temp_reg |= (MCG_C2_RANGE0(2) | (MICRO_OSC_HGO << MCG_C2_HGO0_SHIFT) | (1 << MCG_C2_EREFS0_SHIFT));
                        }

                        MCG.MCG_C2.BYTE = temp_reg;

                        /* Select external oscilator and Reference Divider and clear IREFS to start ext osc If IRCLK is required it must be enabled outside of
                         * this driver, existing state will be maintained CLKS=0, FRDIV=frdiv_val, IREFS=0, IRCLKEN=0, IREFSTEN=0 */
                        temp_reg = MCG.MCG_C1.BYTE;
                        temp_reg &= ~(MCG_C1_CLKS_MASK | MCG_C1_FRDIV_MASK | MCG_C1_IREFS_MASK);	//Clear values in these fields
                        temp_reg = MCG_C1_CLKS(0) | MCG_C1_FRDIV(FRDIV_DEF);							//Set the required CLKS and FRDIV values
                        MCG.MCG_C1.BYTE = temp_reg;

                        /* if the external oscillator is used need to wait for OSCINIT to set */
                        for (i = 0; i < 10000; i++)
                        {
                            if (MCG.MCG_S.BYTE & MCG_S_OSCINIT0_MASK)
                                break;																//jump out early if OSCINIT sets before loop finishes
                        }

                        /* wait for Reference clock Status bit to clear */
                        for (i = 0; i < 2000; i++)
                        {
                            if (!(MCG.MCG_S.BYTE & MCG_S_IREFST_MASK))
                                break;																// jump out early if IREFST clears before loop finishes
                        }

                        /* Wait for clock status bits to show clock source is ext ref clk */
                        for (i = 0; i < 2000; i++)
                        {
                            if (((MCG.MCG_S.BYTE & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2)
                                break;																//jump out early if CLKST shows EXT CLK slected before loop finishes
                        }

                        /* Now in FBE: It is recommended that the clock monitor is enabled when using an
                         * external clock as the clock source/reference. It is enabled here but can be removed if this is not required.*/
                        MCG.MCG_C6.BYTE |= MCG_C6_CME0_MASK;

                        MCG.MCG_C4.BIT.DMX32 = 1;
                        MCG.MCG_C4.BIT.DRST_DRS = DCO_RANGE;
                        for (i = 0; i < 5000; i++)
                        {
                            NOP();
                        }

                    }
            #else
                    //FBE: For FLL Bypassed External Reference Clock
                    //1 External reference clock is enabled.
                    OSC.OSC_CR |= (OSC_CR_ERCLKEN_MASK) | (MICRO_OSC_SC2P << OSC_CR_SC2P_SHIFT) | (MICRO_OSC_SC4P << OSC_CR_SC4P_SHIFT) | \
                                        (MICRO_OSC_SC8P << OSC_CR_SC8P_SHIFT) | (MICRO_OSC_SC16P << OSC_CR_SC16P_SHIFT);

                    OSC.OSC_DIV = OSC_DIV_ERPS(MICRO_ERPS_OSCERCLK); 			// OSC CLock divider register.
                    MCG.MCG_C2.BYTE = 0x2C;										//High frequency range selected for the crystal oscillator & high-gain.
                    MCG.MCG_C2.BYTE |= (MICRO_OSC_HGO << MCG_C2_HGO0_SHIFT);

                    temp_reg = MCG_C1_CLKS(2) | MCG_C1_FRDIV(FRDIV_DEF); 			//External reference clock selected
                    MCG.MCG_C1.BYTE = temp_reg;

                    /* if the external oscillator is used need to wait for OSCINIT to set */
                    for (i = 0; i < 10000; i++)
                    {
                        if (MCG.MCG_S.BYTE & MCG_S_OSCINIT0_MASK)
                            break; 												//jump out early if OSCINIT sets before loop finishes
                    }

                    /* wait for Reference clock Status bit to clear */
                    for (i = 0; i < 2000; i++)
                    {
                        if (!(MCG.MCG_S.BYTE & MCG_S_IREFST_MASK))
                            break; 												//jump out early if IREFST clears before loop finishes
                    }

                    /* Wait for clock status bits to show clock source is ext ref clk */
                    for (i = 0; i < 2000; i++)
                    {
                        if (((MCG.MCG_S.BYTE & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT)== 0x2)
                            break; 												//jump out early if CLKST shows EXT CLK slected before loop finishes
                    }

                    // Now in FBE: It is recommended that the clock monitor is enabled when using an external clock as the clock source/reference.
                    MCG.MCG_C6.BYTE |= MCG_C6_CME0_MASK;

            #endif


        #else
                #if(ICS_CLK_MODE_PEE == ENABLED)
                {
                    //PEE: For PLL Engaged External Reference Clock
                    //hgo_val = 0; 												//Configured crystal oscilllator for high gain operation.
                    //1 External reference clock is enabled.
                    OSC.OSC_CR |= (OSC_CR_ERCLKEN_MASK) | (MICRO_OSC_SC2P << OSC_CR_SC2P_SHIFT) | (MICRO_OSC_SC4P << OSC_CR_SC4P_SHIFT) | \
                                        (MICRO_OSC_SC8P << OSC_CR_SC8P_SHIFT) | (MICRO_OSC_SC16P << OSC_CR_SC16P_SHIFT);

                    temp_reg = MCG.MCG_C2.BYTE;
                    temp_reg &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK	| MCG_C2_EREFS0_MASK);	//clear fields before writing new values

                    if (HSE_VALUE <= 40000)
                    {
                        temp_reg |= (MCG_C2_RANGE0(0) | (MICRO_OSC_HGO << MCG_C2_HGO0_SHIFT) | (1 << MCG_C2_EREFS0_SHIFT));
                    }
                    else if (HSE_VALUE <= 8000000)
                    {
                        temp_reg |= (MCG_C2_RANGE0(1) | (MICRO_OSC_HGO << MCG_C2_HGO0_SHIFT) | (1 << MCG_C2_EREFS0_SHIFT));
                    }
                    else
                    {
                        temp_reg |= (MCG_C2_RANGE0(2) | (MICRO_OSC_HGO << MCG_C2_HGO0_SHIFT) | (1 << MCG_C2_EREFS0_SHIFT));
                    }

                    MCG.MCG_C2.BYTE = temp_reg;
                    MCG.MCG_C1.BYTE = 0x98;										//External reference clock selected

                    for (i = 0; i < 10000; i++)									//if the external oscillator is used need to wait for OSCINIT to set
                    {
                        if (MCG.MCG_S.BYTE & MCG_S_OSCINIT0_MASK)
                            break;												//jump out early if OSCINIT sets before loop finishes
                    }

                    for (i = 0; i < 2000; i++)									//wait for Reference clock Status bit to clear
                    {
                        if (!(MCG.MCG_S.BYTE & MCG_S_IREFST_MASK))
                            break;												//jump out early if IREFST clears before loop finishes
                    }

                    for (i = 0; i < 2000; i++)									//Wait for clock status bits to show clock source is ext ref clk
                    {
                        if (((MCG.MCG_S.BYTE & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2)
                            break;												// jump out early if CLKST shows EXT CLK slected before loop finishes
                    }

                    //Now in FBE: It is recommended that the clock monitor is enabled when using an external clock as the clock source/reference.
                    MCG.MCG_C6.BYTE |= MCG_C6_CME0_MASK;

                    //Configure MCG_C5: If the PLL is to run in STOP mode then the PLLSTEN bit needs to be OR'ed in here or in user code.
                    temp_reg = MCG.MCG_C5.BYTE;
                    temp_reg &= ~MCG_C5_PRDIV_MASK;
                    temp_reg |= MCG_C5_PRDIV(PRDIV);							//set PLL ref divider
                    MCG.MCG_C5.BYTE = temp_reg;

                    //FBE must transition to PBE mode
                    /* Configure MCG_C6: The PLLS bit is set to enable the PLL, MCGOUT still sourced from ext ref clk. The clock monitor is not enabled here as it has likely
                     * been enabled previously and so the value of CME is not altered here. The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE bit in MCG_C6 */
                    temp_reg = MCG.MCG_C6.BYTE;									// store present C6 value
                    temp_reg &= ~MCG_C6_VDIV_MASK;								//clear VDIV settings
                    temp_reg |= MCG_C6_PLLS_MASK | MCG_C6_VDIV(VDIV0_DEF);			//write new VDIV and enable PLL
                    MCG.MCG_C6.BYTE = temp_reg;									//update MCG_C6

                    for (i = 0; i < 2000; i++)									//wait for PLLST status bit to set
                    {
                        if (MCG.MCG_S.BYTE & MCG_S_PLLST_MASK)
                            break;												//jump out early if PLLST sets before loop finishes
                    }

                    for (i = 0; i < 2000; i++)									//Wait for LOCK bit to set
                    {
                        if (MCG.MCG_S.BYTE & MCG_S_LOCK0_MASK)
                            break;												//jump out early if LOCK sets before loop finishes
                    }

                    //PBE mode transition into PEE mode
                    MCG.MCG_C1.BYTE &= ~MCG_C1_CLKS_MASK;						//clear CLKS to switch CLKS mux to select PLL as MCG_OUT

                    for (i = 0; i < 2000; i++)									// Wait for clock status bits to update
                    {
                        NOP();
                    }

                    //Now in PEE
                }
                #else
                    //For PBE PLL bypass external
                    {

                    }
                #endif

            #endif
            }
    #else				///////For Internal Oscillator ////////
        {
            //FEI: For FLL Engaged Internal
            #if(ICS_CLK_MODE_FEI == ENABLED)					//Default Mode FEI ( FLL Engaged Internal is the default mode of operation )
                    {
                        MCG.MCG_C4.BIT.DMX32 = 1;
                        MCG.MCG_C4.BIT.DRST_DRS = DCO_RANGE;
                        for (i = 0; i < 5000; i++)				// Wait for clock status bits to update
                        {
                            NOP();
                        }
                    }
            #else
            {
                //FEI: For FLL Bypassed Internal
                if (IRCS_CLOCK == 1)											//Select the desired IRC
                {
                    MCG.MCG_C2.BYTE |= MCG_C2_IRCS_MASK; 						//select Fast IRCS
                }
                else
                {
                    MCG.MCG_C2.BYTE &= ~MCG_C2_IRCS_MASK; 						//Select Slow IRCS
                }

                /* Change the CLKS mux to select the IRC as the MCGOUT */
                temp_reg = MCG.MCG_C1.BYTE;
                temp_reg &= ~MCG_C1_CLKS_MASK; 									//clear CLKS
                temp_reg |= MCG_C1_CLKS(1); 									// select IRC as the MCG clock sourse
                MCG.MCG_C1.BYTE = temp_reg;

                /* wait until internal reference switches to requested irc. */
                if (!(irc_select))
                {
                    for (i = 0; i < 2000; i++)
                    {
                        if (!(MCG.MCG_S.BYTE & MCG_S_IRCST_MASK))
                            break; 												//jump out early if IRCST clears before loop finishes
                    }

                }
                else
                {
                    for (i = 0; i < 2000; i++)
                    {
                        if (MCG.MCG_S.BYTE & MCG_S_IRCST_MASK)
                            break; 												//jump out early if IRCST sets before loop finishes
                    }

                }

                /* Wait for clock status bits to update */
                for (i = 0; i < 2000; i++)
                {
                    if (((MCG.MCG_S.BYTE & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x1)
                        break; 													//jump out early if CLKST shows IRC slected before loop finishes
                }

                MCG.MCG_SC.BYTE = MCG_SC_FCRDIV(FCRDIV_CLOCK);					//Fast Clock Internal Reference Divider

            }
            #endif

        }
    #endif

}

/**
 *    @brief    This method initializes system clock to a Low Speed.
 *    @details  Initialization of the clock based on the internal oscillator.
 *              Tuning of the frequency can be done using the DCO_RANGE_INT parameter.
 *
 */
static void MicroInitClkLowSpeed(void)
{
    uint32 i;
    uint32 temp2_reg;

    temp2_reg = 0;
    // Set the range
    MCG.MCG_C4.BIT.DRST_DRS = DCO_RANGE_INT;
    for (i = 0; i < 5000; i++)              // Wait for clock status bits to update
    {
        NOP();
    }
    // Set the DMX32 bit to narrow the frequency
    MCG.MCG_C4.BIT.DMX32 = 1;
    temp2_reg |= (0 | SIM_CLKDIV1_OUTDIV1(LOWSPEED_SIM_OUTDIV1) | SIM_CLKDIV1_OUTDIV2(LOWSPEED_SIM_OUTDIV2) | SIM_CLKDIV1_OUTDIV3(LOWSPEED_SIM_OUTDIV3) | SIM_CLKDIV1_OUTDIV4(LOWSPEED_SIM_OUTDIV4));
	SIM.CLKDIV1 = temp2_reg;

}
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    Update the clock registers with the current clock info.
 *    @details  Updates the Micro_ClocksStatus register.
 */
static void MicroGetClocksStatus()
 {
	uint8 tmp;
	uint32 Mcgoutclock;
	#if(MICRO_HIGHSPEEDRUN_MODE == ENABLED)
		uint16 i;
	#endif

	tmp = 0;
	Mcgoutclock = 0;
	tmp = (MCG.MCG_S.BYTE & MCG_S_CLKST_MASK);					//Get SYSCLK source
	tmp = tmp >> 2;

	switch (tmp)
	{
	case CLKS_FLL_SELECT:        								//Output of the FLL is selected (reset default)
		tmp = (MCG.MCG_C1.BYTE & MCG_C1_IREFS_MASK);
		tmp = tmp >> 2;
		if (tmp)
		{
			//FEI - Internal reference clock is selected - HSI used as system clock
			Micro_ClocksStatus.SYSCLK_Frequency = DCO_FREQUENCY_INT;
			Micro_ClocksStatus.BUSCLK_Frequency = DCO_FREQUENCY_INT;
			Micro_ClocksStatus.FLEXBUS_Frequency = DCO_FREQUENCY_INT;
			Micro_ClocksStatus.FLASH_Frequency = DCO_FREQUENCY_INT;
		}
		else
		{
			//FEE - External reference clock is selected
			Micro_ClocksStatus.SYSCLK_Frequency = DCO_FREQUENCY;
			Micro_ClocksStatus.BUSCLK_Frequency = DCO_FREQUENCY;
			Micro_ClocksStatus.FLEXBUS_Frequency = DCO_FREQUENCY;
			Micro_ClocksStatus.FLASH_Frequency = DCO_FREQUENCY;
		}
		break;

	case CLKS_HSE_SELECT: 										//External reference clock is selected.
		//FBE - FLL Bypass External Mode
		Micro_ClocksStatus.SYSCLK_Frequency = HSE_VALUE;
		Micro_ClocksStatus.BUSCLK_Frequency = HSE_VALUE;
		Micro_ClocksStatus.FLEXBUS_Frequency = HSE_VALUE;
		Micro_ClocksStatus.FLASH_Frequency = HSE_VALUE;
		break;

	case CLKS_PLL_SELECT:										//Output of the PLL is selected.
		Micro_ClocksStatus.SYSCLK_Frequency = MICRO_HSE_VALUE_PLL_CLK;
		Micro_ClocksStatus.BUSCLK_Frequency = MICRO_HSE_VALUE_PLL_CLK;
		Micro_ClocksStatus.FLEXBUS_Frequency = MICRO_HSE_VALUE_PLL_CLK;
		Micro_ClocksStatus.FLASH_Frequency = MICRO_HSE_VALUE_PLL_CLK;
		break;

	case CLKS_HSI_SELECT:										//Internal reference clock is selected
		//FEI - Internal reference clock is selected - HSI used as system clock
		if (MCG.MCG_S.BIT.IRCST == 1)
		{
			Mcgoutclock = MICRO_LSI_FAST_MCGIRCLK_VALUE; 		//Slow internal reference clock selected.
		}
		else
		{
			Mcgoutclock = MICRO_LSI_SLOW_MCGIRCLK_VALUE; 		//Fast internal reference clock selected.
		}

		Micro_ClocksStatus.SYSCLK_Frequency = Mcgoutclock;
		Micro_ClocksStatus.BUSCLK_Frequency = Mcgoutclock;
		Micro_ClocksStatus.FLEXBUS_Frequency = Mcgoutclock;
		Micro_ClocksStatus.FLASH_Frequency = Mcgoutclock;
		break;

	default:
		//FEI - Internal reference clock is selected - HSI used as system clock
		Micro_ClocksStatus.SYSCLK_Frequency = HSI_VALUE;
		break;
	}

	//Core/System Clock
	Micro_ClocksStatus.SYSCLK_Frequency = (Micro_ClocksStatus.SYSCLK_Frequency / (((SIM.CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT) + 1));

	//Compute HCLK,
	Micro_ClocksStatus.HCLK_Frequency = Micro_ClocksStatus.SYSCLK_Frequency;

	//Get BUSCLK clock
	Micro_ClocksStatus.BUSCLK_Frequency = (Micro_ClocksStatus.BUSCLK_Frequency / (((SIM.CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> SIM_CLKDIV1_OUTDIV2_SHIFT) + 1));

	//Get FLEXBUS clock
	Micro_ClocksStatus.FLEXBUS_Frequency = (Micro_ClocksStatus.FLEXBUS_Frequency / (((SIM.CLKDIV1 & SIM_CLKDIV1_OUTDIV3_MASK) >> SIM_CLKDIV1_OUTDIV3_SHIFT) + 1));

	//Get FLASH clock
	Micro_ClocksStatus.FLASH_Frequency = (Micro_ClocksStatus.FLASH_Frequency / (((SIM.CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> SIM_CLKDIV1_OUTDIV4_SHIFT) + 1));

}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function initializes the micro hardware
 */
#pragma inline=forced
static void MicroHWInit(void)
{
	MicroWatchdogDisable();								//Disable Watchdog          -- For every reset By default Watchdog is enabled
    SYSTICK.CTRL &= SYSTICK_COUNTER_DISABLE;

    // Sequence for enabling the FPU in both privileged and user
    // modes. The processor must be in privileged mode to read from and write to the CPACR
    // Set bits 20-23 to enable CP10 and CP11 co-processors
    SCB.CPACR |= MICRO_SCB_CPACR_ENABLE_CP10_CP11_CO_PROCESSOR;

    DISABLE_INTERRUPTS();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function tests the watchdog during boot time
 */
#if (MICRO_SR_FEATURE_TEST == ENABLED)
#pragma inline=forced
static void MicroBootWatchdogTest(void)
{
    //-----------------------------------------------------------------------------------------------------------------
    // CLASSB Test : Watchdog Test
    //
    //Its mandatory to use the Independent watchdog for the CLASSB test
    //-----------------------------------------------------------------------------------------------------------------
    if (Micro__GetLastResetstatus()->WATCHDOG == FALSE)
    {
       Micro__ServiceWatchdog();

        while (1);  //Force a reset by watchdog
    }

    SERVICE_WATCHDOG();
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function initializes the watchdog
 */
//#pragma inline=forced
void Micro__ConfigureWDT(void)
{
#if (MICRO_WATCHDOG_FEATURE == ENABLED)
	{
		//First unlock the watchdog so that we can write to registers
		WDOG_Unlock();

		WDOG.STCTRLH = WDOG_STCTRLH_BYTESEL(0x00) |		// test byte 0 when in byte test mode
				WDOG_STCTRLH_ALLOWUPDATE_MASK |			// allow further updates
				WDOG_STCTRLH_WDOGEN_MASK |				// enable watchdog
			#if (MICRO_WATCHDOG_DEBUG_MODE == ENABLED)
				WDOG_STCTRLH_DBGEN_MASK |				//Watchdog enabled in debug mode
			#endif
				0x0100U;




		WDOG.PRESC = WDOG_PRESC_PRESCVAL(WDG_PRESCALER);			// prescalar = PRESC+1
		WDOG.WINH = 0;
		WDOG.WINL = 0;

		//Since the prescalar is now 1, for a 100 mSec timeout: load the time-out high (WDOG_TOVALH) register with 0, and the time-out low (WDOG_TOVALL) register with 100.
		WDOG.TOVALH = 0;
		WDOG.TOVALL = IWDG_RELOAD_VALUE;
		SERVICE_WATCHDOG();

}
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function is used to disable watchdog
 * @param none
 */
#pragma inline=forced
static void MicroWatchdogDisable(void)
 {
	/* First unlock the watchdog so that we can write to registers */
	WDOG_Unlock();

	/* Clear the WDOGEN bit to disable the watchdog */
	WDOG.STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -Reads the actual reset value from the micro and clears it
 * @return
 */
#pragma inline=forced
static MICRO_LAST_RESET_TYPE MicroReadActualReset(void)
{
	MICRO_LAST_RESET_TYPE reset;

	*((uint8 *) (&reset)) = 0;

	//Watchdog reset flag
	if ((RCM.SRS0 & RCM_SRS0_WDOG_MASK) || (RCM.SSRS0 & RCM_SSRS0_SWDOG_MASK))				//if ((SIM_CLK.SIM_SRSID & SIM_SRSID_WDOGRSTF) != FALSE)
	{
		reset.WATCHDOG = TRUE;
	}

	//Low-power reset flag
	if (RCM.SRS0 & RCM_SRS0_LVD_MASK)
	{
		reset.LVD = TRUE;
	}

	//Software reset flag or Option byte loader reset flag
	if (RCM.SRS1 & RCM_SRS1_SW_MASK)
	{
		reset.ILLEGAL_OP = TRUE;				//Reset caused by software setting of SYSRESETREQ bit in AIRCR Register in the Arm Core
	}

	//POR reset flag
	if (RCM.SRS0 & RCM_SRS0_POR_MASK)
	{
		reset.POR = TRUE;		//This POR bits get set all the time for every power up without debugger
								//Temporary this is commented to resolve this POR bit issue talking with FAE
	}

    // External reset pin which makes external reset flag to set on all other type of resets.
    // IWDG reset, WWDG reset, Power reset, Software reset and Low power management reset asserts 20us of low signal on the
    // External reset pin which makes external reset flag to set on all other type of resets.
    // Actual External reset can only be differentiated only if no other reset occurs other than external reset.
	if (RCM.SRS0 & RCM_SRS0_PIN_MASK)
	{
		reset.EXTERNAL = TRUE;
	}

	RCM.SSRS0 = 0x7F;						//Clear System Reset Status Register
	RCM.SSRS1 = 0x3F;

    return(reset);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * List all the generic static functions used
 */

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This is the routine where the application will end up if a test fails during the boot time.
 *    @details   MicroFailSafe function execution can be customized which depends on the SRMICRO_HALT_ON_FAIL macro defined
 *              in the SRMicro_prv.h file.
 *              If the SRMICRO_HALT_ON_FAIL is ENABLED then-
 *              User can perform the fail routine task in the MICRO_SR_BOOTTEST_FAILURE_TASK() which is defined in the SRMicro_prv.h file.
 *
 *              If the MICRO_SR_BOOTTEST_FAILURE_TASK is commented then-
 *              System reset
 *
 *              This routine been called if any one of the below boot test fails-
 *              CPU Register test,
 *              MARCH C Ram test,
 *              FLASH CRC test
 */
#if (MICRO_SR_FEATURE_TEST == ENABLED)
static void MicroFailSafe(void)
{
     while(1)
     {
        #ifdef MICRO_SR_BOOTTEST_FAILURE_TASK
            SERVICE_WATCHDOG();
            MICRO_SR_BOOTTEST_FAILURE_TASK();
        #else
             SYSTEM_RESET();
        #endif
     }
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function performs the flash boot test
 */
#if (MICRO_SR_FEATURE_TEST == ENABLED)
#pragma inline=forced
static void MicroBootFlashTest(void)
{
    //Maximum 12 variables can be assigned as register type. More than that could cause problem.
    //-----------------------------------------------------------------------------------------------------------------
    // CLASSB Test : Flash CRC test
    //
    //This test is used to validate the non-volatile memory consistency
    //-----------------------------------------------------------------------------------------------------------------
    Micro_Flash_Crc = MICRO_CRC_INITIAL;

    // for each word in the block
    for ( Micro_Flash_Crc_Pt = MICRO_FLASH_START_ADDRESS; Micro_Flash_Crc_Pt <= MICRO_FLASH_END_ADDRESS; Micro_Flash_Crc_Pt++ )
    {
        //Calculates the CRC over a range of memory
        Micro_Flash_Crc = Crc16__RangeProcess((uint8 *)Micro_Flash_Crc_Pt,Micro_Flash_Crc,4);

        //Refresh the independent watchdog
        SERVICE_WATCHDOG();
    }

    //compare the calculated CRC against a CRC value stored in FLASH memory by the IELF tool.
    if ((uint16)MICRO_FLASH_CRC !=  Micro_Flash_Crc)
    {
        MicroFailSafe();
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This method handles the reset and realizes which reset method to be apply
 */
#pragma inline=forced
static void MicroResetHandling(void)
{
    MICRO_RESET_INFO_HEADER_TYPE local_reset_info;

    //Is Signature present
    if (MicroIsResetSignatureValid() == TRUE)
    {
        MicroValidateResetInfoDataCrc();

        //Perform the Cold reset if the number of warm resets are more than the MICRO_WARM_RESET_COUNTER_LIMIT
        if (Micro_ResetInfo.Warm_Reset_Count >= MICRO_WARM_RESET_COUNTER_LIMIT)
        {
            MicroValidateResetInfoDataCrc();
            Micro_ResetInfo.Force_Cold_Reset = TRUE;
            MicroUpdateResetInfoDataCrc();
        }
    }
    else
    {
        MicroClearResetData();

        MicroValidateResetInfoDataCrc();
        Micro_ResetInfo.Force_Cold_Reset = TRUE;
        MicroUpdateResetInfoDataCrc();
    }




    //Validate and store the Micro_ResetInfo value into stack variable
    MicroValidateResetInfoDataCrc();
    memcpy(&local_reset_info, &Micro_ResetInfo, sizeof(Micro_ResetInfo));


    //Read the actual reset register content
    local_reset_info.Actual_Reset = MicroReadActualReset();

    //do not save the reset type if the previous reset was intentional. Intentional reset was done in watch dog testing
    if ((local_reset_info.Warm_Reset_Actions & MICRO_WARM_RESET_ACTION_SKIP_WATCHDOG_TEST) != MICRO_WARM_RESET_ACTION_SKIP_WATCHDOG_TEST)
    {
        local_reset_info.Reset = local_reset_info.Actual_Reset;
    }
    else
    {
        //clear the watch dog test
        local_reset_info.Warm_Reset_Actions &= ~MICRO_WARM_RESET_ACTION_SKIP_WATCHDOG_TEST;
    }

    //What is the reset type?Cold or Warm
    if ((local_reset_info.Reset.POR          == 1) ||
        (local_reset_info.Reset.LVD          == 1) ||
        (local_reset_info.Force_Cold_Reset   == TRUE))
    {
        local_reset_info.Mode = MICRO_RESET_MODE_COLD;
    }
    else
    {
        local_reset_info.Warm_Reset_Count++;
        local_reset_info.Mode = MICRO_RESET_MODE_WARM;
    }

    //Restore and update the Crc of the Micro_ResetInfo
    memcpy(&Micro_ResetInfo, &local_reset_info, sizeof(Micro_ResetInfo));
    MicroUpdateResetInfoDataCrc();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function clears the reset data
 */
#pragma inline=forced
static void MicroClearResetData(void)
{
    MicroSetRamSignature();

    (*(uint8 *)&(Micro_ResetInfo.Reset))        = 0;
    (*(uint8 *)&(Micro_ResetInfo.Actual_Reset)) = 0;
    Micro_ResetInfo.Mode                        = MICRO_RESET_MODE_COLD;
    Micro_ResetInfo.Warm_Reset_Actions          = (MICRO_WARM_RESET_ACTION_TYPE)0;
    Micro_ResetInfo.Force_Cold_Reset            = FALSE;
    Micro_ResetInfo.Warm_Reset_Count            = 0;

    MicroUpdateResetInfoDataCrc();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - sets the RAM signature
 */
#pragma inline=forced
static void MicroSetRamSignature(void)
{
    uint8 index;

    for (index = 0; index < MICRO_RESETINFO_STRING_SIZE; index++)
    {
        Micro_ResetInfo.Header[index] = MICRO_RESETINFO_HEADER[index];
        Micro_ResetInfo_Footer[index] = MICRO_RESETINFO_FOOTER[index];
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function validates the reset signature
 * @return
 */
#pragma inline=forced
static BOOL_TYPE MicroIsResetSignatureValid(void)
{
    BOOL_TYPE sign_valid;
    uint8 index;

    sign_valid = TRUE;

    for (index = 0; index < MICRO_RESETINFO_STRING_SIZE; index++)
    {
        if ((Micro_ResetInfo.Header[index] != MICRO_RESETINFO_HEADER[index]) ||
            (Micro_ResetInfo_Footer[index] != MICRO_RESETINFO_FOOTER[index]))
        {
            sign_valid = FALSE;
            break;
        }
    }

    return(sign_valid);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Validates the Data integrity of the Micro_ResetInfo variable
 * @return
 */
static void MicroValidateResetInfoDataCrc(void)
{
    if (Micro_ResetInfo.Crc8_Data_Integrator != Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE)) //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    {
        SRException__Queue(SREXCEPTION_MICRO_RESET_SIGNATURE_CORUPTION,0xFF,0xFF);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - Update the Data integrity of the Micro_ResetInfo variable
 * @return
 */
static void MicroUpdateResetInfoDataCrc(void)
{
    Micro_ResetInfo.Crc8_Data_Integrator = Crc8(0xFF,MICRO_RESETINFO_CLASSB_DATA_FROM, MICRO_RESETINFO_CLASSB_DATA_SIZE); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
}



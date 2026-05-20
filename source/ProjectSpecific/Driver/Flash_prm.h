/**
 *  @file       
 *
 *  @brief    This file contains configurable parameter for Flash module
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: Flash_prm.h 1.1 2015/06/11 08:28:49EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2015/06/11 08:28:49EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FLASH_PRM_H_
#define FLASH_PRM_H_
#include "C_Extensions.h"

//=====================================================================================================================
/**
 *    @brief  - Defines the lower and upper limits for flash memory.
 *    @details- The application code also sits in the main program area so before configuring the
 *              FLASH start address one should take care of the application code size.
 * Following table describes the FLASH memory size for Kinetis MKE0xx series of controllers
 * <pre>
 *  -------------------------------------------------------------
 * |  MKE0xx       | FLASH Memory  | FLASH  Start | FLASH  End   |
 * |  Device       |     Size      |   Address    |   Address    |
 * |---------------|---------------|--------------|--------------|
 * |  LOW Density  |   8K Bytes    |   0x00000100 |   0x00001FFF |
 * |               |   16K Bytes   |   0x00000100 |   0x00003FFF |
 * |---------------|---------------|--------------|--------------|
 * | Medium Density|   32K Bytes   |   0x00000100 |   0x00007FFF |
 * |---------------|---------------|--------------|--------------|
 * | HIGH Density  |   64K Bytes   |   0x00000100 |   0x0000FFFF |
 * |               |   128K Bytes  |   0x00000100 |   0x0001FFFF |
 *  -------------------------------------------------------------
 *  </pre>
 *
 *  In Config.icf
 *  define symbol __ICFEDIT_region_ROM_start__ = 0x00000100;         //Flash Start Address
 *  define symbol __ICFEDIT_region_ROM_end__   = 0x0000FFFF;         //Flash End Address
 *
 *  define symbol __FlashConfig_start__        = 0x00000400;         //Flash config start - Do not change this address
 *  define symbol __FlashConfig_end__          = 0x0000041f;         //Flash config end - Do not change this address
 *
 *  //use below define as it in the config.icf
 *  define region ROM_region = mem:[from __ICFEDIT_region_ROM_start__ to (__FlashConfig_start__ - 1)] | mem:[from (__FlashConfig_end__+1)  to __ICFEDIT_region_ROM_end__];
 *
 *
 *    @note   - Following are the Examples\n
 *              "#define FLASH_LOW_LIMIT     0x00000000"
 *              "#define FLASH_HIGH_LIMIT    0x0001FFFF"
 */
//=====================================================================================================================
extern uint32 __SettingFile_X_Start__;
extern uint32 __SetingFile_End;

#define FLASH_LOW_LIMIT     (uint32)(&__SettingFile_X_Start__)          // Flash lower boundary.
#define FLASH_HIGH_LIMIT    (uint32)(&__SetingFile_End)                 // Flash upper boundary.

//=====================================================================================================================
/**
 *    @brief  - Memory is addressed as relative or absolute address mode.
 *    @details- Absolute address –
 *              means address must be between
 *                  FLASH_USED_AS_DATAFLASH_LOW_LIMIT & FLASH_USED_AS_DATAFLASH_HIGH_LIMIT for EEPROM
 *                  FLASH_LOW_LIMIT & FLASH_HIGH_LIMIT for FLASH memory
 *
 *             Relative address -
 *             System works with relative address meaning 0x0000 is starting address of flash.
 *             Here EEPROM and FLASH memory is treated as one segment of Memory.
 *
 *             When the relative address is enabled and Word by Word writing is enabled then-
 *             DATAFLASH_HIGH_LIMIT macro value should be
 *             DATAFLASH_HIGH_LIMIT = EEPROM address divide of 4 – 1
 *
 *             FLASH_LOW_LIMIT macro value should be
 *             DATAFLASH_HIGH_LIMIT = FLASH address divide of 4
 *
 *    @param  - ENABLED(1) - Relative address mode
 *              DISABLED(0) – Absolute address mode
 *
 *    @note   - Following are the Examples\n
 *              "#define FLASH_AS_DATAFLASH_RELATIVE_ADDRESS   ENABLED" // Relative address mode enabled
 *              "#define FLASH_AS_DATAFLASH_RELATIVE_ADDRESS   DISABLED"// Absolute address mode enabled
 */
//=====================================================================================================================
#define FLASH_AS_DATAFLASH_RELATIVE_ADDRESS      DISABLED

//=====================================================================================================================
/**
 *    @brief  - Defines the lower and upper limits for FLASH_USED_AS_DATAFLASH memory.
 *    @details- The application code also sits in the main program area so before configuring the
 *              Data FLASH_USED_AS_DATAFLASH start address should take care of the application code size.
 *
 *    @note   - Following are the Examples\n
 *              "#define DATAFLASH_LOW_LIMIT     0x00009000"
 *              "#define DATAFLASH_HIGH_LIMIT    0x0000A000"
 */
//=====================================================================================================================
#define FLASH_USED_AS_DATAFLASH_LOW_LIMIT     0x0000A000          // Flash used as Dataflash lower boundary.
#define FLASH_USED_AS_DATAFLASH_HIGH_LIMIT    0x0000C000          // Flash used as Dataflash upper boundary.

//=====================================================================================================================
/**
 * @brief definition of minimum page size that can be erased in one operation. This definition refers to a physical flash page.
 * @details Flash devices allows the system to write once in each position, after that if the system wants to
 * write over a position of memory again, the page that constrains the this memory has to be erase. This parameter defines
 * how big the erasing page is.
 * @note
 * <pre>
 *          For MKV3x(Low density): #define FLASH_PAGE_FORMAT_SIZE                          0x200
 *          For MKE04x(Medium density): #define FLASH_PAGE_FORMAT_SIZE                       0x200
 *          For MKE06x(High density):#define FLASH_PAGE_FORMAT_SIZE                          0x200
 *
 * </pre>
 */
//=====================================================================================================================
#define FLASH_PAGE_FORMAT_SIZE     0x800

//=====================================================================================================================

#endif // FLASH_PRM_H_



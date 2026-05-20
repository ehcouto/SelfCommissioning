/**
 *  @file
 *    @brief    All the possible configurations are defined within this file
 *
 *  $Header: DataFlash_prm.h 1.8 2015/12/07 13:51:59IST Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  *****  Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL  *****
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef DATAFLASH_PRM_H_
    #define DATAFLASH_PRM_H_
#include "C_Extensions.h"

//=====================================================================================================================
/**
 *    @brief  - Enables or disables the system to compile the Fill feature.
 *    @details- Fill feature fills the specific no of bytes of data into the Data Flash with specific value.
 *              Destination address is starting address of data flash where specific value to be filled.
 *
 *    @param  - ENABLED(1) - Enables fill feature
 *              DISABLED(0) – Disables fill feature
 *
 *    @note   - Following are the Examples\n
 *              "#define FILL_DATA_FEATURE				ENABLED"	// Enables fill feature
 *              "#define FILL_DATA_FEATURE				DISABLED"	// Disables fill feature
 */
//=====================================================================================================================
#define FILL_DATA_FEATURE               DISABLED
#define DATAFLASH_RELATIVE_ADDRESS		DISABLED

//=====================================================================================================================
/**
 *    @brief  - This macro assigns the DataFlash Write timeout value.
 *    @details- The timeout value should be more than 10ms.
 *              The parameter must be set to the appropriate value depending on the DataFlash__Handler() call rate.
 *
 *    @param  - The below values depend on the DataFlash__Handler() call rate-
 *              250us--->40
 *              1ms----->10
 *              5ms----->2
 *              25ms---->1
 *
 *    @note   - Following are the Examples\n
 *              //If the DataFlash__Handler() is called in the 250us then-
 *              "#define DATAFLASH_TIMEOUT               40"
 *
 *              //If the DataFlash__Handler() is called in the 1ms then-
 *              "#define DATAFLASH_TIMEOUT               10"
 *
 *              //If the DataFlash__Handler() is called in the 5ms then-
 *              "#define DATAFLASH_TIMEOUT               2"
 *
 *              //If the DataFlash__Handler() is called in the 25ms then-
 *              "#define DATAFLASH_TIMEOUT               1"
 */
//=====================================================================================================================
#define DATAFLASH_TIMEOUT               50

//=====================================================================================================================
/**
 *    @brief  - Enables either Byte by Byte or Word by Word write operation.
 *    @details- If this is Enabled it slows down the write process but saves the code memory or
 *              if it’s Disabled it increases the write speed but the code memory increases.
 *
 *    @param  - ENABLED(1) - Enables Byte By Byte Write
 *              DISABLED(0) – Enables Word by Word Write
 *
 *    @note   - Following are the Examples\n
 *              "#define DATAFLASH_WRITE_BYTE_BY_BYTE    ENABLED"// Byte programming Enabled
 *              "#define DATAFLASH_WRITE_BYTE_BY_BYTE    DISABLED"// Word programming Enabled
 */
//=====================================================================================================================
#define DATAFLASH_WRITE_BYTE_BY_BYTE  						DISABLED

//=====================================================================================================================
/**
 *    @brief  - Macro Enables or disables the system to do blocking or non blocking i.e it will use the Background operation mode
 *              for DataFlash Write, Erase, Fill data
 *
 *    @param  - ENABLED(1)  - The System will perform blocking operations.
 *              DISABLED(0) – The system will use the back  ground operation mode, so it doesn't block the system but it requires the
 *    						  handler to be called every 1ms or 5ms.
 *
 *    @note   - Following are the Examples\n
 *              "#define DATAFLASH_WRITE_BLOCKING_FEATURE   ENABLED"   // Enables Blocking mode
 *              "#define DATAFLASH_WRITE_BLOCKING_FEATURE   DISABLED"  // Disables Blocking mode
 */
//=====================================================================================================================
#define DATAFLASH_WRITE_BLOCKING_FEATURE 					DISABLED

//=====================================================================================================================

//UPDATES FOR NUCLEUS:
//=====================================================================================================================
//=====================================================================================================================
/**
 * @brief Memory alignment is related to the fact that some devices have constrains in terms of the smallest memory cell that allow writing.
 * @details For most device there's no constrains so this directive should be defined as 1. In some cases thre's a constrains which can be
 * managed in the Storage device module like the Cortex microcontroller
 * @note
 * <pre>
 *          For R8C3x:                     #define NVDATA_MEMORY_ALIGMENT                              1
 *          For RX62T:                     #define NVDATA_MEMORY_ALIGMENT                              8
 *          For STM32F10x:                 #define NVDATA_MEMORY_ALIGMENT                              2
 *          For STM32F0x:                 #define NVDATA_MEMORY_ALIGMENT                              2
 *          For MCF51AC:                   #define NVDATA_MEMORY_ALIGMENT                              1
 *          For MC9S08AC:                  #define NVDATA_MEMORY_ALIGMENT                              1
 *          For STM8:                      #define NVDATA_MEMORY_ALIGMENT                              1
 *          For External E2prom:           #define NVDATA_MEMORY_ALIGMENT                              1
 *          For SAMD20:                    #define NVDATA_MEMORY_ALIGMENT                              2
 *          For MKE0x(Flash as Dataflash): #define NVDATA_MEMORY_ALIGMENT                              4
 *          For MKE0x(EEPROM as Dataflash):#define NVDATA_MEMORY_ALIGMENT                              1
 * </pre>
 */
#define NVDATA_MEMORY_ALIGMENT                              1
//=====================================================================================================================
/**
 * @brief definition of minimum page size that can be erased in one operation. This definition refers to a physical flash page.
 * @details Flash devices allows the system to write once in each position, after that if the system wants to
 * write over a position of memory again, the page that constrains the this memory has to be erase. This parameter defines
 * how big the erasing page is.
 * @note
 * <pre>
 *          For MKE02x(Low density)   : #define FLASH_PAGE_FORMAT_SIZE                       0x200
 *          For MKE04x(Medium density): #define FLASH_PAGE_FORMAT_SIZE                       0x200
 *          For MKE06x(High density)  : #define FLASH_PAGE_FORMAT_SIZE                       0x200
 *
 * </pre>
 */
//=====================================================================================================================
#define FLASH_PAGE_FORMAT_SIZE     0x200

//=====================================================================================================================
/**
 * @brief definition of minimum page size that can be erased in one operation. This definition refers to a physical dataflash page.
 * @details DataFlash devices allows the system to write once in each position, after that if the system wants to
 * write over a position of memory again, the page that constrains the this memory has to be erase. This parameter defines
 * how big the erasing page is.
 * @note
 * <pre>
 *          For R8C3x:                            #define NVDATA_PAGE_FORMAT_SIZE              0x400
 *          For RX62T:                            #define NVDATA_PAGE_FORMAT_SIZE              0x800
 *          For STM32F10x(Low density) :          #define NVDATA_PAGE_FORMAT_SIZE              0x400
 *          For STM32F10x(High density):          #define NVDATA_PAGE_FORMAT_SIZE              0x800
 *          For STM32F0x(Low density)  :          #define NVDATA_PAGE_FORMAT_SIZE              0x400
 *          For STM32F0x(High density) :          #define NVDATA_PAGE_FORMAT_SIZE              0x800
 *          For MCF51AC:                          #define NVDATA_PAGE_FORMAT_SIZE              0x800
 *          For MC9S08AC:                         #define NVDATA_PAGE_FORMAT_SIZE              0x200
 *          For STM8:                             #define NVDATA_PAGE_FORMAT_SIZE              0x80  (This memory is nor really formated since this is a eeprom)
 *          For External Eeprom:                  #define NVDATA_PAGE_FORMAT_SIZE              0x80  (This memory is nor really formated since this is a eeprom)
 *          For SAMD20:                           #define NVDATA_PAGE_FORMAT_SIZE              0x100
 *          For MKE0x(Flash as Dataflash):        #define NVDATA_PAGE_FORMAT_SIZE              0x200
 *          For MKE0x(EEPROM as Dataflash):       #define NVDATA_PAGE_FORMAT_SIZE              0xFF
 *
 * </pre>
 */
#define NVDATA_PAGE_FORMAT_SIZE                             0xFF
//=====================================================================================================================
/**
 * @brief definition of the size of a logical page in the storage device (the logical page is composed of 1 or more physical pages)
 * @details When using Dataflash module over Flash, this configuration has to match the size of a data flash page or multiple of that.
  * The size of a page has to be big enough to hold at least one copy of each segment plus header of each segment
 * (6 bytes including header and CRC).\n
 * For E2prom usage this number is not really used.\n
 * @note
 * <pre>
 *      consider a RX62T as examples..
 *      Each physical page has 0x800 bytes
 *      The microcontroller has 16 pages of 0x800 each.
 *      We can take the first 8 physical pages to compose each logical page.
 *
 *      #define NVDATA_PAGE_FORMAT_SIZE                             0x400
 *      #define NVDATA_PAGE_SIZE                                    (NVDATA_PAGE_FORMAT_SIZE * 8)
 *
 *      In this case the system will consider a big page of the size = (NVDATA_PAGE_FORMAT_SIZE * 8) but when it needs to erase the page it will to
 *      in chunks of 0x400.
 *
 * </pre>
 * <pre>
 *          #define NVDATA_PAGE_SIZE                            (NVDATA_PAGE_FORMAT_SIZE * 1)
 *          #define NVDATA_PAGE_SIZE                            (NVDATA_PAGE_FORMAT_SIZE * 2)
 *          #define NVDATA_PAGE_SIZE                            (NVDATA_PAGE_FORMAT_SIZE * 3)
 * </pre>
 */
#define NVDATA_PAGE_SIZE                                    (NVDATA_PAGE_FORMAT_SIZE * 1)

#define NVDATA_STATIC_DATA_LOCATION                         (NVDATA_PAGE_SIZE * 2)
//=====================================================================================================================
/**
 * @brief Macro that defines what value is consider that a write or read operation is completed.
 * @note
 * <pre>
 *          For R8C3x:          #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_ACCEPTED
 *
 *          For RX62T using DATAFLASH_WRITE_BLOCKING_FEATURE = ENABLED on DataFlash:
 *                              #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_DONE
 *
 *          For RX62Tusing DATAFLASH_WRITE_BLOCKING_FEATURE = DISABLED on DataFlash::
 *                                         #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_ACCEPTED
 *
 *          For STM32F10x:                 #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_DONE
 *          For STM32F0x:                  #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_DONE
 *          For MCF51AC:                   #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_DONE
 *          For MC9S08AC:                  #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_DONE
 *          For STM8:                      #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_ACCEPTED
 *          For External E2prom:           #define NVDATA_PROCESS_OKAY                                TRUE
 *          For SAMD20:                    #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_DONE
 *          For MKE0x(Flash as Dataflash): #define NVDATA_PROCESS_OKAY                                FLASH_RESPONSE_DONE
 *          For MKE0x(EEPROM as Dataflash):#define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_DONE
 * </pre>
 */
#define NVDATA_PROCESS_OKAY                                 DATAFLASH_RESPONSE_DONE

//=====================================================================================================================
#define NVDATA__INITIALIZEDEVICE()                      DataFlash__Initialize()
//=====================================================================================================================
/**
 * @brief Macro that defines the method used to write into the storage system.
 * @details Normally it's a call to NVData__Write or EepromI2c__Write
 * @note
 * <pre>
 *          For R8C3x:                      #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 *          For RX62T:                      #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 *          For STM32F10x:                  #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 *          For STM32F0x:                   #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 *          For MCF51AC:                    #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 *          For MC9S08AC:                   #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 *          For STM8:                       #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 *          For External E2prom:            #define NVDATA__WRITE(destination,source,size)              EepromI2c__WriteData(source,destination,size)
 *          For SAMD20:                     #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 *          For MKE0x(Flash as Dataflash):  #define NVDATA__WRITE(destination,source,size)              Flash__Write(destination,source,size)
 *          For MKE0x(EEPROM as Dataflash): #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(source,destination,size)
 * </pre>
 */
#define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)

//=====================================================================================================================
/**
 * @brief Macro that defines the method used to read from the storage system.
 * @details Normally it's a call to NVData__Read or EepromI2c__Read
 * @note
 * <pre>
 *          For R8C3x:                     #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 *          For RX62T:                     #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 *          For STM32F10x:                 #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 *          For STM32F0x:                  #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 *          For MCF51AC:                   #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 *          For MC9S08AC:                  #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 *          For STM8:                      #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 *          For External E2prom:           #define NVDATA__READ(destination,source,size)              EepromI2c__ReadData(source,destination,size)
 *          For SAMD20:                    #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 *          For MKE0x(Flash as Dataflash): #define NVDATA__READ(destination,source,size)              Flash__Read(destination,source,size)
 *          For MKE0x(EEPROM as Dataflash):#define NVDATA__READ(destination,source,size)              DataFlash__Read(source,destination,size)
 * </pre>
 */
#define NVDATA__READ(destination,source,size)               DataFlash__Read(destination,source,size)

/**
 * @brief Command used to erase a page of the target device.
 * @details Only required when the storage device doesn't allow the system to overwrite data before the position have been erased.
 * This macro is only used when the directive NVDATA_MEMORY_STRIPING is defined as ENABLED
 * @note
 * <pre>
 *          For R8C3x:                     #define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)
 *          For RX62T:                     #define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)
 *          For STM32F10x:                 #define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)
 *          For STM32F0x:                  #define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)
 *          For MCF51AC:                   #define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)
 *          For MC9S08AC:                  #define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)
 *          For STM8:                      #define NVDATA__PAGEERASE(page_address)
 *          For External E2prom:           #define NVDATA__PAGEERASE(page_address)
 *          For SAMD20:                    #define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)
 *          For MKE0x(Flash as Dataflash): #define NVDATA__PAGEERASE(page_address)                     Flash__FormatPage(page_address)
 *          For MKE0x(EEPROM as Dataflash):#define NVDATA__PAGEERASE(page_address)
 * </pre>
 */
#define NVDATA__PAGEERASE(page_address)

#endif // DATAFLASH_PRM_H_

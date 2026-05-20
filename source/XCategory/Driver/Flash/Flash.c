/**
 *  @file       
 *
 *    @brief    This module implements the standard Flash API (Application Programming Interface)
 *              for Freescale Kinetis MKV03x Micro.
 *    @details  This modules provides the interface to application in order to write or format the Flash of MKV03x.
 *              Note -  1. Controller supports only 4-byte alligned even addressed write. If the application tries to write on odd address
 *                         gives error.
 *                      2. Controller supports only long word write (i.e 4bytes). Application can write 1, 2 or 3 byte were the next adjacent byte
 *                         is written with 0xff(because the controller support word writing)
 *                      3. Before writing data on the memory the flash memory should have erase value(0xff).
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: Micro.c $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Flash.h"
#include "uc.h"
#include "Micro.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//This symbols are exported from config.icf file
extern uint32 __ICFEDIT_region_RAM_start__;
extern uint32 __ICFEDIT_region_RAM_end__;

#define RAM_LOW_LIMIT                   ((uint32)&__ICFEDIT_region_RAM_start__)         // This symbol comes from the linker file
#define RAM_HIGH_LIMIT                  ((uint32)&__ICFEDIT_region_RAM_end__)           // This symbol comes from the linker file
#define FLASH_PAGE_FORMAT_SIZE          0x800


#define FLASH_LOW_LIMIT_VAL         (FLASH_LOW_LIMIT - 1)
#define FLASH_HIGH_LIMIT_VAL        (FLASH_HIGH_LIMIT + 1)

typedef enum
{
	FLASH_READ_ONE_BLOCK             = 0x00,
	FLASH_READ_ONE_SECTION           = 0x01,
	FLASH_PROGRAM_CHECK              = 0x02,			//Program up to two longwords in a flash block
	FLASH_READ_RESOURCE              = 0x03,			//Program up to two longwords in a flash block
	FLASH_PROGRAM_LONGWORD           = 0x06,			//Program up to two longwords in a flash block
	FLASH_ERASE_BLOCK                = 0x08,			//Program up to two longwords in a flash block
	FLASH_ERASE_SECTOR               = 0x09,			//Program up to two longwords in a flash block
	FLASH_READ_ONCE_ALL_BLOCK        = 0x40,			//Program up to two longwords in a flash block
	FLASH_READ_ONCE_BLOCK            = 0x41,			//Program up to two longwords in a flash block
	FLASH_PROGRAM_ONCE               = 0x43,			//Program up to two longwords in a flash block
	FLASH_ERASE_ALL_BLOCKS           = 0x44,			//Program up to two longwords in a flash block
	FLASH_VERIFY_BACKDOOR_ACESS_KEY  = 0x45,
}FLASH_COMMAND;

static FLASH_STATE_TYPE Flash_Status;

#define FLASH_PAGE_SECTOR_SIZE                   FLASH_PAGE_FORMAT_SIZE
#define FLASH_LOWBYTE_DEFAULT                    ((uint32)0x00ff)
#define FLASH_HIGHBYTE_DEFAULT_1_BYTE            ((uint32)0xFFFFFF00)
#define FLASH_HIGHBYTE_DEFAULT_2_BYTE            ((uint32)0xFFFF0000)
#define FLASH_HIGHBYTE_DEFAULT_3_BYTE            ((uint32)0xFF000000)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static FLASH_RESPONSE_TYPE FlashProgram1LongWord(FLASH_ADDRESS_TYPE address, uint32 data);
static void Flash_Launchcmd(uint8 bWaitComplete);

#if(MICRO_HIGHSPEEDRUN_MODE == ENABLED)
    static void Flash_SetNormalSpeedMode(void);
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the flash module.
 *    @details  This method initializes flash related variables.
 */
void Flash__Initialize(void)
{
    Flash_Status = FLASH_STATE_FREE;
	SIM.FCFG1 &= ~(SIM_FCFG1_FLASHDIS_MASK);				//Flash module clock Enabled
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method Copies data from the RAM to the Flash as per the size and returns
 *              the response for write operation.
 *    @details  This method writes data from source (RAM) to destination ( Flash) as per the size
 *              and returns the response for the write operation.
 *              Note -  1. Controller supports only even addressed write. If the application tries to write on odd address
 *                         gives error.
 *                      2. Controller supports only word write. Application can write 1 byte were the next adjacent byte
 *                         is written with 0xff(because the controller support word writing)
 *                      3. The software design supports for odd size write.
 *                         The next adjacent byte is written with 0xff(because the controller support word writing)
 *                      4. Before writing data on the memory the flash memory should have erase value(0xff).
 *
 *    @param    destination : Defines the address of the flash where data to be written
 *    @param    source      : Defines the address of RAM from where data to be written to flash
 *    @param    size        : Number of bytes to be written
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the write operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the write operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
FLASH_RESPONSE_TYPE Flash__Write(FLASH_ADDRESS_TYPE destination,uint8 * source, FLASH_SIZE_TYPE size)
{
	FLASH_RESPONSE_TYPE response;
	response = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;

	//Check for Flash address within range limits
	if ((destination > FLASH_LOW_LIMIT_VAL)	&&
			((destination + (FLASH_ADDRESS_TYPE) size)<= FLASH_HIGH_LIMIT_VAL))
	{
		//MKE02x Flash memory is 4-byte alligned even addressed write
		if ((destination & ((FLASH_ADDRESS_TYPE) 0x03))  != FALSE)
		{
			response = FLASH_RESPONSE_INVALID_SIZE;
		}

		else
		{
			response = FLASH_RESPONSE_DONE;
			Flash_Status = FLASH_STATE_BUSY;

			//Writing long word (4bytes) data in the flash memory
			while ((size >= 4) && (response == FLASH_RESPONSE_DONE))
			{
				SERVICE_WATCHDOG();
				response = FlashProgram1LongWord(destination,*((uint32 *) source));
				source += 4;
				destination += 4;
				size -= 4;
			}

			//Writing the single byte in the flash memory at the end
            if (size == 0x01)
            {
                response = FlashProgram1LongWord((destination ), (FLASH_HIGHBYTE_DEFAULT_1_BYTE | (*((uint32 *) source))));
            }

            //Writing the double byte in the flash memory at the end
            if (size == 0x02)
            {
            	response = FlashProgram1LongWord((destination ), (FLASH_HIGHBYTE_DEFAULT_2_BYTE | (*((uint32 *) source))));
            }

            //Writing the 3 bytes in the flash memory at the end
            if (size == 0x03)
            {
                response = FlashProgram1LongWord((destination ), (FLASH_HIGHBYTE_DEFAULT_3_BYTE | (*((uint32 *) source))));
            }

            //Update Flash_Status
			if (response == FLASH_RESPONSE_DONE)
			{
				Flash_Status = FLASH_STATE_FREE;
			}
			else
			{
				Flash_Status = FLASH_STATE_ERROR;
			}

		}
	}
	return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs erase/format operation on Flash for sector of 512 bytes.
 *    @param    start_address :  address from where the erase to be started
 *    @param    end_address   :  address till erase to be performed
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the erase operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the erase operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
FLASH_RESPONSE_TYPE Flash__FormatPage(FLASH_ADDRESS_TYPE page_init)
{
	FLASH_RESPONSE_TYPE response;
	response = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;
	Flash_Status = FLASH_STATE_BUSY;

	#if(MICRO_HIGHSPEEDRUN_MODE == ENABLED)
		Flash_SetNormalSpeedMode();
	#endif

	if (page_init & 0x03)											//Check for even address alignment
	{
		return (response);
	}

	if ((page_init > FLASH_LOW_LIMIT_VAL)&& (page_init < FLASH_HIGH_LIMIT_VAL))
	{
		response = FLASH_RESPONSE_ERROR_BUSY;

		//Check Flash Memory controller is idle or busy executing a flash command (CCIF = 0)
		if((FTFA.FSTAT && FTFA_FSTAT_CCIF_MASK) == TRUE)
		{
			response = FLASH_RESPONSE_DONE;

		    SERVICE_WATCHDOG();										//Refresh Watchdog
			
			MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

			MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

			//Clear error flags
			FTFA.FSTAT = 0x30;										//ACCERR & FPVIOL No access and protection violation detected
			FTFA.FCCOB0 = 0x09;										//Flash Erase Command
			FTFA.FCCOB1 = page_init >> 16;							//Memory address bits [23:16]
			FTFA.FCCOB2 = page_init >> 8;							//Memory address bits [15:8]
			FTFA.FCCOB3 = page_init;								//Memory address bits [7:0]
			Flash_Launchcmd(TRUE);                  				//Launch the command

			//Check error status
			if(FTFA.FSTAT & FTFA_FSTAT_ACCERR_MASK)
			{
				response = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;		//Access error id detected
				Flash_Status = FLASH_STATE_ERROR;
			}

			if(FTFA.FSTAT & FTFA_FSTAT_FPVIOL_MASK)
			{
				response = FLASH_RESPONSE_WRITE_ERROR;				//Protection violation is detected
				Flash_Status = FLASH_STATE_ERROR;
			}
			if(FTFA.FSTAT & FTFA_FSTAT_MGSTAT0_MASK)
			{
				response = FLASH_RESPONSE_ERROR_BUSY;				//Flash command execution error or flash reset sequence error
				Flash_Status = FLASH_STATE_ERROR;
			}

			//Update Flash_Status
			if (response == FLASH_RESPONSE_DONE)
			{
				Flash_Status = FLASH_STATE_FREE;
			}
			else
			{
				Flash_Status = FLASH_STATE_ERROR;
			}

	#if(MICRO_HIGHSPEEDRUN_MODE == ENABLED)
				Micro__SetNormalSpeedMode();
	#endif

       		MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

		}
	}

	return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs erase/format operation on Flash.
 *    @param    start_address :  address from where the erase to be started
 *    @param    end_address   :  address till erase to be performed
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the erase operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the erase operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
FLASH_RESPONSE_TYPE Flash__Format(FLASH_ADDRESS_TYPE start_address, FLASH_ADDRESS_TYPE end_address)
 {
	FLASH_RESPONSE_TYPE response;
	response = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;

	uint32 No_of_pages;
	uint8 i;

	if (start_address & 0x03)				//Check for even address alignment
	{
		return (response);
	}

	if ((start_address > FLASH_LOW_LIMIT_VAL) && (end_address < FLASH_HIGH_LIMIT_VAL))
	{
		response = FLASH_RESPONSE_DONE;
		No_of_pages = (end_address - start_address);
		No_of_pages = (No_of_pages / FLASH_PAGE_SECTOR_SIZE);

		for (i = 0; i <= No_of_pages; i++)
		{
			if (response == FLASH_RESPONSE_DONE
					&& Flash_Status == FLASH_STATE_FREE)
			{
				response = Flash__FormatPage(start_address);
				start_address = start_address + FLASH_PAGE_FORMAT_SIZE;
			} else
			{
				response = FLASH_RESPONSE_WRITE_ERROR;
				return (response);
			}
		}
	}
	return (response);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs erase operation as flash range mentioned in the configuration file
 *    @details  Erases a memory from  FLASH_LOW_LIMIT_VAL to FLASH_HIGH_LIMIT_VAL
 *
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the erase operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the erase operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */

FLASH_RESPONSE_TYPE Flash__FormatChip(void)
{
    return(Flash__Format(FLASH_LOW_LIMIT, FLASH_HIGH_LIMIT));
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method provides status of  flash whether busy/free/error
 *    @return   enum FLASH_STATE_TYPE
 *      @retval FLASH_STATE_FREE    :  Flash is free for read write
 */
FLASH_STATE_TYPE Flash__GetStatus(void)
{
    return (FLASH_STATE_FREE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads data from source (Flash) and copies to destination (RAM) as per the size
*               and returns the response for the read operation
*     @param    destination : Defines the address of the RAM where data to be copied
*     @param    source      : Defines the address of Data Flash from where data to be read
*     @param    size        : Number of bytes to be read
*     @return   enum FLASH_RESPONSE_TYPE
*       @retval FLASH_RESPONSE_DONE(0)                 : Read operation is performed successfully
*       @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 */
FLASH_RESPONSE_TYPE Flash__Read(uint8 * destination, FLASH_ADDRESS_TYPE source, FLASH_SIZE_TYPE size)
 {
	FLASH_RESPONSE_TYPE response = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;
	FLASH_SIZE_TYPE count;

	#if (FLASH_AS_DATAFLASH_RELATIVE_ADDRESS == ENABLED)
		source += FLASH_USED_AS_DATAFLASH_LOW_LIMIT;
	#endif

	if (Flash_Status != FLASH_STATE_BUSY)
	{
		Flash_Status = FLASH_STATE_BUSY;
	}

	if (((uint32) destination >= (FLASH_ADDRESS_TYPE) RAM_LOW_LIMIT) &&
			(((uint32) destination + (uint32) size - (uint32) 1)<= (FLASH_ADDRESS_TYPE) RAM_HIGH_LIMIT))
	{
		for (count = 0; count < size; count++)
		{
			destination[count] = ((uint8 *) source)[count];
		}
		response = FLASH_RESPONSE_DONE;
		Flash_Status = FLASH_STATE_FREE;                    //Flash_Status Free

	}
	return (response);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes 1 long word data at the specific flash address and
 *              returns the status of flash operation.
 *    @param    address    : Defines address of flash.
 *    @param    data       : Defines data to be written at flash.
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the write operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the write operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
static FLASH_RESPONSE_TYPE FlashProgram1LongWord(FLASH_ADDRESS_TYPE address, uint32 data)
 {
	FLASH_RESPONSE_TYPE status;
	status = FLASH_RESPONSE_ERROR_BUSY;

	MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

	MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

	#if(MICRO_HIGHSPEEDRUN_MODE == ENABLED)
		Flash_SetNormalSpeedMode();
	#endif

	//Clear error flags
	FTFA.FSTAT = 0x30;								//ACCERR & FPVIOL No access and protection violation detected
	FTFA.FCCOB0 = FLASH_PROGRAM_LONGWORD;			//Program flash command
	FTFA.FCCOB1 = address >> 16;					//Memory address bits [23:16]
	FTFA.FCCOB2 = address >> 8;						//Memory address bits [15:8]
	FTFA.FCCOB3 = address;							//Memory address bits [7:0]

	FTFA.FCCOB4 = data >> 24;					     //Big Endian 0A0B0C0D       Memory
	FTFA.FCCOB5 = data >> 16;                        //               | | | |------  a|0A|
	FTFA.FCCOB6 = data >> 8;						 //               | | |------- a+1|0B|
	FTFA.FCCOB7 = data;						         //               | |--------- a+2|0C|
                                                     //               |----------- a+3|0D|

	Flash_Launchcmd(TRUE);                  		//Launch the command

	status = FLASH_RESPONSE_DONE;

	//Check error status
	if(FTFA.FSTAT & FTFA_FSTAT_ACCERR_MASK)
	{
		status = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;		//Access error id detected
	}

	if(FTFA.FSTAT & FTFA_FSTAT_FPVIOL_MASK)
	{
		status = FLASH_RESPONSE_WRITE_ERROR;			//Protection violation is detected
	}

	if(FTFA.FSTAT & FTFA_FSTAT_MGSTAT0_MASK)
	{
		status = FLASH_RESPONSE_ERROR_BUSY;				//Flash command execution error or flash reset sequence error
	}

	#if(MICRO_HIGHSPEEDRUN_MODE == ENABLED)
			Micro__SetNormalSpeedMode();
	#endif

    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

	return (status);									//Return Flash Program Status

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This function wait's till the flash command is executed.
 *    @details
 *    @param    bWaitComplete     : true if need wait till command completed before return.
 */
static void Flash_Launchcmd(uint8 bWaitComplete)
 {
	FTFA.FSTAT |= (FTFA_FSTAT_CCIF_MASK);

	if (bWaitComplete)
	{
		// Wait till command is completed
		while (!(FTFA.FSTAT & FTFA_FSTAT_CCIF_MASK));
	}

}
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This function will step down the speed of Micro.
 *    @details  This method provides a way to switch from High to normal mode of operation.
 *
 */
#if(MICRO_HIGHSPEEDRUN_MODE == ENABLED)
static void Flash_SetNormalSpeedMode(void)
{

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint32 i;

    //Save and Disable the Interrupts
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

	/* The PMPROT register may have already been written by init code
	 If so then this next write is not done.
	 PMPROT is write once after RESET
	 this write-once bit allows the MCU to enter Normal Speed Run Mode (HSRUN)*/
	SMC.PMPROT.BIT.AHSRUN = 0;

	/* Set the RUNM bits to 0b011 for HSRUN mode */
	SMC.PMCTRL = SMC_PMCTRL_RUNM(0);

	/* Wait for HSRUN regulator mode to be confirmed */
	for (i = 0; i < 10000; i++)
	{ /* check that the value of SMC_PMSTAT is not 0x10 once it is 0x10, we can stop checking */
		if (SMC.PMSTAT != 0x01)
		{
			// 0x01 Current mode is normal run
			// 0x80 Current mode is high speed run
		}
		else
		break;
    }

    //Restore Interrupts
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
}
#endif

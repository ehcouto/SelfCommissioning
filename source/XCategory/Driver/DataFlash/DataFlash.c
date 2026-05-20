/**
 *  @file
 *  @brief    This module implements the standard Data Flash API (Application Programming Interface)
 *            for Kinetis MKE02x series only
 *  @details  This modules provides the interface to application in order to write, read, fill or
 *            format the DataFlash of MKE02x.
 *            Note -  1. Controller supports byte by byte and 1 long word write.
 *                    2. The software design supports for both even and odd size write.
 *
 *  
 *
 *   @copyright  Copyright 2016-. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "uc.h"
#include "Micro.h"
#include "DataFlash.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#ifndef DATAFLASH_LOW_LIMIT
	extern uint32 __DataFlash_Start;
	#define DATAFLASH_LOW_LIMIT		(uint32)(&__DataFlash_Start)  // DataFlash lower boundary.
#endif

#ifndef DATAFLASH_HIGH_LIMIT
	extern uint32 __DataFlash_End;
	#define DATAFLASH_HIGH_LIMIT	(uint32)(&__DataFlash_End)
#endif

#ifndef RAM_LOW_LIMIT
	extern uint32 __ICFEDIT_region_RAM_start__;
	#define RAM_LOW_LIMIT                   ((uint32)&__ICFEDIT_region_RAM_start__)      	// This symbol comes from the linker file
#endif

#ifndef RAM_HIGH_LIMIT
	extern uint32 __ICFEDIT_region_RAM_end__;
	#define RAM_HIGH_LIMIT                  ((uint32)&__ICFEDIT_region_RAM_end__)      		// This symbol comes from the linker file
#endif

#ifndef DATAFLASH_RELATIVE_ADDRESS
    #define DATAFLASH_RELATIVE_ADDRESS      ENABLED
#endif


#define EEPROM_SIZE             ((DATAFLASH_HIGH_LIMIT - DATAFLASH_LOW_LIMIT) + 1)

#define DATAFLASH_SIZE          (EEPROM_SIZE)

#if (DATAFLASH_WRITE_BYTE_BY_BYTE == DISABLED)
    static uint8 DataFlash_Alignment;
#endif


typedef enum
{
    DATAFLASH_HANDLER_STATE_IDLE,
    DATAFLASH_HANDLER_STATE_ERASE_BEFORE_WRITE,
    DATAFLASH_HANDLER_STATE_WRITE,
    DATAFLASH_HANDLER_STATE_WAIT_WRITE,
    DATAFLASH_HANDLER_STATE_ERASE_BEFORE_FILL,
    DATAFLASH_HANDLER_STATE_FILL,
    DATAFLASH_HANDLER_STATE_WAIT_FILL
}DATAFLASH_HANDLER_STATE_TYPE;

typedef enum
{
    DATAFLASH_BUSY = 1,
    DATAFLASH_ERROR_WRP,
    DATAFLASH_ERROR_PROGRAM,
    DATAFLASH_COMPLETE,
    DATAFLASH_TIME_OUT
}DATAFLASH_STATUS;

static DATAFLASH_HANDLER_STATE_TYPE DataFlash_State;

typedef enum
{
	DATAFLASH_ERASE_VERIFY_ALL_BLOCKS    = 0x01,			//Verifies that all flash blocks are erased
	DATAFLASH_ERASE_VERIFY_BLOCK         = 0x02,			//Verifies that a flash block is erased
	DATAFLASH_ERASE_ALL_BLOCK            = 0x08,			//Erase All Flash Blocks
	DATAFLASH_ERASE_BLOCK                = 0x09,			//Erases a Flash Block
	DATAFLASH_UNSECURE                   = 0x0B,
	DATAFLASH_SET_USER_MARGIN_LEVEL      = 0x0D,
	DATAFLASH_EARSE_VERIFY               = 0x10,
	DATAFLASH_PROGRAM                    = 0X11,
	DATAFLASH_ERASE_SECTOR               = 0X12,
}DATAFLASH_COMMAND;

#if (FILL_DATA_FEATURE == ENABLED)
    static uint16 DataFlash_Fill;
#endif

static uint8 * DataFlash_Ram_Pointer;

#if (DATAFLASH_WRITE_BLOCKING_FEATURE == DISABLED)
	static uint32 DataFlash_Flash_Pointer;
	static unsigned short int DataFlash_Size;
	static uint8 DataFlash_TimeOut;
	static uint16 Temp_Data1, Temp_Data2;
#endif

static DATAFLASH_STATE_TYPE DataFlash_Status;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static void DataFlash_Launchcmd(uint8 bWaitComplete);
#if (DATAFLASH_WRITE_BLOCKING_FEATURE == ENABLED)
	static DATAFLASH_STATUS DataFlashProgramHalfWord(uint32 address, uint16 data);
	static DATAFLASH_STATUS DataFlashEraseHalfWord(ADDRESS_TYPE page_init);
#endif
#if (DATAFLASH_WRITE_BLOCKING_FEATURE == DISABLED)
	static void DataFlashNoWaitWrite(ADDRESS_TYPE destination, uint8 * source, SIZE_TYPE size);
	static void DataFlashNoWaitErase(ADDRESS_TYPE destination, uint8 * source, SIZE_TYPE size);
	static void DataflashEraseCmd(ADDRESS_TYPE destination);
	static void DataflashWriteCmd(uint32 destination);
	#if (FILL_DATA_FEATURE == ENABLED)
		static void DataFlashNoWaitFill(ADDRESS_TYPE destination,SIZE_TYPE size);
	#endif
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the Data flash module.
 *    @details  This method initializes data flash related variables.
 */
void DataFlash__Initialize(void)
{
    #if 0
	uint32 DataFlashbusclk;

	DataFlash_State = DATAFLASH_HANDLER_STATE_IDLE;
	DataFlash_Status = DATAFLASH_STATE_FREE;
	SIM_CLK.SIM_SCGC |= SIM_SCGC_FLASH_MASK;							//Bus Clock to the Flash Module is Enabled
	DataFlashbusclk = (Micro__GetClock()->BUSCLK_Frequency) / 1000000 - 1;	//Write FCLKDIV register to divide BUSCLK down to a target FCLK (flash clock) of 1MHz

	//Check Flash command status
	if ((FTMRE.FSTAT.BIT.CCIF) == TRUE)
	{
		if (!(FTMRE.FCLKDIV.BYTE & FTMRE_FCLKDIV_FDIVLCK_MASK)) 		//Check clock divider lock
		{
			FTMRE.FCLKDIV.BYTE |= DataFlashbusclk; 						//FDIVLCK = 0 - Clock divider (i.e FDIV) field is Open for Writing
		}
	}

    #ifdef MKE02_FAMILY
        FTMRE.EEPROT.BIT.DPOPEN = 1; 										//Enables EEPROM memory protection from pgm & erase with protected add range defined by DPS bits
        FTMRE.EEPROT.BIT.DPS = 0;											//EEPROM protection size
    #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method Copies data from the RAM to the DataFlash as per the size and returns
 *              the response for write operation.
 *    @details  This method writes data from source (RAM) to destination (Data Flash) as per the size
 *              and returns the response for the write operation.
 *            Note -
 *            	    1. Implemented in this API - Dataflash (EEPROM) byte must be in erased state before being programmed.
 *            		2. Controller supports byte by byte and 1 long write.
 *                  3. The software design supports for both even and odd size write.
 *
 *    @param    destination : Defines the address of the Data flash where data to be written
 *    @param    source      : Defines the address of RAM from where data to be written to flash
 *    @param    size        : Number of bytes to be written
 *    @return   enum DATAFLASH_RESPONSE_TYPE
 *      @retval DATAFLASH_RESPONSE_DONE(0)                 : If the write operation is complete.
 *      @retval DATAFLASH_RESPONSE_WRITE_ERROR(3)          : If the write operation is unsuccessful.
 *      @retval DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 *      @retval DATAFLASH_RESPONSE_INVALID_SIZE(5)         : If the size is invalid i.e. odd size
 *
 *    @note For MKE02x EEROM memory is 256 bytes which is divided into 128 sectors
 *   			1 sector = 2bytes
 *   			To program 1 byte = 10 msec minimum time is required.
 *   			To program 2 byte = 18 msec minimum time is required.
 *   			To program 1 byte = 26 msec minimum time is required.
 *   			To program 1 byte = 33 msec minimum time is required.
 */
DATAFLASH_RESPONSE_TYPE DataFlash__Write(ADDRESS_TYPE destination, uint8 * source, SIZE_TYPE size)
 {
    #if 0
	DATAFLASH_RESPONSE_TYPE response;
	DATAFLASH_STATUS status;

	#if (DATAFLASH_WRITE_BLOCKING_FEATURE == ENABLED)
		FAR uint8 * pflash;
		uint16 temp1;
		uint16 temp2;
		uint8 count;

		temp1 = 0;
		temp2 = 0;
	#endif

	response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;
	status = DATAFLASH_COMPLETE;

	#if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)
		destination += DATAFLASH_LOW_LIMIT;
	#endif

	//Check for the requested size
	if (size > 0)
	{
		//For Absolute Address
		if ((((uint32) source >= RAM_LOW_LIMIT)	&&
				(((uint32) source + (uint32) size - (uint32) 1) <= (RAM_HIGH_LIMIT))) &&
				((destination >= DATAFLASH_LOW_LIMIT) && ((destination + size - 1) <= (DATAFLASH_HIGH_LIMIT))))
		{

		#if (DATAFLASH_WRITE_BLOCKING_FEATURE == DISABLED)
			if (DataFlash_State == DATAFLASH_HANDLER_STATE_IDLE)
			{
				if (DataFlash_Status != DATAFLASH_STATE_BUSY)
				{
				#if (DATAFLASH_WRITE_BYTE_BY_BYTE == DISABLED)
					DataFlash_Alignment = (uint8)(destination % 4);

					if (DataFlash_Alignment > 0)
					{
						DataFlash_Alignment = 4 - DataFlash_Alignment;
					}
				#endif
					DataFlash_Flash_Pointer = destination; //Assign the Address of Flash memory
					DataFlash_Ram_Pointer = source;//Assign source address of RAM memory
					DataFlash_Size = size;//Assign the size

					DataFlash_Status = DATAFLASH_STATE_BUSY;
					DataFlash_State = DATAFLASH_HANDLER_STATE_ERASE_BEFORE_WRITE;
					response = DATAFLASH_RESPONSE_ACCEPTED;
				}
			}

			else
			{
				response = DATAFLASH_RESPONSE_ERROR_BUSY;
				status = DATAFLASH_ERROR_WRP;
			}

		#else

			pflash = (FAR uint8 *) destination;             		//For High byte byte
			DataFlash_Ram_Pointer = source;

			//Cortex Flash memory is 2-byte aligned, it writes only on the even aligned address
			//The write address should be even
			SERVICE_WATCHDOG();

			if ((destination % 2) != 0)
			{
				destination = destination - 1;						//For Low byte
				pflash = (FAR uint8 *) destination;
				temp2 = *pflash;									//Read the even address byte
				temp1 = *DataFlash_Ram_Pointer;
				temp1 = temp1 << 8;
				temp1 = temp1 | temp2;								//apply the mask  = Even_Address_Read_Data | New Data Byte
				DataFlashEraseHalfWord(destination);
				status = DataFlashProgramHalfWord((destination), (temp1));//Write the Mask which starts from even address
				size--;
				destination++;
				destination++;
				source += 1;
			}

			if ((((destination + size) % 2) != 0))
			{
				destination = destination + size;
				pflash = (FAR uint8 *) destination;					//Read the even address byte
				temp1 = *pflash;
				temp1 = temp1 << 8;
				DataFlash_Ram_Pointer += size - 1;
				temp2 = (*DataFlash_Ram_Pointer);// + size - 1);
				temp1 = temp1 | temp2;								//apply the mask  = Even_Address_Read_Data | New Data Byte
				destination = destination - 1;
				DataFlashEraseHalfWord(destination);
				status = DataFlashProgramHalfWord((destination), (temp1));//Write the Mask which starts from even address
				size--;
				destination = destination - size;
			}

			SERVICE_WATCHDOG();

			//Writing the halfword byte in the flash memory at the end
			count = 0;
			while ((count < size))  								//Writing the word data in the flash memory
			{
				DataFlashEraseHalfWord(destination);
				temp1 = *(source + 1);
				temp1 <<= 8;
				temp1 |= *source;
				status = DataFlashProgramHalfWord((destination), (temp1));
				source += 2;
				destination += 2;
				count += 2;
			}

			if (status == DATAFLASH_COMPLETE)
			{
				response = DATAFLASH_RESPONSE_DONE;
			}
			else if (status == DATAFLASH_ERROR_WRP)
			{
				response = DATAFLASH_RESPONSE_WRITE_ERROR;
			}
			else if (status == DATAFLASH_ERROR_PROGRAM)
			{
				response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;
			}
			else
			{
				response = DATAFLASH_RESPONSE_ERROR_BUSY;				//DATAFLASH_BUSY;
			}
		#endif
		}

	}

	else
	{
		response = DATAFLASH_RESPONSE_INVALID_SIZE;
	}

	return (response);

    #else
	    return (DATAFLASH_RESPONSE_INVALID_SIZE);
	#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads data from source (Data Flash) and copies to destination (RAM) as per the size
*               and returns the response for the read operation
*     @param    destination : Defines the address of the RAM where data to be copied
*     @param    source      : Defines the address of Data Flash from where data to be read
*     @param    size        : Number of bytes to be read
*     @return   enum DATAFLASH_RESPONSE_TYPE
*       @retval DATAFLASH_RESPONSE_DONE(0)                 : Read operation is performed successfully
*       @retval DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 */
DATAFLASH_RESPONSE_TYPE DataFlash__Read(uint8 * destination, ADDRESS_TYPE source, SIZE_TYPE size)
{
    #if 0

    DATAFLASH_RESPONSE_TYPE response;
    FAR uint8 * pflash;
    SIZE_TYPE count;

    response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;

    //Check for the requested size
    if (size > 0)
    {
        #if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)		//For relative address
              source += DATAFLASH_LOW_LIMIT;
		#endif

        // Check for the memory boundaries
        if ((((uint32)destination >= RAM_LOW_LIMIT) &&
        		(((uint32)destination + (uint32)size - (uint32)1)<= (RAM_HIGH_LIMIT))) &&
                ((source >= DATAFLASH_LOW_LIMIT) &&
                ((source + size - 1) <= (DATAFLASH_HIGH_LIMIT))))
            {

                pflash = (FAR uint8 *)source;

                for (count = 0; count < size; count++)
                {
                    #if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)
                            if(((pflash + count) == ((uint8 *)(DATAFLASH_HIGH_LIMIT + 1))))
                            {
                                destination = destination + count;
                                size = size - count;
                                count = 0;
                            }
                    #endif

                    destination[count] = pflash[count];
                }

                response = DATAFLASH_RESPONSE_DONE;
            }
    }
    else
    {
        response = DATAFLASH_RESPONSE_INVALID_SIZE;
    }

    return (response);

    #else
        return (DATAFLASH_RESPONSE_INVALID_SIZE);
    #endif
}

#if (FILL_DATA_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 *   @brief    This method fills the Data Flash with specific value as per the size and returns the response of fill operation
 *   @details  This method fills the specific no of bytes of data into the Data Flash with specific value.
 *              Destination address is starting address of data flash where specific value to be filled.
 *              It returns the Data flash response for write operation
 *            Note -  1. Controller supports byte by byte and 1 long word write.
 *                    2. The software design supports for both even and odd size write.
 *
 *    @param    destination : Defines the address of the Data flash where data to be written
 *    @param    size        : Number of bytes to be written
 *    @param    value       : Defines the specific value which should be filled into Data Flash
 *    @return   enum DATAFLASH_RESPONSE_TYPE
 *      @retval DATAFLASH_RESPONSE_DONE(0)                 : If the write operation is complete.
 *      @retval DATAFLASH_RESPONSE_WRITE_ERROR(3)          : If the write operation is unsuccessful.
 *      @retval DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 *      @retval DATAFLASH_RESPONSE_INVALID_SIZE(5)         : If the size is invalid i.e. odd size
 */
DATAFLASH_RESPONSE_TYPE DataFlash__Fill(ADDRESS_TYPE destination, SIZE_TYPE size, uint8 value)
	#if (DATAFLASH_WRITE_BLOCKING_FEATURE == DISABLED)
	{
		DATAFLASH_RESPONSE_TYPE response;
		response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;

		//Check for the requested size
		if (size > 0)
		{
			// Check for the memory boundaries
			//For relative address
			#if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)
					destination += DATAFLASH_LOW_LIMIT;

			#else
				//For Absolute Address
			if ((((uint32) source >= RAM_LOW_LIMIT)	&&
					(((uint32) source + (uint32) size - (uint32) 1)	<= (RAM_HIGH_LIMIT))) &&
					((destination >= DATAFLASH_LOW_LIMIT) &&
					((destination + size - 1) <= (DATAFLASH_HIGH_LIMIT))))
				{
			#endif
					if (DataFlash_State == DATAFLASH_HANDLER_STATE_IDLE)
					{
						DataFlash_Fill = value;                                 // As Dataflash can write 2 bytes a time this will speed up write and erase operation
						DataFlash_Fill <<= 8;
						DataFlash_Fill |= value;
						DataFlash_Flash_Pointer = destination;
						DataFlash_Size = size;
						DataFlash_Status = DATAFLASH_STATE_BUSY;
						DataFlash_State = DATAFLASH_HANDLER_STATE_ERASE_BEFORE_FILL;
						response = DATAFLASH_RESPONSE_ACCEPTED;
					}
					else
					{
						response = DATAFLASH_RESPONSE_ERROR_BUSY;
					}
				}
		else
		{
			response = DATAFLASH_RESPONSE_INVALID_SIZE;
		}

		return (response);
		}

	#else
	{

		 {
			DATAFLASH_RESPONSE_TYPE response;
			FAR uint8 * pflash;
			uint16 temp1;
			uint16 temp2;
			DATAFLASH_STATUS status;
			uint16 count;
			uint16 data;

			response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;
			status = DATAFLASH_COMPLETE;
			temp1 = 0;
			temp2 = 0;
			data = 0;

			#if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)
				destination += DATAFLASH_LOW_LIMIT;
			#endif

			//Check for the requested size
			if (size > 0)
			{
				//For Absolute Address
				if ((destination >= DATAFLASH_LOW_LIMIT) &&
						((destination + (uint32) size - (uint32) 1)	<= DATAFLASH_HIGH_LIMIT))
				{
					pflash = (FAR uint8 *) destination;             //For High byte byte

					//Cortex Flash memory is 2-byte aligned, it writes only on the even aligned address
					//The write address should be even
					SERVICE_WATCHDOG();

					if ((destination % 2) != 0)
					{
						destination = destination - 1;					//For Low byte
						temp2 = *pflash;								//Read the even address byte
						temp1 = value;
						temp1 = temp1 << 8;
						temp1 = temp1 | temp2;							//apply the mask  = Even_Address_Read_Data | New Data Byte

						DataFlashEraseHalfWord(destination);
						status = DataFlashProgramHalfWord((destination), (temp1));//Write the Mask which starts from even address
						size--;
						destination++;
						destination++;
					}

					if ((((destination + size) % 2) != 0))
					{
						destination = destination + size;
						pflash = (FAR uint8 *) destination;						//Read the even address byte
						temp1 = *pflash;
						temp1 = temp1 << 8;
						temp2 = value;
						temp1 = temp1 | temp2;									//apply the mask  = Even_Address_Read_Data | New Data Byte

						destination = destination - 1;
						DataFlashEraseHalfWord(destination);
						status = DataFlashProgramHalfWord((destination), (temp1));//Write the Mask which starts from even address
						size--;
						destination = destination - size;
					}

					SERVICE_WATCHDOG();

					data = value;
					data <<= 8;
					data |= value;
					count = 0;

					//Writing the halfword byte in the flash memory at the end
					count = 0;
					while ((count <= size))  										//Writing the word data in the flash memory
					{
						DataFlashEraseHalfWord(destination);
						status = DataFlashProgramHalfWord(destination, data);
						destination += 2;
						count += 2;
					}

					if (status == DATAFLASH_COMPLETE)
					{
						response = DATAFLASH_RESPONSE_DONE;
					}
					else if (status == DATAFLASH_ERROR_WRP)
					{
						response = DATAFLASH_RESPONSE_WRITE_ERROR;
					}
					else if (status == DATAFLASH_ERROR_PROGRAM)
					{
						response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;
					}
					else
					{
						response = DATAFLASH_RESPONSE_ERROR_BUSY;
					}

				}
			}

			else
			{
				response = DATAFLASH_RESPONSE_INVALID_SIZE;
			}

			return (response);

		}
	}
	#endif

#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs erase/format operation on particular page.
 *    @param    page_init :  address within page which to be formatted.
 *
 *    @note     For Freescale Kinetis MKE02x their is EEPROM of 256 byte it means only one page
 *              This page is divided in 128 sectors each sector is of 2 bytes
 *              so at a time erase of one sector is possible i.e 2 bytes
 *              Erase supports only Even addressing
 *
 *    @note For MKE02x EEROM memory is 256 bytes which is divided into 128 sectors
 *   			1 sector = 2bytes
 *   			To erase 1 sector = 5.05 msec minimum time is required.
 *
 *    @return   enum DATAFLASH_RESPONSE_TYPE
 *      @retval DATAFLASH_RESPONSE_DONE(0)                 : If the write operation is complete.
 *      @retval DATAFLASH_RESPONSE_WRITE_ERROR(3)          : If the write operation is unsuccessful.
 *      @retval DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 */
DATAFLASH_RESPONSE_TYPE DataFlash__FormatPage(ADDRESS_TYPE page_init)
{
    #if 0
	DATAFLASH_RESPONSE_TYPE response;
	response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;
	DataFlash_Status = DATAFLASH_STATE_BUSY;

	#if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)
		page_init += DATAFLASH_LOW_LIMIT;
    #endif

	MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

	if ((page_init >= DATAFLASH_LOW_LIMIT)&& (page_init <= DATAFLASH_HIGH_LIMIT))
	{
		response = DATAFLASH_RESPONSE_ERROR_BUSY;
		MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

		//Check Flash Memory controller is idle or busy executing a flash command (CCIF = 0)
		if ((FTMRE.FSTAT.BYTE & FTMRE_FSTAT_MGBUSY_MASK) == FALSE)
		{
			response = DATAFLASH_RESPONSE_DONE;
			SERVICE_WATCHDOG();                                      //Refresh Watchdog

			FTMRE.FSTAT.BYTE = 0x30;                                 //Clear error flags
			FTMRE.FCCOBIX.BYTE = 0x00;                               //Write CCOBIX[2:0] index to specify the command code to be loaded
			FTMRE.FCCOBHI = DATAFLASH_ERASE_SECTOR;                  //Program eeprom command
			FTMRE.FCCOBLO = page_init >> 16;                         //Memory address bits [23:16]
			FTMRE.FCCOBIX.BYTE = 0x01;                               //Write CCOBIX[2:0] index to specify the lower byte memory address bits[15:0] to be loaded
			FTMRE.FCCOBHI = page_init >> 8;                          //Memory address bits [7:0]
			FTMRE.FCCOBLO = page_init;                               //Memory address bits [15:8]
			DataFlash_Launchcmd(TRUE);                               //Launch the command

			//Check error status
			if (FTMRE.FSTAT.BYTE & FTMRE_FSTAT_ACCERR_MASK)
			{
				response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;	 //Access error id detected
			}

			if (FTMRE.FSTAT.BYTE & FTMRE_FSTAT_FPVIOL_MASK)
			{
				response = DATAFLASH_RESPONSE_WRITE_ERROR;			 //Protection violation is detected
			}

			if (FTMRE.FSTAT.BYTE & FTMRE_FSTAT_MGSTAT_MASK)
			{
				response = DATAFLASH_RESPONSE_ERROR_BUSY;
			}

			//Update Flash_Status
			if (response == DATAFLASH_RESPONSE_DONE)
			{
				DataFlash_Status = DATAFLASH_STATE_FREE;
			}
			else
			{
				DataFlash_Status = DATAFLASH_STATE_ERROR;
			}

			MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

		}
	}

	return (response);

    #else
        return (DATAFLASH_RESPONSE_INVALID_SIZE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    DataFlash Handler
 *    @details  N/A
 */
void DataFlash__Handler(void)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method provides status of Data flash whether busy/free/error
 *    @return   enum DATAFLASH_STATE_TYPE
 *    @retval   DATAFLASH_STATE_BUSY    : Data Flash is busy for read write
 *              DATAFLASH_STATE_FREE    : Data Flash is free for read write
 *              DATAFLASH_STATE_ERROR   : Data Flash is encountered error condition
 */
DATAFLASH_STATE_TYPE DataFlash__GetStatus(void)
{
	return (DataFlash_Status);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------


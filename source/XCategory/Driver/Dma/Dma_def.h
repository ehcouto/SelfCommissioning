/**
 *  @file       
 *
 *  @brief      Controller specific definition for Dma
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: Dma_def.h 1.3 2013/06/04 09:56:10IST Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2013/06/04 09:56:10IST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DMA_DEF_H_
#define DMA_DEF_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * It defines the type of the address variable
 */
#define DMA_ADDRESS_TYPE                     uint32

/**
 * It defines the type of the size variable
 */
#define DMA_SIZE_TYPE                        uint32


typedef enum
{
    DMA_DATA_DIRECTION_PERIPH_TO_MEM,
    DMA_DATA_DIRECTION_MEM_TO_PERIPH,
    DMA_DATA_DIRECTION_MEM_TO_MEM,
    DMA_DATA_DIRECTION_PERIPH_TO_PERIPH
}DMA_DATA_DIRECTION_TYPE;

typedef enum
{
    DMA_ADDRESS_INCREMENT_DISABLE,
    DMA_ADDRESS_INCREMENT_FOR_PERIPH_ADDRESS,
    DMA_ADDRESS_INCREMENT_FOR_MEM_ADDRESS,
    DMA_ADDRESS_INCREMENT_FOR_PERIPH_MEM_ADDRESS
}DMA_ADDRESS_INCREMENT_TYPE;

typedef enum
{
    DMA_DATA_WIDTH_BYTE,
    DMA_DATA_WIDTH_HALFWORD,
    DMA_DATA_WIDTH_WORD
}DMA_DATA_WIDTH_TYPE;

typedef enum
{
    DMA_MODE_NORMAL,
    DMA_MODE_CIRCULAR
}DMA_MODE_TYPE;

typedef enum
{
    DMA_PRIORITY_LOW,
    DMA_PRIORITY_MEDIUM,
    DMA_PRIORITY_HIGH,
    DMA_PRIORITY_VERYHIGH
}DMA_PRIORITY_TYPE;

/**
  * @brief  DMA Init structures definition
  */
typedef struct
{
    DMA_ADDRESS_TYPE             DMA_PeriBaseAddr;   // Specifies the peripheral base address for DMAy Channelx.
    DMA_ADDRESS_TYPE             DMA_MemBaseAddr;    // Specifies the memory base address for DMAy Channelx.
    DMA_DATA_DIRECTION_TYPE      DMA_DIR;            // Specifies if the peripheral is the source or destination.
    DMA_SIZE_TYPE                DMA_BufferSize;     // Specifies the buffer size, in data unit, of the specified Channel.
    DMA_ADDRESS_INCREMENT_TYPE   DMA_AddressInc;     // Specifies whether the Peripheral address register is incremented or not.
    DMA_DATA_WIDTH_TYPE          DMA_PeriDataSize;   // Specifies the Peripheral data width.
    DMA_DATA_WIDTH_TYPE          DMA_MemDataSize;    // Specifies the Memory data width.
    DMA_MODE_TYPE                DMA_Mode;           // Specifies the operation mode of the DMAy Channelx The circular buffer mode cannot be used if the memory-to-memory
    DMA_PRIORITY_TYPE            DMA_Priority;       // Specifies the software priority for the DMAy Channelx.
    uint8                        DMA_Mux_Source;
}DMA_INIT_TYPE;;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#endif // DMA_DEF_H_



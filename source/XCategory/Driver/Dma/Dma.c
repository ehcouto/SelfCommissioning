/**
 *  @file       
 *
 *  @brief      Direct memory access (DMA) is used in order to provide high-speed data transfer between
 *              peripherals and memory as well as memory to memory. Data can be quickly moved by DMA
 *              without any CPU actions. This keeps CPU resources free for other operations.
 *
 *  @details    MKV03x supports 16 channels
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "uc.h"
#include "Micro.h"
#include "Dma.h"
#include "Dma_def.h"
#include "Dma_prm.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define DMA_MAX_CHANNEL					15				//DMA Channel from DMA0 to DMA15

#define DMA1_MAX_ENGINE					1
//Dummy Dma channel is created to match the index of the Cortex-M3 hardware Dma channel with 'DMA_CHANNEL_TYPE'
#define DMA1_CHANNEL0					(*(DMA1_CHANNEL_DEF *) NULL)


#define CCR_CLEAR_MASK					((uint32)0xFFFF800F) // DMA Channel configuration registers Masks

//This array holds all the Dma channel. Cortex-M3 doesn't consists of Dma channel 0. In-order to match the index of hardware Dma channel with 'DMA_CHANNEL_TYPE'
// a DMA1_CHANNEL0 is added at the beginning of the array
//static DMA1_CHANNEL_DEF * const DMA_CHANNEL[DMA_MAX_CHANNEL+1] =
//                                                    {   &DMA1_CHANNEL0,
//                                                        &DMA1_CHANNEL1,
//                                                        &DMA1_CHANNEL2,
//                                                        &DMA1_CHANNEL3,
//                                                        &DMA1_CHANNEL4,
//                                                        &DMA1_CHANNEL5
//                                                    };

#define FSL_FEATURE_EDMA_MODULE_CHANNEL                16
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DMA and its variables.
 *  @details    The engine is not enabled if its previously enabled to prevent the reset of the configuration.
 *
 */
void Dma__EngineInitialize(void)
 {

	uint8 i;

	//Enable the DMA device in the AHB bus
	SIM.SCGC6 |= SIM_SCGC6_DMAMUX_MASK;									//Enable DMAMUX
	SIM.SCGC7 |= SIM_SCGC7_DMA_MASK;									//Enable DMA clock

	/* Init eDMA module in hardware level. */
	DMA0->CR = 0x00000000;												//Clear all the flags

	for (i = 0; i < FSL_FEATURE_EDMA_MODULE_CHANNEL; i++)
	{
		//Clear all TCDn channel field
		DMA0->TCD[i].SADDR = 0x00000000;
		DMA0->TCD[i].SOFF = 0x0000;
		DMA0->TCD[i].ATTR = 0x0000;
		DMA0->TCD[i].NBYTES_MLNO = 0x0000;
		DMA0->TCD[i].SLAST = 0x00;
		DMA0->TCD[i].DADDR = 0x00;
		DMA0->TCD[i].DOFF = 0x00;
		DMA0->TCD[i].CITER_ELINKNO = 0x00;
		DMA0->TCD[i].DLAST_SGA = 0x00;
        DMA0->TCD[i].CSR = 0;

//		DMA.TCD[i].CSR = DMA_CSR_ESG_MASK;
		DMA0->TCD[i].BITER_ELINKNO = 0x00;
		DMAMUX->CHCFG[i] = 0x00;
	}

	//Set Channel Priority
	DMA0->DCHPRI0 = DMA_CHPRI0 << DMA_DCHPRI0_CHPRI_SHIFT;
	DMA0->DCHPRI1 = DMA_CHPRI1 << DMA_DCHPRI1_CHPRI_SHIFT;
	DMA0->DCHPRI2 = DMA_CHPRI2 << DMA_DCHPRI2_CHPRI_SHIFT;
	DMA0->DCHPRI3 = DMA_CHPRI3 << DMA_DCHPRI3_CHPRI_SHIFT;
	DMA0->DCHPRI4 = DMA_CHPRI4 << DMA_DCHPRI4_CHPRI_SHIFT;
	DMA0->DCHPRI5 = DMA_CHPRI5 << DMA_DCHPRI5_CHPRI_SHIFT;
	DMA0->DCHPRI6 = DMA_CHPRI6 << DMA_DCHPRI6_CHPRI_SHIFT;
	DMA0->DCHPRI7 = DMA_CHPRI7 << DMA_DCHPRI7_CHPRI_SHIFT;
	DMA0->DCHPRI8 = DMA_CHPRI8 << DMA_DCHPRI8_CHPRI_SHIFT;
	DMA0->DCHPRI9 = DMA_CHPRI9 << DMA_DCHPRI9_CHPRI_SHIFT;
	DMA0->DCHPRI10 = DMA_CHPRI10 << DMA_DCHPRI10_CHPRI_SHIFT;
	DMA0->DCHPRI11 = DMA_CHPRI11 << DMA_DCHPRI11_CHPRI_SHIFT;
	DMA0->DCHPRI12 = DMA_CHPRI12 << DMA_DCHPRI12_CHPRI_SHIFT;
	DMA0->DCHPRI13 = DMA_CHPRI13 << DMA_DCHPRI13_CHPRI_SHIFT;
	DMA0->DCHPRI14 = DMA_CHPRI14 << DMA_DCHPRI14_CHPRI_SHIFT;
	DMA0->DCHPRI15 = DMA_CHPRI15 << DMA_DCHPRI15_CHPRI_SHIFT;
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Initializes the Dma channel according to the specified parameters
  *         in the DMA_InitStruct.
  * @param  channel: Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  * @param  DMA_InitStruct: pointer to a DMA_InitTypeDef structure that contains
  *         the configuration information for the specified DMA Channel.
  * @retval DMA_RETURN_TYPE
 *      @retval DMA_RETURN_FAIL(0)      : Dma channel was fail to initialize
 *      @retval DMA_RETURN_SUCCESS(1)   : Dma channel was successfully initialized
  */
DMA_RETURN_TYPE Dma__Initialize(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel, DMA_INIT_TYPE* DMA_InitStruct)
{

#if 0
    DMA_RETURN_TYPE ret;

    ret = DMA_RETURN_FAIL;

    if ((channel <= DMA_MAX_CHANNEL) && (channel))
    {
		//Set the channel in Arbitration or pripority mode
		#if (CHANNEL_ARBITRATION == 1)
			DMA.CR |= DMA_CR_ERCA_MASK;									//Round Robin Arbitration is used for channel selection
		#else
			DMA.CR &= ~(DMA_CR_ERCA_MASK); 								//Fixed Priority Arbitration is used for channel selection
		#endif

		//Clear the source and enable bit
        Dma__Disable(dma_engine, channel);

		DMA.EEI = 0x00000000;											//Enable Error Interrupt register - '0'
		DMA.CDNE |= DMA_CDNE_NOP_MASK;									//No operation, ignore the other bits in this DMA_CDNE register
		DMA.SSRT |= DMA_SSRT_NOP_MASK;									//No operation, ignore the other bits in this DMA_SSRT register




//        DMA.TCD[channel].BITER_ELINKNO = 4;
//        DMA.TCD[channel].CITER_ELINKNO = 4;
//        DMA.TCD[channel].NBYTES_MLNO = 0;                               // Write to DMA Channel Counter
//        DMA.TCD[channel].SADDR = DMA_InitStruct->DMA_MemBaseAddr;       // Write to DMA Channel SADDR
//        DMA.TCD[channel].SOFF = 1;
//
////      if (DMA_InitStruct->DMA_PeriDataSize == DMA_DATA_WIDTH_HALFWORD)
//        {
//            DMA.TCD[channel].ATTR |= 0 << DMA_ATTR_SSIZE_SHIFT;
//        }
////      else if (DMA_InitStruct->DMA_PeriDataSize == DMA_DATA_WIDTH_WORD)
////      {
////          DMA.TCD[channel].ATTR |= 1 << DMA_ATTR_SSIZE_SHIFT;
////      }
//        DMA.TCD[channel].SLAST = 1;
//
//        DMA.TCD[channel].DADDR = DMA_InitStruct->DMA_PeriBaseAddr;      // Write to DMA Channel DADDR
//        DMA.TCD[channel].DOFF = 0;
//
////      if (DMA_InitStruct->DMA_PeriDataSize == DMA_DATA_WIDTH_HALFWORD)
//        {
//            DMA.TCD[channel].ATTR |= 0 << DMA_ATTR_DSIZE_SHIFT;
//        }
////      else if (DMA_InitStruct->DMA_PeriDataSize == DMA_DATA_WIDTH_WORD)
////      {
////          DMA.TCD[channel].ATTR |= 1 << DMA_ATTR_DSIZE_SHIFT;
////      }
//
//        DMAMUX.CHCFG[channel].BIT.SOURCE = DMA_InitStruct->DMA_Mux_Source;
//
//        DMA.TCD[channel].DLAST_SGA = 1;
//        ret = DMA_RETURN_SUCCESS;
//
//
//
//

//		if (DMA_DATA_DIRECTION_PERIPH_TO_MEM)
//		{
//	        DMA_SADDR_REG(DMA_BASE_PTR, channel)          = DMA_InitStruct->DMA_PeriBaseAddr;                  // Source address
//	        DMA_DADDR_REG(DMA_BASE_PTR, channel)          = DMA_InitStruct->DMA_MemBaseAddr;                   // Destination address
//		}
//		else
//		{
//            DMA_SADDR_REG(DMA_BASE_PTR, channel)          = DMA_InitStruct->DMA_MemBaseAddr;                    // Source address
//            DMA_DADDR_REG(DMA_BASE_PTR, channel)          = DMA_InitStruct->DMA_PeriBaseAddr;                   // Destination address
//		}
//
//
//        DMA_SOFF_REG(DMA_BASE_PTR, channel)           = 1;                                                  // Source address increments by number of bytes per transfer
//        DMA_SLAST_REG(DMA_BASE_PTR, channel)          = 0;                                                  // After the major loop ends, reset pointer to beginning of buffer
//        DMA_DOFF_REG(DMA_BASE_PTR, channel)           = 0x0;                                                // Destination offset increments 0 bytes (uint32)
//        DMA_DLAST_SGA_REG(DMA_BASE_PTR, channel)      = 0;                                                  // Destination address shift
//        DMA_NBYTES_MLNO_REG(DMA_BASE_PTR, channel)    = 0;                                                  // The minor loop moves 32 bytes per transfer
//        DMA_BITER_ELINKNO_REG(DMA_BASE_PTR, channel)  = 1;                                                  // Major loop iterations
//        DMA_CITER_ELINKNO_REG(DMA_BASE_PTR, channel)  = 1;                                                  // Set current iteration count
//        DMA_ATTR_REG(DMA_BASE_PTR, channel)           = (DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(0));            // Source a destination size 0:8bit; 1:16-bit, 2:32-bit, 5:16-byte, 6:32-byte
//        //DMA_CSR_REG(DMA_BASE_PTR, channel)            = DMA_CSR_INTMAJOR_MASK | DMA_CSR_DREQ_MASK;        // Enable end of loop DMA interrupt; clear ERQ @ end of major iteration
//
//        // Configure the DMAMUX to link the desired DMA channel to the peripheral request
//        DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,channel)    = (DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(DMA_InitStruct->DMA_Mux_Source));     // DMA source DMA Mux


            if (DMA_InitStruct->DMA_DIR == DMA_DATA_DIRECTION_PERIPH_TO_MEM)
            {
               DMA_SADDR_REG(DMA_BASE_PTR, channel)          = DMA_InitStruct->DMA_PeriBaseAddr;                  // Source address
               DMA_DADDR_REG(DMA_BASE_PTR, channel)          = DMA_InitStruct->DMA_MemBaseAddr;                   // Destination address
               DMA_SOFF_REG(DMA_BASE_PTR, channel)           = 0;                                                  // Source address increments by number of bytes per transfer
               DMA_SLAST_REG(DMA_BASE_PTR, channel)          = 0;                                                  // After the major loop ends, reset pointer to beginning of buffer
               DMA_DOFF_REG(DMA_BASE_PTR, channel)           = 1;                                                  // Destination offset increments 0 bytes (uint32)
               DMA_DLAST_SGA_REG(DMA_BASE_PTR, channel)      = 0;                                                  // Destination address shift
            }
            else
            {
                DMA_SADDR_REG(DMA_BASE_PTR, channel)          = DMA_InitStruct->DMA_MemBaseAddr;                    // Source address
                DMA_DADDR_REG(DMA_BASE_PTR, channel)          = DMA_InitStruct->DMA_PeriBaseAddr;                   // Destination address
                DMA_SOFF_REG(DMA_BASE_PTR, channel)           = 1;                                                  // Source address increments by number of bytes per transfer
                DMA_DOFF_REG(DMA_BASE_PTR, channel)           = 0;                                                  // Destination offset increments 0 bytes (uint32)
                DMA_SLAST_REG(DMA_BASE_PTR, channel)          = 0;                                                  // After the major loop ends, reset pointer to beginning of buffer
                DMA_DLAST_SGA_REG(DMA_BASE_PTR, channel)      = 0;                                                  // Destination address shift
            }

            DMA_NBYTES_MLNO_REG(DMA_BASE_PTR, channel)    = 0;                                                  // The minor loop moves 32 bytes per transfer
            DMA_BITER_ELINKNO_REG(DMA_BASE_PTR, channel)  = 1;                                                  // Major loop iterations

            DMA_BITER_ELINKNO_REG(DMA_BASE_PTR, channel)  = DMA_BITER_ELINKNO_BITER(1);                             // Major loop iterations
            DMA_CITER_ELINKNO_REG(DMA_BASE_PTR, channel)  = DMA_CITER_ELINKNO_CITER(1);                             // Set current iteration count
            DMA_ATTR_REG(DMA_BASE_PTR, channel)           = (DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(0));            // Source a destination size 0:8bit; 1:16-bit, 2:32-bit, 5:16-byte, 6:32-byte

            //DMA_CSR_REG(DMA_BASE_PTR, channel)            = DMA_CSR_INTMAJOR_MASK | DMA_CSR_DREQ_MASK;        // Enable end of loop DMA interrupt; clear ERQ @ end of major iteration

            // Configure the DMAMUX to link the desired DMA channel to the peripheral request
//            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,channel)    = (DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(DMA_InitStruct->DMA_Mux_Source));     // DMA source DMA Mux
            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,channel)    = (DMAMUX_CHCFG_SOURCE(DMA_InitStruct->DMA_Mux_Source));     // DMA source DMA Mux
            ret = DMA_RETURN_SUCCESS;
	}

    return (ret);

#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    Starts the Dma transfer.
 * @details  Before calling this Channel should be initialized by calling Dma__Initialize() API
 * @param    channel: Defines the specific Dma channel from the DMA_CHANNEL_TYPE
 * @param    DataNumber  : Number of data to be transfer
 * @return   BOOL_TYPE
 *      @retval TRUE : Dma data transfer started.
 *      @retval FALSE: Dma data transfer failed.
 */
BOOL_TYPE Dma__StartTransfer(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel, DMA_SIZE_TYPE DataNumber)
{

#if 0
    BOOL_TYPE status;

    status = FALSE;

    if ((channel <= DMA_MAX_CHANNEL) && (channel))
    {

			if (channel == DMA_CH5)
			{
				DMA.TCD[channel].SLAST = -DataNumber;   //it uses 2's complement
                DMA.TCD[channel].DLAST_SGA = -0; //it uses 2's complement
			}
			if (channel == DMA_CH4)
			{
                DMA.TCD[channel].SLAST = -0;   //it uses 2's complement
				DMA.TCD[channel].DLAST_SGA = -DataNumber; //it uses 2's complement
			}

			DMA.TCD[channel].NBYTES_MLNO = DataNumber; 				//Set the data size to be transfered

//            DMA_BITER_ELINKNO_REG(DMA_BASE_PTR, channel)  = DMA_BITER_ELINKNO_BITER(1);                             // Major loop iterations
//            DMA_CITER_ELINKNO_REG(DMA_BASE_PTR, channel)  = DMA_CITER_ELINKNO_CITER(1);                             // Set current iteration count

			Dma__Enable(dma_engine,channel);
            DMA.TCD[channel].CSR |= DMA_CSR_START_MASK;             // Software Trigger to initiate the transfer

			status = TRUE;
	}

      return (status);

#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    Stops the Dma transfer.
 * @details
 * @param  channel: Defines the specific Dma channel from the DMA_CHANNEL_TYPE
 */
void Dma__StopTransfer(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
#if 0
	if ((channel <= DMA_MAX_CHANNEL) && (channel))
	{
		DMA.TCD[channel].CSR &= (~DMA_CSR_START_MASK); 		//Disable the DMA channel to initiate the new transfer
		DMA.TCD[channel].NBYTES_MLNO = 0; 					//Set the data size to be transfered
	}
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Enables the specified DMA channel
  * @param  channel: Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  */
void Dma__Enable(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
#if 0
    if ((channel <= DMA_MAX_CHANNEL) &&
        (channel))
    {
    	DMA.SERQ = channel & DMA_SERQ_SERQ_MASK;
//    	DMA.ERQ |= 1<< channel;           		//The DMA request signal for the corresponding channel is enabled
//    	DMA.INT |= 1<< channel;

        DMAMUX.CHCFG[channel].BIT.ENBL = TRUE;
    }

#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Disables the specified DMAy Channelx.
  * @param  channel: Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  */
void Dma__Disable(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
#if 0
    if ((channel <= DMA_MAX_CHANNEL) && (channel))
    {
    	DMAMUX.CHCFG[channel].BIT.ENBL = FALSE;
    	DMA.ERQ |= 0<< channel;              //The DMA request signal for the corresponding channel is disabled
    }

#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Returns the number of remaining data units in the current
  *         DMAy Channelx transfer.
  * @param  channel: Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  * @retval The number of remaining data units in the current DMAy Channelx
  *         transfer.
  */
DMA_SIZE_TYPE Dma__GetCurrDataCounter(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
 {
#if 0

	DMA_SIZE_TYPE ret;

	ret = 0;

	if ((channel <= DMA_MAX_CHANNEL) && (channel))
	{
		//Return the number of remaining data units for DMAy Channelx
		return ((DMA_SIZE_TYPE) (DMA.TCD[channel].NBYTES_MLNO));
	}
	else
	{
		return (ret);
	}
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Returns the status of the specified Dma channel.
  * @param  channel: Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  * @retval DMA_TRANSFER_STATUS_TYPE-
  *      @retval DMA_TRANSFER_IDLE      : Dma channel is in idle state
  *      @retval DMA_TRANSFER_ERROR     : Dma channel encountered data transfer error
  *      @retval DMA_TRANSFER_ONGOING   : Dma channel is transferring data
  *      @retval DMA_TRANSFER_COMPLETE  : Dma channel completed data transfer
  */
DMA_TRANSFER_STATUS_TYPE Dma__GetFlagStatus(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{

#if 0
    DMA_TRANSFER_STATUS_TYPE status;
    uint16 csr_status;

    status = DMA_TRANSFER_IDLE;
    csr_status = 0;
    csr_status = DMA.TCD[channel].CSR;

    if ((channel <= DMA_MAX_CHANNEL) && (channel))
    {
//        if ((DMA_CHANNEL[channel]->CNDTR) != 0)
   	  if((csr_status & DMA_CSR_ACTIVE_MASK) != 0)		//Check Channel is currently in execution
        {
            status = DMA_TRANSFER_ONGOING;
        }
        // Check the status of the specified DMA flag
//        else if ((DMA1.ISR & (DMA_ISR_TEIF1 <<channel*4)) != FALSE)
//        {
//            status = DMA_TRANSFER_ERROR;
//        }
        else if ((csr_status & DMA_CSR_DONE_MASK) != FALSE)		//eDMA has completed the major loop
        {
            status = DMA_TRANSFER_COMPLETE;
        }

        //Clear all the flags
        DMA.TCD[channel].CSR =0x0000;
    }

    // Return the DMA_FLAG status
    return  status;

#endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



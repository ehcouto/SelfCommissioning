/**
 *  @file
 *
 *  @brief      This module handles the Comparator channel
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *
 *  @copyright  Copyright 2016- Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Comparator.h"
#include "uc_MKV3x.h"
#include "SRMicro.h"
#include "Gpio.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the  Comparator module and its variables
 *
 */
void Comparator__Initialize(void)
{
	SIM.SCGC4 |= SIM_SCGC4_CMP_MASK;		      //Clock Enabled for Comparator module
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This module configures the Comparator channel into desired modes
 * @param channel
 * @param inverted_input - cmp positive input selection
 * @param noninverted_input - cmp negative input selection
 * @param mode - comparator modes
 * @return
 */
PASS_FAIL_TYPE Comparator__Config(COMPARATOR_CHANNEL_DEF channel,COMPARATOR_INVERTED_INPUT_DEF inverted_input,COMPARATOR_NONINVERTED_INPUT_DEF noninverted_input,COMPARATOR_MODE_DEF mode, uint16 cmp_thr)
 {
	PASS_FAIL_TYPE response;

	response = FAIL;
	#if (COMPARATOR0_FEATURE == ENABLED)

		if (channel == COMPARATOR_CHANNEL0)
		{
		    //For Comparator 0 Decide Output Pin
            #if(COMPARATOR0_REMAP == 0)
                PCIPORTA.PCR[1] = PORT_PCR_MUX(4);              //PTA pin 1 - Comparartor Output CMPOUT0
            #endif

		#if(COMPARATOR0_REMAP == 1)
			PCIPORTB.PCR[20] = PORT_PCR_MUX(6);				//PTB pin 20 - Comparartor Output CMPOUT0
		#endif

		#if(COMPARATOR0_REMAP == 2)
			PCIPORTC.PCR[5] = PORT_PCR_MUX(6);				//PTC pin 5 - Comparartor Output CMPOUT0
		#endif

		if ((inverted_input == COMPARATOR_INVERTED_INPUT7 || noninverted_input == COMPARATOR_NONINVERTED_INPUT7))
		{
			CMP0.DACCR.BIT.DACEN = 1;
			CMP0.DACCR.BIT.VRSEL = CMP0_SUPPLY_REFERENCE_SELECT;
			CMP0.DACCR.BIT.VOSEL = cmp_thr;
		}

			CMP0.MUXCR.BIT.MSEL = inverted_input;				//Configure Plus Input of the comparator
			CMP0.MUXCR.BIT.PSEL = noninverted_input;				//Configure Minus Input of the comparator

			switch (mode)
			{
				case COMPARATOR_MODE_DISABLE:
					CMP0.CR1.BIT.EN = 0;
					break;

				case COMPARATOR_MODE_CONTINUOUS:
					CMP0.CR1.BIT.WE = 0;
					CMP0.CR1.BIT.SE = 0;
					CMP0.CR0.BIT.FILTER_CNT = COMPARATOR0_FILTER_CNT;
					if (COMPARATOR0_FILTER_CNT == 0)			//For Mode 2A Continuous Mode
					{
						CMP0.FPR.BYTE = 1;
					}
					else								//For Mode 2B Continuous Mode
					{
						CMP0.FPR.BYTE = 0;
					}

					break;

				case COMPARATOR_MODE_SAMPLED_NONFILTERED:	//For Mode 3A Sampled Enabled use External Clock
				case COMPARATOR_MODE_SAMPLED_FILTERED:
					CMP0.CR1.BIT.WE = 0;
					CMP0.CR1.BIT.SE = COMPARATOR0_SAMPLE_ENABLED;
					CMP0.CR0.BIT.FILTER_CNT = COMPARATOR0_FILTER_CNT;
					CMP0.FPR.BYTE = COMPARATOR0_FPR;
					break;

				case COMPARATOR_MODE_WINDOWED:
				case COMPARATOR_MODE_WINDOWED_RESAMPLED:
				case COMPARATOR_MODE_WINDOWED_FILTERED:
					CMP0.CR1.BIT.WE = 1;
					CMP0.CR1.BIT.SE = COMPARATOR0_SAMPLE_ENABLED;
					CMP0.CR0.BIT.FILTER_CNT = COMPARATOR0_FILTER_CNT;
					CMP0.FPR.BYTE = COMPARATOR0_FPR;
					break;

				default:
					break;
			}

			CMP0.CR1.BIT.OPE = 1;					//Enable Comparator Output Pin
			CMP0.CR1.BIT.EN = 1;					//Enable Comparator Module
			response = PASS;

		}


	#endif

	#if(COMPARATOR1_FEATURE == ENABLED)

		if(channel == COMPARATOR_CHANNEL1)
		{
		    //For Comparator 1 Decide Output Pin
            #if(COMPARATOR1_REMAP == 0)
                PCIPORTA.PCR[2] = PORT_PCR_MUX(4);              //PTA pin 2 - Comparartor Output CMPOUT1
            #endif

		#if(COMPARATOR1_REMAP == 1)
			PCIPORTB.PCR[21] = PORT_PCR_MUX(6);				//PTB pin 21 - Comparartor Output CMPOUT1
		#endif

		#if(COMPARATOR1_REMAP == 2)
			PCIPORTC.PCR[4] = PORT_PCR_MUX(6);				//PTC pin 4 - Comparartor Output CMPOUT1
		#endif

		if((inverted_input == COMPARATOR_INVERTED_INPUT7 || noninverted_input == COMPARATOR_NONINVERTED_INPUT7))
		{
			CMP1.DACCR.BIT.DACEN = 1;
			CMP1.DACCR.BIT.VRSEL = CMP1_SUPPLY_REFERENCE_SELECT;
			CMP1.DACCR.BIT.VOSEL= cmp_thr;
		}

			CMP1.MUXCR.BIT.MSEL = inverted_input;				//Configure Plus Input of the comparator
			CMP1.MUXCR.BIT.PSEL = noninverted_input;				//Configure Minus Input of the comparator

			switch (mode)
			{
				case COMPARATOR_MODE_DISABLE:
					CMP1.CR1.BIT.EN = 0;
					break;

				case COMPARATOR_MODE_CONTINUOUS:
					CMP1.CR1.BIT.WE = 0;
					CMP1.CR1.BIT.SE = 0;
					CMP1.CR0.BIT.FILTER_CNT = COMPARATOR1_FILTER_CNT;
					if (COMPARATOR1_FILTER_CNT == 0)			//For Mode 2A Continuous Mode
					{
						CMP1.FPR.BYTE = 1;
					}
					else								//For Mode 2B Continuous Mode
					{
						CMP1.FPR.BYTE = 0;
					}

					break;

				case COMPARATOR_MODE_SAMPLED_NONFILTERED:	//For Mode 3A Sampled Enabled use External Clock
				case COMPARATOR_MODE_SAMPLED_FILTERED:
					CMP1.CR1.BIT.WE = 0;
					CMP1.CR1.BIT.SE = COMPARATOR1_SAMPLE_ENABLED;
					CMP1.CR0.BIT.FILTER_CNT = COMPARATOR1_FILTER_CNT;
					CMP1.FPR.BYTE = COMPARATOR1_FPR;
					break;

				case COMPARATOR_MODE_WINDOWED:
				case COMPARATOR_MODE_WINDOWED_RESAMPLED:
				case COMPARATOR_MODE_WINDOWED_FILTERED:
					CMP1.CR1.BIT.WE = 1;
					CMP1.CR1.BIT.SE = COMPARATOR1_SAMPLE_ENABLED;
					CMP1.CR0.BIT.FILTER_CNT = COMPARATOR1_FILTER_CNT;
					CMP1.FPR.BYTE = COMPARATOR1_FPR;
					break;

				default:
					break;

			}

			CMP1.CR1.BIT.OPE = 1;					//Enable Comparator Output Pin
			CMP1.CR1.BIT.EN = 1;					//Enable Comparator Module
			response = PASS;


		}
	#endif

return (response);
}

//-----------------------------------------------------------------------------------------------------------
/*
 *
 * @brief This method used for checking the comparator channel output status
 * @param: channel
 * @return: status of comparator
 *
 */
uint8 Comparator__GetStatus(COMPARATOR_CHANNEL_DEF channel)
{
	uint8 status;
	status = 0;
	if (channel == COMPARATOR_CHANNEL0)
	{
		if (CMP0.SCR.BIT.COUT == 1)
		{
			status = CMP0.SCR.BIT.COUT;
		}
	}

	else if (channel == COMPARATOR_CHANNEL1)
	{
		if (CMP1.SCR.BIT.COUT == 1)
		{
			status = CMP1.SCR.BIT.COUT;
		}
	}
	return (status);
}


//-----------------------------------------------------------------------------------------------------------
/*
 *
 * @brief This method used to set the comparator threshold
 * @param: channel
 * @return: status of comparator
 *
 */
void Comparator__SetThr(COMPARATOR_CHANNEL_DEF channel, uint16 cmp_thr)
{

	if (channel == COMPARATOR_CHANNEL0)
	{
		CMP0.DACCR.BIT.VOSEL = cmp_thr;
	}

	else if (channel == COMPARATOR_CHANNEL1)
	{
		CMP1.DACCR.BIT.VOSEL = cmp_thr;
	}
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 *  @file       SRMCInputCapture_prm.h
 *  @defgroup   CLASS_B
 *  @brief      SRInputCapture module Api parameters.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SOURCE_PROJECTSPECIFIC_DRIVER_MCCONFIG_SRMCINPUTCAPTURE_PRM_H_
#define SOURCE_PROJECTSPECIFIC_DRIVER_MCCONFIG_SRMCINPUTCAPTURE_PRM_H_

#include "Mci_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#if((WINDY_STRIP_BOARD == 1)||(KV31_EVALUATION_BOARD == 1)|| (WINDY_INTERNATION_BOARD == 1))

	#define IC_MODULE_0 FTM0
	#define IC_MODULE_1 FTM1
	#define IC_MODULE_2 FTM2

	#define INPUT_CAPTURE_MODULE_N					  3
	#define INPUT_CAPTURE_CHANNEL_N					  8

	#define INPUTCAPTURE_MODULE                       1   //!< 0 - FTM0; 1 - FTM1; 2 - FTM2
	#define INPUTCAPTURE_CHANNEL                      1   //!< Choose the channel for the selected module.
														  //!< Value from 0 to 7
    #define INPUT_CAPTURE_PS 5
    #define INPUT_CAPTURE_CLOCK (SYSTEM_CLOCK >> (INPUT_CAPTURE_PS + 1))

	#define INPUTCAPTURE_CHANNEL_MAX_OV_TO_NOT_DEFINED 2 //!< Define the number of overflow before declaring
                                                         //!< the input capture has a defect, e.g.,
                                                         //!< sensor cable disconnected
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


#endif // SOURCE_PROJECTSPECIFIC_DRIVER_MCCONFIG_SRMCINPUTCAPTURE_PRM_H_

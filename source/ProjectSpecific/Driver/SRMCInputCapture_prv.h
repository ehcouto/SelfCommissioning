/**
 *  @defgroup CLASS_B
 *  @file       
 *
 *  @brief      Private parameters for the SRMCInputCapture module
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRMCINPUTCAPTURE_PRV_H_
    #define SRMCINPUTCAPTURE_PRV_H_

#include "SRFlow.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 *    @brief  - Call-back for Flow control when the Module is initialized.
 *    @details- The macros should be defined connecting the macro with the SRFlow modules.
 *
 *    @param  - User defined application function prototype to log the Flow control when the module is initialized.
 *
 *    @note  -  "#define SRMICRO_FLOW_LOG_RAM_TEST_BEGIN()    SRFlow__MainLogEvent(SRFLOW_SRRAMTEST_MAIN_BEGIN)"
 */
//=====================================================================================================================
#define SRMCINPUTCAPTURE_FLOW_INITIALIZE_BEGIN()           //SRFlow__InitLogEvent(SRFLOW_SRMCINPUTCAPTURE_INIT_BEGIN)
#define SRMCINPUTCAPTURE_FLOW_INITIALIZE_END()             //SRFlow__InitLogEvent(SRFLOW_SRMCINPUTCAPTURE_INIT_END)


#endif // SRMCINPUTCAPTURE_PRV_H_



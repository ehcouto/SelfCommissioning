/**
 * @brief       Public interface to the API004Debug module.
 *
 * @details     Refer to the API004Debug.c source file for more detailed information.
 *
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _API004DEBUG_H_
#define _API004DEBUG_H_

#if (API004DEBUG_FEATURE == ENABLED)
#include "Reveal.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define API004DEBUG_NUM        4
#define API004DEBUG_TYPE       1
#define API004DEBUG_VERSION    1
#define API004DEBUG_INSTANCES  1


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
REVEAL_RECIPE_STATUS_TYPE API004Debug__CommandParser(REVEAL_MSG_TYPE * buffer);
#endif //#if (API004DEBUG_FEATURE == ENABLED)
#endif //_API004DEBUG_H_



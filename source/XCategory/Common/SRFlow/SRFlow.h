/**
 *  @defgroup   CLASS_B
 *  @file
 *
 *  @brief      Standard API for SRFlow module.
 *
 *
 *  
 *
 *  @copyright  Copyright 2011-. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRFLOW_H_
	#define SRFLOW_H_

#include "C_Extensions.h"
#include "SRFlow_prm.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRFlow__Initialize(void);
void SRFlow__InitLogEvent(SRFLOW_INIT_EVENT_TYPE event);
void SRFlow__InitCheck(void);
void SRFlow__MainLogEvent(SRFLOW_MAIN_EVENT_TYPE event);
void SRFlow__MainCheck(void);
void SRFlow__ISRLogEvent(SRFLOW_ISR_EVENT_TYPE event);
void SRFlow__ISRCheck(void);
void SRFlow__LowPowerLogEvent(SRFLOW_LOWPOWER_EVENT_TYPE event);
void SRFlow__LowPowerCheck(void);

#endif // SRFLOW_H_



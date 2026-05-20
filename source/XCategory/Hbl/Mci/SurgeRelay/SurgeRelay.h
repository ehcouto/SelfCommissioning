/**
 *  @file       SurgeRelay.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SURGERELAY_H_
	#define SURGERELAY_H_

#include "SurgeRelay_prm.h"

typedef enum
{
    SURGERELAY_OPEN = 1,
    SURGERELAY_CLOSE,
    SURGERELAY_NORMAL,
} SURGERELAY_ACTION_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SurgeRelay__Initialize(void);
void SurgeRelay__25msHandler(void);
unsigned char SurgeRelay__IsBusReady(void);
void SurgeRelay__Action(SURGERELAY_ACTION_TYPE action);

#endif // SURGERELAY_H_



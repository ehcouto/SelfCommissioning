/**
 *  @file       
 *
 *  @brief      Basic description of file contents
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
#ifndef _BRAKING_H_
#define _BRAKING_H_
#include "MathCalc.h"
#include "MclSpeedCtrl.h"
#include "MclDqRef.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Braking__ResetState(MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params);
float32 Braking__SpeedCtrlHandler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params);
float32 Braking__DqRefHandler(MCL_DQ_REF_IO_F_TYPE *io, MCLDQREF_JOINT_PARAMS_TYPE *params);
#endif // _BRAKING_H_

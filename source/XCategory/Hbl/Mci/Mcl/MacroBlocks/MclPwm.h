/**
 *  @file       MclPwm.h
 *
 *  @brief      Motor Control Loop 3-phase motors: Pwm Modulation Macro Block.
 *
 *  @section
 *
 *  $Header:    MclPwm.h
 *
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCL_PWM_H_
#define MCL_PWM_H_


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MclCrossTypes.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    float32                     Speed_Threshold_InvComp;    //!< [rpm] - speed to enable or disable inverter comp
} MCL_PWM_PARAMS_TYPE;

typedef struct{
//! Pwm Parameters
    MCL_PWM_PARAMS_TYPE		    *PwmPrm;
    MATHCALC_LUT_EXT_F_TYPE     PwmModulationPrm;
    MATHCALC_LUT_EXT_F_TYPE     PwmModulationLossOnStatePrm;
} MCLPWM_JOINT_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MclPwm__ResetState(MCL_PWM_IO_F_TYPE *io, MCLPWM_JOINT_PARAMS_TYPE *params);
void MclPwm__Initialize(MCL_PWM_IO_F_TYPE *io, MCLPWM_JOINT_PARAMS_TYPE *params);
void MclPwm__RunningHandler(MCL_PWM_IO_F_TYPE *io, MCLPWM_JOINT_PARAMS_TYPE *params);
void MclPwm__25msHandler(MCL_PWM_IO_F_TYPE *io);

#endif // MCL_PWM_H_

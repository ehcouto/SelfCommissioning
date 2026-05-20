/**
 *  @file       MotorSafetyMgr.h
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef MOTORSAFETYMGR_H_
	#define MOTORSAFETYMGR_H_

#include "SRMotorSafetyMgr.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#define MotorSafetyMgr__Prepare2StartMotor()                        SRMotorSafetyMgr__Prepare2StartMotor()
#define MotorSafetyMgr__Prepare2StopMotor()                         SRMotorSafetyMgr__Prepare2StopMotor()
#define MotorSafetyMgr__UpdatePwm(duty_a, duty_b, duty_c)           SRMotorSafetyMgr__UpdatePwm((duty_a), (duty_b), (duty_c))
#define MotorSafetyMgr__ConfigPwmPattern(pattern)                   SRMotorSafetyMgr__ConfigPwmPattern((OUTPUT_PATTERN_TYPE)(pattern))
#define MotorSafetyMgr__GetMaxPwm()                                 SRMotorSafetyMgr__GetMaxPwm()
#define MotorSafetyMgr__DoesClassAHaveAccessToPwm()                 SRMotorSafetyMgr__GetFlag(SR_FLAG_CLASS_A_PWM_ACCESS)
#define MotorSafetyMgr__GetFlag(flag)                               SRMotorSafetyMgr__GetFlag((flag))
#define MotorSafetyMgr__GetFault(fault)                             SRMotorSafetyMgr__GetFault((fault))
#define MotorSafetyMgr__AnyFaultFound()                             SRMotorSafetyMgr__AnyFaultFound()
#define MotorSafetyMgr__IsInIdle()                                  SRMotorSafetyMgr__IsInIdle()
#define MotorSafetyMgr__ForceStop()                                 SRMotorSafetyMgr__ForceStop()
#define MotorSafetyMgr__GetInputCaptureCounterValue()				SRMotorSafetyMgr__GetInputCaptureCounterValue()
#define MotorSafetyMgr__GetInputCaptureMatchEvent()                 SRMotorSafetyMgr__GetInputCaptureMatchEvent()
#define MotorSafetyMgr__UpdatePwmPeriod(pwm_period_perc)  			SRMotorSafetyMgr__UpdatePwmPeriod(pwm_period_perc)
#endif // MOTORSAFETYMGR_H_


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================



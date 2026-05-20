/**
 *  @file       SpeedRefGen.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SPEEDREFGEN_H_
	#define SPEEDREFGEN_H_

#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    SPEED_REF_ZEROED,
    SPEED_REF_ACCELERATING,
    SPEED_REF_DECELERATING,
    SPEED_REF_AT_PLATEAU
} SPEED_REF_STATUS_TYPE;

typedef enum
{
    SPEED_REF_DENIED        =  0,                   //!< either speed or acceleration denied
    SPEED_REF_ACCEPTED      =  1,                   //!< both speed and acceleration accepted
    SPEED_REF_NEG_STEP      =  2,                   //!< negative step on speed reference accepted
    SPEED_REF_POS_STEP      =  3,                   //!< positive step on speed reference accepted
} SPEED_REF_SET_NEW_SPEED_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void SpeedRefGen__Initialize(void);
void SpeedRefGen__Reset(void);
void SpeedRefGen__SetSpeedParams(float32 default_accel);
SPEED_REF_SET_NEW_SPEED_TYPE SpeedRefGen__SetNewSpeed(sint32 target_rpm, sint32 accel_rpm_s);
float32 SpeedRefGen__Generator(void);
float32 SpeedRefGen__GetSpeedRef(void);
float32 SpeedRefGen__GetSpeedTarget(void);
SPEED_REF_STATUS_TYPE SpeedRefGen__GetStatus(void);
void SpeedRefGen__ApplyDefaultAccel(uint8 apply_min_accel);

sint32 SpeedRefGen__GetSpeedRefIQ(float32 speed_base, uint8 iq_format);
sint32 SpeedRefGen__GetSpeedTargetIQ(float32 speed_base, uint8 iq_format);

#endif // SPEEDREFGEN_H_



/**
 *  @file       SRMCUSpeedMonitor.h
 *  @brief      MCU Speed monitor header file
 *  @defgroup   CLASS_B Class_B_Group Class B Motor Control Unit Speed Monitor
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2017.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMCUSPEEDMONITOR_H_
    #define SRMCUSPEEDMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void SRMCUSpeedMonitor__Initialize(void);
void SRMCUSpeedMonitor__SlowHandler(void);
void SRMCUSpeedMonitor__High_Speed_Detected(uint8 high_speed_detected);
void SRMCUSpeedMonitor__SetUnlockDoorFault(void);

#endif /* SRMCUSPEEDMONITOR_H_ */

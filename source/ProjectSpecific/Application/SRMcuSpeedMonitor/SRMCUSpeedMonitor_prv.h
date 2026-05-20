/**
 *  @file       SRMCUSpeedMonitor_prv.h
 *  @brief      Parameter and calls to other required modules.
 *  @defgroup   CLASS_B Class_B_Group Class B Motor Control Unit Speed Monitor
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2017.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

#include "SRMotorSafetyMgr.h"                       //!< Include Class B motor control
#include "SRFlow.h"                                 //!< Include Class Flow Check
#include "SRException.h"                            //!< Include exception module

//! Use to debug the algorithm. For certification the line below shall be erased!!!
//#define SRMCU_SPEED_MONITOR_DEBUG                   //!< Shall be NOT defined for certification.

//! Define command to stop the motor from class B area
#define SRMCUSPEEDMONITOR__STOP_MOTOR()                 //SRMotorSafetyMgr__ForceStop()

//! Read unlock door speed flag from the Class B motor control
#define SRMCUSPEEDMONITOR__UNLOCK_SPEED_DETECTED()      SRMotorSafetyMgr__GetFlag(SR_FLAG_UNLOCK_DOOR_SPEED)

//! Read unlock door speed failed flag from the Class B motor control
#define SRMCUSPEEDMONITOR__UNLOCK_SPEED_FAILED()        SRMotorSafetyMgr__GetFaultHolder(SR_FAULT_UNLOCK_DOOR_SPEED_FAILED)

//! Define reaction in the case of corrupted var.
#define SRMCUSPEEDMONITOR__CORRUPTED_VAR()              SRException__Queue(EXCEPTION_DATA_BYTE, 100, 101);


//! Connect MCU Speed monitor to the flow sequence checks.
#define SRMCUSPEEDMONITOR_FLOW_INITIALIZE_BEGIN()       SRFlow__InitLogEvent(SRFLOW_SRMCUSPEEDMONITOR_INIT_BEGIN)
#define SRMCUSPEEDMONITOR_FLOW_INITIALIZE_END()         SRFlow__InitLogEvent(SRFLOW_SRMCUSPEEDMONITOR_INIT_END)
#define SRMCUSPEEDMONITOR_5MS_FLOW_HANDLER_BEGIN()      SRFlow__MainLogEvent(SRFLOW_SRMCUSPEEDMONITOR_5MS_MAIN_BEGIN)
#define SRMCUSPEEDMONITOR_5MS_FLOW_HANDLER_END()        SRFlow__MainLogEvent(SRFLOW_SRMCUSPEEDMONITOR_5MS_MAIN_END)




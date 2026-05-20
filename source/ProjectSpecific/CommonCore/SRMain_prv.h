/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      Private parameter file for Safety Relevant SRMain module.
 *
 *  $Header: SRMain_prv.h 1.2 2015/11/17 08:50:13CET Maciej  Kobuszewski (KOBUSM) Exp  $
 *
 *  @copyright  *****  Copyright 2008-2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL  *****
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


#ifndef SRMAIN_PRV_H_INCLUDED
#define SRMAIN_PRV_H_INCLUDED


//-------------------------------------- Include Files ----------------------------------------------------------------
//! Basic
#include "C_Extensions.h"
#include "SRSystemConfig.h"
//! Drivers
#include "Gpio.h"
#include "Comparator.h"
#include "SRMicro.h"

//! System
#include "SRFlow.h"
#include "SRException.h"
#include "SRClockMonitor.h"
#include "SRIsrMonitor.h"
#include "SRMCUSpeedMonitor.h"
//! Wide

//! Reveal

//! Reveal Queue


//! Others
#include "SRMotorSafetyMgr.h"
#include "SRTimerReference.h"
#if (API020SAFETY_FEATURE == ENABLED)
#include "SRAPI020Safety.h"
#endif
#include "SettingFile.h"
#include "Mode.h"


//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#define SR_FLOW_INITILIZE()                             SRFlow__Initialize()
#define SR_FLOW_CHECK_INIT()                            SRFlow__InitCheck()
#define SR_FLOW_CHECK_MAIN()                            SRFlow__MainCheck()
#define SR_FLOW_CHECK_ISR()                             SRFlow__ISRCheck()
#define SR_FLOW_CHECK_LOWPOWER()                        //SRFlow__LowPowerCheck()


#define SR_EXCEPTION_MAIN_SLOT_VIOLATION(actual,limit)  //SRException__Queue(EXCEPTION_MAIN_SLOT_VIOLATION,actual,limit)
#define SR_EXCEPTION_MAIN_CORRUPT_BYTE_EXCEPTION(data,ndata)
//  --- Macro Definitions

//! Add a routine to perform noise immunity tasks such as (for example):
//!     Check stack pointer
//!     Enable interrupts
//!     Refresh static registers
//!     Check critical values
//!     Background ROM check
//!     etc.
#define SRNOISE_IMMUNITY()


//! This definition should be set to one of the following:
//!     ENABLED = Reset the stack pointer in noise immunity section of main loop (Preferred)
//!     DISABLED = Do not reset the stack pointer in the main loop.
#define SRMAIN_RESETS_STACK_POINTER     ENABLED

//  --- Constants

//! Number of interrupts per millisecond.  This value should be a whole number.
//! Example: If interrupt period is 250 microseconds, then this value should be 4.
#define SR_INTERRUPTS_PER_MILLISECOND   4

//! Number of milliseconds per main slot.  This value should be a whole number.
//! Example: If each main slot takes 5 milliseconds, then this value should be 5.
#define SR_MILLISECONDS_PER_MAIN_SLOT   5

//! Definition if the Low Power feature is ENABLED/DISABLED
#define SRMAIN_LOW_POWER_FEATURE   DISABLED

//  --- Variables

/** @brief      List of tasks executed before entering the main loop.
 *  @details    Add pointers to routines that should be called during initialization.  These
 *              routines are intended to initialize or begin the initialization process for all the
 *              modules in the application.
 */
TASK_TYPE SRInitialization_Tasks[] =
{
    Micro__Initialize,
    Micro__SetNormalSpeedMode,
    Mode__Initialize,
    SettingFile__Initialize,
    Gpio__Initialize,
    Comparator__Initialize,
	SRException__Initialize,
	SRClockMonitor__Initialize,
	SRIsrMonitor__Initialize,
	SRMotorSafetyMgr__Initialize,
    SRTimerReference__Initialize,
	SRMCUSpeedMonitor__Initialize,
    #if (API020SAFETY_FEATURE == ENABLED)
	SRAPI020Safety__Initialize,
    #endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};


/** @brief      List of tasks executed at the start of every main loop slot.
 *  @details    Add pointers to routines that should be called every main loop slot.
 */
TASK_TYPE SREvery_Slot_Tasks[] =
{
    SRMotorSafetyMgr__Handler5ms,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #0 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE SRMain_Slot0_Tasks[] =
{
    Micro__SRRamTest,
	//-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #1 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE SRMain_Slot1_Tasks[] =
{
    Micro__SRFlashTest,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #2 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE SRMain_Slot2_Tasks[] =
{
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    SRMotorSafetyMgr__SlowHandler,
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #3 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE SRMain_Slot3_Tasks[] =
{
    //-------------------------------------------------------------------------
    SRMCUSpeedMonitor__SlowHandler,
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #4 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE SRMain_Slot4_Tasks[] =
{
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      Collection of arrays of function pointers.  Each element represents a single slot
 *              in the main loop.
 *  @details    Each element of this array represents a slot in the main loop.  Each element is a
 *              list of tasks to execute in the main loop slot.  Add all the arrays of tasks that
 *              make up each slot in the application.  The size of this array determines the number
 *              of slots in the main loop.
 */
TASK_LIST_TYPE SRMain_Tasks[] =
{
    SRMain_Slot0_Tasks,
    SRMain_Slot1_Tasks,
    SRMain_Slot2_Tasks,
    SRMain_Slot3_Tasks,
    SRMain_Slot4_Tasks

    //-------------------------------------------------------------------------
    // Do not place a terminating NULL_TASK in this list.
};


/** @brief      List of tasks executed at the start of every timer interrupt.
 *  @details    Add pointers to routines that should run every timer interrupt.
 */
TASK_TYPE SREvery_Interrupt_Tasks[] =
{
    SRMotorSafetyMgr__Handler250us,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #0 of the timer interrupt.
 *  @details    Add pointers to routines that should run during this slot in the timer interrupt.
 *              Tasks that should be executed in every interrupt slot should be included here if
 *              EVERY_INTERRUPT_TASKS_ENABLED is not defined.
 */
TASK_TYPE SRInterrupt_Slot0_Tasks[] =
{
    SRClockMonitor__Monitor1ms,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #1 of the timer interrupt.
 *  @details    Add pointers to routines that should run during this slot in the timer interrupt.
 *              Tasks that should be executed in every interrupt slot should be included here if
 *              EVERY_INTERRUPT_TASKS_ENABLED is not defined.
 */
TASK_TYPE SRInterrupt_Slot1_Tasks[] =
{
    SRIsrMonitor__Handler,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list

    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #2 of the timer interrupt.
 *  @details    Add pointers to routines that should run during this slot in the timer interrupt.
 *              Tasks that should be executed in every interrupt slot should be included here if
 *              EVERY_INTERRUPT_TASKS_ENABLED is not defined.
 */
TASK_TYPE SRInterrupt_Slot2_Tasks[] =
{
    SRMotorSafetyMgr__Handler1ms,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #3 of the timer interrupt.
 *  @details    Add pointers to routines that should run during this slot in the timer interrupt.
 *              Tasks that should be executed in every interrupt slot should be included here if
 *              EVERY_INTERRUPT_TASKS_ENABLED is not defined.
 */
TASK_TYPE SRInterrupt_Slot3_Tasks[] =
{
    SRTimerReference__Handler1ms,
	//-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      Collection of array of function pointers to execute in the interrupt slots.
 *  @details    Each element of this array represents a slot in the interrupt service routine.
 *              Each element is a list of tasks to execute in the interrupt slot.  Add all the
 *              arrays of tasks that make up each slot in the interrupt.  The size of this array
 *              determines the number of slots in the interrupt service routine.
 */
TASK_LIST_TYPE SRInterrupt_Tasks[] =
{
    SRInterrupt_Slot0_Tasks,
    SRInterrupt_Slot1_Tasks,
    SRInterrupt_Slot2_Tasks,
    SRInterrupt_Slot3_Tasks

    //-------------------------------------------------------------------------
    // Do not place a terminating NULL_TASK in this list.
};

#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)


/** @brief      List of tasks executed in low power mode.
 *  @details    Add pointers to routines that should run during the low power state every time the system wakes up to check dunring the Low Power state
 */
TASK_TYPE SRLowPower_Tasks[] =
{
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};



#endif

#endif                                              // SRMAIN_PRV_H_INCLUDED

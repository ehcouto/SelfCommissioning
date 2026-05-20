/**
 *  @file       
 *  @brief      Parametrs for SRMain module related to non safety relevant tasks
 *
 *  $Header: Main_prv.h 1.7 2016/01/22 14:08:21CET Maciej  Kobuszewski (KOBUSM) Exp  $
 *
 *  @copyright  *****  Copyright 2008-2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL  *****
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


#ifndef MAIN_PRV_PRM_H_INCLUDED
#define MAIN_PRV_PRM_H_INCLUDED


//-------------------------------------- Include Files ----------------------------------------------------------------
//! Basic
#include "C_Extensions.h"

//! Drivers
#include "Micro.h"
#include "TimerInterrupt.h"

#include "Uart.h"
#include "Win.h"
#include "Reveal.h"

#include "API003Disc.h"
#include "API007Data.h"
#include "API007App.h"
#include "API007BulkData.h"
#include "API009System.h"
#include "API010PollVar.h"
#include "API013RemoteFunction.h"
#include "API019ProductInfo.h"
#include "APIMotorControl.h"
#include "API221MotionCtrl.h"
//! System
#include "Millis.h"
#include "Log.h"
#include "Mode.h"
#include "SFUpdater.h"

#include "MciParametersLoader.h"




//! Others
//#include "Tachometer.h"
#include "Mci.h"                                    //!< Comment out to disable Mci

//! Application
#include "ModeManagement.h"
#include "MotorSelector.h"
//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//! Add a routine to perform timing check on Main Slots
//! This routine will be called before any instruction with the slot synchronization
#define MAIN_DEBUG_BEFORE_SLOTS()

//! Add a routine to perform timing check on Main Slots
//! This routine will be called after all instructions with the slot synchronization
#define MAIN_DEBUG_AFTER_SLOTS()

//! Add a routine to perform timing check on Timer Interruption Slots
//! This routine will be called before any instruction in the Timer Interruption
#define MAIN_DEBUG_BEFORE_INTERRUPT_SLOTS()

//! Add a routine to perform timing check on Timer Interruption Slots
//! This routine will be called after all instructions in the Timer Interruption
#define MAIN_DEBUG_AFTER_INTERRUPT_SLOTS()

//! This routine to log execution time for the interrupt slots
#define MAIN_TIMER_ISR_END(slot)       //TimerInterrupt__CaptureIntSlotDuration(slot)

//!Add a routine to perform timing check for 5ms slots
//!This routine will be called after the execution of the code of any 5ms slot.
#define MAIN_SLOT_CHECK(time,slot)    //TimerInterrupt__CaptureIntSlotDuration(time,slot)


//  --- Variables

/** @brief      List of tasks executed before entering the main loop.
 *  @details    Add pointers to routines that should be called during initialization.  These
 *              routines are intended to initialize or begin the initialization process for all the
 *              modules in the application.
 */
TASK_TYPE Initialization_Tasks[] =
{
#if (LOG_MODULE_ENABLE == ENABLED)
	Log__Initialize,
#endif

	Millis__Initialize,
    SFUpdater__Initialize,
	Win__Initialize,
	Reveal__Initialize,
	API003Disc__Initialize,
    API007Data__Initialize,
	API009System__Initialize,
#if (API010POLLVAR_FEATURE == ENABLED)
	API010PollVar__Initialize,
#endif
    #if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))
        API013RemoteFunction__Initialize,
    #endif
	API019ProductInfo__Initialize,
	APIMotorControl__Initialize,
	#if ((API221_CLIENT_FEATURE == ENABLED) || (API221_SERVER_FEATURE == ENABLED))
        API221MotionCtrl__Initialize,
    #endif
	#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
	API007App__Initialize,
	#endif

    MciParametersLoader__Initialize,
	TimerInterrupt__Initialize,
	MotorSelector__Initialize,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};



/** @brief      List of tasks executed at the start of every main loop slot.
 *  @details    Add pointers to routines that should be called every main loop slot.
 */
TASK_TYPE Every_Slot_Tasks[] =
{
#ifdef MCI_H_
    Mci__5msHandler,
#endif

	Reveal__Handler,
	//ModeManagement__Handler,
    #if (API007DATA_BULK_DATA_FEATURE == ENABLED)
    API007BulkDataMgr__Handler,
    #endif

    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #0 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE Main_Slot0_Tasks[] =
{

    API009System__Handler25ms,
    Millis__SlowHandler,

    #if (API013_SERVER_FEATURE == ENABLED)
        API013RemoteFunction__Background,
    #endif
    #if (API221_SERVER_FEATURE == ENABLED)
        API221MotionCtrl__Background,
    #endif

	#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
		API007App__Handler,
	#endif

    MciParametersLoader__25msHandler,
    MotorSelector__Handler,
	//-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #1 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE Main_Slot1_Tasks[] =
{
#ifdef MCI_H_
    Mci__25msHandler,
#endif


    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #2 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE Main_Slot2_Tasks[] =
{
	#if (API220MOTORCONTROL_FEATURE == ENABLED)
	APIMotorControl__SlowHandler,
	#endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #3 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE Main_Slot3_Tasks[] =
{
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #4 of the main loop.
 *  @details    Add pointers to routines that should run during this slot in the main loop.
 */
TASK_TYPE Main_Slot4_Tasks[] =
{
#if (LOG_MODULE_ENABLE == ENABLED)
	Log__Background,
#endif

    Mode__Handler,
#if (API010POLLVAR_FEATURE == ENABLED)
	API010PollVar__SlowHandler,
#endif
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
TASK_LIST_TYPE Main_Tasks[] =
{
    Main_Slot0_Tasks,
    Main_Slot1_Tasks,
    Main_Slot2_Tasks,
    Main_Slot3_Tasks,
    Main_Slot4_Tasks

    //-------------------------------------------------------------------------
    // Do not place a terminating NULL_TASK in this list.
};

/** @brief      List of tasks executed at the start of every timer interrupt.
 *  @details    Add pointers to routines that should run every timer interrupt.
 */
TASK_TYPE Every_Interrupt_Tasks[] =
{
//    TimerInterrupt__SetupNext,
    Uart__Handler,

#ifdef MCI_H_
    Mci__250usHandler,
#endif

    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};


/** @brief      List of tasks executed in SLOT #0 of the timer interrupt.
 *  @details    Add pointers to routines that should run during this slot in the timer interrupt.
 *              Tasks that should be executed in every interrupt slot should be included here if
 *              EVERY_INTERRUPT_TASKS_ENABLED is not defined.
 */
TASK_TYPE Interrupt_Slot0_Tasks[] =
{
    Win__Handle1ms,
	Reveal__Handler1ms,

	//-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #1 of the timer interrupt.
 *  @details    Add pointers to routines that should run during this slot in the timer interrupt.
 *              Tasks that should be executed in every interrupt slot should be included here if
 *              EVERY_INTERRUPT_TASKS_ENABLED is not defined.
 */
TASK_TYPE Interrupt_Slot1_Tasks[] =
{

    Millis__Handler1ms,

#ifdef MCI_H_
    Mci__1msHandler,
#endif

//    FlexHandler__Execute1MS,

	//-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #2 of the timer interrupt.
 *  @details    Add pointers to routines that should run during this slot in the timer interrupt.
 *              Tasks that should be executed in every interrupt slot should be included here if
 *              EVERY_INTERRUPT_TASKS_ENABLED is not defined.
 */
TASK_TYPE Interrupt_Slot2_Tasks[] =
{
    Reveal__Handler1ms,
	API010PollVar__FastHandler,

    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed in SLOT #3 of the timer interrupt.
 *  @details    Add pointers to routines that should run during this slot in the timer interrupt.
 *              Tasks that should be executed in every interrupt slot should be included here if
 *              EVERY_INTERRUPT_TASKS_ENABLED is not defined.
 */
TASK_TYPE Interrupt_Slot3_Tasks[] =
{
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
TASK_LIST_TYPE Interrupt_Tasks[] =
{
    Interrupt_Slot0_Tasks,
    Interrupt_Slot1_Tasks,
    Interrupt_Slot2_Tasks,
    Interrupt_Slot3_Tasks

    //-------------------------------------------------------------------------
    // Do not place a terminating NULL_TASK in this list.
};

#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)


/** @brief      List of tasks executed in low power mode.
 *  @details    Add pointers to routines that should run during the low power state every time the system wakes up to check dunring the Low Power state
 */
TASK_TYPE LowPower_Tasks[] =
{
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};



#endif

#endif                                              // MAIN_PRV_PRM_H_INCLUDED

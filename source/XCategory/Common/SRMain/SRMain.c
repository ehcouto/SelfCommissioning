/**
 *  @defgroup   CLASS_B
 *  @file       SRMain.c
 *  @brief      main() routine and primary timer ISR routine.
 *
 *  @details    This module contains the main() routine and the main interrupt service routine.
 *              The main() routine is the program's entry point.  The main interrupt service routine
 *              manages the timing for the application.
 *
 *              This module has been constructed similar to a GDM.  So the only required changes are
 *              to the main_prv_prm.h file.  Both main.c and main.h are not intended to be changed
 *              when used in a different application.
 *
 *              There are a series of constant arrays that are lists of routines to call at certain
 *              points in the execution of the main loop and the main interrupt service routine.
 *              Each array is described in the comments associated with that array.
 *              They are summarized here:
 *
 *              <pre>
 *              Function List               Purpose
 *              -----------------------     --------------------------------------------------------
 *              Initialization_Tasks        NULL terminated list of tasks to execute before entering
 *                                          the infinite loop.
 *              -----------------------     --------------------------------------------------------
 *              Every_Slot_Tasks            NULL terminated list of tasks to execute at the
 *                                          beginning of every slot.
 *              -----------------------     --------------------------------------------------------
 *              Main_Slot{n}_Tasks          NULL terminated list of tasks to execute during slot(n)
 *                                          of the main routine.
 *              -----------------------     --------------------------------------------------------
 *              Main_Tasks                  Array made up of the Main_Slot{n}_Tasks[] arrays.
 *                                          Determines the sequence of slots and the number of slots.
 *              -----------------------     --------------------------------------------------------
 *              Every_Interrupt_Tasks       NULL terminated list of tasks to execute at the start of
 *                                          every main interrupt.
 *              -----------------------     --------------------------------------------------------
 *              Interrupt_Slot(n)_Tasks     NULL terminated list of tasks to execute during slot(n)
 *                                          of the main interrupt service routine.
 *              -----------------------     --------------------------------------------------------
 *              Interrupt_Tasks             Array made up of the Interrupt_Slot(n)_Tasks[] arrays.
 *                                          Determines the sequence of slots and the number of slots.
 *              -----------------------     --------------------------------------------------------
 *              </pre>
 *
 *              When configuring this module for a new application, the programmer should create a
 *              Main_Slot(n)_Tasks[] array for each of the slots desired in the main loop.  Also,
 *              create an Interrupt_Slot(n)_Tasks[] array for each of the slots desired in the main
 *              interrupt service routine.  The programmer should set the SLOT_TIME equal to the
 *              number of interrupts that make up a single slot.
 *
 *
 *  
 *
 *  @copyright  Copyright 2007 -   Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SRMicro.h"
#include "SRMain.h"

//! A cosntant pointer to a function with no return value or parameters.
typedef void (* const TASK_TYPE)(void);

//! A constant pointer to a list of TASK_TYPESs.
typedef void (* const * const TASK_LIST_TYPE)(void);

//! A NULL pointer to use as a terminator for function lists.
#define NULL_TASK ((TASK_TYPE)0)


#include "SRMain_prv.h"
#include "Main_prv.h"

#ifndef SRMAIN_RESETS_STACK_POINTER
    #define SRMAIN_RESETS_STACK_POINTER  ENABLED
#endif

#ifndef SRMAIN_LOW_POWER_FEATURE
    #define SRMAIN_LOW_POWER_FEATURE   DISABLED
#endif

#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)
    #include "SRPowerMgr.h"
#endif


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------



#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)
    typedef enum
    {
        SRMAIN_STATE_NORMAL_POWER,
        SRMAIN_STATE_SWITHCING_TO_LOW_POWER,
        SRMAIN_STATE_LOW_POWER,
        SRMAIN_STATE_SWITHING_TO_NORMAL_POWER
    } SRMAIN_POWER_STATE;
#endif

//  --- Constants
//! Number of slots in the main loop
#define MAIN_SLOT_COUNT (sizeof(Main_Tasks)/sizeof(TASK_LIST_TYPE))
#define SR_MAIN_SLOT_COUNT (sizeof(SRMain_Tasks)/sizeof(TASK_LIST_TYPE))

//! Number of slots in the main interrupt
#define INTERRUPT_SLOT_COUNT (sizeof(Interrupt_Tasks)/sizeof(TASK_LIST_TYPE))
#define SR_INTERRUPT_SLOT_COUNT (sizeof(Interrupt_Tasks)/sizeof(TASK_LIST_TYPE))


/** @breif      Duration of each slot in the main loop.
 *  @details    The number of interrupts per main slot in the gated slotted architecture.
 *              Example: 250 microsecond interrupt rate is 4 interrupts per millisecond.
 *                       For 5 millisecond slots: SLOT_TIME = 5 * 4.
 */
#define SLOT_TIME (SR_INTERRUPTS_PER_MILLISECOND * SR_MILLISECONDS_PER_MAIN_SLOT)


//  --- Variables

#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)
    SRMAIN_POWER_STATE SRMain_Power_State;
#endif


//! Number of the current main loop slot in the main() routine
static uint8 Main_Slot;

//! Number of the current interrupt slot in the timer interrupt service routine
static TINY uint8 Interrupt_Slot;

//! Timer to measure a single slot duration
static uint8 Main_Slot_Timer;

//! Value of Main_Slot_Timer the last time a Slot was executed
static uint8 Main_Slot_Timer_Diff;

#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)
    static SRMAIN_POWER_STATE N_SRMain_Power_State;
#endif

//! Number of the current main loop slot in the main() routine
static uint8 N_Main_Slot;

//! Number of the current interrupt slot in the timer interrupt service routine
static TINY uint8 N_Interrupt_Slot;


#define SRMAIN_COMPLEMENT(data)    (~(data))


//!Add a routine to perform timing check for 5ms slots
//!This routine will be called after the execution of the code of any 5ms slot.
#ifndef MAIN_SLOT_CHECK
    #define MAIN_SLOT_CHECK(a,b)
#endif

//! Add a routine to perform timing check on Main Slots
//! This routine will be called before any instruction with the slot synchronization
#ifndef MAIN_DEBUG_BEFORE_SLOTS
    #define MAIN_DEBUG_BEFORE_SLOTS()
#endif

//! Add a routine to perform timing check on Main Slots
//! This routine will be called after all instructions with the slot synchronization
#ifndef MAIN_DEBUG_AFTER_SLOTS
    #define MAIN_DEBUG_AFTER_SLOTS()
#endif

//! Add a routine to perform timing check on Timer Interruption Slots
//! This routine will be called before any instruction in the Timer Interruption
#ifndef MAIN_DEBUG_BEFORE_INTERRUPT_SLOTS
    #define MAIN_DEBUG_BEFORE_INTERRUPT_SLOTS()
#endif

//! Add a routine to perform timing check on Timer Interruption Slots
//! This routine will be called after all instructions in the Timer Interruption
#ifndef MAIN_DEBUG_AFTER_INTERRUPT_SLOTS
    #define MAIN_DEBUG_AFTER_INTERRUPT_SLOTS()
#endif

//! This macro could be used to verify the integrity of the SR Varaibles
#ifndef SRMAIN_CHECK_SR_VARIABLES
    #define SRMAIN_CHECK_SR_VARIABLES()
#endif

//! This macro is used along with SRMAIN_CHECK_SR_VARIABLES by CRCs the variables
#ifndef SRMAIN_CRC_SR_VARIABLES
    #define SRMAIN_CRC_SR_VARIABLES()
#endif


//! Macro for checking the SR Variables in the Timer Interrupt
#ifndef SRMAIN_CHECK_INTERRUPT_SR_VARIABLES
    #define SRMAIN_CHECK_INTERRUPT_SR_VARIABLES()
#endif

//! Macro for checking the SR Variables  in the Timer Interrupt
#ifndef SRMAIN_CRC_INTERRUPT_SR_VARIABLES
    #define SRMAIN_CRC_INTERRUPT_SR_VARIABLES()
#endif


//! Macro for SQC Timer interrupt slot test
#ifndef MAIN_TIMER_ISR_END
    #define MAIN_TIMER_ISR_END(slot)
#endif

static uint8 previews_slot;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

void ExecuteTasks(TASK_TYPE * task_list);
static void SRMainUpdateByte(uint8 * data, uint8 * ndata,uint8 value);
static BOOL_TYPE SRMainCheckByte(uint8 * data, uint8 * ndata);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      The application's main routine
 *
 *  @image      html SRMain_Activity.jpg
 *
 *  @details    This routine implements a main loop with a gated slotted architecture.
 *              It can be configured through the parameter files.
 *
 *              NOTE:
 *              If the MAIN_RESETS_STACK_POINTER compiler directive is defined, then all
 *              local variables must be declared as static or they will be placed on the
 *              stack.  Since the main loop resets the stack pointer, the variables will
 *              be lost.
 */
void main(void)
{

    #if (SRMAIN_RESETS_STACK_POINTER == ENABLED)
        // Execute all the time that we are not executing a Slot.
        // Ensure that the stack pointer is correct.
        DISABLE_INTERRUPTS();   // Ensure that the interrupts are disabled before reset the stack pointer
        RESET_STACK_POINTER();
        DISABLE_INTERRUPTS();   // ensure that the interrupts are disabled after reset the stack pointer
    #endif
    //--------------------------------------------------------------------------
    //  --- Initialize program

    // Initialize the SR Flow control module
    SR_FLOW_INITILIZE();

    // Initialize main() routine variables.
    Main_Slot_Timer = SLOT_TIME;                    // Timer for main slots
    Main_Slot_Timer_Diff = 0;                       // Provides mutual exclusion with the timer interrupt

    //Main_Slot = 0;
    SRMainUpdateByte(&Main_Slot, &N_Main_Slot,0);

    //Interrupt_Slot = 0;
    SRMainUpdateByte(&Interrupt_Slot, &N_Interrupt_Slot,0);

    // Initialize the SR microcontroller and application modules
    ExecuteTasks(SRInitialization_Tasks);

    // Initialize the Class A application modules
    ExecuteTasks(Initialization_Tasks);

    // Macro that calls the SRFlow modules to test if the Initialize sequence was executed property
    SR_FLOW_CHECK_INIT();

    #if (SRMAIN_LOW_POWER_FEATURE == ENABLED)
        SRMainUpdateByte((uint8 *)&SRMain_Power_State,(uint8 *)&N_SRMain_Power_State,SRMAIN_STATE_NORMAL_POWER);
    #endif


    //--------------------------------------------------------------------------
    //  --- Begin Main (Infinite) Loop
    for (;;)
    {

        #if (SRMAIN_LOW_POWER_FEATURE == ENABLED)

            if (SRMainCheckByte((uint8 *)&SRMain_Power_State,(uint8 *)&N_SRMain_Power_State) == FALSE)
            {
                SRMainUpdateByte((uint8 *)&SRMain_Power_State,(uint8 *)&N_SRMain_Power_State,SRMAIN_STATE_NORMAL_POWER);
            }

            switch (SRMain_Power_State)
            {
                case SRMAIN_STATE_NORMAL_POWER:
        #endif

                    // Time Gate for Slotted Architecture
                    // Implementation catches up if it falls behind
                    if ((uint8)(Main_Slot_Timer - Main_Slot_Timer_Diff) >= SLOT_TIME)
                    {
                        // '+=' operator used here so that overruns are eventually caught up
                        Main_Slot_Timer_Diff += SLOT_TIME;

                        // Macro for timing analyzes
                        MAIN_DEBUG_BEFORE_SLOTS();

                        // Macro for checking the SR Variables
                        SRMAIN_CHECK_SR_VARIABLES();

                        // Placement here insures that both the main loop and the main timer interrupt are running.
                        // Ideally, this would be the only call anywhere in the code.
                        SRMicro__ServiceWatchdog();

                        // Execute tasks that run SR every slot
                        ExecuteTasks(SREvery_Slot_Tasks);

                        // Execute tasks that only run during a specific slot
                        ExecuteTasks(SRMain_Tasks[Main_Slot]);

                        // Macro for checking the SR Variables
                        SRMAIN_CRC_SR_VARIABLES();

                        // Execute tasks that run every slot
                        ExecuteTasks(Every_Slot_Tasks);


                        // Execute tasks that only run during a specific slot for Class A code
                        if (Main_Slot < MAIN_SLOT_COUNT)
                        {
                            ExecuteTasks(Main_Tasks[Main_Slot]);
                        }

                        previews_slot = Main_Slot;

                        // Move to the next main loop Slot.  Roll back to zero if necessary.
                        if (SRMainCheckByte(&Main_Slot,&N_Main_Slot) == FALSE)
                        {
                            // Main_Slot data corrupted
                            SRMainUpdateByte(&Main_Slot, &N_Main_Slot,0);
                        }
                        else
                        {
                            // If slot if okay, keeping counting
                            SRMainUpdateByte(&Main_Slot, &N_Main_Slot,Main_Slot+1);
                        }

                        if (Main_Slot >= SR_MAIN_SLOT_COUNT)
                        {
                            SRMainUpdateByte(&Main_Slot, &N_Main_Slot,0);
                            SR_FLOW_CHECK_MAIN();
                        }

                        // Tests if the Slot Time was respected
                        if ((uint8)(Main_Slot_Timer - Main_Slot_Timer_Diff) >= SLOT_TIME)
                        {
                            // Exception: Main Slot Time Violation
                            SR_EXCEPTION_MAIN_SLOT_VIOLATION((Main_Slot_Timer - Main_Slot_Timer_Diff),SLOT_TIME);

                        }
                        MAIN_SLOT_CHECK((Main_Slot_Timer - Main_Slot_Timer_Diff),previews_slot);

                        // Macro for timing analyzes
                        MAIN_DEBUG_AFTER_SLOTS();
                    }                                           //End 5 millisecond gate
                    else
                    {
                        #if (SRMAIN_RESETS_STACK_POINTER == ENABLED)
                            // Execute all the time that we are not executing a Slot.
                            // Ensure that the stack pointer is correct.
                            DISABLE_INTERRUPTS();
                            RESET_STACK_POINTER();
                            ENABLE_INTERRUPTS();

                        #endif

                        // The code associated to NOISE_IMMUNITY runs between slots.
                        // Ensure that the code is not too long that could cause problems to the slots.
                        SRNOISE_IMMUNITY();
                    }
#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)
                    if (SRPowerMgr__GetPowerModeState() == SRPOWERMGR_STATE_LOW_POWER)
                    {
                        SRMainUpdateByte((uint8 *)&SRMain_Power_State,(uint8 *)&N_SRMain_Power_State,SRMAIN_STATE_SWITHCING_TO_LOW_POWER);
                    }
                    break;
                case SRMAIN_STATE_SWITHCING_TO_LOW_POWER:
                    SRPowerMgr__SwitchToLowPowerTasks();
                    SRMainUpdateByte((uint8 *)&SRMain_Power_State,(uint8 *)&N_SRMain_Power_State,SRMAIN_STATE_LOW_POWER);

                    SR_FLOW_INITILIZE();    // Forces Initialization so the LowPower events can be log from the begin.
                    break;
                case SRMAIN_STATE_LOW_POWER:
                    // Placement here insures that both the main loop and the main timer interrupt are running.
                    // Ideally, this would be the only call anywhere in the code.
                    SRMicro__ServiceWatchdog();

                    // Execute tasks that run SR every slot
                    ExecuteTasks(SRLowPower_Tasks);

                    SR_FLOW_CHECK_LOWPOWER();

                    // Execute tasks that run every slot
                    ExecuteTasks(LowPower_Tasks);

                    if (SRPowerMgr__GetPowerModeState() == SRPOWERMGR_STATE_NORMAL_POWER)
                    {
                        SRMainUpdateByte((uint8 *)&SRMain_Power_State,(uint8 *)&N_SRMain_Power_State,SRMAIN_STATE_SWITHING_TO_NORMAL_POWER);
                    }
                    else
                    {
                        SRPowerMgr__GoSleep();
                    }

                    break;
                case SRMAIN_STATE_SWITHING_TO_NORMAL_POWER:

                    // Actions to be taken when switching to the Normal Power Mode.
                    // tasks that must be initializes should be included within SRPowerMgr__SwitchToNormalPowerTasks
                    SRPowerMgr__SwitchToNormalPowerTasks();

                    // Initialize the SR Flow control module
                    SR_FLOW_INITILIZE();

                    //Main_Slot = 0;
                    SRMainUpdateByte(&Main_Slot, &N_Main_Slot,0);

                    //Interrupt_Slot = 0;
                    SRMainUpdateByte(&Interrupt_Slot, &N_Interrupt_Slot,0);

                    // There's no Break in this state, it falls down to the default case
                    //lint -fallthrough
                default:
                    SRMainUpdateByte((uint8 *)&SRMain_Power_State,(uint8 *)&N_SRMain_Power_State,SRMAIN_STATE_NORMAL_POWER);
                    break;
            }
#endif
    }                                               //End forever loop
}                                                   //End main


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Interrupt handler for the timer interrupt.
 *
 *  @image      html SRMain_ISR_Activity.jpg
 *
 *  @details    This routine implements an interrupt service routine with a gated slotted
 *              architecture.  Each interrupt calls the routines for the next interrupt slot.
 *              It can be configured through the parameter files.
 */
#ifdef NUCLEUS_FAST_TIMER_ISR
INTERRUPT_DIRECTIVE void SRMain__FastTimerISR( void )
#else
INTERRUPT_DIRECTIVE void SRMain__TimerISR( void )
#endif
{
    // Macro for timing analyzes
    MAIN_DEBUG_BEFORE_INTERRUPT_SLOTS();

#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)
    if (SRMain_Power_State == SRMAIN_STATE_NORMAL_POWER)
    {
#endif


        // Macro for checking the SR Variables
        SRMAIN_CHECK_INTERRUPT_SR_VARIABLES();

        // SR Tasks every Timer Interrupt
        ExecuteTasks(SREvery_Interrupt_Tasks);

        // SR Tasks specific Timer Interrupt Slot
        ExecuteTasks(SRInterrupt_Tasks[Interrupt_Slot]);

        // Macro for checking the SR Variables
        SRMAIN_CRC_INTERRUPT_SR_VARIABLES();

        // Class A Tasks every Timer Interrupt
        ExecuteTasks(Every_Interrupt_Tasks);

        // Class A Tasks specific Timer Interrupt Slot
        if (Interrupt_Slot < INTERRUPT_SLOT_COUNT)
        {
            ExecuteTasks(Interrupt_Tasks[Interrupt_Slot]);
        }

        MAIN_TIMER_ISR_END(Interrupt_Slot);

        // Move to the next interrupt Slot.  Roll back to zero if necessary.
        //Interrupt_Slot++;
        if (SRMainCheckByte(&Interrupt_Slot, &N_Interrupt_Slot) == FALSE)
        {
            // slot corrupted
            SRMainUpdateByte(&Interrupt_Slot, &N_Interrupt_Slot,0);
        }
        else
        {
            SRMainUpdateByte(&Interrupt_Slot, &N_Interrupt_Slot,Interrupt_Slot + 1);
        }


        if (Interrupt_Slot >= SR_INTERRUPT_SLOT_COUNT)
        {
            //Interrupt_Slot = 0;
            SRMainUpdateByte(&Interrupt_Slot, &N_Interrupt_Slot,0);
            // Macro to the SR Flow control on the ISR SR tasks
            SR_FLOW_CHECK_ISR();
        }

        // Increment Main_Slot_Timer to synch Main Slot Time
        Main_Slot_Timer++;

#if (SRMAIN_LOW_POWER_FEATURE == ENABLED)
    }
    else
    {
        TimerInterrupt__SetupNext();
    }
#endif

    // Macro for timing analyzes
    MAIN_DEBUG_AFTER_INTERRUPT_SLOTS();
}

#ifdef NUCLEUS_FAST_TIMER_ISR
INTERRUPT_DIRECTIVE void SRMain__TimerISR( void )
#else
INTERRUPT_DIRECTIVE void SRMain__FastTimerISR(void)
#endif
{
    ;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Execute the list of tasks pointed to by task_list.
 *  @details    This code is approximately a 33% improvement in execution time and a 44%
 *              improvement in code size over a version that uses an index into the task_list
 *              array.  Former code shown here.
 *              <pre>
 *                  void ExecuteTasks(TASK_TYPE task_list[])
 *                  {
 *                      uint8 task;
 *
 *                      for (task=0; task_list[task] != NULL_TASK; task++)
 *                      {
 *                          task_list[task]();
 *                      }
 *                  }
 *              </pre>
 *
 *  @param      task_list = Null-terminated list of tasks to execute.
 *                          Each task must have the prototype: void Task(void)
 */
void ExecuteTasks(TASK_TYPE * task_list)
{
    while (*task_list != NULL_TASK)
    {
        (*task_list)();
        task_list++;
    }
}


/**
 * This method updates a Safety Byte Variable and it's backup variable
 *
 * @param data
 * @param ndata
 * @param value
 */
static void SRMainUpdateByte(uint8 * data, uint8 * ndata,uint8 value)
{
    *data = value;
    *ndata = SRMAIN_COMPLEMENT(value);
}

/**
 * This method checks a Safety Byte Variable and its backup
 *
 * @param data
 * @param ndata
 * @return
 */
static BOOL_TYPE SRMainCheckByte(uint8 * data, uint8 * ndata)
{
    uint8 aux;
    BOOL_TYPE response;
    response = TRUE;

    aux = SRMAIN_COMPLEMENT(*data);
    if (aux != (*ndata) )
    {

    	SR_EXCEPTION_MAIN_CORRUPT_BYTE_EXCEPTION(*data,*ndata);
        response = FALSE;
    }
    return (response);
}


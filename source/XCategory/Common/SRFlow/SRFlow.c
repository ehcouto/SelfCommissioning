/**
 *  @defgroup   CLASS_B
 *  @file
 *
 *  @brief      This module intends to check the execution flow of the main processes in the system.
 *
 *  @details    Basically this module accepts log of events from other modules and in the appropriated time check the
 *              logged events against a predefined events list.
 *              <pre>
 *              This module basically checks the execution log of:
 *                      Initialization
 *                      Main slots 25ms and every slot
 *                      Interrupt Slots and every timer interruption
 *

 *
 *              The variables used in the module are not handled as SR Variables because of the way the variables works
 *              itself ensures the integrity.
 *
 *              The system basically have a array where the events are logger and a pointer which works as a index to the list.
 *              If a event is logged and other process interfere or the memory position is damaged in the array the
 *              system will point a exception because the events wont match the expected values.
 *              If the pointer is affected by other process it will interfere in the sequence of the logged events as well
 *              as by the time the system will check the logged events it could be shorter or longer than the expected
 *              list of events.
 *              So basically the pointer and the array work together and is a single bit failure happen the system will
 *              generate a exception about that.
 *
 *              <pre>
 *
 *  
 *
 *  @copyright  Copyright 2011-. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SRFlow.h"
#include "SRFlow_prv.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#if (SRFLOW_NUM_INIT_EVENTS > 0)
    static SRFLOW_INIT_EVENT_TYPE SRFlow_Init_Events[SRFLOW_NUM_INIT_EVENTS];
    static unsigned char SRFlow_Init_Pointer;
#endif

#if (SRFLOW_NUM_MAIN_EVENTS > 0)
    static SRFLOW_MAIN_EVENT_TYPE SRFlow_Main_Events[SRFLOW_NUM_MAIN_EVENTS];
    static unsigned char SRFlow_Main_Pointer;
#endif

#if (SRFLOW_NUM_ISR_EVENTS > 0)
    static SRFLOW_ISR_EVENT_TYPE SRFlow_ISR_Events[SRFLOW_NUM_ISR_EVENTS];
    static unsigned char SRFlow_ISR_Pointer;
#endif

#if (SRFLOW_NUM_LOWPOWER_EVENTS > 0)
    static SRFLOW_LOWPOWER_EVENT_TYPE SRFlow_LowPower_Events[SRFLOW_NUM_LOWPOWER_EVENTS];
    static unsigned char SRFlow_LowPower_Pointer;
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void SRFlowClearInit(void);
static void SRFlowClearMain(void);
static void SRFlowClearISR(void);
static void SRFlowClearISR(void);
static void SRFlowClearLowPower(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRFlow and its variables
 */
void SRFlow__Initialize(void)
{
    SRFlowClearInit();
    SRFlowClearMain();
    SRFlowClearISR();
    SRFlowClearLowPower();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method logs a Initialization event
 *  @param      event
 */
void SRFlow__InitLogEvent(SRFLOW_INIT_EVENT_TYPE event)
{
	#if (SRFLOW_NUM_INIT_EVENTS > 0)
	    if (SRFlow_Init_Pointer < SRFLOW_NUM_INIT_EVENTS)
	    {
	        SRFlow_Init_Events[SRFlow_Init_Pointer] = event;
	        SRFlow_Init_Pointer++;
	    }
	    else
	    {
	        // Exception: More events than expected

	        SRFLOW_INIT_OVERFILLED_EXCEPTION();
	    }
	#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method verifies the sequence of the Initialization Execution.
 */
void SRFlow__InitCheck(void)
{
#if (SRFLOW_NUM_INIT_EVENTS > 0)
    unsigned char count;

    if (SRFlow_Init_Pointer != SRFLOW_NUM_INIT_EVENTS)
    {
        // Exception: fewer events than expected
        SRFLOW_INIT_INCOMPLETE_EXCEPTION();
    }
    else
    {
        for (count=0;count<SRFLOW_NUM_INIT_EVENTS;count++)
        {
            if (SRFlow_Init_Events[count] != SRFLOW_EVENT_INIT_LIST[count])
            {
                // Exception : wrong sequence executed
                SRFLOW_INIT_SEQUENCE_EXCEPTION();
                break;
            }
        }
    }
    SRFlowClearInit();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method logs a Main Flow Event
 *  @param      event
 */
void SRFlow__MainLogEvent(SRFLOW_MAIN_EVENT_TYPE event)
{
#if (SRFLOW_NUM_MAIN_EVENTS > 0)
    if (SRFlow_Main_Pointer < SRFLOW_NUM_MAIN_EVENTS)
    {
        SRFlow_Main_Events[SRFlow_Main_Pointer] = event;
        SRFlow_Main_Pointer++;
    }
    else
    {
        // Exception: More events than expected
        SRFLOW_MAIN_OVERFILLED_EXCEPTION();

    }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method verifies the sequence of the Main Flow Execution.
 */
void SRFlow__MainCheck(void)
{
#if (SRFLOW_NUM_MAIN_EVENTS > 0)
    unsigned char count;

    if (SRFlow_Main_Pointer != SRFLOW_NUM_MAIN_EVENTS)
    {
        // Exception: fewer events than expected
        SRFLOW_MAIN_INCOMPLETE_EXCEPTION();
    }
    else
    {
        for (count=0;count<SRFLOW_NUM_MAIN_EVENTS;count++)
        {
            if (SRFlow_Main_Events[count] != SRFLOW_EVENT_MAIN_LIST[count])
            {
                SRFLOW_MAIN_SEQUENCE_EXCEPTION();
                break;  // break the for loop
            }
        }
    }
    SRFlowClearMain();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method logs a ISR Flow Event.
 *  @param      event
 */
void SRFlow__ISRLogEvent(SRFLOW_ISR_EVENT_TYPE event)
{
#if (SRFLOW_NUM_ISR_EVENTS > 0)
    if (SRFlow_ISR_Pointer < SRFLOW_NUM_ISR_EVENTS)
    {
        SRFlow_ISR_Events[SRFlow_ISR_Pointer] = event;
        SRFlow_ISR_Pointer++;
    }
    else
    {
        // Exception: More events than expected
        SRFLOW_ISR_OVERFILLED_EXCEPTION();
    }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method verifies the sequence of the Timer ISR Flow Execution.
 */
void SRFlow__ISRCheck(void)
{
#if (SRFLOW_NUM_ISR_EVENTS > 0)
   unsigned char count;

   if (SRFlow_ISR_Pointer != SRFLOW_NUM_ISR_EVENTS)
   {
       // Exception: fewer events than expected
       SRFLOW_ISR_INCOMPLETE_EXCEPTION();
   }
   else
   {
       for (count=0;count<SRFLOW_NUM_ISR_EVENTS;count++)
       {
           if (SRFlow_ISR_Events[count] != SRFLOW_EVENT_ISR_LIST[count])
           {
               // Exception : wrong sequence executed
               SRFLOW_ISR_SEQUENCE_EXCEPTION();
               break;
           }
       }
   }
   SRFlowClearISR();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method logs a LowPowe Flow Event.
 *  @param      event
 */
void SRFlow__LowPowerLogEvent(SRFLOW_LOWPOWER_EVENT_TYPE event)
{
#if (SRFLOW_NUM_LOWPOWER_EVENTS > 0)
    if (SRFlow_LowPower_Pointer < SRFLOW_NUM_LOWPOWER_EVENTS)
    {
        SRFlow_LowPower_Events[SRFlow_LowPower_Pointer] = event;
        SRFlow_LowPower_Pointer++;
    }
    else
    {
        // Exception: More events than expected
        SRFLOW_LOWPOWER_OVERFILLED_EXCEPTION();
    }
#endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method verifies the sequence of the Timer ISR Flow Execution.
 */
void SRFlow__LowPowerCheck(void)
{
#if (SRFLOW_NUM_LOWPOWER_EVENTS > 0)
   unsigned char count;

   if (SRFlow_LowPower_Pointer != SRFLOW_NUM_LOWPOWER_EVENTS)
   {
       // Exception: fewer events than expected
       SRFLOW_LOWPOWER_INCOMPLETE_EXCEPTION();
   }
   else
   {
       for (count=0;count<SRFLOW_NUM_LOWPOWER_EVENTS;count++)
       {
           if (SRFlow_LowPower_Events[count] != SRFLOW_EVENT_LOWPOWER_LIST[count])
           {
               // Exception : wrong sequence executed
               SRFLOW_LOWPOWER_SEQUENCE_EXCEPTION();
               break;
           }
       }
   }
   SRFlowClearLowPower();
#endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method clear all the events in the Init List of events
 */
static void SRFlowClearInit(void)
{
#if (SRFLOW_NUM_INIT_EVENTS > 0)
    unsigned char count;
    for (count=0;count<SRFLOW_NUM_INIT_EVENTS;count++)
    {
        SRFlow_Init_Events[count] = SRFLOW_CLEAR_INIT_ALL;
    }
    SRFlow_Init_Pointer = 0;
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method clear all the events in the Main List of events
 */
static void SRFlowClearMain(void)
{
#if (SRFLOW_NUM_MAIN_EVENTS > 0)
    unsigned char count;
    for (count=0;count<SRFLOW_NUM_MAIN_EVENTS;count++)
    {
        SRFlow_Main_Events[count] = SRFLOW_CLEAR_MAIN_ALL;
    }
    SRFlow_Main_Pointer = 0;
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method clear all the events in the ISR List of events
 */
static void SRFlowClearISR(void)
{
#if (SRFLOW_NUM_ISR_EVENTS > 0)
    unsigned char count;
    for (count=0;count<SRFLOW_NUM_ISR_EVENTS;count++)
    {
        SRFlow_ISR_Events[count] = SRFLOW_CLEAR_ISR_ALL;
    }
    SRFlow_ISR_Pointer = 0;
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method clear all the events in the LowPower List of events
 */
static void SRFlowClearLowPower(void)
{
#if (SRFLOW_NUM_LOWPOWER_EVENTS > 0)
    unsigned char count;
    for (count=0;count<SRFLOW_NUM_LOWPOWER_EVENTS;count++)
    {
        SRFlow_LowPower_Events[count] = SRFLOW_LOWPOWER_CLEAR_ALL;
    }
    SRFlow_LowPower_Pointer = 0;
#endif
}


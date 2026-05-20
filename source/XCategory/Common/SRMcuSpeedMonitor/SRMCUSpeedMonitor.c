/**
 * @file        SRMCUSpeedMonitor.c
 * @brief       MCU Speed monitor - ACU extension in MCU to monitor and allow/deny motor to run
 *              Monitors the status from ACU and enables motor control accordingly. Also,
 *              monitors the status from MCU and publish to the WIN bus
 * @defgroup    CLASS_B Class_B_Group Class B Motor Control Unit Speed Monitor
 *
 * @details     Motor shall be allow to run under few sceneries (safe to run):
 *              1.  API20 non error: timeout is not expired or correct sequence,
 *                  no error is detected from API20. API20 living is the link that
 *                  ensures the data from door lock status installed in the ACU.
 *                  Upon an API20 failure, the status are not be updated anymore, therefore
 *                  the default reaction is to block the motor by class B means.
 *              2.  Motor enabled and not limited: allowed to run at any speed
 *              3.  Motor enabled and limited: allowed to run with speed below a threshold
 *              4.  Motor not enabled: motor is blocked to start
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 *
 */

// --- Include Files --------------------------------------------------------------------------------------------------
#include "SRSystemConfig.h"                         //!< System configuration
#include "Compiler_defs.h"                          //!< Compiler definition
#include "C_Extensions.h"                           //!< C extension
#include "SRAPI020Safety.h"                         //!< Class B API020 (Safety relevant communication)
#include "SRMCUSpeedMonitor.h"                      //!< Class B speed monitor header
#include "SRMCUSpeedMonitor_prv.h"                  //!< Class B speed monitor parameters
#include "Micro.h"                                  //!< Micro definition

// --- Private Declarations -------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

// -- Private Structure Type Declarations --


// --- Private Definitions --------------------------------------------------------------------------------------------

//! Convert the timer set from seconds to the routine ticks.
const uint8 SR_SPEED_MONITOR_TX_TIMER = (uint8)((1.0f/25.0f) * 1300); //!< Set to 1.3 seconds to prevent marginal timing
                                                                      //!< with SRAPI020.
                                                                      //!< Refer to SRAPI020_SPEED_MONITOR_TX_RATE_MS

#ifdef SR_REQUEST_CLASS_A_DEBOUNCER

    #if (SR_REQUEST_CLASS_A_DEBOUNCER >= 255)
        #error Debouncer must be smaller than or equal to 254. Sanity check of uint8 size to prevent overflow.
    #endif

    //! Debouncer used when class A requests unlock door speed fault
    const uint8 SR_REQUEST_CLASS_A_DEBOUNCER_RELOAD = (uint8)SR_REQUEST_CLASS_A_DEBOUNCER;
#endif

// -- Private Variable Definitions --

#ifdef SRMCU_SPEED_MONITOR_DEBUG

    #warning SRMCU_SPEED_MONITOR_DEBUG shall not be used in certification!!
    #warning SRMCU_SPEED_MONITOR_DEBUG shall not be used in certification!!
    #warning SRMCU_SPEED_MONITOR_DEBUG shall not be used in certification!!
    #warning SRMCU_SPEED_MONITOR_DEBUG shall not be used in certification!!
    #warning SRMCU_SPEED_MONITOR_DEBUG shall not be used in certification!!

    volatile signed char my_speed_limited;          //!< DEBUG variable - shall not be disabled in certification
    volatile signed char my_motor_enabled;          //!< DEBUG variable - shall not be disabled in certification
    volatile signed char my_api20_still_alive;      //!< DEBUG variable - shall not be disabled in certification
    volatile signed char my_high_speed_detected;    //!< DEBUG variable - shall not be disabled in certification

    volatile signed char my_fbk_speed_limited;      //!< DEBUG variable - shall not be disabled in certification
    volatile signed char my_fbk_motor_enabled;      //!< DEBUG variable - shall not be disabled in certification
    volatile signed char my_fbk_api20_still_alive;  //!< DEBUG variable - shall not be disabled in certification
    volatile signed char my_fbk_high_speed_detected;//!< DEBUG variable - shall not be disabled in certification

#endif


//! Flag TRUE/FALSE warning if high speed is found in the Class B motor control
uint8 SR_Speed_Monitor_High_Speed_Found;
//! Contains the data to be sent to MCU.
SRAPI020_SPEED_MONITOR_TX_BIT_TYPE SR_API020_Speed_Monitor_TX_Status;
//! Timer to reflect the timer. Count in routine tick
uint8 SR_Speed_Monitor_TX_Handler_Timer;

#ifdef SR_REQUEST_CLASS_A_DEBOUNCER
    //! Class A request - use only to set flags. no safety purpose - 0 - no request, else fault request debouncer
    uint8 Request_Set_Fault_From_ClassA;
#endif

//! Compliment of 2 of SR_Speed_Monitor_High_Speed_Found
uint8 NSR_Speed_Monitor_High_Speed_Found;
//! Compliment of 2 of NSR_API020_Speed_Monitor_TX_Status
SRAPI020_SPEED_MONITOR_TX_BIT_TYPE NSR_API020_Speed_Monitor_TX_Status;
//! Compliment of 2 of NSR_Speed_Monitor_TX_Handler_Timer
uint8 NSR_Speed_Monitor_TX_Handler_Timer;

#ifdef SR_REQUEST_CLASS_A_DEBOUNCER
    //! Class A request - use only to set flags. no safety purpose - 0 - no request, else fault request debouncer
    uint8 NRequest_Set_Fault_From_ClassA;
#endif

//  --- Private Function Prototypes -----------------------------------------------------------------------------------
static void Monitor_RX_Status_Reaction(void);
static void Monitor_TX_Status(void);

//! Macros to update safety relevant data
//! Update SR byte
#define SR_MCU_SM_UPDATE_BYTE(var, value)  {auto uint8 temp_sm_update_u8; (var) = (value); temp_sm_update_u8 = (uint8)~(value); (N##var) = temp_sm_update_u8;}
//! Update SR short
#define SR_MCU_SM_UPDATE_SHORT(var, value) {auto uint16 temp_sm_update_u16; (var) = (value); temp_sm_update_u16 = (uint16)~(value); (N##var) = temp_sm_update_u16;}
//!< Update SR long
#define SR_MCU_SM_UPDATE_LONG(var, value)  {auto uint32 temp_sm_update_u32; (var) = (value); temp_sm_update_u32 = (uint16)~(value); (N##var) = temp_sm_update_u32;}


//! Macros to check safety relevant data
//! Check SR byte
#define SR_MCU_SM_CHECK_BYTE(check,var)    {auto uint8 temp_u8;  temp_u8 = (uint8)(var); check += (temp_u8 !=  (uint8)(~(N##var)));}
//! Check SR short
#define SR_MCU_SM_CHECK_SHORT(check,var)   {auto uint16 temp_u16;  temp_u16 = (uint16)(var); check += (temp_u16 != (uint16)(~(N##var)));}
//! Check SR long
#define SR_MCU_SM_CHECK_LONG(check,var)    {auto uint32 temp_u32;  temp_u32 = (uint32)(var); check += (temp_u32 != (uint32)(~(N##var)));}


//=====================================================================================================================
//-------------------------------------------- Public Functions -------------------------------------------------------
//=====================================================================================================================



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize the module's variables.
 */
void SRMCUSpeedMonitor__Initialize(void)
{
    SRMCUSPEEDMONITOR_FLOW_INITIALIZE_BEGIN();

    SR_MCU_SM_UPDATE_BYTE(SR_Speed_Monitor_High_Speed_Found, TRUE); //lint !e64
    SR_MCU_SM_UPDATE_BYTE(SR_API020_Speed_Monitor_TX_Status, (SRAPI020_SPEED_MONITOR_TX_BIT_TYPE)0); //lint !e64
    SR_MCU_SM_UPDATE_BYTE(SR_Speed_Monitor_TX_Handler_Timer, 0); //lint !e64

#ifdef SR_REQUEST_CLASS_A_DEBOUNCER
    SR_MCU_SM_UPDATE_BYTE(Request_Set_Fault_From_ClassA, 0);
#endif

    SRMCUSPEEDMONITOR__STOP_MOTOR();

    SRMCUSPEEDMONITOR_FLOW_INITIALIZE_END();

#ifdef SRMCU_SPEED_MONITOR_DEBUG

    my_speed_limited = -1;
    my_motor_enabled = -1;
    my_api20_still_alive = -1;
    my_high_speed_detected = -1;

#endif

}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Handle the flags, status from ACU door lock and send updated status from MCU
 */
void SRMCUSpeedMonitor__SlowHandler(void)
{
    SRMCUSPEEDMONITOR_5MS_FLOW_HANDLER_BEGIN();

    Monitor_RX_Status_Reaction();
    Monitor_TX_Status();

    SRMCUSPEEDMONITOR_5MS_FLOW_HANDLER_END();
}




/*
 * @brief       Set or reset the warning that speed is above the threshold
 * @param[in]   high_speed_detected [0 - FALSE/Otherwise - TRUE] speed above the threshold
 *
 */
void SRMCUSpeedMonitor__High_Speed_Detected(uint8 high_speed_detected)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint32 check_integrity_status;

    check_integrity_status = FALSE;

    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
    SR_MCU_SM_CHECK_BYTE(check_integrity_status, SR_Speed_Monitor_High_Speed_Found);

    if(check_integrity_status == FALSE)
    {
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL()

        if(high_speed_detected)
        {
            SR_MCU_SM_UPDATE_BYTE(SR_Speed_Monitor_High_Speed_Found, TRUE); //lint !e64
        }
        else
        {
            SR_MCU_SM_UPDATE_BYTE(SR_Speed_Monitor_High_Speed_Found, FALSE); //lint !e64
        }
    }
    else
    {
        // Then block motor driving
        SRMCUSPEEDMONITOR__STOP_MOTOR();

        // Invoke Exception
        SRMCUSPEEDMONITOR__CORRUPTED_VAR();
    }
}




//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Set unlock door speed fault to be reported by SRAPI020.
 * @param[in]   None
 *
 */
void SRMCUSpeedMonitor__SetUnlockDoorFault(void)
{
    #ifdef SR_REQUEST_CLASS_A_DEBOUNCER

        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        uint32 check_integrity_status;

        check_integrity_status = FALSE;

        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        SR_MCU_SM_CHECK_BYTE(check_integrity_status, Request_Set_Fault_From_ClassA);
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL()

        if(check_integrity_status)
        {
        	// Integrity is corrupt
        	
            // Then block motor driving
            SRMCUSPEEDMONITOR__STOP_MOTOR();

            // Invoke Exception
            SRMCUSPEEDMONITOR__CORRUPTED_VAR();
        }
        else
        {
        	// Integrity is intact
        	
            SR_MCU_SM_UPDATE_BYTE(Request_Set_Fault_From_ClassA, SR_REQUEST_CLASS_A_DEBOUNCER_RELOAD);
        }

    #endif
}



//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Handle the status from API20 (ACU Door lock module) and MCU and take proper reaction to them.
 *
   @startuml{SRMCUSpeedMonitor_RX_Monitor.png}
       (*)  --> "Get SRAPI020 is valid/alive,\nmotor enabled and speed limited status flags"

       if "" then
           -left-> [neither alive nor valid] "motor enabled = FALSE"
           --> " " as cont1
       else
           --> [alive and valid] cont1
       endif

       if "" then
           --> [motor enabled] if "" then
                         --> [speed limited] if "" then
                                       --> [high speed\ndetected] "Invoke Class B motor control to force to stop motor" as force_stop
                                   else
                                       --> [high speed\nnot detected] "No action is taken" as no_action_taken
                                       --> (*)
                                   endif
                     else
                         --> [speed not limited] no_action_taken
                         --> (*)
                     endif
       else
           --> [motor disabled] force_stop
       endif

       force_stop --> (*)
   @enduml
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
    #pragma inline = forced
#endif
void Monitor_RX_Status_Reaction(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint32 check_integrity_status;
    uint8 motor_enabled;
    uint8 speed_limited;
    uint8 high_speed_detected;
    uint8 srapi20_alive;
    SRAPI020_SPEED_MONITOR_RX_BIT_TYPE speed_monitor_status_api20_rx;

    check_integrity_status = FALSE;

    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
    SR_MCU_SM_CHECK_BYTE(check_integrity_status, SR_Speed_Monitor_High_Speed_Found);

    // Variable is corrupted
    if(check_integrity_status == TRUE)
    {
        // Then block motor driving
        SRMCUSPEEDMONITOR__STOP_MOTOR();

        // Invoke Exception
        SRMCUSPEEDMONITOR__CORRUPTED_VAR();
    }

    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();


    // Check speed monitor package status from SRAPI020
    srapi20_alive = SRAPI020Safety__GetRxValue(API020_RX_SPEED_MONITOR_STATUS, &speed_monitor_status_api20_rx);

    //// Update status flags
    // from SRAPI020 Receiver
    motor_enabled = BIT_TEST(speed_monitor_status_api20_rx, SRAPI020_SPEED_MONITOR__RX_ENABLED);
    speed_limited = BIT_TEST(speed_monitor_status_api20_rx, SRAPI020_SPEED_MONITOR__RX_LIMITED);
    // from SRSafetyMotorMgr (Motor Control)
    high_speed_detected = SR_Speed_Monitor_High_Speed_Found;


#ifdef SRMCU_SPEED_MONITOR_DEBUG

    //////////// START debug code
    if(my_motor_enabled >= 0)           motor_enabled = my_motor_enabled;
    if(my_speed_limited >= 0)           speed_limited = my_speed_limited;
    if(my_api20_still_alive >= 0)       srapi20_alive = my_api20_still_alive;
    if(my_high_speed_detected >= 0)     high_speed_detected = my_high_speed_detected;

    my_fbk_speed_limited       = speed_limited;
    my_fbk_motor_enabled       = motor_enabled;
    my_fbk_api20_still_alive   = srapi20_alive;
    my_fbk_high_speed_detected = high_speed_detected;
    /////////// END debug code.

#endif


    // If communication is NOT alive and NOT valid
    // (SRAPI020 timeout, invalid sequence, invalid data, etc.)
    if( !srapi20_alive )
    {
        // disable motor driving because communication is invalid
        motor_enabled = FALSE;
    }


    //// Do logic with status flags
    // If motor driving enabled
    if(motor_enabled)
    {
        // if the speed is requested to be limited
        if(speed_limited)
        {
            // if the speed is detected to be above the threshold
            // (threshold is set in the SRMotorSafetyMngr)
            if(high_speed_detected)
            {
                // Then block motor driving
                SRMCUSPEEDMONITOR__STOP_MOTOR();
            }
        }
    }
    else // else motor driving is disabled
    {
        // Then block motor driving
        SRMCUSPEEDMONITOR__STOP_MOTOR();
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Handle the flags from the SR Motor control module and update the API20, ACU SRDoor lock module
 *
 *
   @startuml{SRMCUSpeedMonitor_Monitor_TX_Status.png}
       (*)  --> "Wait for publishing time"
       "Wait for publishing time" --> [not expired] "Wait for publishing time"
       --> [time expired] "Read unlock speed\nunlock speed failed and\nhigh speed flags" as read_flags
       if "   try enqueue data" then
           --> [enqueued] "Reload timer"
           --> (*)
       else
           --> [not enqueued] read_flags
       endif
   @enduml
 *
 */
#ifdef __IAR_SYSTEMS_ICC__
    #pragma inline = forced
#endif
static void Monitor_TX_Status(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint8 temp8;
    #ifdef SR_REQUEST_CLASS_A_DEBOUNCER
        uint8 temp8_1;
    #endif
    SRAPI020_SPEED_MONITOR_TX_BIT_TYPE temp_status;
    uint32 check_integrity_status;

    check_integrity_status = FALSE;

    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
    SR_MCU_SM_CHECK_BYTE(check_integrity_status, SR_Speed_Monitor_TX_Handler_Timer);
    SR_MCU_SM_CHECK_BYTE(check_integrity_status, SR_Speed_Monitor_High_Speed_Found);
    SR_MCU_SM_CHECK_BYTE(check_integrity_status, SR_API020_Speed_Monitor_TX_Status);

    #ifdef SR_REQUEST_CLASS_A_DEBOUNCER
        SR_MCU_SM_CHECK_BYTE(check_integrity_status, Request_Set_Fault_From_ClassA);
    #endif

    if(check_integrity_status == FALSE)
    {
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL()

        if(SR_Speed_Monitor_TX_Handler_Timer)
        {
        	temp8 = SR_Speed_Monitor_TX_Handler_Timer;
        	temp8--;
            SR_MCU_SM_UPDATE_BYTE(SR_Speed_Monitor_TX_Handler_Timer, temp8); //lint !e64
        }
        else
        {
            temp_status = SR_API020_Speed_Monitor_TX_Status;
            // Evaluate if high speed detected
            if(SR_Speed_Monitor_High_Speed_Found)
            {
                BIT_SET(temp_status, SRAPI020_SPEED_MONITOR__TX_HIGH_SPEED_FOUND);    //lint !e64
            }
            else
            {
                BIT_CLR(temp_status, SRAPI020_SPEED_MONITOR__TX_HIGH_SPEED_FOUND);    //lint !e64
            }

            // Evaluate if unlock door speed detected
            temp8 = SRMCUSPEEDMONITOR__UNLOCK_SPEED_DETECTED();
            if(temp8)
            {
                BIT_SET(temp_status, SRAPI020_SPEED_MONITOR__TX_UNLOCK_SPEED_FOUND);  //lint !e64
            }
            else
            {
                BIT_CLR(temp_status, SRAPI020_SPEED_MONITOR__TX_UNLOCK_SPEED_FOUND);  //lint !e64
            }

            // Evaluate if the unlock door speed detection failed
            temp8 = (SRMCUSPEEDMONITOR__UNLOCK_SPEED_FAILED() > 0);   // Return only 1 if get is not zero

            #ifdef SR_REQUEST_CLASS_A_DEBOUNCER

                // Debounce request from class A
                temp8_1 = Request_Set_Fault_From_ClassA;
                if(temp8_1)
                {
                    temp8_1--;
                    SR_MCU_SM_UPDATE_BYTE(Request_Set_Fault_From_ClassA, temp8_1);

                    BIT_SET(temp_status, SRAPI020_SPEED_MONITOR__TX_SHIFTER_FAILED); //lint !e64
                }
                else
                {
                    BIT_CLR(temp_status, SRAPI020_SPEED_MONITOR__TX_SHIFTER_FAILED); //lint !e64
                }

                // Do OR between fault from class B and request to fault from class A
                temp8 += temp8_1;
            #endif

            if(temp8)
            {
                BIT_SET(temp_status, SRAPI020_SPEED_MONITOR__TX_UNLOCK_SPEED_FAILED); //lint !e64
                BIT_CLR(temp_status, SRAPI020_SPEED_MONITOR__TX_UNLOCK_SPEED_FOUND);  //lint !e64
            }
            else
            {
                BIT_CLR(temp_status, SRAPI020_SPEED_MONITOR__TX_UNLOCK_SPEED_FAILED); //lint !e64
            }

            SR_MCU_SM_UPDATE_BYTE(SR_API020_Speed_Monitor_TX_Status, temp_status); //lint !e64

            // Handle the unlock door speed and unlock door speed failed flags...
            temp8 = SRAPI020Safety__SetTxValue(API020_TX_SPEED_MONITOR_STATUS, &SR_API020_Speed_Monitor_TX_Status);
            // If the TX command was properly stacked...
            if(temp8)
            {
                // Reload the TX timer
                SR_MCU_SM_UPDATE_BYTE(SR_Speed_Monitor_TX_Handler_Timer, SR_SPEED_MONITOR_TX_TIMER);
            }
        }
    }
    else
    {
        // Then block motor driving
        SRMCUSPEEDMONITOR__STOP_MOTOR();

        // Invoke Exception
        SRMCUSPEEDMONITOR__CORRUPTED_VAR();
    }
}

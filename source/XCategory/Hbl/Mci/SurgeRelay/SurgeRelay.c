/**
 *  @file       SurgeRelay.c
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SurgeRelay.h"
#include "C_Extensions.h"
#include "Gpio.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#ifndef SRAPI20_CHECK
#define SRAPI20_CHECK() TRUE
#else
SRAPI020_SPEED_MONITOR_RX_BIT_TYPE data_api20_rx;
#endif

#ifdef SURGE_RELAY_USED
    typedef enum
    {
        SURGE_INITIALIZATION,
        SURGE_WAITING,
        SURGE_CLOSE,
        SURGE_OPEN,
        SURGE_MANUAL_MODE
    } SURGE_STATES_TYPE;

    static SURGE_STATES_TYPE Surge_State;
    static unsigned short int Bus_Voltage;
    static unsigned short int Bus_Voltage_Previous;
    static unsigned char Bus_Voltage_Timer;
#endif


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifdef SURGE_RELAY_USED
    #define RELAY_CONFIG()  Gpio__PinConfig(SURGE_RELAY_PORT, SURGE_RELAY_PIN, OUTPUT_PUSHPULL)
    #define RELAY_OPEN()    Gpio__PinWrite(SURGE_RELAY_PORT, SURGE_RELAY_PIN, FALSE)
    #define RELAY_CLOSE()   Gpio__PinWrite(SURGE_RELAY_PORT, SURGE_RELAY_PIN, TRUE)

    #define BUS_BASE_TIME   (VBUS_HANDLER_CALLRATE_MS/25)
    #define VBUS_DV_DT      (VBUS_TOLERANCE_DV_DT / (1000/VBUS_HANDLER_CALLRATE_MS))
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#ifdef SURGE_RELAY_USED
    static unsigned char IsdVbusdtStable(void);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SurgeRelay and its variables
 *
 */
void SurgeRelay__Initialize(void)
{
#ifdef SURGE_RELAY_USED
    RELAY_CONFIG();
    Surge_State = SURGE_WAITING;
    Bus_Voltage_Previous = 0;
    Bus_Voltage_Timer = 1;                            // Force to execute the handler at first call.
    RELAY_OPEN();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handler the state machine which will control the surge relay. The use of Vbus voltage is required.
 *
 *  @param      None
 *  @return     None
 */
void SurgeRelay__25msHandler(void)
{

#ifdef SURGE_RELAY_USED
    Bus_Voltage_Timer--;
    if(!Bus_Voltage_Timer)
    {
        Bus_Voltage_Timer = BUS_BASE_TIME;
        Bus_Voltage = SURGERELAY_READ_DCBUS_VOLTAGE();
        switch(Surge_State)
        {
            case SURGE_WAITING:
                if( (IsdVbusdtStable())  &&
                    (Bus_Voltage >= VBUS_RELAY_MIN_WAIT_VOLTAGE)&&(SRAPI20_CHECK()==TRUE))
                {
                    RELAY_CLOSE();
                    Surge_State = SURGE_CLOSE;
                }
                break;

            case SURGE_CLOSE:
                if((Bus_Voltage < VBUS_RELAY_OPEN_VOLTAGE)||(SRAPI20_CHECK()==FALSE))
                {
                    RELAY_OPEN();
                    Surge_State = SURGE_OPEN;
                }
                else
                {
                    RELAY_CLOSE();                      // Do refresh on Surge relay pin.
                }
                break;

            case SURGE_OPEN:
                if(Bus_Voltage > VBUS_RELAY_CLOSE_VOLTAGE)
                {
                    Surge_State = SURGE_WAITING;
                }
                break;

            default:
                // This was left blank intentionally.
                break;
        }
    }
#endif



}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Returns if the DC bus voltage is already stabilized and set to go.
 *
 *  @param      None
 *  @return     TRUE - DC Bus ready
 *              FALSE - DC Bus not ready.
 */
unsigned char SurgeRelay__IsBusReady(void)
{
#ifdef SURGE_RELAY_USED
    unsigned char result;

    result = FALSE;

    if(Surge_State == SURGE_CLOSE)
    {
        result = TRUE;
    }
    else if(Surge_State == SURGE_MANUAL_MODE)
    {
        result = TRUE;
    }

    return(result);
#else
    return(TRUE);
#endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Manual operation of surge relay.
 *
 *  @param      [in] action SURGERELAY_OPEN = force open state
 *                          SURGERELAY_CLOSE = force closed state
 *                          SURGERELAY_NORMAL = returns to the state machine to control the output
 */
void SurgeRelay__Action(SURGERELAY_ACTION_TYPE action)
{
#ifdef SURGE_RELAY_USED
    if(action == SURGERELAY_OPEN)
    {
        // Force state change immediately
        Surge_State = SURGE_MANUAL_MODE;
        RELAY_OPEN();
    }
    else if(action == SURGERELAY_CLOSE)
    {
        // Force state change immediately
        Surge_State = SURGE_MANUAL_MODE;
        RELAY_CLOSE();
    }
    else if(action == SURGERELAY_NORMAL)
    {
        if(Surge_State == SURGE_MANUAL_MODE)
        {
            // Force state change immediately
            Surge_State = SURGE_WAITING;
            RELAY_OPEN();
        }
    }
#endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate the dVbus/dt and apply an hysteresis.
 *
 *  @param      None
 *  @return     TRUE - DC Bus voltage is stable
 *              FALSE - DC Bus voltage is not stable
 */
#ifdef SURGE_RELAY_USED
unsigned char IsdVbusdtStable(void)
{
    unsigned short int result;
    unsigned short int vbus;

    vbus = SURGERELAY_READ_DCBUS_VOLTAGE();

    if(vbus >= Bus_Voltage_Previous)
    {
        result = vbus - Bus_Voltage_Previous;
    }
    else
    {
        result = Bus_Voltage_Previous - vbus;
    }

    Bus_Voltage_Previous = vbus;

    if(result <= VBUS_DV_DT)
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }

    return(result);
}
#endif



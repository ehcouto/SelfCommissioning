/**
 * @file
 * @defgroup    CLASS_B
 * @brief       Private parameters for the SRAPI020Safety module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef SRAPI020SAFETY_PRV_H_
#define SRAPI020SAFETY_PRV_H_

#include "SRFlow.h"

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Variable Definitions --
static SRAPI020_SPEED_MONITOR_RX_BIT_TYPE API020_Door_Lock_RX_Status;


static SRAPI020_SPEED_MONITOR_RX_BIT_TYPE N_API020_Door_Lock_RX_Status;


// -- Private Flash-Based Constant Definitions --

#if (API020_NUMBER_OF_RX_CHANNELS > 0)
    //! An array of configuration data for each Receive Channel.
    //! Refer to the #API020_RX_CONFIG_TYPE structure for more details.
    const API020_RX_CONFIG_TYPE API020_RX_CHANNEL[] =
    {
    //  {Address,                       N_Address,                      Period,                                 Op_Code,    Node,       Length}
        {&API020_Door_Lock_RX_Status,   &N_API020_Door_Lock_RX_Status,  SRAPI020_SPEED_MONITOR_RX_RATE_MS,      2,          1,          sizeof(SRAPI020_SPEED_MONITOR_RX_BIT_TYPE)}
    };
#endif      // #if (API020_NUMBER_OF_RX_CHANNELS > 0)


#if (API020_NUMBER_OF_TX_CHANNELS > 0)
    //! An array of configuration data for each Transmit Channel.
    //! Refer to the #API020_TX_CONFIG_TYPE structure for more details.
    const API020_TX_CONFIG_TYPE API020_TX_CHANNEL[] =
    {
    //  {Period,                                Op_Code,    Node,       Length}
        {SRAPI020_SPEED_MONITOR_TX_RATE_MS,     2,          1,          1}
    };
#endif      // #if (API020_NUMBER_OF_TX_CHANNELS > 0)


//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Macro Declarations --

//! Macro used by the ClassB framework to ensure that the API020 module is initialized at the correct point in the initialization sequence.
#define API020_INIT_TASK_SEQUENCE()         SRFlow__InitLogEvent(SRFLOW_SRAPI020_INIT_BEGIN)



#endif      // SRAPI020SAFETY_PRV_H_

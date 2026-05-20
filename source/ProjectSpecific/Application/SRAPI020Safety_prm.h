/**
 * @file
 * @defgroup    CLASS_B
 * @brief       Public parameters for the SRAPI020Safety module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef SRAPI020SAFETY_PRM_H_
#define SRAPI020SAFETY_PRM_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "API019ProductInfo.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --

//! Bit definition for the RX speed monitor
typedef enum
{
    SRAPI020_SPEED_MONITOR__RX_ENABLED = 0,         //!< 0 - door status from ACU is closed - enables motor control
    SRAPI020_SPEED_MONITOR__RX_LIMITED,             //!< 1 - lock status from ACU is locked
} SRAPI020_SPEED_MONITOR_RX_BIT_TYPE;


//! Bit definition for the TX speed monitor status
typedef enum
{
    SRAPI020_SPEED_MONITOR__TX_HIGH_SPEED_FOUND = 0,//!< 0 - high speed is found
    SRAPI020_SPEED_MONITOR__TX_UNLOCK_SPEED_FOUND,  //!< 1 - unlock door speed is found
    SRAPI020_SPEED_MONITOR__TX_UNLOCK_SPEED_FAILED, //!< 2 - unlock door speed failed
} SRAPI020_SPEED_MONITOR_TX_BIT_TYPE;


#define SRAPI020_SPEED_MONITOR_RX_RATE_MS     8000  //!< [ms] update rate of RX channel
#define SRAPI020_SPEED_MONITOR_TX_RATE_MS     1800  //!< [ms] update rate of TX channel




//! The number of Receive Channels used by this appliance category.
//! This value must match the number of Receive Channels defined in the SRAPI020Safety_prv.h file.
#define API020_NUMBER_OF_RX_CHANNELS 1

//! The number of Transmit Channels used by this appliance category.
//! This value must match the number of Transmit Channels defined in the SRAPI020Safety_prv.h file.
#define API020_NUMBER_OF_TX_CHANNELS 1

//! The appliance category.
#define API020_PLATFORM PLATFORM_HA_WASHER


// -- Public Enumerated Constant Declarations --

#if (API020_NUMBER_OF_RX_CHANNELS > 0)
    //! An enumerated list of names used by the application to reference the Receive Channels.
    typedef enum API020_RX_CHANNEL_INDEX_ENUM
    {
        API020_RX_SPEED_MONITOR_STATUS
    } API020_RX_CHANNEL_INDEX_TYPE;
#endif



#if (API020_NUMBER_OF_TX_CHANNELS > 0)
    //! An enumerated list of names used by the application to reference the Transmit Channels.
    typedef enum API020_TX_CHANNEL_INDEX_ENUM
    {
        API020_TX_SPEED_MONITOR_STATUS
    } API020_TX_CHANNEL_INDEX_TYPE;
#endif



//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // SRAPI020SAFETY_PRM_H_

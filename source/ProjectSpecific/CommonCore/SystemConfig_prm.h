/**
 *  @file
 *
 *  @brief      System configuration definitions.
 *
 *  $Header: SystemConfig.h 1.2 2015/07/01 17:33:20EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/01 17:33:20EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SYSTEMCONFIG_H_
#define SYSTEMCONFIG_H_

#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- GENERAL     -------------------------------------------------
//=====================================================================================================================
#define SYSTEM_HMI_HANDLING             DISABLED
#define SYSTEM_ACU_HANDLING             DISABLED
#define SYSTEM_MCU_HANDLING             ENABLED

/**
 * This definition enables/disables the handling of API012Exp on Reveal and on SRMain
 */
#define SYSTEM_EXPANSION_HANDLING       DISABLED

//! ENABLED if this board is an Expansion ACU.
#define SYSTEM_ACU_EXPANSION_HANDLING   DISABLED

//! ENABLED if this board is an Expansion HMI.
#define SYSTEM_HMI_EXPANSION_HANDLING	DISABLED

/**
 * This definition enables/disables the integration of connectivity APIs and handling.
 */
#define SYSTEM_CONNECTIVITY_HANDLING    DISABLED

#define API013_SERVER_FEATURE           ENABLED

#define API020SAFETY_FEATURE            ENABLED


#define SFUPDATER_MANAGE_SF_ID_DATA     ENABLED

#define API011APPCTRL_FEATURE           ENABLED

#define API220MOTORCONTROL_FEATURE		ENABLED

//-------------------
// WIN Configuration
//-------------------
#include "Uart.h"

#define WIN_MCU_ADDRESS        0

//! Node address for the ACU
#define WIN_ACU_ADDRESS        1

//! Node address for the HMI
#define WIN_UI_ADDRESS         4

#define WIN_UI_EXP_ADDRESS     5

#define FLASH_ALIGNMENT		   4




#define WIN_PORT                COM0
#define WIN_NODE_DEF            WIN_MCU_ADDRESS

#define PLATFORM_HA_WASHER      0

#endif // SYSTEMCONFIG_H_

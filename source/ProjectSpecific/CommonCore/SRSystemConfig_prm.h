/**
 *  @file       SRSystemConfig_prm.h
 *
 *  @brief      Public (prm) configuration file for SRSystemConfig module
 *
 *  @defgroup	CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRSYSTEMCONFIG_PRM_H_
#define SRSYSTEMCONFIG_PRM_H_
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! ENABLED to enable CLASS B features
#define		CLASSB_FEATURE			        DISABLED

//! ENABLED to send or receive Class B content over Reveal.
#define     API020SAFETY_FEATURE            ENABLED

//! DISABLED to disable Low Power feature (enabled by default)
#define		LOW_POWER_FEATURE				DISABLED


//TODO: TEMPORARY SOLUTION -> TO AVOID TASKSEQUENCE USAGE
#ifndef SRCLOCKMONITOR_FEATURE
    #define SRCLOCKMONITOR_FEATURE      ENABLED
#endif

#ifndef SYSTEM_ENABLE_ISRMONITOR
    #define SYSTEM_ENABLE_ISRMONITOR    ENABLED
#endif


//---------------------------------------------------------------------------------------------------------------------
// LLI - Define the type and number of LLIs on this board (Not defined or defined as 0 will unlink the feature).
//---------------------------------------------------------------------------------------------------------------------
#define HBL_LLI_NUM_SRANALOG        0
#define HBL_LLI_NUM_SRFEEDBACK      0
#define HBL_LLI_NUM_SRTRIACFEEDBACK 0
#define HBL_LLI_NUM_SRINPUTCAPTURE  0

//---------------------------------------------------------------------------------------------------------------------
// SRPilots - Define the type and number of Pilots on this board (Not defined or defined as 0 will unlink the feature).
//---------------------------------------------------------------------------------------------------------------------
#define HBL_PILOT_NUM_SRDOORLOCKEM			0
#define HBL_PILOT_NUM_SRDOORLOCKWAX			0
#define HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI	0


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRSYSTEMCONFIG_PRM_H_

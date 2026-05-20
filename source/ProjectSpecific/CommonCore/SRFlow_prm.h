/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      Public Parameters for SRFlow module
 *
 *
 *  $Header: SRFlow_prm.h 1.2 2015/11/17 08:50:15CET Maciej  Kobuszewski (KOBUSM) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/11/17 08:50:15CET $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef SRFLOW_PRM_H_
	#define SRFLOW_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#include "SRMC_defs.h"

typedef enum
{
    SRFLOW_SRMICRO_INIT_BEGIN,
    SRFLOW_SRMICRO_INIT_END,
    SRFLOW_SRCLOCKMONITOR_INIT,
    SRFLOW_SRMOTORSAFETYMGR_INIT_BEGIN,
    SRFLOW_SRMOTORSAFETYMGR_INIT_END,
    SRFLOW_SRMCUSPEEDMONITOR_INIT_BEGIN,
    SRFLOW_SRMCUSPEEDMONITOR_INIT_END,
    SRFLOW_SRAPI020_INIT_BEGIN,
    // Mandatory SRFLOW_CLEAR_INIT_ALL must be at the end of the list & don't change it
    SRFLOW_CLEAR_INIT_ALL = 0XFF
} SRFLOW_INIT_EVENT_TYPE;


typedef enum
{
    SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_BEGIN,
    SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_END,
    SRFLOW_SRRAMTEST_MAIN_BEGIN,
    SRFLOW_SRRAMTEST_MAIN_END,
    SRFLOW_SRFLASHTEST_MAIN_BEGIN,
    SRFLOW_SRFLASHTEST_MAIN_END,
    SRFLOW_SRMOTORSAFETYMGR_25MS_MAIN_BEGIN,
    SRFLOW_SRMOTORSAFETYMGR_25MS_MAIN_END,
    SRFLOW_SRMCUSPEEDMONITOR_5MS_MAIN_BEGIN,
    SRFLOW_SRMCUSPEEDMONITOR_5MS_MAIN_END,
    // Mandatory SRFLOW_CLEAR_MAIN_ALL must be at the end of the list & don't change it
    SRFLOW_CLEAR_MAIN_ALL = 0XFF
}SRFLOW_MAIN_EVENT_TYPE;





typedef enum
{
    SRFLOW_SRMOTORSAFETYMGR_250US_ISR_BEGIN,
    SRFLOW_SRMOTORSAFETYMGR_250US_ISR_END,
    SRFLOW_SRCLOCKMONITOR_1MS_ISR,
    SRFLOW_SRMOTORSAFETYMGR_1MS_ISR_BEGIN,
    SRFLOW_SRMOTORSAFETYMGR_1MS_ISR_END,
    // Mandatory SRFLOW_CLEAR_ISR_ALL must be at the end of the list & don't change it
    SRFLOW_CLEAR_ISR_ALL = 0XFF
}SRFLOW_ISR_EVENT_TYPE;

typedef enum
{
    SRFLOW_LOWPOWER_1,
    SRFLOW_LOWPOWER_2,
    SRFLOW_LOWPOWER_3,
    // Mandatory SRFLOW_LOWPOWER_CLEAR_ALL must be at the end of the list & don't change it
    SRFLOW_LOWPOWER_CLEAR_ALL = 0XFF
}SRFLOW_LOWPOWER_EVENT_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRFLOW_PRM_H_



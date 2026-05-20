/**
 *  @defgroup CLASS_B
 *  @file
 *
 *  @brief      Private parameters for SRFlow module
 *
 *
 *
 *  @copyright  Copyright 2011-. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef SRFLOW_PRV_H_
	#define SRFLOW_PRV_H_
#include "SRMC_defs.h"
#include "SRException.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define SRFLOW_NUM_INIT_EVENTS       8

const SRFLOW_INIT_EVENT_TYPE SRFLOW_EVENT_INIT_LIST[SRFLOW_NUM_INIT_EVENTS] = {SRFLOW_SRMICRO_INIT_BEGIN,
                                                                               SRFLOW_SRMICRO_INIT_END,
                                                                               SRFLOW_SRCLOCKMONITOR_INIT,
                                                                               SRFLOW_SRMOTORSAFETYMGR_INIT_BEGIN,
                                                                               SRFLOW_SRMOTORSAFETYMGR_INIT_END,
                                                                               SRFLOW_SRMCUSPEEDMONITOR_INIT_BEGIN,
                                                                               SRFLOW_SRMCUSPEEDMONITOR_INIT_END,
                                                                               SRFLOW_SRAPI020_INIT_BEGIN,
                                                                                };
#define SRFLOW_NUM_MAIN_EVENTS      18

const SRFLOW_MAIN_EVENT_TYPE SRFLOW_EVENT_MAIN_LIST[SRFLOW_NUM_MAIN_EVENTS] = {SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_BEGIN,    // Every Slot
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_END,
                                                                               SRFLOW_SRRAMTEST_MAIN_BEGIN,               // Slot 0
                                                                               SRFLOW_SRRAMTEST_MAIN_END,
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_BEGIN,    // Every Slot
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_END,
                                                                               SRFLOW_SRFLASHTEST_MAIN_BEGIN,             // Slot 1
                                                                               SRFLOW_SRFLASHTEST_MAIN_END,
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_BEGIN,    // Every Slot
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_END,
                                                                               SRFLOW_SRMOTORSAFETYMGR_25MS_MAIN_BEGIN,   // Slot 2
                                                                               SRFLOW_SRMOTORSAFETYMGR_25MS_MAIN_END,
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_BEGIN,    // Every Slot
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_END,
                                                                               SRFLOW_SRMCUSPEEDMONITOR_5MS_MAIN_BEGIN,
                                                                               SRFLOW_SRMCUSPEEDMONITOR_5MS_MAIN_END,
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_BEGIN,    // Every Slot
                                                                               SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_END};


#define SRFLOW_NUM_ISR_EVENTS       11

const SRFLOW_ISR_EVENT_TYPE SRFLOW_EVENT_ISR_LIST[SRFLOW_NUM_ISR_EVENTS] = {SRFLOW_SRMOTORSAFETYMGR_250US_ISR_BEGIN,         // Every Slot
                                                                            SRFLOW_SRMOTORSAFETYMGR_250US_ISR_END,
                                                                            SRFLOW_SRCLOCKMONITOR_1MS_ISR,                   // Slot 0
                                                                            SRFLOW_SRMOTORSAFETYMGR_250US_ISR_BEGIN,         // Every Slot
                                                                            SRFLOW_SRMOTORSAFETYMGR_250US_ISR_END,
                                                                            SRFLOW_SRMOTORSAFETYMGR_250US_ISR_BEGIN,         // Every Slot
                                                                            SRFLOW_SRMOTORSAFETYMGR_250US_ISR_END,
                                                                            SRFLOW_SRMOTORSAFETYMGR_1MS_ISR_BEGIN,           // Slot 2
                                                                            SRFLOW_SRMOTORSAFETYMGR_1MS_ISR_END,
                                                                            SRFLOW_SRMOTORSAFETYMGR_250US_ISR_BEGIN,         // Every Slot
                                                                            SRFLOW_SRMOTORSAFETYMGR_250US_ISR_END,
                                                                            };

#define SRFLOW_NUM_LOWPOWER_EVENTS  4
const SRFLOW_LOWPOWER_EVENT_TYPE SRFLOW_EVENT_LOWPOWER_LIST[SRFLOW_NUM_LOWPOWER_EVENTS] = {  SRFLOW_LOWPOWER_1,
                                                                                        SRFLOW_LOWPOWER_2,
                                                                                        SRFLOW_LOWPOWER_3};



#define SRFLOW_INIT_SEQUENCE_EXCEPTION()        SRException__Queue(EXCEPTION_INIT_FLOW,1,0)
#define SRFLOW_INIT_INCOMPLETE_EXCEPTION()      SRException__Queue(EXCEPTION_INIT_FLOW,2,0)
#define SRFLOW_INIT_OVERFILLED_EXCEPTION()      SRException__Queue(EXCEPTION_INIT_FLOW,3,0)

#define SRFLOW_MAIN_SEQUENCE_EXCEPTION()        SRException__Queue(EXCEPTION_MAIN_FLOW,1,0)
#define SRFLOW_MAIN_INCOMPLETE_EXCEPTION()      SRException__Queue(EXCEPTION_MAIN_FLOW,2,0)
#define SRFLOW_MAIN_OVERFILLED_EXCEPTION()      SRException__Queue(EXCEPTION_MAIN_FLOW,3,0)

#define SRFLOW_ISR_SEQUENCE_EXCEPTION()         SRException__Queue(EXCEPTION_ISR_FLOW,1,0)
#define SRFLOW_ISR_INCOMPLETE_EXCEPTION()       SRException__Queue(EXCEPTION_ISR_FLOW,2,0)
#define SRFLOW_ISR_OVERFILLED_EXCEPTION()       SRException__Queue(EXCEPTION_ISR_FLOW,3,0)

#define SRFLOW_LOWPOWER_SEQUENCE_EXCEPTION()         //SRException__Queue(EXCEPTION_LOWPOWER_FLOW,1,0)
#define SRFLOW_LOWPOWER_INCOMPLETE_EXCEPTION()       //SRException__Queue(EXCEPTION_LOWPOWER_FLOW,2,0)
#define SRFLOW_LOWPOWER_OVERFILLED_EXCEPTION()       //SRException__Queue(EXCEPTION_LOWPOWER_FLOW,3,0)

#endif // SRFLOW_PRV_H_



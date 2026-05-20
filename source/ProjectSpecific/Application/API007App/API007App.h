/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2013-$Date: 2013/09/13 06:18:50EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007APP_H_
#define API007APP_H_

#include "API007App_prm.h"
#include "API007Data_prm.h"
#include "API007Data.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define MONITORING_NVDATA_SEGMENTS 		DISABLED

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void API007App__Initialize(void);
void API007App__Handler(void);

BOOL_TYPE API007App__PrepareSegment(API007APP_SEGMENT_ID segment_id,uint16 index,uint16 size);
uint8* API007App__IsDataReady(API007APP_SEGMENT_ID segment, uint16 index);
void API007App__ReceivedTransmissionCompleted(API007APP_SEGMENT_ID segment_id);

BOOL_TYPE API007App__WriteSegmentData(API007APP_SEGMENT_ID segment_id ,uint16 index ,uint8 * data ,uint8 size);
BOOL_TYPE API007App__IsWriteEngineFree(API007APP_SEGMENT_ID segment_id);
void API007App__ServerwriteCompleted(uint8 node);

API007_SEGMENT_INFO_TYPE * API007App__GetSegmentInfo(API007APP_SEGMENT_ID segment_id);
void API007App__SetSegmentInfo(uint8 segment_id, API007_SEGMENT_ROLE_TYPE role, uint16 size, uint16 crc);
BOOL_TYPE API007App__AreAllDataReceived(void);

#endif

// #endif // API007APP_H_



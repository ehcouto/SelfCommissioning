/**
 *  @file
 *
 *  @brief      Standard API007Data module used for bulk data transfer across boards using Reveal.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: API007Data.h 1.6 2015/06/25 14:33:52EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2012-$Date: 2015/06/25 14:33:52EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007DATA_H_
#define API007DATA_H_

#include "C_Extensions.h"
#include "API007Data_prm.h"
#include "Reveal.h"
#include "SystemConfig.h"
#include "SettingFile.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define API007DATA_NUM          0x7
#define API007DATA_TYPE         5
#define API007DATA_VERSION      0
#define API007DATA_INSTANCES    1

//! Reveal Priority Queue
#ifndef API007DATA_USE_PRIORITY_QUEUE
    #define API007DATA_USE_PRIORITY_QUEUE               DISABLED
#endif

#ifndef API007DATA_BULK_DATA_FEATURE
    #define API007DATA_BULK_DATA_FEATURE    			DISABLED
#endif

// Handler is expected to be called from communication thread every 5ms
#ifndef COMMUNICATION_THREAD_CALL_RATE_MS
    #define COMMUNICATION_THREAD_CALL_RATE_MS               5
#endif

#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
    typedef enum
    {
        API007_SEGMENT_ROLE_CLIENT,                         //00 - Segment ID role as Client
        API007_SEGMENT_ROLE_SERVER                          //01 - Segment ID role as Server
    } API007_SEGMENT_ROLE_TYPE;

    typedef struct
    {
        uint16  crc;                                        // CRC 16
        uint16  size;                                       // Index size, 1 Index = 10 bytes of data
        API007_SEGMENT_ROLE_TYPE segment_role;              // 00 client, 01 server
    } API007_SEGMENT_INFO_TYPE;
#endif

typedef enum
{
    API007_ERROR_NO_ERROR = 0,                           //Error - No response from external node for successful write of the segment data.
    API007_ERROR_NO_RESPONSE,                           //Error - No response from external node for successful write of the segment data.
    API007_ERROR_WRITING,                               //Error - Segment write more than define size or undefined memory.
    API007_ERROR_BUSY_WRITING,                          //Error - Busy for writing the previous received data.
    API007_ERROR_WRITE_ON_SERVER,                       //Error - Writing on Server Segment
    API007_ERROR_SEGMENT_NOT_FOUND,                     //Error - Segment ID not found
    API007_ERROR_SEGMENT_DATA_CORRUPTED                 //Error  - If the CRC of the stored data did not match
}API007DATA_ERROR_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void API007Data__Initialize(void);
void API007Data__Handler(void);

REVEAL_RECIPE_STATUS_TYPE API007Data__CommandParser(REVEAL_MSG_TYPE * buffer);
void API007Data__FeedbackParser(REVEAL_MSG_TYPE * buffer);
void API007Data__AckParser(uint8 address,uint8 api,uint8 opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode);

BOOL_TYPE API007Data__RequestSettingFileIDInfo(uint8 address, SF_ID_TYPE sf_id);
BOOL_TYPE API007Data__RequestSettingFileIDData(uint8 address,SF_ID_TYPE sf_id, uint16 data_index, uint8 size);

BOOL_TYPE API007Data__RequestSettingFileDisplacementInfo(uint8 address, SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement);
BOOL_TYPE API007Data__RequestSettingFileDisplacementData(uint8 address, SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement, uint16 data_index, uint8 size);

BOOL_TYPE API007Data__RequestSettingFileTableSize(uint8 address);
BOOL_TYPE API007Data__RequestSettingFileTable(uint8 address,uint16 data_index, uint8 size);

//TODO : This below 2 API's will be deprecated in next release
BOOL_TYPE API007Data__RequestSettingFileInfo(uint8 address);
BOOL_TYPE API007Data__RequestSettingData(uint8 address,uint32 data_index, uint8 size);

//Below API's are valid only when the API007DATA_BULK_DATA_FEATURE
#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
    BOOL_TYPE API007__RequestSegmentsList(uint8 address);
    BOOL_TYPE API007__RequestInfoSegment(uint8 address, uint8 segment);
    BOOL_TYPE API007__RequestGetDataSegment(uint8 address, uint8 segment);
    BOOL_TYPE API007Data__PublishSegmentData(uint8 address,uint8 segment, uint16 index, uint8 * data, uint8 size);
    void API007Data__SendTransmissionComplete(uint8 address);
    void API007Data__SendError(uint8 address ,uint8 segment_id, API007DATA_ERROR_TYPE error);
    void API007Data__WriteRequestCompleted(uint8 address);
#endif

#endif // API007DATA_H_



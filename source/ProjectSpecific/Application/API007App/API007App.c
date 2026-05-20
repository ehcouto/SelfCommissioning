 /**
 *  @file       
 *
 *  @brief      API007App module implementation
 *
 *  @details    This module takes care of reading and writing mechanism to the memory for bulk data storage.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *
 *  @copyright  Copyright 2013-$Date: 2013/09/13 10:23:07EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Log.h"
#include "API007Data.h"
#include "API007App_prm.h"
#include "Reveal.h"
#include "Crc16.h"
#include "Micro.h"
#include "Mode.h"
#include "String.h"
#include "SystemTimers.h"
#include "Micro.h"
#include "DataFlash.h"
#include "API007App_prv.h"
#include "API007App.h"


#if (API007DATA_BULK_DATA_FEATURE == ENABLED)

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
typedef enum
{
	API007APP_MACHINE_REQ_INIT  = 0,
	API007APP_MACHINE_REQ_SETUP = 1,
	API007APP_MACHINE_REQ_ID0 = 2,
	API007APP_MACHINE_REQ_ID1 = 3,
	API007APP_MACHINE_REQ_ID2 = 4,
	API007APP_MACHINE_REQ_ID3 = 5,
	API007APP_MACHINE_REQ_ID4 = 6,
    API007APP_MACHINE_REQ_ID5 = 7,
    API007APP_MACHINE_REQ_ID6 = 8,

	API007APP_MACHINE_REQ_ID_MAX
} API007APP_MACHINE_REQUESTOR_TYPE;

//Add here segments Definition with segment ID's
#define API007APP_SEG0      0
#define API007APP_SEG1      1
#define API007APP_SEG2      2
#define API007APP_SEG3      3
#define API007APP_SEG4      4
#define API007APP_SEG6      6


//! Max size of segment on server
#define API007APP_SERVER_DATA_SEGMENT0_MAX_SIZE         50

#define TIMER_MACHINE_REQUESTOR_SETUP     		250
#define TIMER_MACHINE_REQUESTOR_DATAS     		500

#define API007DATA_INVALID_SEGMENT_INDEX        255
#define MESSAGE_RESPONSE_TIMER                  5000
#define MESSAGE_RESPONSE_TIMER_REQ              5000


//! list of module Log messages
typedef enum
{
     INVALID_CRC_FAILED = 1                          //!< data: INVALID_CRC_FAILED
} MODULE_API007APP_LOG_MESSAGE_ID_TYPE;
#define MODULE_NAME MODULE_API007APP

typedef enum
{
    API007APP_RX_STATE_IDLE = 0,
    API007APP_RX_STATE_WRITE = 1,
    API007APP_RX_STATE_WAIT = 2,
    API007APP_RX_STATE_COMPLETE = 3,
    API007APP_RX_STATE_ERROR = 4
} API007APP_RX_STATE_TYPE;

//! Flags to check the flow of the data sent by ACU to MCU.
typedef union API007_DATA_FLOW_CHECK_STRUCT
{
    struct
    {
        uint8 Friction_Factory_Received		:1;
        uint8 Friction_Lifetime_Received	:1;
        uint8 Floor_Displ_Ref_Received		:1;
        uint8 Vib_Map_Cross_Spd_Received	:1;
        uint8 Unused						:4;
    } Bits;

    uint8 All;
} API007_DATA_FLOW_CHECK_TYPE;

static API007_SEGMENT_INFO_TYPE Segment_Info;

static API007_DATA_FLOW_CHECK_TYPE Data_Flow_Check;


#define BASELINE_ARRAY_DIM						(BASELINE_ARRAY_MAX_SIZE * sizeof(uint16))

#define BASELINE_MAX_SIZE 						(BASELINE_ARRAY_MAX_SIZE * sizeof(uint16))
#define BASELINE_MAX_SIZE_CRC					(BASELINE_MAX_SIZE + 2)

#define STATS_EXTRACT_ARRAY_MAX_SIZE             17U
#define STATS_EXTRACTS_ARRAY_MAX_SIZE_CRC        (STATS_EXTRACT_ARRAY_MAX_SIZE + 2)


static uint8 Array_CpyExtrAccelVibMap[4];
static uint8 Array_StatsExtrData[STATS_EXTRACT_ARRAY_MAX_SIZE];
static uint16 Message_States_Timer;
static API007APP_MACHINE_REQUESTOR_TYPE State_Machine_Requestor;

static uint16 Message_Response_Request;
static API007APP_RX_STATE_TYPE API007APP_Rx_State;
static uint16 API007APP_Rx_Size;
static uint8*  Pnt_Rx_Buffer;


#if (MONITORING_NVDATA_SEGMENTS == ENABLED)
extern uint32 API007_CLIENT_NV_DATA$$Base;
extern uint32 API007_CLIENT_NV_DATA$$Limit;

static uint8 * const  API007_CLIENT_NV_DATA_START =               (uint8 * )(&API007_CLIENT_NV_DATA$$Base);
static const uint32   API007_CLIENT_NV_DATA_LIMIT =               (uint32)(&API007_CLIENT_NV_DATA$$Limit);

extern uint32 API007_CLIENT_NV_DATA_DEFAULTS$$Base;
extern uint32 API007_CLIENT_NV_DATA_DEFAULTS$$Limit;
static uint8 * const  API007_CLIENT_NV_DATA_DEFAULTS_START =               (uint8 * )(&API007_CLIENT_NV_DATA_DEFAULTS$$Base);
static const uint32   API007_CLIENT_NV_DATA_DEFAULTS_LIMIT =               (uint32)(&API007_CLIENT_NV_DATA_DEFAULTS$$Limit);
#endif


#define API007_CLIENT_NV_DATA_SIZE ((uint8)(API007_CLIENT_NV_DATA_LIMIT - (uint32)API007_CLIENT_NV_DATA_START))

#define API007_CLIENT_NV_DATA_DEFAULTS_SIZE ((uint8)(API007_CLIENT_NV_DATA_DEFAULTS_LIMIT - (uint32)API007_CLIENT_NV_DATA_DEFAULTS_START))

//! Possible states of Client state machine.
typedef enum
{
    REQUEST_SEGMENT_INFO,
    WAITING_FOR_SERVER_ALIVE,
    WAITING_FOR_SEGMENT_INFO,
    REQUEST_DATA,
    WAIT_FOR_DATA_RECEPTION,
    RESTORE_DEFAULTS,
    MONITOR_DATA_CHANGES,
    SEND_UPDATED_DATA,
    WAITING_FOR_SERVER_COMPLETED_WRITING,
    NUM_OF_CLIENT_STATE_MACHINE_STATES
}API007APP_CLIENT_STATE_MACHINE;

//! Defines number of loops that Client waits for data. After timeout elapsed Client restores default values.
#define WATING_FOR_DATA_TIMEOUT_LOOPS       250

//! Crc check call rate
#define CRC_CHECK_CALL_RATE      50


typedef enum INDEX_EXTRACTION_STATISTICS
{
    INDEX_EXTR_UNBALANCE_LOW_SPEED_0               = 0,
    INDEX_EXTR_UNBALANCE_LOW_SPEED_1               = 1,
    INDEX_EXTR_FRICTION_HIGH_SPEED_0               = 2,
    INDEX_EXTR_FRICTION_HIGH_SPEED_1               = 3,
    INDEX_EXTR_VISCOUS_FRICTION_HIGH_SPEED_0       = 4,
    INDEX_EXTR_VISCOUS_FRICTION_HIGH_SPEED_1       = 5,
    INDEX_EXTR_MAX_CABINET_ACCELERATION_0          = 6,
    INDEX_EXTR_MAX_CABINET_ACCELERATION_1          = 7,
    INDEX_EXTR_SPEED_AT_MAX_CABINET_ACCELERATION_0       = 8,
    INDEX_EXTR_SPEED_AT_MAX_CABINET_ACCELERATION_1       = 9,
    INDEX_EXTR_MAX_FLOOR_DISPLACEMENT_0                 = 10,
    INDEX_EXTR_MAX_FLOOR_DISPLACEMENT_1                 = 11,

    INDEX_EXTR_REBAL_CNT_LOW_SPEED               = 12,
    INDEX_EXTR_REBAL_CNT_HIGH_SPEED              = 13,
    INDEX_EXTR_CEM_EVENT_COUNT                   = 14,
    INDEX_EXTR_SOFT_CAB_HIT_COUNT                = 15,
    INDEX_EXTR_HARD_CAB_HIT_COUNT                =16,
    EXTRACTION_STATISTICS_SIZE
} INDEX_EXTRACTION_STATISTICS_TYPE;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
//=====================================================================================================================
static void PrepareExtStatisticsData (void);
static void ResetInitDatas (void);

//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module API007App and its variables
 *
 */
void API007App__Initialize(void)
{
	if (SystemTimers__GetState(Message_States_Timer) == SYSTEMTIMERS_STATE_INVALID)
	{
		Message_States_Timer  = SystemTimers__GetHandle(SYSTEMTIMERS_TYPE_DOWN_COUNTER);
	}

	ResetInitDatas();
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief Writing and Reading handlers
 *
 */

void API007App__Handler (void)
{
	if (BIT_TEST(Reveal__GetNodestatus(), WIN_ACU_ADDRESS) == TRUE)
	{
		if (SystemTimers__GetState(Message_States_Timer) == SYSTEMTIMERS_STATE_DONE)
		{
			Message_Response_Request--;

			if (State_Machine_Requestor == API007APP_MACHINE_REQ_INIT)
			{
				if (Mode__GetMode() == MODE_NORMAL)
				{
					API007__RequestSegmentsList (WIN_ACU_ADDRESS);
					API007__RequestInfoSegment (WIN_ACU_ADDRESS, API007APP_SEG0);
					API007__RequestInfoSegment (WIN_ACU_ADDRESS, API007APP_SEG1);

					API007__RequestInfoSegment (WIN_ACU_ADDRESS, API007APP_SEG3);
					API007__RequestInfoSegment (WIN_ACU_ADDRESS, API007APP_SEG4);
					API007__RequestInfoSegment (WIN_ACU_ADDRESS, API007APP_SEG6);

					SystemTimers__SetMs(Message_States_Timer, TIMER_MACHINE_REQUESTOR_SETUP);
					State_Machine_Requestor = API007APP_MACHINE_REQ_SETUP;
				}
			}
			else if (State_Machine_Requestor >= API007APP_MACHINE_REQ_SETUP)
			{
				switch (State_Machine_Requestor)
				{
					case API007APP_MACHINE_REQ_SETUP:
					{
						API007__RequestGetDataSegment(WIN_ACU_ADDRESS, API007APP_SEG0);
						SystemTimers__SetMs(Message_States_Timer, TIMER_MACHINE_REQUESTOR_SETUP);
					}
					break;
					case API007APP_MACHINE_REQ_ID0:
					{
						API007__RequestGetDataSegment(WIN_ACU_ADDRESS, API007APP_SEG1);
						SystemTimers__SetMs(Message_States_Timer, TIMER_MACHINE_REQUESTOR_DATAS);
					}
					break;
					case API007APP_MACHINE_REQ_ID1:
					{
						API007__RequestGetDataSegment(WIN_ACU_ADDRESS, API007APP_SEG3);
						SystemTimers__SetMs(Message_States_Timer, TIMER_MACHINE_REQUESTOR_DATAS);
					}
					break;
					case API007APP_MACHINE_REQ_ID3:
					{
						API007__RequestGetDataSegment(WIN_ACU_ADDRESS, API007APP_SEG4);
						SystemTimers__SetMs(Message_States_Timer, TIMER_MACHINE_REQUESTOR_DATAS);
					}
					break;
					case API007APP_MACHINE_REQ_ID4:
					{
						API007__RequestGetDataSegment(WIN_ACU_ADDRESS, API007APP_SEG6);
						SystemTimers__SetMs(Message_States_Timer, TIMER_MACHINE_REQUESTOR_DATAS);
					}
					break;
					case API007APP_MACHINE_REQ_ID6:
					{
						State_Machine_Requestor = API007APP_MACHINE_REQ_ID_MAX;
					}
					break;
					case API007APP_MACHINE_REQ_ID_MAX:
					{
						Message_Response_Request = MESSAGE_RESPONSE_TIMER_REQ;
					}
					break;

					default:
						break;
				}
			}
		}
		else if ((State_Machine_Requestor < API007APP_MACHINE_REQ_ID_MAX) && (Message_Response_Request == 0))
		{
			State_Machine_Requestor = API007APP_MACHINE_REQ_INIT;
			Message_Response_Request = MESSAGE_RESPONSE_TIMER_REQ;
		}
	}
	else
	{
		ResetInitDatas();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Exposes the information about a segment_id in this node.
 * @param segment_id   Segment ID for a single logical block of data.
 * @return pointer to the information about the requested segment_id.
 */
API007_SEGMENT_INFO_TYPE * API007App__GetSegmentInfo(API007APP_SEGMENT_ID segment_id)
{
	Segment_Info.crc = 0;

    switch (segment_id)
    {
        case API007APP_SEGMENT_0:
        case API007APP_SEGMENT_2:
        case API007APP_SEGMENT_3:

        break;

        case API007APP_SEGMENT_4:
        {
        	Segment_Info.segment_role = API007_SEGMENT_ROLE_SERVER;
            Segment_Info.size = sizeof(float32);  	//last 2 byte are used for storing the Crc value
        }
        break;

        case API007APP_SEGMENT_6:
        {
            Segment_Info.segment_role = API007_SEGMENT_ROLE_SERVER;
            Segment_Info.size = STATS_EXTRACT_ARRAY_MAX_SIZE;    //last 2 byte are used for storing the Crc value
        }
        break;

        default:
            break;
    }

    return (&Segment_Info);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method prepares the segment_id ready to read segment_id data from the defined segment_id ID address
 *
 */
BOOL_TYPE API007App__PrepareSegment(API007APP_SEGMENT_ID segment_id,uint16 index,uint16 size)
{
    BOOL_TYPE ret = FALSE;

    switch (segment_id)
    {
        case API007APP_SEGMENT_0:
        case API007APP_SEGMENT_1:
        case API007APP_SEGMENT_2:
        case API007APP_SEGMENT_3:
        case API007APP_SEGMENT_4:
        case API007APP_SEGMENT_6:
            //Set address pointer to read a memory.
            ret = TRUE;
            break;

        default:
            break;
    }

    return (ret);
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method write the segment_id data received from a server to the defined segment_id ID location.
 * @param buffer contains the Wide package that contains the message
 */
uint8* API007App__IsDataReady(API007APP_SEGMENT_ID segment, uint16 index)
{
	float32 extr_accel_vibmap;

    uint8* ret_pnt = NULL;


    switch (segment)
    {
        case API007APP_SEGMENT_0:

			break;

        case API007APP_SEGMENT_2:

			break;

        case API007APP_SEGMENT_3:

			break;

    	case API007APP_SEGMENT_4:

            break;

        case API007APP_SEGMENT_6:
            {
                PrepareExtStatisticsData();
                ret_pnt = (&Array_StatsExtrData[0]);
            }
            break;

        default:
            ret_pnt = (NULL);
            break;
    }

    return (ret_pnt);
}



BOOL_TYPE API007App__WriteSegmentData(API007APP_SEGMENT_ID segment_id ,uint16 index ,uint8 * data ,uint8 size)
{
BOOL_TYPE ret_value = FALSE;

    if (API007APP_Rx_State == API007APP_RX_STATE_IDLE)
    {
        // API007APP_Rx_Segment = segment_id;
        // API007APP_Rx_Segment_Index = GetSegmentIndex(segment_id);
        if (segment_id != API007DATA_INVALID_SEGMENT_INDEX)
        {
			API007APP_Rx_Size = size;
			API007APP_Rx_State = API007APP_RX_STATE_IDLE;

			Pnt_Rx_Buffer = data;

			// API007Data__WriteRequestCompleted(APPI007APP_SERVER_NODE_ID);
			ret_value = TRUE;
        }
        else
        {
            // Publish error that segment ID not found for this node
            // API007Data__SendError(address, segment, API007_ERROR_SEGMENT_NOT_FOUND);
        }
    }
    else
    {   // Publish an error busy writing
        // API007Data__SendError(address, segment, API007_ERROR_BUSY_WRITING);
    }

    return ret_value;
}




//----------------------------------------------------------------------------------------------------------
/**
 * brief Received End of Transmission command from external node segment_id (server) ID side.
 * @param node
 */
void API007App__ReceivedTransmissionCompleted (API007APP_SEGMENT_ID segment_id)
{
    switch (segment_id)
    {
        case API007APP_SEGMENT_0:
#if (API007APP_SERVER_FEATURE == ENABLED)
#endif //(API007APP_SERVER_FEATURE == ENABLED)
            break;

        default:

            break;
    }
 }

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method manages the status of the writing mechanism
 *        The writing scheme could be writing on the External EEPROM or Internal Flash memory or RAM memory
 *
 * @param segment_id
 * @return
 */
BOOL_TYPE API007App__IsWriteEngineFree(API007APP_SEGMENT_ID segment_id)
{
float32 ExtrAccelVibMap;
BOOL_TYPE ret_value = FALSE;

    if (Pnt_Rx_Buffer != NULL)
	{
		switch (segment_id)
		{                  					//Copy/Write the received segment ID data from Rx_Buffer to defined memory location.
			case API007APP_SEGMENT_0:
			{
			   Data_Flow_Check.Bits.Friction_Factory_Received = 1;
			   State_Machine_Requestor=API007APP_MACHINE_REQ_ID0;
			   ret_value = TRUE;
			}
			break;

			case API007APP_SEGMENT_1:
			   Data_Flow_Check.Bits.Friction_Lifetime_Received = 1;
			   State_Machine_Requestor=API007APP_MACHINE_REQ_ID1;
			   ret_value = TRUE;
			   break;

			case API007APP_SEGMENT_3:
			   Data_Flow_Check.Bits.Floor_Displ_Ref_Received = 1;
			   State_Machine_Requestor=API007APP_MACHINE_REQ_ID3;
			   ret_value = TRUE;
			   break;

			case API007APP_SEGMENT_4:
				memcpy((void*)&ExtrAccelVibMap, Pnt_Rx_Buffer, sizeof(float32));
				Data_Flow_Check.Bits.Vib_Map_Cross_Spd_Received = 1;
				State_Machine_Requestor=API007APP_MACHINE_REQ_ID4;
				ret_value = TRUE;
			   break;

	         case API007APP_SEGMENT_6:
	            State_Machine_Requestor=API007APP_MACHINE_REQ_ID6;
	            ret_value = TRUE;
	               break;

			default:
			   State_Machine_Requestor=API007APP_MACHINE_REQ_ID_MAX;
			   break;
		}
	}

    return ret_value;
}

//----------------------------------------------------------------------------------------------------------
/**
 *
 * @param segment_id
 * @param role
 * @param size
 * @param crc
 */
void API007App__SetSegmentInfo(uint8 segment_id, API007_SEGMENT_ROLE_TYPE role, uint16 size, uint16 crc)
{
    Segment_Info.crc = crc;
    Segment_Info.segment_role = role;
    Segment_Info.size = size;
}

//----------------------------------------------------------------------------------------------------------
/**
 *
 * @param node
 */
void API007App__ServerwriteCompleted(uint8 node)
{
    API007Data__SendTransmissionComplete(APPI007APP_SERVER_NODE_ID);
}


BOOL_TYPE API007App__AreAllDataReceived(void)
{
	BOOL_TYPE ret_value;

	ret_value = FALSE;

	if (Data_Flow_Check.Bits.Friction_Factory_Received && Data_Flow_Check.Bits.Friction_Lifetime_Received &&
		Data_Flow_Check.Bits.Floor_Displ_Ref_Received && Data_Flow_Check.Bits.Vib_Map_Cross_Spd_Received)
	{
		ret_value = TRUE;
	}

	return ret_value;
}

//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================

static void ResetInitDatas (void)
{
	if (Message_States_Timer != SYSTEMTIMERS_FAIL)
	{
		SystemTimers__SetMs(Message_States_Timer, TIMER_MACHINE_REQUESTOR_SETUP);
		State_Machine_Requestor = API007APP_MACHINE_REQ_INIT;
	}
	else
	{
		State_Machine_Requestor = API007APP_MACHINE_REQ_ID_MAX;
	}

	Pnt_Rx_Buffer = NULL;
	Message_Response_Request = MESSAGE_RESPONSE_TIMER_REQ;
	API007APP_Rx_State = API007APP_RX_STATE_IDLE;
}

static void PrepareExtStatisticsData(void)
{


}

#endif




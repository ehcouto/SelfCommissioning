/**
 * @file
 * @defgroup    CLASS_B
 * @brief       API020 Safety is used to transfer Class B data over a Class A communication bus.
 *
 * @details     The strategy of this module is to define receive (RX) and transmit (TX) Class B
 *              data channels through the configuration files.
 *
 *              Receive (RX) Channels:
 *
 *                  The SRAPI020Safety module keeps track of the received messages on the
 *                  communications bus and ensures that the received data is valid. A Class B
 *                  module can call SRAPI020Safety__GetRxValue() at any time to check if there is
 *                  any valid data from the remote node.
 *
 *                  The SRAPI020Safety module considers the RX channel valid when the following
 *                  conditions are met:
 *                      - The last message was received within the timeout period.
 *                      - The time between the last two received messages was less than the
 *                          timeout period.
 *                      - The last two received messages were sequential messages.
 *
 *              Transmit (TX) Channels:
 *
 *                  The SRAPI020Safety module manages the sequence and timing for transmitted
 *                  messages. A Class B module must call SRAPI020Safety__SetTxValue() periodically
 *                  in order to maintain a valid sequence of messages. The time between calls must
 *                  be less than the timeout specified in the configuration.
 *
 *                  The SRAPI020Safety module sends the messages with valid sequence numbers as
 *                  long as the SRAPI020Safety__SetTxValue() function is called with the
 *                  appropriate timing.
 *
 *              The receiver will not consider the value valid until two sequential message have
 *              been received. The sender can send two sequential messages in rapid succession to
 *              force this condition.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SRSystemConfig.h"

#ifndef API020SAFETY_FEATURE
    // API020SAFETY_FEATURE should be ENABLED for applications that must transfer Class B data over Reveal.
    #error API020SAFETY_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif

#if (API020SAFETY_FEATURE == ENABLED)

#include "SRAPI020Safety.h"
#include "SRAPI020Safety_prv.h"

// -- Other Modules --
#include "Crc8.h"
#include <limits.h>
#include "Micro_Defs.h"
#include "SRData.h"
#include "SRTimerReference.h"
#include <string.h>
#include "Win.h"


#if (API020_NUMBER_OF_RX_CHANNELS > 0)
    // The number of receive channels specified must match the number defined in API020_RX_CHANNEL.
    CT_ASSERT(API020_NUMBER_OF_RX_CHANNELS == ELEMENTS_IN_ARRAY(API020_RX_CHANNEL));
#endif
#if (API020_NUMBER_OF_TX_CHANNELS > 0)
    // The number of transmit channels specified must match the number defined in API020_TX_CHANNEL.
    CT_ASSERT(API020_NUMBER_OF_TX_CHANNELS == ELEMENTS_IN_ARRAY(API020_TX_CHANNEL));
#endif
#ifndef WIN_TOOL_ADDRESS
    #define WIN_TOOL_ADDRESS 14
#endif


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! The number of bytes that each API020 payload consumes as overhead when transferring safety data.
#define API020_PACKET_OVERHEAD (sizeof(API020_SAFETY_PACKET_TYPE) - MAX_CLASSB_DATA_SIZE)

//! The maximum size of Class B Safety Data on a single Transmit Channel or Receive Channel.
#define MAX_CLASSB_DATA_SIZE 11

#if (API020_NUMBER_OF_TX_CHANNELS > 0)
    //! The first Sequence Number used on a Transmit Channel after a reset.
	#define SEQUENCE_RESET 0
#endif

//! The Sequence Number that sequentially follows the SEQUENCE_LAST Sequence Number.
//! This is also the first Sequence Number that represents a potentially valid sequence.
#define SEQUENCE_FIRST 1

//! The last Sequence Number before having to restart counting at a lower value.
#define SEQUENCE_LAST 253

#if (API020_NUMBER_OF_TX_CHANNELS > 0)
	//! A Sequence Number that represents that the Transmit Channel did not satisfy the timing requirements.
	#define SEQUENCE_LOST 254
#endif

//! A Sequence Number used internally to represent an invalid Sequence Number.
//! This Sequence Number is not expected in the API020 messages.
#define SEQUENCE_INVALID 255


// -- Private Structure Type Declarations --

//! The format of a data packet.
typedef struct API020_SAFETY_PACKET_STRUCT
{
    uint8 Sequence;
    uint8 Src_Addr;
    uint8 Dst_Addr;
    uint8 Data[MAX_CLASSB_DATA_SIZE + sizeof(uint8) /* sizeof(Crc8) */];
} API020_SAFETY_PACKET_TYPE;

//! A structure that declares all the variables used by this module.
typedef struct API020_DATA_STRUCT
{
    #if (API020_NUMBER_OF_RX_CHANNELS > 0)
        // The time of the last received message for each Receive Channel.
        uint32 Rx_Time[API020_NUMBER_OF_RX_CHANNELS];

        // The expected sequence number for the next received message on each Receive Channel.
        uint8 Rx_Expected_Sequence[API020_NUMBER_OF_RX_CHANNELS];

        // A flag that indicates that the Receive Channel has satisfied all sequence requirements.
        uint8 Rx_Sequence_Valid[API020_NUMBER_OF_RX_CHANNELS];

		#if (API020_USE_RX_ARRAY == ENABLED)
        /* current number of messages stored in the array */
        uint8 Rx_Num_Of_Readings[API020_NUMBER_OF_RX_CHANNELS];

        /* 1 = array overflowed; 0 = no overflow */
        uint8 Rx_Overflowed[API020_NUMBER_OF_RX_CHANNELS];
		#endif
    #endif
    #if (API020_NUMBER_OF_TX_CHANNELS > 0)
        // The time of the last transmitted message for each Transmit Channel.
        uint32 Tx_Time[API020_NUMBER_OF_TX_CHANNELS];

        // The expected sequence number for the next transmitted message on each Transmit Channel.
        uint8 Tx_Next_Sequence[API020_NUMBER_OF_TX_CHANNELS];
    #endif
} API020_DATA_TYPE;

//  --- Private Definitions -------------------------------------------------------------------------------------------

#if (API020_NUMBER_OF_RX_CHANNELS > 0)
	/*
	 * Returns if the message Period time has been exceeded.
	 * @param rx_channel = The Receive Channel whose Class B Safety Data is requested.
	 * @param now the current free running timer ticks
	 * @return TRUE time between messages is valid
	 * @return FALSE the time between messages has exceeded the maximum Period
	 */
	#define IS_MESSAGE_PERIOD_VALID(rx_channel, now)\
		( (now - Api020.Rx_Time[rx_channel] <= API020_RX_CHANNEL[rx_channel].Period) ? TRUE : FALSE )
#endif

// -- Private Variable Definitions --

//! The variables used by this module.
static API020_DATA_TYPE Api020;

//! Inverted memory for the variables used by this module.
static API020_DATA_TYPE N_Api020;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

#if (API020_NUMBER_OF_RX_CHANNELS > 0)
static void ResetRxChannel(API020_RX_CHANNEL_INDEX_TYPE rx_channel);
static void SaveSafetyData(API020_RX_CHANNEL_INDEX_TYPE rx_channel, uint8 sequence, void* data);
	#if (API020_USE_RX_ARRAY == ENABLED)
	static void WriteToArray(API020_RX_CHANNEL_INDEX_TYPE rx_channel, void * data);
	#endif
#endif

//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the module's variables.
 */
void SRAPI020Safety__Initialize(void)
{
    memset(&Api020, 0, sizeof(Api020));
    memset(&N_Api020, ~0, sizeof(N_Api020));
    #if (API020_NUMBER_OF_RX_CHANNELS > 0)
        memset(&Api020.Rx_Expected_Sequence, ~0, sizeof(Api020.Rx_Expected_Sequence));
        memset(&N_Api020.Rx_Expected_Sequence, 0, sizeof(N_Api020.Rx_Expected_Sequence));
    #endif      // #if (API020_NUMBER_OF_RX_CHANNELS > 0)

    API020_INIT_TASK_SEQUENCE();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This method parses API020 feedbacks sent to a broadcast or exclusively to this node.
 * The function will ignore messages that do not satisfy the stringent Class B requirements:
 *      - The opcode of the message must match the opcode used by the Receive Channel
 *      - The sequence number must match the expected sequence number based on the previously received sequence number
 *      - The message must have originated from the expected source address for the Receive Channel
 *      - The source address in the message must match the expected source address for the Receive Channel
 *      - The destination address must match this node's bus address.
 *      - The safety data must be 0 to 11 bytes
 *      - The CRC at the end of the payload must be correct.
 * The function will only accept the data as valid if:
 *      - The last two messages were sequential (no lost message)
 *      - The time between the last two messages was less than the Receive Channel's timeout period.
 * @param fbk = Pointer to the received feedback message.
 */
void SRAPI020Safety__FeedbackParser(REVEAL_MSG_TYPE * fbk)
{
    #if (API020_NUMBER_OF_RX_CHANNELS > 0)
        uint8 size = fbk->size;
        if ((API020_PACKET_OVERHEAD <= size) && (size <= sizeof(API020_SAFETY_PACKET_TYPE)))
        {
            // Check for a correct CRC, a potentially valid Sequence Number, and a correct destination node.
            uint8 crc8 = Crc8(UCHAR_MAX, fbk->payload, size - 1);
            API020_SAFETY_PACKET_TYPE* packet = (API020_SAFETY_PACKET_TYPE*)(void*)fbk->payload;
            if ((fbk->payload[size - 1] == crc8) &&         // if CRC is valid, and...
                (packet->Sequence != SEQUENCE_INVALID) &&   // if sequence is potentially valid, and...
                (packet->Dst_Addr == WIN_NODE_DEF))         // if message is intended for this bus address
            {
                // Look for the RX channel used to receive this Class B data.
                for (API020_RX_CHANNEL_INDEX_TYPE rx_channel = (API020_RX_CHANNEL_INDEX_TYPE)0; rx_channel < API020_NUMBER_OF_RX_CHANNELS; rx_channel++)
                {
                    API020_RX_CONFIG_TYPE const* config = &API020_RX_CHANNEL[rx_channel];
                    if (((fbk->node_id == config->Node) ||
                         (fbk->node_id == WIN_TOOL_ADDRESS)) &&                     // if received from expected node, and...
                        (fbk->op_code == config->Op_Code) &&                        // if received expected opcode, and...
                        (fbk->size == API020_PACKET_OVERHEAD + config->Length) &&   // if received expected number of bytes, and...
                        ((packet->Src_Addr == config->Node) ||
                         (packet->Src_Addr == WIN_TOOL_ADDRESS)))                   // if source node in the message matches expected source node.
                    {
                        SaveSafetyData(rx_channel, packet->Sequence, packet->Data);
                        break;                      // Break out of the for() loop.
                    }
                }
            }
        }
    #endif      // #if (API020_NUMBER_OF_RX_CHANNELS > 0)
}


#if (API020_NUMBER_OF_RX_CHANNELS > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the most recently received Class B Safety Data if it's validity has been maintained.
 * @param rx_channel = The Receive Channel whose Class B Safety Data is requested.
 * @param data = Pointer to a buffer to fill with the secure Class B Safety Data.
 * @return TRUE if and only if the Receive Channel messages have preserved the sequence.
 */
BOOL_TYPE SRAPI020Safety__GetRxValue(API020_RX_CHANNEL_INDEX_TYPE rx_channel, void* data)
{
    if ( rx_channel < ELEMENTS_IN_ARRAY(API020_RX_CHANNEL) )
    {
    	uint32 now = SRTimerReference__GetTimer();

    	MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    	MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        // Check whether the RX channel has preserved the required message sequence.
        SR_CHECK_BYTE(Api020.Rx_Sequence_Valid[rx_channel]);
        if (Api020.Rx_Sequence_Valid[rx_channel] == TRUE)
        {
            // Determine how long it has been since the last received message.
            SR_CHECK_LONG(Api020.Rx_Time[rx_channel]);
            if ( IS_MESSAGE_PERIOD_VALID(rx_channel, now) == FALSE )
            {
            	ResetRxChannel(rx_channel);
            }
            else
            {
                // Return the recently received data.
				API020_RX_CONFIG_TYPE const* config = &API020_RX_CHANNEL[rx_channel];
                (void)SRData__CheckArray(config->Address, config->N_Address, config->Length);
                memcpy(data, config->Address, config->Length);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                return TRUE;
            }
        }
    	MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }
    return FALSE;
}

#if (API020_USE_RX_ARRAY == ENABLED)

/**
 * Publishes the API020 messages for the specified Rx channel.
 * The private array is cleared when this API function is called.
 * @param rx_channel = Specifies which RX channel to read. The specified Rx channel must support message arrays.
 * @param message_array = Pointer to an array to fill with the recently received data.
 * 						  Array pointed to should be equal than or greater to the array size defined in
 * 						  API020_RX_CHANNEL[rx_channel].Array_Size.
 * @param messages_received = Number of messages received since the last call to this function.
 *                            Returns 0 if no messages have been received.
 * @retval TRUE buffer has overflowed since previous read
 * @retval FALSE buffer has not overflowed prior to being read
 *
 *	@startuml{SRAPI020Safety_Get_Buffer.jpg}
 *		title SRAPI020Safety Get Buffer
 *
 *		participant Application as App
 *		participant SRAPI020Safety as Api020
 *
 *		App -> Api020 : SRAPI020Safety__GetRxArray(rx_channel, *message_array, *messages_received)
 *		alt any function argument is invalid
 *			Api020 -> Api020 : set *messages_received = 0
 *			App <-- Api020 : FALSE
 *		else function arguments are valid
 *			Api020 -> Api020 : write all the messages in the private array to *message_array
 *			Api020 -> Api020 : *messages_received = number of messages in *message_array
 *			Api020 -> Api020 : clear private buffer
 *			alt buffer has overflowed since previous call to SRAPI020Safety__GetRxArray()
 *				App <-- Api020 : TRUE
 *			else buffer has not overflowed since previous call to SRAPI020Safety__GetRxArray()
 *				App <-- Api020 : FALSE
 *			end
 *		end
 *	@enduml
 */
BOOL_TYPE SRAPI020Safety__GetRxArray(API020_RX_CHANNEL_INDEX_TYPE rx_channel, void* message_array, uint8* messages_received)
{
	if (( rx_channel >= ELEMENTS_IN_ARRAY(API020_RX_CHANNEL) ) ||
		( API020_RX_CHANNEL[rx_channel].Array_Size < 2 ))
	{
		*messages_received = 0;
		return ( FALSE );
	}

	API020_RX_CONFIG_TYPE const * config = &API020_RX_CHANNEL[rx_channel];
	uint32 now = SRTimerReference__GetTimer();

	MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
	MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

	/* write the Error_Value to the array if the Rx_Time has exceeded the Period for the channel */
	SR_CHECK_LONG(Api020.Rx_Time[rx_channel]);
	if ( IS_MESSAGE_PERIOD_VALID(rx_channel, now) == FALSE )
	{
		WriteToArray(rx_channel, config->Error_Value);
		ResetRxChannel(rx_channel);
	}

	MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
	MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

	/* get the current number of readings in the array and reset the static RAM variable */
	SR_CHECK_BYTE(Api020.Rx_Num_Of_Readings[rx_channel]);
	uint8 num_of_readings = Api020.Rx_Num_Of_Readings[rx_channel];
	SR_SET_BYTE(Api020.Rx_Num_Of_Readings[rx_channel], 0);

	/* get if the array has overflowed since the previous read and reset the static RAM variable */
	SR_CHECK_BYTE(Api020.Rx_Overflowed[rx_channel]);
	uint8 overflowed = Api020.Rx_Overflowed[rx_channel];
	SR_SET_BYTE(Api020.Rx_Overflowed[rx_channel], 0);

	/* write the array to the application only if there are readings in the array */
	if ( num_of_readings > 0 )
	{
		SRData__CheckArray(config->Address, config->N_Address, num_of_readings * config->Length);
		memcpy(message_array, config->Address, num_of_readings * config->Length);
	}

	MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

	*messages_received = num_of_readings;
	return ( overflowed != 0 ? TRUE : FALSE );
}

/**
 * Returns the Error_Value for the specified Rx channel.
 * @param rx_channel = Specifies which RX channel to read. The specified Rx channel must support message arrays.
 * @retval void * pointer to the memory address at which the value of Error_Value is stored
 */
void * SRAPI020Safety__GetRxErrorValue(API020_RX_CHANNEL_INDEX_TYPE rx_channel)
{
	return ( rx_channel >= ELEMENTS_IN_ARRAY(API020_RX_CHANNEL) ? NULL : API020_RX_CHANNEL[rx_channel].Error_Value );
}

#endif /* #if (API020_USE_RX_ARRAY == ENABLED) */

#endif /* #if (API020_NUMBER_OF_RX_CHANNELS > 0) */






#if (API020_NUMBER_OF_TX_CHANNELS > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Transmit the next sequential message on the specified Transmit Channel.
 * @param tx_channel = The Transmit Channel on which the message should be transmitted.
 * @param data = Pointer to the data to send on the transmit channel.
 * @return TRUE if and only if Reveal was able to build and queue the message.
 */
BOOL_TYPE SRAPI020Safety__SetTxValue(API020_TX_CHANNEL_INDEX_TYPE tx_channel, void* data)
{
    BOOL_TYPE result = FALSE;
    if (tx_channel < API020_NUMBER_OF_TX_CHANNELS)
    {
        // Determine the next Sequence Number value.
        (void)SR_CHECK_BYTE(Api020.Tx_Next_Sequence[tx_channel]);
        uint8 sequence = Api020.Tx_Next_Sequence[tx_channel];

        // Determine the time since the last message was sent.
        (void)SR_CHECK_LONG(Api020.Tx_Time[tx_channel]);
        uint32 now = SRTimerReference__GetTimer();

        // If it has been too long since the last message, then break the sequence.
        API020_TX_CONFIG_TYPE const* config = &API020_TX_CHANNEL[tx_channel];
        if (sequence != SEQUENCE_RESET)
        {
            uint32 milliseconds = now - Api020.Tx_Time[tx_channel];
            if (milliseconds >= config->Period)
            {
                sequence = SEQUENCE_LOST;
            }
        }

        // Transmit the packet.
        if (config->Node == WIN_NODE_DEF)
        {
            #if (API020_NUMBER_OF_RX_CHANNELS > 0)
                // Find the receive channel that receives this safety data.
                for (API020_RX_CHANNEL_INDEX_TYPE rx_channel = (API020_RX_CHANNEL_INDEX_TYPE)0; rx_channel < API020_NUMBER_OF_RX_CHANNELS; rx_channel++)
                {
                    if ((API020_RX_CHANNEL[rx_channel].Node == WIN_NODE_DEF) &&
                        (API020_RX_CHANNEL[rx_channel].Op_Code == config->Op_Code) &&
                        (API020_RX_CHANNEL[rx_channel].Length == config->Length))
                    {
                        SaveSafetyData(rx_channel, sequence, data);
                        result = TRUE;
                        break;                      // Break out of the for() loop.
                    }
                }
            #endif      // #if (API020_NUMBER_OF_RX_CHANNELS > 0)
        }
        else
        {
            // Build the packet for this transmit channel.
            API020_SAFETY_PACKET_TYPE packet;
            packet.Sequence = sequence;
            packet.Src_Addr = WIN_NODE_DEF;
            packet.Dst_Addr = config->Node;
            memcpy(&packet.Data, data, config->Length);
            uint8 size = API020_PACKET_OVERHEAD + config->Length;
            packet.Data[config->Length] = Crc8(UCHAR_MAX, (uint8*)(void*)&packet, size - 1);

            REVEAL_COMMAND_TYPE* msg = Reveal__PriorityNewFeedback(API020SAFETY_NUM, config->Op_Code, config->Node, size);
            if ((Reveal__AddDataU8Buffer(msg, (uint8*)(void*)&packet, size) == PASS) &&
                (Reveal__QueueCommand(msg) == TRUE))
            {
                result = TRUE;
            }
        }
        if (result == TRUE)
        {
            // Successfully passed to receiving node, so update transmit variables.
            sequence = (sequence < SEQUENCE_LAST) ? sequence + 1 : SEQUENCE_FIRST;
            SR_SET_BYTE(Api020.Tx_Next_Sequence[tx_channel], sequence);
            SR_SET_LONG(Api020.Tx_Time[tx_channel], now);
        }
    }
    return result;
}
#endif      // #if (API020_NUMBER_OF_TX_CHANNELS > 0)


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


#if (API020_NUMBER_OF_RX_CHANNELS > 0)

//---------------------------------------------------------------------------------------------------------------------
/**
 * Save the received safety data.
 * @param rx_channel = The receive channel to which the safety data is related.
 *                     Used as an index into the API020_RX_CHANNEL[rx_channel] array.
 * @param sequence = The sequence of the newly received data.
 * @param data = The safety data that was received.
 */
static void ResetRxChannel(API020_RX_CHANNEL_INDEX_TYPE rx_channel)
{
	SR_SET_BYTE(Api020.Rx_Expected_Sequence[rx_channel], SEQUENCE_INVALID);
	SR_SET_BYTE(Api020.Rx_Sequence_Valid[rx_channel], FALSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Save the received safety data.
 * @param rx_channel = The receive channel to which the safety data is related.
 *                     Used as an index into the API020_RX_CHANNEL[rx_channel] array.
 * @param sequence = The sequence of the newly received data.
 * @param data = The safety data that was received.
 */
static void SaveSafetyData(API020_RX_CHANNEL_INDEX_TYPE rx_channel, uint8 sequence, void* data)
{
    API020_RX_CONFIG_TYPE const* config = &API020_RX_CHANNEL[rx_channel];

    // Determine if the Class B packet sequence was preserved by this message.
    SR_CHECK_BYTE(Api020.Rx_Expected_Sequence[rx_channel]);
    SR_CHECK_LONG(Api020.Rx_Time[rx_channel]);
    uint32 now = SRTimerReference__GetTimer();
    uint8 sequence_preserved = (( sequence == Api020.Rx_Expected_Sequence[rx_channel] ) &&	/* is the sequence id valid AND */
    							( IS_MESSAGE_PERIOD_VALID(rx_channel, now) == TRUE ));		/* is the time between messages valid */

	#if (API020_USE_RX_ARRAY == ENABLED)
    if ( API020_RX_CHANNEL[rx_channel].Array_Size >= 2 )
    {
    	/* if the sequence id and time have been met, write the data to the array, else write the Error_Value */
    	WriteToArray(rx_channel, sequence_preserved ? data : config->Error_Value);
    }
    else /* config->Array_Size < 2 */
	/* write the data to a variable */
	#endif
    {
		if ( sequence_preserved )
		{
			// This is a valid packet received in the expected sequence.
			// Save the received data.
			SRData__UpdateArray(config->Address, config->N_Address, data, config->Length);
		}
    }

    // Determine the next expected Sequence Number for this RX channel.
    sequence = (sequence < SEQUENCE_LAST) ? sequence + 1 : SEQUENCE_FIRST;

    // Save the RX channel status information.
    SR_SET_BYTE(Api020.Rx_Expected_Sequence[rx_channel], sequence);
    SR_SET_LONG(Api020.Rx_Time[rx_channel], now);
    SR_SET_BYTE(Api020.Rx_Sequence_Valid[rx_channel], sequence_preserved);
}


#if (API020_USE_RX_ARRAY == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 * Write data to the array.
 * @param rx_channel = Specifies which RX channel to read. The specified Rx channel must support message arrays.
 * @retval void * pointer to the memory address at which the data is stored
 */
static void WriteToArray(API020_RX_CHANNEL_INDEX_TYPE rx_channel, void * value)
{
	API020_RX_CONFIG_TYPE const* config = &API020_RX_CHANNEL[rx_channel];

	SR_CHECK_BYTE(Api020.Rx_Num_Of_Readings[rx_channel]);
	uint8 num_of_readings = Api020.Rx_Num_Of_Readings[rx_channel];

	/* calculate the address of the slot within the array */
	#define SLOT_ADDR(slot)\
		( slot * API020_RX_CHANNEL[rx_channel].Length )

	BOOL_TYPE write_to_array = TRUE;

	/*
	 * If the data is equal to the Error_Value, write the Error_Value to the array only if the most recently
	 * written to slot in the array does not contain the Error_Value.
	 */
	if (( memcmp(value, config->Error_Value, config->Length) == 0 ) &&		/* the data to write is the Error_Value AND */
		( num_of_readings >= 1 ))											/* the array is not empty */
	{
		uint8 slot = num_of_readings - 1;
		(void)SRData__CheckArray((void*)((uint8*)config->Address + SLOT_ADDR(slot)), (void*)((uint8*)config->N_Address + SLOT_ADDR(slot)), config->Length);
		if ( memcmp((void*)((uint8*)config->Address + SLOT_ADDR(slot)), config->Error_Value, config->Length) == 0 )
		{
			/* the most recently written to slot in the buffer already has the Error_Value written to it, do not write another Error_Value to the array */
			write_to_array = FALSE;
		}
	}

	if ( write_to_array == TRUE )
	{
		/* if the array is full, shift all the readings in the array back by 1 to make room for the new data value to write to the array */
		if ( num_of_readings >= config->Array_Size )
		{
			/* array is full */
			SR_SET_BYTE(Api020.Rx_Overflowed[rx_channel], 1);
			SRData__UpdateArray(config->Address, config->N_Address, (void*)((uint8*)config->Address + SLOT_ADDR(1)), (config->Array_Size - 1) * config->Length);
			num_of_readings = config->Array_Size - 1;
		}

		/* write the value to the array */
		SRData__UpdateArray((void*)((uint8*)config->Address + SLOT_ADDR(num_of_readings)), (void*)((uint8*)config->N_Address + SLOT_ADDR(num_of_readings)), value, config->Length);
		SR_SET_BYTE(Api020.Rx_Num_Of_Readings[rx_channel], num_of_readings + 1);
	}
}

#endif /* (API020_USE_RX_ARRAY == ENABLED) */

#endif      // #if (API020_NUMBER_OF_RX_CHANNELS > 0)

#endif      // #if (API020SAFETY_FEATURE == ENABLED)



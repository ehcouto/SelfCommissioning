/**
 *  @file
 *
 *  @brief      General defines needed by Reveal and Reveal APIs
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef REVEAL_DEF_H_
#define REVEAL_DEF_H_

#define API001CORE_NUM          1
#define API001CORE_TYPE         3
#define API001CORE_VERSION      2
#define API001CORE_INSTANCES    1

#include "Reveal_prm.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#ifndef REVEAL_RECIPE_SPECIFIC
    #define REVEAL_RECIPE_SPECIFIC
#endif

typedef enum
{
    // NVO - Recipe Status State Values
    NVO_RS_READY = 0,                            //The command is successfully executed and Reveal is ready to accept another command.
    NVO_RS_BUSY,                                 //The Reveal module is currently busy executing a command.
    NVO_RS_REJECTED,                             //The command send to Reveal was rejected, because another command still in progress.
    NVO_RS_ACK_EVENT,                            //Command was not executed because Reveal is currently waiting for ACK.
    NVO_RS_UNSUPPORTED,                          //The command was unsupported for some reason.
    NVO_RS_UNSUP_OP_CODE,                        //Command did not execute due to invalid op code
    NVO_RS_UNSUP_UNAVAILABLE,                    //Command did not execute because it is currently unavailable in this state.
    NVO_RS_UNSUP_INVALID_PARAM,                  //Command did not execute due to an invalid or out of bounds parameter.
    NVO_RS_UNSUP_OUT_OF_MEMORY,                  //Command did not execute because the dynamic heap is out of memory.
    NVO_RS_UNSUP_DOOR_OPEN,                      //Command did not execute because the appliance door was open.
    NVO_RS_UNSUP_BOUND_CMD_INCOMPLETE,           //Bounded CMD was not fully received before a specified timeout, so it was not executed.
    NVO_RS_UNSUP_CANNOT_PAUSE_NOW,               //Unable to pause due to state of appliance process.
    NVO_RS_TBD,
    NVO_RS_DONT_SEND_ACK,
    NVO_RS_TIMEOUT
    REVEAL_RECIPE_SPECIFIC                       //Application specific (from 200 to 255) developers may use these return values in their applications..
} REVEAL_RECIPE_STATUS_TYPE;

typedef struct
{
    unsigned char id;
    unsigned short int type;
    unsigned short int version;
    unsigned char instances;
} REVEAL_API_INFO_TYPE;


typedef struct
{
    unsigned char api_id;
    unsigned char instance_id;
    unsigned short int type;
    unsigned short int version;
} REVEAL_INSTACNE_INFO_TYPE;               //Apologize to spell mistake for "Instances" for compatibility we want to keep as it is
                                           //because Reveal 4 module is used in some projects

#ifndef REVEAL_PAYLOAD_SIZE
    #define REVEAL_PAYLOAD_SIZE 13
#endif

#ifndef REVEAL_LLC_CRC_SIZE
    #define REVEAL_LLC_CRC_SIZE 2
#endif


#if (REVEAL_PAYLOAD_SIZE < 13)
    #error: "REVEAL_PAYLOAD_SIZE must be defined as 155 or greater than 13."
#endif

typedef struct
{
    unsigned char destination       :4;                 //Destination node
    unsigned char node_id           :4;                 //Source node
    #if (REVEAL_PAYLOAD_SIZE == 13)                     //Wide or similar size
        unsigned char sdu_length    :4;
    #else
        unsigned char sdu_length    ;
        unsigned char not_used      :4;
    #endif

    unsigned char sap               :4;                 //SAP
    unsigned char api_id              ;                 //API number
    unsigned char op_code           :5;                 //Opcode
    unsigned char cmd_fb            :1;                 //Command/Feedback
    unsigned char frag              :1;                 //Fragmentation
    unsigned char mmp               :1;                 //More Message Pending
    unsigned char payload[REVEAL_PAYLOAD_INCOMING_SIZE + REVEAL_LLC_CRC_SIZE];
    unsigned char size;                                 //Payload_size
}REVEAL_MSG_TYPE;

typedef REVEAL_RECIPE_STATUS_TYPE (* const REVEAL_PARSER_COMMAND_FUNCTION_TYPE)(REVEAL_MSG_TYPE *);
typedef void (* const REVEAL_PARSER_FEEDBACK_FUNCTION_TYPE)(REVEAL_MSG_TYPE *);
typedef void (* const REVEAL_PARSER_ACK_TYPE)(unsigned char address, unsigned char api, unsigned char opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode);

typedef struct
{
    unsigned char ADDRESS;
    unsigned char API;
    REVEAL_PARSER_COMMAND_FUNCTION_TYPE COMMAND_PARSER;
    REVEAL_PARSER_FEEDBACK_FUNCTION_TYPE FEEDBACK_PARSER;
    REVEAL_PARSER_ACK_TYPE ACK_PARSER;
}REVEAL_PARSER_TYPE;

#define NULL_CMD_PARSER         (REVEAL_PARSER_COMMAND_FUNCTION_TYPE)NULL
#define NULL_FDB_PARSER         (REVEAL_PARSER_FEEDBACK_FUNCTION_TYPE)NULL
#define NULL_ACK_PARSER         (REVEAL_PARSER_ACK_TYPE)NULL

#define REVEAL_ROUTER_DISABLED                  0
#define REVEAL_ROUTER_INTERNAL_EXTERNAL         1
#define REVEAL_ROUTER_INTERNAL_ONLY             2


#define REVEAL_ROUTING_EXTERNAL_DEF     0
#define REVEAL_ROUTING_INTERNAL_DEF     1


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // REVEAL_DEF_H_



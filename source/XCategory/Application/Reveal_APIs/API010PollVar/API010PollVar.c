/**
 * @file
 * @brief       Communications to get periodic publication of system and memory-mapped variables
 *
 * @details     API 010 Poll Variables is used to publish standard and custom variables for the
 *              purposes of data acquisition.
 *
 *              Fixed Variables:
 *              Fixed variables are standard variables that are defined for a specific Product
 *              Type. Some boards (such as expansion boards) may not support any standard
 *              variables. Use OpCode 21 to determine what is supported. These standard variables
 *              make it easier to quickly get up and running with a system that can monitor the
 *              basic variables of a product.
 *
 *              Memory-Mapped Variables:
 *              Memory-mapped variables allow a user to customize the data being monitored. Since
 *              memory-mapped variables are a compile-time option in the firmware, some boards
 *              may not support memory-mapped variables. Use OpCode 21 to determine what is
 *              supported. To add a memory-mapped variable to the list of monitored variables,
 *              the user must know the address and size of the variable.
 *
    @startuml{API010Publisher_sm.png}
        title API 10 Publisher State Machine
        ' Use State = Api010.State
        ' Suppress transition checks

        [*] --> Inactive

        Inactive : entry / ResetPublisher()
        Inactive : do / CheckMetadataDue()
        Inactive --> Active : evStartPublishing

        Active : do / UpdateTimer()\n\t CheckDataDue()
        Active -u-> Inactive : evStopPublishing
        Active -r-> Publishing : evProceed

        Publishing : entry / PublishNextPacket()
        Publishing : do / UpdateTimer()\n\t PublishNextPacket()
        Publishing --> Active : evProceed
        Publishing --> Inactive : evStopPublishing
    @enduml
 *
 * @copyright   Copyright 2014-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

#ifndef API010POLLVAR_FEATURE
    // API010POLLVAR_FEATURE should be ENABLED for applications that use API010 for development data acquisition.
    #error API010POLLVAR_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif

#if (API010POLLVAR_FEATURE == ENABLED)

#include "API010PollVar.h"
#include "API010PollVar_prv.h"

#ifndef API010_USES_FIXED_VARIABLES_FEATURE
    // API010_USES_FIXED_VARIABLES_FEATURE should be ENABLED for applications that use the fixed system variables.
    #error API010_USES_FIXED_VARIABLES_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif
#ifndef API010_CAN_USE_SPI_MODULE
    // Assume that API010 uses Reveal only.
    #define API010_CAN_USE_SPI_MODULE DISABLED
#endif
#ifndef API010_CAN_USE_SPIMGR_MODULE
    // Assume that API010 uses Reveal only.
    #define API010_CAN_USE_SPIMGR_MODULE DISABLED
#endif
#if ((API010_CAN_USE_SPI_MODULE == ENABLED) && (API010_CAN_USE_SPIMGR_MODULE == ENABLED))
    #error The features API010_CAN_USE_SPI_MODULE and API010_CAN_USE_SPIMGR_MODULE cannot be enabled at the same time
#endif
#ifndef API010_DEBUG_ACQ_CONFIG
    // This is only required while debugging API010. Disable by default.
    #define API010_DEBUG_ACQ_CONFIG DISABLED
#endif

// -- Other Modules --
#include "Log.h"
#include "Micro.h"
#include "Millis.h"
#if (API010_CAN_USE_SPI_MODULE == ENABLED)
    #include "Spi.h"
#endif
#if (API010_CAN_USE_SPIMGR_MODULE == ENABLED)
    #include "SpiMgr.h"
#endif
#include <string.h>
#include "Utilities.h"
#if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
    #include "Variable.h"
    #define API010_FIXED_VARS_COUNT VARIABLE_NUM_VARIABLES
#else
    #define API010_FIXED_VARS_COUNT 0
#endif


// The number of memory mapped variables supported must be at least 1.
CT_ASSERT(API010_MMAP_VARS_COUNT >= 0);

// There is no point in enabling this module if there are no variables to publish.
// Disable the module in SystemConfig_prm.h by setting API010POLLVAR_FEATURE to DISABLED.
CT_ASSERT((API010_USES_FIXED_VARIABLES_FEATURE == ENABLED) || (API010_MMAP_VARS_COUNT > 0));


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for API 10 Publisher State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the API 10 Publisher state machine.
typedef enum API_10_PUBLISHER_STATE_ENUM
{
    STATE_API_10_PUBLISHER_NONE       = 0,

    STATE_API_10_PUBLISHER_INACTIVE   = 1,          //!< (initial_state)
    STATE_API_10_PUBLISHER_ACTIVE     = 2,
    STATE_API_10_PUBLISHER_PUBLISHING = 3,

    STATE_API_10_PUBLISHER_END        = 4
} API_10_PUBLISHER_STATE_TYPE;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckDataDue(void);
static void CheckMetadataDue(void);
static void PublishNextPacket(void);
static void ResetPublisher(void);
static void UpdateTimer(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void API10PublisherExecute(void);
static void API10PublisherInitialize(void);
static void EventProceed(void);
static void EventStartPublishing(void);
static void EventStopPublishing(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! The total number of variables that can be managed by the API010 module.
#define API010_VARS_COUNT                       (API010_FIXED_VARS_COUNT + API010_MMAP_VARS_COUNT)

#ifndef API010_STREAM_BUFFER_SIZE
    //! The size of the incoming and outgoing data streams for transferring the periodic data.
    // The '6' in the equation is a guess of the average memory used by each variable in the data stream.
    #define API010_STREAM_BUFFER_SIZE           (API010_VARS_COUNT * 6)
#endif

#ifndef API010_ON_CHANGE_BUFFER_SIZE
    //! The size of the buffer that can be used for detecting variables "on change".
    #define API010_ON_CHANGE_BUFFER_SIZE        0
#endif

//! The minimum publication period allowed by API010.
#define API010_MINIMUM_PUBLICATION_PERIOD       10

//! The maximum number of metadata that can fit in a Reveal packet.
#define API010_MAXIMUM_METADATA_PER_PACKET      (REVEAL_PAYLOAD_SIZE / sizeof(API010_VARIABLE_METADATA_TYPE))

//! The name of this module for use by the Log module.
#define MODULE_NAME                             MODULE_API010_POLL_VAR


// -- Private Enumerated Constant Declarations --

//! Methods of data extraction supported by API010.
typedef enum API010_ACQUISITION_METHOD_ENUM
{
    API010_METHOD_REVEAL = 0,
    API010_METHOD_SPI,
    API010_METHOD_DIRECT_SERIAL,
    API010_METHOD_SIZE
} API010_METHOD_TYPE;

//! All the API010 command and feedback OpCodes.
typedef enum API010_OPCODE_ENUM
{
    API010_CMD_REQUEST_SPECIFIC_VARIABLE            = 1,    //!< Request data for one specific variable.
    API010_FBK_PUBLISH_SPECIFIC_VARIABLE            = 2,    //!< Publish data for one specific variable.
    API010_CMD_ENABLE_PERIODIC_DATA                 = 3,    //!< Enable or disable the periodic publication of selected variables.
    API010_FBK_PUBLISH_DATA_STREAM                  = 4,    //!< Publish the data with the selected variables.
    API010_CMD_DESELECT_ALL_VARIABLES               = 5,    //!< Remove all variables from the list of published variables.
    API010_CMD_SELECT_ALL_VARIABLES                 = 6,    //!< Add all variables to the list of published variables.
    API010_CMD_DESELECT_SPECIFIC_VARIABLES          = 7,    //!< Remove specific variables from the list of published variables.
    API010_CMD_SELECT_SPECIFIC_VARIABLES            = 8,    //!< Add specific variables to the list of published variables.
    API010_CMD_SETUP_PERIODIC_PUBLICATION           = 9,    //!< Setup the publication period and the method of sending variables.
    API010_CMD_DEFINE_MEMORY_MAPPED_VARIABLE        = 10,   //!< Add a single memory-mapped variable.
    API010_CMD_UNDEFINE_MEMORY_MAPPED_VARIABLE      = 11,   //!< Remove memory-mapped variable(s).
    API010_CMD_CONFIGURE_ON_CHANGE_BEHAVIOR         = 12,   //!< Specify that a variable be published (or not) on change.
    API010_CMD_REQUEST_FIXED_VARIABLE_METADATA      = 13,   //!< Request a fixed variable's metadata.
    API010_FBK_PUBLISH_FIXED_VARIABLE_METADATA      = 14,   //!< Publish a fixed variable's metadata.
    API010_CMD_REQUEST_ACQUISITION_CONFIGURATION    = 18,   //!< Request the API010 acquisition state.
    API010_FBK_PUBLISH_ACQUISITION_CONFIGURATION    = 19,   //!< Publish the API010 acquisition state.
    API010_CMD_REQUEST_NUM_OF_VARIABLES             = 20,   //!< Request the number of fixed and memory-mapped variables.
    API010_FBK_PUBLISH_NUM_OF_VARIABLES             = 21,   //!< Publish the number of fixed and memory-mapped variables.
    API010_CMD_WRITE_FIXED_VARIABLE_VALUE           = 22,   //!< Set the value of a writable variable.
} API010_OPCODE_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_API010_POLL_VAR_LOG_MESSAGE_ID_ENUM
{
    TOO_MUCH_DATA = 1,
} MODULE_API010_POLL_VAR_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

//! The control structure that tracks the configuration of a single data variable.
typedef struct API010_DATA_CONTROL_STRUCT
{
    BOOL_TYPE Selected  : 1;                        //!< Selected for periodic publication.
#if (API010_ON_CHANGE_BUFFER_SIZE > 0)
    BOOL_TYPE On_Change : 1;                        //!< Also selected for publication on change.
    BOOL_TYPE Changed   : 1;                        //!< A change was detected in this variable.
#endif
} API010_DATA_CONTROL_TYPE;

//! The first part of the payload when publishing API010 periodic data.
typedef uint16 API010_STREAM_INDEX_TYPE;

//! A structure for all the variables used by API010.
typedef struct API010_RAM_DATA_STRUCT
{
    //! The period of time between publication of API010 data.
    uint16 Publication_Period;

    //! The number of bytes in the Stream_Buffer[] array that must be published.
    API010_STREAM_INDEX_TYPE Stream_Length;

    //! The number of bytes in the Stream_Buffer[] array that have already been published.
    API010_STREAM_INDEX_TYPE Stream_Index;

    #if (API010_MMAP_VARS_COUNT > 0)
        //! An array of configuration structures for potential memory-mapped variables.
        API010_VARIABLE_TYPE Mem_Mapped_Vars[API010_MMAP_VARS_COUNT];
    #endif

    //! The configuration and control of all of the data variables.
    API010_DATA_CONTROL_TYPE Control[API010_VARS_COUNT];

    //! A value that saves the time of the last periodic publication.
    uint8 Publication_Counter;

    #if (API010_ON_CHANGE_BUFFER_SIZE > 0)
        //! Offsets into the Last_Sent[] array for each variable.
        uint16 Last_Sent_Offset[API010_VARS_COUNT + 1];

        //! The RAM used to detect changed values in "on change" variables.
        uint8 Last_Sent[API010_ON_CHANGE_BUFFER_SIZE];
    #endif

    //! Buffer used to build a packet for publishing data on Reveal or SPI.
    uint8 Stream_Buffer[API010_STREAM_BUFFER_SIZE];

    #if (API010_CAN_USE_SPIMGR_MODULE == ENABLED)
        //! Buffer required for interactions with SpiMgr module. (Not used by this module.)
        uint8 Rx_Buffer[API010_STREAM_BUFFER_SIZE];
    #endif

    //! The selected extraction method actively being used by API010.
    API010_METHOD_TYPE Method;

    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        //! The number of variables for which the variable metadata must still be sent.
        uint8 Remaining_Metadata;
    #endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)

    //! Node to which packets should be sent.
    uint8 Reply_To;

    //! The checksum for the Stream_Buffer[] array.
    uint8 Stream_Csum;

    //! The sequence number for the Stream_Buffer[] array published with API010_FBK_PUBLISH_DATA_STREAM.
    uint8 Sequence_Number;

    //! Flag that indicates the state of the API010 module.
    API_10_PUBLISHER_STATE_TYPE State;

    //! Flag that indicates whether the API010 client has requested periodic data.
    BOOL_TYPE Periodic_Data_Requested;

    //! A flag that indicates that API010 data must be sent.
    BOOL_TYPE Send_Data;

    //! A flag that indicates that it is time to publish the API010 periodic data.
    BOOL_TYPE Is_Periodic_Data_Due;

    //! Flag that indicates whether this is the first publication of data since it was enabled.
    //! Used to publish errors only once when they are first detected.
    BOOL_TYPE Enable_Error_Reporting;
} API010_RAM_DATA_TYPE;

#if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
    //! Structure for holding a variable's metadata information.
    typedef PACKED struct API010_VARIABLE_METADATA_STRUCT
    {
        uint8 Variable_Index;
        uint8 Variable_Array_Elements;
        DATATYPE_TYPE Variable_Element_Type;
        uint32 Key;
    } API010_VARIABLE_METADATA_TYPE;
#endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --


// -- Private Variable Definitions --

//! A structure that holds all the variables used by this module.
//! This allows for more efficient initialization and does not add any run-time overhead.
static API010_RAM_DATA_TYPE Api010;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

uint8 GetMMapVarsCount(void);
static BOOL_TYPE BuildStream(BOOL_TYPE all);
static BOOL_TYPE GetVariableInfo(uint8 variable_index, API010_VARIABLE_TYPE* var_info);
static BOOL_TYPE IsOnChangeDataDue(void);
#if (API010_ON_CHANGE_BUFFER_SIZE > 0)
    static void AddToOffsets(uint8 first_index, sint16 delta);
    static BOOL_TYPE OnChangeInsertVariable(uint8 variable_index);
    static void OnChangeRemoveVariable(uint8 variable_index);
    static BOOL_TYPE SetOnChangeBehavior(BOOL_TYPE on_change, uint8 first_index, uint8 last_index);
#endif      // #if (API010_ON_CHANGE_BUFFER_SIZE > 0)
static void UpdateTimer(void);
#if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
    static void PublishNextMetadata(void);
    static PASS_FAIL_TYPE RevealAppendMetadata(REVEAL_COMMAND_TYPE *msg, uint8 variable_index);
#endif
#if (API010_MMAP_VARS_COUNT > 0)
    static void RemoveMemoryMappedVariable(uint8 dyn_index);
#endif


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the API010PollVar module variables.
 */
void API010PollVar__Initialize(void)
{
    // Initialize the Spi module if it is only used by API010.
    #if (API010_CAN_USE_SPI_MODULE == ENABLED)
        Spi__Initialize(API010POLLVAR_SPI_CHANNEL);
    #endif

    if (Micro__GetResetMode() != MICRO_RESET_MODE_WARM)
    {
        // Initialize all of the module's RAM to zeros.
        memset(&Api010, 0, sizeof(Api010));

        // Set other default values.
        Api010.Publication_Period = 1000;

        // Initialize the state machine.
        API10PublisherInitialize();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Handle the messages that must be sent on Reveal.
 * Handles the periodic publication of data when the API010_METHOD_REVEAL method is selected.
 * Handles the (lower priority) publication of metadata when requested.
 */
void API010PollVar__SlowHandler(void)
{
    if (Api010.Method == API010_METHOD_REVEAL)
    {
        API10PublisherExecute();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Handle the messages that must be sent on SPI.
 * Typically managed on a 250us interrupt time base.
 */
void API010PollVar__FastHandler(void)
{
#if (API010_CAN_USE_SPI_MODULE == ENABLED)
    if (Api010.Method == API010_METHOD_SPI)
    {
        API10PublisherExecute();
    }
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This method parses API010 commands sent to a broadcast or exclusively to this node.
 * @param cmd = Pointer to the received command message.
 * @return REVEAL_RECIPE_STATUS_TYPE that expresses the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API010PollVar__CommandParser(REVEAL_MSG_TYPE* cmd)
{
    // For convenience extract parts of the command into automatic variables.
    uint8 size = cmd->size;
    uint8* payload = cmd->payload;

    // Assume that the message is not supported.
    REVEAL_RECIPE_STATUS_TYPE reveal_response = NVO_RS_UNSUP_INVALID_PARAM;
    switch ((API010_OPCODE_TYPE)cmd->op_code)
    {
        case API010_CMD_ENABLE_PERIODIC_DATA:
            // Expected Payload:
            //      One byte: 0 = Stop publishing periodic data, 1 = Start publishing periodic data.
            if (size >= 1)
            {
                BOOL_TYPE data_requested = ((payload[0] == 0) ? FALSE : TRUE);
                if (data_requested == TRUE)
                {
                    EventStartPublishing();
                }
                else
                {
                    EventStopPublishing();
                }
                reveal_response = NVO_RS_READY;
            }
            break;

        case API010_CMD_SETUP_PERIODIC_PUBLICATION:
            // Expected Payload:
            //      One byte for the extraction method.
            //      Two big-endian bytes for the publication period in milliseconds.
            if (size >= 3)
            {
                API010_METHOD_TYPE method = (API010_METHOD_TYPE)payload[0];
                uint16 milliseconds = Utilities__ConvertArrayTo16bits(&payload[1]);
                uint16 max_ms = (method == API010_METHOD_REVEAL) ? API010_MINIMUM_PUBLICATION_PERIOD : 1;
                if (milliseconds >= max_ms)
                {
                    Api010.Method = method;
                    Api010.Publication_Period = milliseconds;
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case API010_CMD_SELECT_ALL_VARIABLES:
            // Expected No Payload
            #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                for (uint8 index = 0; index < API010_FIXED_VARS_COUNT; index++)
                {
                    Api010.Control[index].Selected = TRUE;
                }
            #endif      // #if ((API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
            #if (API010_MMAP_VARS_COUNT > 0)
                for (uint8 index = 0; index < API010_MMAP_VARS_COUNT; index++)
                {
                    if (Api010.Mem_Mapped_Vars[index].Address != NULL)
                    {
                        Api010.Control[API010_FIXED_VARS_COUNT + index].Selected = TRUE;
                    }
                }
            #endif      // #if (API010_MMAP_VARS_COUNT > 0)
            reveal_response = NVO_RS_READY;
            break;

        case API010_CMD_DESELECT_ALL_VARIABLES:
            // Expected No Payload
            for (uint8 index = 0; index < API010_VARS_COUNT; index++)
            {
                Api010.Control[index].Selected = FALSE;
            }
            reveal_response = NVO_RS_READY;
            break;

        case API010_CMD_SELECT_SPECIFIC_VARIABLES:
            // Expected Payload:
            //      One byte for each selected variable index.
            reveal_response = NVO_RS_READY;
            for (uint8 index = 0; index < size; index++)
            {
                uint8 variable_index = payload[index];
                if (
                  #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                    (variable_index < API010_FIXED_VARS_COUNT)
                    #if (API010_MMAP_VARS_COUNT > 0)
                     ||
                    #endif
                  #endif
                  #if (API010_MMAP_VARS_COUNT > 0)
                    ((variable_index < API010_VARS_COUNT) &&
                     (Api010.Mem_Mapped_Vars[variable_index - API010_FIXED_VARS_COUNT].Address != NULL))
                  #endif
                   )
                {
                    Api010.Control[variable_index].Selected = TRUE;
                }
                else
                {
                    reveal_response = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            break;

        case API010_CMD_DESELECT_SPECIFIC_VARIABLES:
            // Expected Payload:
            //      One byte for each deselected variable index.
            reveal_response = NVO_RS_READY;
            for (uint8 index = 0; index < size; index++)
            {
                uint8 variable_index = payload[index];
                if (variable_index < API010_VARS_COUNT)
                {
                    Api010.Control[variable_index].Selected = FALSE;
                }
                else
                {
                    reveal_response = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            break;

    #if (API010_ON_CHANGE_BUFFER_SIZE > 0)
        case API010_CMD_CONFIGURE_ON_CHANGE_BEHAVIOR:
            // Expected Payload:
            //      One byte for the variable index.
            //      One byte for the "on-change" flag. 0 = do not publish on change, 1 = publish on change.
            if (size >= 2)
            {
                uint8 variable_index = payload[0];
                BOOL_TYPE on_change = ((payload[1] == 0) ? FALSE : TRUE);
                if (variable_index < API010_VARS_COUNT)
                {
                    // Set the "on change" behavior for the specified variable.
                    if (SetOnChangeBehavior(on_change, variable_index, variable_index) == TRUE)
                    {
                        reveal_response = NVO_RS_READY;
                    }
                }
                else if (variable_index == 255)
                {
                    // Set the "on change" behavior for all variables.
                    if (SetOnChangeBehavior(on_change, 0, API010_VARS_COUNT - 1) == TRUE)
                    {
                        reveal_response = NVO_RS_READY;
                    }
                }
            }
            break;
    #endif      // #if (API010_ON_CHANGE_BUFFER_SIZE > 0)

    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        case API010_CMD_REQUEST_FIXED_VARIABLE_METADATA:
            // Expected Payload:
            //      One byte for the variable index.
            if (size >= 1)
            {
                uint8 variable_index = payload[0];
                if (variable_index < API010_FIXED_VARS_COUNT)
                {
                    // Publish metadata for specified variable.
                    REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_FIXED_VARIABLE_METADATA, cmd->node_id, sizeof(API010_VARIABLE_METADATA_TYPE));
                    if ((msg != NULL) &&
                        (RevealAppendMetadata(msg, variable_index) == PASS) &&
                        (Reveal__QueueCommand(msg) == TRUE))
                    {
                        reveal_response = NVO_RS_READY;
                    }
                    else
                    {
                        // Reveal rejected the request to publish a message.
                        reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
                    }
                }
                else if (variable_index == 255)
                {
                    // Publish metadata for all fixed variables.
                    Api010.Reply_To = cmd->node_id;
                    Api010.Remaining_Metadata = API010_FIXED_VARS_COUNT;
                    reveal_response = NVO_RS_READY;
                }
            }
            break;
    #endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)

    #if (API010_MMAP_VARS_COUNT > 0)
        case API010_CMD_DEFINE_MEMORY_MAPPED_VARIABLE:
            // Expected Payload:
            //      One byte for the variable index.
            //      One byte for the size of the variable's data.
            //      Four big-endian bytes for the variable's pointer.
            if (size >= 6)
            {
                uint8 variable_index = payload[0];
                if (
                    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                     (API010_FIXED_VARS_COUNT <= variable_index) &&
                    #endif
                     (variable_index < API010_VARS_COUNT))
                {
                    // adjust index
                    uint8 mmap_index = variable_index - API010_FIXED_VARS_COUNT;
                    Api010.Mem_Mapped_Vars[mmap_index].Size = payload[1];
                    Api010.Mem_Mapped_Vars[mmap_index].Address = (void*)Utilities__ConvertArrayTo32bits(&payload[2]); //lint !e923 Suppress Info: cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
                    reveal_response = NVO_RS_READY;
                }
            }
            break;

        case API010_CMD_UNDEFINE_MEMORY_MAPPED_VARIABLE:
            // Expected Payload:
            //      One byte for each undefined variable index.
            if ((size == 1) && (payload[0] == 255))
            {
                // Undefine all memory-mapped variables.
                for (uint8 mmap_index = 0; mmap_index < API010_MMAP_VARS_COUNT; mmap_index++)
                {
                    RemoveMemoryMappedVariable(mmap_index);
                }
                reveal_response = NVO_RS_READY;
            }
            else
            {
                for (uint8 index = 0; index < size; index++)
                {
                    uint8 variable_index = payload[index];
                    if (
                        #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
                         (API010_FIXED_VARS_COUNT <= variable_index) &&
                        #endif
                         (variable_index < API010_VARS_COUNT))
                    {
                        // Undefine the specified memory-mapped variable.
                        uint8 mmap_index = variable_index - API010_FIXED_VARS_COUNT;
                        RemoveMemoryMappedVariable(mmap_index);
                        reveal_response = NVO_RS_READY;
                    }
                }
            }
            break;
    #endif      // #if (API010_MMAP_VARS_COUNT > 0)

        case API010_CMD_REQUEST_SPECIFIC_VARIABLE:
            // Expected Payload:
            //      One byte for the variable index.
            if (size >= 1)
            {
                // This opcode sends the data of one specific variable, this opcode doesn't take care of endianess, it will be send out in the endianess of the micro
                // The same process used by the stream transmission.
                uint8 variable_index = payload[0];
                API010_VARIABLE_TYPE variable;
                if (GetVariableInfo(variable_index, &variable) == TRUE)
                {
                    REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_SPECIFIC_VARIABLE, cmd->node_id, variable.Size + 1);
                    if ((Reveal__AddDataU8(msg, variable_index) == PASS) &&
                        (Reveal__AddDataU8Buffer(msg, variable.Address, variable.Size) == PASS) &&
                        (Reveal__QueueCommand(msg) == TRUE))
                    {
                        reveal_response = NVO_RS_READY;
                    }
                    else
                    {
                        // Reveal rejected the request to publish a message.
                        reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
                    }
                }
            }
            break;

    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        case API010_CMD_WRITE_FIXED_VARIABLE_VALUE:
            // Expected Payload
            //      One byte for the variable index.
            //      One byte for the array position.
            //      One or more bytes for the variable value in native endian format.
            if (size >= 3)
            {
                VARIABLE_ID_TYPE variable_index = (VARIABLE_ID_TYPE)payload[0];
                if (variable_index < API010_FIXED_VARS_COUNT)
                {
                    VARIABLE_TYPE variable;
                    Variable__GetByIndex(variable_index, &variable);
                    if (variable.Is_Writable == TRUE)
                    {
                        uint8 array_position = payload[1];
                        uint8 data_size = size - 2;
                        uint8 variable_size = VARIABLE_SIZE[variable.Type];
                        uint8 element_count = data_size / variable_size;
                        if (((data_size % variable_size) == 0) &&
                            (array_position + element_count <= variable.Length))
                        {
                            uint8* data_source = &payload[2];
                            uint8* data_destination = variable.Address;
                            data_destination += array_position * variable_size;
                            memcpy(data_destination, data_source, data_size);
                            reveal_response = NVO_RS_READY;
                        }
                    }
                }
            }
            break;
    #endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)

    #if (API010_DEBUG_ACQ_CONFIG == ENABLED)
        case API010_CMD_REQUEST_ACQUISITION_CONFIGURATION:
        {
            // Expected No Payload
            uint8 selected[BIT_STORAGE_SIZE(API010_VARS_COUNT)];
            uint8 on_change[BIT_STORAGE_SIZE(API010_VARS_COUNT)];
            for (uint8 index = 0; index < API010_VARS_COUNT; index++)
            {
                Utilities__SetBitInArray(selected, index, Api010.Control[index].Selected);
                Utilities__SetBitInArray(on_change, index, Api010.Control[index].On_Change);
            }
            REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_ACQUISITION_CONFIGURATION, cmd->node_id, 2 * BIT_STORAGE_SIZE(API010_VARS_COUNT) + 4);
            if ((Reveal__AddDataU8(msg, Api010.Method) == PASS) &&
                (Reveal__AddDataU16(msg, Api010.Publication_Period) == PASS) &&
                (Reveal__AddDataU8(msg, BIT_STORAGE_SIZE(API010_VARS_COUNT)) == PASS) &&
                (Reveal__AddDataU8Buffer(msg, selected, BIT_STORAGE_SIZE(API010_VARS_COUNT)) == PASS) &&
                (Reveal__AddDataU8Buffer(msg, on_change, BIT_STORAGE_SIZE(API010_VARS_COUNT)) == PASS) &&
                (Reveal__QueueCommand(msg) == TRUE))
            {
                reveal_response = NVO_RS_READY;
            }
            else
            {
                reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
            }
            break;
        }
    #endif      // #if (API010_DEBUG_ACQ_CONFIG == ENABLED)

        case API010_CMD_REQUEST_NUM_OF_VARIABLES:
        {
            // Expected No Payload
            REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_NUM_OF_VARIABLES, cmd->node_id, 2);
            if ((Reveal__AddDataU8(msg, API010_FIXED_VARS_COUNT) == PASS) &&
                (Reveal__AddDataU8(msg, API010_MMAP_VARS_COUNT) == PASS) &&
                (Reveal__AddDataU8(msg, GetMMapVarsCount()) == PASS) &&
                (Reveal__QueueCommand(msg) == TRUE))
            {
                reveal_response = NVO_RS_READY;
            }
            else
            {
                // Reveal rejected the request to publish a message.
                reveal_response = NVO_RS_UNSUP_OUT_OF_MEMORY;
            }
            break;
        }

        default:
            reveal_response = NVO_RS_UNSUP_OP_CODE;
            break;
    }

    return (reveal_response);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the current list of memory-mapped variables.
 * These are the variables configured by an external tool.
 * @param list = Pointer to a buffer to fill with the variable information.
 * @param max_vars = The maximum number of variables that the list can hold.
 * @param publication_period = Pointer to a buffer to fill with the publication period.
 * @return The number of memory-mapped variables that were copied.
 */
uint8 API010PollVar__GetMemMapVarsList(API010_VARIABLE_TYPE* list, uint8 max_vars, uint16* publication_period)
{
    uint8 vars_copied = 0;
    #if API010_MMAP_VARS_COUNT > 0
        for (uint8 i = 0; (i < API010_MMAP_VARS_COUNT) && (vars_copied < max_vars); i++)
        {
            if (Api010.Mem_Mapped_Vars[i].Address != NULL)
            {
                list[vars_copied].Address = Api010.Mem_Mapped_Vars[i].Address;
                list[vars_copied].Size = Api010.Mem_Mapped_Vars[i].Size;
                vars_copied++;
            }
        }
    #endif
    *publication_period = Api010.Publication_Period;

    return vars_copied;
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the number of memory mapped variables that have been defined.
 * @return The number of memory mapped variables that have been defined.
 */
uint8 GetMMapVarsCount(void)
{
    uint8 count = 0;
    #if (API010_MMAP_VARS_COUNT > 0)
        for (uint8 i = 0; i < API010_MMAP_VARS_COUNT; i++)
        {
            if (Api010.Mem_Mapped_Vars[i].Address != NULL)
            {
                count++;
            }
        }
    #endif      // #if (API010_MMAP_VARS_COUNT > 0)
    return count;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Build the stream of data for the specified variables.
 * The stream of data consists of the following:
 *      One byte for the stream sequence number. Incremented for each new stream of data.
 *      For each published variable:
 *          One byte for the variable index.
 *          One or more bytes for the variable value.
 * @param selected = A flag that specifies which set of variables to put in the data stream.
 *              FALSE = Only put the variables that have changed in the data stream.
 *              TRUE = Put all the variables that were selected as part of the periodic data.
 * @return TRUE if the data stream was built and there is data in it.
 */
static BOOL_TYPE BuildStream(BOOL_TYPE selected)
{
    Api010.Stream_Buffer[0] = Api010.Sequence_Number;
    Api010.Sequence_Number++;
    Api010.Stream_Length = 1;
    for (uint8 index = 0; index < API010_VARS_COUNT; index++)
    {
#if (API010_ON_CHANGE_BUFFER_SIZE > 0)
        if ((Api010.Control[index].Selected == TRUE) &&
            ((selected == TRUE) ||
             ((Api010.Control[index].On_Change == TRUE) &&
              (Api010.Control[index].Changed == TRUE))))
#else
        if ((Api010.Control[index].Selected == TRUE) &&
            (selected == TRUE))
#endif
        {
            API010_VARIABLE_TYPE variable;
            if ((GetVariableInfo(index, &variable) == TRUE) &&
                (Api010.Stream_Length + variable.Size + 1 < API010_STREAM_BUFFER_SIZE))
            {
                Api010.Stream_Buffer[Api010.Stream_Length] = index;
                Api010.Stream_Length++;
                memcpy(&Api010.Stream_Buffer[Api010.Stream_Length], variable.Address, variable.Size);
                #if (API010_ON_CHANGE_BUFFER_SIZE > 0)
                    if (Api010.Control[index].On_Change == TRUE)
                    {
                        uint16 offset = Api010.Last_Sent_Offset[index];
                        memcpy(&Api010.Last_Sent[offset], variable.Address, variable.Size);
                    }
                    Api010.Control[index].Changed = FALSE;
                #endif
                Api010.Stream_Length += variable.Size;
            }
            else
            {
                if (Api010.Enable_Error_Reporting == TRUE)
                {
                    Api010.Enable_Error_Reporting = FALSE;
                    LOG_ADD_EXCEPTION(TOO_MUCH_DATA, index);
                }
                break;                              // Out of the for() loop.
            }
        }
    }
    if (Api010.Stream_Length > 1)
    {
        // Setup the supporting stream variables.
        Api010.Stream_Csum = 0;
        for (API010_STREAM_INDEX_TYPE i = 0; i < Api010.Stream_Length; i++)
        {
            Api010.Stream_Csum += Api010.Stream_Buffer[i];
        }
        Api010.Stream_Index = 0;
    }
    return (BOOL_TYPE)(Api010.Stream_Length > 1);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get generic information about a variable whether it is a fixed variable or a memory-mapped variable.
 * @param variable_index = The variable to get.
 * @param var_info = Pointer to a structure to fill with the variable information.
 * @return TRUE if the var_info structure was filled. Otherwise FALSE.
 */
static BOOL_TYPE GetVariableInfo(uint8 variable_index, API010_VARIABLE_TYPE* var_info)
{
    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        if (variable_index <= API010_FIXED_VARS_COUNT)
        {
            VARIABLE_TYPE variable;
            if (Variable__GetByIndex((VARIABLE_ID_TYPE)variable_index, &variable) == TRUE)
            {
                var_info->Address = variable.Address;
                var_info->Size = VARIABLE_SIZE[variable.Type] * variable.Length;
                return TRUE;
            }
        }
        #if (API010_MMAP_VARS_COUNT > 0)
        else
        #endif      // #if (API010_MMAP_VARS_COUNT > 0)
    #endif          // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
    #if (API010_MMAP_VARS_COUNT > 0)
        if (variable_index < API010_VARS_COUNT)
        {
            uint8 mmap_index = variable_index - API010_FIXED_VARS_COUNT;
            API010_VARIABLE_TYPE* mmap_var = &Api010.Mem_Mapped_Vars[mmap_index];
            if (mmap_var->Address != NULL)
            {
                memcpy(var_info, mmap_var, sizeof(API010_VARIABLE_TYPE));
                return TRUE;
            }
        }
    #endif      // #if (API010_MMAP_VARS_COUNT > 0)
    return FALSE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Scan the variables that are setup for publication on change for any new changes.
 * @return TRUE if any variables are set as changed.
 */
static BOOL_TYPE IsOnChangeDataDue(void)
{
    BOOL_TYPE change_found = FALSE;
    #if (API010_ON_CHANGE_BUFFER_SIZE > 0)
        for (uint8 variable_index = 0; variable_index < API010_VARS_COUNT; variable_index++)
        {
            if ((Api010.Control[variable_index].Selected == TRUE) &&
                (Api010.Control[variable_index].On_Change == TRUE))
            {
                if (Api010.Control[variable_index].Changed == TRUE)
                {
                    change_found = TRUE;
                }
                else
                {
                    API010_VARIABLE_TYPE variable;
                    if (GetVariableInfo(variable_index, &variable) == TRUE)
                    {
                        uint16 offset = Api010.Last_Sent_Offset[variable_index];
                        uint8* last_sent = &Api010.Last_Sent[offset];
                        if (memcmp(variable.Address, last_sent, variable.Size) != 0)
                        {
                            Api010.Control[variable_index].Changed = TRUE;
                            change_found = TRUE;
                        }
                    }
                }
            }
        }
    #endif      // #if (API010_ON_CHANGE_BUFFER_SIZE > 0)
    return change_found;
}


#if (API010_ON_CHANGE_BUFFER_SIZE > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Add or subtract a delta value from the Last_Sent_OFfset[] array.
 * @param first_index = The first index that needs updating.
 * @param delta = The amount of change to add or subtract.
 *                Positive values are added. Negative values are subtracted.
 */
static void AddToOffsets(uint8 first_index, sint16 delta)
{
    for (uint8 variable_index = first_index; variable_index <= API010_VARS_COUNT; variable_index++)
    {
        Api010.Last_Sent_Offset[variable_index] += delta;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Insert the specified variable into the on change monitoring.
 * @param variable_index = The variable to insert into "on change" monitoring.
 * @return TRUE if the specified variable was inserted into "on change" monitoring. Otherwise FALSE.
 */
static BOOL_TYPE OnChangeInsertVariable(uint8 variable_index)
{
    // Only insert the variable if it is not already inserted.
    if ((variable_index < API010_VARS_COUNT) &&
        (Api010.Control[variable_index].On_Change == FALSE))
    {
        // Make sure there is space in the Last_Sent[] array for this variable.
        uint16 space_consumed = Api010.Last_Sent_Offset[API010_VARS_COUNT];
        uint16 space_available = API010_ON_CHANGE_BUFFER_SIZE - space_consumed;
        API010_VARIABLE_TYPE variable;
        if ((GetVariableInfo(variable_index, &variable) == TRUE) &&
            (variable.Size <= space_available))
        {
            // Update all the offsets to the Last_Sent[] data.
            AddToOffsets(variable_index + 1, variable.Size);

            // Setup automatic variables to reference the variable's Last_Sent value.
            uint16 var_offset = Api010.Last_Sent_Offset[variable_index];
            uint8* var_data = &Api010.Last_Sent[var_offset];

            // Shift the Last_Sent[] values to make space for the new variable.
            if (variable_index + 1 < API010_VARS_COUNT)
            {
                // Shift the other values to make space for this variable.
                uint8* move_to = &Api010.Last_Sent[var_offset + variable.Size];
                memmove(move_to, var_data, space_consumed - var_offset);
            }

            // Update the Control of the variable so that it is selected, monitored, and changed.
            Api010.Control[variable_index].Selected = TRUE;
            Api010.Control[variable_index].On_Change = TRUE;
            memcpy(var_data, variable.Address, variable.Size);
            Api010.Control[variable_index].Changed = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Remove the specified variable from the on change monitoring.
 * @param variable_index = The variable to remove from "on change" monitoring.
 */
static void OnChangeRemoveVariable(uint8 variable_index)
{
    // Only remove the variable if it is already inserted.
    if ((variable_index < API010_VARS_COUNT) &&
        (Api010.Control[variable_index].On_Change == TRUE))
    {
        // Get the variable information.
        API010_VARIABLE_TYPE variable;
        if (GetVariableInfo(variable_index, &variable) == TRUE)
        {
            // Update all the offsets to the Last_Sent[] data.
            AddToOffsets(variable_index + 1, -variable.Size);

            // Setup automatic variables to reference the variable's Last_Sent value.
            uint16 var_offset = Api010.Last_Sent_Offset[variable_index];
            uint8* var_data = &Api010.Last_Sent[var_offset];

            // Shift the Last_Sent[] values to collapse the space that is no longer needed.
            if (variable_index + 1 < API010_VARS_COUNT)
            {
                // Shift the other values to make space for this variable.
                uint8 space_consumed = Api010.Last_Sent_Offset[API010_VARS_COUNT];
                uint8* move_from = &var_data[variable.Size];
                memmove(var_data, move_from, space_consumed - var_offset);
            }

            // Update the Control of the variable so that it is not monitored or changed.
            Api010.Control[variable_index].On_Change = FALSE;
            Api010.Control[variable_index].Changed = FALSE;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the "on change" behavior for one or more variables.
 * There are no checks for out of range values.
 *      first_index should be less than or equal to last_index.
 *      Both values should be in the range [0 .. API010_VARS_COUNT).
 * @param on_change = Desired "on change" behavior for the selected variable(s).
 * @param first_index = First variable index in the range of variables to affect.
 * @param last_index = Last variable index in the range of variables to affect.
 * @return TRUE if the on change behavior was updated. Otherwise FALSE.
 */
static BOOL_TYPE SetOnChangeBehavior(BOOL_TYPE on_change, uint8 first_index, uint8 last_index)
{
    uint8 variables_added = 0;
    for (uint8 variable_index = first_index; variable_index <= last_index; variable_index++)
    {
        if (on_change == TRUE)
        {
            if ((Api010.Control[variable_index].Selected == TRUE) &&
                (OnChangeInsertVariable(variable_index) == TRUE))
            {
                variables_added++;
            }
        }
        else
        {
            OnChangeRemoveVariable(variable_index);
        }
    }
    return (BOOL_TYPE)((on_change == FALSE) || (variables_added > 0));
}
#endif      // #if (API010_ON_CHANGE_BUFFER_SIZE > 0)


#if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Send the next packet of metadata information.
 * If Reveal returns an error, then the function will try to send the same packet on the next call.
 */
static void PublishNextMetadata(void)
{
    // Determine the size of the payload.
    uint8 metadata_in_packet = Api010.Remaining_Metadata;
    if (metadata_in_packet > API010_MAXIMUM_METADATA_PER_PACKET)
    {
        metadata_in_packet = API010_MAXIMUM_METADATA_PER_PACKET;
    }
    uint8 payload_size = metadata_in_packet * sizeof(API010_VARIABLE_METADATA_TYPE);
    REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_FIXED_VARIABLE_METADATA, Api010.Reply_To, payload_size);

    // Determine the number of variables whose metadata will fit in the payload.
    if (msg != NULL)
    {
        BOOL_TYPE success = TRUE;       // Assume no errors while building the packet.
        uint8 metadata_in_packet = payload_size / sizeof(API010_VARIABLE_METADATA_TYPE);
        uint8 first_variable_in_packet = API010_FIXED_VARS_COUNT - Api010.Remaining_Metadata;
        for (uint8 i = 0; i < metadata_in_packet; i++)
        {
            uint8 variable_index = first_variable_in_packet + i;
            if (RevealAppendMetadata(msg, variable_index) == FAIL)
            {
                success = FALSE;
                break;                  // Out of the for() loop.
            }
        }
        if (success == TRUE)
        {
            // If we have more messages to send, we set the MMP flag.
            if (metadata_in_packet < Api010.Remaining_Metadata)
            {
                BIT_SET(msg->opcode, 7);
            }
            //Queuing and updating the index.
            if (Reveal__QueueCommand(msg) == TRUE)
            {
                Api010.Remaining_Metadata -= metadata_in_packet;
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Append the specified variable's metadata information to the provided Reveal message.
 * @param variable_index = The index of the requested variable.
 * @param msg = Pointer to the previously allocated Reveal message. This value must not be NULL.
 * @return PASS if the metadata was successfully appended. Otherwise FAIL.
 */
static PASS_FAIL_TYPE RevealAppendMetadata(REVEAL_COMMAND_TYPE *msg, uint8 variable_index)
{
    VARIABLE_TYPE variable;
    API010_VARIABLE_METADATA_TYPE metadata;
    memset(&metadata, 0, sizeof(API010_VARIABLE_METADATA_TYPE));
    if (variable_index < API010_FIXED_VARS_COUNT)
    {
        if (Variable__GetByIndex((VARIABLE_ID_TYPE)variable_index, &variable) == TRUE)
        {
            metadata.Variable_Index = variable_index;
            metadata.Variable_Array_Elements = variable.Length;
            metadata.Variable_Element_Type = variable.Type;
            // TODO: add key from KVP here when available.
            metadata.Key = variable_index;
            return Reveal__AddDataU8Buffer(msg, (uint8*)(void*)&metadata, sizeof(metadata));
        }
    }
    return FAIL;
}
#endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)


#if (API010_MMAP_VARS_COUNT > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Remove the specified memory-mapped variable from the list of variables in the periodic data.
 * @param mmap_index = An index that identifies the memory-mapped variable configuration slot.
 *                     This value must be in the range [0..API010_MMAP_VARS_COUNT).
 *                     A value not in this range will have no effect.
 */
static void RemoveMemoryMappedVariable(uint8 mmap_index)
{
    if (mmap_index < API010_MMAP_VARS_COUNT)
    {
        memset(&Api010.Mem_Mapped_Vars[mmap_index], 0, sizeof(API010_VARIABLE_TYPE));
        Api010.Control[mmap_index + API010_FIXED_VARS_COUNT].Selected = FALSE;
        #if (API010_ON_CHANGE_BUFFER_SIZE > 0)
            Api010.Control[mmap_index + API010_FIXED_VARS_COUNT].On_Change = FALSE;
        #endif
    }
}
#endif      // #if (API010_MMAP_VARS_COUNT > 0)


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Check if any data is due for publication.
 * First checks the time to see if the periodic data should be published.
 * Then checks for any changes in variables that are monitored "on change".
 * If data is due for publication, then it will generate the evProceed event.
 */
static void CheckDataDue(void)
{
    if (Api010.Is_Periodic_Data_Due == TRUE)
    {
        Api010.Is_Periodic_Data_Due = FALSE;
        // Start publishing the periodic data.
        if (BuildStream(TRUE) == TRUE)
        {
            EventProceed();
        }
    }
    else if (IsOnChangeDataDue() == TRUE)
    {
        // Start publishing the changed data.
        if (BuildStream(FALSE) == TRUE)
        {
            EventProceed();
        }
    }
    else
    {
        // Check if there is still variable metadata that must be sent.
        CheckMetadataDue();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Check if more variable metadata should be sent out.
 */
static void CheckMetadataDue(void)
{
    #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
        // Check if there is still variable metadata that must be sent.
        if (Api010.Remaining_Metadata > 0)
        {
            PublishNextMetadata();
        }
    #endif      // #if (API010_USES_FIXED_VARIABLES_FEATURE == ENABLED)
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Publish the next Reveal packet that makes up the periodic data.
 * If Reveal returns an error, then the function will try to send the same packet on the next call.
 */
static void PublishNextPacket(void)
{
    switch (Api010.Method)
    {
        case API010_METHOD_REVEAL:
            if (Api010.Stream_Index < Api010.Stream_Length)
            {
                // Check if the rest of the stream will fit in a single Reveal packet.
                uint8 size;
                if ((Api010.Stream_Length - Api010.Stream_Index) > (REVEAL_PAYLOAD_SIZE - sizeof(uint16)))
                {
                    // It does not fit. Send as must as will fit.
                    size = REVEAL_PAYLOAD_SIZE - sizeof(uint16);
                }
                else
                {
                    // It does fit. Send the whole thing.
                    size = Api010.Stream_Length - Api010.Stream_Index;
                }

                // Prepare the Reveal message.
                REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM,
                                                               API010_FBK_PUBLISH_DATA_STREAM,
                                                               REVEAL_BROADCAST_NODE,
                                                               size + sizeof(API010_STREAM_INDEX_TYPE));
                if ((Reveal__AddDataU16(msg, Api010.Stream_Index) == PASS) &&
                    (Reveal__AddDataU8Buffer(msg, &Api010.Stream_Buffer[Api010.Stream_Index], size) == PASS) &&
                    (Reveal__QueueCommand(msg) == TRUE))
                {
                    // Update the number of bytes sent.
                    Api010.Stream_Index += size;
                }
            }
            else
            {
                // Send the termination packet and return to the STATE_API_10_PUBLISHER_ACTIVE state.
                REVEAL_COMMAND_TYPE* msg = Reveal__NewFeedback(API010POLLVAR_NUM, API010_FBK_PUBLISH_DATA_STREAM, REVEAL_BROADCAST_NODE, 3);
                if ((Reveal__AddDataU16(msg, 0xFFFF) == PASS) &&
                    (Reveal__AddDataU8(msg, Api010.Stream_Csum) == PASS) &&
                    (Reveal__QueueCommand(msg) == TRUE))
                {
                    EventProceed();
                }
            }
            break;

        case API010_METHOD_SPI:
            #if (API010_CAN_USE_SPI_MODULE == ENABLED)
                if (Spi__Write(API010POLLVAR_SPI_CHANNEL, Api010.Stream_Buffer, Api010.Stream_Length) == TRUE)
                {
                    EventProceed();
                }
            #elif (API010_CAN_USE_SPIMGR_MODULE == ENABLED)
                if (SpiMgr__QueueMsg(API010_SPIMGR_DEVICE, Api010.Stream_Buffer, Api010.Rx_Buffer, Api010.Stream_Length) == TRUE)
                {
                    EventProceed();
                }
            #else
                EventProceed();
            #endif
            break;

        default:
            EventProceed();
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Reset API010 Publisher variables.
 * Restarts the timer for periodic data.
 * Re-enables publication of errors.
 */
static void ResetPublisher(void)
{
    Api010.Publication_Counter = 0;
    Api010.Enable_Error_Reporting = TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the timer that keeps track of when to send the next periodic data.
 */
static void UpdateTimer(void)
{
    uint8 new_counter = (uint16)(Millis__GetFreeRunningCounter() / Api010.Publication_Period);
    if (Api010.Publication_Counter != new_counter)
    {
        Api010.Publication_Counter = new_counter;
        Api010.Is_Periodic_Data_Due = TRUE;
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for API 10 Publisher State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the API 10 Publisher state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void API10PublisherExecute(void)
{
    switch(Api010.State)
    {
        case STATE_API_10_PUBLISHER_INACTIVE:
            CheckMetadataDue();                     // Do function.
            break;

        case STATE_API_10_PUBLISHER_ACTIVE:
            UpdateTimer();                          // Do function.
            CheckDataDue();                         // Do function.
            break;

        case STATE_API_10_PUBLISHER_PUBLISHING:
            UpdateTimer();                          // Do function.
            PublishNextPacket();                    // Do function.
            break;

        default:
            // There is nothing to do for all other values of Api010.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the API 10 Publisher state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void API10PublisherInitialize(void)
{
    Api010.State = STATE_API_10_PUBLISHER_INACTIVE;
    ResetPublisher();                               // Entry Function.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Proceed Event for the API 10 Publisher state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventProceed(void)
{
    switch (Api010.State)
    {
        case STATE_API_10_PUBLISHER_ACTIVE:
            Api010.State = STATE_API_10_PUBLISHER_PUBLISHING;
            PublishNextPacket();                    // Entry Function.
            break;

        case STATE_API_10_PUBLISHER_PUBLISHING:
            Api010.State = STATE_API_10_PUBLISHER_ACTIVE;
            break;

        default:
            // Event is ignored for all other values of Api010.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Publishing Event for the API 10 Publisher state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartPublishing(void)
{
    switch (Api010.State)
    {
        case STATE_API_10_PUBLISHER_INACTIVE:
            Api010.State = STATE_API_10_PUBLISHER_ACTIVE;
            break;

        default:
            // Event is ignored for all other values of Api010.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Stop Publishing Event for the API 10 Publisher state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStopPublishing(void)
{
    switch (Api010.State)
    {
        case STATE_API_10_PUBLISHER_ACTIVE:
        case STATE_API_10_PUBLISHER_PUBLISHING:
            Api010.State = STATE_API_10_PUBLISHER_INACTIVE;
            ResetPublisher();                       // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Api010.State.
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


#endif      // #if (API010POLLVAR_FEATURE == ENABLED)


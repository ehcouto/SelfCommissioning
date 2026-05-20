/**
 *  @file
 *  @brief      Automatically generated public parameters for the Log module.
 *
 *  @details    This file was automatically generated on 29/03/2022 10:46:29 by
 *              LogPreprocessor.exe (v1.3.0)  Copyright © 2013-2017 Whirlpool Corporation.
 *
 *              LogPreprocessor.exe attempts to preserve user settings.
 *              The user should test to make sure that changes to this file are handled as expected.
 *
 *  $Header: Program.cs 1.13 2015/10/30 11:22:11EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 *  @copyright  Copyright 2012 - $Date: 2015/10/30 11:22:11EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef LOG_PRM_H_
#define LOG_PRM_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Parameters ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Compiler Directives --


//---------------------------------------------------------------------------------------------------------------------
/**
 * The LOG_MODULE_ENABLE declaration is used to enable or disable the Log module.
 *
 * Valid settings:
 *
 * ENABLED = Module works as intended. Functions are defined and compiled into the code.
 *
 * DISABLED = The module's public interface is replaced with dummy macros so that the system will
 *          compile. However, none of the functionality is present.
 *
 * The Log.h file shall declare LOG_MODULE_ENABLE as ENABLED if it is missing.
 */
#define LOG_MODULE_ENABLE                           (DISABLED)


//---------------------------------------------------------------------------------------------------------------------
/**
 * The LOG_PEEK_BUFFER_FEATURE declaration is used to enable or disable the Log__PeekBuffer()
 * function.
 *
 * ENABLED = The Log__PeekBuffer() function is defined and available to the application.
 *
 * DISABLED = The Log__PeekBuffer() function is not defined.
 *
 * The Log.h file shall declare LOG_PEEK_BUFFER_FEATURE as DISABLED if it is missing.
 */
#define LOG_PEEK_BUFFER_FEATURE                     (DISABLED)


//---------------------------------------------------------------------------------------------------------------------
/**
 * The LOG_RUN_TIME_FILTERING declaration is used to enable or disable the Log__SetMessageFilter()
 * function.
 *
 * ENABLED = The Log__SetMessageFilter() function is defined and available to the application.
 *          Initial message filters are determined at compile-time through the Log_prva.h file,
 *          but they can be modified at run-time through the Log__SetMessageFilter() function.\
 *          This feature requires an extra byte of RAM for each module that generates Log Messages.
 *
 * DISABLED = The Log__SetMessageFilter() function is not defined. Message filters are determined
 *          at compile-time through the Log_prva.h file.
 *
 * The Log.h file shall declare LOG_RUN_TIME_FILTERING as DISABLED if it is missing.
 */
#define LOG_RUN_TIME_FILTERING                      (DISABLED)


// -- Constant Declarations --

/**
 * The maximum number of Log Messages that the Message Queue can hold.
 * Set this to a value in the range [1..255].
 * The Log module will allocate 5 bytes of RAM for each message that the buffer can hold.
 */
#define LOG_MAX_MESSAGES                            (10)


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Automated Parameters ------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Automated list of Module Identifiers for all the application modules that generate Log Messages.
 *
 * The LogPreprocessor.exe tool searches all project code for any source files that declare
 * Message Identifiers. The search looks for the pattern below where <NAME> is replaced with the
 * name of the module.
 * <pre>
 *      typedef enum
 *      {
 *          // Log messages enumerated here...
 *      } MODULE_<NAME>_LOG_MESSAGE_ID_TYPE;
 * </pre>
 *
 * For example, if a 'Hello' module declares the MODULE_HELLO_LOG_MESSAGE_ID_TYPE type, then the
 * LogPreprocessor.exe tool will add an enumerated value 'MODULE_HELLO' to the LOG_MODULE_ID_TYPE
 * declared below.
 */
typedef enum LOG_MODULE_ID_ENUM
{
    MODULE_API007APP              = 0,   // 0x00
    MODULE_API007BULKDATA         = 1,   // 0x01
    MODULE_API010_POLL_VAR        = 2,   // 0x02
    MODULE_API011APPCTRL          = 3,   // 0x03
    MODULE_API013_REMOTE_FUNCTION = 4,   // 0x04
    MODULE_API221_MOTION_CTRL     = 5,   // 0x05
    MODULE_MODE                   = 6,   // 0x06
    MODULE_PRODUCTINFO            = 7,   // 0x07
    MODULE_SETTINGFILE            = 8,   // 0x08
    MODULE_SYSTEMTIMERS           = 9,   // 0x09

    //! Number of modules that can add Log Messages to the Message Queue.
    NUMBER_OF_MODULES             = 10
} LOG_MODULE_ID_TYPE;


//---------------------------------------------------------------------------------------------------------------------
/**
 * Automated list of unique identifiers for all application specific Log Messages from all modules.
 *
 * The LogPreprocessor.exe tool searches the code for any source files that declare log messages.
 * Every enumerated message from every module is assigned a unique identifier based on the
 * Module Identifier and the Message Identifier.
 *
 * The debugger will then display the enumeration text instead of the decimal value, making
 * viewing the Log messages much easier during debugging.
 */
typedef enum LOG_MODULE_MESSAGE_ID_ENUM
{
    MODULE_API007APP_INVALID_CRC_FAILED                           = 1,     // 0x0001
    MODULE_API007BULKDATA_MESSAGE_NONE                            = 256,   // 0x0100
    MODULE_API010_POLL_VAR_TOO_MUCH_DATA                          = 513,   // 0x0201
    MODULE_API011APPCTRL_TO_MANY_CONSECUTIVE_REGULATIONS_SETS     = 768,   // 0x0300
    MODULE_API011APPCTRL_TO_MANY_CONSECUTIVE_REGULATIONS_REQUESTS = 769,   // 0x0301
    MODULE_API011APPCTRL_TO_MANY_CONSECUTIVE_STATUS_REQUESTS      = 770,   // 0x0302
    MODULE_API013_REMOTE_FUNCTION_FEEDBACK_MESSAGE_NOT_HANDLED    = 1025,  // 0x0401
    MODULE_API221_MOTION_CTRL_MOTOR_STOPPED_DUE_TO_TIMEOUT        = 1281,  // 0x0501
    MODULE_API221_MOTION_CTRL_MOTOR_REJECTED_STOP_FROM_TIMEOUT    = 1282,  // 0x0502
    MODULE_API221_MOTION_CTRL_PERIODIC_PUBLICATION_FAILED         = 1283,  // 0x0503
    MODULE_API221_MOTION_CTRL_FEEDBACK_OPCODE_NOT_SUPPORTED       = 1284,  // 0x0504
    MODULE_API221_MOTION_CTRL_ADD_CHANS_INVALID_PARAMS            = 1285,  // 0x0505
    MODULE_API221_MOTION_CTRL_CLEAR_FAILURES_INVALID_PARAMS       = 1286,  // 0x0506
    MODULE_API221_MOTION_CTRL_MOTION_INVALID_PARAMS               = 1287,  // 0x0507
    MODULE_API221_MOTION_CTRL_PULSE_INVALID_PARAMS                = 1288,  // 0x0508
    MODULE_API221_MOTION_CTRL_PULSE_INVALID_COMMAND               = 1289,  // 0x0509
    MODULE_API221_MOTION_CTRL_ROTATE_INVALID_PARAMS               = 1290,  // 0x050A
    MODULE_API221_MOTION_CTRL_RUN_INVALID_PARAMS                  = 1291,  // 0x050B
    MODULE_API221_MOTION_CTRL_STOP_INVALID_PARAMS                 = 1292,  // 0x050C
    MODULE_API221_MOTION_CTRL_WASH_INVALID_PARAMS                 = 1293,  // 0x050D
    MODULE_API221_MOTION_CTRL_GET_DATA_INVALID_PARAMS             = 1294,  // 0x050E
    MODULE_API221_MOTION_CTRL_GET_SYNC_DATA_INVALID_PARAMS        = 1295,  // 0x050F
    MODULE_API221_MOTION_CTRL_IS_MOTOR_PRESENT_INVALID_PARAMS     = 1296,  // 0x0510
    MODULE_API221_MOTION_CTRL_DEPRECATED_KEEP_RUNNING_API         = 1297,  // 0x0511
    MODULE_API221_MOTION_CTRL_REMOVE_CHANS_INVALID_PARAMS         = 1298,  // 0x0512
    MODULE_API221_MOTION_CTRL_REQ_ANALOG_DATA_INVALID_PARAMS      = 1299,  // 0x0513
    MODULE_API221_MOTION_CTRL_REQ_FAILURES_INVALID_PARAMS         = 1300,  // 0x0514
    MODULE_API221_MOTION_CTRL_REQ_PER_DATA_STATUS_INVALID_PARAMS  = 1301,  // 0x0515
    MODULE_API221_MOTION_CTRL_REQ_STATUS_INVALID_PARAMS           = 1302,  // 0x0516
    MODULE_API221_MOTION_CTRL_SET_PERIOD_INVALID_PARAMS           = 1303,  // 0x0517
    MODULE_MODE_INVALID_SYSTEM_TIMER                              = 1536,  // 0x0600
    MODULE_PRODUCTINFO_INVALID_PROJECT_DATA                       = 1792,  // 0x0700
    MODULE_PRODUCTINFO_INVALID_BOARD                              = 1793,  // 0x0701
    MODULE_PRODUCTINFO_INVALID_PROJECT                            = 1794,  // 0x0702
    MODULE_SETTINGFILE_UNHANDLED_SECTION_ENDIANNESS               = 2048,  // 0x0800
    MODULE_SETTINGFILE_UNHANDLED_HARDCODED_ENDIANNESS             = 2049,  // 0x0801
    MODULE_SYSTEMTIMERS_INVALID_HANDLE                            = 2305,  // 0x0901

    //! This is a purposely invalid Module/Message ID used to identify invalid Log Messages.
    MODULE_LOG_INVALID_MESSAGE                                    = 65535
} LOG_MODULE_MESSAGE_ID_TYPE;



#endif  // LOG_PRM_H_



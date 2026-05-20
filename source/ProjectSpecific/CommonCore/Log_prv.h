/**
 *  @file
 *  @brief      Automatically generated private parameters for the Log module.
 *
 *  @details    This file was automatically generated on 03/10/2018 15:44:05 by
 *              LogPreprocessor.exe (v1.2.0)  Copyright © 2013-2017 Whirlpool Corporation.
 *
 *              LogPreprocessor.exe attempts to preserve user settings.
 *              The user should test to make sure that changes to this file are handled as expected.
 *
 *  $Header: Program.cs 1.13 2015/10/30 11:22:11EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 *  @copyright  Copyright 2012 - $Date: 2015/10/30 11:22:11EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef LOG_PRV_H_
#define LOG_PRV_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Private Parameters --------------------------------------------------------------------------------------------
//=====================================================================================================================
//! The number of bytes required to store a single log message.
//! This value is exposed publically so that the actual structure does not need to be exposed.
//! There is a compile-time assertion to verify that this value matches the size of the structure.
#define LOG_SIZE_OF_MESSAGE 4




// -- Compiler Directives --


//---------------------------------------------------------------------------------------------------------------------
/**
 * The LOG_OVERWRITE_OLDEST_MESSAGE declaration configures how the Log module reacts when a Log
 * Message is added to a full Message Queue.
 *
 * TRUE = When a Log Message is added to a full Message Queue, the Log module will overwrite the
 *          oldest Log Message in the Message Queue in order to fit the new Log Message.
 *
 * FALSE = When a Log Message is added to a full Message Queue, the Log module will ignore the
 *          request and drop the new Log Message.
 *
 * For users that use the Log__RemoveMessage() function to read the contents of the Message Queue,
 * it is recommended that this configuration be set to FALSE. If a rogue appliation module floods
 * the Message Queue, the Log Messages leading up to the flood are typically more useful than the
 * repeated Log Message that generates the flood.
 *
 * For users that use the Log__PeekBuffer() function exclusively to read the contents of the
 * Message Queue, this configuration should be set to TRUE. Remember that the Log__PeekBuffer()
 * function does not remove Log Messages from the Message Queue; so after a few Log Messages have
 * been added, the Message Queue will be full and will remain full.
 */
#define LOG_OVERWRITE_OLDEST_MESSAGE                (FALSE)


// -- Private Variable Definitions --

#if (LOG_MODULE_ENABLE == ENABLED)
//! The memory buffer that holds the Log Message Queue data.
static uint8 Buffer[LOG_MAX_MESSAGES * LOG_SIZE_OF_MESSAGE];
#endif


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Macro Declarations --


//---------------------------------------------------------------------------------------------------------------------
/**
 * The Log module calls this macro when an attempt is made to add a Log Message to the Message
 * Queue when the Message Queue is full.
 *
 * @param mod = (LOG_MODULE_ID_TYPE) The Module Identifier that was attached to the Log Message
 *          that generated this call.
 */
#define DEBUG_MESSAGE_QUEUE_OVERFLOW(mod)


//---------------------------------------------------------------------------------------------------------------------
/**
 * The Log module calls this macro when an attempt is made to remove a Log Message from the
 * Message Queue when the Message Queue is empty.
 */
#define DEBUG_MESSAGE_QUEUE_UNDERFLOW()


//---------------------------------------------------------------------------------------------------------------------
/**
 * The Log module calls this macro when an invalid Module Identifier is passed as part of a Log
 * Message.
 *
 * @param mod = (LOG_MODULE_ID_TYPE) The Module Identifier that was invalid.
 */
#define DEBUG_MODULE_ID_INVALID(mod)


//---------------------------------------------------------------------------------------------------------------------
/**
 * The Log module calls this macro when an attempt to peek at the contents of the Message Queue is
 * incorrectly requested with a count that is out of range.
 *
 * @param ref = The relative position of the Log Message in the Message Queue.
 *          0 = The position of the most recently added Log Message.
 *          1 = The position of the Log Message before the most recently added Log Message.
 *          etc.
 */
#define DEBUG_PEEK_OUT_OF_RANGE(ref)


//---------------------------------------------------------------------------------------------------------------------
/**
 * The Log module calls this macro when an invalid Message Severity is passed as part of a Log
 * Message.
 *
 * @param mod = (LOG_MODULE_ID_TYPE) The Module Identifier associated with the passed Log Message.
 *          This value can be used to find the source of the problem.
 * @param sev = (LOG_SEVERITY_ID_TYPE) The Message Severity that was invalid.
 */
#define DEBUG_SEVERITY_ID_INVALID(mod, sev)


// -- Private Constant Declarations --

//! Define the API number used to report log messages.
#define LOG_REPORT_API          0x04

//! Define the OpCode number used to report log messages.
#define LOG_REPORT_OPCODE       0x0F


//=====================================================================================================================
//  --- Automated Parameters ------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Automated table of severity filters to enable/disable Log Messages by Message Severity.
 *
 * The LogPreprocessor.exe tool will add entries to this table and remove entries from this table
 * to keep it synchronized with the #LOG_MODULE_ID_TYPE. The tool will attempt to preserve an
 * entry once it is present in the table.
 *
 * The #LOG_MODULE_ID_TYPE is used as an index into this table. Each row in the table represents
 * the filter for a specific application module. The application module's name must be in the
 * comment at the end for the LogPreprocessor.exe tool to know which data belongs with which
 * module.
 *
 * If the #LOG_RUN_TIME_FILTERING is ENABLED, then this table is just the initial setting and
 * these values can be changed at run-time by calling the Log__SetMessageFilter() function.
 */
static const LOG_SEVERITY_FILTER_TYPE LOG_SEVERITY_FILTERS_TABLE[] =
{
//  (LOG_SEVERITY_FILTER_TYPE)(LOG_ALLOW_INFO | LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_NAME
//                            --------------------------------------------------------------------------
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_API007APP
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_API007BULKDATA
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_API010_POLL_VAR
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_API011APPCTRL
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_API013_REMOTE_FUNCTION
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_API221_MOTION_CTRL
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_MODE
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_PRODUCTINFO
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR),   // MODULE_SETTINGFILE
    (LOG_SEVERITY_FILTER_TYPE)(                 LOG_ALLOW_EXCEPTION | LOG_ALLOW_EVENT | LOG_ALLOW_ERROR)    // MODULE_SYSTEMTIMERS
};



#endif  // LOG_PRV_H_



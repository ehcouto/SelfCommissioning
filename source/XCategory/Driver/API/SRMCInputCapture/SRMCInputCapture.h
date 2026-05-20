/**
 *  @file       SRMCInputCapture.h
 *  @defgroup   CLASS_B
 *
 *  @brief      Header for motor control Input Capture
 *
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRMCINPUTCAPTURE_H_
#define SRMCINPUTCAPTURE_H_

#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Define the enumerator feedback to get the input capture status
typedef enum
{
    INPUTCAPTURE_UNDEFINED = 0,                     //!< 0 - Undefined status, ex., cable disconnected?
    INPUTCAPTURE_ENABLED,                           //!< 1 - Input capture enabled
    INPUTCAPTURE_DISABLED,                          //!< 2 - Input capture disabled
    INPUTCAPTURE_NOT_CONFIGURED,                    //!< 3 - Input capture not configured
} INPUTCAPTURE_STATUS_TYPE;

//! Input Capture diagnostic handler states
typedef enum
{
    SR_IC_TEST_NOT_PERFORMED,
    SR_IC_TEST_NOT_INITIALIZED,
    SR_IC_MIN_TACH_ERROR,
    SR_IC_MAX_SPEED_ERROR,
    SR_IC_PERIPHERAL_SAFE,
    SR_IC_ERROR_PERIPHERAL_NOT_SAFE,
} SRMCINPUTCAPTURE_DIAG_ERROR_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRMCInputCapture__Initialize(void);
PASS_FAIL_TYPE SRMCInputCapture__EnableCapture(void);
PASS_FAIL_TYPE SRMCInputCapture__DisableCapture(void);
uint32 SRMCInputCapture__GetCounterValue(void);
INPUTCAPTURE_STATUS_TYPE SRMCInputCapture__GetStatus(void);
uint8 SRMCInputCapture__GetMatchEvent(void);
BOOL_TYPE SRMCInputCapture__GetMovementStatus(uint32 thr);
uint32 SRMCInputCapture__GetMatchEventCounter(void);
void SRMCInputCapture__PwmHandler(void);
SRMCINPUTCAPTURE_DIAG_ERROR_TYPE SRMCInputCapture__GetDiagnosticFeedback(void);
PASS_FAIL_TYPE SRMCInputCapture__InitDiagnostic(void);
uint32 SRMCInputCapture__GetUndertorqueCounter(void);
BOOL_TYPE SRMCInputCapture__ResetUndertorqueCounter(void);
BOOL_TYPE SRMCInputCapture__GetSpeedRegion(void);

void SRMCInputCapture__HilPwmHandler(void);

#endif // SRMCINPUTCAPTURE_H_

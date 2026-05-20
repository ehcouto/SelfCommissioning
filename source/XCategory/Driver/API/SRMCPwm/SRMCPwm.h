/**
 *  @file       SRMCPwm.h
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMCPWM_H_
	#define SRMCPWM_H_
#include "C_Extensions.h"
#include "SRMCPwm_prm.h"
#include "SRMCPwm_macros.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
    SR_MCPWM_ERROR_SHUTDOWN_ALREADY_ON        = 0,
    SR_MCPWM_ERROR_REGISTER_STUCK_AT_1        = 1,
    SR_MCPWM_ERROR_REGISTER_STUCK_AT_0        = 2,
    SR_MCPWM_ERROR_WAITING_START_TRIGGER      = 3,
    SR_MCPWM_ERROR_PERIPHERAL_NOT_SAFE        = 4,
    SR_MCPWM_ERROR_PERIPHERAL_SAFE            = 5,
    SR_MCPWM_ERROR_CHECKING                   = 6
} SRMCPWM_DIAG_ERROR_TYPE;

typedef enum
{
    SR_MCPWM_WAITING_TO_CHECK,
    SR_MCPWM_CHECK_SIGNAL_POL,
    SR_MCPWM_CHECK_STUCK_AT_1,
    SR_MCPWM_CHECK_STUCK_AT_0,
    SR_MCPWM_FEEDBACK_UPDATED
} SRMCPWM_DIAG_STATE_TYPE;


typedef enum
{
    // Definition for a specific gate
    SR_MCPWM_GATE_A_HI,
    SR_MCPWM_GATE_A_LO,
    SR_MCPWM_GATE_B_HI,
    SR_MCPWM_GATE_B_LO,
    SR_MCPWM_GATE_C_HI,
    SR_MCPWM_GATE_C_LO,

    // Definition for either all high/low side
    SR_MCPWM_GATE_ALL_HI,
    SR_MCPWM_GATE_ALL_LO,

    // Definition for all gates
    SR_MCPWM_GATE_ALL_6GATES,
} SRMCPWM_GATE_NAME_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void SRMCPwm__Initialize_Before_Boot(void);
void SRMCPwm__Initialize(uint16 pwm_frequency_hz, uint16 dead_time_ns);

INTERRUPT_DIRECTIVE void SRMCPwm__PwmISR(void);                           // ISR of PWM event.

void SRMCPwm__TurnOnLowSideGates(void);
void SRMCPwm__TurnOffLowSideGates(void);
void SRMCPwm__TurnOnHighSideGates(void);
void SRMCPwm__TurnOffHighSideGates(void);
void SRMCPwm__TurnOffAllGates(void);

uint16 SRMCPwm__GetMaxPwm(void);
void SRMCPwm__LoadAllPwmValues(uint16 pwm_a, uint16 pwm_b, uint16 pwm_c);

void SRMCPwm__StartCounter(void);
void SRMCPwm__StopCounter(void);

// This two methods shall also be able to force this level to all or one switch.
void SRMCPwm__EnablePwmOutput(SRMCPWM_GATE_NAME_TYPE gate_name);
void SRMCPwm__DisablePwmOutput(SRMCPWM_GATE_NAME_TYPE gate_name);

uint16 SRMCPwm__GetPwmFrequencyHz(void);
uint16 SRMCPwm__GetDeadtimeNanosec(void);
unsigned char SRMCPwm__GetHwFaultStatus(void);
void SRMCPwm__Handler250us(void);


void SRMCPwm__DiagnosticHandler(void);
void SRMCPwm__EnableDiagnostic(uint32 timeout_to_zero, uint32 timeout_to_one);
SRMCPWM_DIAG_ERROR_TYPE SRMCPwm__GetDiagnosticFeedback(void);

void SRMCPwm__UpdatePeriod(uint32 pwm_period_perc);

#endif // SRMCPWM_H_



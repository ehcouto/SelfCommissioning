/**
 *  @file       SRException.h
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2007.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SREXCEPTION_H_
	#define SREXCEPTION_H_

#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    EXCEPTION_NONE = 0,
    SREXCEPTION_SRDATA_NOISE,
    SREXCEPTION_SRDATA_BAD_MEMORY,
    SREXCEPTION_SRDATA_NOT_ALLOWED,
    EXCEPTION_CPU_REGISTER_ERROR,
    EXCEPTION_MAIN_DATA_BYTE,
    EXCEPTION_MAIN_DATA_SHORT,
    EXCEPTION_MAIN_DATA_LONG,
    EXCEPTION_MAIN_DATA_NOT_ALLOWED,
    EXCEPTION_ISR_DATA_BYTE,
    EXCEPTION_ISR_DATA_SHORT,
    EXCEPTION_ISR_DATA_LONG,
    EXCEPTION_ISR_DATA_NOT_ALLOWED,
    EXCEPTION_DATA_BYTE,
    EXCEPTION_DATA_SHORT,
    EXCEPTION_DATA_LONG,
    EXCEPTION_MAIN_FLOW,
    EXCEPTION_ISR_FLOW,
    EXCEPTION_INIT_FLOW,
    EXCEPTION_SRGPIO_CONFIG,
    EXCEPTION_SRGPIO_DATA,
    EXCEPTION_SRGPIO_RANGE,
    EXCEPTION_CLOCK_TEST,
    EXCEPTION_MAX_ABSOLUT_CLOCK,
    SREXCEPTION_SRCLOCK_TEST,
    SREXCEPTION_SRCLOCK_OVERRUN,
    EXCEPTION_ATOD_STUCK,
    EXCEPTION_SRATOD_CONFIG,
    EXCEPTION_STACK_OVERFLOW_ERROR,
    EXCEPTION_STACK_UNDERFLOW_ERROR,
    EXCEPTION_FLASH_ERROR,
    EXCEPTION_MAIN_SLOT_VIOLATION,
    EXCEPTION_RAM_RUNTIME_CORRUPTION,
    EXCEPTION_RAM_STARTUP_CORRUPTION,
    EXCEPTION_SEND_PACKET_FAIL,
    EXCEPTION_DIVISION_BY_ZERO,
    SREXCEPTION_SRISR_OVERRUN,
    SREXCEPTION_SRISR_SEQUENCE_MISMATCH,
    SREXCEPTION_SRISR_TIMEOUT,
    SREXCEPTION_MICRO_RESET_SIGNATURE_CORUPTION,
    SREXCEPTION_GPIO_CONFIGURATION_CORRUPTION,
    SREXCEPTION_GPIO_DATA_CORRUPTION,
    SREXCEPTION_HBL_DATA_NOT_CONSISTENT,
    SREXCEPTION_TIMER_CONFIGURATION_CORRUPTION,
    SREXCEPTION_TIMER_DATA_CORRUPTION,
    SREXCEPTION_MICRO_CPU_REGISTER_TEST,
    SREXCEPTION_MICRO_RAM_INTEGRITY_TEST,
    SREXCEPTION_MICRO_FLASH_INTEGRITY_TEST,
    SREXCEPTION_STACK_OVERFLOW,
} EXCEPTION_EVENT_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRException__Initialize(void);
void SRException__Queue(EXCEPTION_EVENT_TYPE event,uint16 data1, uint16 data2);



#endif // SREXCEPTION_H_



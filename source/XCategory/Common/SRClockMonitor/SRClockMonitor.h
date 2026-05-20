/**
 *  @file
 *
 *  @defgroup CLASS_B
 *  @brief      Public interface of the SRClockMonitor module.
 *
 *  $Header: SRClockMonitor.h 1.3 2012/06/18 18:04:27EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2010-$Date: 2012/06/18 18:04:27EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef SRCLOCKMONITOR_H_
#define SRCLOCKMONITOR_H_
#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRClockMonitor__Initialize(void);
void SRClockMonitor__Monitor1ms(void);
void SRClockMonitor__MonitorExtClk(uint16 ext_ticks);
#endif // SRCLOCKMONITOR_H_



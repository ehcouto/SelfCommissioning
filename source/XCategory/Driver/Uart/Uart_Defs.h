/**
 *  @file
 *  @brief      Contains controller specific definition for Freescale Kinetis MKE02x
 *
 *
 *  $Header: Uart_Defs.h 1.1 2015/06/10 13:27:53EDT Natak Rahul (NATAKRB) Exp  $
 *
 *   @copyright  Copyright 2007-2012-$Date: 2015/06/10 13:27:53EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef UART_DEFS_H_
#define UART_DEFS_H_
#include "uc.h"

//=====================================================================================================================
//-------------------------------------- ISR Prototypes----------------------------------------------------------------
//=====================================================================================================================
//=====================================================================================================================
void Uart__Interrupt0(void);
void Uart__Interrupt1(void);
void Uart__Interrupt2(void);

#endif // UART_DEFS_H_

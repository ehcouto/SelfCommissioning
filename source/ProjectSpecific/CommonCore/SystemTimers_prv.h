/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: SystemTimers_prv.h 1.1 2015/06/11 16:54:31EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/06/11 16:54:31EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SYSTEMTIMERS_PRV_H_
#define SYSTEMTIMERS_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#define SYSTEMTIMER_NUM_INSTANCES     10

/**
 *  Macro is called the by the SystemTimers core code when the API function SystemTimers__GetHandle() is called and the
 *  return value is SYSTEMTIMERS_FAIL, which indicates a timer handle could not be allocated.
 *
 *  Macro facilitates project specific handling of running out of timers.
 */
#define CANNOT_ALLOCATE_SYSTEM_TIMER_HANDLE()

#endif // SYSTEMTIMERS_PRV_H_

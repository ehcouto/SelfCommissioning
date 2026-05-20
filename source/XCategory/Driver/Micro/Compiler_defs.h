/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This file contains the compiler related macros like endian, tiny, near etc.
 *
 *  $Header:$
 *
 *  @copyright  Copyright 2012-$Date: 2015/08/11 08:19:31EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef COMPILER_DEFS_H_
#define COMPILER_DEFS_H_
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Cortexm4_Macro.h"
#include <intrinsics.h>
#include "Micro_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define LITTLE_ENDIAN

#ifdef BIG_ENDIAN
    #undef BIG_ENDIAN
#endif





//! Define a macro for zero-page memory
#define TINY

//! Define a macro for non-zero-page memory
#define NEAR

//! Define a macro for far address memory
#define FAR

#define INTERRUPT_DIRECTIVE

// ============================================================
// --------------------- MACRO OPERATIONS ---------------------
// ------------------------------------------------------------
#ifdef __IAR_SYSTEMS_ICC__

    /// define a macro for packed data
    #define PACKED  __packed

    // ============================================================
    // --------------------- MACRO OPERATIONS ---------------------
    // ------------------------------------------------------------
    //! Macro that enables interrupts by using the CLI assembly instruction
    #define ENABLE_INTERRUPTS()     __enable_interrupt()

    //! Macro that disables interrupts by using the SEI assembly instruction
    #define DISABLE_INTERRUPTS()    __disable_interrupt()

    //! Macro to reset the watchdog timer.
    #define SERVICE_WATCHDOG()       { __istate_t local_interrupt_context; \
											local_interrupt_context = __get_interrupt_state(); \
											__disable_interrupt();\
											WDOG.REFRESH = 0xA602; WDOG.REFRESH = 0xB480;\
											__set_interrupt_state(local_interrupt_context);}

    #define LOW_POWER_MODE()        //{MRCC.PWRCTRL.bit.lp_done = 0; MRCC.PWRCTRL.bit.lp = 1; MRCC.PWRCTRL.bit.lp = 1;MRCC.PWRCTRL.bit.lp = 0;MRCC.PWRCTRL.bit.lp = 1;}
                                                                            // See section 2.7.4 STR750 Reference Manual


    // ============================================================
    // --------------------- MACRO OPERATIONS ---------------------
    // ------------------------------------------------------------

    //! Macro to reset the system by executing an illegal instruction.
    //! The nop that follows the illegal instruction is to prevent the optimizing
    //! compiler from accidentally doing a SYSTEM_RESET() because it gets grouped
    //! with other code.
    #define SYSTEM_RESET()          {SCB_AIRCR = AIRC_VECTKEY_MASK | (uint32)0x04;}

    #define RESET_STACK_POINTER()   __MSR_MSP((uint32)__sfe( "CSTACK" ))

    //! Macro to spend CPU cycles by using the NOP assembly instruction

    #define NOP()                   asm(" nop ")

    //! Macro to allocate a variable into NO INIT section were variables are not initialzed during during micro initialization
    #define NO_INIT   __no_init
#else
    // define a macro for packed data
    #define PACKED
    #define ENABLE_INTERRUPTS()
    #define DISABLE_INTERRUPTS()
    #define SERVICE_WATCHDOG()
    #define LOW_POWER_MODE()
    #define SYSTEM_RESET()
    #define RESET_STACK_POINTER()
    #define NOP()
    #define NO_INIT
    #define __no_init
    #define size_t uint8


    #define __UNSIGNED_CHAR_MAX__   (0xFF)

    #define __SIGNED_CHAR_MAX__    (127)
    #define __SIGNED_CHAR_MIN__    (-128)

    #define __UNSIGNED_SHORT_MAX__ (0xFFFF)

    #define __SIGNED_SHORT_MAX__    (32767)
    #define __SIGNED_SHORT_MIN__    (-32768)

    #define __UNSIGNED_LONG_MAX__   (0xFFFFFFFF)
	#define __SIGNED_LONG_MIN__		(-2147483647)
	#define __SIGNED_LONG_MAX__		( 2147483647)


#endif

#endif // COMPILER_DEFS_H_

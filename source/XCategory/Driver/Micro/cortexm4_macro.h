/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      consists of API's to access the Cortex-M4 core.
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2012-$$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
#ifndef __CORTEXM4_MACRO_H
    #define __CORTEXM4_MACRO_H
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void __WFI(void);
void __WFE(void);
void __SEV(void);
void __ISB(void);
void __DSB(void);
void __DMB(void);
void __SVC(void);
void __MSR_CONTROL(uint32 Control);
uint32 __MRS_PSP(void);
void __MSR_PSP(uint32 TopOfProcessStack);
uint32 __MRS_MSP(void);
void __RESETPRIMASK(void);
uint16 __REV_HalfWord(uint16 Data);
uint32 __REV_Word(uint32 Data);
uint8 __StartupRegisterTest(void);
uint8 __RunRegisterTest(void);

#endif

/**
 *  @file       MclFvt.h
 *  @brief      Macroblock Functional Verification Test - FVT
 *  @details    Define the basic commands to interface the application with the MclFvt
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2017.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef MCLFVT_H_
    #define MCLFVT_H_
#include "C_Extensions.h"
#include "MclCrossTypes.h"
#include "Mci.h"
#include "Mcl.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void MclFvt__Entry(void);
void MclFvt__Exit(void);
void MclFvt__PwmHandler(void);
uint8 MclFvt__GuardCondition(void);
uint8 MclFvt__GetStatus(MCI_FVT_STATUS_TYPE this_status);
void MclFvt__GetResult(MCI_FVT_TEST_RESULT_TYPE *fvt_result);


#endif /* MCLFVT_H_ */

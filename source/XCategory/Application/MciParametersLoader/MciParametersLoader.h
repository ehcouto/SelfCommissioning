/**
 *  @file       MciParametersLoader.h
 *
 *  @brief      Header file for Mci Parameters Loader module
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCIPARAMETERSLOADER_H
    #define MCIPARAMETERSLOADER_H


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Mcl.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MciParametersLoader__Initialize(void);
void MciParametersLoader__25msHandler(void);
addr_t* MciParametersLoader__GetClassADispl(uint8 displacement);
addr_t MciParametersLoader__GetClassBDispl(uint8 displacement);


#endif /* MCIPARAMETERSLOADER_H */




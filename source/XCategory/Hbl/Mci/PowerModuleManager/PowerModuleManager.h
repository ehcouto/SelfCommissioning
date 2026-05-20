/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _POWERMODULEMANAGER_H_
#define _POWERMODULEMANAGER_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    uint32 Inv_Temp_Thr_Low;
    uint32 Inv_Temp_Thr_High;
    uint32 I_Rms_Thr_Low;      // IQ16
    uint32 I_Rms_Thr_High;     // IQ16
    uint32 Modulation_Window;
} POWERMODULEMANAGER_PARAMS_DISPL_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void PowerModuleManager__Initialize(void);
void PowerModuleManager__25msHandler(void);
BOOL_TYPE PowerModuleManager__SetParams(void* powermodulemgr_params_address);
BOOL_TYPE PowerModuleManager__GetParameterLoaded(void);

#endif // _POWERMODULEMANAGER_H_

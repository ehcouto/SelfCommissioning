/**
 *  @file       InverterTemp.h
 *
 *  @brief      Header of Inverter temperature module.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: InverterTemp.h 1.5 2016/01/13 20:08:13CET Marcelo Silva (SILVAMC) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2016/01/13 20:08:13CET $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef INVERTERTEMP_H_
#define INVERTERTEMP_H_

#include "MclBasicTypes.h"
#include "InverterTemp_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
typedef struct
{
    sint32  Default_Temp_Adc;
    sint32  Sizeof_Inverter_Temp_Lut;
    sint32  Step_Inv_Inverter_Temp;
    MATHCALC_LUT_TYPE Inverter_Temp_Lut[20];
} INVERTER_TEMP_PARAMS_DISPL_TYPE;
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void InverterTemp__Initialize(void);
void InverterTemp__Handler(void);
void InverterTemp__PwmHandler(void);
sint32 InverterTemp__GetInverterTemp_s16(void);
sint16 InverterTemp__GetInverterRawTemp(void);
BOOL_TYPE InverterTemp__AboveThreshold_s16(sint32 temp_shift_16);
#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == DISABLED)
void InverterTemp__SetLUT(INVERTERTEMP_LUT_INDEX_TYPE lut_index);
#endif
#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
BOOL_TYPE InverterTemp__SetParams(void*  invtemp_params_address);
BOOL_TYPE InverterTemp__GetParameterLoaded(void);
#endif
#endif // INVERTERTEMP_H_



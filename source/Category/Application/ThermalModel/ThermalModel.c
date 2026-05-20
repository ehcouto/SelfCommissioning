/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: ThermalModel.c 1.13.1.3.1.1.1.3 2016/05/25 07:46:59EDT Eduardo Couto (HENRIE2) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2016/05/25 07:46:59EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "ThermalModel.h"
#include "ThermalModelCoreDD.h"
#include "ThermalModelCoreBPM.h"
#include "Mci.h"
#include <math.h>

#ifndef OTE_SET_PARAMETERS_INTERNAL
#include "string.h"
#include "SettingFile.h"
#include "Mode.h"

#ifndef PLATFORM_USED
    #define NUCLEUS_BASED                   0
    #define PLATFORM_2_5_BASED              1

    #define PLATFORM_USED                  NUCLEUS_BASED
#endif


#if (PLATFORM_USED == PLATFORM_2_5_BASED)
#include "Params.h"
#endif

#endif
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint16 Thermal_Model_Start = 0;
static uint16 Set_Temperature_From_Outside = 0;

#define THERMAL_MODEL_AUTO_RESET
#ifdef THERMAL_MODEL_AUTO_RESET
    #define THERMAL_MODEL_COUNTER_MAX (60*60/0.025)//144000
#endif

#ifdef THERMAL_MODEL_AUTO_RESET
static unsigned long Thermal_Model_Reset_Counter;
#endif

float TemperatureFromRes;
float DriftAdjust;
float ResetModel;

static BOOL_TYPE ParamsReady;


OTE_MOTOR_ENUM_TYPE Motor_Type;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ThermalModel and its variables and in case of failure this function is used
 *              to restart the module.
 *
 */
void ThermalModel__Initialize(void)
{
    Thermal_Model_Start = 0;
#ifdef THERMAL_MODEL_AUTO_RESET
    Thermal_Model_Reset_Counter = 0;
#endif
    TemperatureFromRes = 25.0f;
    DriftAdjust = 0.0f;
    ResetModel = 0.0f;

    ParamsReady = FALSE;
    Motor_Type = NOT_DEFINED;
#ifndef OTE_SET_PARAMETERS_INTERNAL
    if (Mode__IsProgrammingEngaged() == FALSE) //Mci is Initialized and parameters not loaded
    {

        SETTINGFILE_LOADER_TYPE Set_OTE_Prm_Ptr;
        if(SettingFile__IsValid() == TRUE)
        {
            if(SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, SF_DISPL_MOTOR_OTE, &Set_OTE_Prm_Ptr) == PASS)
            {
                ThermalModelCoreBPM_Params = (OTE2_PARAMS_BPM_TYPE *)(void*)(Set_OTE_Prm_Ptr.Data);
                ParamsReady = TRUE;
                Motor_Type = sBPM;
                ThermalModelCoreBPM_initialize();
            }
            else if(SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, SF_DISPL_OTE_DD, &Set_OTE_Prm_Ptr) == PASS)
            {
                ThermalModelCoreDD_Params = (OTE2_PARAMS_DD_TYPE *)(void*)(Set_OTE_Prm_Ptr.Data);
                ParamsReady = TRUE;
                Motor_Type = DD;
                ThermalModelCoreDD_initialize();
            }
        }
    }
#else

    #if (THERMALMODEL_SF_DISPL == SF_DISPL_MOTOR_OTE)
        Motor_Type = sBPM
    #elif (THERMALMODEL_SF_DISPL == SF_DISPL_OTE_DD)
        Motor_Type = DD
    #endif

    if(Motor_Type == sBPM)
    {
        ThermalModelCoreBPM_Params = (OTE2_PARAMS_BPM_TYPE *) (ThermalModelCoreBPM_ConstP.MapleSimParameters_Value);
        ParamsReady = TRUE;
        ThermalModelCoreBPM_initialize();
    }
    else if(Motor_Type == DD)
    {
        ThermalModelCoreDD_Params = (OTE2_PARAMS_DD_TYPE *) (ThermalModelCoreDD_ConstP.MapleSimParameters_Value);
        ParamsReady = TRUE;
        ThermalModelCoreDD_initialize();
    }
#endif



}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief This function is used to calculate the thermal model itself.
 */
void ThermalModel__Handler25ms(void)
{
    sint32 value;

    if(ParamsReady == TRUE)
    {
		if(Set_Temperature_From_Outside == 1)
		{
			if(Thermal_Model_Start == 0)
			{
				ResetModel = 1.0f;
				Thermal_Model_Start = 1;
			}
			else
			{
				DriftAdjust = 1.0f;
			}
#ifdef THERMAL_MODEL_AUTO_RESET
			Thermal_Model_Reset_Counter = 0;
#endif
			Set_Temperature_From_Outside = 0;
		}

		if(Motor_Type == sBPM)
		{
            ThermalModelCoreBPM_U.Current = (1.0f/(65536.0f)) * Mci__GetAnalog(MOTOR0, MCI_AI_RMS_MOTOR_CURRENT_S16);
            ThermalModelCoreBPM_U.Speed = (1.0f/(65536.0f)) * Mci__GetAnalog(MOTOR0, MCI_AI_MEAN_SPEED_S16);
            ThermalModelCoreBPM_U.Temperature = TemperatureFromRes;
            ThermalModelCoreBPM_U.DriftCorrection = DriftAdjust;
            ThermalModelCoreBPM_U.Reset = ResetModel;
            ThermalModelCoreBPM_step();
		}
		else if(Motor_Type == DD)
		{
            ThermalModelCoreDD_U.Current = (1.0f/(65536.0f)) * Mci__GetAnalog(MOTOR0, MCI_AI_RMS_MOTOR_CURRENT_S16);
            ThermalModelCoreDD_U.Speed = (1.0f/(65536.0f)) * Mci__GetAnalog(MOTOR0, MCI_AI_MEAN_SPEED_S16);
            ThermalModelCoreDD_U.Temperature = TemperatureFromRes;
            ThermalModelCoreDD_U.DriftCorrection = DriftAdjust;
            ThermalModelCoreDD_U.Reset = ResetModel;
            ThermalModelCoreDD_step();
		}

		DriftAdjust = 0.0f;
		ResetModel = 0.0f;

//		value = (sint32)(ThermalModelCore_Y.WindingTemperature * 65536.0f);
//		Mci__SetAnalog(MOTOR0, MCI_AO_MOTOR_TEMPERATURE_S16, value); //OTE new temperature

#ifdef THERMAL_MODEL_AUTO_RESET
		Thermal_Model_Reset_Counter++;
		if (Thermal_Model_Reset_Counter > THERMAL_MODEL_COUNTER_MAX)
		{
			//ThermalModel__Initialize();
			Thermal_Model_Start = 0;
			Thermal_Model_Reset_Counter = 0;
		}
#endif
    }else
    {

		if (Mode__IsProgrammingEngaged() == FALSE) //Mci is Initialized and parameters not loaded
		{
		    if(SettingFile__IsValid() == TRUE)
            {
		        SETTINGFILE_LOADER_TYPE Set_OTE_Prm_Ptr;
		        if(SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, SF_DISPL_MOTOR_OTE, &Set_OTE_Prm_Ptr) == PASS)
                {
                    ThermalModelCoreBPM_Params = (OTE2_PARAMS_BPM_TYPE *)(void*)(Set_OTE_Prm_Ptr.Data);
                    ParamsReady = TRUE;
                    Motor_Type = sBPM;
                    ThermalModelCoreBPM_initialize();
                }
                else if(SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, SF_DISPL_OTE_DD, &Set_OTE_Prm_Ptr) == PASS)
                {
                    ThermalModelCoreDD_Params = (OTE2_PARAMS_DD_TYPE *)(void*)(Set_OTE_Prm_Ptr.Data);
                    ParamsReady = TRUE;
                    Motor_Type = DD;
                    ThermalModelCoreDD_initialize();
                }
            }
		}

    }
}


void ThermalModel__SetStatorTemperature(float input_temperature)
{
    TemperatureFromRes = input_temperature;

    Set_Temperature_From_Outside = 1;

}

float ThermalModel__GetStatorTemperature(void)
{
    if(Motor_Type == sBPM)
    {
        return (ThermalModelCoreBPM_Y.WindingTemperature);
    }
    else if (Motor_Type == DD)
    {
        return (ThermalModelCoreDD_Y.WindingTemperature);
    }
    else
    {
        return 0.0f;
    }
}


OTE_MOTOR_ENUM_TYPE ThermalModel__GetMotorType(void)
{
   return Motor_Type;
}

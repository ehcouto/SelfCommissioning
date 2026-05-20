/**
 *  @file       
 *
 *  @brief      MCI Power Module Manager
 *
 *  @details
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "PowerModuleManager.h"
#include "PowerModuleManager_prv.h"
#include "Mci.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static BOOL_TYPE PowerModuleMgr_Temp_Params_Loaded;
static POWERMODULEMANAGER_PARAMS_DISPL_TYPE* PowerModuleMgr_Params_Displ;
static uint32 Pm_Mgr_Modulation_Time;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module PowerModuleManager and its variables
 *
 */
void PowerModuleManager__Initialize(void)
{
    PowerModuleMgr_Temp_Params_Loaded = FALSE;
    PowerModuleMgr_Params_Displ = NULL;
    Pm_Mgr_Modulation_Time = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void PowerModuleManager__25msHandler(void)
{
    uint32 temperature;
    uint32 current_rms;
    BOOL_TYPE low_power_mode;


    if (PowerModuleMgr_Temp_Params_Loaded == TRUE && POWERMODULEMGR__IS_MOTOR_RUNNING())
    {
        // sample the temperature
        temperature = POWERMODULEMGR__GET_INVERTER_TEMP();
        // sample the current
        current_rms = POWERMODULEMGR__GET_I_RMS_S16();

        low_power_mode = POWERMODULEMGR__IS_LOW_POWER();
        if (low_power_mode)
        {
            // Low power request - turn off
            if (current_rms < POWERMODULEMGR_I_RMS_THR_LOW_S16)
            {
                if (Pm_Mgr_Modulation_Time <= 0)
                {
                    if (temperature < POWERMODULEMGR_INV_TEMP_THR_LOW)
                    {
                        // switch off request
                        low_power_mode = FALSE;
                    }
                }
                else
                {
                    Pm_Mgr_Modulation_Time--;
                }
            }
            else
            {
                Pm_Mgr_Modulation_Time = POWERMODULEMGR_MODULATION_WINDOW;
            }
        }
        else
        {
            if (temperature > POWERMODULEMGR_INV_TEMP_THR_HIGH)
            {
                // switch on request
                low_power_mode = TRUE;
            }
            else
            {
                if (current_rms > POWERMODULEMGR_I_RMS_THR_HIGH_S16)
                {
                    if (Pm_Mgr_Modulation_Time >= POWERMODULEMGR_MODULATION_WINDOW)
                    {
                        // switch on request
                        low_power_mode = TRUE;
                        Pm_Mgr_Modulation_Time = POWERMODULEMGR_MODULATION_WINDOW;
                    }
                    else
                    {
                        Pm_Mgr_Modulation_Time++;
                    }
                }
                else
                {
                    Pm_Mgr_Modulation_Time = 0;
                }
            }
        }

        // Enable only after steady state
        if (low_power_mode == TRUE && POWERMODULEMGR__IS_STABLE())
        {
            POWERMODULEMGR__ENTER_LOW_POWER();
        }
        else
        {
            POWERMODULEMGR__EXIT_LOW_POWER();
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Write the address of PowerModuleManager parameters.
 *  @details
 *
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
BOOL_TYPE PowerModuleManager__SetParams(void*  powermodulemgr_params_address)
{
    BOOL_TYPE ret_val = FALSE;

    if (powermodulemgr_params_address != NULL)
    {
        PowerModuleMgr_Params_Displ = (POWERMODULEMANAGER_PARAMS_DISPL_TYPE*) powermodulemgr_params_address;
        PowerModuleMgr_Temp_Params_Loaded = TRUE;
        ret_val = TRUE;
    }

    return ret_val;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get parameters loaded status.
 *  @details
 *
 * @return  TRUE - parameters are loaded
 *          FALSE - parameters not loaded
 */
BOOL_TYPE PowerModuleManager__GetParameterLoaded(void)
{
    return PowerModuleMgr_Temp_Params_Loaded;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



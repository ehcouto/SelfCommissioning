 /**
 *  @file       
 *
 *  @brief      API007App module implementation
 *
 *  @details    This module takes care of reading and writing mechanism to the memory for bulk data storage.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *
 *  @copyright  Copyright 2013-$Date: 2013/09/13 10:23:07EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MotorSelector.h"
#include "Mcl_Additional_sBPM_prm.h"
#include "Mcl_Additional_DD_prm.h"
#include "Mcl_Complete_sBPM_prm.h"
#include "Mcl_Complete_DD_prm.h"

#include "Mci.h"
#include "SettingFile.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
unsigned long int* Mcl_Params_SF;
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
//=====================================================================================================================

//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

uint8 Motor_Selector_Params_Raady = FALSE;

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module and its variables
 *
 */
void MotorSelector__Initialize(void)
{
    SETTINGFILE_LOADER_TYPE temp_ptr;
#ifndef    MCI_INTERNAL_PARAMS

    Motor_Selector_Params_Raady = FALSE;

    if((SettingFile__IsValid() == TRUE) &&
    (SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, SF_DISPL_MOTOR_OTE, &temp_ptr) == PASS))
    {
        Mcl_Params_SF = Mcl_Additional_Params_sBPM_SF;

        Motor_Selector_Params_Raady = TRUE;
    }
    else if((SettingFile__IsValid() == TRUE) &&
    (SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, SF_DISPL_OTE_DD, &temp_ptr) == PASS))
    {
        Mcl_Params_SF = Mcl_Additional_Params_DD_SF;

        Motor_Selector_Params_Raady = TRUE;
    }
#else
    #if 1
    Mcl_Params_SF = Mcl_Complete_Params_sBPM_SF;
    #else
    Mcl_Params_SF = Mcl_Complete_Params_DD_SF;
    #endif

    Motor_Selector_Params_Raady = TRUE;
#endif
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief Writing and Reading handlers
 *
 */

void MotorSelector__Handler (void)
{
    SETTINGFILE_LOADER_TYPE temp_ptr;
#ifndef    MCI_INTERNAL_PARAMS

    if(Motor_Selector_Params_Raady == FALSE)
    {
        if((SettingFile__IsValid() == TRUE) &&
        (SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, SF_DISPL_MOTOR_OTE, &temp_ptr) == PASS))
        {
            Mcl_Params_SF = Mcl_Additional_Params_sBPM_SF;
            Motor_Selector_Params_Raady = TRUE;
        }
        else if((SettingFile__IsValid() == TRUE) &&
        (SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, SF_DISPL_OTE_DD, &temp_ptr) == PASS))
        {
            Mcl_Params_SF = Mcl_Additional_Params_DD_SF;
            Motor_Selector_Params_Raady = TRUE;
        }
    }
#endif
}

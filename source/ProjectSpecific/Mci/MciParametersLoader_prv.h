/**
 *  @file       MciParametersLoader_prv.h
 *
 *  @brief      Load Mci parameters
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCIPARAMETERSLOADER_PRV_H
    #define MCIPARAMETERSLOADER_PRV_H


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Mcl.h"
#include "SRMotorSafetyMgr.h"
#include "mci_prm.h"

// Motor Selection for INTERNAL MCL parameters
// list of available motors
#define HEFEI_H27        0
#define NIDEC_H38        1
#define HEFEI_H27_HIGH   2
#define HEFEI_H22        3
#define HEFEI_H17        4
#define HEFEI_DD_TM2     5
#define HEFEI_DD_TM9     6
#define JANUS_MINI       7
#define sBPM_DD_UNIFIED  8

// selected motor
#define MCL_MOTOR   sBPM_DD_UNIFIED

//Some Mci Class-A displacements are not from setting file
#if (MCL_MOTOR == HEFEI_H27)
    #if (WINDY_STRIP_BOARD == 1)
        #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Hefei_H27.h"
        #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm.h"
    #else
        #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Hefei_H27_Windy_Int.h"
        #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm.h"
    #endif
#elif (MCL_MOTOR == NIDEC_H38)
    #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Nidec_H38.h"
    #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm.h"
#elif (MCL_MOTOR == HEFEI_H27_HIGH)
    #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Hefei_H27_High.h"
    #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm.h"
#elif (MCL_MOTOR == HEFEI_H22)
    #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Hefei_H22.h"
    #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm.h"
#elif (MCL_MOTOR == HEFEI_H17)
    #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Hefei_H17.h"
    #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm.h"
#elif (MCL_MOTOR == HEFEI_DD_TM2)
    #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Hefei_DD_TM2.h"
    #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm_Hefei_DD_TM2.h"
#elif (MCL_MOTOR == HEFEI_DD_TM9)
    #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Hefei_DD_TM9.h"
    #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm_Hefei_DD_TM2.h"
#elif (MCL_MOTOR == JANUS_MINI)
    #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "Mcl_prm_Hefei_DD_JanusMini_v_0_2.h"
    #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm_Janus_Mini.h"
#elif (MCL_MOTOR == sBPM_DD_UNIFIED)
    #define INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE "MotorSelector.h"
    #define INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE "MotorSr_prm.h"
#endif

///////////////////////////// MCI - CLASS A Section  ////////////////////////////////////////
// DEFINITION OF DISPLACEMENT AVAILABLE ON THE MCI CLASS A POINTER //
#include "SettingFile.h"

#define DISPL_DTC_CONTROL               SF_DISPL_MOTOR_PARAMS_DTC   //offset 0
#define DISPL_MOTOR_LUT                 SF_DISPL_MOTOR_PARAMS_LUT   //offset 1
#define DISPL_SPEED_CTRL                SF_DISPL_MOTOR_SPEED_CTRL   //offset 2
#define DISPL_MTPA_LUT                  SF_DISPL_MTPA_LUT           //offset 3
#define DISPL_MTPV_LUT                  SF_DISPL_MTPV_LUT           //offset 4
#define DISPL_FLUX_LUT                  SF_DISPL_FLUX_LUT           //offset 5
#define DISPL_ADDITIONAL_DTC            SF_DISPL_ADDITIONAL_DTC     //offset 6
#define DISPL_MCI_PRM                   SF_DISPL_MCI_PRM            //offset 7

// For each used displacement declare its size here
#define DISPL_DTC_CONTROL_SIZE               sizeof(MCL_PARAMS_DISPL1_TYPE)
#define DISPL_MOTOR_LUT_SIZE                 sizeof(MCL_PARAMS_DISPL3_TYPE)
#define DISPL_SPEED_CTRL_SIZE                sizeof(MCL_PARAMS_DISPL4_TYPE)
#define DISPL_MTPA_LUT_SIZE                  sizeof(MCL_PARAMS_DISPL9_TYPE)
#define DISPL_MTPV_LUT_SIZE                  sizeof(MCL_PARAMS_DISPL10_TYPE)
#define DISPL_FLUX_LUT_SIZE                  sizeof(MCL_PARAMS_DISPL11_TYPE)
#define DISPL_ADDITIONAL_DTC_SIZE            sizeof(MCL_PARAMS_DISPL13_TYPE)
#define DISPL_MCI_PRM_SIZE                   sizeof(MCI_PARAMS_TYPE)

// Define here the the memory location of the displacements.
// For each memory location define comma separated displacements in the right order.
// REMARK: Only define the used memory locations. Leave the unused locations undefined
#if (!defined MCI_INTERNAL_PARAMS)
#define SF_RAM_DISPLS           {DISPL_DTC_CONTROL, DISPL_MOTOR_LUT, DISPL_SPEED_CTRL, DISPL_MTPA_LUT, DISPL_MTPV_LUT, DISPL_FLUX_LUT}
#define INTERNAL_DISPLS         {DISPL_ADDITIONAL_DTC, DISPL_MCI_PRM}
#else
#define INTERNAL_DISPLS         {DISPL_DTC_CONTROL, DISPL_MOTOR_LUT, DISPL_SPEED_CTRL, DISPL_MTPA_LUT, DISPL_MTPV_LUT,DISPL_FLUX_LUT,DISPL_ADDITIONAL_DTC, DISPL_MCI_PRM}
#endif
//#define SF_FLASH_DISPLS       {}
//#define INTERNAL_DISPLS

// For internal displacements only, define the sizes, in the same order internal
// displacements definition. Values must be comma separated.
#if (!defined MCI_INTERNAL_PARAMS)
#define INTERNAL_TOT_SIZES       DISPL_ADDITIONAL_DTC_SIZE+ DISPL_MCI_PRM_SIZE
#define INTERNAL_DISPL_SIZES    {DISPL_ADDITIONAL_DTC_SIZE, DISPL_MCI_PRM_SIZE}
#else
#define INTERNAL_TOT_SIZES       DISPL_DTC_CONTROL_SIZE+ DISPL_MOTOR_LUT_SIZE+ DISPL_SPEED_CTRL_SIZE+ DISPL_MTPA_LUT_SIZE+ DISPL_MTPV_LUT_SIZE+DISPL_FLUX_LUT_SIZE+DISPL_ADDITIONAL_DTC_SIZE+ DISPL_MCI_PRM_SIZE
#define INTERNAL_DISPL_SIZES    {DISPL_DTC_CONTROL_SIZE, DISPL_MOTOR_LUT_SIZE, DISPL_SPEED_CTRL_SIZE, DISPL_MTPA_LUT_SIZE, DISPL_MTPV_LUT_SIZE,DISPL_FLUX_LUT_SIZE,DISPL_ADDITIONAL_DTC_SIZE, DISPL_MCI_PRM_SIZE}
#endif
// sum all displacement sizes selected for RAM buffering
#define  MCI_CLASS_A_PARAMETERS_SIZE_RAM     DISPL_DTC_CONTROL_SIZE + DISPL_MOTOR_LUT_SIZE + DISPL_SPEED_CTRL_SIZE + DISPL_MTPA_LUT_SIZE + DISPL_MTPV_LUT_SIZE + DISPL_FLUX_LUT_SIZE

///////////////////////////// MCI - CLASS B Section  ////////////////////////////////////////
// DISPLACEMENT AVAILABLE ON THE MCI CLASS B POINTER (SR) //
#define DISPL_SR_MOTOR           1

// For each used displacement declare its size here
#define DISPL_SR_MOTOR_SIZE            SAFETY_MOTOR_PARAMETERS_NUM

// Define here the the memory location of the displacements.
// For each memory location define comma separated displacements in the right order.
// REMARK: Only define the used memory locations. Leave the unused locations undefined
#if (!defined MCI_INTERNAL_PARAMS)
    #define SF_CLASSB_RAM_DISPLS          {DISPL_SR_MOTOR}
#else
    #define INTERNAL_CLASSB_DISPLS        {DISPL_SR_MOTOR}
#endif
//#define SF_CLASSB_FLASH_DISPLS        {}

// For internal displacements only, define the sizes, in the same order internal
// displacements definition. Values must be comma separated.
#define INTERNAL_CLASSB_DISPL_SIZES    {DISPL_SR_MOTOR_SIZE}

// sum all displacement sizes selected for RAM buffering
#define  MCI_CLASS_B_PARAMETERS_SIZE_RAM     DISPL_SR_MOTOR_SIZE

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


#endif /* MCIPARAMETERSLOADER_PRV_H */


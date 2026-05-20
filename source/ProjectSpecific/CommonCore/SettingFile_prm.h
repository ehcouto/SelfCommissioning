/**
 *  @file
 *
 *  @brief      Public parameters for the SettingFile module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/06/11 16:54:27EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SETTINGFILE_PRM_H_
    #define SETTINGFILE_PRM_H_

#include "C_Extensions.h"

/*
// -Memory Regions- MKV on .icf file.
define symbol __SettingFile_X_Size__ = 0x4800;
define symbol INTERRUPT_TABLE_SIZE	 = 0x500;

define symbol __FLASH_TOTAL_SIZE 	   	   = 0x20000; 		// 128k
define symbol __ICFEDIT_region_RAM_start__ = 0x1fffe000;
define symbol __ICFEDIT_region_RAM_end__   = 0x1fffffff;
define symbol __ICFEDIT_region_ROM_start__ = __ICFEDIT_intvec_start__ + INTERRUPT_TABLE_SIZE;

define symbol __ICFEDIT_region_ROM_end__ = (__ICFEDIT_region_ROM_start__ + __FLASH_TOTAL_SIZE - INTERRUPT_TABLE_SIZE - __SettingFile_X_Size__ - 1);
define symbol __SettingFile_X_Start__    = (__ICFEDIT_region_ROM_end__ + 1);
define symbol MICRO_FLASH_END   	 	 = __ICFEDIT_region_ROM_end__ - 4;
*/

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! Standard displacements for the SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA setting file pointer.
typedef enum SETTINGFILE_PRODUCT_SPECIFIC_DISPLACEMENT_ENUM
{
    // Displacement numbers for SF_PTR_MCU_EXTRACTION_COMMON_PARAMS
    SF_DISPL_DISTRIBUTION_RAMP_PARAMS           = 0x00,
    SF_DISPL_P2P_TORQUE_CALC_PARAMS             = 0x01,
    SF_DISPL_SATEL_VELOCITY_CALC_PARAMS         = 0x02,
    SF_DISPL_HIGH_PASS_FILTER_1_COEFFS          = 0x03,
    SF_DISPL_HIGH_PASS_FILTER_2_COEFFS          = 0x04,
    SF_DISPL_OPTIMUM_RAMP_PARAMS                = 0x05,
    SF_DISPL_LOW_SPEED_PAR_EST_CONFIG           = 0x06,
    SF_DISPL_MDDP_PARAMS                        = 0x07,
    SF_DISPL_LOAD_MASS_PE_CALC_PARAMS           = 0x08,
    SF_DISPL_UNBALANCE_MEAS_PE_CALC_PARAMS      = 0x09,
    SF_DISPL_HIGH_SPD_CONFIG_PARAMS             = 0x0A,
    SF_DISPL_BASELINE_UPD_CFG_PARAMS            = 0x0B,
    SF_DISPL_CEM_CONFIG_PARAMS                  = 0x0C,
    SF_DISPL_UNBALANCE_PARAMS                   = 0x0D,
    SF_DISPL_VIBRATION_MAPPING_PARAMS           = 0x0E,
    SF_DISPL_FLOOR_DISPLACEMENT_CALC_PARAMS     = 0x0F,
    SF_DISPL_CABINET_HIT_PARAMS                 = 0x10,
    SF_DISPL_SORTED_SPIN_TGT_VELOCITIES         = 0x11,
	SF_DISPL_SORTED_UNBALANCE_CURVES			= 0x12,
	SF_DISPL_CLEAN_WASHER_LOAD_DET_PARAMS		= 0x13,
	SF_DISPL_DRY_LOAD_MASS_PE_CALC_PARAMS       = 0x14,

    // Displacement numbers for SF_PTR_MCU_MOTOR_APPLICATION_GENERIC_DATA
    SF_MOTOR_CONTROL_APPLICATION_DATA            = 0x00,
    SF_DISPL_REMOTE_FUNCTION_CHILDSAFETY_PARAMS  = 0x01,
    SF_DISPL_ACCELEROMETER_DATA                  = 0x02,
    SF_DISPL_ACCELEROMETER_FILTER_CONFIG         = 0x03,

    // Displacement numbers for SF_PTR_MCU_MCA
    SF_DISPL_MOTOR_MCA_VERSION                   = 0x00,
    SF_DISPL_MOTOR_MCI_SET_WM                    = 0x01,
    SF_DISPL_MOTOR_MCI_SENSORS_WM                = 0x02,
    SF_DISPL_MOTOR_MCI_SET_WM_FLOAT              = 0x03,

    // Additional setting file for DTC sBPM control
    SF_DISPL_DEADTIME_TABLE             = 0x08,
    SF_DISPL_MTPA_LUT                   = 0x09,
    SF_DISPL_MTPV_LUT                   = 0x0A,
    SF_DISPL_FLUX_LUT                   = 0x0B,
    SF_DISPL_OTE_DD                     = 0x0C,
    SF_DISPL_ADDITIONAL_DTC             = 0x0D,
    SF_DISPL_MCI_PRM                    = 0x0E,

} SETTINGFILE_PRODUCT_SPECIFIC_DISPLACEMENT_TYPE;

// -- Public List Declarations --

//! Setting file pointers to the category-specific process data.
#define SETTINGFILE_APPLICATION_SPECIFIC_SF_PTR \
        SF_PTR_MCU_EXTRACTION_COMMON_PARAMS         = 0x0204,   \
        SF_PTR_MCU_EXTRACTION_DYNAMIC_TABLES        = 0x0205,  \
        SF_PTR_MCU_EXTRACTION_UNBALANCE_DATA        = 0x0206,  \
        SF_PTR_MCU_MOTOR_APPLICATION_GENERIC_DATA   = 0x0207,  \
        SF_PTR_MCU_DYNAMIC_PROFILES_DATA            = 0x0208,  \
        SF_PTR_MCU_DYNAMIC_SPIN_PROFILES_DATA       = 0x0209,  \
        SF_PTR_MCU_SPIN_SET_DATA                    = 0x020A,  \
        SF_PTR_MCU_REBALANCE_DATA                   = 0x020B,  \
        SF_PTR_MCU_HIGH_SPEED_DATA                  = 0x020C,  \
        SF_PTR_MCU_BASELINE_UPDATE_DATA             = 0x020D,  \
        SF_PTR_MCU_LOAD_MASS_LIMIT_CHECK_DATA       = 0x020E,  \
        SF_PTR_MCU_VIBRATION_MAPPING_CONFIG_DATA    = 0x020F,  \
        SF_PTR_MCU_MCA                              = 0x0212,  \




//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SETTINGFILE_PRM_H_

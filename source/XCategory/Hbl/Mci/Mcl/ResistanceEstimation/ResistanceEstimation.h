/**
 *  @file       ResistanceEstimation.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2016.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef RESISTANCEESTIMATION_H_
    #define RESISTANCEESTIMATION_H_

#include "C_Extensions.h"
#include "MclCrossTypes.h"



//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


typedef struct
{
    float32 *vs_voltage_abs;
    float32 *level_injection_out;

    MCL_CS_TYPE *Ctrl_Specific;                     //!< MCL control specific variables
} RES_EST_IO_TYPE;


typedef struct
{
	sint32 timeout_threshold;

	float32 current_level_1st;
	float32 current_level_2nd;
	float32 current_level_3rd;

	sint32 time_1st_step;
	sint32 time_2nd_step;
	sint32 time_3rd_step;

    float32 delta_voltage_to_1st_step;
    float32 delta_voltage_to_2nd_step;
    float32 delta_voltage_to_3rd_step;

    sint32 oscillation_check_timer;

    float32 min_number_of_samples;

    float32 nominal_resistance;

    float32 oscillation_speed_threshold;

    uint32 timer_to_reset_resistance;

    uint32 timer_prescaler;

    float32 speed_threshold_to_estimate_resistance;

    uint32 time_estimate_resistance_after_reset;

    uint32 number_estimation_after_reset;

    uint32 time_estimate_resistance_normal;

    uint32 number_estimation_after_spin;


} RES_EST_PARAM_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


void ResistanceEstimation__Initialize(RES_EST_PARAM_TYPE *param);
void ResistanceEstimation__Start(RES_EST_PARAM_TYPE *param);
BOOL_TYPE ResistanceEstimation__Requested(void);
BOOL_TYPE ResistanceEstimation__Calculate(RES_EST_IO_TYPE *io, RES_EST_PARAM_TYPE *param);
float32 ResistanceEstimation__GetResistance(void);


void ResistanceEstimation__Handler25ms(RES_EST_PARAM_TYPE *param);
BOOL_TYPE ResistanceEstimation__IsResBeenUpdated(void);




#endif // RESISTANCEESTIMATION_H_

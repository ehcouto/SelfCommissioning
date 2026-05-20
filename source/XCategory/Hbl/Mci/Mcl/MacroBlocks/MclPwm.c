/**
 *  @file       MclPwm.c
 *  @brief      Motor Control Loop 3-phase motors: Pwm Modulation Macro Block.
 *  @details    This module implements the Pwm module.
 *  @author     alessio.beato/luigi.fagnano
 *  $Header:
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
*/
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MclPwm.h"
#include "MclConfig.h"
#include "PwmModulation.h"
#include "MclPwm_prv.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
// Inverter Compensation types
#define INVERTER_LOSSES_COMP        0               //!< complete losses compensation
#define DEADTIME_COMP               1               //!< deadtime compensation

#define DUTY_INIT                   0.5f
#define SPEED_THRESHOLD             (0.05f)         //!< [%] speed hysteresis threshold

#ifndef RPM_TO_RADS
    #define RPM_TO_RADS     0.10471975511965977461542144610932f
#endif

static float32 Speed_Threshold_High;                //!< [absolute rad/s mech] speed hysteresis high limits
static float32 Speed_Threshold_Low;                 //!< [absolute rad/s mech] speed hysteresis low limits
static float32 Speed_Threshold;                     //!< [absolute rad/s mech] speed hysteresis selector
static uint8 Inverter_Compensation_Selector;        //!< inverter compensation selector: 0 inverter loss compensation, 1 deadtime compensation
static uint8 Pwm_Sector;                            //!< Pwm sector calculation
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Resets all Mcl PWM quantities.
 *  @details    This routine resets Mcl PWM quantities, it has to be called at every time the pwm is switched off (motor stop or free down ramp).
 *
 *
 *  @param[in]     
 *  @param[out]
 *  @return        
 */
void MclPwm__ResetState(MCL_PWM_IO_F_TYPE *io, MCLPWM_JOINT_PARAMS_TYPE *params)
{
    io->Vs_Alpha_Beta->Alpha = 0;
    io->Vs_Alpha_Beta->Beta = 0;

    io->Duty->A = DUTY_INIT;
    io->Duty->B = DUTY_INIT;
    io->Duty->C = DUTY_INIT;

    Speed_Threshold_High = (params->PwmPrm->Speed_Threshold_InvComp) * (1.0f + SPEED_THRESHOLD) * RPM_TO_RADS;
    Speed_Threshold_Low  = (params->PwmPrm->Speed_Threshold_InvComp) * (1.0f - SPEED_THRESHOLD) * RPM_TO_RADS;
    Speed_Threshold = Speed_Threshold_High;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Loop PWM initialization.
 *  @details    In this routine are called all initialization functions.
 *
 *
 *  @param[in]     
 *  @param[out]     
 *  @param[in]      
 *  @return
 */
void MclPwm__Initialize(MCL_PWM_IO_F_TYPE *io, MCLPWM_JOINT_PARAMS_TYPE* params)
{
    Inverter_Compensation_Selector = INVERTER_LOSSES_COMP;

    params->PwmModulationPrm.ptr_LUT      = Dutycycle_By_Current_LUT;
    params->PwmModulationPrm.sizeof_lut   = &sizeof_inv_comp_lut;
    params->PwmModulationPrm.step_inv     = &step_inv_pwm_comp;

    #ifdef PWM_DOUBLE_TABLE_COMPENSATION
    params->PwmModulationLossOnStatePrm.ptr_LUT      = Dutycycle_By_Current_On_State_LUT;
    params->PwmModulationLossOnStatePrm.sizeof_lut   = &sizeof_inv_on_state_comp_lut;
    params->PwmModulationLossOnStatePrm.step_inv     = &step_inv_pwm_on_state_comp;
    #endif

    // reset MCL quantities
    MclPwm__ResetState(io,params);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Mcl Pwm Running Hanlder
 *  @details   Pwm:          DC voltage ripple compensation
 *                           PWM Duties generation
 *                           Deadtime compensation
 *  @param[in]     
 *  @param[out]     
 *  @param[in]      
 *  @return        
 */
void MclPwm__RunningHandler(MCL_PWM_IO_F_TYPE *io, MCLPWM_JOINT_PARAMS_TYPE *params)
{
	// space vector modulation (including ripple compensation)
	Pwm_Sector = PwmModulation__SpaceVectorModulationF(*(io->Vdc), io->Vs_Alpha_Beta, io->Duty_bc, FALSE);

	// This step is required since the voltage reconstruction uses the duties before compensation
	io->Duty->A = io->Duty_bc->A;
	io->Duty->B = io->Duty_bc->B;
	io->Duty->C = io->Duty_bc->C;

   #ifndef COMPILE_4_SIMULINK
    #if (OBS__INVERTER_DISTORSION_COMPENSATION == DISABLED)
	// inverter compensation strategy --> feedforward approach
	if(Inverter_Compensation_Selector == INVERTER_LOSSES_COMP)
	{
		PwmModulation__InverterLossCompF(io->Is_ABC, io->Duty, &params->PwmModulationPrm);

        #ifdef PWM_DOUBLE_TABLE_COMPENSATION
		    PwmModulation__InverterLossOnStateCompF(io->Is_ABC, io->Duty, &params->PwmModulationLossOnStatePrm, *(io->Vdc));
        #endif
	}
	else
	{
		PwmModulation__DeadtimeCompensationGradientF(io->Is_ABC, io->Duty);
	}
    #endif
   #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle MclPwm events of 25ms.
 *
 *  @param      none
 *  @return     none
 */
void MclPwm__25msHandler(MCL_PWM_IO_F_TYPE *io)
{
    float32 temp_abs_speed;

    temp_abs_speed = *(io->Ctrl_Specific->Speed_Rotor_Observed_Mech_Abs);

    if(temp_abs_speed >= Speed_Threshold)
    {
        Speed_Threshold = Speed_Threshold_Low;

        Inverter_Compensation_Selector = DEADTIME_COMP;
    }
    if(temp_abs_speed <= Speed_Threshold)
    {
        Speed_Threshold = Speed_Threshold_High;

        Inverter_Compensation_Selector = INVERTER_LOSSES_COMP;
    }
}

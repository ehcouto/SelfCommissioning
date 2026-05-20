/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Braking.h"
#include "MclConfig.h"
#include "McMathCalc_macros.h"
#include "Filters.h"
#include "SpeedRefGen.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
/* Braking variables */
static float32 Max_Joule_Loss;          //!< [W] - Max Joule Losses
static float32 Id_Braking;              //!< [A] - Calculated amount of id current to apply during braking
static float32 Id_Braking_Max;          //!< [A] - Calculated max amount of id current to apply during braking
static float32 Torque_Brake_Max;        //!< [Nm] - Max torque available for braking
static float32 Speed_Filtered;
static float32 Braking_Resistance;
static float32 Id_Braking_Target;
static float32 Torque_Brake_Max_thr;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Braking and its variables
 *
 */
void Braking__ResetState(MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params)
{
    //Braking Parameters - Reseting before start
    Id_Braking        = 0;
    Id_Braking_Target = 0;
    Torque_Brake_Max_thr = params->SpeedCtrlAddPrm->Braking_Parameters.Min_Braking_Diss_Torque_High_Thr;
}

/* Executed in 1ms handler */
float32 Braking__SpeedCtrlHandler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params)
{
    float32 speed_abs;
    float32 is;

    Braking_Resistance = (*io->Ctrl_Specific->Stator_Resistance) * params->SpeedCtrlAddPrm->Braking_Parameters.Braking_Resistance_Percentage;

    Id_Braking_Max = -params->SpeedCtrlPrm->Braking_Parameters.Max_Braking_Diss_Current;
    /*==============================================================*/
    /*           Tmax for Braking                                   */
    /*==============================================================*/

    speed_abs = *(io->Ctrl_Specific->Speed_Rotor_Observed_Mech_Abs)+0.1f;  //Just in case to avoid zero division;

    // Filter the observed speed
    Speed_Filtered = FILTERS__LOWPASSFILTER_F(Speed_Filtered, BRAKING_SPEED_LPF_COEF, speed_abs);


    if (io->Ctrl_Specific->flags.bit.braking_active == 1)
    {
        if(params->SpeedCtrlAddPrm->Braking_Parameters.Braking_Mode == 0)
        {
            // braking strategy for IPM
            is = (*io->Ctrl_Specific->Is_Abs);
        }
        else
        {
            // braking strategy for SPM
            is = Id_Braking_Max;
        }

		Max_Joule_Loss = (1.5f* Braking_Resistance * is * is);  //Max power available for high speed
		Torque_Brake_Max = Max_Joule_Loss / Speed_Filtered;                         // Max torque calculation

		if(Torque_Brake_Max > (params->SpeedCtrlPrm->Braking_Parameters.Max_Braking_Diss_Torque))
		{
			Torque_Brake_Max = (params->SpeedCtrlPrm->Braking_Parameters.Max_Braking_Diss_Torque);
		}
		else
		{
			if (Torque_Brake_Max < Torque_Brake_Max_thr)
			{
				Torque_Brake_Max = 0;
				Torque_Brake_Max_thr = params->SpeedCtrlAddPrm->Braking_Parameters.Min_Braking_Diss_Torque_High_Thr;
			}
			else
			{   // hysteresis threshold updated
			    Torque_Brake_Max_thr = params->SpeedCtrlAddPrm->Braking_Parameters.Min_Braking_Diss_Torque_Low_Thr;
			}
		}
    }
    else
    {
        // Keep max braking to Tmax_no_braking_request according to target speed
        if (SpeedRefGen__GetSpeedTarget() < params->SpeedCtrlAddPrm->Braking_Parameters.Max_Braking_Gen_Low_High_Speed_Thr)
        {
            Torque_Brake_Max = params->SpeedCtrlAddPrm->Braking_Parameters.Max_Braking_Gen_Low_Speed;
        }
        else
        {
            Torque_Brake_Max = params->SpeedCtrlAddPrm->Braking_Parameters.Max_Braking_Gen_High_Speed;
        }
    }

    return(Torque_Brake_Max);
}

/* Executed in 1ms handler */
float32 Braking__DqRefHandler(MCL_DQ_REF_IO_F_TYPE *io, MCLDQREF_JOINT_PARAMS_TYPE *params)
{
#ifndef BRAKING__CONSERVATIVE_VARIANT
    float32 iq;
    float32 iq_2;
    float32 id_2;
    float32 is_2;
    float32 torque_abs;
    float32 stator_resistance;
    float32 power_mech;
#endif
    float32 speed;

    speed = *(io->Ctrl_Specific->Speed_Rotor_Observed_Mech);


    // Check if in braking state
    if(SpeedRefGen__GetStatus() == SPEED_REF_DECELERATING)
    {
        if(speed > 0)
        {
            if((*io->Torque_Ref) <= 0)
            {
                io->Ctrl_Specific->flags.bit.braking_active = 1;
            }
        }
        else if(speed < 0)
        {
            if((*io->Torque_Ref) > 0)
            {
                io->Ctrl_Specific->flags.bit.braking_active = 1;
            }
        }
    }
    else
    {
        if(speed > 0 && (*io->Torque_Ref) > 0)
        {
            io->Ctrl_Specific->flags.bit.braking_active = 0;
        }

        if(speed < 0 && (*io->Torque_Ref) < 0)
        {
            io->Ctrl_Specific->flags.bit.braking_active = 0;
        }
    }




    /*==============================================================================================*/
    /*           Braking Calculation for Id injection - Rotor reference frame with SPM approximation*/
    /*==============================================================================================*/
    /*
     * Calculation of Id for braking
     * Id = sqrt[T*w/Rs - (T/Kt)^2]
     * */
    if(io->Ctrl_Specific->flags.bit.braking_active == 1)
    {
        if(params->DqRefAddPrm->Braking_Parameters.Braking_Mode != 0)
        {    if (Torque_Brake_Max>0)
            {

                power_mech =  *(io->Torque_Ref) * speed;
                if (power_mech < 0)  // check if real brake request
                {
                    torque_abs = MATHCALC__FABS(*(io->Torque_Ref));
                    power_mech = torque_abs * Speed_Filtered;


                    stator_resistance = Braking_Resistance + 0.01f;     // Avoid zero division.
                    is_2 = power_mech / (1.5f*stator_resistance);       // I_max_braking^2 = Pmech / Rs
                    iq = torque_abs * params->DqRefPrm->K_torque_inv;               // Iq current calculation
                    iq_2 = (iq * iq);                                   // Iq squared current calculation


                    id_2 = (is_2 - iq_2);
                    if(id_2 > 0 )
                    {
                         Id_Braking_Target = -MC_SQRT_F(id_2);         // Id_Braking here is a module (need to handle signal)
                    }
                    // do not update Id_Braking if the argument of the square root is negative
                    if(Id_Braking_Target < Id_Braking_Max)
                    { /* clamp to the maximum value of Id */
                       Id_Braking_Target = Id_Braking_Max;
                    }
                    Id_Braking -= params->DqRefAddPrm->Braking_Parameters.Id_Braking_Prop_Gain * (Id_Braking - Id_Braking_Target);

                 }
                // do not update Id_Braking if torque and speed have the same sign
            }
        }
        else
        {
            Id_Braking_Target = Id_Braking_Max;
            Id_Braking -= params->DqRefAddPrm->Braking_Parameters.Id_Braking_Prop_Gain * (Id_Braking - Id_Braking_Target);
        }


        if(Id_Braking < Id_Braking_Max)
        { /* clamp to the maximum value of Id */
           Id_Braking = Id_Braking_Max;
        }
    }
    else
    {
        /* Here there is not braking required (flag_under_braking is zero)
         * If not braking then move Id_Braking slowly towards zero.
         */
        Id_Braking += params->DqRefAddPrm->Braking_Parameters.Id_Braking_Step_Back;
        if (Id_Braking > 0)
        {
            Id_Braking = 0;
        }

    }
    return(Id_Braking);
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

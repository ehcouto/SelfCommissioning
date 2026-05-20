/**
 *  @file       SpeedRefGen.c
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SpeedRefGen.h"
#include "MclConfig.h"
#include "McMathCalc_macros.h"
#include "Mci.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

uint8 Apply_Default_Accel;
float32 Default_Accel;
float32 Increment;
float32 SpeedRef_Target_Speed;
float32 SpeedRef_Reference_Speed;

float32 Max_Delta_Rpm;
float32 Starting_Accel;
float32 Min_Speed_For_Fixed_Accel;
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef RPM_TO_RADS
    #define RPM_TO_RADS     0.10471975511965977461542144610932f
#endif

//! Converts a value into TRUE or FALSE; 0 is FALSE, TRUE otherwise.
#ifndef GET_TRUE_FALSE
    #define GET_TRUE_FALSE(value)           (BOOL_TYPE)(((value) == 0) ? (FALSE) : (TRUE))
#endif
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Initialize the speed reference generator.
 */
void SpeedRefGen__Initialize(void)
{
    SpeedRef_Target_Speed = 0.0f;
    SpeedRef_Reference_Speed = 0.0f;
    Default_Accel = 0.0f;
    Apply_Default_Accel = FALSE;
}




//---------------------------------------------------------------------------------------------------------------------
void SpeedRefGen__SetSpeedParams(float32 default_accel)
{
    Default_Accel = default_accel * SPEEDREF__TS * RPM_TO_RADS;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Reset only intermediate variables, speed and accel targets are kept!
 *
 */
void SpeedRefGen__Reset(void)
{
    SpeedRef_Reference_Speed = 0.0f;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Configure the new speed.
 *
 * @param       target_rpm this is the target in RPM
 * @param       accel_rpm_s acceleration in RPM/s
 * @return      SPEED_REF_DENIED    [0] either speed or acceleration denied
 *              SPEED_REF_ACCEPTED  [1] both speed and acceleration accepted
 *              SPEED_REF_NEG_STEP  [2] step on speed reference accepted
 *              SPEED_REF_POS_STEP  [3] step on speed reference accepted
 */
SPEED_REF_SET_NEW_SPEED_TYPE SpeedRefGen__SetNewSpeed(sint32 target_rpm, sint32 accel_rpm_s)
{
    float32 temp_f;
    SPEED_REF_SET_NEW_SPEED_TYPE ret_val;
    sint32 accel_rpm_s_abs;

    ret_val = SPEED_REF_DENIED;

    accel_rpm_s_abs = MATHCALC__ABS(accel_rpm_s);

    if(accel_rpm_s_abs == 0)
    {
        temp_f  = (float32)(MATHCALC__ABS(target_rpm)) * RPM_TO_RADS;
        temp_f -= MATHCALC__FABS(SpeedRefGen__GetSpeedRef());

        if(temp_f < 0)
        {
            SpeedRef_Reference_Speed = (float32) target_rpm * RPM_TO_RADS;
            SpeedRef_Target_Speed = SpeedRef_Reference_Speed;
            Increment = 0.0f;

            ret_val = SPEED_REF_NEG_STEP;
        }
    }
    else
    {
        SpeedRef_Target_Speed = (float32) target_rpm * RPM_TO_RADS;

        //The speed reference is incremented by means of Gradient * FunctionCallTime which is [Rpm/s] * [s] => [Rpm]
        Increment  = (float32) accel_rpm_s_abs * SPEEDREF__TS * RPM_TO_RADS;

        ret_val = SPEED_REF_ACCEPTED;
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       This is the handler to generate the new speed reference
 * @return      the new speed reference in rad\s (mechanical)
 */
float32 SpeedRefGen__Generator(void)
{
	float32 temp_increment;

	if((Apply_Default_Accel)&&(Default_Accel!=0.0f))
	{
		temp_increment = Default_Accel;
	}
	else
	{
		temp_increment = Increment;
	}


    if(SpeedRef_Target_Speed > SpeedRef_Reference_Speed)
    {
    	if ((temp_increment > SPEEDREF__MAX_RPM_PER_SEC_DECEL())&&(SpeedRef_Target_Speed < 0))
    	{
        		temp_increment = SPEEDREF__MAX_RPM_PER_SEC_DECEL();
    	}

        SpeedRef_Reference_Speed += temp_increment;
        if(SpeedRef_Reference_Speed >= SpeedRef_Target_Speed)
        {
            SpeedRef_Reference_Speed = SpeedRef_Target_Speed;
        }
    }
    else if(SpeedRef_Target_Speed < SpeedRef_Reference_Speed)
    {

    	if ((temp_increment > SPEEDREF__MAX_RPM_PER_SEC_DECEL())&&(SpeedRef_Target_Speed >0))
    	{
    			temp_increment = SPEEDREF__MAX_RPM_PER_SEC_DECEL();
    	}


        SpeedRef_Reference_Speed -= temp_increment;
        if(SpeedRef_Target_Speed >= SpeedRef_Reference_Speed)
        {
            SpeedRef_Reference_Speed = SpeedRef_Target_Speed;
        }
    }

    return(SpeedRef_Reference_Speed);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get speed reference
 * @return      speed reference in rad\s
 */
float32 SpeedRefGen__GetSpeedRef(void)
{
    float32 ret_val;

    ret_val = SpeedRef_Reference_Speed;

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get speed target
 * @return      speed target in rad\s
 */
float32 SpeedRefGen__GetSpeedTarget(void)
{
    float32 ret_val;

    ret_val = SpeedRef_Target_Speed;

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the current status of the speed reference, the return data info is
 *              explained in the return data.
 * @return      SPEED_REF_ZEROED - the speed reference is at zero
 *              SPEED_REF_ACCELERATING - speed reference is increasing
 *              SPEED_REF_DECELERATING - speed reference is decreasing
 *              SPEED_REF_AT_PLATEAU - the speed reference is at the target
 *
 *       ***********************  Combinatorial table  *****************************
 *       target ... ref     sign target      sign product        status
 *       ***************************************************************************
 *               <              +                +               decelerating
 *               >              +                +               accelerating
 *               <              -                +               accelerating
 *               >              -                +               decelerating
 *               <              +                -               not possible
 *               >              +                -               decelerating
 *               <              -                -               decelerating
 *               >              -                -               not possible
 *       ***************************************************************************
 */

SPEED_REF_STATUS_TYPE SpeedRefGen__GetStatus(void)
{
    SPEED_REF_STATUS_TYPE status;
    float32 target_temp;
    float32 reference_temp;
    float32 target_ref_product;

    target_temp = MATHCALC__FABS(SpeedRef_Target_Speed);
    reference_temp = MATHCALC__FABS(SpeedRef_Reference_Speed);

    target_ref_product = SpeedRef_Target_Speed * SpeedRef_Reference_Speed;


    if( (!target_temp)  &&
        (!reference_temp)  )
    {
        status = SPEED_REF_ZEROED;
    }
    else if(SpeedRef_Target_Speed == SpeedRef_Reference_Speed)
    {
        status = SPEED_REF_AT_PLATEAU;
    }
    else if(target_ref_product >= 0)
    {
        if(target_temp < reference_temp)
        {
            status = SPEED_REF_DECELERATING;
        }
        else //if(target_temp > reference_temp)
        {
            status = SPEED_REF_ACCELERATING;
        }
    }
    else
    {
        status = SPEED_REF_DECELERATING;
    }

    return(status);
}



/*
 * @brief		Request the speed reference to apply minimum acceleration
 * @param[in]	apply_min_accel: 	TRUE - force default acceleration read from Mci
 * 									FALSE - use user acceleration from Mci API
 */
void SpeedRefGen__ApplyDefaultAccel(uint8 apply_min_accel)
{
	Apply_Default_Accel = GET_TRUE_FALSE(apply_min_accel);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get speed reference in IQ format
 * @params      speed base and IQ format
 * @return      speed target in IQ format
 */
sint32 SpeedRefGen__GetSpeedRefIQ(float32 speed_base, uint8 iq_format)
{
   sint32 ret_val;

   ret_val = (sint32)(SpeedRef_Reference_Speed / speed_base * (float32)(1U<<iq_format));

   return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get speed target in IQ format
 * @params      speed base and IQ format
 * @return      speed target in IQ format
 */
sint32 SpeedRefGen__GetSpeedTargetIQ(float32 speed_base, uint8 iq_format)
{
    sint32 ret_val;

    ret_val = (sint32)(SpeedRef_Target_Speed / speed_base * (float32)(1U<<iq_format));

    return(ret_val);
}


//=====================================================================================================================
//---------------------------------------------- Private Functions ----------------------------------------------------
//=====================================================================================================================




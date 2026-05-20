/**
 *  @file       MclSpeedCntrl.c
 *  @brief      Motor Control Loop 3-phase motors: Speed Controller Macro Block.
 *  @details    This module implements the Speed Controller module.
 *  @author     alessio.beato/luigi.fagnano
 *  $Header:
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
*/
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MclSpeedCtrl.h"
#include "McMathCalc_macros.h"
#include "Pi.h"
#include "Filters.h"
#include "Braking.h"
#include "Mci.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef RPM_TO_RADS
    #define RPM_TO_RADS     0.10471975511965977461542144610932f
#endif

static PI_CONTROLLER_F_TYPE Speed_Controller;
static PI_CONTROLLER_F_TYPE Load_Angle_Limiter;

#if SPEEDCTRL__TORQUE_LIM_OUTPUT_FILTER == ENABLED
static float32 Max_Torque_Lim_Out_Sum;
#endif
#if (SPEEDCTRL__CURR_LIM_IDS_FILTER==ENABLED)
static float32 Id_Stator_Current_Sum;
#endif

static uint8 Forced_Gain_Selector;
static uint16 Forced_Gain_Timer;

static float32 My_Max_Torque;

static float32 Torque_Max_Braking;

static uint16 Delta_Speed_Timer;                           //!< [x25 ms] timer to reset the delta speed when application does not use this feature

static volatile float32 Max_Squared_Inv_Current_Appl;
static volatile float32 Max_Design_Motor_Torque;
static volatile float32 Max_Design_Motor_Torque_Appl;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static float32 CurrentLimitation(MCL_SPEED_CTRL_IO_F_TYPE *io, MCL_SPEED_CTRL_PARAMS_TYPE *params);
static float32 Calculate_MTPV(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params);
static void Speed_Gain_Scheduler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCL_SPEED_CTRL_PARAMS_TYPE *params);
static void Delta_Speed_Handler(MCL_SPEED_CTRL_IO_F_TYPE *io);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Reset all Mcl Speed Controller quantities.
 *  @details    This routine reset all Speed Controller quantities, it has to be called at every time the pwm is switched off (motor stop or free down ramp).
 *
 *
 *  @param[in]     
 *  @param[Out]
 *  @return        
 */
void MclSpeedCtrl__ResetState(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params)
{
    Speed_Controller.Kp = params->SpeedCtrlPrm->speed_gains_table[0].kp;
    Speed_Controller.Ki = params->SpeedCtrlPrm->speed_gains_table[0].ki;

    Speed_Controller.Out        = 0.0f;
    Speed_Controller.Integ_K_1  = 0.0f;

    Load_Angle_Limiter.Upper_Limit = params->SpeedCtrlPrm->Max_Design_Motor_Torque;

    Load_Angle_Limiter.Out = 0.0f;
    // init integral term to saturation
    Load_Angle_Limiter.Integ_K_1 = Load_Angle_Limiter.Upper_Limit;
    

    //initialize the torque limitation filter

#if SPEEDCTRL__TORQUE_LIM_OUTPUT_FILTER == ENABLED
    Max_Torque_Lim_Out_Sum = params->SpeedCtrlPrm->Max_Design_Motor_Torque;
#endif

#if (SPEEDCTRL__CURR_LIM_IDS_FILTER==ENABLED)
    Id_Stator_Current_Sum = 0.0f;
#endif


    *(io->Ctrl_Specific->Delta_Speed_Rot_Ref) = 0.0f;

    Max_Design_Motor_Torque		    = params->SpeedCtrlPrm->Max_Design_Motor_Torque;
    Max_Design_Motor_Torque_Appl    = params->SpeedCtrlPrm->Max_Design_Motor_Torque;
    Max_Squared_Inv_Current_Appl 	= params->SpeedCtrlPrm->Max_Squared_Inv_Current;

    Forced_Gain_Selector = 0;
    Forced_Gain_Timer = 0;

    Braking__ResetState(params);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Loop Speed Controller initialization.
 *  @details    In this routine are called all initialization functions.
 *
 *
 *  @param[in]     
 *  @param[Out]
 *  @param[in]      
 *  @return        
 */
void MclSpeedCtrl__Initialize(MCL_SPEED_CTRL_IO_F_TYPE *io,MCLSPEEDCTRL_JOINT_PARAMS_TYPE* params)
{

    Load_Angle_Limiter.Kp = params->SpeedCtrlPrm->load_angle.Kp;
    Load_Angle_Limiter.Ki = params->SpeedCtrlPrm->load_angle.Ki;
    Load_Angle_Limiter.Lower_Limit = 0.0f;

    // reset MCL quantities
    MclSpeedCtrl__ResetState(io,params);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Speed Controller
 *  @details   Speed Controller: speed PI execution
 *                               torque reference generation
 *  @param[in]     
 *  @param[Out]
 *  @param[in]      
 *  @return        
 */
void MclSpeedCtrl__RunningHandler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params)
{
    float32 max_torque_voltage;
    float32 max_torque;
    float32 max_torque_curr_limit;


#if (SPEEDCTRL__START_TORQUE_LIM  == ENABLED)
    if( (*io->Ctrl_Specific->Elapsed_Time_After_Starting) <= (params->SpeedCtrlPrm->Max_Starting_Torque_Time) )
    {
        max_torque = params->SpeedCtrlPrm->Max_Starting_Torque;

    }
    else
#else
    {

        // Current limitation - max inverter current
        max_torque_curr_limit = CurrentLimitation(io, params->SpeedCtrlPrm);

        max_torque_voltage = Calculate_MTPV(io, params);

        max_torque = MATHCALC__SATURATE_DIRECT(0, max_torque_curr_limit, Max_Design_Motor_Torque_Appl);  // Interfacing with Application Layer

        // saturate
        max_torque = MATHCALC__SATURATE_DIRECT(0, max_torque, max_torque_voltage);

        //==========================================================================//
        //                                  Load Angle Limiter                      //
        //==========================================================================//
         Load_Angle_Limiter.Upper_Limit = max_torque;
         Load_Angle_Limiter.Err  = io->Ctrl_Specific->Load_Angle_Sin_Cos->Cos - params->SpeedCtrlPrm->load_angle.Max_Cos;

         Pi__CalcPiF(&Load_Angle_Limiter);

         max_torque = Load_Angle_Limiter.Out;

    }
#endif



#if (SPEEDCTRL__TORQUE_LIM_OUTPUT_FILTER==ENABLED)
    // Filtering ...
    Max_Torque_Lim_Out_Sum = FILTERS__LOWPASSFILTER_F(Max_Torque_Lim_Out_Sum, MAX_TORQUE_LIM_OUT_LPF_COEF, max_torque);
    max_torque = Max_Torque_Lim_Out_Sum;
#endif



    My_Max_Torque = max_torque;     // Debug Only


    Speed_Controller.Err  = *(io->Speed_Rot_Ref) - *(io->Speed_Rot);

    // Update Speed controller limits (according to motor direction)
    if(io->Ctrl_Specific->flags.bit.motor_direction > 0)
    {   // positive direction
        Speed_Controller.Upper_Limit = max_torque;
        Speed_Controller.Lower_Limit = -Torque_Max_Braking;
    }
    else
    {   // negative direction
        Speed_Controller.Upper_Limit = Torque_Max_Braking;
        Speed_Controller.Lower_Limit = -max_torque;
    }
    
    Pi__CalcPiF(&Speed_Controller);
    *(io->Torque_Ref)  = Speed_Controller.Out;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle MclSpeedCtrl events of 1ms.
 *
 *  @param      none
 *  @return     none
 */
void MclSpeedCtrl__1msHandler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE* params)
{
    //==========================================================================//
    //                                  Braking                                 //
    //        Calculating maximum torque according to braking requirement       //
    //==========================================================================//
    Torque_Max_Braking = Braking__SpeedCtrlHandler(io, params);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle MclSpeedCtrl events of 25ms.
 *
 *  @param      none
 *  @return     none
 */
void MclSpeedCtrl__25msHandler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE* params)
{

    Speed_Gain_Scheduler(io, params->SpeedCtrlPrm);

    Delta_Speed_Handler(io);

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set speed controller limitation.
 *  @details
 *
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
BOOL_TYPE MclSpeedCtrl__SetSpeedRegLimit(sint32  value)
{
	BOOL_TYPE value_accepted;
	float32 value_f;
	value_f = (float32) value / 65536.0f;
	if ((value_f <= 0)||(value_f > Max_Design_Motor_Torque))
	{
		Max_Design_Motor_Torque_Appl = Max_Design_Motor_Torque;
	}
	else
	{
		Max_Design_Motor_Torque_Appl = value_f;
	}

	value_accepted = TRUE;

	return(value_accepted);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set speed controller gains.
 *  @details
 *
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
BOOL_TYPE MclSpeedCtrl__SetSpeedGainIndex(sint32 speed_gains_index, MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params)
{
    BOOL_TYPE accepted;
    accepted = FALSE;

    // If there is a valid speed set, Kp and Ki should be ok.
    if( (params->SpeedCtrlPrm->speed_gains_table[speed_gains_index].speed_abs)  )
    {
        Forced_Gain_Selector = speed_gains_index;

        Forced_Gain_Timer = (uint16)(2.0f/0.025f);  // Define 2 seconds as timeout, it might be hardcoded
                                                    // and the timer is managed at 25 ms.

        accepted = TRUE;
    }

    return accepted;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set a speed reference delta for 2 seconds.
 *  @details
 *
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
BOOL_TYPE MclSpeedCtrl__SetDeltaSpeedRef(sint32 delta_speed_x65536, MCL_SPEED_CTRL_IO_F_TYPE *io)
{
    BOOL_TYPE accepted;
    float32 temp_1;
    float32 temp_2;

    accepted = FALSE;

    temp_1 = (float32)delta_speed_x65536 / 65536.0f * RPM_TO_RADS;

    temp_2 = temp_1 + *(io->Speed_Rot_Ref);
    temp_2 = MATHCALC__FABS(temp_2);

    if( (temp_2 <= (((float32)Mci__GetParams(MOTOR0,MCI_PARAMS_MAX_SPEED_RPM))*RPM_TO_RADS)) &&
        (temp_2 >= (((float32)Mci__GetParams(MOTOR0,MCI_PARAMS_MIN_SPEED_RPM))*RPM_TO_RADS)) )
    {
        *(io->Ctrl_Specific->Delta_Speed_Rot_Ref) = temp_1;
        Delta_Speed_Timer = (uint16)(0.5f/0.025f);  // Define 2 seconds as timeout, it might be hardcoded
                                                    // and the timer is managed at 25 ms.
        accepted = TRUE;
    }

    return accepted;
}



//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
static float32 CurrentLimitation(MCL_SPEED_CTRL_IO_F_TYPE *io, MCL_SPEED_CTRL_PARAMS_TYPE *params)
{
    float32 max_torque_curr_limit;
    float32 temp_1;
    float32 temp_2;


    // Limit the maximum torque according to the available current and maximum current of the inverter
    temp_1 = params->Max_Squared_Inv_Current;

    temp_2 = *(io->Ctrl_Specific->Id_stator_current);

#if (SPEEDCTRL__CURR_LIM_IDS_FILTER==ENABLED)
    // Filtering ...
    Id_Stator_Current_Sum = FILTERS__LOWPASSFILTER_F(Id_Stator_Current_Sum, CURR_LIM_IDS_LPF_COEF, temp_2);
    temp_2 = Id_Stator_Current_Sum;
#endif

    temp_2 = (temp_2*temp_2);

    // (Max inverter current)^2 - (Id)^2
    temp_1 = temp_1 - temp_2;

    if (temp_1>0)
    {
        // Max current = sqrt((Max inverter current)^2 - (Id)^2)
        temp_1 = MC_SQRT_F(temp_1);

        // Max torque according to available current
		max_torque_curr_limit = params->pole_pairs_factor * (temp_1 * *(io->Ctrl_Specific->Stator_Flux_Mag));
		 
    }
    else
    {
        max_torque_curr_limit = 0.0f;
    }

    return max_torque_curr_limit;
}


#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
static float32 Calculate_MTPV(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params)
{
    float32 max_torque_voltage;
    float32 temp_1;

    temp_1 = * io->Ctrl_Specific->Flux_Limit_By_Speed;
    max_torque_voltage = MathCalc__GetInterpolationFastF(temp_1, &params->MTPVPrm);

    return(max_torque_voltage);
}


static void Speed_Gain_Scheduler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCL_SPEED_CTRL_PARAMS_TYPE *params)
{

    static sint32 gains_indexer_previous;
    static sint32 gains_indexer;
    static sint16 relative_time;
    float32 temp_speed_table;
    float32 speed_rotor_abs;
    float32 delta;
    float32 temp_speed_kp;
    float32 temp_speed_ki;

	// If there is no timeout, e.g., application is polling the gains set
    if(Forced_Gain_Timer)
    {
        Forced_Gain_Timer--;

        Speed_Controller.Kp = params->speed_gains_table[Forced_Gain_Selector].kp;
        Speed_Controller.Ki = params->speed_gains_table[Forced_Gain_Selector].ki;
    }
    else
    {
        //// During starting, use default gains.
        if( (*io->Ctrl_Specific->Elapsed_Time_After_Starting) <= (uint32)(params->Force_Default_Gains_Time) )
        {
            Speed_Controller.Kp = params->speed_gains_table[0].kp;
            Speed_Controller.Ki = params->speed_gains_table[0].ki;

            // Initialize the variable, since it is starting now...
            gains_indexer_previous = 0;
            gains_indexer = 0;
            relative_time = 0;
        }
        else // Starting time is already over, so, look for new regions
        {
            // Look for new index only if the transition between regions is already done.
            if(gains_indexer == gains_indexer_previous)
            {
                // Save the speed on stack
                speed_rotor_abs = *(io->Ctrl_Specific->Speed_Rot_Ref_Abs);//*(io->Speed_Rot_Absolute);

                // Look for the indexer
                for(gains_indexer = 0; gains_indexer < (SPEEDCTRL__SPEED_REGIONS-1); gains_indexer++)
                {
                    // Only one indexed reading is performed
                    temp_speed_table = (params->speed_gains_table[gains_indexer].speed_abs) * RPM_TO_RADS;

                    if(temp_speed_table > 0)
                    {
                        if( speed_rotor_abs < temp_speed_table )
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                relative_time = 0;
            }
            else //if(gains_indexer != gains_indexer_previous)
            {
                relative_time++;

                // Calculate flux gains - kp
                delta = (params->speed_gains_table[gains_indexer].kp) - (params->speed_gains_table[gains_indexer_previous].kp);
                delta = delta*relative_time;
                delta = delta/(params->speed_gains_table[gains_indexer].transition_time);
                temp_speed_kp = delta + (params->speed_gains_table[gains_indexer_previous].kp);

                // Calculate flux gains - ki
                delta = (params->speed_gains_table[gains_indexer].ki) - (params->speed_gains_table[gains_indexer_previous].ki);
                delta = delta*relative_time;
                delta = delta/(params->speed_gains_table[gains_indexer].transition_time);
                temp_speed_ki = delta + (params->speed_gains_table[gains_indexer_previous].ki);


                if(relative_time >= (params->speed_gains_table[gains_indexer].transition_time))
                {
                    gains_indexer_previous = gains_indexer;

                    temp_speed_kp = (params->speed_gains_table[gains_indexer].kp);
                    temp_speed_ki = (params->speed_gains_table[gains_indexer].ki);
                }

                Speed_Controller.Kp = temp_speed_kp;
                Speed_Controller.Ki = temp_speed_ki;
            }
        }
    }
}


static void Delta_Speed_Handler(MCL_SPEED_CTRL_IO_F_TYPE *io)
{
    if(Delta_Speed_Timer)
    {
        Delta_Speed_Timer--;
    }
    else
    {
        *(io->Ctrl_Specific->Delta_Speed_Rot_Ref) = 0.0f;
    }
}

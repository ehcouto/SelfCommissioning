/**
 *  @file       MclDqCtrl.c
 *  @brief      Motor Control Loop 3-phase motors: DQ Reference Controllers Macro Block.
 *  @details    This module implements the DQ Control module.
 *  @author     alessio.beato/luigi.fagnano
 *  $Header:
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
*/
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MclDqCtrl.h"
#include "McMathCalc_macros.h"
#include "Pi.h"
#include "ClrkPark.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef SQRT3_INV
    #define SQRT3_INV     (float32) (1.0f/1.7320508075688772935274463415059f)
#endif

#ifndef RPM_TO_RADS
    #define RPM_TO_RADS     0.10471975511965977461542144610932f
#endif

static PI_CONTROLLER_F_TYPE Flux_Controller;
static PI_CONTROLLER_F_TYPE Torque_Controller;


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void Torque_Flux_Gains_Scheduler(MCL_DQ_CTRL_IO_F_TYPE *io, MCL_DQ_CTRL_PARAMS_TYPE *params);
#ifdef DQCTRL__HEXAGON_LIMIT
static float32 HexagonTrajectoryCalculation(MCL_DQ_CTRL_IO_F_TYPE *io);
#endif


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Reset all Mcl Dq Controllers quantities.
 *  @details    This routine reset all Dq Controllers quantities, it has to be called at every time the pwm is switched off (motor stop or free down ramp).
 *
 *
 *  @param[in]     
 *  @param[Out]
 *  @return        
 */
void MclDqCtrl__ResetState(MCL_DQ_CTRL_IO_F_TYPE *io, MCL_DQ_CTRL_PARAMS_TYPE *params)
{
    //Private variables reset
    io->Vs_Alpha_Beta->Alpha = 0.0f;
    io->Vs_Alpha_Beta->Beta = 0.0f;

    Torque_Controller.Kp = params->gains[0].torque_kp;
    Torque_Controller.Ki = params->gains[0].torque_ki;

    Torque_Controller.Integ_K_1 = 0.0f;
    Torque_Controller.Out = 0.0f;

    Flux_Controller.Kp = params->gains[0].flux_kp;
    Flux_Controller.Ki = params->gains[0].flux_ki;

    Flux_Controller.Out = 0.0f;
    Flux_Controller.Integ_K_1 = 0.0f;

    Flux_Controller.Upper_Limit =  params->Vd_Max;
    Flux_Controller.Lower_Limit = -params->Vd_Max;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Loop Dq Controller initialization.
 *  @details    In this routine are called all initialization functions.
 *
 *
 *  @param[in]     
 *  @param[Out]
 *  @param[in]      
 *  @return        
 */
void MclDqCtrl__Initialize(MCL_DQ_CTRL_IO_F_TYPE *io, MCL_DQ_CTRL_PARAMS_TYPE *params)
{
	// reset MCL quantities
	MclDqCtrl__ResetState(io, params);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Mcl DQ Controllers Override Gains.
 *  @details    In this routine Torque and Flux controllers gains are changed according input parameters.
 *
 *
 *  @param[in]
 *  @param[Out]
 *  @param[in]
 *  @return
 */
void MclDqCtrl__OverrideRegGains(MCL_DQ_CTRL_PARAMS_TYPE *params)
{
    Torque_Controller.Kp = params->gains[0].torque_kp;
    Torque_Controller.Ki = params->gains[0].torque_ki;
    Flux_Controller.Kp = params->gains[0].flux_kp;
    Flux_Controller.Ki = params->gains[0].flux_ki;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Dq Reference Controllers
 *  @details   Dq Controller:   DQ PI execution (FOC: current PI, DTC: flux/torque PI)
 *                              alpha-beta voltage reference generation
 *  @param[in]     
 *  @param[Out]
 *  @param[in]      
 *  @return        
 */
void MclDqCtrl__RunningHandler(MCL_DQ_CTRL_IO_F_TYPE *io, MCL_DQ_CTRL_PARAMS_TYPE *params)
{
    float32 vs_max;
    float32 vd_out;
    float32 vq_out_max;
    float32 temp;
    ALPHA_BETA_COOR_SYST_F_TYPE temp_vdq;  // used for phase advance angle rotation
    DQ_COOR_SYST_F_TYPE dq_in;
    SIN_COS_F_TYPE sin_cos;

    dq_in.ZeroSequence = 0.0f;
    temp_vdq.ZeroSequence = 0.0f;

    //==========================================================================//
    //                           Flux control loop                              //
    //==========================================================================//
    Flux_Controller.Err = *(io->D_Ref) -  *(io->D_Est);
    Pi__CalcPiF(&Flux_Controller);
    vd_out = Flux_Controller.Out;


    //==========================================================================//
    //                          Torque control loop                             //
    //==========================================================================//
#ifdef DQCTRL__HEXAGON_LIMIT
    //hexagon trajectory
    temp = HexagonTrajectoryCalculation(io);
    // Calculate the maximum available vq voltage considering the desired usage of available DC bus voltage
        vs_max = (*(io->Vdc) * SQRT3_INV) * temp;
#else
        vs_max = (*(io->Vdc) * SQRT3_INV) * params->m_index;
#endif


    /*! Vq limitation is given by:
     * \f[
     * \begin{aligned}
     *      &V_{qmax} = \sqrt{V_{smax}^2 - v_{ds}^2}
     * \end{aligned}
     * \f]
     */
    vq_out_max  = vs_max*vs_max;
    vq_out_max -= vd_out*vd_out;
    vq_out_max = MC_SQRT_F(vq_out_max);


    Torque_Controller.Upper_Limit =  vq_out_max;
    Torque_Controller.Lower_Limit = -vq_out_max;

    Torque_Controller.Err = *(io->Q_Ref) -  *(io->Q_Est);
    Pi__CalcPiF(&Torque_Controller);


    //==========================================================================//
    //          Check and if requested, overwrite the output voltage            //
    //==========================================================================//
    if( (io->Force_Out_Volt->D != 0) ||
        (io->Force_Out_Volt->Q != 0) )
    {
        dq_in.D = io->Force_Out_Volt->D;
        temp    = io->Force_Out_Volt->D;

        // Load the integral of PI with the output voltage
        Flux_Controller.Integ_K_1= temp;
        Flux_Controller.Out = temp;


        dq_in.Q  = io->Force_Out_Volt->Q;
        temp     = io->Force_Out_Volt->Q;

        // Load the integral of PI with the output voltage
        Torque_Controller.Integ_K_1 = temp;
        Torque_Controller.Out = temp;

        io->Vs_DQ->D = dq_in.D;
        io->Vs_DQ->Q = dq_in.Q;

    }
    else
    {
        dq_in.D = Flux_Controller.Out;
        dq_in.Q = Torque_Controller.Out;

        io->Vs_DQ->D = dq_in.D;
        io->Vs_DQ->Q = dq_in.Q;


        //==========================================================================//
        //                        Phase Advance                                     //
        //==========================================================================//
        #ifdef PWM_DOUBLE_SLOT
        temp = ( *(io->Ctrl_Specific->Speed_Rotor_Observed) * params->Phase_Advance_Coeff * 0.5f); //half the Ts due to double slot (shift 16 instead of 15)
        #else
        temp = ( *(io->Ctrl_Specific->Speed_Rotor_Observed) * params->Phase_Advance_Coeff );
        #endif

        MathCalc__SinCosF(temp,  &sin_cos);


        ClrkPark__InverseParkF(&dq_in, &temp_vdq, &sin_cos);

        dq_in.D = temp_vdq.Alpha;
        dq_in.Q = temp_vdq.Beta;
    }

    //==========================================================================//
    //                Inverse Park transformation (dq-abc)                      //
    //==========================================================================//
    ClrkPark__InverseParkF(&dq_in, io->Vs_Alpha_Beta, io->Sin_Cos);
}


#ifdef PWM_DOUBLE_SLOT
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Loop Dq Controller 2nd Running Handler.
 *  @details    This routine is called in second pwm slots.
 *
 *
 *  @param[in]
 *  @param[Out]
 *  @param[in]
 *  @return
 */
void MclDqCtrl__2ndRunningHandler(MCL_DQ_CTRL_IO_F_TYPE *io, MCL_DQ_CTRL_PARAMS_TYPE *params)
{
    float32 temp_vd;
    float32 temp_vq;
    float32 temp;
    DQ_COOR_SYST_F_TYPE dq_in;
    SIN_COS_F_TYPE sin_cos;

    dq_in.D = (io->Vs_DQ->D);
    dq_in.Q = (io->Vs_DQ->Q);

    //==========================================================================//
    //                        Phase Advance (advance of 1Ts)                    //
    //==========================================================================//
#ifdef SIM_SENSORED_MODE
    temp = ( *(io->Ctrl_Specific->Speed_Rotor_Observed) * params->Phase_Advance_Coeff * 0.5f); // the sincos is updated from simulation so we just need one phase advance step (halved due to double slot)
#else
    temp = ( *(io->Ctrl_Specific->Speed_Rotor_Observed) * params->Phase_Advance_Coeff ); // the sincos is the internal one, so we need two steps (halved due to double slot)
#endif
    MathCalc__SinCosF(temp,  &sin_cos);

    temp_vd  = (dq_in.D * sin_cos.Cos); //(Flux_Controller.Out * sin_cos.Cos)>>15;
    temp_vd -= (dq_in.Q * sin_cos.Sin); //(Torque_Controller.Out * sin_cos.Sin)>>15;
    temp_vq  = (dq_in.D * sin_cos.Sin); //(Flux_Controller.Out * sin_cos.Sin)>>15;
    temp_vq += (dq_in.Q * sin_cos.Cos); //(Torque_Controller.Out * sin_cos.Cos)>>15;

    dq_in.D = temp_vd;//Flux_Controller.Out = temp_vd;
    dq_in.Q = temp_vq;//Torque_Controller.Out = temp_vq;


    //==========================================================================//
    //                Inverse Park transformation (dq-abc)                      //
    //==========================================================================//
    ClrkPark__InverseParkF(&dq_in, io->Vs_Alpha_Beta, io->Sin_Cos);

}
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle MclDqCtrl events of 25ms.
 *
 *  @param      none
 *  @return     none
 */
void MclDqCtrl__25msHandler(MCL_DQ_CTRL_IO_F_TYPE *io, MCL_DQ_CTRL_PARAMS_TYPE *params)
{
    Torque_Flux_Gains_Scheduler(io, params);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

static void Torque_Flux_Gains_Scheduler(MCL_DQ_CTRL_IO_F_TYPE *io, MCL_DQ_CTRL_PARAMS_TYPE *params)
{
    static sint32 gains_indexer_previous;
    static sint32 gains_indexer;
    static sint16 relative_time;
    float32 temp_speed_table;
    float32 speed_rotor_abs;
    float32 delta;
    float32 temp_flux_kp;
    float32 temp_flux_ki;
    float32 temp_torque_kp;
    float32 temp_torque_ki;


    // During starting, use default gains.
    if( (*io->Ctrl_Specific->Elapsed_Time_After_Starting) <= (uint32)params->Force_Gains_Starting_Time )
    {
        Torque_Controller.Kp = params->gains[0].torque_kp;
        Torque_Controller.Ki = params->gains[0].torque_ki;

        Flux_Controller.Kp = params->gains[0].flux_kp;
        Flux_Controller.Ki = params->gains[0].flux_ki;

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
            speed_rotor_abs = *(io->Ctrl_Specific->Speed_Rotor_Observed_Mech_Abs);

            // Look for the indexer
            for(gains_indexer = 0; gains_indexer < (DQCRTL__MAX_GAINS_INDEX-1); gains_indexer++)
            {
                // Only one indexed reading is performed
                temp_speed_table = (params->gains[gains_indexer].abs_speed) * RPM_TO_RADS;

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
            delta = (params->gains[gains_indexer].flux_kp) - (params->gains[gains_indexer_previous].flux_kp);
            delta = delta*relative_time;
            delta = delta/(params->gains[gains_indexer].transition_time);
            temp_flux_kp = delta + (params->gains[gains_indexer_previous].flux_kp);

            // Calculate flux gains - ki
            delta = (params->gains[gains_indexer].flux_ki) - (params->gains[gains_indexer_previous].flux_ki);
            delta = delta*relative_time;
            delta = delta/(params->gains[gains_indexer].transition_time);
            temp_flux_ki = delta + (params->gains[gains_indexer_previous].flux_ki);


            // Calculate torque gains - kp
            delta = (params->gains[gains_indexer].torque_kp) - (params->gains[gains_indexer_previous].torque_kp);
            delta = delta*relative_time;
            delta = delta/(params->gains[gains_indexer].transition_time);
            temp_torque_kp = delta + (params->gains[gains_indexer_previous].torque_kp);

            // Calculate torque gains - ki
            delta = (params->gains[gains_indexer].torque_ki) - (params->gains[gains_indexer_previous].torque_ki);
            delta = delta*relative_time;
            delta = delta/(params->gains[gains_indexer].transition_time);
            temp_torque_ki = delta + (params->gains[gains_indexer_previous].torque_ki);


            if(relative_time >= (params->gains[gains_indexer].transition_time))
            {
                gains_indexer_previous = gains_indexer;

                temp_flux_kp = (params->gains[gains_indexer].flux_kp);
                temp_flux_ki = (params->gains[gains_indexer].flux_ki);

                temp_torque_kp = (params->gains[gains_indexer].torque_kp);
                temp_torque_ki = (params->gains[gains_indexer].torque_ki);
            }

            Flux_Controller.Kp = temp_flux_kp;
            Flux_Controller.Ki = temp_flux_ki;

            Torque_Controller.Kp = temp_torque_kp;
            Torque_Controller.Ki = temp_torque_ki;
        }
    }
}



#ifdef DQCTRL__HEXAGON_LIMIT
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
static float32 HexagonTrajectoryCalculation(MCL_DQ_CTRL_IO_F_TYPE *io)
{
    SIN_COS_F_TYPE sin_cos_reduced;
    float32 hexagon;
    //reduced sine and cosine to 0 - pi/3 range
    sin_cos_reduced.Cos = io->Sin_Cos->Cos;
    sin_cos_reduced.Sin = io->Sin_Cos->Sin;

    if (sin_cos_reduced.Sin < 0)
    {
        sin_cos_reduced.Sin = -sin_cos_reduced.Sin;
        sin_cos_reduced.Cos = -sin_cos_reduced.Cos;
    }

    if ((sin_cos_reduced.Cos < 0.5f) && (sin_cos_reduced.Cos > -0.5f))
    {
        hexagon = 1.0f/sin_cos_reduced.Sin;
    }
    else if ((sin_cos_reduced.Cos < -0.5f))
    {
        hexagon = 2.0f / (-F_SQRT3 * sin_cos_reduced.Cos + sin_cos_reduced.Sin);
    }
    else
    {
        hexagon = 2.0f / (F_SQRT3 * sin_cos_reduced.Cos + sin_cos_reduced.Sin);
    }
    return(hexagon);
}
#endif

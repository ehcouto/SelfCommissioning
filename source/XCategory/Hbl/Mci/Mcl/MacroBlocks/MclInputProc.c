/**
 *  @file       MclInputProc.c
 *  @brief      Motor Control Loop 3-phase motors: Input processing Macro Block.
 *  @details    This module implements the DC-Bus voltage filtering, the motor current clarke transformation.
 *  @author     alessio.beato/luigi.fagnano
 *  $Header:
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
*/
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MclInputProc.h"
#include "McMathCalc_macros.h"
#include "Filters.h"
#include "ClrkPark.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef INPUTPROC__CURRENT_OFFSET_COMP
    #define INPUTPROC__CURRENT_OFFSET_COMP DISABLED
#endif

#define MCL_INPUT_1MS_CALL_RATE_HZ          1000.0f // [Hz] define the call rate of the main handler

#ifndef INV_3
    #define INV_3                           (float32) (1.0f/3.0f)
#endif

#ifndef RPM_TO_RADS
    #define RPM_TO_RADS                     0.10471975511965977461542144610932f
#endif

#if (INPUTPROC__VDCFILTER_MINIMUM == ENABLED)
static float32 DC_Bus_Min;                           // [V] Instant minimum DC bus voltage
static float32 DC_Bus_Min_temp;                      // [V] Instant minimum DC bus voltage - temporary

static float32 DC_Bus_Min_Sum;                       // [V] dc bus filtering history
static float32 DC_Bus_Min_Sum_2nd;                   // [V] dc bus filtering history
#endif

#if (INPUTPROC__VDC_RMS == ENABLED)
static float32  DC_Bus_RMS_Sum;
static volatile float32 DC_Bus_RMS;
#endif

#if (INPUTPROC__CURRENT_OFFSET_COMP == ENABLED)
static BOOL_TYPE Cur_Offset_Compensation;
static float32 Cur_Offset_Comp_Factor = 0.0f;

static float32 Speed_Threshold_High_CurOffComp;
static float32 Speed_Threshold_Low_CurOffComp;

static float32 Cur_Offset_Ia;
static float32 Cur_Offset_Ib;
static float32 Cur_Offset_Ic;
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void VdcFilter(MCL_INPUT_PROC_IO_F_TYPE *io, MCL_INPUT_PROC_PARAMS_TYPE *params);
static void PhaseVoltageReconstruction(MCL_INPUT_PROC_IO_F_TYPE *io);
static void CurrentClarkTransform(MCL_INPUT_PROC_IO_F_TYPE *io);
static void CalcStatorCurrentMagnitude(MCL_INPUT_PROC_IO_F_TYPE *io);

#if (INPUTPROC__CURRENT_OFFSET_COMP == ENABLED)
static void CurrentOffSetComp(MCL_INPUT_PROC_IO_F_TYPE *io, MCL_INPUT_PROC_ADDITIONAL_PARAMS_TYPE *params);
static void CurrentOffSetLogicEnable(MCL_INPUT_PROC_IO_F_TYPE *io, MCL_INPUT_PROC_JOINT_PARAMS_TYPE *params);
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Reset all Mcl Init Processing quantities.
 *  @details    This routine reset all Mcl Init Processing quantities, it has to be called at every time the pwm is switched off (motor stop or free down ramp).
 *
 *
 *  @param[in]     
 *  @param[out]
 *  @return        
 */
void MclInputProc__ResetState(void)
{
#if (INPUTPROC__CURRENT_OFFSET_COMP == ENABLED)
    Cur_Offset_Comp_Factor = 0;
    Cur_Offset_Compensation = FALSE;
    Cur_Offset_Ia=0;
    Cur_Offset_Ib=0;
    Cur_Offset_Ic=0;
#endif
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Loop Input Processing initialization.
 *  @details    In this routine are called all initialization functions.
 *
 *
 *  @param[in]     
 *  @param[out]     
 *  @param[in]      
 *  @return        
 */
void MclInputProc__Initialize(MCL_INPUT_PROC_JOINT_PARAMS_TYPE *params)
{
#if (INPUTPROC__VDCFILTER_MINIMUM == ENABLED)
    DC_Bus_Min_Sum      = 0.0f;
    DC_Bus_Min_Sum_2nd  = 0.0f;
#endif
#if (INPUTPROC__VDC_RMS == ENABLED)
    DC_Bus_RMS_Sum = 0;
#endif

#if (INPUTPROC__CURRENT_OFFSET_COMP == ENABLED)
    Speed_Threshold_High_CurOffComp = params->InputProcPrm->Current_Offset_Comp_Speed_Thr *(1.0f + params->InputProcAddPrm->Current_Offset_Comp_Speed_Hyst_Thr);
    Speed_Threshold_Low_CurOffComp  = params->InputProcPrm->Current_Offset_Comp_Speed_Thr *(1.0f - params->InputProcAddPrm->Current_Offset_Comp_Speed_Hyst_Thr);
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Input processing
 *  @details   Input processing:   motor speed reference absolute value
 *                                 motor phase currents swapping in order to work with a positive speed reference.
 *                                 dc voltage filtering
 *                                 alpha-beta motor phase currents calculation.
 *
 *  @param[in]     
 *  @param[out]     
 *  @param[in]      
 *  @return        
 */
void MclInputProc__RunningHandler(MCL_INPUT_PROC_IO_F_TYPE *io,MCL_INPUT_PROC_JOINT_PARAMS_TYPE *params)
{


#if (INPUTPROC__CURRENT_OFFSET_COMP == ENABLED)
    //==========================================================================//
    //                                                                          //
    //                      Current OffSet Compensation                         //
    //                                                                          //
    //==========================================================================//
    CurrentOffSetComp(io,params->InputProcAddPrm);
#endif

    //==========================================================================//
    //                                                                          //
    //                     Forward Clarke Transform                             //
    //       Transforms ABC currents to Alpha/Beta coordinate system            //
    //                                                                          //
    //==========================================================================//
    CurrentClarkTransform(io);


    //==========================================================================//
    //                                                                          //
    //                    Phase voltage reconstruction                          //
    //                                                                          //
    //==========================================================================//
    PhaseVoltageReconstruction(io);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle MclSpeedCtrl events of 1ms.
 *
 *  @param      none
 *  @return     none
 */
void MclInputProc__1msHandler(MCL_INPUT_PROC_IO_F_TYPE *io, MCL_INPUT_PROC_JOINT_PARAMS_TYPE *params)
{
    //==========================================================================//
    //                                                                          //
    //              Calculating the stator current magnitude                    //
    //                                                                          //
    //==========================================================================//
    CalcStatorCurrentMagnitude(io);


    //==========================================================================//
    //                                                                          //
    //              Filter DC bus and track lowest voltage                      //
    //                                                                          //
    //==========================================================================//
    VdcFilter(io, params->InputProcPrm);


#if (INPUTPROC__CURRENT_OFFSET_COMP == ENABLED)
    //==========================================================================//
    //                                                                          //
    //                      Current OffSet Compensation                         //
    //                                                                          //
    //==========================================================================//
    CurrentOffSetLogicEnable(io, params);
#endif
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Apply Clarke transform to the currents.
 *  @details    By applying Clarke transformation to the currents, it shall provide the alpha and beta currents
 *              from the measured currents a, b and c.
 *
 *  @param[in]  MCL_INPUT_PROC_IO_TYPE *io
 *  @param[out] MCL_INPUT_PROC_IO_TYPE *io
 *  @return     none
 */
static void CurrentClarkTransform(MCL_INPUT_PROC_IO_F_TYPE *io)
{
    ClrkPark__DirectClarkeF(io->Is_ABC, io->Is_Alpha_Beta);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate the peak current (magnitude) from alpha and beta currents
 *  @details    By applying Clarke transformation to the currents, it shall provide the alpha and beta currents
 *              from the measured currents a, b and c.
 *
 *  @param[in]  MCL_INPUT_PROC_IO_TYPE *io
 *  @param[out] MCL_INPUT_PROC_IO_TYPE *io
 *  @return     none
 */
static void CalcStatorCurrentMagnitude(MCL_INPUT_PROC_IO_F_TYPE *io)
{
    float32 temp;

    temp  = (io->Is_Alpha_Beta->Alpha) * (io->Is_Alpha_Beta->Alpha);
    temp += (io->Is_Alpha_Beta->Beta)  * (io->Is_Alpha_Beta->Beta);

    *(io->Ctrl_Specific->Is_Abs) = MC_SQRT_F(temp);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Tracks the minimum instant dc bus voltage
 *  @details    This algorithm tracks the minimum instant dc bus voltage voltage, keep the minimum for certain
 *              amount of time before reseting the temporary minimum and update the value. In this algorithm,
 *              the minimum is always keep at the minimum of the instant voltage.
 *
 *  @param[in]  MCL_INPUT_PROC_IO_TYPE *io
 *  @param[out] MCL_INPUT_PROC_IO_TYPE *io
 *  @return     none
 */
static void VdcFilter(MCL_INPUT_PROC_IO_F_TYPE *io, MCL_INPUT_PROC_PARAMS_TYPE *params)
{
#if (INPUTPROC__VDCFILTER_MINIMUM == ENABLED)
    static uint8 prescaler_25ms = (uint8)(MCL_INPUT_1MS_CALL_RATE_HZ/40.0f);
#endif
#if (INPUTPROC__VDC_RMS == ENABLED)
    static uint8 window_avg = 80;
    float32 Vdc;
#endif
#if (INPUTPROC__VDCFILTER_MINIMUM == ENABLED)
    // Filter the minimum instant dc bus voltage
    DC_Bus_Min_Sum = FILTERS__LOWPASSFILTER_F(DC_Bus_Min_Sum, DC_BUS_LPF_COEF, DC_Bus_Min);

	DC_Bus_Min_Sum_2nd = FILTERS__LOWPASSFILTER_F(DC_Bus_Min_Sum_2nd, DC_BUS_LPF_COEF, DC_Bus_Min_Sum);

	*(io->Vs_Max_Filt) = DC_Bus_Min_Sum_2nd;

	*(io->Vs_Max_Filt) = params->Max_Available_Voltage_Factor * *(io->Vs_Max_Filt);


    // every 25 ms, the temporary minimum is reset
    prescaler_25ms--;
    if(!prescaler_25ms)
    {
        prescaler_25ms = (uint8)(MCL_INPUT_1MS_CALL_RATE_HZ/40.0f);

        // This is to bring up the Dc_Bus_Pu_Min because in AtodISR it only get reduced.
        // dcbus_pu_min_target is reset periodically.
        if(DC_Bus_Min < DC_Bus_Min_temp)
        {
            DC_Bus_Min = DC_Bus_Min_temp;
        }

        //Reset dcbus_pu_min_target. Bring it up. It get reduced at AtodISR.
        DC_Bus_Min_temp = *(io->Vdc);
    }


    // Look for the minimum value
    if(*(io->Vdc) < DC_Bus_Min)
    {
        DC_Bus_Min = *(io->Vdc);
    }
    if(*(io->Vdc) < DC_Bus_Min_temp)
    {
        DC_Bus_Min_temp = *(io->Vdc);
    }
#endif
#if (INPUTPROC__VDC_RMS == ENABLED)

    Vdc = *(io->Vdc);
    DC_Bus_RMS_Sum += (Vdc * Vdc);

    // every 80 samples we update the RMS (8000Hz / 100Hz = 80)
    window_avg--;
    if(!window_avg)
    {
        window_avg = 80;
        DC_Bus_RMS = MC_SQRT_F(0.0125f * DC_Bus_RMS_Sum);  // RMS calculation
        DC_Bus_RMS_Sum = 0.0f;
    }
#endif

}


#ifdef __IAR_SYSTEMS_ICC__
#pragma inline = forced
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Reconstruct applied phase voltage from the last pwm command duty cycles.
 *  @details
 *
 *  @param[in]  MCL_INPUT_PROC_IO_TYPE *io
 *  @param[out] MCL_INPUT_PROC_IO_TYPE *io
 *  @return     none
 */
static void PhaseVoltageReconstruction(MCL_INPUT_PROC_IO_F_TYPE *io)
{
    float32 temp_vdc;
    float32 temp_common_mode;

    // Check if the command to enable lowers on was enabled, if it was enabled
    if( *(io->Ctrl_Specific->Lowers_On) )
    {
        // voltages applied to the motor is zero.
        io->Vs_Alpha_Beta_Rec->Alpha = 0.0f;
        io->Vs_Alpha_Beta_Rec->Beta = 0.0f;

        io->Vabc_Rec->A = 0.0f;
        io->Vabc_Rec->B = 0.0f;
        io->Vabc_Rec->C = 0.0f;

    }
    else
    {
        // Read the last duty cycle and scale it to the DC bus voltage.
        // Note: assume the DC bus voltage is kept constant within one pwm period.
        temp_vdc = *(io->Vdc);
        io->Vabc_Rec->A = (io->Duty->A * temp_vdc);
        io->Vabc_Rec->B = (io->Duty->B * temp_vdc);
        io->Vabc_Rec->C = (io->Duty->C * temp_vdc);

        //ClrkPark__DirectClarkeF(io->Vabc_Rec, io->Vs_Alpha_Beta_Rec);

        temp_common_mode = (io->Vabc_Rec->A) + (io->Vabc_Rec->B) + (io->Vabc_Rec->C);
        temp_common_mode = temp_common_mode * INV_3 ;

        io->Vabc_Rec->A -= temp_common_mode;
        io->Vabc_Rec->B -= temp_common_mode;
        io->Vabc_Rec->C -= temp_common_mode;

        ClrkPark__DirectClarkeF(io->Vabc_Rec, io->Vs_Alpha_Beta_Rec);
    }
}



#if (INPUTPROC__CURRENT_OFFSET_COMP == ENABLED)
static void CurrentOffSetComp(MCL_INPUT_PROC_IO_F_TYPE *io, MCL_INPUT_PROC_ADDITIONAL_PARAMS_TYPE *params)
{

    float32 ia,ib,ic;
    static sint32 cyc_count_A=0;
    static sint32 cyc_count_B=0;
    static sint32 cyc_count_C=0;
    static float32 previous_A=0;
    static float32 previous_B=0;
    static float32 previous_C=0;
    static float32 sum_ia=0;
    static float32 sum_ib=0;
    static float32 sum_ic=0;

    float32 delta;
    float32 max_current_compensation;

    ia = (io->Is_ABC->A);
    ib = (io->Is_ABC->B);
    ic = (io->Is_ABC->C);

    max_current_compensation = params->Current_Offset_Comp_Max_Current_Comp;

    if(previous_A < 0 && ia >= 0)
    {
        if(cyc_count_A > params->Current_Offset_Comp_Full_Cycle_Cnt)
        {
              delta = previous_A /(previous_A - ia);  // positive by definition -- denominator is always different from zero by definition
              Cur_Offset_Ia = sum_ia / (cyc_count_A + delta);
              sum_ia = 0.0f;
              cyc_count_A = -(sint32)delta;
        }
    }

    if(previous_B < 0 && ib >= 0)
    {
        if(cyc_count_B > params->Current_Offset_Comp_Full_Cycle_Cnt)
        {
              delta = previous_B /(previous_B - ib);  // positive by definition -- denominator is always different from zero by definition
              Cur_Offset_Ib = sum_ib / (cyc_count_B + delta);
              sum_ib = 0.0f;
              cyc_count_B = -(sint32)delta;
        }
    }

    if(previous_C < 0 && ic >= 0)
    {
        if(cyc_count_C > params->Current_Offset_Comp_Full_Cycle_Cnt)
        {
              delta = previous_C /(previous_C - ic);  // positive by definition -- denominator is always different from zero by definition
              Cur_Offset_Ic = sum_ic / (cyc_count_C + delta);
              sum_ic = 0.0f;
              cyc_count_C = -(sint32)delta;
        }
    }


    sum_ia += ia;
    sum_ib += ib;
    sum_ic += ic;
    cyc_count_A +=1;
    cyc_count_B +=1;
    cyc_count_C +=1;

    previous_A = ia;
    previous_B = ib;
    previous_C = ic;

    Cur_Offset_Ia =  MATHCALC__SATURATE_DIRECT(-max_current_compensation, Cur_Offset_Ia, max_current_compensation);
    Cur_Offset_Ib =  MATHCALC__SATURATE_DIRECT(-max_current_compensation, Cur_Offset_Ib, max_current_compensation);
    Cur_Offset_Ic =  MATHCALC__SATURATE_DIRECT(-max_current_compensation, Cur_Offset_Ic, max_current_compensation);

    if(Cur_Offset_Comp_Factor)
    {
        io->Is_ABC->A = (io->Is_ABC->A) - ((Cur_Offset_Ia * Cur_Offset_Comp_Factor));
        io->Is_ABC->B = (io->Is_ABC->B) - ((Cur_Offset_Ib * Cur_Offset_Comp_Factor));
        io->Is_ABC->C = (io->Is_ABC->C) - ((Cur_Offset_Ic * Cur_Offset_Comp_Factor));
    }
}


static void CurrentOffSetLogicEnable(MCL_INPUT_PROC_IO_F_TYPE *io, MCL_INPUT_PROC_JOINT_PARAMS_TYPE *params)
{
    float32 temp_abs_cur;
    float32 temp_abs_speed;

    temp_abs_cur = *(io->Ctrl_Specific->Is_Abs);
    temp_abs_speed = *(io->Ctrl_Specific->Speed_Rot_Ref_Abs);

    if(params->InputProcPrm->Current_Offset_Comp_Speed_Thr != 0)
    {
        if((temp_abs_speed >= Speed_Threshold_High_CurOffComp) &&(temp_abs_cur > params->InputProcAddPrm->Current_Offset_Comp_Current_Thr))
        {
            Cur_Offset_Compensation = TRUE;
        }
        if(temp_abs_speed <= Speed_Threshold_Low_CurOffComp)
        {
            Cur_Offset_Compensation = FALSE;
        }
    }
    else
    {
        Cur_Offset_Compensation = FALSE;
    }

    if(Cur_Offset_Compensation == FALSE)
    {
        Cur_Offset_Comp_Factor -= 0.0005f;  // At the rate of 1 ms, 1.0f will take almost 2 sec

        if(Cur_Offset_Comp_Factor <0.0f)
        {
            Cur_Offset_Comp_Factor = 0.0f;
        }
    }
    else
    {
        Cur_Offset_Comp_Factor += 0.0005f;
        if(Cur_Offset_Comp_Factor > 1.0f)
        {
            Cur_Offset_Comp_Factor = 1.0f;
        }
    }
}
#endif

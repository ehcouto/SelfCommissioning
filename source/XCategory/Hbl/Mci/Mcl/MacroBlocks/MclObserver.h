/**
 *  @file       MclObserver.h
 *
 *  @brief      Motor Control Loop 3-phase motors: Observer Macro Block.
 *
 *  @section
 *
 *  $Header:    MclObserver.h
 *
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCL_OBSERVER_H_
#define MCL_OBSERVER_H_


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MclCrossTypes.h"
#include "MclConfig.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    float32 Stator_Resistance;                      //!< [Ohm] - stator resistance
    float32 Resistance_Adapt_Corr_Limits;           //!< [%] - adapted stator resistance limits as percentage
    float32 Resistance_Adapt_Gain;                  //!< [Ohm/(s*Flux)] - stator resistance adaptation gain
    float32 Resistance_Adapt_Current_Thr;           //!< [A] - magnitude of currents to enable/disable resistance adaptation
    float32 Resistance_Adapt_Speed_Thr;             //!< [rad/s mech] - speed to enable/disable resistance adaptation
    float32 Resistance_Adapt_Offset;                //!< [Ohm] - maximum value applied by resistance adaptation
    float32 Rotor_Flux_Nominal;                     //!< [Vs/rad] - Rotor magnets flux constant
    float32 Observer_G_gain;
    float32 Observer_G_integ_gain;
    float32 Observer_G_integ_limits;
    float32 Observer_G_integ_Spd_Threshold;
    float32 Pole_Pairs;

    uint32 Pll_Starting_Threshold;                  //!< [x Ts] - count time after starting

    float32 PLL_Kp;                                 //!< pll proportional gain
    float32 PLL_Ki;                                 //!< pll integral gain
    float32 Pll_Integ_K;                            //!< pll time integration constant

} MCL_OBSERVER_PARAMS_TYPE;


typedef struct
{
    float32 Phase_Advance_Coeff;                    //!< feedback voltage phase advance coefficient
    float32 Resistance_Adapt_Recovery;              //!< [Ohm*s] - adapted stator resistance limits as percentage
    float32 Swap_Phase_Time_Thr;                    //!< [s*8000] - time threshold to enable the swap phase algorithm (0 if it is disabled)
    float32 Swap_Phase_Current_Thr;                 //!< [A] - current threshold to enable swap phase algorithm
    float32 Swap_Phase_Speed_Thr;                   //!< [rad/s mech] - speed threshold to enable swap phase algorithm
    float32 Pll_Engage_Speed_Thr;                   //!< [rad/s mech] - speed threshold to engage the PLL
    float32 Pll_Engage_Speed_Err_Thr;               //!< [rad/s mech] - error speed threshold to engage the PLL
    float32 Dummy_1;
    float32 Dummy_2;

} MCL_OBSERVER_ADDITIONAL_PARAMS_TYPE;


typedef struct
{
    MCL_OBSERVER_PARAMS_TYPE     *ObsPrm;
    MCL_OBSERVER_ADDITIONAL_PARAMS_TYPE     *ObsAddPrm;
    MATHCALC_LUT_EXT_F_TYPE       Ld_lut;
    MATHCALC_LUT_EXT_F_TYPE       Lq_lut;
    MATHCALC_LUT_EXT_F_TYPE       Ldq_lut;
    MATHCALC_LUT_EXT_F_TYPE       Rotor_Flux_lut;
}MCLOBSERVER_JOINT_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MclObserver__ResetState(MCL_OBSERVER_IO_F_TYPE *io, MCLOBSERVER_JOINT_PARAMS_TYPE *params);
void MclObserver__Initialize(MCL_OBSERVER_IO_F_TYPE *io, MCLOBSERVER_JOINT_PARAMS_TYPE *params);
void MclObserver__RunningHandler(MCL_OBSERVER_IO_F_TYPE *io, MCLOBSERVER_JOINT_PARAMS_TYPE *params);
void MclObserver__1msHandler(MCL_OBSERVER_IO_F_TYPE *io, MCLOBSERVER_JOINT_PARAMS_TYPE *params);
void MclObserver__UpdateResistance25ms(sint32 updated_resistance);

#endif // MCL_OBSERVER_H_

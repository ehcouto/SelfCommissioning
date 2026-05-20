/**
 *  @file       MclDqRef.h
 *
 *  @brief      Motor Control Loop 3-phase motors: DQ Reference Generator Macro Block.
 *
 *  @section
 *
 *  $Header:    MclDqRef.h
 *
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCL_DQ_REF_H_
#define MCL_DQ_REF_H_


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MclCrossTypes.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    float32 Rotor_Flux_Nominal;                     //!< [Vs/rad] - rotor flux
    float32 Max_Flux;                               //!< [Vs/rad] - Max abs flux; sqrt((Ls*Ismax)^2 + LambdaM^2)*K
    uint32 Starting_Threshold;                      //!< [x Ts] - count time after starting
    float32 Over_Flux_Decrement;                    //!< [Vs/rad] - decrement rate while forced flux is engaged
    float32 Over_Flux_Speed_Threshold;              //!< [rpm] - Speed limit where overflux is applied
    float32 Over_Flux_Ratio_Max;                    //!< [%] - Over flux percentage during starting
    float32 Torque_Err_K;                           //!< K for torque error compensation
    float32 Torque_Err_Sat;                         //!< torque error compensation saturation value
    float32 m_index;                                //!< modulation index limitation
    float32  K_torque_inv;                          //!< 1/Ktorque constant [Nm/A] - equal to 1.5*p*Rotor_Lambda_M
} MCL_DQ_REF_PARAMS_TYPE;


typedef struct
{
    struct
    {
        sint32 Braking_Mode;                        //!< [NA] - Mode = 0 -> CONSERVATIVE Variant, 1 = NORMAL variant
        float32 Id_Braking_Prop_Gain;               //!< [NA] - Prop gain to compensate the error between the real Id and the reference Id during the braking
        float32 Id_Braking_Step_Back;               //!< [A/ms] - 1ms Current step to decrease Id_braking to zero when the active braking is off
    }Braking_Parameters;

    float32 Overflux_Charging;                      //!< [%] - Increase the starting flux factor towards the maximum in a rate of 125us
    float32 Overflux_Min_Perc;                      //!< [%] - Minimum overflux factor
    float32 Flux_Adaptation_G_Gain;                 //!< [Wb/(A*s)] - Flux adaptation G Integ gain
    float32 Flux_Adaptation_Low_Limit;              //!< [Wb] - Flux adaptation Low Limit
    float32 Flux_Adaptation_High_Limit;             //!< [Wb] - Flux adaptation High Limit
    float32 Dummy_1;
    float32 Dummy_2;
} MCL_DQ_REF_ADDITIONAL_PARAMS_TYPE;


typedef struct{
    MCL_DQ_REF_PARAMS_TYPE  *DqRefPrm;
    MCL_DQ_REF_ADDITIONAL_PARAMS_TYPE *DqRefAddPrm;
    MATHCALC_LUT_EXT_F_TYPE   MTPAPrm;
} MCLDQREF_JOINT_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MclDqRef__ResetState(MCLDQREF_JOINT_PARAMS_TYPE *params);
void MclDqRef__Initialize(void);
void MclDqRef__RunningHandler(MCL_DQ_REF_IO_F_TYPE *io, MCLDQREF_JOINT_PARAMS_TYPE *params);
void MclDqRef__1msHandler(MCL_DQ_REF_IO_F_TYPE *io, MCLDQREF_JOINT_PARAMS_TYPE *params);

#endif // MCL_DQ_REF_H_

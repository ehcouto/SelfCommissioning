/**
 *  @file       MclSpeedCtrl.h
 *
 *  @brief      Motor Control Loop 3-phase motors: Speed Controller Macro Block.
 *
 *  @section
 *
 *  $Header:    MclSpeedCtrl.h
 *
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCL_SPEED_CTRL_H_
#define MCL_SPEED_CTRL_H_


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MclCrossTypes.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Define the type of variables used by speed controller.
typedef struct
{
    float32 Max_Squared_Inv_Current;                //!< [A^2] - Maximum current that inverter is able to deliver
    float32 Max_Design_Motor_Torque;                //!< [Nm] - Maximum torque according to the motor design
    float32 Rotor_Flux_Nominal;                     //!< [Vs/rad] - Rotor magnets flux constant
    uint32 Force_Default_Gains_Time;                //!< [x Ts] - time after starting to force the default gains

    struct
    {
        float32 Max_Cos;                            //!< [n/a] - define cosine of maximum load angle
        float32 Kp;
        float32 Ki;
    } load_angle;

    struct
    {
        float32 kp;                                 //!< define the speed kp gain
        float32 ki;                                 //!< define the speed ki gain
        float32 speed_abs;                          //!< define below what speed the kp and ki are used, this one
                                                    //!< defines which set of gains are valid!
        sint32 transition_time;                     //!< [x 25ms] - Time to transitate between regions
    } speed_gains_table[SPEEDCTRL__SPEED_REGIONS];  //!< speed gains table, used for speed gains scheduling

    struct
    {
        float32 Max_Braking_Diss_Current;           //!< [A] - Max braking current
        float32 Max_Braking_Diss_Torque;            //!< [Nm] - Max Braking Torque available
    }Braking_Parameters;

    float32  pole_pairs_factor;
} MCL_SPEED_CTRL_PARAMS_TYPE;


typedef struct
{
    struct
    {
        float32 Min_Braking_Diss_Torque_Low_Thr;        //!< [Nm] - Min braking torque when dissipative braking is active - low threshold
        float32 Min_Braking_Diss_Torque_High_Thr;       //!< [Nm] - Min braking torque when dissipative braking is active - high threshold
        float32 Max_Braking_Gen_Low_Speed;              //!< [Nm] - Max braking torque when generative braking is active at low speed
        float32 Max_Braking_Gen_High_Speed;             //!< [Nm] - Max braking torque when generative braking is active at low speed
        float32 Max_Braking_Gen_Low_High_Speed_Thr;     //!< [Nm] - Speed threshold between high and low region to apply the max braking generative torque
        float32 Braking_Resistance_Percentage;          //!< [%] - resistance percentage to be used for braking dissipative power calculation
        uint32 Braking_Mode;                            //!< [NA] - Mode = 0 -> CONSERVATIVE Variant, 1 = NORMAL variant
    }Braking_Parameters;

    float32 Dummy_1;
    float32 Dummy_2;
} MCL_SPEED_CTRL_ADDITIONAL_PARAMS_TYPE;


typedef struct
{
    MCL_SPEED_CTRL_PARAMS_TYPE  *SpeedCtrlPrm;
    MCL_SPEED_CTRL_ADDITIONAL_PARAMS_TYPE  *SpeedCtrlAddPrm;
    MATHCALC_LUT_EXT_F_TYPE       MTPVPrm;
}MCLSPEEDCTRL_JOINT_PARAMS_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MclSpeedCtrl__ResetState(MCL_SPEED_CTRL_IO_F_TYPE *io,MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params);
void MclSpeedCtrl__Initialize(MCL_SPEED_CTRL_IO_F_TYPE *io,MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params);
void MclSpeedCtrl__RunningHandler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE* params);
void MclSpeedCtrl__1msHandler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE* params);
void MclSpeedCtrl__25msHandler(MCL_SPEED_CTRL_IO_F_TYPE *io, MCLSPEEDCTRL_JOINT_PARAMS_TYPE* params);

BOOL_TYPE MclSpeedCtrl__SetSpeedRegLimit(sint32 value);
BOOL_TYPE MclSpeedCtrl__SetSpeedGainIndex(sint32 speed_gains_index, MCLSPEEDCTRL_JOINT_PARAMS_TYPE *params);
BOOL_TYPE MclSpeedCtrl__SetDeltaSpeedRef(sint32 delta_speed_x65536, MCL_SPEED_CTRL_IO_F_TYPE *io);

#endif // MCL_SPEED_CTRL_H_

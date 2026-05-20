/**
 *  @file
 *  @brief       Foc Loop Paramters CIM 3-phase motors for Self Commissioning.
 *  @details     This module defines all paramaters of the field oriented control loop for CIM motors.
 *  @author      alessio.beato/luigi.fagnano  (only temporary, since it is not integrated in MKS)
 *  $Header: FOC/ScBpm_prm.h 
 * @copyright Copyright 2012 - $Date: 2015/08/17 23:01:27CEST $. Whirlpool Corporation. All rights reserved ? CONFIDENTIAL
*/
/*
 *
 *---------------------------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//Setting File Parameters WM-BPM H53
//Author Luigi Fagnano
//Created in  25/07/2018 10:34:26 


#ifndef FSBPM_PRM_H_
#define FSBPM_PRM_H_

#include "MathCalc.h"

#define SC_INVERTER_LOSS_COMP   1
#define SC_DEADTIME_COMP        2

#define MOTOR_HA_WM_SBPM        1
#define MOTOR_DW_PMSM_PUMP      2


#define SC_MOTOR_TYPE           MOTOR_HA_WM_SBPM


#if (SC_MOTOR_TYPE == MOTOR_HA_WM_SBPM)
#define SC_POLE_PAIRS                   4.0f

#define SC_RS                           5.0f

#define SC_LD                           20.0f
#define SC_LQ                           20.0f

#define SC_TS                           0.0000625f
#define SC_INVERTER_COMPENSATION        SC_INVERTER_LOSS_COMP

#define SC_DEFAULT_SPEED                500 // [rpm]      100 SBPM
#define SC_DEFAULT_ACCEL                500  // [rpm/s]   100 SBPM

#define SC_OPENLOOP_CURRENT             0.1f // [A]
#define SC_CLOSELOOP_SPEED              200.0f // [rpm]

#define SC_SPEEDCONTROLLER_KP           1.0f
#define SC_SPEEDCONTROLLER_KI           50.0f
#define SC_SPEEDCONTROLLER_LIMIT        6.0f // [A]    6.0f SBPM

#define SC_TRACKINGOBSERVER_KP          100.0f
#define SC_TRACKINGOBSERVER_KI          2000.0f

#define SC_CURRENTCONTROLLER_KP         62.0f // 32.0f SBPM
#define SC_CURRENTCONTROLLER_KI         9500.0f // 355.0f SBPM


#else
#define SC_POLE_PAIRS                   3.0f

#define SC_RS                           35.0f

#define SC_LD                           90.0f
#define SC_LQ                           90.0f

#define SC_TS                           0.0001f
#define SC_INVERTER_COMPENSATION        SC_DEADTIME_COMP

#define SC_DEFAULT_SPEED                2000 // [rpm]      100 SBPM
#define SC_DEFAULT_ACCEL                500  // [rpm/s]    10 SBPM

#define SC_OPENLOOP_CURRENT             0.1f // [A]
#define SC_CLOSELOOP_SPEED              1000.0f // [rpm]

#define SC_SPEEDCONTROLLER_KP           0.4f // 50.0f SBPM
#define SC_SPEEDCONTROLLER_KI           1.0f // 250.0f SBPM
#define SC_SPEEDCONTROLLER_LIMIT        0.8f // [A]    6.0f SBPM

#define SC_TRACKINGOBSERVER_KP          64.0f // 32.0f SBPM
#define SC_TRACKINGOBSERVER_KI          700.0f // 355.0f SBPM

#define SC_CURRENTCONTROLLER_KP         19.3f // 32.0f SBPM
#define SC_CURRENTCONTROLLER_KI         6613.0f // 355.0f SBPM


#endif
MATHCALC_LUT_F_TYPE Sc_Dutycycle_By_Current_LUT[] =
{
         // Input current            Output - Ratio to scale in dc bus
        { -4.000f,       -0.0232194961800000f  },
        { -3.875f,       -0.0232327112901724f  },
        { -3.750f,       -0.0231836238555470f  },
        { -3.625f,       -0.0231014283579975f  },
        { -3.500f,       -0.0230069008260000f  },
        { -3.375f,       -0.0229132005052368f  },
        { -3.250f,       -0.0228266715292032f  },
        { -3.125f,       -0.0227476445898120f  },
        { -3.000f,       -0.0226712386080000f  },
        { -2.875f,       -0.0225881624043325f  },
        { -2.750f,       -0.0224855163696094f  },
        { -2.625f,       -0.0223475941354703f  },
        { -2.500f,       -0.0221566842450000f  },
        { -2.375f,       -0.0218938718233345f  },
        { -2.250f,       -0.0215398402482657f  },
        { -2.125f,       -0.0210756728208472f  },
        { -2.000f,       -0.0204836544360000f  },
        { -1.875f,       -0.0197480732531177f  },
        { -1.750f,       -0.0188560223666718f  },
        { -1.625f,       -0.0182176406315157f  },
        { -1.500f,       -0.0175060990260000f  },
        { -1.375f,       -0.0169285195539843f  },
        { -1.250f,       -0.0163874304123750f  },
        { -1.125f,       -0.0157853597980782f  },
        { -1.000f,       -0.0150248359080000f  },
        { -0.875f,       -0.0140360554949062f  },
        { -0.750f,       -0.0137160000000000f  },
        { -0.625f,       -0.0135000000000000f  },
        { -0.500f,       -0.0132840000000000f  },
        { -0.375f,       -0.0129600000000000f  },
        { -0.250f,       -0.0118800000000000f  },
        { -0.125f,       -0.0108000000000000f  },
        {  0.000f,        0.0000000000000000f  },
        {  0.125f,        0.0108000000000000f  },
        {  0.250f,        0.0118800000000000f  },
        {  0.375f,        0.0129600000000000f  },
        {  0.500f,        0.0133920000000000f  },
        {  0.625f,        0.0136080000000000f  },
        {  0.750f,        0.0150983233189453f  },
        {  0.875f,        0.0162392548183867f  },
        {  1.000f,        0.0168092172000000f  },
        {  1.125f,        0.0174608718187500f  },
        {  1.250f,        0.0182512662750000f  },
        {  1.375f,        0.0191288643187500f  },
        {  1.500f,        0.0200421297000000f  },
        {  1.625f,        0.0209395261687500f  },
        {  1.750f,        0.0209784249210936f  },
        {  1.875f,        0.0219663169543213f  },
        {  2.000f,        0.0225600984000000f  },
        {  2.125f,        0.0228955182853270f  },
        {  2.250f,        0.0230768420132812f  },
        {  2.375f,        0.0231805799729735f  },
        {  2.500f,        0.0232592161499999f  },
        {  2.625f,        0.0233449367367920f  },
        {  2.750f,        0.0234533587429686f  },
        {  2.875f,        0.0235872586056885f  },
        {  3.000f,        0.0237403007999998f  },
        {  3.125f,        0.0239007664491945f  },
        {  3.250f,        0.0240552819351561f  },
        {  3.375f,        0.0241925475087159f  },
        {  3.500f,        0.0243070658999994f  },
        {  3.625f,        0.0244028709287834f  },
        {  3.750f,        0.0244972561148435f  },
        {  3.875f,        0.0246245032883053f  },
        {  4.000f,        0.0248396111999998f  },
};


float32 sc_step_inv_pwm_comp   = 1.0f/0.125f;
sint32 sc_sizeof_inv_comp_lut = sizeof(Sc_Dutycycle_By_Current_LUT)/sizeof(MATHCALC_LUT_F_TYPE);

#endif // SCPBPM_PRM_H_

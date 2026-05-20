/**
 *  @file        ApplicationParams_prv.h
 *  @brief       Application Parameters for 3-phase motors.
 *
 *               ==== AUTOMATIC GENERATED FILE! DO NOT TOUCH IT, USE EXCEL MACRO! ====
 *
 *  @details     This module defines all application parameters. Created in 20/11/2017 15:20:30
 *               Setting File Parameters for 
 *  @copyright   Copyright 2016.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL
 *
 */

#ifndef APPLICATIONPARAMS_PRV_H
#define APPLICATIONPARAMS_PRV_H


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#include "Mci_prm.h" 


#ifdef INTERNAL_PARAMS_INDESIT_BD
//Motor Control Application Parameters

//Algorithm 
//Algo Frequency = 200 Hz

//Speed Controller (SC)
//Threshold Wash = 60 rpm_D
//Threshold Distribution = 88 rpm_D
//Threshold Spin = 1280 rpm_D
//Max Speed Motor = 19800 rpm_M
//Kp (rpm<wash) = 8 Nmm/rpm
//Ki (rpm<wash) = 6.5 Nmm/(rpm s)
//Kp (rpm>wash) = 1.3 Nmm/rpm
//Ki(rpm>wash) = 5.9 Nmm/(rpm s)
//Kp (rpm<spin) = 1.3 Nmm/rpm
//Ki (rpm<spin) = 5.9 Nmm/(rpm s)
//Kp (rpm>spin) = 0.5 Nmm/rpm
//Ki (rpm>spin) = 1.9 Nmm/(rpm s)
//Transmission Ratio = 11.7 adm

//Is Max
//Is Max Spin = 0 Apeak
//Is Max Tumbling = 7 Apeak
//Is Max Thr Speed = 6000 rpm_M


signed short int Command_Params_SF[10] =
{
/*Transmission_Ratio = Transmission Ratio * 256 =*/                     2995,\
/*Free_Down_Gain = Algo Frequency /(Threshold Wash * Transmission Ratio)  *32768 =*/                     9336,\
/*Max_Allowed_Speed = 0.93 * Base Speed / Transmission Ratio =*/                     174,\
/*Low_Regulators_Thr_Speed = Threshold Wash * Transmission Ratio =*/                     702,\
/*Med_Regulators_Thr_Speed = Threshold Distribution * Transmission Ratio  =*/                     1030,\
/*High_Regulators_Thr_Speed = Threshold Spin * Transmission Ratio  =*/                     14976,\
/*Max_Deceleration_Low_Speed = Max Deceleration Low Speed * Transmission Ratio =*/                     585,\
/*Max_Deceleration_High_Speed = Max Deceleration High Speed * Transmission Ratio =*/                     176,\
/*Max_Acceleration_Low_Speed = Max Acceleration Low Speed * Transmission Ratio =*/                     1544,\
/*Max_Acceleration_High_Speed = Max Acceleration High Speed * Transmission Ratio =*/                     293,\
};




signed int MciSetWm_32bit_Params_SF[19] =
{
/*Low_Regulators_Thr_Speed = Threshold Wash * Transmission Ratio / SpeedBase =*/                     10520,\
/*Med_Regulators_Thr_Speed = Threshold Distribution * Transmission Ratio / SpeedBase   =*/                     15429,\
/*High_Regulators_Thr_Speed = Threshold Spin * Transmission Ratio  / SpeedBase   =*/                     224428,\
/*Very_High_Regulators_Thr_Speed = Max Speed  / SpeedBase   =*/                     296719,\
/*Prop_Gain_Speed_Low = KpSpeedLow*32768*/                     41582,\
/*Integ_Gain_Speed_Low = KiSpeedLow*2^20*/                     135,\
/*Prop_Gain_Speed_Med = KpSpeedMed*32768*/                     6757,\
/*Integ_Gain_Speed_Med = KiSpeedMed*2^20*/                     123,\
/*Prop_Gain_Speed_High = KpSpeedHigh*32768*/                     6757,\
/*Integ_Gain_Speed_High = KiSpeedHigh*2^20*/                     123,\
/*Prop_Gain_Speed_VeryHigh = KpSpeedVeryHigh*32768*/                     2599,\
/*Integ_Gain_Speed_VeryHigh = KiSpeedVeryHigh*2^20*/                     40,\
/*IsMaxThrSpeed = Ismax threshold speed  / SpeedBase   =*/                     89915,\
/*Max_Squared_Tumbling_Current = Ismax_tumbling ^2 / CurrentBase =*/                     195332,\
/*Max_Squared_Spin_Current = Ismax_spinning ^2  / CurrentBase =*/                     0,\
/*IndesitSpeedBase = */                     19800,\
/*TorqueBaseFactor = Base Torque Indesit / Base Torque DTC*/                     1049923940,\
/*   Prop_Gain_Speed_Conversion = Base Torque/ Base Speed / 32768*/                     877564957,\
/*   Integ_Gain_Speed_Conversion = Base Torque/ Base Speed / 32768 / 32 * Control Freq*/                     944356517,\
};




signed short int MciSetWm_16bit_Params_SF[41] =
{
/*SpeedLimit_0 = Speed Limit 1 * Transmission Ratio / Base Speed * 32768 =*/                     6777,\
/*SpeedLimit_1 = Speed Limit 2 * Transmission Ratio / Base Speed * 32768 =*/                     11811,\
/*SpeedLimit_2 = Speed Limit 3 * Transmission Ratio / Base Speed * 32768 =*/                     19557,\
/*SpeedLimit_3 = Speed Limit 4 * Transmission Ratio / Base Speed * 32768 =*/                     23429,\
/*SpeedLimit_4 = Speed Limit 5 * Transmission Ratio / Base Speed * 32768 =*/                     27302,\
/*SpeedLimit_5 = Speed Limit 6 * Transmission Ratio / Base Speed * 32768 =*/                     0,\
/*SpeedLimit_6 = Speed Limit 7 * Transmission Ratio / Base Speed * 32768 =*/                     0,\
/*SpeedLimit_7 = Speed Limit 8 * Transmission Ratio / Base Speed * 32768 =*/                     0,\
/*TorqueLimit_0 = Torque Limit 1 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     3572,\
/*TorqueLimit_1 = Torque Limit 2 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     3572,\
/*TorqueLimit_2 = Torque Limit 3 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     2541,\
/*TorqueLimit_3 = Torque Limit 4 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     2301,\
/*TorqueLimit_4 = Torque Limit 5 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     2816,\
/*TorqueLimit_5 = Torque Limit 6 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     0,\
/*TorqueLimit_6 = Torque Limit 7 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     0,\
/*TorqueLimit_7 = Torque Limit 8 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     0,\
/*TorqueLimit_Norm_0 = Torque Limit Norm 1 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     3572,\
/*TorqueLimit_Norm_1 = Torque Limit Norm 2 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     3572,\
/*TorqueLimit_Norm_2 = Torque Limit Norm 3 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     2541,\
/*TorqueLimit_Norm_3 = Torque Limit Norm 4 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     2301,\
/*TorqueLimit_Norm_4 = Torque Limit Norm 5 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     2816,\
/*TorqueLimit_Norm_5 = Torque Limit Norm 6 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     0,\
/*TorqueLimit_Norm_6 = Torque Limit Norm 7 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     0,\
/*TorqueLimit_Norm_7 = Torque Limit Norm 8 / (Base Torque[Kg cm_M]  *  Trasmission Ratio) * 32768 =*/                     0,\
/*Mlimit_0 = 0*/                     0,\
/*Mlimit_1 = se(Speed Limit 2 -Speed Limit 1)>0;(Torque Limit 2 - Torque Limit 1)/(Speed Limit 2 -Speed Limit 1) * Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     0,\
/*Mlimit_2 = se(Speed Limit 3 -Speed Limit 2>0;((Torque Limit 3 - Torque Limit 2)/(Speed Limit 3 -Speed Limit 2) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     -4359,\
/*Mlimit_3 = se(Speed Limit 4 -Speed Limit 3>0;((Torque Limit 4 - Torque Limit 3)/(Speed Limit 4 -Speed Limit 3) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     -2034,\
/*Mlimit_4 = se(Speed Limit 5 -Speed Limit 4>0;((Torque Limit 5 - Torque Limit 4)/(Speed Limit 5 -Speed Limit 4) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     4359,\
/*Mlimit_5 = se(Speed Limit 6 -Speed Limit 5>0;((Torque Limit 6 - Torque Limit 5)/(Speed Limit 6 -Speed Limit 7) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     0,\
/*Mlimit_6 = se(Speed Limit 7 -Speed Limit 6>0;((Torque Limit 7 - Torque Limit 6)/(Speed Limit 7 -Speed Limit 6) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     0,\
/*Mlimit_7 = se(Speed Limit 8 -Speed Limit 7>0;((Torque Limit 8 - Torque Limit 7)/(Speed Limit 8 -Speed Limit 7) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     0,\
/*Mlimit_Norm_0 = 0*/                     0,\
/*Mlimit_Norm_1 = se(Speed Limit 2 -Speed Limit 1)>0;(Torque Limit Norm 2 - Torque Limit Norm 1)/(Speed Limit 2 -Speed Limit 1) * Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     0,\
/*Mlimit_Norm_2 = se(Speed Limit 3 -Speed Limit 2>0;((Torque Limit Norm 3 - Torque Limit Norm 2)/(Speed Limit 3 -Speed Limit 2) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     -4359,\
/*Mlimit_Norm_3 = se(Speed Limit 4 -Speed Limit 3>0;((Torque Limit Norm 4 - Torque Limit Norm 3)/(Speed Limit 4 -Speed Limit 3) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     -2034,\
/*Mlimit_Norm_4 = se(Speed Limit 5 -Speed Limit 4>0;((Torque Limit Norm 5 - Torque Limit Norm 4)/(Speed Limit 5 -Speed Limit 4) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     4359,\
/*Mlimit_Norm_5 = se(Speed Limit 6 -Speed Limit 5>0;((Torque Limit Norm 6 - Torque Limit Norm 5)/(Speed Limit 6 -Speed Limit 7) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     0,\
/*Mlimit_Norm_6 = se(Speed Limit 7 -Speed Limit 6>0;((Torque Limit Norm 7 - Torque Limit Norm 6)/(Speed Limit 7 -Speed Limit 6) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     0,\
/*Mlimit_Norm_7 = se(Speed Limit 8 -Speed Limit 7>0;((Torque Limit Norm 8 - Torque Limit Norm 7)/(Speed Limit 8 -Speed Limit 7) *Base Speed /(Base Torque[Kg cm_M] * Trasmission Ratio^2)/ * 32768;0) =*/                     0,\
/*Vdc_Norm = Vdc Normativa *1.414 =*/                     311,\
};


signed short int MciSensorsWm_Params_SF[3] =
{
/*K_Turns  = 2^15 / 60 / Turns Base /Algo Frequency * 32768 = Speedbase[Hz] / 128/ Algo Frequency* 32768 =*/                     699,\
/*Mean_Over_Drum_Rev = 1.0 * Transmission Ratio / Turns Base * 32768 =1.0 * Transmission Ratio / 128 * 32768*/                     2995,\
/*Spinning_Thr_Speed = Threshold Distribution * Transmission Ratio  =*/                     1030,\
};


#endif //


#endif //APPLICATIONPARAMS_PRV_H

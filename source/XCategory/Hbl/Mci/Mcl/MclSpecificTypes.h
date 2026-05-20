/*
 * MclSpecificTypes.h
 *
 *  Created on: 13 apr 2018
 *      Author: lab.motori
 */

#ifndef MCLSPECIFICTYPES_H_
#define MCLSPECIFICTYPES_H_
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MclBasicTypes.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================




typedef union
{
    struct
    {
        uint32 params_loaded               :  1;    //!< parameters are loaded
        uint32 params_ready				   :  1;    //!< parameters are ready to be loaded
        uint32 wrong_speed_direction       :  1;    //!< 1 --> wrong speed direction
        uint32 pll_engaged                 :  1;    //!< 1 --> PLL is engaged and used as observer speed
        uint32 wait_pll_engagement         :  1;    //!< 1 --> waiting for PLL engagement (check blocked rotor condition)
        uint32 resistance_est_error        :  1;    //!< 1 --> timeout error during resistance estimation procedure (probably due to motor disconnected)
        uint32 overflux_active             :  1;    //!< 1 --> overflux is active
        uint32 braking_active              :  1;    //!< 1 --> braking is active
        uint32 motor_direction             :  1;    //!< 1 --> positive, 0 --> negative
        uint32 stopped_ramp                :  1;    //!< 1 --> ramp is stopped (i.e. waiting for end of braking during speed inversion)
        uint32 reserved                    : 21;    //!< Reserved bits
    } bit;
    uint32 all;                                     //!< Mcl Flags
} MCL_FLAGS_TYPE;                                      //!< Type containing flags for control specific purposes


typedef struct
{
    MCL_FLAGS_TYPE flags;
    float32 *Speed_Rot_Ref_Abs;                      //!< [rad/s] - absolute speed reference
    float32 *Speed_Rotor_Observed;                   //!< [rad/s] - observed rotor speed
    float32 *Speed_Rotor_Observed_Mech;              //!< [rad/s] - observed rotor speed mechanical
    float32 *Speed_Rotor_Observed_Mech_Abs;          //!< [rad/s] - observed rotor absolute speed
    uint32 *Elapsed_Time_After_Starting;             //!< [x Ts]  - count time after starting
    float32 *Stator_Resistance;                      //!< [ohm]   - stator resistance
    float32 *Id_stator_current;                      //!< [A]     - stator direct axis current
    float32 *Iq_stator_current;                      //!< [A]     - stator quadrature axis current
    float32 *Id_Rotor_Current;                       //!< [A]     - rotor direct axis current
    float32 *Torque;                                 //!< [Nm]    - torque value
    float32 *Vs_Flux;                                //!< [V]     - voltage available for flux weakening calculation
    float32 *Flux_Ref;                               //!< [Vs]    - flux reference value
    float32 *Flux_Limit_By_Speed;                    //!< [Vs]    - flux limited by speed (field weakening operation)
    float32 *Stator_Flux_Mag;                        //!< [Vs]    - stator flux magnitude
    float32 *DcInjection_Angle;                      //!< [rad]   - injected angle during startup
    SIN_COS_F_TYPE *Load_Angle_Sin_Cos;              //!< []      - sin/cos of load angle
    DQ_COOR_SYST_F_TYPE *Ldq_Ind;                    //!< [H]     - Ld and Lq inductances
    float32 *Ldq_Cross_Ind;                          //!< [H]     - cross saturation from q do the d axis
    float32 *Lqd_Cross_Ind;                          //!< [H]     - cross saturation from d do the q axis
    float32 *Is_Abs;                                 //!< [A]     - motor current absolute value
    float32 *Rotor_Flux;                             //!< [Wb]    - rotor magnet flux
    float32 *Delta_Speed_Rot_Ref;                    //!< [rad/s] - delta rotor speed to be added to the reference value

    uint8 *Lowers_On;                                //!< [n/a] Turn on all lowers IGBTs
    uint8 *All_Devices_Off;                          //!< [n/a] Turn off all gates of IGBTs
} MCL_CS_TYPE;



typedef struct
{
    float Speed;
    float Radial;
    float Torque;
    float Flux;
    float DC_Bus_Voltage;
    float Phase_Current;
    float Angle;
    float Inductance;
} MCL_BASES_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================




#endif /* MCLSPECIFICTYPES_H_ */


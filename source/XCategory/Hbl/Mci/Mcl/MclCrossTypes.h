/*
 * MclCrossTypes.h
 *
 *  Created on: 12 apr 2018
 *      Author: lab.motori
 */

#ifndef MCLCROSSTYPES_H_
#define MCLCROSSTYPES_H_

#include "MclBasicTypes.h"
#include "MclSpecificTypes.h"

//! Motor control macro block - INPUT PROCESSING type definition
typedef struct
{
    //MclInputProcessing Input
    sint32 *Vdc;                                        //!< [V] Q17.15 - DC-Bus Voltage
    ABC_COOR_SYST_TYPE *Duty;                           //!< [%] Q17.15 - PWM Applied Duties before feedforward compensation
    ABC_COOR_SYST_TYPE *Is_ABC;                         //!< [A] Q17.15 - Motor Phase Currents


    //MclInputProcessing Output
    ALPHA_BETA_COOR_SYST_TYPE *Is_Alpha_Beta;           //!< [A] Q17.15 - Motor phase current in alpha-beta reference system
    ALPHA_BETA_COOR_SYST_TYPE *Vs_Alpha_Beta_Rec;       //!< [V] Q17.15 - Motor phase voltage reconstructed in alpha-beta reference system
    ABC_COOR_SYST_TYPE *Vabc_Rec;                       //!< [V] Q17.15 - Motor phase voltage reconstructed in abc system
    sint32 *Vs_Max_Filt;                                //!< [V] Q15.15 - Mininum Dc bus voltage filtered
    sint32 *Is_Abs;                                     //!< [A] Q17.15 - Motor stator current magnitude (modulus)


    //Control specific
    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_INPUT_PROC_IO_TYPE;


//! Motor control macro block - INPUT PROCESSING type definition - floating point
typedef struct
{
    //MclInputProcessing Input
    float32 *Vdc;                                       //!< [V] - DC-Bus Voltage
    ABC_COOR_SYST_F_TYPE *Duty;                         //!< [%] - PWM Applied Duties before feedforward compensation
    ABC_COOR_SYST_F_TYPE *Is_ABC;                       //!< [A] - Motor Phase Currents


    //MclInputProcessing Output
    ALPHA_BETA_COOR_SYST_F_TYPE *Is_Alpha_Beta;         //!< [A] - Motor phase current in alpha-beta reference system
    ALPHA_BETA_COOR_SYST_F_TYPE *Vs_Alpha_Beta_Rec;     //!< [V] - Motor phase voltage reconstructed in alpha-beta reference system
    ABC_COOR_SYST_F_TYPE *Vabc_Rec;                     //!< [V] - Motor phase voltage reconstructed in abc system
    float32 *Vs_Max_Filt;                               //!< [V] - Mininum Dc bus voltage filtered


    //Control specific
    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_INPUT_PROC_IO_F_TYPE;



//! Motor control macro block - OBSERVER type definition
typedef struct
{

    // MclObserver Input
    ALPHA_BETA_COOR_SYST_TYPE *Is_Alpha_Beta;           //!< [A] Q17.15 - motor phase current in alpha-beta reference system
    ALPHA_BETA_COOR_SYST_TYPE *Vs_Alpha_Beta;           //!< [V] Q17.15 - motor phase voltage reconstructed in alpha-beta reference system
    ABC_COOR_SYST_TYPE *Is_ABC;                         //!< [A] Q17.15 - Motor Phase Currents
    sint32 *Vdc;                                        //!< [V] Q17.15 - DC-Bus Voltage
    sint32 *Is_Abs;                                     //!< [A] Q17.15 - Motor stator current magnitude (modulus)

    // MclObserver Output
    SIN_COS_TYPE *Sin_Cos_Position_Flux;                   //!< sin/cos of rotor-flux position used in current park transformation
    sint32 *Speed_Rotor_Observed;                       //!< [rad/s or rpm] - Q17.15 - observed rotor speed
    sint32 *Rotor_Position;                             //!< Q17.15 - Rotor Position in rad/s
    sint16 *Load_Angle;                                 //!< [rad] Q1.15 - load angle


    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_OBSERVER_IO_TYPE;




//! Motor control macro block - OBSERVER type definition
typedef struct
{

    // MclObserver Input
    ALPHA_BETA_COOR_SYST_F_TYPE *Is_Alpha_Beta;         //!< [A] - motor phase current in alpha-beta reference system
    ALPHA_BETA_COOR_SYST_F_TYPE *Vs_Alpha_Beta;         //!< [V] - motor phase voltage reconstructed in alpha-beta reference system
    ABC_COOR_SYST_F_TYPE *Is_ABC;                       //!< [A] - Motor Phase Currents
    float32 *Vdc;                                       //!< [V] - DC-Bus Voltage

    // MclObserver Output
    SIN_COS_F_TYPE *Sin_Cos_Position_Flux;                 //!< sin/cos of rotor-flux position used in current park transformation
    float32 *Speed_Rotor_Observed;                      //!< [rad/s] - observed rotor speed electrical
    float32 *Rotor_Position;                            //!< [rad] - Rotor Position in rad/s
    float32 *Load_Angle;                                //!< [rad] - load angle


    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_OBSERVER_IO_F_TYPE;





//! Motor control macro block - SPEED CONTROLLER type definition
typedef struct
{
    // MclSpeedCtrl Input
    sint32 *Speed_Rot_Ref;                              //!< Q17.15 - rotor speed reference value
    sint32 *Speed_Rot;                                  //!< Q17.15 - rotor speed value


    // MclSpeedCtrl Output
    sint32 *Torque_Ref;                                 //!< Q17.15 - Torque Reference value


    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_SPEED_CTRL_IO_TYPE;




//! Motor control macro block - SPEED CONTROLLER type definition
typedef struct
{
    // MclSpeedCtrl Input
    float32 *Speed_Rot_Ref;                             //!< rotor speed reference value
    float32 *Speed_Rot;                                 //!< rotor speed value


    // MclSpeedCtrl Output
    float32 *Torque_Ref;                                //!< Torque Reference value


    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_SPEED_CTRL_IO_F_TYPE;





//! Motor control macro block - DQ REFERENCE type definition
typedef struct
{
    // MclDqReference Input
    sint32 *Torque_Ref;                                 //!< [Nm] Q17.15 - Torque Reference value  it is I/O in case of DTC
    sint32 *Force_Q_Ref;                                //!< Q17.15 - Force q-axis reference value
    sint32 *Force_D_Ref;                                //!< Q17.15 - Force d-axis reference value
    sint32 *Vdc;                                        //!< [V] Q17.15 - Instantaneous DC bus voltage

    // MclDqReference Output
    sint32 *Q_Ref;                                      //!< Q17.15 - d axis reference value
    sint32 *D_Ref;                                      //!< Q17.15 - q axis reference value

    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_DQ_REF_IO_TYPE;




//! Motor control macro block - DQ REFERENCE type definition
typedef struct
{
    // MclDqReference Input
    float32 *Torque_Ref;                                //!< [Nm] Q17.15 - Torque Reference value  it is I/O in case of DTC
    float32 *Force_Q_Ref;                               //!< Force q-axis reference value
    float32 *Force_D_Ref;                               //!< Force d-axis reference value
    float32 *Vdc;                                       //!< [V] Q17.15 - Instantaneous DC bus voltage

    // MclDqReference Output
    float32 *Q_Ref;                                     //!< d axis reference value
    float32 *D_Ref;                                     //!< q axis reference value

    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_DQ_REF_IO_F_TYPE;




//! Motor control macro block - DQ CONTROLLER type definition
typedef struct
{
    //MclDQController Input
    sint32 *D_Ref;                                      //!< Q17.15 - d axis reference value
    sint32 *D_Est;                                      //!< Q17.15 - d axis feedback value
    sint32 *Q_Ref;                                      //!< Q17.15 - q axis reference value
    sint32 *Q_Est;                                      //!< Q17.15 - q axis feedback value

    SIN_COS_TYPE *Sin_Cos;                              //!< sin/cos for inverse park transformation
    DQ_COOR_SYST_TYPE *Force_Out_Volt;                  //!< [V] Q17.15 - force dq output voltage

    sint32 *Speed_Rot;                                  //!< [rad/s or rpm] Q17.15 - rotor speed value

    sint32 *Vdc;                                        //!< [V] Q17.15 -  Maximum available phase voltage


    //MclDQController Output
    DQ_COOR_SYST_TYPE *Vs_DQ;                           //!< [V] Q17.15 - motor phase voltage in dq reference system
    ALPHA_BETA_COOR_SYST_TYPE *Vs_Alpha_Beta;           //!< [V] Q17.15 - motor phase voltage in alpha-beta reference system

    //Control specific
    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_DQ_CTRL_IO_TYPE;


//! Motor control macro block - DQ CONTROLLER type definition
typedef struct
{
    //MclDQController Input
    float32 *D_Ref;                                     //!< d axis reference value
    float32 *D_Est;                                     //!< d axis feedback value
    float32 *Q_Ref;                                     //!< q axis reference value
    float32 *Q_Est;                                     //!< q axis feedback value

    SIN_COS_F_TYPE *Sin_Cos;                            //!< sin/cos for inverse park transformation
    DQ_COOR_SYST_F_TYPE *Force_Out_Volt;                //!< [V]- force dq output voltage

    float32 *Speed_Rot;                                 //!< [rad/s or rpm] - rotor speed value

    float32 *Vdc;                                       //!< [V] -  Maximum available phase voltage


    //MclDQController Output
    DQ_COOR_SYST_F_TYPE *Vs_DQ;                         //!< [V] - motor phase voltage in dq reference system
    ALPHA_BETA_COOR_SYST_F_TYPE *Vs_Alpha_Beta;         //!< [V] - motor phase voltage in alpha-beta reference system

    //Control specific
    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_DQ_CTRL_IO_F_TYPE;




//! Motor control macro block - PWM MODULATION type definition
typedef struct
{
    //MclPwm Input
    sint32 *Vdc;                                        //!< [V] Q17.15 - DC-Bus Voltage
    ALPHA_BETA_COOR_SYST_TYPE *Vs_Alpha_Beta;           //!< [V] Q17.15 - Motor phase voltage in alpha-beta reference system
    ABC_COOR_SYST_TYPE *Is_ABC;                         //!< [A] Q17.15 - Stator currents


    //MclPwm Output
    ABC_COOR_SYST_TYPE *Duty_bc;                        //!< [%] Q17.15 - PWM Duties after modulation and before compensation
    ABC_COOR_SYST_TYPE *Duty;                           //!< [%] Q17.15 - PWM Duties


    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_PWM_IO_TYPE;


//! Motor control macro block - PWM MODULATION type definition
typedef struct
{
    //MclPwm Input
    float32 *Vdc;                                       //!< [V] - DC-Bus Voltage
    ALPHA_BETA_COOR_SYST_F_TYPE *Vs_Alpha_Beta;         //!< [V] - Motor phase voltage in alpha-beta reference system
    ABC_COOR_SYST_F_TYPE *Is_ABC;                       //!< [A] - Stator currents


    //MclPwm Output
    ABC_COOR_SYST_F_TYPE *Duty_bc;                      //!< [%] - PWM Duties after modulation and before compensation
    ABC_COOR_SYST_F_TYPE *Duty;                         //!< [%] - PWM Duties

    MCL_CS_TYPE *Ctrl_Specific;                         //!< MCL control specific variables
} MCL_PWM_IO_F_TYPE;


#endif /* MCLCROSSTYPES_H_ */

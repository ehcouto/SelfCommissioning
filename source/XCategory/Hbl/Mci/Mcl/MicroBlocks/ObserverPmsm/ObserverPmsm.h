/**
 *  @file
 *  @brief       Rotor Flux Speed Position observer for PMSM motor
 *  @details     This module implements a rotor flux speed/position observer for a 3-phase PMSM motor.
 *               Fixed point implementation.
 *  @author      alessio.beato/luigi.fagnano  (only temporary, since it is not integrated in MKS)
 *  $Header: FOC/ObserverPmsm.h 1.4 2015/09/24 20:12:50CEST Luigi Fagnano (FAGNAL) Exp  $
 * @copyright Copyright 2012 - $Date: 2015/09/24 20:12:50CEST $. Whirlpool Corporation. All rights reserved – CONFIDENTIAL
*/

#ifndef OBSERVERPMSM_H_
#define OBSERVERPMSM_H_

#include "MclBasicTypes.h"
#include "Pi.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef struct
{

    //Estimated Extended BEMF - d/q
    struct
    {
        mc_sint32 D;        //!< Q2.30  - Estimated back-EMF voltage in d-axis
        mc_sint32 Q;        //!< Q2.30  - Estimated back-EMF voltage in q-axis
    } Bemf_Est;

    //Estimated Current - d/q
    struct
    {
        mc_sint32 D;        //!< Q2.30  - Estimated current in d-axis
        mc_sint32 Q;        //!< Q2.30  - Estimated current in q-axis
    } Current_Est;

    //Observer integral states for controllers
    struct
    {
        mc_sint32        State_D;        //!< Q2.30  - PI D component state variable
        mc_sint32        State_Q;        //!< Q2.30  - PI Q component state variable
    } Pi_States;

    //Observer Pi parameters
    CONTROLLER_PI_PARAMS_TYPE Pi_Parameters;

    //misalignment error of reference frame
    mc_sint16    Theta_Flux_Error;

    //PMSM motor model parameters
    mc_sint32   I_Coeff;     //!< Q1.15  -   Ld/(Ld+Ts*Rs) - Current coefficient
    mc_sint32   U_Coeff;     //!< Q1.15  -   Ts/(Ld+Ts*Rs) - Voltage coefficient
    mc_sint32   WI_Coeff;    //!< Q1.15  -   Ts*Lq/(Ld+Ts*Rs)/2 - Speed/Current coefficient
    mc_sint32   E_Coeff;     //!< Q1.15  -   Ts/(Ld+Ts*Rs)/2 - Back-EMF coefficient

}BEMF_OBSERVER_PARAMS_TYPE;  //!< Parameters and quantities of back-emf observer



typedef struct
{
    //Tracking observer integral state for controller
    mc_sint32    State_PI;        //!< Q2.30  - PI state variable

    //Tracking observer Pi parameters
    CONTROLLER_PI_PARAMS_TYPE Pi_Parameters;

    //Integrator parameter
    INTEGRATOR_PARAMS_TYPE Integ_Parameters;

    mc_sint32 Omega_Est;        //!< Q2.30  - Estimated speed
    mc_sint32 Theta_Est;        //!< Q2.30  - Estimated position

}TRACKING_OBSERVER_PARAMS_TYPE;  //!< Parameters and quantities of tracking observer




typedef struct
{
    mc_sint32   K_1;     //!< Q17.15     -1/(Ls*Iq)
    mc_sint32   K_2;     //!< Q17.15     1/(phim)

    //Startup observer integral state for controller
    mc_sint32   State_PI;        //!< Q2.30  - PI state variable

    //Startup observer Pi parameters
    CONTROLLER_PI_PARAMS_TYPE Pi_Parameters;

    //Integrator parameters
    INTEGRATOR_PARAMS_TYPE Integ_Parameters;

    mc_sint32 Theta_Est;       //!< Q2.30  - Estimated position
    mc_sint32 Theta_Est_D;     //!< Q2.30  - Estimated position
    mc_sint32 Theta_Est_Q;     //!< Q2.30  - Estimated position


    mc_sint32 Omega_Est;       //!< Q17.15  - Estimated speed
    mc_sint32 Omega_Est_D;     //!< Q17.15  - Estimated speed
    mc_sint32 Omega_Est_Q;     //!< Q17.15  - Estimated speed

}STARTUP_OBSERVER_PARAMS_TYPE;  //!< Parameters startup observer



typedef struct
{

    //Estimated Extended BEMF - d/q
    struct
    {
        float32 D;        //!< Q2.30  - Estimated back-EMF voltage in d-axis
        float32 Q;        //!< Q2.30  - Estimated back-EMF voltage in q-axis
    } Bemf_Est;

    //Estimated Current - d/q
    struct
    {
        float32 D;        //!< Q2.30  - Estimated current in d-axis
        float32 Q;        //!< Q2.30  - Estimated current in q-axis
    } Current_Est;

    //Observer Pi parameters
    PI_CONTROLLER_F_TYPE Pi_D;
    PI_CONTROLLER_F_TYPE Pi_Q;

    //misalignment error of reference frame
    float32   Theta_Flux_Error;

    //PMSM motor model parameters
    float32   I_Coeff;     // Ld/(Ld+Ts*Rs) - Current coefficient
    float32   U_Coeff;     // Ts/(Ld+Ts*Rs) - Voltage coefficient
    float32   WI_Coeff;    // Ts*Lq/(Ld+Ts*Rs)/2 - Speed/Current coefficient
    float32   E_Coeff;     // Ts/(Ld+Ts*Rs)/2 - Back-EMF coefficient

}BEMF_OBSERVER_PARAMS_F_TYPE;  //!< Parameters and quantities of back-emf observer


typedef struct
{
    //Tracking observer Pi parameters
    PI_CONTROLLER_F_TYPE Pi;

    //Integrator parameter
    INTEGRATOR_PARAMS_F_TYPE Integ_Parameters;

    float32 Omega_Est;     //!< Estimated speed
    float32 Theta_Est;     //!< Estimated position
}TRACKING_OBSERVER_PARAMS_F_TYPE;  //!< Parameters and quantities of tracking observer


typedef struct
{
    float32   K_1;     //!<      -1/(Ls*Iq)
    float32   K_2;     //!<      1/(phim)

    //Startup observer Pi parameters
    PI_CONTROLLER_F_TYPE Pi;

    //Integrator parameters
    INTEGRATOR_PARAMS_F_TYPE Integ_Parameters;

    float32 Theta_Est;       //!< Estimated position
    float32 Theta_Est_D;     //!< Estimated position
    float32 Theta_Est_Q;     //!< Estimated position


    float32 Omega_Est;       //!<  Estimated speed
    float32 Omega_Est_D;     //!<  Estimated speed
    float32 Omega_Est_Q;     //!<  Estimated speed

}STARTUP_OBSERVER_PARAMS_F_TYPE;  //!< Parameters startup observer


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void ObserverPmsm__Initialize(void);

void ObserverPmsm__BemfObsDQ(DQ_COOR_SYST_TYPE* i_dq, DQ_COOR_SYST_TYPE* v_dq, mc_sint32 omega_flux, BEMF_OBSERVER_PARAMS_TYPE* params);

mc_sint16 ObserverPmsm__TrackingObs(mc_sint16 theta_err, TRACKING_OBSERVER_PARAMS_TYPE *params);

mc_sint32 ObserverPmsm__StartupObs(DQ_COOR_SYST_TYPE* v_dq, mc_sint32 vq_alignment, STARTUP_OBSERVER_PARAMS_TYPE* params);

void ObserverPmsm__BemfObsDQF(DQ_COOR_SYST_F_TYPE* i_dq, DQ_COOR_SYST_F_TYPE* v_dq, float32 omega_flux, BEMF_OBSERVER_PARAMS_F_TYPE* params);
float32 ObserverPmsm__TrackingObsF(float32 theta_err, TRACKING_OBSERVER_PARAMS_F_TYPE *params);
float32 ObserverPmsm__StartupObsF(DQ_COOR_SYST_F_TYPE* v_dq, float32 vq_alignment, STARTUP_OBSERVER_PARAMS_F_TYPE* params);

#endif /* OBSERVERPMSM_H_ */

/**
 *  @file
 *  @brief       Pwm modulation prv file
 *  @details     This file contains the private definition for Pwm Modulation Module
 *  @author      
 * @copyright Copyright 2012 - $Date: 2016/02/02 17:26:49CET $. Whirlpool Corporation. All rights reserved  CONFIDENTIAL
*/
/*
 *
 *---------------------------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
 
#ifndef PWMMODULATION_PRV_H
#define PWMMODULATION_PRV_H

#include "Mci_prm.h"
#include "MclBasicTypes.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// FLOATING POINT SECTION - begin /////////////////////////////////

#define F_DUTY_100                  1.00f


////////////////////////////////////////////////////////////////////////////////////////
//                           Dead-time compensation (gradient version)                 //
////////////////////////////////////////////////////////////////////////////////////////
#define DT_COMP_DT                  1.0f     //!< [us]  Dead-time value
#define DT_COMP_I_THR_REAL          1.0f     //!< [Apk] Current Threshold for the linear region



#ifndef COMPILE_4_SIMULINK

#if(WINDY_STRIP_BOARD == 1)
/********* configuration available with third current reconstruction strategy  *******/

////////////////////////////////////////////////////////////////////////////////////////
//                           Limits for Space Vector Modulation                       //
////////////////////////////////////////////////////////////////////////////////////////
#define F_DUTY_LIMIT_MID            0.92f               //!< [% /100] max duty value for duty limitation strategy (1.0f --> no duty limitation)
#define F_DUTY_ANTI_GLITCH_UPPER    0.94f               //!< [% /100] max duty before saturating to 100% (1.0f --> anti-glitch upper disabled)
#define F_DUTY_ANTI_GLITCH_LOWER    0.00f               //!< [% /100] min duty before saturating to 0%   (0.0f --> anti-glitch lower disabled)

#define F_OVERMODULATION_THR        1.00f               //!<  OverModulation Threshold (used also for limiting all duty-cycles)


/*************************************************************************************/

#elif(WINDY_INTERNATION_BOARD == 1)
/********* configuration with standard current reading strategy **********************/
////////////////////////////////////////////////////////////////////////////////////////
//                           Limits for Space Vector Modulation                       //
////////////////////////////////////////////////////////////////////////////////////////
#include "SRMotorSafetyMgr_prv.h"
#define F_DUTY_LIMIT_MID            SR_MODULATION_LIMIT //!< [% /100] max duty value for duty limitation strategy (1.0f --> no duty limitation)  it must match with the parameter used in ClassB
#define F_DUTY_ANTI_GLITCH_UPPER    1.00f               //!< [% /100] max duty before saturating to 100% (1.0f --> anti-glitch upper disabled)
#define F_DUTY_ANTI_GLITCH_LOWER    0.00f               //!< [% /100] min duty before saturating to 0%   (0.0f --> anti-glitch lower disabled)

#define F_OVERMODULATION_THR        F_DUTY_LIMIT_MID    //!<  OverModulation Threshold (used also for limiting all duty-cycles)
/*************************************************************************************/
#else
#error "Need to define a board"
#endif

#else /********* SIMULATION ***************/
// no limitation in simulation
#define F_DUTY_LIMIT_MID            1.0f               //!< [% /100] max duty value for duty limitation strategy (1.0f --> no duty limitation)
#define F_DUTY_ANTI_GLITCH_UPPER    1.0f               //!< [% /100] max duty before saturating to 100% (1.0f --> anti-glitch upper disabled)
#define F_DUTY_ANTI_GLITCH_LOWER    0.0f               //!< [% /100] min duty before saturating to 0%   (0.0f --> anti-glitch lower disabled)

#define F_OVERMODULATION_THR        1.0f               //!<  OverModulation Threshold (used also for limiting all duty-cycles)
#endif

///////////////////////////// FLOATING POINT SECTION - end /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// FIXED POINT SECTION - begin /////////////////////////////////
#define PWM_DUTY_LIMITATION                 ENABLED
#define PWM_DUTY_BOOTSTRAP_LIMITATION       DISABLED
#define PWM_DUTY_ANTI_GLITCH_LIMITATION     ENABLED


#define OVERMODULATION_THR          MC_FLOAT_TO_IQ15L(1.0f)     //!<  OverModulation Threshold in Q17.15 notation



// Duty Limits
#define DUTY_LIMIT_MID              MC_FLOAT_TO_IQ15L(F_DUTY_LIMIT_MID)    //!< percentage
#define DUTY_ANTI_GLITCH_UPPER      MC_FLOAT_TO_IQ15L(F_DUTY_ANTI_GLITCH_UPPER)    //!< percentage
#define DUTY_ANTI_GLITCH_LOWER      MC_FLOAT_TO_IQ15L(F_DUTY_ANTI_GLITCH_LOWER)    //!< percentage
#define DUTY_100                    MC_FLOAT_TO_IQ15L(F_DUTY_100)


// Deadtime compensation
// Real quantities
#define DT_COMP_TSW         (1000000.0f/FSW) //!< [us]  Switching Period
#define DT_COMP_M_REAL      (DT_COMP_DT * BASE_CURRENT / DT_COMP_I_THR_REAL)
#define DT_COMP_M_FLOAT     (DT_COMP_DT / DT_COMP_TSW / DT_COMP_I_THR_REAL)

// P.U. quantities
#define DT_COMP_M           (signed long)IQMATH__REAL2IQ(DT_COMP_M_REAL, DT_COMP_TSW, IQ15)
#define DT_COMP_I_THR       (signed long)IQMATH__REAL2IQ(DT_COMP_I_THR_REAL, BASE_CURRENT, IQ15)
#define DT_COMP_DT_PU       (signed long)IQMATH__REAL2IQ(DT_COMP_DT,DT_COMP_TSW,IQ15)


///////////////////////////// FIXED POINT SECTION - end /////////////////////////////////


#endif

/**
 *  @defgroup   CLASS_B
 *
 *  @file       SRMCPwm_macros.h
 *
 *  @brief      Macro definitions for the Motor Control PWM Driver
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMCPWM_MACROS_H_
#define SRMCPWM_MACROS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define PWM_OFF_CHANNELS (FTM_OUTMASK_CH5OM_MASK | FTM_OUTMASK_CH4OM_MASK | FTM_OUTMASK_CH3OM_MASK | FTM_OUTMASK_CH2OM_MASK | FTM_OUTMASK_CH1OM_MASK | FTM_OUTMASK_CH0OM_MASK)

/**
 * Load the PWM values for all the channels A, B, C
 */
#define SRMCPWM__LOADALLPWMVALUES(pwm_a, pwm_b, pwm_c)    SRMCPwm__LoadAllPwmValues(pwm_a, pwm_b, pwm_c)//{ FTM0.CONTROLS[SR_PWM_U_HIGH].CnV = pwm_a; FTM0.CONTROLS[SR_PWM_U_LOW].CnV = -pwm_a; FTM0.CONTROLS[SR_PWM_V_HIGH].CnV = pwm_b; FTM0.CONTROLS[SR_PWM_V_LOW].CnV = -pwm_b; FTM0.CONTROLS[SR_PWM_W_HIGH].CnV =  pwm_c; FTM0.CONTROLS[SR_PWM_W_LOW].CnV = -pwm_c;  FTM0.PWMLOAD = FTM_PWMLOAD_LDOK_MASK;}

/**
 * Get the maximum PWM value
 */
#define SRMCPWM__GETMAXPWM()                              SRMCPwm__GetMaxPwm()

/**
 * Get the dead time value
 */
#define SRMCPWM__GET_DEAD_TIME()                          FTM0.DEADTIME

/**
 * Get the ON value for lowers IGBTs
 */
#define SRMCPWM__GET_LOWERS_ON_PWM()                      (0)

/**
 * Load the PWM value for the A channel
 */
#define SRMCPWM__LOADPWM_A(value)       				  { FTM0.CONTROLS[SR_PWM_U_LOW].CnV = -value; FTM0.CONTROLS[SR_PWM_U_HIGH].CnV = value;  FTM0.PWMLOAD = FTM_PWMLOAD_LDOK_MASK;}
/**
 * Load the PWM value for the B channel
 */
#define SRMCPWM__LOADPWM_B(value)                         { FTM0.CONTROLS[SR_PWM_V_LOW].CnV = -value; FTM0.CONTROLS[SR_PWM_V_HIGH].CnV = value;  FTM0.PWMLOAD = FTM_PWMLOAD_LDOK_MASK;}
/**
 * Load the PWM value for the C channel
 */
#define SRMCPWM__LOADPWM_C(value)                         { FTM0.CONTROLS[SR_PWM_W_LOW].CnV = -value; FTM0.CONTROLS[SR_PWM_W_HIGH].CnV = value;  FTM0.PWMLOAD = FTM_PWMLOAD_LDOK_MASK;}

/**
 * Get the PWM value for the A channel
 */
#define SRMCPWM__GET_PWM_A()                              (FTM0.CONTROLS[SR_PWM_U_HIGH].CnV)
/**
 * Get the PWM value for the B channel
 */
#define SRMCPWM__GET_PWM_B()                              (FTM0.CONTROLS[SR_PWM_V_HIGH].CnV)
/**
 * Get the PWM value for the C channel
 */
#define SRMCPWM__GET_PWM_C()                              (FTM0.CONTROLS[SR_PWM_W_HIGH].CnV)

/**
 * Get the output state
 */
#define SRMCPWM__OUT_STATE()							  (~(FTM0.OUTMASK | ~PWM_OFF_CHANNELS))

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#endif // SRMCPWM_MACROS_H_



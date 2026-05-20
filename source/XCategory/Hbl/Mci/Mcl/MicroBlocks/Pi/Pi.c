/**
 *  @file
 *  @brief       PI regulator
 *  @details     PI regulator with 2 different anti-windup strategies:
 *               back-calculation and clamping
 *  @author      alessio.beato/luigi.fagnano (only temporary, since it is not integrated in MKS)
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
*/

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Pi.h"
#include "McMathCalc_macros.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      PI Regulator with back-calculation anti-windup strategy
 *  @details    PI controller algorithm.
 *              Definitions:
 *
 *              <b>e(t)</b> - input error in continuous time domain.
 *
 *              <b>u(t)</b> - controller output in continuous time domain.
 *
 *              <b>e(k)</b> - input error at step k - discrete time domain.
 *
 *              <b>u(k)</b> - controller output at step k - discrete time domain.
 *
 *              <b>e_max</b> - max range of e(k).
 *
 *              <b>u_max</b> - max range of u(k).
 *
 *            PI controller algorithm in <b>continuous time domain</b>:
 *
 *          \f[ u(t) = K\left(e(t) +\frac{1}{T_i}\int_0^t \! e(t) \, \mathrm{d}t. \right)\f]
 *           where:
 *            K :  controller gain
 *
 *            Ti: integral time constant
 *
 *           PI controller algorithm in <b>fractional arithmetic</b>:
 *
 *           \f[
 *             \begin{aligned}
 *                & u_f(k) = K_{p_{sc}} \cdot e_f(k) + u_f(k - 1) + K_{i_{sc}} \cdot e_f(k) \\
 *                & e_f(k) = \frac{e(k)}{e_{max}}\\
 *                & u_f(k) = \frac{u(k)}{u_{max}}\\
 *                & K_{p_{sc}} =  K \frac{e(k)}{e_{max}}\\
 *                & K_{i_{sc}} =  K \frac{T}{T_i}\frac{e(k)}{e_{max}}\\
 *             \end{aligned}
 *           \f]
 *           where:
 *           <b>T</b> : sampling time
 *
 *  The <b>anti-windup</b> technique is implemented as follows:
 *  when the output saturates, the integral
 *  term in the controller is recomputed so that its new value gives an
 *  output at the saturation limit
 *
 *  @param[in]      pi_params       = PI parameters: gains and limits
 *  @param[in]      err             = PI input error in Q17.15 notation
 *  @param[in, out] integ_term_k_1  = PI state variable in Q2.30 notation
 *  @return         PI output after saturation in Q17.15 notation
 */
mc_sint32 Pi__PiRegBackCalc(mc_sint32 err, mc_sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_TYPE* pi_params)
{

    mc_sint64 prop_term;
    mc_sint64 integ_term;
    mc_sint64 tmp;

    // prop_term calculation in Q34.30 notation
    prop_term = MC_MULT_LL_LL(pi_params->KpMant, err, (IQ15 + pi_params->KpExp), IQ15, IQ30);

    // prop_term Saturation
    if (prop_term > (mc_sint64)pi_params->UpperLimit)
    {
        prop_term = (mc_sint64)pi_params->UpperLimit;
    }
    else if (prop_term < (mc_sint64)pi_params->LowerLimit)
    {
        prop_term = (mc_sint64)pi_params->LowerLimit;
    }

    // integ_term calculation in Q34.30 notation
    integ_term = MC_MULT_LL_LL(pi_params->KiMant, err, (IQ15 + pi_params->KiExp), IQ15, IQ30);

    integ_term = integ_term + (mc_sint64)*integ_term_k_1;

    // output Saturation & state variable back-calculation anti-windup
    tmp = prop_term + integ_term;

    if (tmp > (mc_sint64)pi_params->UpperLimit)
    {
        tmp = (mc_sint64)pi_params->UpperLimit;
        *integ_term_k_1 = pi_params->UpperLimit - (mc_sint32)prop_term;
    }
    else if (tmp < pi_params->LowerLimit)
    {
        tmp = (mc_sint64)pi_params->LowerLimit;
        *integ_term_k_1 = pi_params->LowerLimit - (mc_sint32)prop_term;
    }
    else
    {
        *integ_term_k_1 = (mc_sint64)integ_term;
    }

    // output in Q17.15 notation
    return ((mc_sint32)(MC_SCALE(tmp,IQ30,IQ15)));
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      PI Regulator with clamping anti-windup
 *  @details    PI controller algorithm.
 *              Definitions:
 *
 *              <b>e(t)</b> - input error in continuous time domain.
 *
 *              <b>u(t)</b> - controller output in continuous time domain.
 *
 *              <b>e(k)</b> - input error at step k - discrete time domain.
 *
 *              <b>u(k)</b> - controller output at step k - discrete time domain.
 *
 *              <b>e_max</b> - max range of e(k).
 *
 *              <b>u_max</b> - max range of u(k).
 *
 *            PI controller algorithm in <b>continuous time domain</b>:
 *
 *          \f[ u(t) = K\left(e(t) +\frac{1}{T_i}\int_0^t \! e(t) \, \mathrm{d}t. \right)\f]
 *           where:
 *            K :  controller gain
 *
 *            Ti: integral time constant
 *
 *           PI controller algorithm in <b>fractional arithmetic</b>:
 *
 *           \f[
 *             \begin{aligned}
 *                & u_f(k) = K_{p_{sc}} \cdot e_f(k) + u_f(k - 1) + K_{i_{sc}} \cdot e_f(k) \\
 *                & e_f(k) = \frac{e(k)}{e_{max}}\\
 *                & u_f(k) = \frac{u(k)}{u_{max}}\\
 *                & K_{p_{sc}} =  K \frac{e(k)}{e_{max}}\\
 *                & K_{i_{sc}} =  K \frac{T}{T_i}\frac{e(k)}{e_{max}}\\
 *             \end{aligned}
 *           \f]
 *           where:
 *           <b>T</b> : sampling time
 *
 *  The <b>anti-windup</b> technique is implemented as follows.
 *
 *  It stops integration when the sum of proportional and integral components exceeds the output limits
 *  and the two components have the same sign.
 *
 *  @param[in]      pi_params       = PI parameters: gains and limits
 *  @param[in]      err             = PI input error in Q17.15 notation
 *  @param[in, out] integ_term_k_1  = PI state variable in Q2.30 notation
 *  @return         PI output after saturation in Q17.15 notation
 */
mc_sint32 Pi__PiRegClamp(mc_sint32 err, mc_sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_TYPE* pi_params)
{

    mc_sint64 prop_term;
    mc_sint64 integ_term;
    mc_sint64 tmp;


    // prop_term calculation in Q34.30 notation
    prop_term = MC_MULT_LL_LL(pi_params->KpMant, err, (IQ15 + pi_params->KpExp), IQ15, IQ30);

    // integ_term calculation in Q34.30 notation
    integ_term = MC_MULT_LL_LL(pi_params->KiMant, err, (IQ15 + pi_params->KiExp), IQ15, IQ30);

    integ_term = integ_term + (mc_sint64)*integ_term_k_1;

    integ_term = MATHCALC__SATURATE(MC_MINUS_ONE_IQ30, integ_term, MC_ONE_IQ30);

    tmp = prop_term + integ_term;

    // output Saturation & state variable clamping anti-windup
    if (tmp > (mc_sint64) pi_params->UpperLimit)
    {
        tmp = (mc_sint64)pi_params->UpperLimit;
        // check components sign to decide if clamping is needed
        if ((MATHCALC__SIGN(err)) != (MATHCALC__SIGN(integ_term)))
        {
            *integ_term_k_1 = (mc_sint64)integ_term;
        }
    }
    else if (tmp < (mc_sint64) pi_params->LowerLimit)
    {
        tmp = (mc_sint64)pi_params->LowerLimit;
        // check components sign to decide if clamping is needed
        if ((MATHCALC__SIGN(err)) != (MATHCALC__SIGN(integ_term)))
        {
            *integ_term_k_1 = (mc_sint64)integ_term;
        }
    }
    else
    {
        *integ_term_k_1 = (mc_sint64)integ_term;
    }

    // output in Q17.15 notation
    return ((mc_sint32)MC_SCALE(tmp,IQ30,IQ15));
}






//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      PI Regulator without anti-windup
 *  @details    PI controller algorithm.
 *              Definitions:
 *
 *              <b>e(t)</b> - input error in continuous time domain.
 *
 *              <b>u(t)</b> - controller output in continuous time domain.
 *
 *              <b>e(k)</b> - input error at step k - discrete time domain.
 *
 *              <b>u(k)</b> - controller output at step k - discrete time domain.
 *
 *              <b>e_max</b> - max range of e(k).
 *
 *              <b>u_max</b> - max range of u(k).
 *
 *            PI controller algorithm in <b>continuous time domain</b>:
 *
 *          \f[ u(t) = K\left(e(t) +\frac{1}{T_i}\int_0^t \! e(t) \, \mathrm{d}t. \right)\f]
 *           where:
 *            K :  controller gain
 *
 *            Ti: integral time constant
 *
 *           PI controller algorithm in <b>fractional arithmetic</b>:
 *
 *           \f[
 *             \begin{aligned}
 *                & u_f(k) = K_{p_{sc}} \cdot e_f(k) + u_f(k - 1) + K_{i_{sc}} \cdot e_f(k) \\
 *                & e_f(k) = \frac{e(k)}{e_{max}}\\
 *                & u_f(k) = \frac{u(k)}{u_{max}}\\
 *                & K_{p_{sc}} =  K \frac{e(k)}{e_{max}}\\
 *                & K_{i_{sc}} =  K \frac{T}{T_i}\frac{e(k)}{e_{max}}\\
 *             \end{aligned}
 *           \f]
 *           where:
 *           <b>T</b> : sampling time
 *
 *
 *  It stops integration when the sum of proportional and integral components exceeds the output limits
 *  and the two components have the same sign.
 *
 *  @param[in]      pi_params       = PI parameters: gains and limits
 *  @param[in]      err             = PI input error in Q17.15 notation
 *  @param[in, out] integ_term_k_1  = PI state variable in Q2.30 notation
 *  @return         PI output after saturation in Q2.30 notation
 */
mc_sint32 Pi__PiRegNoAntiWindup(mc_sint32 err, mc_sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_TYPE* pi_params)
{

    mc_sint64 prop_term;
    mc_sint64 integ_term;
    mc_sint64 tmp64;


    // prop_term calculation in Q34.30 notation
    prop_term = MC_MULT_LL_LL(pi_params->KpMant, err, (IQ15 + pi_params->KpExp), IQ15, IQ30);

    // integ_term calculation in Q34.30 notation
    integ_term = MC_MULT_LL_LL(pi_params->KiMant, err, (IQ15 + pi_params->KiExp), IQ15, IQ30);

    integ_term = integ_term + (mc_sint64)*integ_term_k_1;

    // output Saturation & state variable limitation
    tmp64 = prop_term + integ_term;

    tmp64 = MATHCALC__SATURATE((mc_sint64)pi_params->LowerLimit, tmp64, (mc_sint64)pi_params->UpperLimit);

    integ_term = MATHCALC__SATURATE(MC_MINUS_ONE_IQ30, integ_term, MC_ONE_IQ30);

    *integ_term_k_1 = (mc_sint32)integ_term;

    // output in Q2.30 notation
    return ((mc_sint32)tmp64);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      PI Regulator with back-calculation anti-windup strategy
 *  @details    PI controller algorithm.
 *              Definitions:
 *
 *              <b>e(t)</b> - input error in continuous time domain.
 *
 *              <b>u(t)</b> - controller output in continuous time domain.
 *
 *              <b>e(k)</b> - input error at step k - discrete time domain.
 *
 *              <b>u(k)</b> - controller output at step k - discrete time domain.
 *
 *              <b>e_max</b> - max range of e(k).
 *
 *              <b>u_max</b> - max range of u(k).
 *
 *            PI controller algorithm in <b>continuous time domain</b>:
 *
 *          \f[ u(t) = K\left(e(t) +\frac{1}{T_i}\int_0^t \! e(t) \, \mathrm{d}t. \right)\f]
 *           where:
 *            K :  controller gain
 *
 *            Ti: integral time constant
 *
 *           PI controller algorithm in <b>fractional arithmetic</b>:
 *
 *           \f[
 *             \begin{aligned}
 *                & u_f(k) = K_{p_{sc}} \cdot e_f(k) + u_f(k - 1) + K_{i_{sc}} \cdot e_f(k) \\
 *                & e_f(k) = \frac{e(k)}{e_{max}}\\
 *                & u_f(k) = \frac{u(k)}{u_{max}}\\
 *                & K_{p_{sc}} =  K \frac{e(k)}{e_{max}}\\
 *                & K_{i_{sc}} =  K \frac{T}{T_i}\frac{e(k)}{e_{max}}\\
 *             \end{aligned}
 *           \f]
 *           where:
 *           <b>T</b> : sampling time
 *
 *  The <b>anti-windup</b> technique is implemented as follows:
 *  when the output saturates, the integral
 *  term in the controller is recomputed so that its new value gives an
 *  output at the saturation limit
 *
 *  @param[in]      pi_params       = PI parameters: gains in floating p.u. and limits in IQ
 *  @param[in]      err             = PI input error in Q17.15 notation
 *  @param[in, out] integ_term_k_1  = PI state variable in Q2.30 notation
 *  @return         PI output after saturation in Q17.15 notation
 */
sint32 Pi__PiRegBackCalc_FloatGains(sint32 err, sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_FLOATGAINS_TYPE* pi_params)
{
    /* p.u. float gains --> kfloat*(IQ<<15) */
    sint64 prop_term;
    sint64 integ_term;
    sint64 tmp;

    // prop_term calculation in Q34.30 notation
    //prop_term = (sint64) (pi_params->Kp * (float)(MC_SCALE(err, IQ15, IQ30)));
    prop_term = (sint64) (pi_params->Kp * err);

    // prop_term Saturation
    if (prop_term > pi_params->UpperLimit)
    {
        prop_term = pi_params->UpperLimit;
    }
    else if (prop_term < pi_params->LowerLimit)
    {
        prop_term = pi_params->LowerLimit;
    }

    // integ_term calculation in Q34.30 notation
    //integ_term = (sint64)(pi_params->Ki * (float)(MC_SCALE(err, IQ15, IQ30)));
    integ_term = (sint64)(pi_params->Ki * err);

    integ_term = integ_term + *integ_term_k_1;

    // output Saturation & state variable back-calculation anti-windup
    tmp = prop_term + integ_term;

    if (tmp > pi_params->UpperLimit)
    {
        tmp = pi_params->UpperLimit;
        *integ_term_k_1 = pi_params->UpperLimit - prop_term;
    }
    else if (tmp < pi_params->LowerLimit)
    {
        tmp = pi_params->LowerLimit;
        *integ_term_k_1 = pi_params->LowerLimit - prop_term;
    }
    else
    {
        *integ_term_k_1 = integ_term;
    }

    // output in Q17.15 notation
    return ((sint32)(MC_SCALE(tmp,IQ30,IQ15)));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Integral routine: it approximates the integral of an input quantities according to Backward-Euler Method.
 *  @details
 *  This function approximates the integral of an input signal according to the Backward-Euler Method.
 *  The output is eventually saturated to the upper or lower limit if needed.
 *
 *          Integrator in <b>continuous time domain</b>:
 *          \f[ y(t) =  K_i\int_0^t \! x(t) \, \mathrm{d}t. \f]
 *           where:
 *           Ki : integrator gain
 *
 *           Integrator in <b>fractional arithmetic</b>, <b>discrete time domain</b>:
 *
 *           \f[ y_f(k) =  y_f(k - 1) + T_s \cdot K_{i_{f}} \cdot x_f(k) \f]
 *           where:
 *           Ts : sampling period
 *
 *  @param[in]      integ_params    = integrator parameters: integrator gain and limits
 *  @param[in, out] y_k_1           = integrator output in Q2.30 notation  (input is at k-1 step, output is at k step)
 *  @param[in]      x_k             = integrator input in Q17.15 notation
 *  @return         integrator output after saturation in Q17.15 notation
 *
 */
mc_sint32 Pi__IntegBESat(mc_sint32 x_k, mc_sint32* y_k_1, INTEGRATOR_PARAMS_TYPE* integ_params)
{
    mc_sint64 temp64;
    temp64 = (mc_sint64) (*y_k_1) + MC_MULT_LL_LL(x_k,integ_params->KiMant, IQ15,(IQ15+integ_params->KiExp),IQ30);

    // Saturation
    *y_k_1 = (mc_sint32)MATHCALC__SATURATE((mc_sint64)integ_params->LowerLimit, temp64, (mc_sint64)integ_params->UpperLimit);
/*
    if (temp64 > integ_params->UpperLimit)
    {
        *y_k_1 = integ_params->UpperLimit;     //2.30
    }
    else if (temp64 < integ_params->LowerLimit)
    {
        *y_k_1 = integ_params->LowerLimit;     //2.30
    }
    else
    {
        *y_k_1 = (sint32)temp64;
    }
*/
    return (MC_SCALE(*y_k_1, IQ30, IQ15));    //y_k in 17.15 notation
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief     Integral routine: it approximates the integral of an input quantities according to Backward-Euler Method.
 *  @details
 *  This function approximates the integral of an input signal according to the Backward-Euler Method.
 *  The output is not saturated, so overflow may occur.
 *
 *          Integrator in <b>continuous time domain</b>:
 *          \f[ y(t) =  K_i\int_0^t \! x(t) \, \mathrm{d}t. \f]
 *           where:
 *           Ki : integrator gain
 *
 *           Integrator in <b>fractional arithmetic</b>, <b>discrete time domain</b>:
 *
 *           \f[ y_f(k) =  y_f(k - 1) + T_s \cdot K_{i_{f}} \cdot x_f(k) \f]
 *           where:
 *           Ts : sampling period
 *
 *  @param[in]      integ_params    = integrator parameters: integrator gain and limits
 *  @param[in, out] y_k_1           = integrator output in Q2.30 notation  (input is at k-1 step, output is at k step)
 *  @param[in]      x_k             = integrator input in Q17.15 notation
 *  @return         integrator output in Q17.15 notation
 *
 */
mc_sint32 Pi__IntegBE(mc_sint32 x_k, mc_sint32* y_k_1, INTEGRATOR_PARAMS_TYPE* integ_params)
{
    mc_sint32 temp;

    temp = MC_MULT_L(x_k, integ_params->KiMant,IQ15, (IQ15+integ_params->KiExp), IQ30);  //2.30 not saturated
    *y_k_1 = *y_k_1 + temp;   //y_k in 2.30 notation

    return (MC_SCALE(*y_k_1, IQ30, IQ15));    //y_k in 17.15 notation
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      PI Regulator with back-calculation anti-windup strategy
 *  @details    PI controller algorithm.
 *              Definitions:
 *
 *              <b>e(t)</b> - input error in continuous time domain.
 *
 *              <b>u(t)</b> - controller output in continuous time domain.
 *
 *              <b>e(k)</b> - input error at step k - discrete time domain.
 *
 *              <b>u(k)</b> - controller output at step k - discrete time domain.
 *
 *              <b>e_max</b> - max range of e(k).
 *
 *              <b>u_max</b> - max range of u(k).
 *
 *            PI controller algorithm in <b>continuous time domain</b>:
 *
 *          \f[ u(t) = K\left(e(t) +\frac{1}{T_i}\int_0^t \! e(t) \, \mathrm{d}t. \right)\f]
 *           where:
 *            K :  controller gain
 *
 *            Ti: integral time constant
 *
 *           PI controller algorithm in <b>fractional arithmetic</b>:
 *
 *           \f[
 *             \begin{aligned}
 *                & u_f(k) = K_{p_{sc}} \cdot e_f(k) + u_f(k - 1) + K_{i_{sc}} \cdot e_f(k) \\
 *                & e_f(k) = \frac{e(k)}{e_{max}}\\
 *                & u_f(k) = \frac{u(k)}{u_{max}}\\
 *                & K_{p_{sc}} =  K \frac{e(k)}{e_{max}}\\
 *                & K_{i_{sc}} =  K \frac{T}{T_i}\frac{e(k)}{e_{max}}\\
 *             \end{aligned}
 *           \f]
 *           where:
 *           <b>T</b> : sampling time
 *
 *  The <b>anti-windup</b> technique is implemented as follows:
 *  when the output saturates, the integral
 *  term in the controller is recomputed so that its new value gives an
 *  output at the saturation limit
 *
 *  @param[in]      pi_params       = PI parameters: gains and limits
 *  @param[in]      err             = PI input error in Q17.15 notation
 *  @param[in, out] integ_term_k_1  = PI state variable in Q2.30 notation
 *  @return         PI output after saturation in Q17.15 notation
 */
mc_sint32 Pi__PiRegBackCalc_FixedGains(mc_sint32 err, mc_sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_FIXEDGAINS_TYPE* pi_params)
{

    mc_sint64 prop_term;
    mc_sint64 integ_term;
    mc_sint64 tmp;

    // prop_term calculation in Q.39 notation
    prop_term = (mc_sint64) pi_params->Kp * (mc_sint64) err;
    prop_term = MC_SCALE(prop_term, IQ39, IQ30);

    // prop_term Saturation
    if (prop_term > (mc_sint64)pi_params->UpperLimit)
    {
        prop_term = (mc_sint64)pi_params->UpperLimit;
    }
    else if (prop_term < (mc_sint64)pi_params->LowerLimit)
    {
        prop_term = (mc_sint64)pi_params->LowerLimit;
    }

    // integ_term calculation in Q.44 notation
    integ_term = (mc_sint64) pi_params->Ki * (mc_sint64) err;
    integ_term = MC_SCALE(integ_term, IQ44, IQ30);

    integ_term = integ_term + (mc_sint64)*integ_term_k_1;

    // output Saturation & state variable back-calculation anti-windup
    tmp = prop_term + integ_term;

    if (tmp > (mc_sint64)pi_params->UpperLimit)
    {
        tmp = (mc_sint64)pi_params->UpperLimit;
        *integ_term_k_1 = pi_params->UpperLimit - (mc_sint32)prop_term;
    }
    else if (tmp < pi_params->LowerLimit)
    {
        tmp = (mc_sint64)pi_params->LowerLimit;
        *integ_term_k_1 = pi_params->LowerLimit - (mc_sint32)prop_term;
    }
    else
    {
        *integ_term_k_1 = (mc_sint64)integ_term;
    }

    // output in Q17.15 notation
    return ((mc_sint32)(MC_SCALE(tmp,IQ30,IQ15)));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize pi parameters.
 *
 *  @param      sum, out
 *  @return     none
 */
void Pi__InitPiQ60( TPiParmQ60 *parmPtr )
{
    parmPtr->sum = 0;
    parmPtr->out = 0;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate pi control value, internal variable is Q60 format.
 *
 *  @param      Kp, Ki, outMax, outMin, inRef, inMeas
 *  @return     out
 */
void Pi__CalcPiQ60(TPiParmQ60 *parmPtr)
{
    signed long long int tempS64;

    parmPtr->err = parmPtr->inRef - parmPtr->inMeas;

    tempS64 = ((signed long long int)parmPtr->Kp * (signed long long int)parmPtr->err) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Kp - parmPtr->Fmt.In);

    if (parmPtr->err >= 0)
    {
        if(tempS64 >= ((signed long long int)parmPtr->outMax << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out)))
        {
            parmPtr->prop = (signed long long int)(parmPtr->outMax) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out);
        }
        else
        {
            parmPtr->prop = tempS64;
        }
    }
    else
    {
        if(tempS64 <= ((signed long long int)(parmPtr->outMin) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out)) )
        {
            parmPtr->prop = (signed long long int)parmPtr->outMin << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out);
        }
        else
        {
            parmPtr->prop = tempS64;
        }
    }

    if ( parmPtr->err >= 0)
    {
        parmPtr->int_uplim = ((signed long long int)parmPtr->outMax << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out)) - parmPtr->prop;
        parmPtr->int_lowlim = -(parmPtr->int_uplim);
    }
    else
    {
        parmPtr->int_lowlim = ((signed long long int)parmPtr->outMin << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out)) - parmPtr->prop;
        parmPtr->int_uplim = -(parmPtr->int_lowlim);
    }

    tempS64 = (((signed long long int)parmPtr->Ki * (signed long long int)parmPtr->err) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Ki - parmPtr->Fmt.In)) + parmPtr->sum;

    if (tempS64 >= parmPtr->int_uplim)
    {
        parmPtr->sum = parmPtr->int_uplim;
    }
    else if (tempS64 <= parmPtr->int_lowlim)
    {
        parmPtr->sum = parmPtr->int_lowlim;
    }
    else
    {
        parmPtr->sum = tempS64;
    }
    parmPtr->out = (signed long int)((parmPtr->sum + parmPtr->prop) >> (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out));
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate pi control value, internal variable is Q60 format.
 *
 *  @param      Kp, Ki, outMax, outMin, inRef, inMeas
 *  @return     out
 */
void Pi__CalcPiQ30(TPiParmQ30 *parmPtr)
{
    signed long int tempS32;

    parmPtr->err = parmPtr->inRef - parmPtr->inMeas;


    tempS32 = ((signed long int)parmPtr->Kp * (signed long int)parmPtr->err) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Kp - parmPtr->Fmt.In);
    //tempS32 is in Q(FmtPropSum) format now

    if (parmPtr->err >= 0)
    {
        if(tempS32 >= ((signed long int)parmPtr->outMax << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out)))
        {
            parmPtr->prop = (signed long int)(parmPtr->outMax) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out);
        }
        else
        {
            parmPtr->prop = tempS32;
        }
    }
    else
    {
        if(tempS32 <= ((signed long int)(parmPtr->outMin) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out)) )
        {
            parmPtr->prop = (signed long int)parmPtr->outMin << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out);
        }
        else
        {
            parmPtr->prop = tempS32;
        }
    }

    if ( parmPtr->err >= 0)
    {
        parmPtr->int_uplim = ((signed long int)parmPtr->outMax << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out)) - parmPtr->prop;
        parmPtr->int_lowlim = -(parmPtr->int_uplim);
    }
    else
    {
        parmPtr->int_lowlim = ((signed long int)parmPtr->outMin << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out)) - parmPtr->prop;
        parmPtr->int_uplim = -(parmPtr->int_lowlim);
    }

    tempS32 = (((signed long int)parmPtr->Ki * (signed long int)parmPtr->err) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Ki - parmPtr->Fmt.In)) + parmPtr->sum;

    if (tempS32 >= parmPtr->int_uplim)
    {
        parmPtr->sum = parmPtr->int_uplim;
    }
    else if (tempS32 <= parmPtr->int_lowlim)
    {
        parmPtr->sum = parmPtr->int_lowlim;
    }
    else
    {
        parmPtr->sum = tempS32;
    }
    parmPtr->out = (signed long int)((parmPtr->sum + parmPtr->prop) >> (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out));
}




void Pi__InitPiQ30( TPiParmQ30 *parmPtr )
{
    parmPtr->sum = 0;
    parmPtr->out = 0;
}   /* end of InitPi() */



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate pi control value, internal variable is Q30 format.
 *
 *  @param      Kp, Ki, outMax, outMin, inRef, inMeas
 *  @return     out
 */
void Pi__CalcPiQ30_new(TPiParmQ30 *parmPtr)
{
    sint32 tempS32;

    parmPtr->err = parmPtr->inRef - parmPtr->inMeas;

    parmPtr->prop = ((signed long long int)parmPtr->Kp * (signed long long int)parmPtr->err) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Kp - parmPtr->Fmt.In); // proportional term

    tempS32 = ((signed long long int)parmPtr->Ki * (signed long long int)parmPtr->err) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Ki - parmPtr->Fmt.In); // new integrated error

    parmPtr->out = (signed long int)((parmPtr->sum + tempS32 + parmPtr->prop) >> (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out));

    if(parmPtr->out >= parmPtr->outMax)   // Saturation on the positive limit
    {
        parmPtr->out = parmPtr->outMax;         // Limit the output
        tempS32 = (tempS32 > 0) ? 0 : tempS32;  // Do not allow increase of the sum (antiwindup)
    }
    else if(parmPtr->out <= parmPtr->outMin)
    {
        parmPtr->out = parmPtr->outMin;              // Limit the output
        tempS32 = (tempS32 < 0) ? 0 : tempS32;  // Do not allow decrease of the sum (antiwindup)
    }

    parmPtr->sum += tempS32;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate pi control value, internal variable is Q60 format.
 *
 *  @param      Kp, Ki, outMax, outMin, inRef, inMeas
 *  @return     out
 */
void Pi__CalcPiQ60_new(TPiParmQ60 *parmPtr)
{
    signed long long int tempS64;

    parmPtr->err = parmPtr->inRef - parmPtr->inMeas;

    parmPtr->prop = ((signed long long int)parmPtr->Kp * (signed long long int)parmPtr->err) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Kp - parmPtr->Fmt.In); // proportional term

    tempS64 = ((signed long long int)parmPtr->Ki * (signed long long int)parmPtr->err) << (parmPtr->Fmt.PropSum - parmPtr->Fmt.Ki - parmPtr->Fmt.In); // new integrated error

    parmPtr->out = (signed long int)((parmPtr->sum + tempS64 + parmPtr->prop) >> (parmPtr->Fmt.PropSum - parmPtr->Fmt.Out));

    if(parmPtr->out >= parmPtr->outMax)   // Saturation on the positive limit
    {
        parmPtr->out = parmPtr->outMax;         // Limit the output
        tempS64 = (tempS64 > 0) ? 0 : tempS64;  // Do not allow increase of the sum (antiwindup)
    }
    else if(parmPtr->out <= parmPtr->outMin)
    {
        parmPtr->out = parmPtr->outMin;              // Limit the output
        tempS64 = (tempS64 < 0) ? 0 : tempS64;  // Do not allow decrease of the sum (antiwindup)
    }

    parmPtr->sum += tempS64;
}


void Pi__CalcPiF(PI_CONTROLLER_F_TYPE *param)
{
    float32 prop_term;
    float32 integ_term;
    float32 out;

    // calculate proportional term
    prop_term = param->Kp * param->Err;

    // calculate integral term step k
    integ_term = param->Ki * param->Err;

    // prepare the output for saturation
    out = prop_term + integ_term  + param->Integ_K_1;

    // saturate and apply anti-windup to the state
    if (out > param->Upper_Limit)
    {
        param->Out = param->Upper_Limit;
        if (integ_term > 0.0f)
        {
            integ_term = 0.0f;
        }
    }
    else if (out < param->Lower_Limit)
    {
        param->Out = param->Lower_Limit;
        if (integ_term<0.0f)
        {
            integ_term = 0.0f;
        }
    }
    else
    {
        param->Out = out;
    }

    param->Integ_K_1 += integ_term;
}


void Pi__CalcPiBackCalcF(PI_CONTROLLER_F_TYPE *param)
{
    float32 prop_term;
    float32 integ_term;
    float32 tmp;

    prop_term = param->Kp * param->Err;
    // prop_term Saturation
    if (prop_term > param->Upper_Limit)
    {
       prop_term = param->Upper_Limit;
    }
    else if (prop_term < param->Lower_Limit)
    {
       prop_term = param->Lower_Limit;
    }

    integ_term = param->Ki * param->Err;
    integ_term = integ_term + param->Integ_K_1;
    // output Saturation & state variable back-calculation anti-windup
    tmp = prop_term + integ_term;

    if (tmp > param->Upper_Limit)
    {
       tmp = param->Upper_Limit;
       param->Integ_K_1 = param->Upper_Limit - prop_term;
    }
    else if (tmp < param->Lower_Limit)
    {
       tmp = param->Lower_Limit;
       param->Integ_K_1 = param->Lower_Limit - prop_term;
    }
    else
    {
        param->Integ_K_1 = integ_term;
    }

    param->Out = tmp;
}


void Pi__PiRegNoAntiWindupF(PI_CONTROLLER_F_TYPE *param)
{
    float32 prop_term;
    float32 integ_term;
    float32 out;

    // calculate proportional term
    prop_term = param->Kp * param->Err;

    // calculate integral term step k
    integ_term = param->Ki * param->Err + param->Integ_K_1;

    // prepare the output for saturation
    out = prop_term + integ_term ;

    if (out > param->Upper_Limit)
    {
        out = param->Upper_Limit;
    }
    else if (out < param->Lower_Limit)
    {
        out = param->Lower_Limit;
    }

    param->Integ_K_1 = integ_term;
    param->Out = out;
}

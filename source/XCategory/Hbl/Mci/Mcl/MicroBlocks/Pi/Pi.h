/**
 *  @file
 *  @brief       PI regulator
 *  @details     PI regulator with 2 different anti-windup strategies:
 *               back-calculation and clamping
 *  @author      alessio.beato/luigi.fagnano  (only temporary, since it is not integrated in MKS)
 *  $Header: FOC/Pi.h 1.1 2015/08/17 23:01:38CEST Marcelo Silva (SILVAMC) Exp  $
 * @copyright Copyright 2012 - $Date: 2015/08/17 23:01:38CEST $. Whirlpool Corporation. All rights reserved – CONFIDENTIAL
*/

#ifndef PI_H_
#define PI_H_

#include "MclBasicTypes.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef struct
{
   mc_sint32   KpMant;        //!< Q17.15 - Proportional Gain mantissa value
   mc_sint32   KiMant;        //!< Q17.15 - Integral Gain mantissa value
   mc_sint32   UpperLimit;    //!< Q2.30  - Regulator upper limit
   mc_sint32   LowerLimit;    //!< Q2.30  - Regulator lower limit
   uint8    KpExp;         //!< Proportional Gain exponent value (used to represent small numbers)
   uint8    KiExp;         //!< Integral Gain exponent value (used to represent small numbers)
}CONTROLLER_PI_PARAMS_TYPE;//!< PI Controller parameters (gains and saturation limits)

typedef struct
{
   float    Kp;        //!< Proportional Gain float p.u. value
   float    Ki;        //!< Integral Gain float p.u. value
   sint32   UpperLimit;    //!< Q2.30  - Regulator upper limit
   sint32   LowerLimit;    //!< Q2.30  - Regulator lower limit
}CONTROLLER_PI_PARAMS_FLOATGAINS_TYPE;//!< PI Controller parameters (gains and saturation limits)

typedef struct
{
   mc_sint32   Kp;        //!< Q8.24 - Proportional Gain mantissa value
   mc_sint32   Ki;        //!< Q3.29 - Integral Gain mantissa value
   mc_sint32   UpperLimit;    //!< Q2.30  - Regulator upper limit
   mc_sint32   LowerLimit;    //!< Q2.30  - Regulator lower limit
}CONTROLLER_PI_PARAMS_FIXEDGAINS_TYPE;//!< PI Controller parameters (gains and saturation limits)

typedef struct
{
    float32  Err;            	//!<  error
    float32  Integ_K_1;      	//!< integral term state
    float32  Out;            	//!< output (after saturation)
    float32  Kp;             	//!< Proportional Gain
    float32  Ki;             	//!< Integral Gain
    float32  Upper_Limit;     	//!< Regulator upper limit
    float32  Lower_Limit;     	//!< Regulator lower limit
}PI_CONTROLLER_F_TYPE;//!< PI Controller (floating point version)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

mc_sint32 Pi__PiRegBackCalc(mc_sint32 err, mc_sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_TYPE* pi_params);
mc_sint32 Pi__PiRegClamp(mc_sint32 err, mc_sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_TYPE* pi_params);
mc_sint32 Pi__PiRegNoAntiWindup(mc_sint32 err, mc_sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_TYPE* pi_params);
sint32 Pi__PiRegBackCalc_FloatGains(sint32 err, sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_FLOATGAINS_TYPE* pi_params);
mc_sint32 Pi__PiRegBackCalc_FixedGains(mc_sint32 err, mc_sint32* integ_term_k_1, CONTROLLER_PI_PARAMS_FIXEDGAINS_TYPE* pi_params);

mc_sint32 Pi__IntegBESat(mc_sint32 x_k, mc_sint32* y_k_1, INTEGRATOR_PARAMS_TYPE* integ_params);
mc_sint32 Pi__IntegBE(mc_sint32 x_k, mc_sint32* y_k_1, INTEGRATOR_PARAMS_TYPE* integ_params);
void Pi__CalcPiF(PI_CONTROLLER_F_TYPE *param);
void Pi__CalcPiBackCalcF(PI_CONTROLLER_F_TYPE *param);
void Pi__PiRegNoAntiWindupF(PI_CONTROLLER_F_TYPE *param);

typedef struct
{
    unsigned char In;
    unsigned char Out;
    unsigned char Kp;
    unsigned char Ki;
    unsigned char PropSum;
} PI_FORMAT_TYPE;

typedef struct
{
    signed long long int    sum;
    signed long long int    prop;
    signed long long int    int_uplim;
    signed long long int    int_lowlim;
    signed long int         Kp;
    signed long int         Ki;
    signed long int         err;
    signed long int         err_max;
    signed long int         outMax;
    signed long int         outMin;
    signed long int         inRef;
    signed long int         inMeas;
    signed long int         out;
    PI_FORMAT_TYPE          Fmt;
} TPiParmQ60;


typedef struct
{
    signed long int         sum;
    signed long int         prop;
    signed long int         int_uplim;
    signed long int         int_lowlim;
    signed long int         Kp;
    signed long int         Ki;
    signed long int         err;
    signed long int         err_max;
    signed long int         outMax;
    signed long int         outMin;
    signed long int         inRef;
    signed long int         inMeas;
    signed long int         out;
    PI_FORMAT_TYPE          Fmt;
} TPiParmQ30;


void Pi__InitPiQ30(TPiParmQ30 *parmPtr);
void Pi__CalcPiQ60_new(TPiParmQ60 *parmPtr);
void Pi__CalcPiQ30_new(TPiParmQ30 *parmPtr);
void Pi__CalcPiQ30(TPiParmQ30 *parmPtr);
void Pi__CalcPiQ60(TPiParmQ60 *parmPtr);
void Pi__InitPiQ60( TPiParmQ60 *parmPtr );



static inline void PI__INITPIQ30( TPiParmQ30 *parmPtr )
{
    parmPtr->sum = 0;
    parmPtr->out = 0;
}   /* end of InitPi() */



static inline void PI__CALCPIQ60_NEW(TPiParmQ60 *parmPtr)
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



static inline void PI__CALCPIQ30_NEW(TPiParmQ30 *parmPtr)
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



static inline void PI__CALCPIQ30(TPiParmQ30 *parmPtr)
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



static inline void PI__CALCPIQ60(TPiParmQ60 *parmPtr)
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




static inline void PI__INITPIQ60( TPiParmQ60 *parmPtr )
{
    parmPtr->sum = 0;
    parmPtr->out = 0;
}






#endif /* PI_H_ */

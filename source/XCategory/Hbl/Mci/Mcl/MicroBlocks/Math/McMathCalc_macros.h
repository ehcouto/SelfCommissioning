/**
 *  @file       McMathCalc_macros.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef MC_MATH_CALC_MACROs_H_
	#define MC_MATH_CALC_MACROs_H_
#include "MclConfig.h"
#include "FloatConstants.h"
#include "MathCalc.h"
#include <math.h>
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#ifndef MC_FLOATING_POINT

//#define MC_SQRT(value)                  MathCalc__SqrtLut((value))
//#define MC_SQRT(value)                  MathCalc__SqrtNwtRph((value))
#define MC_SQRT(value)                  MathCalc__SqrtFloat((value))
#define MC_SINCOS(theta,pointer)        MathCalc__SinCos((theta),(pointer))
#define MC_ATAN2(x,y)                   MathCalc__Arctan2((x),(y))
#define MC_INTERPOLATE(y1,x1,y0,x0,value)   MathCalc__Interpolate((y1),(x1),(y0),(x0),(value))
#define MC_COUNTLEADINGBITS(in)         MathCalc__CountLeadingBitsOnSigned((in))


#define MC_SQRT_F(value)                        MathCalc__SqrtF(value)
#define MC_SINCOS_F(theta,pointer)              MathCalc__SinCosF((theta),(pointer))
#define MC_ATAN2_F(x,y)                         atan2f(y,x)
#define MC_INTERPOLATE_F(y1,x1,y0,x0,value)     MathCalc__f_Interpolate((y1),(x1),(y0),(x0),(value))

// MCI MACROS to manage IQ/FLOAT operations
 //Redefinition for easier reading
#define MC_SCALE(x, qx, qy)            IQMATH__IQXtoIQY(x, qx, qy)
#define MC_SCALE_DOWN_EXP(value,exp)        ((value)>>(exp))
#define MC_SCALE_UP_EXP(value,exp)        ((value)<<(exp))
#define MC_SCALE_UPDOWN_EXP(value,exp)  ( ((exp) >= 0) ? \
                                            ((value) >> (exp)) \
                                            :   ((value) << (-(exp))) )
#define MC_DIV_SH(value,bits)          ((value)>>(bits))
#define MC_MULT_SH(value,bits)         ((value)<<(bits))

#define MC_MULT(a, b, a_format, b_format, out_format)          IQMATH__IQxMUL(a, b, a_format, b_format, out_format)
#define MC_MULT_L(a, b, a_format, b_format, out_format)       (signed long int)(((signed long int)(a) * (signed long int)(b)) >> ((a_format)+(b_format)-(out_format)))
#define MC_MULT_LL(a, b, a_format, b_format, out_format)      (signed long int)(((signed long long int)(a) * (signed long long int)(b)) >> ((a_format)+(b_format)-(out_format)))
#define MC_MULT_LL_LL(a, b, a_format, b_format, out_format)   (((signed long long int)(a) * (signed long long int)(b)) >> ((a_format)+(b_format)-(out_format)))

//!< Perform a multiplication between IQ15 returning a IQ15 (the result can be in the entire 17.15 range without overflow)
#define MC_MULTQ15_LL(a,b)  (sint32)(((sint64)(a) * (sint64)(b)) >> IQ15)

//!< Need to be careful with very low denominators in fixed point logic, this can increase your division error by much because of the dynamic range
#define MC_DIV(a, b, a_format, b_format, out_format)               IQMATH__IQxDIV(a, b, a_format, b_format, out_format)

// for parameters reconstruction
#define MC_MERGE_HILO(hi, lo)   (sint32)(((hi)<<16) + (uint16)(lo))
#define MC_FLOATGAIN(hi,lo,exp)  ((float)(((hi)<<16) + (uint16)(lo))/(float)((1)<<(exp)))
#define MC_FIXEDGAIN(hi,lo,exp, iq) (((sint32)(((hi)<<16) + (uint16)(lo))<<((iq)-IQ15))>>(exp))

#define MC_FLOAT_TO_IQ30(A)         IQMATH__IQX((A), Q30_MAX)
#define MC_FLOAT_TO_IQ15L(A)        IQMATH__IQX((A), Q15_MAX)
#define MC_FLOAT_TO_IQ15S(A)        (sint16) ( ((IQMATH__IQX((A), Q15_MAX)) == Q15_MAX) ? \
                                       (Q15_MAX - 1)   \
                                       : (IQMATH__IQX((A), Q15_MAX))  )


#define MC_RNDQ30Q15(value)     IQMATH__RNDQ30Q15(value)

/*-------------------------------------------------
 * @brief Definitions of constants
 *
 * ------------------------------------------------
 *-------------------------------------------------*/
#define MC_ONE_OVER_TWO           MC_FLOAT_TO_IQ15L(F_ONE_OVER_TWO)
#define MC_ONE_OVER_FOUR          MC_FLOAT_TO_IQ15L(F_ONE_OVER_FOUR)
#define MC_ONE_OVER_FOUR_IQ30     MC_FLOAT_TO_IQ30(F_ONE_OVER_FOUR)
#define MC_ONE_OVER_THREE         MC_FLOAT_TO_IQ15L(F_ONE_OVER_THREE)
#define MC_SQRT2                  MC_FLOAT_TO_IQ15L(F_SQRT2)
#define MC_SQRT2_INV              MC_FLOAT_TO_IQ15L(F_SQRT2_INV)
#define MC_SQRT3                  MC_FLOAT_TO_IQ15L(F_SQRT3)
#define MC_SQRT3_OVER_TWO         MC_FLOAT_TO_IQ15L(F_SQRT3_OVER_TWO)
#define MC_SQRT3_INV              MC_FLOAT_TO_IQ15L(F_SQRT3_INV)
#define MC_ONE                    MC_FLOAT_TO_IQ15S(F_ONE)
#define MC_MINUS_ONE              MC_FLOAT_TO_IQ15S(F_MINUS_ONE)
#define MC_ONE_IQ30               MC_FLOAT_TO_IQ30(F_ONE)
#define MC_MINUS_ONE_IQ30         MC_FLOAT_TO_IQ30(F_MINUS_ONE)


#else

#define MC_SINCOS(theta,pointer)            MathCalc__f_SinCos((theta),(pointer))
#define MC_SQRT(value)                      sqrtf((value))
#define MC_ATAN2(x,y)                       atan2f((y),(x))
#define MC_INTERPOLATE(y1,x1,y0,x0,value)   MathCalc__f_Interpolate((y1),(x1),(y0),(x0),(value))
#define MC_COUNTLEADINGBITS(in)        0


// MCI MACROS to manage IQ/FLOAT operations
 //Redefinition for easier reading
#define MC_SCALE(x, qx, qy)            (x)
#define MC_SCALE_DOWN_EXP(value,exp)   (value)
#define MC_SCALE_UP_EXP(value,exp)     (value)
#define MC_SCALE_UPDOWN_EXP(value,exp) (value)
#define MC_DIV_SH(value,bits)          ((value)/(1<<(bits)))
#define MC_MULT_SH(value,bits)         ((value)*(1<<(bits)))

#define MC_MULT(a, b, a_format, b_format, out_format)      ((a)*(b))
#define MC_MULT_L(a, b, a_format, b_format, out_format)    ((a)*(b))
#define MC_MULT_LL(a, b, a_format, b_format, out_format)   ((a)*(b))
#define MC_MULT_LL_LL(a, b, a_format, b_format, out_format)((a)*(b))

#define MC_MULTQ15_LL(a,b)  ((a)*(b))

//!< Need to be careful with very low denominators in fixed point logic, this can increase your division error by much because of the dynamic range
#define MC_DIV(a, b, a_format, b_format, out_format)  ((a)/(b))

// for parameters reconstruction
#define MC_MERGE_HILO(hi, lo)   hi
#define MC_FLOATGAIN(hi,lo,exp)  ((float)(((hi)<<16) + (uint16)(lo))/(float)((1)<<(exp)))
#define MC_FLOATGAIN_LONG(hi,lo,exp)  ((float)(((hi)<<16) + (uint16)(lo))/(float)((32768)<<(exp)))
#define MC_FLOATGAIN_SHORT(gain,exp)   ((float)(gain)/(float)((32768)<<(exp)))
#define MC_FLOATGAIN_SHORT_HALFEXP_HI(gain,exp) ((float)(gain)/(float)((1)<<((exp)>>8)))
#define MC_FLOATGAIN_SHORT_HALFEXP_LO(gain,exp) ((float)(gain)/(float)((1)<<((exp)&0x0FF)))
#define MC_PARAM_IQ_TO_FLOAT(param) IQMATH__IQ2REALF((param), 1.0, IQ15)

#define MC_FLOAT_TO_IQ30(A) (A)
#define MC_FLOAT_TO_IQ15L(A) (A)
#define MC_FLOAT_TO_IQ15S(A) (A)

#define MC_RNDQ30Q15(value)     ((value))

/*-------------------------------------------------
 * @brief Definitions of constants
 *
 * ------------------------------------------------
 *-------------------------------------------------*/
#define MC_ONE_OVER_TWO           MC_FLOAT_TO_IQ15L(F_ONE_OVER_TWO)
#define MC_ONE_OVER_FOUR          MC_FLOAT_TO_IQ15L(F_ONE_OVER_FOUR)
#define MC_ONE_OVER_FOUR_IQ30     MC_FLOAT_TO_IQ30(F_ONE_OVER_FOUR)
#define MC_ONE_OVER_THREE         MC_FLOAT_TO_IQ15L(F_ONE_OVER_THREE)
#define MC_SQRT2                  MC_FLOAT_TO_IQ15L(F_SQRT2)
#define MC_SQRT2_INV              MC_FLOAT_TO_IQ15L(F_SQRT2_INV)
#define MC_SQRT3                  MC_FLOAT_TO_IQ15L(F_SQRT3)
#define MC_SQRT3_OVER_TWO         MC_FLOAT_TO_IQ15L(F_SQRT3_OVER_TWO)
#define MC_SQRT3_INV              MC_FLOAT_TO_IQ15L(F_SQRT3_INV)
#define MC_ONE                    MC_FLOAT_TO_IQ15S(F_ONE)
#define MC_MINUS_ONE              MC_FLOAT_TO_IQ15S(F_MINUS_ONE)
#define MC_ONE_IQ30               MC_FLOAT_TO_IQ30(F_ONE)
#define MC_MINUS_ONE_IQ30         MC_FLOAT_TO_IQ30(F_MINUS_ONE)

#endif


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


#endif // MC_MATH_CALC_MACROs_H_



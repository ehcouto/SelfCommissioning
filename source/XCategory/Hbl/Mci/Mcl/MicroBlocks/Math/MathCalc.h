/**
 *  @file
 *  @brief      Basic description of file contents
 *
 *  $Header: $
 *
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef MATHCALC_H_
#define MATHCALC_H_

#include "MclBasicTypes.h"
#include "IQMath.h"
#include <math.h>

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#define MATHCALC__ABS(value) (((value) < 0) ? (-(value)) : (value))
#define MATHCALC__FABS(value)   fabsf((value))
#define MATHCALC__SATURATE(lim_inf, value, lim_sup)                                                 \
((lim_sup) >= (lim_inf)) ?                                                                          \
 (((value) >= (lim_sup)) ? (lim_sup) : ( ((value) <= (lim_inf)) ? (lim_inf) : (value) )) :          \
 (((value) >= (lim_inf)) ? (lim_inf) : ( ((value) <= (lim_sup)) ? (lim_sup) : (value) ))

#define MATHCALC__SATURATE_DIRECT(lim_inf, value, lim_sup)                                          \
 (((value) >= (lim_sup)) ? (lim_sup) : ( ((value) <= (lim_inf)) ? (lim_inf) : (value) ))            \

#define MATHCALC__SIGN(value) (((value) < 0) ? (-1) : (1))

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
//-------------  Fixed Point Implementation Section ---------------//
sint32 MathCalc__Interpolate(sint32 y1, sint32 x1,sint32 y0, sint32 x0, sint32 value);
sint32 MathCalc__SqrtLut(sint32 value);
sint32 MathCalc__SqrtFloat(sint32 value);
sint32 MathCalc__SqrtNwtRph(sint32 value);
sint32 MathCalc__Arctan2(sint32 x, sint32 y);
sint32 MathCalc__Sin(sint16 x);
sint32 MathCalc__Cos(sint16 x);
void   MathCalc__SinCosLut(sint32 motTheta, SIN_COS_TYPE *parmPtr);
void   MathCalc__SinCos(sint32 theta,  SIN_COS_TYPE* sincos);
void   MathCalc__GetSinCos(MATHCALC_MOD_SIN_COS_TYPE *data);

//-------------  Interpolation Section ---------------//
sint32 MathCalc__GetInterpolation(sint32 value, MATHCALC_LUT_TYPE * lut, unsigned char size);
void MathCalc__GetInterpolation2(MATHCALC_LUT2_TYPE * in_out, MATHCALC_LUT2_TYPE * lut, unsigned char size);
sint32 MathCalc__GetInterpolationFast(sint32 value, MATHCALC_LUT_EXT_TYPE *lut_ext);
void MathCalc__GetInterpolation2Fast(MATHCALC_LUT2_TYPE * in_out, MATHCALC_LUT2_EXT_TYPE *lut_ext);


// Added later (BEATOA)
uint8 MathCalc__CountLeadingBitsOnSigned(sint32 in);
uint8 MathCalc__CountLeadingBitsOnUnsigned(uint32 in);



//-------------  Floating Point Implementation Section ---------------//
float32 MathCalc__f_Interpolate(float32 y1, float32 x1, float32 y0, float32 x0, float32 value);
void MathCalc__f_SinCos(float32 motTheta, SIN_COS_TYPE *parmPtr);

float32 MathCalc__GetInterpolationFastF(float32 value, MATHCALC_LUT_EXT_F_TYPE *lut_ext);
float32 MathCalc__SqrtF(float32 x);
float32 MathCalc__Arctan2F(float32 x, float32 y);
float32 MathCalc__SinF(float32 x);
float32 MathCalc__CosF(float32 x);
void MathCalc__SinCosF(float32 theta,  SIN_COS_F_TYPE* sincos);
void MathCalc__GetSinCosF(MATHCALC_MOD_SIN_COS_F_TYPE *data);

#endif // MATHCALC_H_



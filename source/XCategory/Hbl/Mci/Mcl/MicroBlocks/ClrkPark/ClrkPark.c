/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Direct and Inverse Clark and Park Transformation functions using MathCalc that works for the Fixed Point and floating point version.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------


#include "ClrkPark.h"
#include "McMathCalc_macros.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate Clarke transforms.
 *
 *  @details   Clarke transformation is given by:
 *   \f[
 *      \begin{align}
 *         & out_{\alpha} = in_a - \frac{in_a + in_b + in_c}{3} \\
 *         & out_{\beta} = \frac{in_b - in_c}{\sqrt{3}} \\
 *         & out_0 = \frac{in_a + in_b + in_c}{3}
 *      \end{align}
 *    \f]
 *
 *  @param[in]  input   = a-b-c components
 *  @param[out] output  = alpha-beta-zero components
 *
 */
void ClrkPark__DirectClarke(ABC_COOR_SYST_TYPE *input , ALPHA_BETA_COOR_SYST_TYPE *output)
{
	output->ZeroSequence = MC_MULT((input->A + input->B + input->C) , MC_ONE_OVER_THREE, IQ15, IQ15, IQ15);
	output->Alpha = input->A - output->ZeroSequence;
	output->Beta = MC_MULT((input->B - input->C), MC_SQRT3_INV, IQ15, IQ15, IQ15);
}



/**
 *  @brief      Calculate Inverse Clarke transforms.
 *
 *  @details  Inverse Clarke transformation is given by:
 *
 *     \f[
 *         \begin{aligned}
 *          & out_a = in_{\alpha} + in_0 \\
 *          & out_b = -\frac{in_{\alpha}}{2} + \frac{in_{\beta}  \cdot \sqrt(3)}{2} + in_0 \\
 *          & out_c = -\frac{in_{\alpha}}{2} - \frac{in_{\beta}  \cdot \sqrt(3)}{2} + in_0
 *         \end{aligned}
 *     \f]
 *
 *  @param[in]    input     = alpha-beta-zero components
 *  @param[out]   output    = a-b-c components
 *
 */
void ClrkPark__InverseClarke(ALPHA_BETA_COOR_SYST_TYPE *input, ABC_COOR_SYST_TYPE *output)
{
    output->A = input->Alpha+input->ZeroSequence;
    output->B = (-input->Alpha*MC_ONE_OVER_TWO + MC_SQRT3_OVER_TWO * input->Beta);
    output->B = MC_SCALE(output->B, IQ30, IQ15)+input->ZeroSequence ; //No Effect in floating point
    output->C = (-input->Alpha*MC_ONE_OVER_TWO - MC_SQRT3_OVER_TWO * input->Beta);
    output->C = MC_SCALE(output->C, IQ30, IQ15)+input->ZeroSequence ; //No Effect in floating point
}



/**
 *  @brief      Calculate the Direct Park Transformation and phase advance
 *
 *  @details Park transformation is given by:
 *			 \f[
 * 				\begin{aligned}
 * 				& out_d = in_{\alpha} * cos(\theta) + in_{\beta} * sin(\theta) \\
 *     			& out_q = -in_{\alpha} * sin(\theta) + in_{\beta} * cos(\theta) \\
 *     			& out_0 = in_0
 * 				\end{aligned}
 * 			\f]
 *
 *  @param[in]  input   = alpha-beta-zero components
 *  @param[in]  sincos  = sine and cosine rotating reference
 *  @param[out] output  = d-q-zero components
 */
void ClrkPark__DirectPark(ALPHA_BETA_COOR_SYST_TYPE *input ,DQ_COOR_SYST_TYPE *output ,SIN_COS_TYPE *sincos)
{
    mc_sint32 tmpD;
    mc_sint32 tmpQ;
    tmpD = input->Alpha * sincos->Cos + input->Beta * sincos->Sin;
    tmpQ = -input->Alpha * sincos->Sin + input->Beta * sincos->Cos;
    output->D = MC_SCALE(tmpD, IQ30, IQ15);
    output->Q = MC_SCALE(tmpQ, IQ30, IQ15);
    output->ZeroSequence =  input->ZeroSequence;
}



/**
 *  @brief      Calculate the Inverse Park Transformation
 *
 *  @details 	Inverse Park Transformation is given by
 *       	\f[
 * 				\begin{aligned}
 * 				& out_{\alpha} = in_d * cos(\theta) - in_q * sin(\theta) \\
 *      		& out_{\beta} = in_d * sin(\theta) + in_q * cos(\theta) \\
 *      		& out_0 = in_0
 * 				\end{aligned}
 * 			\f]
 *  @param[in]  input   = d-q-zero components
 *  @param[in]  sincos  = sine and cosine rotating reference
 *  @param[out] output  = alpha-beta-zero components
 */
void ClrkPark__InversePark(DQ_COOR_SYST_TYPE *input ,ALPHA_BETA_COOR_SYST_TYPE *output ,SIN_COS_TYPE *sincos)
{
    mc_sint32 tmpD;
    mc_sint32 tmpQ;
    tmpD = input->D * sincos->Cos - input->Q * sincos->Sin;
    tmpQ = input->D * sincos->Sin + input->Q * sincos->Cos;
    output->Alpha = MC_SCALE(tmpD, IQ30, IQ15);
    output->Beta = MC_SCALE(tmpQ, IQ30, IQ15);
    output->ZeroSequence =  input->ZeroSequence;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate Clarke transforms.
 *
 *  @details   Clarke transformation is given by:
 *   \f[
 *      \begin{align}
 *         & out_{\alpha} = in_a - \frac{in_a + in_b + in_c}{3} \\
 *         & out_{\beta} = \frac{in_b - in_c}{\sqrt{3}} \\
 *         & out_0 = \frac{in_a + in_b + in_c}{3}
 *      \end{align}
 *    \f]
 *
 *  @param[in]  input   = a-b-c components
 *  @param[out] output  = alpha-beta-zero components
 *
 */
void ClrkPark__DirectClarkeF(ABC_COOR_SYST_F_TYPE *input, ALPHA_BETA_COOR_SYST_F_TYPE *output)
{
    output->ZeroSequence = (input->A + input->B + input->C) * (1.0f/3.0f);
    output->Alpha = input->A - output->ZeroSequence;
    output->Beta = (input->B - input->C)*(1.0f/1.732050807f);
}



/**
 *  @brief      Calculate Inverse Clarke transforms.
 *
 *  @details  Inverse Clarke transformation is given by:
 *
 *     \f[
 *         \begin{aligned}
 *          & out_a = in_{\alpha} + in_0 \\
 *          & out_b = -\frac{in_{\alpha}}{2} + \frac{in_{\beta}  \cdot \sqrt(3)}{2} + in_0 \\
 *          & out_c = -\frac{in_{\alpha}}{2} - \frac{in_{\beta}  \cdot \sqrt(3)}{2} + in_0
 *         \end{aligned}
 *     \f]
 *
 *  @param[in]    input     = alpha-beta-zero components
 *  @param[out]   output    = a-b-c components
 *
 */
void ClrkPark__InverseClarkeF(ALPHA_BETA_COOR_SYST_F_TYPE *input, ABC_COOR_SYST_F_TYPE *output)
{
    output->A = input->Alpha+input->ZeroSequence;
    output->B = (-input->Alpha/(2.0f) + input->Beta*(1.732050807f/2.0f)) + input->ZeroSequence;
    output->C = (-input->Alpha/(2.0f) - input->Beta*(1.732050807f/2.0f)) + input->ZeroSequence;
}



/**
 *  @brief      Calculate the Direct Park Transformation and phase advance
 *
 *  @details Park transformation is given by:
 *           \f[
 *              \begin{aligned}
 *              & out_d = in_{\alpha} * cos(\theta) + in_{\beta} * sin(\theta) \\
 *              & out_q = -in_{\alpha} * sin(\theta) + in_{\beta} * cos(\theta) \\
 *              & out_0 = in_0
 *              \end{aligned}
 *          \f]
 *
 *  @param[in]  input   = alpha-beta-zero components
 *  @param[in]  sincos  = sine and cosine rotating reference
 *  @param[out] output  = d-q-zero components
 */
void ClrkPark__DirectParkF(ALPHA_BETA_COOR_SYST_F_TYPE *input, DQ_COOR_SYST_F_TYPE *output, SIN_COS_F_TYPE *sincos)
{
    output->D = input->Alpha * sincos->Cos + input->Beta * sincos->Sin;
    output->Q = -input->Alpha * sincos->Sin + input->Beta * sincos->Cos;
    output->ZeroSequence =  input->ZeroSequence;
}



/**
 *  @brief      Calculate the Inverse Park Transformation
 *
 *  @details    Inverse Park Transformation is given by
 *          \f[
 *              \begin{aligned}
 *              & out_{\alpha} = in_d * cos(\theta) - in_q * sin(\theta) \\
 *              & out_{\beta} = in_d * sin(\theta) + in_q * cos(\theta) \\
 *              & out_0 = in_0
 *              \end{aligned}
 *          \f]
 *  @param[in]  input   = d-q-zero components
 *  @param[in]  sincos  = sine and cosine rotating reference
 *  @param[out] output  = alpha-beta-zero components
 */
void ClrkPark__InverseParkF(DQ_COOR_SYST_F_TYPE *input, ALPHA_BETA_COOR_SYST_F_TYPE *output, SIN_COS_F_TYPE *sincos)
{
    output->Alpha = input->D * sincos->Cos - input->Q * sincos->Sin;
    output->Beta = input->D * sincos->Sin + input->Q * sincos->Cos;
    output->ZeroSequence =  input->ZeroSequence;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


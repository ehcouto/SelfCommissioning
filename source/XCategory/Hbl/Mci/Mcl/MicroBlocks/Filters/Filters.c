/**
 *  @file
 *  @brief      Basic description of file contents
 *
 *  $Header: $
 *
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Filters.h"
#include "McMathCalc_macros.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------



//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

/**
 * Calculates the moving average based on the parameters passed to this method.
 * @param moving_avg pointer.
 * @return the last averaged sample
 */
uint16 Filters__MovingAverageShort(MOVING_AVG_TYPE *data, sint16 new_sample)
{
    uint32 sum = 0;
    uint16 temp;

    if(data->sample_index < data->nr_samples)
    {
        data->mvg_avg_pointer[data->sample_index] = new_sample;

        data->sample_index++;
        if(data->sample_index < data->nr_samples)
        {
            data->sample_index = 0;
        }

        for(temp = 0; temp < data->nr_samples; temp++)
        {
            sum += data->mvg_avg_pointer[temp];
        }

        temp = (uint16)(sum / data->nr_samples);
    }
    else
    {
        temp = 0;
    }


    return(temp);
}

/**
 * Calculates the moving average with unsigned parameters based on the parameters passed to this method.
 * @param moving_avg pointer.
 * @return the last averaged sample
 */
uint16 Filters__MovAvgUShort(USHORT_MOVING_AVG_TYPE *data, uint16 new_sample)
{
    uint32 sum = 0;
    uint16 temp;

    if(data->sample_index < data->nr_samples)
    {
        data->mvg_avg_pointer[data->sample_index] = new_sample;

        data->sample_index++;
        if(data->sample_index < data->nr_samples)
        {
            data->sample_index = 0;
        }

        for(temp = 0; temp < data->nr_samples; temp++)
        {
            sum += data->mvg_avg_pointer[temp];
        }

        temp = (uint16)(sum / data->nr_samples);
    }
    else
    {
        temp = 0;
    }


    return(temp);
}


// From Horacio
mc_sint32 Filters__SimpleLpf(mc_sint32 x, mc_sint32 y, uint16 shift)
{
    mc_sint64 sum;
    sum = y - MC_DIV_SH(y,shift) + x;
    return (sint32)(MC_DIV_SH(sum,shift));

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      FIR Filter
 *  @details    The implemented FIR Filter has power of 2 number of taps
 *
 *
 *  @param[in]  buffer  = FIR filter buffer; elements are in Q17.15 notation
 *  @return     filtered output in Q17.15 format
 */
mc_sint32 Filters__FIRFilter(mc_sint32 *buffer, FIR_TAPS_TYPE *samples)
{
    mc_sint32  accumulator;
    uint8   i;
    mc_sint32  y;

    accumulator = 0;
    // accumulate all the samples
    for (i=0; i<samples->Value; i++)
    {
        accumulator += buffer[i];
    }

    // divide for the number of taps (>> TapsLog) to get the new filtered samples
    y = MC_DIV_SH(accumulator,(samples->Log));

    return(y);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      First Order Direct Form 1 IIR Filter
 *  @details    The initialization of the buffers of the IIR Filter
 *
 *
 *  @param[in]  params  = IIR filter parameters (coefficients and buffers)
 *  @return     void
 */
void Filters__IIR1Init(IIR1_TYPE *params)
{
    params->x_1 = (mc_sint32)0;
    params->y_1 = (mc_sint32)0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      First-Order Direct Form 1 IIR Filter
 *  @details    The implementation of the IIR Filter
 *   Filter equation:
 *   y(n) = b0*x(n) + b1*x(n-1) - a1*y(n-1)
 *   overflow may occur in case x or y are large numbers [fractional implementation (b*x) > 2 or (a*y)>2 ]
 *  @param[in]  x  = IIR filter input
 *  @param[in]  params  = IIR filter parameters (coefficients and buffers)
 *  @return     void
 */
mc_sint32 Filters__IIR1(mc_sint32 x, IIR1_TYPE *params)
{
    mc_sint32 y;

    // calculate the new output
    y = (mc_sint64)MC_MULT(params->coeff.b0,x,IQ15,IQ15,IQ30) + (mc_sint64)MC_MULT(params->coeff.b1,params->x_1,IQ15,IQ15,IQ30) \
         - (mc_sint64)MC_MULT(params->coeff.a1,params->y_1,IQ15,IQ15,IQ30);

    // saturate if needed
    y = MATHCALC__SATURATE(MC_MINUS_ONE_IQ30, y, MC_ONE_IQ30);

    y = MC_SCALE(y,IQ30,IQ15);
    // save old input and output
    params->x_1 = x;
    params->y_1 = y;
    return(y);

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      First Order Direct Form 1 IIR Filter
 *  @details    The initialization of the buffers of the IIR Filter
 *
 *
 *  @param[in]  params  = IIR filter parameters (coefficients and buffers)
 *  @return     void
 */
void Filters__IIR1InitF(IIR1_F_TYPE *params)
{
    params->x_1 = 0.0f;
    params->y_1 = 0.0f;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      First-Order Direct Form 1 IIR Filter
 *  @details    The implementation of the IIR Filter
 *   Filter equation:
 *   y(n) = b0*x(n) + b1*x(n-1) - a1*y(n-1)
 *   overflow may occur in case x or y are large numbers [fractional implementation (b*x) > 2 or (a*y)>2 ]
 *  @param[in]  x  = IIR filter input
 *  @param[in]  params  = IIR filter parameters (coefficients and buffers)
 *  @return     void
 */
float32 Filters__IIR1F(float32 x, IIR1_F_TYPE *params)
{
    float32 y;

    // calculate the new output
    y = params->coeff.b0 * x + params->coeff.b1 * params->x_1 - params->coeff.a1 * params->y_1;

    // save old input and output
    params->x_1 = x;
    params->y_1 = y;

    return(y);
}


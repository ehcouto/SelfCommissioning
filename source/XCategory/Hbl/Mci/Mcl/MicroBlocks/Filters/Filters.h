/**
 *  @file
 *  @brief      Basic description of file contents
 *
 *  $Header: $
 *
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef FILTERS_H_
#define FILTERS_H_

#include "MclBasicTypes.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    unsigned short int new_sample;
    unsigned short int *mvg_avg_pointer;
    unsigned short int sample_index;
    unsigned short int nr_samples;
} MOVING_AVG_TYPE;

typedef struct {
    unsigned short int new_sample;
    unsigned short int *mvg_avg_pointer;
    unsigned short int sample_index;
    unsigned short int nr_samples;
} USHORT_MOVING_AVG_TYPE;

typedef struct
{
    mc_sint32 b0; /* B0 coefficient of an IIR1 filter */
    mc_sint32 b1; /* B1 coefficient of an IIR1 filter */
    mc_sint32 a1; /* A1 coefficient of an IIR1 filter */
} IIR1_COEFF_TYPE;

typedef struct
{
    IIR1_COEFF_TYPE coeff; /* Sub-structure containing filter coefficients. */
    mc_sint32 y_1; /* Internal accumulator buffer */
    mc_sint32 x_1; /* Input buffer of an IIR1 filter */
} IIR1_TYPE;


typedef struct
{
    float32 b0; /* B0 coefficient of an IIR1 filter */
    float32 b1; /* B1 coefficient of an IIR1 filter */
    float32 a1; /* A1 coefficient of an IIR1 filter */
} IIR1_COEFF_F_TYPE;

typedef struct
{
    IIR1_COEFF_F_TYPE coeff; /* Sub-structure containing filter coefficients. */
    float32 y_1; /* Internal accumulator buffer */
    float32 x_1; /* Input buffer of an IIR1 filter */
} IIR1_F_TYPE;


enum
{
    FILTER_SHIFT_FS_16KHZ_FC_1915HZ = 1, //1
    FILTER_SHIFT_FS_16KHZ_FC_746HZ,      //2
    FILTER_SHIFT_FS_16KHZ_FC_347HZ,      //3
    FILTER_SHIFT_FS_16KHZ_FC_166HZ,      //4
    FILTER_SHIFT_FS_16KHZ_FC_82HZ,       //5
    FILTER_SHIFT_FS_16KHZ_FC_42HZ,       //6
    FILTER_SHIFT_FS_16KHZ_FC_19HZ,       //7
    FILTER_SHIFT_FS_16KHZ_FC_11HZ        //8
};

enum
{
    FILTER_SHIFT_FS_8KHZ_FC_956HZ = 1,   //1         //0.1197
    FILTER_SHIFT_FS_8KHZ_FC_373HZ,       //2         //0.0466
    FILTER_SHIFT_FS_8KHZ_FC_174HZ,       //3         //0.0217
    FILTER_SHIFT_FS_8KHZ_FC_83HZ,        //4         //0.0104
    FILTER_SHIFT_FS_8KHZ_FC_41HZ,        //5         //0.0051
    FILTER_SHIFT_FS_8KHZ_FC_21HZ,        //6         //0.0026
    FILTER_SHIFT_FS_8KHZ_FC_10HZ,        //7         //0.0012
    FILTER_SHIFT_FS_8KHZ_FC_6HZ          //8         //0.0007
};

enum
{
    FILTER_SHIFT_FS_4KHZ_FC_479HZ = 1,  //1
    FILTER_SHIFT_FS_4KHZ_FC_186HZ,      //2
    FILTER_SHIFT_FS_4KHZ_FC_87HZ,       //3
    FILTER_SHIFT_FS_4KHZ_FC_42HZ,       //4
    FILTER_SHIFT_FS_4KHZ_FC_20HZ,       //5
    FILTER_SHIFT_FS_4KHZ_FC_10HZ,       //6
    FILTER_SHIFT_FS_4KHZ_FC_5HZ,        //7
    FILTER_SHIFT_FS_4KHZ_FC_3HZ         //8
};

enum
{
    FILTER_SHIFT_FS_1KHZ_FC_120HZ = 1,  //1
    FILTER_SHIFT_FS_1KHZ_FC_47HZ,       //2
    FILTER_SHIFT_FS_1KHZ_FC_21HZ,       //3
    FILTER_SHIFT_FS_1KHZ_FC_10HZ,       //4
    FILTER_SHIFT_FS_1KHZ_FC_5HZ,        //5
    FILTER_SHIFT_FS_1KHZ_FC_3HZ,        //6
    FILTER_SHIFT_FS_1KHZ_FC_1HZ,        //7
};

enum
{
    FILTER_SHIFT_FS_200HZ_FC_24HZ = 1,  //1
    FILTER_SHIFT_FS_200HZ_FC_9HZ,       //2
    FILTER_SHIFT_FS_200HZ_FC_4HZ,       //3
    FILTER_SHIFT_FS_200HZ_FC_2HZ,       //4
    FILTER_SHIFT_FS_200HZ_FC_1HZ,       //5
};

typedef struct
{
    uint8 Value;    //!< FIR Filter taps (must be a power of 2)
    uint8 Log;      //!< FIR Filter taps expressed as log2
} FIR_TAPS_TYPE;    //!< FIR Filter Taps structure



/*
    FILTER ORDER LOW PASS FILTER
    ============================

    Equation:
        sum = sum - (sum >> shift) + input
        output = sum >> shift

|---------------------------------------------------------|
|           CUT OFF FREQUENCY TABLE FOR FILTER            |
|---------------------------------------------------------|
|SLOTS (ms) || 0.0625  | 0.25  | 1     |  5      |  25    |
|---------------------------------------------------------|
|K | fs(Hz) || 16000   | 4000  | 1000  |  200    |  40    |
|---------------------------------------------------------|
|1 | 0.1197 || 1915.2  | 478.8 | 119.7 |  23.94  |  4.788 |
|2 | 0.0466 || 745.6   | 186.4 | 46.6  |  9.32   |  1.864 |
|3 | 0.0217 || 347.2   | 86.8  | 21.7  |  4.34   |  0.868 |
|4 | 0.0104 || 166.4   | 41.6  | 10.4  |  2.08   |  0.416 |
|5 | 0.0051 || 81.6    | 20.4  | 5.1   |  1.02   |  0.204 |
|6 | 0.0026 || 41.6    | 10.4  | 2.6   |  0.52   |  0.104 |
|7 | 0.0012 || 19.2    | 4.8   | 1.2   |  0.24   |  0.048 |
|8 | 0.0007 || 11.2    | 2.8   | 0.7   |  0.14   |  0.028 |
|---------------------------------------------------------|

Remark:
    Variable "sum" must have the double size of the "new_input" variable
*/

#define FILTERS__FILTER_CALC(new_input, sum, shift) (sum += (-sum>>shift) + new_input)
#define FILTERS__FILTER_GET_FILTERED_VAR(sum, shift) (sum>>shift)

#define FILTERS__LOWPASSFILTER_F(y,a,u) ( (1.0f - (a)) * (y) + (a) * (u) )

// From FOC implementation
#define FILTERS__LOWPASSFILTERQ15(x,y,a,b) (mc_sint32) (MC_SCALE((((mc_sint64) (y) * (mc_sint64) (a))        +   ((mc_sint64) (x) * (mc_sint64) (b))), IQ30,IQ15))  //!< IIR first order low pass filter x and y are in Q17.15, coefficients a and b are in Q1.15
#define FILTERS__LOWPASSFILTERQ30(x,y,a,b) (mc_sint32) (MC_SCALE(((mc_sint64) (y) * (mc_sint64) (a)),IQ30,IQ15) +   ((mc_sint64) (x) * (mc_sint64) (b)))           //!< IIR first order low pass filter x is in Q17.15, y is in Q2.30; coefficients a and b are in Q1.15


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

uint16 Filters__MovingAverageShort(MOVING_AVG_TYPE *data, sint16 new_sample);
uint16 Filters__MovAvgUShort(USHORT_MOVING_AVG_TYPE *data, uint16 new_sample);
mc_sint32 Filters__SimpleLpf(mc_sint32 x, mc_sint32 y, uint16 shift);
mc_sint32 Filters__FIRFilter(mc_sint32 *buffer, FIR_TAPS_TYPE *samples);

void Filters__IIR1Init(IIR1_TYPE *params);
mc_sint32 Filters__IIR1(mc_sint32 x, IIR1_TYPE *params);

void Filters__IIR1InitF(IIR1_F_TYPE *params);
float32 Filters__IIR1F(float32 x, IIR1_F_TYPE *params);

#endif /* FILTERS_H_ */

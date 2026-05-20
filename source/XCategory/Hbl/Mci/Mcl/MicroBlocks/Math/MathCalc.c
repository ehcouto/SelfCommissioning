/**
 *  @file
 *  @brief      MathCalc module performs basic mathematical operations, this is a support module for the MCI.
 *
 *  @details    Detailed description of the file contents
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MathCalc.h"
#include "C_Extensions.h"
#include "FloatConstants.h"
#include <math.h>

#if ((__ICCARM__ == 1) && (__CORE__ == __ARM7EM__))  //  ARM IAR Compiler && CortexM4
#include <arm_math.h>
#endif
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
// Taylor coefficients for sine function (7th order approximation)
//  The coefficients are scaled and divided by two to avoid overflow
#define A1  0xC910      //!< \f$ a_1 = \frac{\pi}{2}         =  1.570796327 \f$ in Q17.15 notation
                        //!< fractional 1st order coefficient for Taylor sin approximation

#define A3  0xFFFFAD51      //!< \f$ a_3 = \frac{-\pi^3}{3!\cdot2^3} = -0.645964098 \f$ in Q17.15 notation
                        //!< fractional 3rd order coefficient for Taylor sin approximation

#define A5  0x0A33      //!< \f$ a_5 = \frac{\pi^5}{5!\cdot2^5}  =  0.079692626 \f$ in Q17.15 notation
                        //!< fractional 5th order coefficient for Taylor sin approximation

#define A7  0xFFFFFF67      //!< \f$ a_7 = \frac{-\pi^7}{7!\cdot2^7} = -0.004681754 \f$ in Q17.15 notation
                        //!< fractional 7th order coefficient for Taylor sin approximation


#define PI_2        0x4000     //!< \f$ \frac{\pi}{2} \f$ in Q1.15 notation
#define MINUS_PI_2  0xC000     //!< \f$ \frac{-\pi}{2} \f$ in Q1.15 notation
#define ONE         0x7FFF     //!<  1 in Q17.15 notation
#define MINUS_ONE   0xFFFF8000     //!< -1 in Q17.15 notation


#ifndef PI
    #define PI 3.1415926535897932384626433832795f
#endif


#ifndef RAD_2_DEG
    #define RAD_2_DEG 57.295779513082320876798154814105f   //180/pi_greco
#endif

#define EPSILON_FLOAT32         2.0f * 1.1920928955078125e-7


#define ATAN2_ITERATION_NR      8   //!< Sets the number of Atan2 iterations.
#define ATAN2_USE_FOR_LOOP      0   //!< 1 - use "for" loop (memory size); 0 - expands the iterations in sequence (speed)







// This lookup table was started in Excel spreadsheet, but it some of its entries were changed properly to fit
// the error within +-0.3% range. Each single entry was simulated.
const sint16 SquareRootTable[] = {
        1200, 2100, 2300, 2700, 6451, 7066, 7633, 8160, 8655, 9123, 9568, 9994, 10402, 10794,
        11173, 11540, 11895, 12240, 12575, 12902, 13221, 13532, 13836, 14133, 14425, 14710,
        14991, 15266, 15536, 15802, 16063, 16320, 16573, 16822, 17068, 17310, 17549, 17784,
        18017, 18246, 18473, 18697, 18918, 19137, 19353, 19567, 19778, 19988, 20195, 20400,
        20603, 20804, 21003, 21200, 21396, 21589, 21781, 21971, 22160, 22347, 22533, 22716,
        22899, 23080, 23260, 23438, 23615, 23790, 23965, 24138, 24309, 24480, 24649, 24818,
        24985, 25151, 25316, 25480, 25642, 25804, 25965, 26125, 26284, 26442, 26598, 26754,
        26910, 27064, 27217, 27370, 27521, 27672, 27822, 27971, 28120, 28267, 28414, 28560,
        28705, 28850, 28994, 29137, 29280, 29421, 29563, 29703, 29843, 29982, 30120, 30258,
        30396, 30532, 30668, 30804, 30938, 31073, 31206, 31339, 31472, 31604, 31735, 31866,
        31996, 32126, 32255, 32384, 32512, 32640
};

#define SQRTROOT_TABLE_SIZE         (sizeof(SquareRootTable)/2)
#define FACTOR_DIVIDER              ((sint32)32768/SQRTROOT_TABLE_SIZE)

#define SIN_TABLE_SIZE          256
#define COS_TABLE_OFFSET        64

static const sint16 sinTable[SIN_TABLE_SIZE] =
{
    0,804,1608,2410,3212,4011,4808,5602,
    6393,7179,7962,8739,9512,10278,11039,11793,
    12539,13279,14010,14732,15446,16151,16846,17530,
    18204,18868,19519,20159,20787,21403,22005,22594,
    23170,23731,24279,24811,25329,25832,26319,26790,
    27245,27683,28105,28510,28898,29268,29621,29956,
    30273,30571,30852,31113,31356,31580,31785,31971,
    32137,32285,32412,32521,32609,32678,32728,32757,
    32767,32757,32728,32678,32609,32521,32412,32285,
    32137,31971,31785,31580,31356,31113,30852,30571,
    30273,29956,29621,29268,28898,28510,28105,27683,
    27245,26790,26319,25832,25329,24811,24279,23731,
    23170,22594,22005,21403,20787,20159,19519,18868,
    18204,17530,16846,16151,15446,14732,14010,13279,
    12539,11793,11039,10278,9512,8739,7962,7179,
    6393,5602,4808,4011,3212,2410,1608,804,
    0,-804,-1608,-2410,-3212,-4011,-4808,-5602,
    -6393,-7179,-7962,-8739,-9512,-10278,-11039,-11793,
    -12539,-13279,-14010,-14732,-15446,-16151,-16846,-17530,
    -18204,-18868,-19519,-20159,-20787,-21403,-22005,-22594,
    -23170,-23731,-24279,-24811,-25329,-25832,-26319,-26790,
    -27245,-27683,-28105,-28510,-28898,-29268,-29621,-29956,
    -30273,-30571,-30852,-31113,-31356,-31580,-31785,-31971,
    -32137,-32285,-32412,-32521,-32609,-32678,-32728,-32757,
    -32767,-32757,-32728,-32678,-32609,-32521,-32412,-32285,
    -32137,-31971,-31785,-31580,-31356,-31113,-30852,-30571,
    -30273,-29956,-29621,-29268,-28898,-28510,-28105,-27683,
    -27245,-26790,-26319,-25832,-25329,-24811,-24279,-23731,
    -23170,-22594,-22005,-21403,-20787,-20159,-19519,-18868,
    -18204,-17530,-16846,-16151,-15446,-14732,-14010,-13279,
    -12539,-11793,-11039,-10278,-9512,-8739,-7962,-7179,
-6393,-5602,-4808,-4011,-3212,-2410,-1608,-804
};

static const unsigned short int ATAN2_TABLE[] =
{
        8192,                                       // arctan(1)
        4836,                                       // arctan(0.5)
        2555,                                       // arctan(0.25)
        1297,                                       // arctan(0.125)
        651,                                        // arctan(0.0625)
        326,                                        // arctan(...)
        163,                                        // arctan(...)
        81,                                         // arctan(...)
        41,                                         // arctan(...)
        20,                                         // arctan(...)
        10,                                         // arctan(...)
        5,                                          // arctan(...)
        3,                                          // arctan(...)
        1,                                          // arctan(...)
        1                                           // arctan(...)
};


// Added later BEATOA
//keep here not to brake the module
#define THREE_LL            0xC0000000L  //!< \f$ {3} \f$ in Q34.30 notation
#define THREE_DIV_2         0x60000000L  //!< \f$ \frac{3}{2} \f$ in Q2.30 notation
#define INV_SQRT_2          23170       //!< \f$ \frac{1}{\sqrt{3}} \f$ in Q1.15 notation

#define DEBRUIJN_SEQ 0x077CB531UL   //!< De Bruijn sequence used to index a single 1 in a 32bit word
#define MAX_32BIT_POSITIVE 0x7FFFFFFFL              //1< 2^31-1
#define MAX_32BIT_NEGATIVE 0x80000000L              //1< -2^31

/** table to convert De Bruijn index to standard index (unsigned version) */
const unsigned char debruijn_to_std_unsigned[32] =
{
    0, 31,  4, 30,  3, 18,  8, 29, 2, 10, 12, 17,  7, 15, 28, 24,
    1,  5, 19,  9, 11, 13, 16, 25, 6, 20, 14, 26, 21, 27, 22, 23
};

/** table to convert De Bruijn index to standard index (signed version- MSB is discarded because it is used for sign) */
const unsigned char debruijn_to_std_signed[32] =
{
    31, 30,  3, 29,  2, 17,  7, 28, 1,  9, 11, 16,  6, 14, 27, 23,
     0,  4, 18,  8, 10, 12, 15, 24, 5, 19, 13, 25, 20, 26, 21, 22
};

#define INTERPOLATION_STEP_IQ   IQ15




#define FAST_MATH_TABLE_SIZE    512

const float32 sinTable_wp_f32[FAST_MATH_TABLE_SIZE + 1] = {
   0.00000000f, 0.01227154f, 0.02454123f, 0.03680722f, 0.04906767f, 0.06132074f,
   0.07356456f, 0.08579731f, 0.09801714f, 0.11022221f, 0.12241068f, 0.13458071f,
   0.14673047f, 0.15885814f, 0.17096189f, 0.18303989f, 0.19509032f, 0.20711138f,
   0.21910124f, 0.23105811f, 0.24298018f, 0.25486566f, 0.26671276f, 0.27851969f,
   0.29028468f, 0.30200595f, 0.31368174f, 0.32531029f, 0.33688985f, 0.34841868f,
   0.35989504f, 0.37131719f, 0.38268343f, 0.39399204f, 0.40524131f, 0.41642956f,
   0.42755509f, 0.43861624f, 0.44961133f, 0.46053871f, 0.47139674f, 0.48218377f,
   0.49289819f, 0.50353838f, 0.51410274f, 0.52458968f, 0.53499762f, 0.54532499f,
   0.55557023f, 0.56573181f, 0.57580819f, 0.58579786f, 0.59569930f, 0.60551104f,
   0.61523159f, 0.62485949f, 0.63439328f, 0.64383154f, 0.65317284f, 0.66241578f,
   0.67155895f, 0.68060100f, 0.68954054f, 0.69837625f, 0.70710678f, 0.71573083f,
   0.72424708f, 0.73265427f, 0.74095113f, 0.74913639f, 0.75720885f, 0.76516727f,
   0.77301045f, 0.78073723f, 0.78834643f, 0.79583690f, 0.80320753f, 0.81045720f,
   0.81758481f, 0.82458930f, 0.83146961f, 0.83822471f, 0.84485357f, 0.85135519f,
   0.85772861f, 0.86397286f, 0.87008699f, 0.87607009f, 0.88192126f, 0.88763962f,
   0.89322430f, 0.89867447f, 0.90398929f, 0.90916798f, 0.91420976f, 0.91911385f,
   0.92387953f, 0.92850608f, 0.93299280f, 0.93733901f, 0.94154407f, 0.94560733f,
   0.94952818f, 0.95330604f, 0.95694034f, 0.96043052f, 0.96377607f, 0.96697647f,
   0.97003125f, 0.97293995f, 0.97570213f, 0.97831737f, 0.98078528f, 0.98310549f,
   0.98527764f, 0.98730142f, 0.98917651f, 0.99090264f, 0.99247953f, 0.99390697f,
   0.99518473f, 0.99631261f, 0.99729046f, 0.99811811f, 0.99879546f, 0.99932238f,
   0.99969882f, 0.99992470f, 1.00000000f, 0.99992470f, 0.99969882f, 0.99932238f,
   0.99879546f, 0.99811811f, 0.99729046f, 0.99631261f, 0.99518473f, 0.99390697f,
   0.99247953f, 0.99090264f, 0.98917651f, 0.98730142f, 0.98527764f, 0.98310549f,
   0.98078528f, 0.97831737f, 0.97570213f, 0.97293995f, 0.97003125f, 0.96697647f,
   0.96377607f, 0.96043052f, 0.95694034f, 0.95330604f, 0.94952818f, 0.94560733f,
   0.94154407f, 0.93733901f, 0.93299280f, 0.92850608f, 0.92387953f, 0.91911385f,
   0.91420976f, 0.90916798f, 0.90398929f, 0.89867447f, 0.89322430f, 0.88763962f,
   0.88192126f, 0.87607009f, 0.87008699f, 0.86397286f, 0.85772861f, 0.85135519f,
   0.84485357f, 0.83822471f, 0.83146961f, 0.82458930f, 0.81758481f, 0.81045720f,
   0.80320753f, 0.79583690f, 0.78834643f, 0.78073723f, 0.77301045f, 0.76516727f,
   0.75720885f, 0.74913639f, 0.74095113f, 0.73265427f, 0.72424708f, 0.71573083f,
   0.70710678f, 0.69837625f, 0.68954054f, 0.68060100f, 0.67155895f, 0.66241578f,
   0.65317284f, 0.64383154f, 0.63439328f, 0.62485949f, 0.61523159f, 0.60551104f,
   0.59569930f, 0.58579786f, 0.57580819f, 0.56573181f, 0.55557023f, 0.54532499f,
   0.53499762f, 0.52458968f, 0.51410274f, 0.50353838f, 0.49289819f, 0.48218377f,
   0.47139674f, 0.46053871f, 0.44961133f, 0.43861624f, 0.42755509f, 0.41642956f,
   0.40524131f, 0.39399204f, 0.38268343f, 0.37131719f, 0.35989504f, 0.34841868f,
   0.33688985f, 0.32531029f, 0.31368174f, 0.30200595f, 0.29028468f, 0.27851969f,
   0.26671276f, 0.25486566f, 0.24298018f, 0.23105811f, 0.21910124f, 0.20711138f,
   0.19509032f, 0.18303989f, 0.17096189f, 0.15885814f, 0.14673047f, 0.13458071f,
   0.12241068f, 0.11022221f, 0.09801714f, 0.08579731f, 0.07356456f, 0.06132074f,
   0.04906767f, 0.03680722f, 0.02454123f, 0.01227154f, 0.00000000f, -0.01227154f,
   -0.02454123f, -0.03680722f, -0.04906767f, -0.06132074f, -0.07356456f,
   -0.08579731f, -0.09801714f, -0.11022221f, -0.12241068f, -0.13458071f,
   -0.14673047f, -0.15885814f, -0.17096189f, -0.18303989f, -0.19509032f,
   -0.20711138f, -0.21910124f, -0.23105811f, -0.24298018f, -0.25486566f,
   -0.26671276f, -0.27851969f, -0.29028468f, -0.30200595f, -0.31368174f,
   -0.32531029f, -0.33688985f, -0.34841868f, -0.35989504f, -0.37131719f,
   -0.38268343f, -0.39399204f, -0.40524131f, -0.41642956f, -0.42755509f,
   -0.43861624f, -0.44961133f, -0.46053871f, -0.47139674f, -0.48218377f,
   -0.49289819f, -0.50353838f, -0.51410274f, -0.52458968f, -0.53499762f,
   -0.54532499f, -0.55557023f, -0.56573181f, -0.57580819f, -0.58579786f,
   -0.59569930f, -0.60551104f, -0.61523159f, -0.62485949f, -0.63439328f,
   -0.64383154f, -0.65317284f, -0.66241578f, -0.67155895f, -0.68060100f,
   -0.68954054f, -0.69837625f, -0.70710678f, -0.71573083f, -0.72424708f,
   -0.73265427f, -0.74095113f, -0.74913639f, -0.75720885f, -0.76516727f,
   -0.77301045f, -0.78073723f, -0.78834643f, -0.79583690f, -0.80320753f,
   -0.81045720f, -0.81758481f, -0.82458930f, -0.83146961f, -0.83822471f,
   -0.84485357f, -0.85135519f, -0.85772861f, -0.86397286f, -0.87008699f,
   -0.87607009f, -0.88192126f, -0.88763962f, -0.89322430f, -0.89867447f,
   -0.90398929f, -0.90916798f, -0.91420976f, -0.91911385f, -0.92387953f,
   -0.92850608f, -0.93299280f, -0.93733901f, -0.94154407f, -0.94560733f,
   -0.94952818f, -0.95330604f, -0.95694034f, -0.96043052f, -0.96377607f,
   -0.96697647f, -0.97003125f, -0.97293995f, -0.97570213f, -0.97831737f,
   -0.98078528f, -0.98310549f, -0.98527764f, -0.98730142f, -0.98917651f,
   -0.99090264f, -0.99247953f, -0.99390697f, -0.99518473f, -0.99631261f,
   -0.99729046f, -0.99811811f, -0.99879546f, -0.99932238f, -0.99969882f,
   -0.99992470f, -1.00000000f, -0.99992470f, -0.99969882f, -0.99932238f,
   -0.99879546f, -0.99811811f, -0.99729046f, -0.99631261f, -0.99518473f,
   -0.99390697f, -0.99247953f, -0.99090264f, -0.98917651f, -0.98730142f,
   -0.98527764f, -0.98310549f, -0.98078528f, -0.97831737f, -0.97570213f,
   -0.97293995f, -0.97003125f, -0.96697647f, -0.96377607f, -0.96043052f,
   -0.95694034f, -0.95330604f, -0.94952818f, -0.94560733f, -0.94154407f,
   -0.93733901f, -0.93299280f, -0.92850608f, -0.92387953f, -0.91911385f,
   -0.91420976f, -0.90916798f, -0.90398929f, -0.89867447f, -0.89322430f,
   -0.88763962f, -0.88192126f, -0.87607009f, -0.87008699f, -0.86397286f,
   -0.85772861f, -0.85135519f, -0.84485357f, -0.83822471f, -0.83146961f,
   -0.82458930f, -0.81758481f, -0.81045720f, -0.80320753f, -0.79583690f,
   -0.78834643f, -0.78073723f, -0.77301045f, -0.76516727f, -0.75720885f,
   -0.74913639f, -0.74095113f, -0.73265427f, -0.72424708f, -0.71573083f,
   -0.70710678f, -0.69837625f, -0.68954054f, -0.68060100f, -0.67155895f,
   -0.66241578f, -0.65317284f, -0.64383154f, -0.63439328f, -0.62485949f,
   -0.61523159f, -0.60551104f, -0.59569930f, -0.58579786f, -0.57580819f,
   -0.56573181f, -0.55557023f, -0.54532499f, -0.53499762f, -0.52458968f,
   -0.51410274f, -0.50353838f, -0.49289819f, -0.48218377f, -0.47139674f,
   -0.46053871f, -0.44961133f, -0.43861624f, -0.42755509f, -0.41642956f,
   -0.40524131f, -0.39399204f, -0.38268343f, -0.37131719f, -0.35989504f,
   -0.34841868f, -0.33688985f, -0.32531029f, -0.31368174f, -0.30200595f,
   -0.29028468f, -0.27851969f, -0.26671276f, -0.25486566f, -0.24298018f,
   -0.23105811f, -0.21910124f, -0.20711138f, -0.19509032f, -0.18303989f,
   -0.17096189f, -0.15885814f, -0.14673047f, -0.13458071f, -0.12241068f,
   -0.11022221f, -0.09801714f, -0.08579731f, -0.07356456f, -0.06132074f,
   -0.04906767f, -0.03680722f, -0.02454123f, -0.01227154f, -0.00000000f
};

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================



/**
 *  @brief      Calculate \f$ sin(\theta) \f$
 *  @details    This function calculates \f$ sin(\theta) \f$ using Taylor 7-th order polynomial.
 *
 *  \f[
 *      \begin{aligned}
 *          & \theta=[-\pi; \pi] \\
 *          & sin(\theta) = \theta - \frac{\theta^3}{3!} + \frac{\theta^5}{5!} - \frac{\theta^7}{7!}\\
 *          & sin(\theta) = \theta \cdot \left (1 - \frac{\theta^2}{6} \cdot \left (1 - \frac{\theta^2}{20} \cdot \left ((1 - \frac{\theta^2}{42}\right )\right )\right)\\
 *          & sin(\theta) = \theta(d_1 + \theta^2(d_3 + \theta^2(d_5 + \theta^2\cdot d_7)))
 *      \end{aligned}
 *  \f]
 *
 * where:
 *
 *  \f[
 *      \begin{aligned}
 *          & d_1 =   1\\
 *          & d_3 = -\frac{1}{3!}\\
 *          & d_5 =  \frac{1}{5!}\\
 *          & d_7 = -\frac{1}{7!}
 *      \end{aligned}
 *  \f]
 *
 * In fractional implementation \f$\theta\f$ value is in range [-1; 1) that corresponds to \f$[-\pi; \pi]\f$,
 * thus the coefficients 'd' needs to be scaled. Assuming \f$ x = \theta \cdot \pi \f$
 *
 * \f[
 *  \begin{aligned}
 *      & sin(x) = x(c_1 + x^2(c_3 + x^2(c_5 + x^2\cdot c_7)))\\ \\
 *      & c_1 = d_1 \cdot \pi\\
 *      & c_3 = d_3 \cdot \pi^3\\
 *      & c_5 = d_5 \cdot \pi^5\\
 *      & c_7 = d_7 \cdot \pi^7
 * \end{aligned}
 * \f]
 *
 *  The 7-th order polynomial approximation of the sine function
 *  has very good accuracy in range [-pi/2; pi/2] of argument, but in wider
 *  range the calculation error is quickly growing up. Therefore the sine
 *  function is approximated only in range [-pi/2; pi/2] and it is used the
 *  symmetry of the sine function [sin(alpha) = sin(pi - alpha)].
 *  Therefore the alpha argument is in range [-pi/2; pi/2) that corresponds
 *  to x [-0.5; 0.5).
 *  To make calculations more precise (because we need to use the value
 *  of x^2 rounded to 16-bit fractional number in our calculations),
 *  the given argument value x (that is in range [-0.5; 0.5)) is shifted
 *  by 1 bit to the left (multiplied by 2), then the calculated x^2 value
 *  is in range [-1; 1) instead of [-0.25; 0.25).
 *  Then the polynomial coefficients 'c' needs to be scaled:
 *
 * \f[
 *      \begin{aligned}
 *          & b_1 = \frac{c_1}{2}\\
 *          & b_3 = \frac{c_3}{2^3}\\
 *          & b_5 = \frac{c_5}{2^5}\\
 *          & b_7 = \frac{c_7}{2^7}
 *      \end{aligned}
 * \f]
 *
 * To avoid the saturation error during the polynomial calculation
 * the coefficients 'b' are divided by 2. After the polynomial calculation
 * the result is multiplied by 2 (shifted by 1 bit to the left)
 * to take the right result (in range [-1; 1)).
 *
 * \f[
 * \begin{aligned}
 *     & a_1 = \frac{b_1}{2} = \frac{\pi}{2^2}         &&=  0.785398163\\
 *     & a_3 = \frac{b_3}{2} = \frac{-\pi}{3!\cdot2^4} &&= -0.322982049\\
 *     & a_5 = \frac{b_5}{2} = \frac{\pi}{5!\cdot2^5}  &&=  0.039846313\\
 *     & a_7 = \frac{b_7}{2} = \frac{-\pi}{7!\cdot2^7} &&= -0.002340877
 * \end{aligned}
 * \f]
 *
 * In Q1.15 fractional representation:
 * \f[
 * \begin{aligned}
 *      & a_1 = 0x6488 \\
 *      & a_3 = 0xD6A9\\
 *      & a_5 = 0x051A\\
 *      & a_7 = 0xFFB3\\ \\
 *      & sin(\pi \cdot x) = (x<<1)\cdot(a_1 + (x<<1)^2(a_3 + (x<<1)^2(a_5 + (x<<1)^2\cdot a_7))) << 1
 * \end{aligned}
 * \f]
 *
 *  @param[in]    x in Q1.15 notation (fractional representation of \f$\theta\f$, \f$\pi\f$ as base angle)
 *  @return       \f$ sin(\pi \cdot x) \f$ in Q1.15 notation
 */
sint32 MathCalc__Sin(sint16 x)
{
    sint16 x_2;
    sint32 sin_x;
    sint32 sq_x_2;

    // preliminary check to avoid overflow
    if (x ==  (sint16) MINUS_PI_2)
    {
        sin_x = (sint32) MINUS_ONE;  // to avoid overflow when calculating x_2 = 2*x
    }
    else if (x ==  (sint16) PI_2)
    {
        sin_x =  ONE;  // to avoid overflow when calculating x_2 = 2*x
    }
    else
    {
        x_2 = x << 1; // 2*theta - calculation with overflow

        if ((x_2^x)<0)
        {
            x_2 = - x_2;
        }

        sq_x_2 = ((sint32)x_2 * (sint32) x_2)>>IQ15;       // (2*x)^2

        sin_x  = (sint32) A7;
        sin_x  = (sin_x * sq_x_2)>>IQ15;  // (2*x)^2 * a7

        sin_x  = sin_x + A5;                // a5 + (2*x)^2 * a7
        sin_x  = (sin_x * sq_x_2)>>IQ15;  // (2*x)^2 * (a5 + (2*x)^2 * a7)

        sin_x  = sin_x + A3;                // a3 + (2*x)^2 * (a5 + (2*x)^2 * a7)
        sin_x  = (sin_x * sq_x_2)>>IQ15;  // (2*x)^2 * (a3 + (2*x)^2 * (a5 + (2*x)^2 * a7))

        sin_x  = sin_x + A1;                    // a1 + (2*x)^2 * (a3 + (2*x)^2 * (a5 + (2*x)^2 * a7))
        sin_x  = (sin_x * (sint32) x_2)>>IQ15; // (2*x) * (a1 + (2*x)^2 * (a3 + (2*x)^2 * (a5 + (2*x)^2 * a7 )))
                                              // IQ15-1 instead of IQ15 to compensate the coefficient scaling
    }
    return (sin_x);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate \f$ cos(\theta) \f$
 *  @details    \f$ cos(\theta) \f$ is calculated as \f$sin(\theta + \frac{\pi}{2})\f$.
 *
 *
 *  @param[in]    x in Q1.15 notation (fractional representation of \f$\theta\f$, \f$\pi\f$ as base angle)
 *  @return       \f$ cos(\pi \cdot x) \f$ in Q1.15 notation
 */
sint32 MathCalc__Cos(sint16 x)
{
    sint32 cos_x;
    x = PI_2 + x;  // x + pi/2
    cos_x = MathCalc__Sin(x);

    return (cos_x);
}



/**
 *  @brief      Calculates the sine and cosine based on alpha, beta and modulus components.
 *
 *  @param      sin, beta and modulus
 *  @return     sine and cosine
 */
void MathCalc__GetSinCos(MATHCALC_MOD_SIN_COS_TYPE *data)
{
    // ************ Sin and Cos calculations **********************
    if(data->Beta > 0)
    {
        data->Sin = 32767;
        if(data->Beta < (data->Mod << 15))
        {
            if(data->Mod == 0)
            {
                data->Mod = 1;
            }
            data->Sin = data->Beta / (data->Mod);  // Q.30 / Q.15 => Q.15;
        }
    }
    else if(data->Beta < 0)
    {
        data->Sin = -32768;
        if((-data->Beta) < (data->Mod << 15))
        {
            if(data->Mod == 0)
            {
                data->Mod = 1;
            }
            data->Sin = data->Beta / (data->Mod);  // Q.30 / Q.15 => Q.15;
        }
    }
    else
    {
        data->Sin = 0;
    }

    if(data->Alpha > 0)
    {
        data->Cos = 32767;
        if(data->Alpha < ( data->Mod << 15))
        {
            if(data->Mod == 0)
            {
                data->Mod = 1;
            }
            data->Cos = data->Alpha / (data->Mod); // Q.30 / Q.15 => Q.15;
        }
    }
    else if(data->Alpha < 0)
    {
        data->Cos = -32768;
        if((-data->Alpha) < (data->Mod << 15))
        {
            if(data->Mod == 0)
            {
                data->Mod = 1;
            }
            data->Cos = data->Alpha / (data->Mod); // Q.30 / Q.15 => Q.15;
        }
    }
    else
    {
        data->Cos = 0;
    }
}


/**
 * It calculates the linear interpolation between two points. The range of x's AND y's variables must to
 * be within -1 and +1 in fixed point (Q15 format). No values outside this range is allowed to be used.
 * The equation is as follow: deltaY/deltaX*(value - x0) + y0.
 * @param y1 is Y coordinate of the point 1
 * @param x1 is X coordinate of the point 1
 * @param y0 is Y coordinate of the point 0
 * @param x0 is X coordinate of the point 0
 * @param value is the value to be interpolated within the x1 and x0
 * @return
 */
sint32 MathCalc__Interpolate(sint32 y1, sint32 x1, sint32 y0, sint32 x0, sint32 value)

{
    sint32 ret_val;
    sint32 temp;

    temp = (y1 - y0); // Q15 - Q15 = Q15
    temp = temp*(value - x0);                               // Q15 * (Q15 - Q15) = Q31

    if( (x1 - x0) )
    {
        temp = temp/(x1 - x0); // Q31/Q15 = Q15
        temp = temp + y0;          // Q15 + Q15 = Q15
    }
    else
    {
        temp = 0;                                   // To prevent any division by 0.
    }

    ret_val = temp;
    return(ret_val);
}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculates the square root of the argument.
 *  @details
 *  This function calculates Sqrt(X) using a Newton-Raphson method (10 iteration is the recommended value).
 *  The input value is truncated to the 16 most significant bits
 *  @param[in]  value   = input value in Q2.30 format
 *  @param[out] ret_val = result in Q17.15 notation
 */
sint32 MathCalc__SqrtNwtRph(sint32 value)
{
    uint8 i;
    sint32 ret_val;
    sint32 x1;

    ret_val = 0;
    //truncating
    value = IQMATH__IQXtoIQY(value, IQ30, IQ15);

    if(value > 0)
    {
        x1 = value;
        ret_val = ( x1 + 1 )>>1; /* +1 for rounding */
        for (i = 10; i > 0; i--)
        {
            ret_val = (ret_val + ((x1<<15) / ret_val))>>1;
        }
    }
    return ret_val;
}







/**
 *  @brief      Calculates the square root of the argument.
 *  @details
 *  This function calculates Sqrt(X) using a LookUpTable approach.
 *  @param[in]  value   = input value in Q2.30 format
 *  @param[out] ret_val = result in Q17.15 notation
 */
sint32 MathCalc__SqrtLut(sint32 value)
{
    sint32 ret_val;
    sint32 value_iq15;

    ret_val = 0;
    //truncating
    value_iq15 = IQMATH__IQXtoIQY(value, IQ30, IQ15);

    // saturation (not to exceed the LUT size)
    if (value_iq15 > IQMATH__IQMAX(IQ15))
    {
        value_iq15 = IQMATH__IQMAX(IQ15);
    }

    if(value > 0)
    {
        ret_val = (sint32)SquareRootTable[value_iq15 / FACTOR_DIVIDER];

        ret_val = (ret_val + (value / ret_val)) >> 1;
        ret_val = (ret_val + (value / ret_val)) >> 1;
        ret_val = (ret_val + (value / ret_val)) >> 1;
    }

    return(ret_val);
}


/**
 *  @brief      Calculates the square root of the argument.
 *  @details
 *  This function calculates Sqrt(X) using the floating point square root function in IAR ARM compiler.
 *  Cast is needed to adapt the in and out values.
 *  @param[in]  value  = input value in Q2.30 format
 *  @param[out] ret_val = result in Q17.15 notation
 */
sint32 MathCalc__SqrtFloat(sint32 value)
{
    sint32 ret_val;

#if ((__ICCARM__ == 1) && (__CORE__ == __ARM7EM__))  //  ARM IAR Compiler && CortexM4

    ret_val = (sint32)sqrtf((float)value);

#else

    ret_val = (sint32)sqrt((float)value);

#endif

    return ret_val;
}


/* Interpolation function usage example
 *      - Call example:
 *           - MathCalc__GetInterpolation(ctrlParm.TeRef, (MATHCALC_LUT_TYPE *)&Mtpa_Lut, MTPA_LUT_SIZE);
 *      - Table example:
 *           const MATHCALC_LUT_TYPE Mtpa_Lut[] =
 *           {
 *                   Input Vector                 Output Vector
 *               {TORQUE_TO_IQ(MTPA_TQ_REF_0), FLUX_TO_IQ(MTPA_FLUX_REF_0)},
 *               {TORQUE_TO_IQ(MTPA_TQ_REF_1), FLUX_TO_IQ(MTPA_FLUX_REF_1)},
 *               {TORQUE_TO_IQ(MTPA_TQ_REF_2), FLUX_TO_IQ(MTPA_FLUX_REF_2)},
 *               {TORQUE_TO_IQ(MTPA_TQ_REF_3), FLUX_TO_IQ(MTPA_FLUX_REF_3)},
 *               {TORQUE_TO_IQ(MTPA_TQ_REF_4), FLUX_TO_IQ(MTPA_FLUX_REF_4)},
 *               {TORQUE_TO_IQ(MTPA_TQ_REF_5), FLUX_TO_IQ(MTPA_FLUX_REF_5)},
 *               {TORQUE_TO_IQ(MTPA_TQ_REF_6), FLUX_TO_IQ(MTPA_FLUX_REF_6)},
 *               {TORQUE_TO_IQ(MTPA_TQ_REF_7), FLUX_TO_IQ(MTPA_FLUX_REF_7)}
 *           };
 *     - Size definition example:
 *         - #define MTPA_LUT_SIZE           (sizeof(Mtpa_Lut)/8)
 */
sint32 MathCalc__GetInterpolation(sint32 value, MATHCALC_LUT_TYPE * lut, unsigned char size)
{
    sint32 index;
    sint32 tmp1;
    sint32 tmp2;
    sint32 output;
    sint32 lut_x_max;
    sint32 lut_x_min;
    sint32 lut_x_step;

    lut_x_max  = lut[size-1].input;
    lut_x_min  = lut[0].input;
    lut_x_step = MATHCALC__ABS(lut[size-1].input - lut[size-2].input);

    if (value >= lut_x_max)
    {
        index = size - 1;
        output = lut[index].output;
    }
    else if (value <= lut_x_min)
    {
        index = 0;
        output = lut[index].output;
    }
    else
    {
        index = (sint32)(MATHCALC__ABS(value-lut[0].input)/lut_x_step);

        tmp1 = MATHCALC__ABS(value - lut[index].input);
        tmp2 = lut[index+1].output - lut[index].output;
        output = lut[index].output + (tmp1*tmp2)/lut_x_step;
    }

    return(output);
}

void MathCalc__GetInterpolation2(MATHCALC_LUT2_TYPE * in_out, MATHCALC_LUT2_TYPE * lut, unsigned char size)
{
    sint32 index;
    sint32 tmp1;
    sint32 tmp2;
    sint32 lut_x_max;
    sint32 lut_x_min;
    sint32 lut_x_step;

    lut_x_max  = lut[size-1].input;
    lut_x_min  = lut[0].input;
    lut_x_step = MATHCALC__ABS(lut[size-1].input - lut[size-2].input);

    if (in_out->input >= lut_x_max)
    {
        index = size - 1;
        in_out->output1 = lut[index].output1;
        in_out->output2 = lut[index].output2;
    }
    else if (in_out->input <= lut_x_min)
    {
        index = 0;
        in_out->output1 = lut[index].output1;
        in_out->output2 = lut[index].output2;
    }
    else
    {
        index = (sint32)(MATHCALC__ABS(in_out->input-lut[0].input)/lut_x_step);

        tmp1 = MATHCALC__ABS(in_out->input - lut[index].input);
        tmp2 = lut[index+1].output1 - lut[index].output1;
        in_out->output1 = lut[index].output1 + (tmp1*tmp2)/lut_x_step;

        tmp2 = lut[index+1].output2 - lut[index].output2;
        in_out->output2 = lut[index].output2 + (tmp1*tmp2)/lut_x_step;
    }
}




sint32 MathCalc__GetInterpolationFast(sint32 value, MATHCALC_LUT_EXT_TYPE *lut_ext)
{
    sint32 index;
    sint32 tmp1;
    sint32 tmp2;
    sint32 output;
    sint32 lut_x_max;
    sint32 lut_x_min;

    sint32 size     = *(lut_ext->sizeof_lut);
    sint32 step_inv = *(lut_ext->step_inv);
    MATHCALC_LUT_TYPE *lut = lut_ext->ptr_LUT;

    lut_x_max  = lut[size-1].input;
    lut_x_min  = lut[0].input;

    if (value >= lut_x_max)
    {
        index = size - 1;
        output = lut[index].output;
    }
    else if (value <= lut_x_min)
    {
        index = 0;
        output = lut[index].output;
    }
    else
    {
        // index calculation -- fast algorithm -- no division required
        // step is already in absolute value
        // input must be monotonically increasing -- abs is not required
        // NO OVERFLOW MANAGEMENT--> always check the STEP and VALUES notation
        // max_delta_y * delta_x must be represented in less than 16bit
        // step_inv must be represented in less than 16bit
        index = ((value-lut[0].input)*step_inv) >> INTERPOLATION_STEP_IQ;
        tmp1 = (value - lut[index].input)*step_inv;
        tmp2 = lut[index+1].output - lut[index].output;
        output = lut[index].output + ((tmp1*tmp2) >> INTERPOLATION_STEP_IQ);
    }

    return(output);
}








void MathCalc__GetInterpolation2Fast(MATHCALC_LUT2_TYPE * in_out, MATHCALC_LUT2_EXT_TYPE *lut_ext)
{
    sint32 index;
    sint32 tmp1;
    sint32 tmp2;
    sint32 lut_x_max;
    sint32 lut_x_min;

    sint32 size     = *(lut_ext->sizeof_lut);
    sint32 step_inv = *(lut_ext->step_inv);
    MATHCALC_LUT2_TYPE *lut = lut_ext->ptr_LUT;

    lut_x_max  = lut[size-1].input;
    lut_x_min  = lut[0].input;

    if (in_out->input >= lut_x_max)
    {
        index = size - 1;
        in_out->output1 = lut[index].output1;
        in_out->output2 = lut[index].output2;
    }
    else if (in_out->input <= lut_x_min)
    {
        index = 0;
        in_out->output1 = lut[index].output1;
        in_out->output2 = lut[index].output2;
    }
    else
    {
        // index calculation -- fast algorithm -- no division required
        // step is already in absolute value
        // input must be monotonically increasing -- abs is not required
        // NO OVERFLOW MANAGEMENT--> always check the STEP and VALUES notation
        // max_delta_y * delta_x must be represented in less than 16bit
        // step_inv must be represented in less than 16bit
        index = ((in_out->input-lut[0].input)*step_inv)>> INTERPOLATION_STEP_IQ;

        tmp1 = (in_out->input - lut[index].input)*step_inv;

        tmp2 = lut[index+1].output1 - lut[index].output1;
        in_out->output1 = lut[index].output1 + ((tmp1*tmp2) >> INTERPOLATION_STEP_IQ);

        tmp2 = lut[index+1].output2 - lut[index].output2;
        in_out->output2 = lut[index].output2 + ((tmp1*tmp2) >> INTERPOLATION_STEP_IQ);
    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate sin and cos from angle, update the values in clrkParkGbl (using lookup table and linear interpolation in between).
 *
 *  @param      motTheta
 *  @return     sinTheta, cosTheta
 */
void MathCalc__SinCosLut(sint32 motTheta, SIN_COS_TYPE *parmPtr)
{
    uint32 tempU32;
    uint32 tableIndex;
    sint32 remainder;
    sint32 y0;
    sint32 y1;

    tempU32 = (uint32)(( (uint16)motTheta ) * (uint32)SIN_TABLE_SIZE);
    tableIndex = tempU32 >> 16;
    remainder = ( tempU32 & 0xFFFF) >> 1;
    if (remainder == 0)
    {
        parmPtr->Sin = sinTable[ tableIndex ];
        parmPtr->Cos = sinTable[ ( tableIndex + COS_TABLE_OFFSET ) & ( SIN_TABLE_SIZE - 1 ) ];
    }
    else
    {
        y0 = sinTable[ tableIndex ];
        y1 = sinTable[ ( tableIndex + 1 ) & ( SIN_TABLE_SIZE - 1 ) ];
        y1 = ( ( y1 - y0 ) * remainder) >> 15;
        parmPtr->Sin = y0 + y1;

        y0 = sinTable[ ( tableIndex + COS_TABLE_OFFSET ) & ( SIN_TABLE_SIZE - 1 ) ];
        y1 = sinTable[ ( tableIndex + COS_TABLE_OFFSET + 1 ) & ( SIN_TABLE_SIZE - 1 ) ];
        y1 = ( ( y1 - y0 ) * remainder) >> 15;
        parmPtr->Cos = y0 + y1;
    }
}

/**
 *  @brief      Calculate the atan2 function. The used method is the CORDIC method.
 *
 *  @param      x and y coordinates of the input vector. They are represent in Q15 with 1 as base.
 *  @return     angle in Q15 with PI as base.
 */
sint32 MathCalc__Arctan2(sint32 x, sint32 y)
{
#if(ATAN2_USE_FOR_LOOP == 1)    // Better for program memory size.
    register sint32 theta;
    register sint32 i;
    register sint32 last_x;
    register sint32t x_temp;
    register sint32 y_temp;

    x_temp = x;
    y_temp = y;

    if( (MATHCALC__ABS(x) < 1024) &&
        (MATHCALC__ABS(y) < 1024) )
    {
        x_temp = x<<5;
        y_temp = y<<5;
    }

    if(x_temp < 0)
    {
        theta = 32768L;
        x_temp = -x_temp;
        y_temp = -y_temp;
    }
    else
    {
        theta = 0;
    }

    for(i = 0; i < ATAN2_ITERATION_NR; i++)
    {
        last_x = x_temp;

        if(y_temp<0)
        {
            x_temp -= y_temp >> i;
            y_temp += last_x >> i;
            theta -= (sint32)ATAN2_TABLE[i];
        }
        else
        {
            x_temp += y_temp >> i;
            y_temp -= last_x >> i;
            theta += (sint32)ATAN2_TABLE[i];
        }
    }

    // The if's below are used to solve some specific non CORDIC convergence,
    // they were found through computer numerical analysis and they are solved
    // one by one manually in order to reduce the final calculated error.
    if(!x)
    {
        if(y == 1)  theta = 16384L;
        if(y == 0)  theta = 0;
    }
#if(ATAN2_ITERATION_NR <= 7)
    else if(x == 1)
    {
        if(y == 9)
        {
            theta = 15230L;
        }
    }
    else if(x == -1)
    {
        if(y == -9)
        {
            theta = -17538L;
        }
    }
    else if(x == 9)
    {
        if(y == -1)
        {
            theta = -1154L;
        }
    }
    else if(x == -9)
    {
        if(y == 1)
        {
            theta = 31614L;
        }
    }
#endif

   return(theta);

#elif(ATAN2_USE_FOR_LOOP == 0)  // Better for program speed.
    register sint32 theta;
    register sint32 last_x;
    register sint32 x_temp;
    register sint32 y_temp;

    x_temp = x;
    y_temp = y;

    // This is the magnifying window. The values of 1024 and the 5 left shifts were found by computer
    // simulation analysis.
    if( (MATHCALC__ABS(x) < 1024) &&
        (MATHCALC__ABS(y) < 1024) )
    {
        x_temp = x<<5;
        y_temp = y<<5;
    }

    // Sets the initial angle to the cordic calculation.
    if(x_temp < 0)
    {
        theta = 32768L;
        x_temp = -x_temp;
        y_temp = -y_temp;
    }
    else
    {
        theta = 0;
    }


    /// 1st Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp;
        y_temp += last_x;
        theta -= (sint32)ATAN2_TABLE[0];
    }
    else
    {
        x_temp += y_temp;
        y_temp -= last_x;
        theta += (sint32)ATAN2_TABLE[0];
    }

    /// 2nd Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 1;
        y_temp += last_x >> 1;
        theta -= (sint32)ATAN2_TABLE[1];
    }
    else
    {
        x_temp += y_temp >> 1;
        y_temp -= last_x >> 1;
        theta += (sint32)ATAN2_TABLE[1];
    }

    /// 3rd Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 2;
        y_temp += last_x >> 2;
        theta -= (sint32)ATAN2_TABLE[2];
    }
    else
    {
        x_temp += y_temp >> 2;
        y_temp -= last_x >> 2;
        theta += (sint32)ATAN2_TABLE[2];
    }

    /// 4th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 3;
        y_temp += last_x >> 3;
        theta -= (sint32)ATAN2_TABLE[3];
    }
    else
    {
        x_temp += y_temp >> 3;
        y_temp -= last_x >> 3;
        theta += (sint32)ATAN2_TABLE[3];
    }

    /// 5th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 4;
        y_temp += last_x >> 4;
        theta -= (sint32)ATAN2_TABLE[4];
    }
    else
    {
        x_temp += y_temp >> 4;
        y_temp -= last_x >> 4;
        theta += (sint32)ATAN2_TABLE[4];
    }

#if(ATAN2_ITERATION_NR >= 6)
    /// 6th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 5;
        y_temp += last_x >> 5;
        theta -= (sint32)ATAN2_TABLE[5];
    }
    else
    {
        x_temp += y_temp >> 5;
        y_temp -= last_x >> 5;
        theta += (sint32)ATAN2_TABLE[5];
    }
#endif

#if(ATAN2_ITERATION_NR >= 7)
    /// 7th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 6;
        y_temp += last_x >> 6;
        theta -= (sint32)ATAN2_TABLE[6];
    }
    else
    {
        x_temp += y_temp >> 6;
        y_temp -= last_x >> 6;
        theta += (sint32)ATAN2_TABLE[6];
    }
#endif

#if(ATAN2_ITERATION_NR >= 8)
    /// 8th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 7;
        y_temp += last_x >> 7;
        theta -= (sint32)ATAN2_TABLE[7];
    }
    else
    {
        x_temp += y_temp >> 7;
        y_temp -= last_x >> 7;
        theta += (sint32)ATAN2_TABLE[7];
    }
#endif

#if(ATAN2_ITERATION_NR >= 9)
    /// 9th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 8;
        y_temp += last_x >> 8;
        theta -= (sint32)ATAN2_TABLE[8];
    }
    else
    {
        x_temp += y_temp >> 8;
        y_temp -= last_x >> 8;
        theta += (sint32)ATAN2_TABLE[8];
    }
#endif

#if(ATAN2_ITERATION_NR >= 10)
    /// 10th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 9;
        y_temp += last_x >> 9;
        theta -= (sint32)ATAN2_TABLE[9];
    }
    else
    {
        x_temp += y_temp >> 9;
        y_temp -= last_x >> 9;
        theta += (sint32)ATAN2_TABLE[9];
    }
#endif

#if(ATAN2_ITERATION_NR >= 11)
    /// 11th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 10;
        y_temp += last_x >> 10;
        theta -= (sint32)ATAN2_TABLE[10];
    }
    else
    {
        x_temp += y_temp >> 10;
        y_temp -= last_x >> 10;
        theta += (sint32)ATAN2_TABLE[10];
    }
#endif

#if(ATAN2_ITERATION_NR >= 12)
    /// 12th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 11;
        y_temp += last_x >> 11;
        theta -= (sint32)ATAN2_TABLE[11];
    }
    else
    {
        x_temp += y_temp >> 11;
        y_temp -= last_x >> 11;
        theta += (sint32)ATAN2_TABLE[11];
    }
#endif

#if(ATAN2_ITERATION_NR >= 13)
    /// 13th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 12;
        y_temp += last_x >> 12;
        theta -= (sint32)ATAN2_TABLE[12];
    }
    else
    {
        x_temp += y_temp >> 12;
        y_temp -= last_x >> 12;
        theta += (sint32)ATAN2_TABLE[12];
    }
#endif

#if(ATAN2_ITERATION_NR >= 14)
    /// 14th Iteration
    /////////////////////////////////////////////
    last_x = x_temp;
    if(y_temp < 0)
    {
        x_temp -= y_temp >> 13;
        y_temp += last_x >> 13;
        theta -= (sint32)ATAN2_TABLE[13];
    }
    else
    {
        x_temp += y_temp >> 13;
        y_temp -= last_x >> 13;
        theta += (sint32t)ATAN2_TABLE[13];
    }
#endif

    // The if's below are used to solve some CORDIC convergence discrete error,
    // they were found through computer numerical analysis and they are solved
    // one by one manually in order to reduce the final calculated error.
    if(!x)
    {
        if(y == 1)  theta = 16384L;
        if(y == 0)  theta = 0;
    }
#if(ATAN2_ITERATION_NR <= 7)
    else if(x == 1)
    {
        if(y == 9)
        {
            theta = 15230L;
        }
    }
    else if(x == -1)
    {
        if(y == -9)
        {
            theta = -17538L;
        }
    }
    else if(x == 9)
    {
        if(y == -1)
        {
            theta = -1154L;
        }
    }
    else if(x == -9)
    {
        if(y == 1)
        {
            theta = 31614L;
        }
    }
#endif

   return((sint16)theta);
#endif
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calculate \f$ sin(\theta) \f$ and \f$ cos(\theta) \f$.
 *  @details    Only \f$ sin(\theta) \f$ is implemented.
 *              \f$ cos(\theta) \f$ is calculated as \f$sin(\theta + \frac{\pi}{2})\f$
 *              \f$ sin(\theta) \f$ is calculated using Taylor 7-th order polynomial
 *  @param[in]  theta = input angle in Q1.15 notation
 *  @param[out] sincos = structure containing sin(theta) and cos(theta) values
 */
void MathCalc__SinCos(sint32 theta,  SIN_COS_TYPE* sincos)
{
        // calculate sin(theta)
        sincos->Sin = MathCalc__Sin((sint16)theta);
        // calculate cos(theta) = sin(theta +pi/2)
        theta = PI_2 + theta;  // theta + pi/2
        sincos->Cos = MathCalc__Sin((sint16)theta);
}







/*************** ADDED LATER  ************* BEATOA */
#if ((__ICCARM__ == 1) && (__CORE__ == __ARM7EM__))  //  ARM IAR Compiler && CortexM4
#include <intrinsics.h>
static inline uint32 AbsSat32(register sint32 val)
{
      register sint32 temp = 1;
              asm volatile( "cmp %0, #0 \n"                   /* Compares Val with 0 */
                              "itt lt \n"                       /* If Val < 0, then executes next two commands */
                              "mvnlt %0, %0 \n"                 /* Val = ~f32Val */
                              "qaddlt %0, %0, %1 \n"            /* Val = Val + 1 */
                              : "+l"(val): "l"(temp));
      return val;
}


uint8 MathCalc__CountLeadingBitsOnSigned(sint32 in)
{
    return (__CLZ(AbsSat32(in)<<1));
}

uint8 MathCalc__CountLeadingBitsOnUnsigned(uint32 in)
{
    return (__CLZ(in));
}

#else
// Not on ARM Architecture


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Count leading bits of a 32 bit signed input (MSB is not considered because it is for sign).
 *  @details    The implemented algorithm uses the de Bruijn sequences and SWAR algorithm.
 *
 *  [Here](http://supertech.csail.mit.edu/papers/debruijn.pdf) and
 *  [here](http://aggregate.org/MAGIC/#Most%20Significant%201%20Bit)
 *  you can find the reference documentation for the overall algorithm.
 *
 *  The steps of the algorithm are the following:
 *
 *          1- check sign: if the input is negative, it is negate (the algorithm requires positive values)
 *
 *          2- build a sint32 value with a single 1 in the MSB position of input value (SWAR algorithm is used)
 *
 *          3- index the single 1 position of step 2
 *
 *          4- translate the index into number of leading bits using a conversion constant array
 *
 *  Respect to the original documentation, the proposed implementation is slightly different because of
 *  the last step of the SWAR algorithm to build a sint32 value with a single 1 in the MSB position
 *  of input value and because of the adopted De Bruijn sequence. This version is taken from this
 *  [link](http://stackoverflow.com/questions/23856596/counting-leading-zeros-in-a-32-bit-unsigned-integer-with-best-algorithm-in-c-pro)
 *
 *  @param[in]  in  = input value
 *  @return     number of leading bits (excluding the MSB because it is used for sign)
 */
uint8 MathCalc__CountLeadingBitsOnSigned(sint32 in)
{
    uint8 out;

    // check sign
    if (in<0)
    {
        if (in == MAX_32BIT_NEGATIVE)
        {
            in = MAX_32BIT_POSITIVE;
        }
        else
        {
            in = -in;
        }
    }


    // isolate a 1: build a sint32 with a single 1 in the MSB position of in
    // SWAR algorithm is used
    in |= in>>1;
    in |= in>>2;
    in |= in>>4;
    in |= in>>8;
    in |= in>>16;   // all ones from LSB to leftmost 1 position
    in += 1;        // one in the (leftmost 1 position + 1) bit --> the conversion array takes it into account

    // Use de Bruijn sequences to efficiently find the position
    in = ((in*DEBRUIJN_SEQ)>>27)&31;
    out = debruijn_to_std_signed[in];

    return out;
}



/**
 *  @brief      Count leading bits of a 32 bit signed input.
 *  @details    The implemented algorithm uses the de Bruijn sequences and SWAR algorithm.
 *
 *  [Here](http://supertech.csail.mit.edu/papers/debruijn.pdf) and
 *  [here](http://aggregate.org/MAGIC/#Most%20Significant%201%20Bit)
 *  you can find the reference documentation for the overall algorithm.
 *
 *  The steps of the algorithm are the following:
 *
 *          1- check for zero: if the input is zero, the result is directly 32
 *
 *          2- build a sint32 value with a single 1 in the MSB position of input value (SWAR algorithm is used)
 *
 *          3- index the single 1 position of step 2
 *
 *          4- translate the index into number of leading bits using a conversion constant array
 *
 *  Respect to the original documentation, the proposed implementation is slightly different because of
 *  the last step of the SWAR algorithm to build a sint32 value with a single 1 in the MSB position
 *  of input value and because of the adopted De Bruijn sequence. This version is taken from this
 *  [link](http://stackoverflow.com/questions/23856596/counting-leading-zeros-in-a-32-bit-unsigned-integer-with-best-algorithm-in-c-pro)
 *
 *  @param[in]  in  = input value
 *  @return     number of leading bits (excluding the MSB because it is used for sign)
 */
uint8 MathCalc__CountLeadingBitsOnUnsigned(uint32 in)
{
    uint8 out;
    // check not zero
    if (in == 0)
    {
        out = 32;
    }
    else
    {
        // isolate a 1: build a sint32 with a single 1 in the MSB position of in
        // SWAR algorithm is used
        in |= in>>1;
        in |= in>>2;
        in |= in>>4;
        in |= in>>8;
        in |= in>>16;   // all ones from LSB to leftmost 1 position
        in += 1;        // one in the (leftmost 1 position + 1) bit --> the conversion array takes it into account

        // Use de Bruijn sequences to efficiently find the position
        in = ((in*DEBRUIJN_SEQ)>>27)&31;
        out = debruijn_to_std_unsigned[in];
    }

    return out;

}


#endif


//------------------------------- Floating Point Section ----------------------------------//
/**
 * It calculates the linear interpolation between two points.
 * The equation is as follow: deltaY/deltaX*(value - x0) + y0.
 * @param y1 is Y coordinate of the point 1
 * @param x1 is X coordinate of the point 1
 * @param y0 is Y coordinate of the point 0
 * @param x0 is X coordinate of the point 0
 * @param value is the value to be interpolated within the x1 and x0
 * @return
 */
float32 MathCalc__f_Interpolate(float32 y1, float32 x1, float32 y0, float32 x0, float32 value)

{
    float32 temp;

    temp = y1 - y0;
    temp = temp*(value - x0);

    if( (x1 - x0) )
    {
        temp = temp/(x1 - x0); // Q31/Q15 = Q15
        temp = temp + y0;          // Q15 + Q15 = Q15
    }
    else
    {
        temp = 0;                                   // To prevent any division by 0.
    }

    return(temp);
}

void MathCalc__f_SinCos(float32 motTheta, SIN_COS_TYPE *parmPtr)
{
    parmPtr->Sin = (mc_sint32) sinf(F_PI * motTheta);
    parmPtr->Cos = (mc_sint32) cosf(F_PI * motTheta);
}



float32 MathCalc__GetInterpolationFastF(float32 value, MATHCALC_LUT_EXT_F_TYPE *lut_ext)
{
    sint32 index;
    float32 tmp1;
    float32 tmp2;
    float32 output;
    // size of lut is the real size of the lut
    sint32 last_available_index  = *(lut_ext->sizeof_lut)-2;

    float32 step_inv = *(lut_ext->step_inv);
    MATHCALC_LUT_F_TYPE *lut = lut_ext->ptr_LUT;


    // index calculation -- fast algorithm -- no division required
    // step is already in absolute value
    // input must be monotonically increasing -- abs is not required
    tmp1 = (value-lut[0].input)*step_inv;

    if(tmp1 < 0.0f)
    {
        index = -1;
    }
    else
    {
        index = (sint32)(tmp1);
    }

    // check index in range
    if (index > last_available_index)
    {
        output = lut[last_available_index+1].output;
    }
    else if (index < 0)
    {
        output = lut[0].output;
    }
    else
    {
        // index is in the correct range
        tmp1 = (value - lut[index].input)*step_inv;
        tmp2 = lut[index+1].output - lut[index].output;
        output = lut[index].output + (tmp1*tmp2);
    }

    return(output);
}



/**
 *  @brief      Calculate \f$ sqrt(x) \f$
 *  @details    This function calculates \f$ sqrt(x) \f$ using c-language math.h library.
 *              These functions return the sqrt of x. The return value is in the range -1 to 1.
 *  @param[in]    x in float
 *  @return       \f$ sqrt(x) \f$ in float
 */
float32 MathCalc__SqrtF(float32 x)
{

#if ((__ICCARM__ == 1) && (__CORE__ == __ARM7EM__))  //  ARM IAR Compiler && CortexM4

    float32 ret;

    arm_sqrt_f32(x,&ret);

    return ret;

#else

    return (sqrtf(x));

#endif

}



/**
 *  @brief      This function computes the arc tangent of y/x, but the signs of both arguments are used to determine the
 * quadrant of the result, and x is permitted to be zero. The return value is given in radians and is in the range -pi to
 * pi, inclusive.
 * If x and y are coordinates of a point in the plane, atan2 returns the signed angle between the line from the origin
 * to that point and the x-axis. Thus, atan2 is useful for converting Cartesian coordinates to polar coordinates. (To
 * compute the radial coordinate, use hypot; see Exponents and Logarithms.)
 * If both x and y are zero, atan2 returns zero.
 *
 *  @param      x and y coordinates of the input vector. They are represent in float.
 *  @return     angle in Q15 with PI as base.
 */
float32 MathCalc__Arctan2F(float32 x, float32 y)
{
    return atan2f(y,x);
}



/**
 *  @brief      Calculate \f$ sin(\theta) \f$
 *  @details    This function calculates \f$ sin(\theta) \f$ using based on CMSIS library.
 *              These functions return the sine of x, where x is given in radiant. The return value is in the range -1 to 1.
 *  @param[in]    x in float - angle in radiant
 *  @return       \f$ sin(\pi \cdot x) \f$ in float32
 */
float32 MathCalc__SinF(float32 x)
{
    float32 sinVal, fract, in;                           /* Temporary variables for input, output */
    uint16 index;                                        /* Index variable */
    float32 a, b;                                        /* Two nearest output values */
    sint32 n;
    float32 findex;

    /* input x is in radians */
    /* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi */
    in = x * 0.159154943092f;

    /* Calculation of floor value of input */
    n = (sint32) in;

    /* Make negative values towards -infinity */
    if(in < -EPSILON_FLOAT32)
    {
      n--;
    }

    /* Map input value to [0 1] */
    in = in - (float32) n;

    /* Calculation of index of the table */
    findex = (float32) FAST_MATH_TABLE_SIZE * in;
    if (findex >= 512.0f) {
      findex -= 512.0f;
    }

    index = ((uint16)findex) & 0x1ff;

    /* fractional value calculation */
    fract = findex - (float32) index;

    /* Read two nearest values of input value from the sin table */
    a = sinTable_wp_f32[index];
    b = sinTable_wp_f32[index+1];

    /* Linear interpolation process */
    sinVal = (1.0f-fract)*a + fract*b;

    /* Return the output value */
    return (sinVal);
}



/**
 *  @brief      Calculate \f$ cos(\theta) \f$
 *  @details    This function calculates \f$ cos(\theta) \f$ using c-language based on CMSIS library.
 *              These functions return the cosine of x, where x is given in radiant. The return value is in the range -1 to 1.
 *  @param[in]    x in float - angle in radiant
 *  @return       \f$ sin(\pi \cdot x) \f$ in float
 */
float32 MathCalc__CosF(float32 x)
{
    float32 cosVal, fract, in;                   /* Temporary variables for input, output */
    uint16 index;                                /* Index variable */
    float32 a, b;                                /* Two nearest output values */
    sint32 n;
    float32 findex;

    /* input x is in radians */
    /* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi, add 0.25 (pi/2) to read sine table */
    in = x * 0.159154943092f + 0.25f;

    /* Calculation of floor value of input */
    n = (sint32) in;

    /* Make negative values towards -infinity */
    if(in < -EPSILON_FLOAT32)
    {
      n--;
    }

    /* Map input value to [0 1] */
    in = in - (float32) n;

    /* Calculation of index of the table */
    findex = (float32) FAST_MATH_TABLE_SIZE * in;
    index = ((uint16)findex) & 0x1ff;

    /* fractional value calculation */
    fract = findex - (float32) index;

    /* Read two nearest values of input value from the cos table */
    a = sinTable_wp_f32[index];
    b = sinTable_wp_f32[index+1];

    /* Linear interpolation process */
    cosVal = (1.0f-fract)*a + fract*b;

    /* Return the output value */
    return (cosVal);
}



/**
 *  @brief      Calculate \f$ sin(\theta) \f$ and \f$ cos(\theta) \f$.
 *  @details    Only \f$ sin(\theta) \f$ is implemented.
 *              \f$ cos(\theta) \f$ is c-language based on CMSIS library.
 *              \f$ sin(\theta) \f$ is c-language based on CMSIS library.
 *  @param[in]  theta = input angle in radiant
 *  @param[out] sincos = structure containing sin(theta) and cos(theta) values
 */
void MathCalc__SinCosF(float32 theta,  SIN_COS_F_TYPE* sincos)
{
    float32 fract, in;                             /* Temporary variables for input, output */
    uint16 indexS, indexC;                         /* Index variable */
    float32 f1, f2, d1, d2;                        /* Two nearest output values */
    sint32 n;
    float32 findex;

    /* input x is in degrees */
    /* Scale the input, divide input by 2*pi, for cosine add 0.25 (pi/2) to read sine table */
    in = theta * 0.159154943092f;


    /* Calculation of floor value of input */
    n = (sint32) in;

    /* Make negative values towards -infinity */
    if(in < -EPSILON_FLOAT32)
    {
      n--;
    }
    /* Map input value to [0 1] */
    in = in - (float32) n;

    /* Calculation of index of the table */
    findex = (float32) FAST_MATH_TABLE_SIZE * in;
    indexS = ((uint16)findex) & 0x1ff;
    indexC = (indexS + (FAST_MATH_TABLE_SIZE / 4)) & 0x1ff;

    /* fractional value calculation */
    fract = findex - (float32) indexS;

    /* Read two nearest values of input value from the cos & sin tables */
    f1 = sinTable_wp_f32[indexC];
    f2 = sinTable_wp_f32[indexC+1];
    d1 = sinTable_wp_f32[indexS];
    d2 = sinTable_wp_f32[indexS+1];


    /* Calculation of cosine value */
    sincos->Cos = (1.0f-fract)*f1 + fract*f2;

    /* Calculation of sine value */
    sincos->Sin = (1.0f-fract)*d1 + fract*d2;
}



/**
 *  @brief      Calculates the sine and cosine based on alpha, beta and modulus components.
 *
 *  @param      sin, beta and modulus
 *  @return     sine and cosine
 */
void MathCalc__GetSinCosF(MATHCALC_MOD_SIN_COS_F_TYPE *data)
{
    // ************ Sin and Cos calculations **********************
    if(data->Beta > 0.0f)
    {
        data->Sin = 1.0f;
        if(data->Beta < data->Mod)
        {
            data->Sin = data->Beta / (data->Mod);
        }
    }
    else if(data->Beta < 0.0f)
    {
        data->Sin = -1.0f;
        if((-data->Beta) < data->Mod)
        {
            data->Sin = data->Beta / (data->Mod);
        }
    }
    else
    {
        data->Sin = 0.0f;
    }

    if(data->Alpha > 0.0f)
    {
        data->Cos = 1.0f;
        if(data->Alpha < data->Mod)
        {
            data->Cos = data->Alpha / (data->Mod);
        }
    }
    else if(data->Alpha < 0.0f)
    {
        data->Cos = -1.0f;
        if((-data->Alpha) < data->Mod)
        {
            data->Cos = data->Alpha / (data->Mod);
        }
    }
    else
    {
        data->Cos = 0.0f;
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


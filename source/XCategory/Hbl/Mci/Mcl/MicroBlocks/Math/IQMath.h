/*
 * IQMath.h
 *
 *  Created on: 30 apr 2016
 *      Author: alessio.beato
 */

#ifndef IQMATH_H_
#define IQMATH_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//Fixed Point Math defines
#define IQ60         60
#define IQ44         44
#define IQ40         40
#define IQ39         39
#define IQ38         38
#define IQ37         37
#define IQ36         36
#define IQ35         35
#define IQ34         34
#define IQ33         33
#define IQ32         32
#define IQ31         31
#define IQ30         30
#define IQ29         29
#define IQ28         28
#define IQ27         27
#define IQ26         26
#define IQ25         25
#define IQ24         24
#define IQ23         23
#define IQ22         22
#define IQ21         21

#define IQ20         20
#define IQ19         19
#define IQ18         18
#define IQ17         17
#define IQ16         16
#define IQ15         15
#define IQ14         14
#define IQ13         13
#define IQ12         12
#define IQ11         11

#define IQ10         10
#define IQ9          9
#define IQ8          8
#define IQ7          7
#define IQ6          6
#define IQ5          5
#define IQ4          4
#define IQ3          3
#define IQ2          2
#define IQ1          1

typedef signed long int IQMATH_IQ_TYPE;

//--- maximum value of Qx type data ---
#define Q30_MAX     1073741824L //2^30: 0x40000000
#define Q29_MAX     536870912L  //2^29: 0x20000000
#define Q28_MAX     268435456L  //2^28: 0x10000000
#define Q27_MAX     134217728L  //2^27: 0x08000000
#define Q26_MAX     67108864L   //2^26: 0x04000000
#define Q25_MAX     33554432L   //2^25: 0x02000000
#define Q24_MAX     16777216L   //2^24: 0x01000000
#define Q23_MAX     8388608L    //2^23: 0x00800000
#define Q22_MAX     4194304L    //2^22: 0x00400000
#define Q21_MAX     2097152L    //2^21: 0x00200000
#define Q20_MAX     1048576L    //2^20: 0x00100000
#define Q19_MAX     524288L     //2^19: 0x00080000
#define Q18_MAX     262144L     //2^18: 0x00040000
#define Q17_MAX     131072L     //2^17: 0x00020000
#define Q16_MAX     65536L      //2^16: 0x00010000
#define Q15_MAX     32768L      //2^15: 0x00008000
#define Q14_MAX     16384L      //2^14: 0x00004000
#define Q13_MAX     8192L       //2^13: 0x00002000
#define Q12_MAX     4096L       //2^12: 0x00001000
#define Q11_MAX     2048L       //2^11: 0x00000800
#define Q10_MAX     1024L       //2^10: 0x00000400
#define Q9_MAX      512L        //2^9:  0x00000200
#define Q8_MAX      256L        //2^8:  0x00000100
#define Q7_MAX      128L        //2^7 : 0x00000080
#define Q6_MAX      64L         //2^6 : 0x00000040
#define Q5_MAX      32L         //2^5 : 0x00000030
#define Q4_MAX      16L         //2^4 : 0x00000010
#define Q3_MAX      8L          //2^3 : 0x00000008
#define Q2_MAX      4L          //2^2 : 0x00000004
#define Q1_MAX      2L          //2^1 : 0x00000002


#define MAX_16BIT_POSITIVE 0x00007FFFL              //1< 2^15-1: 0x00007FFF
#define MAX_16BIT_NEGATIVE 0x00008000L              //1< -2^15:  0x00008000
#define MAX_31BIT_POSITIVE 0x3FFFFFFFL              //1< 2^30-1
#define MAX_31BIT_NEGATIVE 0xC0000000L              //1< -2^30
#define MAX_32BIT_POSITIVE 0x7FFFFFFFL              //1< 2^31-1
#define MAX_32BIT_NEGATIVE 0x80000000L              //1< -2^31

#define IQMATH__IQX(a, q_max)   ( ((a) < 0.0) ?    (IQMATH_IQ_TYPE)((q_max) * (a) - 0.5) \
                                             :    (IQMATH_IQ_TYPE)((q_max) * (a) + 0.5) )

#define IQMATH__IQ30(A) IQMATH__IQX((A), Q30_MAX)

#define IQMATH__IQ29(A) IQMATH__IQX((A), Q29_MAX)
#define IQMATH__IQ28(A) IQMATH__IQX((A), Q28_MAX)
#define IQMATH__IQ27(A) IQMATH__IQX((A), Q27_MAX)
#define IQMATH__IQ26(A) IQMATH__IQX((A), Q26_MAX)
#define IQMATH__IQ25(A) IQMATH__IQX((A), Q25_MAX)
#define IQMATH__IQ24(A) IQMATH__IQX((A), Q24_MAX)
#define IQMATH__IQ23(A) IQMATH__IQX((A), Q23_MAX)
#define IQMATH__IQ22(A) IQMATH__IQX((A), Q22_MAX)
#define IQMATH__IQ21(A) IQMATH__IQX((A), Q21_MAX)
#define IQMATH__IQ20(A) IQMATH__IQX((A), Q20_MAX)

#define IQMATH__IQ19(A) IQMATH__IQX((A), Q19_MAX)
#define IQMATH__IQ18(A) IQMATH__IQX((A), Q18_MAX)
#define IQMATH__IQ17(A) IQMATH__IQX((A), Q17_MAX)
#define IQMATH__IQ16(A) IQMATH__IQX((A), Q16_MAX)
#define IQMATH__IQ15(A) IQMATH__IQX((A), Q15_MAX)
#define IQMATH__IQ14(A) IQMATH__IQX((A), Q14_MAX)
#define IQMATH__IQ13(A) IQMATH__IQX((A), Q13_MAX)
#define IQMATH__IQ12(A) IQMATH__IQX((A), Q12_MAX)
#define IQMATH__IQ11(A) IQMATH__IQX((A), Q11_MAX)
#define IQMATH__IQ10(A) IQMATH__IQX((A), Q10_MAX)

#define IQMATH__IQ9(A)  IQMATH__IQX((A), Q9_MAX)
#define IQMATH__IQ8(A)  IQMATH__IQX((A), Q8_MAX)
#define IQMATH__IQ7(A)  IQMATH__IQX((A), Q7_MAX)
#define IQMATH__IQ6(A)  IQMATH__IQX((A), Q6_MAX)
#define IQMATH__IQ5(A)  IQMATH__IQX((A), Q5_MAX)
#define IQMATH__IQ4(A)  IQMATH__IQX((A), Q4_MAX)
#define IQMATH__IQ3(A)  IQMATH__IQX((A), Q3_MAX)
#define IQMATH__IQ2(A)  IQMATH__IQX((A), Q2_MAX)
#define IQMATH__IQ1(A)  IQMATH__IQX((A), Q1_MAX)

/**
 *----------------------------------------------------------
 * @brief Division of numbers that are up to 16bits
 * @param a (var size of 16bits)
 * @param a_format (var size of 16bits)
 * @param b (var size of 16bits)
 * @param b_format (var size of 16bits)
 * @return (a/b) in Q15 format (var size of 16bits)
 *----------------------------------------------------------
 */
#define IQMATH__IQxDIV(a, b, a_format, b_format, out_format)        ( ((out_format + (b_format) - (a_format))>=0) ? \
                                                                    ( (((signed long int)(a) << (out_format + (b_format) - (a_format))) / (signed long int)(b)) )     \
                                                                    :       ( (((signed long int)(a) / (signed long int)(b)) >> (-out_format - (b_format) + (a_format)) ) ) )

#define IQMATH__IQ15DIV(a, a_format, b, b_format)         ( ((15 + (b_format) - (a_format))>=0) ? \
                                                            ( (((signed long int)(a) << (15 + (b_format) - (a_format))) / (signed long int)(b)) )     \
                                                    :       ( (((signed long int)(a) / (signed long int)(b)) >> (-15 - (b_format) + (a_format)) ) ) )

#define IQMATH__IQ30DIV(a, a_format, b, b_format)           ( ((30 + (b_format) - (a_format))>=0) ? \
                                                            ( (signed long int)(((signed long long int)(a) << (30 + (b_format) - (a_format))) / (signed long long int)(b)) )     \
                                                    :       ( (signed long int)(((signed long long int)(a) / (signed long long int)(b)) >> (-30 - (b_format) + (a_format)) ) ) )

//This performs multiplication of two numbers in q format. The result is stored in a 32bits variable and the q format of the result is specified
//The following condition shall be satisfied for this macro: (a_format)+(b_format) >= (out_format)
#define IQMATH__IQxMUL(a, b, a_format, b_format, out_format)    ( (((a_format)+(b_format))>30) ? \
                                                                ( (signed long int)(((signed long long int)(a) * (signed long long int)(b)) >> ((a_format)+(b_format)-(out_format))) ) \
                                                        :       ( (signed long int)(((signed long int)(a) * (signed long int)(b)) >> ((a_format)+(b_format)-(out_format))) ) )



#define Q15Mul(a, b, a_format, b_format)                    ( (((a_format)+(b_format))>15) ? \
                                                            ( ((signed long int)(a) * (signed long int)(b)) >> ((a_format)+(b_format)-15) ) \
                                                    :       ( ((signed long int)(a) * (signed long int)(b)) << (15-(a_format)-(b_format)) ) )

#define Q30Mul(a, b, a_format, b_format)                    ( (((a_format)+(b_format))>30) ? \
                                                            ( (signed long int)(((signed long long int)(a) * (signed long long int)(b)) >> ((a_format)+(b_format)-30)) ) \
                                                    :       ( (signed long int)(((signed long int)(a) * (signed long int)(b)) << (30-(a_format)-(b_format))) ) )

#define IQMATH__IQ15MUL(a, b, a_format, b_format)           ( (((a_format)+(b_format))>30) ? \
                                                            ((signed long int)((Q30Mul((a), (b), (a_format), (b_format))) >> 15))    \
                                                    :       ((signed long int)Q15Mul((a), (b), (a_format), (b_format))) )


//This performs multiplication of two numbers in q format. The result is stored in a 32bits variable with Q30 format
#define IQMATH__IQ30MUL(a, b, a_format, b_format)           Q30Mul((a), (b), (a_format), (b_format))


/**
 *-------------------------------------------------
 * @brief Subtraction
 * @param m
 *-------------------------------------------------
 */
//TODO fp - add comments about the macros operation
#define   IQMATH__IQ15SUBQ30(x, y)   ( (signed long int)(((x) - (y)) >> 15) )
#define   IQMATH__IQ30SUBQ30(x, y)   ( (signed long int)((x) - (y)) )
#define   IQMATH__IQ15SUBQ15(x, y)   ( (signed long int)((x) - (y)) )
#define   IQMATH__IQ30SUBQ15(x, y)   ( ((signed long int)(x) - (signed long int)(y)) << 15 )

/**
 *-------------------------------------------------
 * @brief Sum
 * @param m
 *-------------------------------------------------
 */
#define   IQMATH__IQ15SUMQ30(x, y)   ( (signed short int)(((x) + (y)) >> 15) )
#define   IQMATH__IQ30SUMQ30(x, y)   ( (signed long int)((x) + (y)) )
#define   IQMATH__IQ15SUMQ15(x, y)   ( (signed short int)((x) + (y)) )
#define   IQMATH__IQ30SUMQ15(x, y)   ( ((signed long int)(x) + (signed long int)(y)) << 15 )




/**
 *-------------------------------------------------
 * @brief Convert IQ to IQ1 ~ IQ30
 * @param m
 *-------------------------------------------------
 */
#define   IQMATH__IQXtoIQY(x, qx, qy)     ( ((qx)>(qy)) ?         \
                                              ((x) >> ((qx)-(qy)))    \
                                            : ((signed long int)(x) << ((qy)-(qx))))

/**
 *-------------------------------------------------
 * @brief Convert numbers in real unit to q format
 * @param value, base, q format
 *-------------------------------------------------
 */
#define IQMATH__REAL2IQ(value, base, format)    (((value) * (1 << (format)))/(base))

/**
 *-------------------------------------------------
 * @brief Convert numbers from q format units to real units
 * @param value, base, qformat
 *-------------------------------------------------
 */
#define IQMATH__IQ2REAL(value, base, format)    (((value)   * (signed long int)(base)) >> (format))


/**
 *-------------------------------------------------
 * @brief Convert numbers from q format units to real units
 * @param value, base, qformat
 *-------------------------------------------------
 */
#define IQMATH__IQ2REALF(value, base, format)    (((value) * (float)(base)) / (float)(1<<(format)))

/**
 *-------------------------------------------------
 * @brief Convert iq number from one base to other base
 * @param value, qformat from, qformat to
 *-------------------------------------------------
 */
#define IQMATH__IQ2IQ(value, from, to)    ( ((from)>=(to)) ? \
                                    ((value) >> ((from)-(to))) \
                                :   ((value) << ((to)-(from))) )

/**
 *-------------------------------------------------
 * @brief Get the maximum integer number of the iq representation
 * @param qformat
 *-------------------------------------------------
 */
#define IQMATH__IQMAX(qformat)            ( ((qformat) >= 0) ? \
                                            ((1 << (qformat)) - 1) \
                                        :   (-(1 << (-qformat))))



/**
 * @brief Round Q30 to Q15 (not saturated)
 * @param value   value to be rounded
 */
#define IQMATH__RNDQ30Q15(value)  (((value) + (sint32)0x00004000)>>15)


#define IQMATH__FLOATTOIQ15(x) ((sint16)((x) < 0.999969482421875 ? ((x) >= -1 ? (x)*MAX_16BIT_NEGATIVE : MAX_16BIT_NEGATIVE) : MAX_16BIT_POSITIVE))     //!< Convert int/float to Frac16; constant x generates compile time constant
#define IQMATH__FLOATTOIQ32(x) ((sint32)((x) < 1 ? ((x) >= -1 ? (x)*MAX_32BIT_NEGATIVE : MAX_32BIT_NEGATIVE) : MAX_32BIT_POSITIVE))                     //!< Convert int/float to Frac32; constant x generates compile time constant
#define IQMATH__Q15MULQ15(a,b)  (sint16)(((sint32) (a)  * (sint32) (b)) >> IQ15)    //!< Multiplication of two numbers in Q1.15 notation. The result is Q1.15 notation
#define IQMATH__Q15MULQx(a,b,x) (sint16)(((sint32) (a)  * (sint32) (b)) >> (x))      //!< Multiplication of two numbers in Q1.15 notation. The result is Q1.(30-x) notation

#define IQMATH__I17Q15MULI17Q15(a,b)  (sint32)(((sint64) (a)  * (sint64) (b)) >> IQ15)    //!< Multiplication of two numbers in Q17.15 notation. The result is Q17.15 notation
#define IQMATH__I2Q30MULI17Q15(a,b)  (sint32)(((sint64) (a)  * (sint64) (b)))    //!< Multiplication of two numbers in Q17.15 notation. The result is Q2.30 notation. Overflow may occurs if the result is higher than 2^31.
#define IQMATH__I1Q31MULI17Q15(a,b)  (sint32)((((sint64) (a)  * (sint64) (b))) << 1)   //!< Multiplication of two numbers in Q17.15 notation. The result is Q1.31 notation. Overflow may occurs if the result is higher than 2^31.
#define IQMATH__I1Q31MULI1Q15(a,b)   (sint32)((((sint32) (a)  * (sint32) (b))) << 1)   //!< Multiplication of two numbers in Q1.15 notation. The result is Q1.31 notation. Overflow may occurs if the result is higher than 2^31.
#define IQMATH__I2Q30MULI17Q15_I2Q30(a,b)  (sint32)(( (sint64) (a) * (sint64) (b)) >> IQ15)    //!< Multiplication of a number Q17.15 with number Q2.30 . The result is Q2.30 notation
#define IQMATH__I2Q30MULI17Q15_I2Q30_SHIFT_X(a,b,x)  (sint32)(( (sint64) (a) * (sint64) (b)) >> (IQ15-x))    //!< Multiplication of a number Q17.15 with number Q2.30<<x . The result is Q2.30 notation
#define IQMATH__I17Q15MULQx(a,b,x) (sint32)(( (sint64) (a) * (sint64) (b)) >> (x))     //!< Multiplication of two numbers in Q17.15 notation. The result is Q1.(30-x) notation



#endif /* IQMATH_H_ */

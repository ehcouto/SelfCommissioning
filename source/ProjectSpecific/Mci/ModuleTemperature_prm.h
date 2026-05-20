/**
 *  @file       ModuleTemperature_prm.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef MODULETEMPERATURE_PRM_H_
#define MODULETEMPERATURE_PRM_H_

#include "AtoD.h"
#include "Mci_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#if(HAVANA_BOARD == 1)
    #define MODULE_TEMP_ADC_IN_CHANNEL          ATOD_CH11
    #define MODULE_TEMP_ADC_RESOLUTION          ATOD_RESOLUTION_10BITS
#elif(ETNA_BOARD == 1)
    #define MODULE_TEMP_ADC_IN_CHANNEL          ATOD_CH3
    #define MODULE_TEMP_ADC_RESOLUTION          ATOD_RESOLUTION_10BITS
#elif(WEBER2_BOARD == 1)
    #define MODULE_TEMP_ADC_IN_CHANNEL          ATOD_CH18
    #define MODULE_TEMP_ADC_RESOLUTION          ATOD_RESOLUTION_12BITS
#endif


#define BASE_IPM_TEMP			200

/* Referenced values:
 * Input : AD counts
 * Output: Temperature in Q15, base is 200C
 * Conversion of Q15 to real value is: [Q15_value * 200 / 32768]    */
#define	IPM_TEMPERATURE_TABLE_HAVANA																	  \
{  (signed long)(17),  (signed long)(-6554 ) },    \
{  (signed long)(44),  (signed long)(-3932 ) },    \
{  (signed long)(71),  (signed long)(-2414 ) },    \
{  (signed long)(98),  (signed long)(-1276 ) },    \
{  (signed long)(125), (signed long)(-360  ) },    \
{  (signed long)(152), (signed long)(465   ) },    \
{  (signed long)(179), (signed long)(1171  ) },    \
{  (signed long)(206), (signed long)(1826  ) },    \
{  (signed long)(233), (signed long)(2436  ) },    \
{  (signed long)(260), (signed long)(3011  ) },    \
{  (signed long)(287), (signed long)(3564  ) },    \
{  (signed long)(314), (signed long)(4096  ) },    \
{  (signed long)(341), (signed long)(4625  ) },    \
{  (signed long)(368), (signed long)(5134  ) },    \
{  (signed long)(395), (signed long)(5632  ) },    \
{  (signed long)(422), (signed long)(6135  ) },    \
{  (signed long)(449), (signed long)(6646  ) },    \
{  (signed long)(476), (signed long)(7135  ) },    \
{  (signed long)(503), (signed long)(7660  ) },    \
{  (signed long)(530), (signed long)(8171  ) },    \
{  (signed long)(557), (signed long)(8704  ) },    \
{  (signed long)(584), (signed long)(9257  ) },    \
{  (signed long)(611), (signed long)(9830  ) },    \
{  (signed long)(638), (signed long)(10414 ) },    \
{  (signed long)(665), (signed long)(11048 ) },    \
{  (signed long)(692), (signed long)(11703 ) },    \
{  (signed long)(719), (signed long)(12396 ) },    \
{  (signed long)(746), (signed long)(13172 ) },    \
{  (signed long)(773), (signed long)(13990 ) },    \
{  (signed long)(800), (signed long)(14944 ) },    \
{  (signed long)(827), (signed long)(15974 ) },    \
{  (signed long)(854), (signed long)(17161 ) },    \
{  (signed long)(881), (signed long)(18596 ) },    \
{  (signed long)(908), (signed long)(20380 ) },    \

 /* Referenced values:
 * Input : AD counts
 * Output: Temperature in Q15, base is 200C
 * Conversion of Q15 to real value is: [Q15_value * 200 / 32768]    */
#define	IPM_TEMPERATURE_TABLE_ETNA																	  \
{  (signed long)(147), (signed long)(20477) },    \
{  (signed long)(174), (signed long)(19189) },    \
{  (signed long)(201), (signed long)(18093) },    \
{  (signed long)(228), (signed long)(17134) },    \
{  (signed long)(255), (signed long)(16280) },    \
{  (signed long)(282), (signed long)(15505) },    \
{  (signed long)(309), (signed long)(14792) },    \
{  (signed long)(336), (signed long)(14130) },    \
{  (signed long)(363), (signed long)(13508) },    \
{  (signed long)(390), (signed long)(12919) },    \
{  (signed long)(417), (signed long)(12357) },    \
{  (signed long)(444), (signed long)(11816) },    \
{  (signed long)(471), (signed long)(11294) },    \
{  (signed long)(498), (signed long)(10785) },    \
{  (signed long)(525), (signed long)(10287) },    \
{  (signed long)(552), (signed long)(9796 ) },    \
{  (signed long)(579), (signed long)(9310 ) },    \
{  (signed long)(606), (signed long)(8826 ) },    \
{  (signed long)(633), (signed long)(8340 ) },    \
{  (signed long)(660), (signed long)(7851 ) },    \
{  (signed long)(687), (signed long)(7355 ) },    \
{  (signed long)(714), (signed long)(6848 ) },    \
{  (signed long)(741), (signed long)(6326 ) },    \
{  (signed long)(768), (signed long)(5783 ) },    \
{  (signed long)(795), (signed long)(5213 ) },    \
{  (signed long)(822), (signed long)(4608 ) },    \
{  (signed long)(849), (signed long)(3955 ) },    \
{  (signed long)(876), (signed long)(3236 ) },    \
{  (signed long)(903), (signed long)(2426 ) },    \
{  (signed long)(930), (signed long)(1475 ) },    \
{  (signed long)(957), (signed long)(294  ) },    \
{  (signed long)(984), (signed long)(-1349) },    \
{  (signed long)(1011),(signed long)(-4459) },    \

#define IPM_TABLE_SELECTED IPM_TEMPERATURE_TABLE_ETNA

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#endif // MODULETEMPERATURE_PRM_H_


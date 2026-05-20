/**
 *  @defgroup   CLASS_B
 *  @file       SRMCAtod_macros.h
 *  @brief      Motor control conversion macros
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMCATOD_MACROS_H_
#define SRMCATOD_MACROS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * There are definitions for data registers referred to according analog port
 */

#define SRMCATOD__ADC0_RA()							(signed short int)(ADC0.ADC_R[0])
#define SRMCATOD__ADC0_RB()     					(signed short int)(ADC0.ADC_R[1])
#define SRMCATOD__ADC1_RA()     					(signed short int)(ADC1.ADC_R[0])
#define SRMCATOD__ADC1_RB()                     	(signed short int)(ADC1.ADC_R[1])

#define SRMCATOD__SET_DELAY_1ST_CONV_CH0(value)     {PDB0.CH0DLY0 = value; PDB0.SC |= PDB_SC_LDOK_MASK;}
#define SRMCATOD__SET_DELAY_2ND_CONV_CH0(value)     {PDB0.CH0DLY1 = value; PDB0.SC |= PDB_SC_LDOK_MASK;}
#define SRMCATOD__SET_DELAY_1ST_CONV_CH1(value)     {PDB0.CH1DLY0 = value; PDB0.SC |= PDB_SC_LDOK_MASK;}
#define SRMCATOD__SET_DELAY_2ND_CONV_CH1(value)     {PDB0.CH1DLY1 = value; PDB0.SC |= PDB_SC_LDOK_MASK;}

#define SR_MCATOD_MINIMUM_12BITS_AD_VALUE      		0x0000
#define SR_MCATOD_MAXIMUM_12BITS_AD_VALUE      		0x0FFF
#define SR_MCATOD_HALF_RANGE_12BITS_AD_VALUE        0x051E          // 1.05V / 3.3V * 4096 (12bits resolution)

//------------------------------ Additional MACROS for changing sampled channel on fly  ----
#define SRMCATOD__SET_ADC0_SAMPLE_CHN(sample,channel)   ADC0.ADC_SC1[(sample)] = (ADC0.ADC_SC1[(sample)] & 0xFFE0 ) | ADC_SC1_ADCH((channel))
#define SRMCATOD__SET_ADC1_SAMPLE_CHN(sample,channel)   ADC1.ADC_SC1[(sample)] = (ADC1.ADC_SC1[(sample)] & 0xFFE0 ) | ADC_SC1_ADCH((channel))


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // MCATOD_MACROS_H_

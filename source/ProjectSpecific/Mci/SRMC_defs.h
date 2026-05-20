/**
 *  @file       SRMC_defs.h
 *  @defgroup   CLASS_B
 *  @brief      Class B relevant defines for SR Motor Control
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2019.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

#ifndef SRMC_DEFS_H_
#define SRMC_DEFS_H_

//* PROJECT SPECIFIC SECTION -begin */
//! Platform selection:
#define NUCLEUS_BASED                   0
#define PLATFORM_2_5_BASED              1

#define PLATFORM_USED                  NUCLEUS_BASED
//#define PLATFORM_USED                  PLATFORM_2_5_BASED


//! Board selection: 1 - Selection / 0 - Not selected
#define WINDY_STRIP_BOARD               1
#define WINDY_INTERNATION_BOARD         0
//* PROJECT SPECIFIC SECTION -end */


//* MANDATORY SECTION -begin *//

#if(WINDY_STRIP_BOARD == 1)
    #define BASE_VOLTAGE_BUS                    446.8f              //!< [V] Base DC Bus Voltage
    #define BASE_CURRENT                        8.212f              //!< [A] Base current
#elif(WINDY_INTERNATION_BOARD == 1)
    #define BASE_VOLTAGE_BUS                    299.414f*1.7320508075688772935274463415059f             //!< [V] Base DC Bus Voltage
    #define BASE_CURRENT                        10.6256f            //!< [A] Base current

#else
    #error "Need to define a board"
#endif


// duplicated class A macros to manipulate class B stuff
#define SRMC__REAL2IQ(value, base, format)    (((value) * (1 << (format)))/(base))
#define SRMC__IQ2REALF(value, base, format)   (((value) * (float)(base)) / (float)(1<<(format)))
#define SRMC__FILTER_CALC(new_input, sum, shift) (sum += (-sum>>shift) + new_input)
#define SRMC__FILTER_GET_FILTERED_VAR(sum, shift) (sum>>shift)


//* MANDATORY SECTION -end *//

#endif /* SRMC_DEFS_H_ */

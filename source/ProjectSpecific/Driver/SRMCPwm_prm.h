/**
 *  @file       SRMCPwm_prm.h
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMCPWM_PRM_H_
    #define SRMCPWM_PRM_H_

#include "SRMC_defs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
// This value is used to fine tune conversion trigger event in order to avoid ringing cross talk
    #define SR_MCPWM_ADC_START_DELAY			0x06// This constant ADC start delay 3PCLK + 3ADCLK (in peripheral cycles)


    #define SR_PWM_ENABLE_IRQ                  0    //!< 1 - Enable IRQ from the peripheral; 0 - disable peripheral IRQ

    #if(SR_PWM_ENABLE_IRQ == 1)
        #define SR_PWM_DELAY_ENABLE            1    //!< 1 - Application of duties is delayed of 1 Pwm period
    #endif



    #define SR_PWM_HIGH_CHANNELS_ACTIVE_LEVEL  1    //!< 1 = active level high; 0 = active level low
    #define SR_PWM_LOW_CHANNELS_ACTIVE_LEVEL   1    //!< 1 = active level high; 0 = active level low

    #define SR_PWM_LOWERS_ON_DUTY_CYCLE        SRMCPWM__GET_LOWERS_ON_PWM()
    #define SR_PWM_UPPERS_ON_DUTY_CYCLE        SRMCPWM__GETMAXPWM()

    #define SR_PWM_FREQUENCY_HZ                16000//!< [Hz] Switching frequency
    #define SR_PWM_DEADTIME_NS                 1000 //!< [ns] Dead time inserted when changing the states of same inverter leg

	#define SR_PWM_INIT_STATE			       ENABLED // Disabled: pwm is off at startup
													    // Enabled: pwm if on at startup, lower Igbts are forced on

	//FAGNAL -> Not Used
    //#define SR_PWM_SEL_INTRNL_SHUTDOWN         1	//!< 1 - use internal shut down circuit detection; 0 - use external shut down circuit

	#define PIN_OF_HIGHERS_PWM_IDLE_STATE	   0    //!< Upper side state when peripheral is disconnected from the pin
	#define PIN_OF_LOWERS_PWM_IDLE_STATE	   0    //!< Low side state when peripheral is disconnected from the pin
//	#define SD_PIN_TYPE_DEF 				   OUTPUT_PUSHPULL
//	#define SD_PIN_INIT_STATE				   1

	#define SR_PWM__REMAP         			   1

	#define SR_PWM_U_LOW  				       4
	#define SR_PWM_U_HIGH			           5

	#define SR_PWM_V_LOW  				       2
	#define SR_PWM_V_HIGH 			           3

	#define SR_PWM_W_LOW 				       0
	#define SR_PWM_W_HIGH 			           1


	#define PIN								   0
	#define CMP								   1

	#define SR_PWM_FAULT_0					   ENABLED
	#define SR_PWM_FAULT_1					   DISABLED
	#define SR_PWM_FAULT_2					   DISABLED
	#define SR_PWM_FAULT_3					   DISABLED

	#define SR_PWM_FAULT_0_SRC				   CMP
	#define SR_PWM_FAULT_1_SRC				   PIN
	
	#if(WINDY_STRIP_BOARD == 1)
	#define SR_PWM_FAULT_0_CMP_LEV				35 	// (Vin/64) * (SR_PWM_FAULT_0_CMP_LEV + 1)	    1856 mV //!< [mV] Comparator threshold
	#elif(WINDY_INTERNATION_BOARD == 1)
	#define SR_PWM_FAULT_0_CMP_LEV			  	40 	// (SR_PWM_FAULT_0_CMP_LEV*CMP_SCALE_x_100)/100000 - 1	2000 mV//!< [mV] Comparator threshold
	#else
    #error "Need to define ports for beagle"
    #endif

	#define SR_PWM_FAULT_0_CMP_LEV_TEST		  	8 	// (Vin/64) * (SR_PWM_FAULT_0_CMP_LEV_TEST + 1)	464 mV  //!< [mV] Comparator threshold

    #define SR_COMPARATOR_CHANNEL              COMPARATOR_CHANNEL0              // Channel used for OC protection.
    #define SR_COMPARATOR_INVERTED_INPUT       COMPARATOR_INVERTED_INPUT7       // Selected inverted input of comparator
    #define SR_COMPARATOR_NONINVERTED_INPUT    COMPARATOR_NONINVERTED_INPUT0    // Selected non invert input of comparator
    #define SR_COMPARATOR_MODE                 COMPARATOR_MODE_CONTINUOUS       // Operation mode of comparator

#define SR_TRANSMIT_AT_CREST (1)                    // 1 for transmit at crest
													// 0 for transmit at trough


#define SR_MCPWM_SD_PULSE_WIDTH_BASE250US       1  // At least 3ms at low level

#define SR_MCPWM_SHUTDOWN_ACTIVE_POLARITY       1   // define the level which the shut down circuit is active LEVEL 0

#define SR_MCPWM_DIAG_TOTAL_SAMPLES_NR          25  // this is equivalent to ~1.56ms @62.5us
#define SR_MCPWM_DIAG_SAMPLES_4_DECISION        15  // 60% of the samples in this level defines a decision shift,
                                                    // 966.8us as worst low level case to read the shut down as zero

//#define FORCE_SHUTDOWN_FAILURE_ALREADY_ACTIVE
//#define FORCE_SHUTDOWN_FAILURE_STUCK_AT_1
//#define FORCE_SHUTDOWN_FAILURE_STUCK_AT_0

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRMCPWM_PRM_H_



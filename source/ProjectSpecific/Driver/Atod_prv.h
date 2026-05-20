/**
 *  @file       Atod_prv.h
 *  @brief      Contains Class B related configuration parameters for the SRAtod GDM
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef ATOD_PRV_H_
	#define ATOD_PRV_H_
//#include "SRException.h"
//#include "SRFlow.h"
//
////=====================================================================================================================
////-------------------------------------- CLASS B Configuration Parameter-----------------------------------------------
////=====================================================================================================================
////=====================================================================================================================
///**
// *    @brief  - Defines the reference voltage supplied for the ADC Engine.
// *    @details- Define the exact voltage (in terms of millisecond) provided for the Microcontroller.
// *              The supply voltage for the STM32F10x controller varies from 2.0v to 3.6v.
// *
// *
// *    @param  - 2000L to 3600L
// *
// *    @note -   //If the reference voltage supplied for the ADC Engine is 3.3 then-
// *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS          3300L"
// *
// *              //If the reference voltage supplied for the ADC Engine is 2.0 then-
// *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS          2000L"
// *
// *              //If the reference voltage supplied for the ADC Engine is 3.6 then-
// *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS          3600L"
// */
////=====================================================================================================================
//#define ATOD_ENGINE_REF_IN_MILLIVOLTS                        3300L
//
////=====================================================================================================================
///**
// *    @brief  - Defines the output voltage range of the “Internal Power Block” in terms of millivolt.
// *    @details- SRAtod module reads the “Internal Power Block” outputs periodically to make sure the functionality
// *              of the ADC and ADC multiplexer are working fine.
// *              The Internal power block output is connected to the ATOD_CH17.
// *              Its available only on ADC1 engine.
// *              It uses the 10-bit resolution.
// *
// *<pre>
// *      The output of the Internal power block varies as given below -
// *      ----------------------------------------------------------------------
// *      |  Conditions      |      Min       |     Typ        |      Max       |
// *      |in Degree celsius | in milliVolt   | in milliVolt   | in milliVolt   |
// *      |------------------|----------------|----------------|----------------|
// *      | -40 < T < +105   |    1160        |    1200        |    1260        |
// *      |------------------|----------------|----------------|----------------|
// *      | -40 < T < +85    |    1160        |    1200        |    1240        |
// *       --------------------------------------------------------------------- </pre>
// *
// *    @param  - For ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS - 1060 to 1160
// *              For ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS - 1260 to 1360
// *
// *              The Internal Power block requires constant voltage supply.
// *              If the supply voltage fluctuates then user has to take care of the ranges by adjusting
// *              the above macro reference value. For better understanding please refer the below examples
// *
// *    @note -   For the constant external supply voltage of 3.3v the macro value should be-
// *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS                        3300L"
// *              "#define ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS                  1160"
// *              "#define ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS                  1260"
// *
// *              For the constant external supply voltage of 2.0v the macro value should be-
// *              "#define ATOD_ENGINE_REF_IN_MILLIVOLTS                        2000L"
// *              "#define ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS                  1160"
// *              "#define ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS                  1260"
// */
////=====================================================================================================================
//#define ATOD_MIN_INTERNAL_REF_IN_MILLIVOLTS               1060
//#define ATOD_MAX_INTERNAL_REF_IN_MILLIVOLTS               1360
//
////=====================================================================================================================
///**
// *    @brief  - Maximum de-bounce counter value for invalid reference voltage detection.
// *    @details- When the ADC get stuck then the de-bounce counter starts incrementing and
// *              whenever the counter reaches to the above macro value it throws an SRATOD_STUCK_EXCEPTION.
// *
// *    @param  - 1/2/3/4
// *
// *    @note -   "#define MAX_INTERNAL_REF_DEBOUNCE          5"
// *              "#define MAX_INTERNAL_REF_DEBOUNCE          10"
// */
////=====================================================================================================================
//#define MAX_INTERNAL_REF_DEBOUNCE                       5
//
////=====================================================================================================================
///**
// *    @brief  - Call-back for Flow control when the SRAtod__EngineInitialize() API is called.
// *    @details- The macros should be defined connecting the macro with the SRFlow modules.
// *
// *    @param  - User defined application function prototype to log the Flow control when the
// *              SRAtod__EngineInitialize() API is called.
// *
// *    @note  -  "#define SRATOD_FLOW_INITIALIZE()                       SRFlow__InitLogEvent(SRATOD_INITIALIZE)"
// */
////=====================================================================================================================
//#define SRATOD_FLOW_INITIALIZE()                    //SRFlow__InitLogEvent(SRATOD_INITIALIZE);
//
////=====================================================================================================================
///**
// *    @brief  - Call-back for Flow control when the SRAtod__Handler() API is called.
// *    @details- The macros should be defined connecting the macro with the SRFlow modules.
// *
// *    @param  - User defined application function prototype to log the Flow control when the
// *              SRAtod__ Handler() API is called.
// *
// *    @note  -  "#define SRATOD_FLOW_HANDLER()                          SRFlow__MainLogEvent(SRATOD_HANDLER)"
// */
////=====================================================================================================================
//#define SRATOD_FLOW_HANDLER()                       //SRFlow__MainLogEvent(SRATOD_HANDLER);
//
////=====================================================================================================================
///**
// *    @brief  - Call-back if the ADC value read from the "Internal Power Block" is out of range.
// *              This occurs if the ADC Engine is stuck or the variation in the supply voltage which is not suitable
// *              for performing the analog to digital conversion.
// *    @details- The macros should be defined connecting the exceptions with the error handling module.
// *
// *    @param  - User defined application function prototype for handling the SRAtod stuck exception.
// *
// *    @note  -  "#define SRATOD_STUCK_EXCEPTION()          SRException__Queue(EXCEPTION_ATOD_STUCK, 0,0)"
// */
////=====================================================================================================================
//#define SRATOD_STUCK_EXCEPTION()                    //SRException__Queue(EXCEPTION_ATOD_STUCK, 0,0);
//
////=====================================================================================================================
///**
// *    @brief  - Call-back if the requested Analog channel configuration has got corrupted.
// *    @details- The macros should be defined connecting the exceptions with the error handling module.
// *
// *    @param  - User defined application function prototype for handling the SRAtod configuration exception.
// *
// *    @note  -  "#define SRATOD_CONFIG_EXCEPTION(channel)   SRException__Queue(EXCEPTION_SRATOD_CONFIG, channel,0)"
// */
////=====================================================================================================================
//#define SRATOD_CONFIG_EXCEPTION(channel)            //SRException__Queue(EXCEPTION_SRATOD_CONFIG, channel,0);

#endif // ATOD_PRV_H_

/**
 *  @file       SRMotorSafetyMgr_prv.h
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMOTORSAFETYMGR_PRV_H_
#define SRMOTORSAFETYMGR_PRV_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#include "SRMC_defs.h"
#include "SRException.h"                            // Connect the Class B library from this point - configured by user.
#include "SRFlow.h"
#include "SRIsrMonitor.h"
#include "Mci.h"
//---------------------------------------------------------------------------------------------------------------------------
//	Motor type selection

//#define SR_CIM_MOTOR
#define SR_BPM_MOTOR

#ifndef COMPILE_4_SIMULINK
#if(WINDY_STRIP_BOARD == 1)
// Current reading strategy  -- Third Current reconstruction strategy
#define SR_THIRD_CURRENT_RECONSTRUCTION // comment this line to disable it (standard 3 currents reading)
#elif(WINDY_INTERNATION_BOARD == 1)
#include "SRMCUSpeedMonitor.h"
//#define SR_THIRD_CURRENT_RECONSTRUCTION // comment this line to disable it (standard 3 currents reading)
#define SR_MODULATION_LIMIT         0.92f        // it must match with F_OVERMODULATION_THR value defined in PwmModulation_prm.h
#else
#error "Need to define a board"
#endif
#endif


#ifdef SR_THIRD_CURRENT_RECONSTRUCTION
#define SRMOTOR_RUNNING_PWM()
#define SRMOTOR_RUNNING_2ND_PWM()     UpdateAdcConfig()
#endif


//---------------------------------------------------------------------------------------------------------------------------
#define SR_MCATOD_PLAUSIBILITY_CHECK_THRESHOLD      125     // [AD_Counts] 10% of 1V reference


#define SR_UNLOCKDOORSPEED_MIN_CURR_READ            10000   // [ADCounts^2] this is the min current with noise comming from current sensors
#define SR_UNLOCKDOORSPEED_DEBOUNCING_TIME          1.6     // [Seconds] time to count before recognizing stopped motor
#define SR_UNLOCKDOORSPEED_FAIL_DETECTION_TIME      256.0    // [Seconds] time to count before recognizing there was a failure in the zero speed detection

#define SR_POWERING_UP_CURRENTS_THRESHOLD           10      // [AD Counts] the powering up will wait until the current differences are smaller than this value.
#ifdef SIM_MODE
#define SR_POWER_UP_DELAY_TIME                      0.0     // [seconds] - define the time delay when powering up to start checking Class B
#else
#define SR_POWER_UP_DELAY_TIME                      1.0     // [seconds] - define the time delay when powering up to start checking Class B
#endif
#define SR_CALIBRATION_CNTR_THRESHOLD               5       // [Counts] Number of samples needed to declare that power-up is finished

#define SR_CURRENT_PLAUSIBILITY_HIGH_LIM            4090    //!< Maximum current level allowed [AD Counts]
#define SR_CURRENT_PLAUSIBILITY_LOW_LIM             5       //!< Minimum current level allowed [AD Counts]

#if(WINDY_STRIP_BOARD == 1)
#define CURR_OFFSET_REF                             2048    // [AD Counts] current offset value expected while not driving the motor. 3.3V / 2.
#elif(WINDY_INTERNATION_BOARD == 1)
#define CURR_OFFSET_REF                             2095    // [AD Counts] current offset value expected while not driving the motor. 3.3V / 2.
#endif

#define CURR_OFFSET_THD                             100     // [AD Counts] maximum variation expected for the current offset. 5% of the expected offset value.
#define VBUS_SR_HIGH_LIM                            (4095-50)// [AD Counts] maximum bus voltage level expected
#define VBUS_SR_LOW_LIM                             50      // [AD Counts] minimum bus voltage level expected

#define VBUS_SR_LOW_THR                        		140.0f	//!< [V] Minimal voltage which the motor will run
#define VBUS_SR_HIGH_THR                            430.0f  //!< [V] Maximum allowed bus voltage
#define VBUS_SR_NOMINAL                             310.0f  //!< [V] Nominal voltage when the motor is IDLE

#define VBUS_SR_LOW_THR_AD_COUNTS                   (signed short)SRMC__REAL2IQ(VBUS_SR_LOW_THR, BASE_VOLTAGE_BUS, 12)  	//!< Maximum allowed bus voltage in VOLTS
#define VBUS_SR_HIGH_THR_AD_COUNTS                  (signed short)SRMC__REAL2IQ(VBUS_SR_HIGH_THR, BASE_VOLTAGE_BUS,12)		//!< Minimal voltage in VOLTS which the motor will run
#define VBUS_SR_NOMINAL_AD_COUNTS                   (signed short)SRMC__REAL2IQ(VBUS_SR_NOMINAL, BASE_VOLTAGE_BUS, 12)      //!< Nominal voltage in VOLTS which dc bus is in idle.

#ifdef SIM_MODE
#define AVERAGE_CURR_SENSOR_OFFSET_LPF_FACTOR       32000     //[N/A] Used to configure the low pass cut off frequency of the current sensors offset
#else
#define AVERAGE_CURR_SENSOR_OFFSET_LPF_FACTOR       640     //[N/A] Used to configure the low pass cut off frequency of the current sensors offset
#endif

#define SD_PIN_ZERO_TIME_MS                         2.5f    // [msec] time which will keep the sd signal to zero and then read the feedback
#define SD_PIN_ONE_TIME_MS                          20.0f   // [msec] time whcih will wait until the sd signal comes back to one (normal operation)

#define PHASE_LOST_COUNTER_THLD                     1000	// [count] Threshold to detect the phase lost, every 200count correspond to 1second

#define CURRENT_LOW_THLD_A_FLT                      0.3     // [A]  current level considered as zero current.
#define CURRENT_HIGH_THLD_A_FLT                     1.0     // [A]  current level considered as a value higher than noise.

#define CURRENT_LOW_THLD_A                          (signed long int) ( ( (CURRENT_LOW_THLD_A_FLT  * CURRENT_LOW_THLD_A_FLT)  * 32767) / (BASE_CURRENT*BASE_CURRENT))
#define CURRENT_HIGH_THLD_A                         (signed long int) ( ( (CURRENT_HIGH_THLD_A_FLT * CURRENT_HIGH_THLD_A_FLT) * 32767) / (BASE_CURRENT*BASE_CURRENT))

#define RMS_SAMPLING_TIME_MS						1000	/* [ms] Defines the time window to calculate rms current for the 3 phases */

#define ELECTRONIC_FAIL_TIMEOUT                     120     /* 600ms /5ms = 120 */
#define TACHO_TIMEOUT                   	        120     /* 600ms /5ms = 120 */
#define PHASE_LOST_TIMEOUT                   	    200     /* 1000ms /5ms = 200 */
#define HW_FAULT_POSTRUN_TIMEOUT                    12000   /* 1minute --> 60sec --> 60000ms /5ms = 12000 */
#define ONE_SEC_TICK                                40       // 1 sec counter in 25ms task
#define VOLTAGE_ERROR_TIMEOUT                   	2000    /* 10000ms /5ms = 200 */


#ifdef SR_BPM_MOTOR

    #define CHECK_UNLOCK_DOOR_SPEED_TIMEOUT_TIME        2000.0f //!< [ms] define the timeout to look for minimum current (hw integrity check)
    #define CHECK_UNLOCK_DOOR_SPEED_IDLING_BEFORE_CHECK_TIME  5000.0f //!< [ms] amount of time in idle before checking the hw integrity
    #define CHECK_UNLOCK_DOOR_MAX_VOLTAGE_DUTY          1000    //!< [output Q15 pu] maximum voltage to be increased while checking hw integrity
    #define CHECK_UNLOCK_DOOR_MAX_VOLTAGE_DUTY_STEP     50      //!< [output Q15 pu] delta voltage to increase the output voltage
    #define CHECK_UNLOCK_DOOR_CURRENT_THRESHOLDS        25      //!< [ADC] current thresholds used to detect hw integrity

    //! Define the cut off frequency used in the averaged frequency algorithm
    #define SRMOTOR_SAFETY_AVG_CALC_COFF_FREQ_LOW_SPD         3 //FILTER_SHIFT_FS_4KHZ_FC_87HZ//FILTER_SHIFT_FS_4KHZ_FC_10HZ
	#define SRMOTOR_SAFETY_AVG_CALC_COFF_FREQ_HI_SPD          1 //FILTER_SHIFT_FS_4KHZ_FC_479HZ//FILTER_SHIFT_FS_4KHZ_FC_10HZ
  	#define SRMOTOR_SAFETY_AVG_HIST_LOW_TO_HIGH_THRES		  240

#endif



//=====================================================================================================================

//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#define SRMOTORSAFETYMGR_FLOW_INITIALIZE_BEGIN()      SRFlow__InitLogEvent(SRFLOW_SRMOTORSAFETYMGR_INIT_BEGIN)
#define SRMOTORSAFETYMGR_FLOW_INITIALIZE_END()        SRFlow__InitLogEvent(SRFLOW_SRMOTORSAFETYMGR_INIT_END)
#define SRMOTORSAFETYMGR_5MS_FLOW_HANDLER_BEGIN()     SRFlow__MainLogEvent(SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_BEGIN)
#define SRMOTORSAFETYMGR_5MS_FLOW_HANDLER_END()       SRFlow__MainLogEvent(SRFLOW_SRMOTORSAFETYMGR_5MS_MAIN_END)
#define SRMOTORSAFETYMGR_25MS_FLOW_HANDLER_BEGIN()    SRFlow__MainLogEvent(SRFLOW_SRMOTORSAFETYMGR_25MS_MAIN_BEGIN)
#define SRMOTORSAFETYMGR_25MS_FLOW_HANDLER_END()      SRFlow__MainLogEvent(SRFLOW_SRMOTORSAFETYMGR_25MS_MAIN_END)
#define SRMOTORSAFETYMGR_250US_FLOW_HANDLER_BEGIN()   SRFlow__ISRLogEvent(SRFLOW_SRMOTORSAFETYMGR_250US_ISR_BEGIN)
#define SRMOTORSAFETYMGR_250US_FLOW_HANDLER_END()     SRFlow__ISRLogEvent(SRFLOW_SRMOTORSAFETYMGR_250US_ISR_END)
#define SRMOTORSAFETYMGR_1MS_FLOW_HANDLER_BEGIN()     SRFlow__ISRLogEvent(SRFLOW_SRMOTORSAFETYMGR_1MS_ISR_BEGIN)
#define SRMOTORSAFETYMGR_1MS_FLOW_HANDLER_END()       SRFlow__ISRLogEvent(SRFLOW_SRMOTORSAFETYMGR_1MS_ISR_END)

#define SRMOTORSAFETYMGR_ISRMONITOR_MCPWM_HANDLER_BEGIN() SRIsrMonitor__EnterIsr()
#define SRMOTORSAFETYMGR_ISRMONITOR_MCPWM_HANDLER_END()   SRIsrMonitor__ExitIsr()

#define SR_MOTORSAFETY_MGR_ERROR_HANDLER(address)   // SRException__Queue(EXCEPTION_DATA_BYTE,(((unsigned short)(address))>>16), (unsigned short)(address))
#define INVOKE_CLASS_A_INIT_HANDLER()               Mci__Initialize()
#define INVOKE_CLASS_A_MCPWM_HANDLER(value_ia, value_ib, value_ic, value_dcbus)			Mci__PwmHandler((value_ia), (value_ib), (value_ic), (value_dcbus))

#define SR_MOTORSAFETY_MGR_SERVICE_WATCHDOG_STARTUP()	//SERVICE_WATCHDOG();

/**
 * @brief Callbacks that are used to connect the module with the Exception handler of the system for issues with SR Variables
 * @details If not used should be kept blank.
 * Normally this kind of exception is not recoverable.
 */
//lint -emacro( 923, SRDATA_CORRUPTION_EXCEPTION )
#define SRDATA_CORRUPTION_EXCEPTION(address)      SRException__Queue(SREXCEPTION_SRDATA_NOISE,((unsigned short int)address)/65536   ,(unsigned short int)address)



//! Definition to communicate Class B Motor control to the Speed monitor
#define AVG_THRESHOLD_HT                5.0f        //!< [%] Hysteresis for averaged speed calculation
#define AVG_FREQUENCY_TOTAL_CYCLES      20           //!< Number of cycles used to calculate the averaged speed
#define AVG_FREQUENCY_THRESHOLD_1       12.0f       //!< [Hz] RPM/60*Pole_Pairs, e.g. Janus (12 pole pairs); 60 rpm == 12 Hz

#if(WINDY_STRIP_BOARD == 1)
#define SR_MOTOR_SAFETY_MGR__AVG_FREQUENCY_ABOVE_THREHOLD() //SRMCUSpeedMonitor__High_Speed_Detected(TRUE)
#define SR_MOTOR_SAFETY_MGR__AVG_FREQUENCY_BELOW_THREHOLD() //SRMCUSpeedMonitor__High_Speed_Detected(FALSE)
#elif(WINDY_INTERNATION_BOARD == 1)
#define SR_MOTOR_SAFETY_MGR__AVG_FREQUENCY_ABOVE_THREHOLD() SRMCUSpeedMonitor__High_Speed_Detected(TRUE)
#define SR_MOTOR_SAFETY_MGR__AVG_FREQUENCY_BELOW_THREHOLD() SRMCUSpeedMonitor__High_Speed_Detected(FALSE)
#endif

#endif // SRMOTORSAFETYMGR_PRV_H_

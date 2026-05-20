/**
 *  @file       SRMotorSafetyMgr.h
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMOTORSAFETYMGR_H_
    #define SRMOTORSAFETYMGR_H_

#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


// For all bits below: 1 means failure; 0 no failure found. Check individual meaning for 1 or 0.
typedef enum
{
    SR_FAULT_CURRENT_SENSOR_FAILED = 0,             //  0 - 1: when the current sensor has failed
    SR_FAULT_VOLTAGE_SENSOR_FAILED,                 //  1 - 1: when the voltage sensor has failed
	SR_FAULT_PWM_SHUTDOWN_FAILED,                   //  2 - 1: when the circuitry of the shut down circuitry has failed.
    SR_FAULT_UNLOCK_DOOR_SPEED_FAILED,              //  3 - 1: when the zero speed could not be found due to hardware or other issues.

    SR_FAULT_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED,  //  4 - 1: when the InputCapture plausibility check has failed.
    SR_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED,  //  5 - 1: when the InputCapture plausibility check for maximum speed change failed.
	SR_FAULT_OVER_CURRENT,                          //  6 - 1: hardware has detected an over current; 0: no over current detected
	SR_FAULT_PHASE_LOST,                            //  7 - 1: Reports when the phase lost is detected. 0: no phase lost situation present

	SR_FAULT_OVER_LOAD,                             //  8 - 1: Reports the motor is under risk of over load 0: no over load situation present
    SR_FAULT_DCBUS_UNDER_VOLTAGE,                   //  9 - 1: DC bus link under voltage detected
    SR_FAULT_DCBUS_OVER_VOLTAGE,                    //  10 - 1: DC bus link over voltage detected
} SR_MOTOR_FAULT_LIST_TYPE;


// For the bits below, check individual meaning for 1 or 0.
typedef enum
{
    SR_FLAG_UNLOCK_DOOR_SPEED = 0,                  //  0 - 1: 0: motor moving; 1: Motor stopped/Unlock door speed reached (long time)
    SR_FLAG_CLASS_A_PWM_ACCESS,                     //  1 - 1: Class A software is allowed to write to the Pwm duties. 0: access denied
    SR_FLAG_REQUESTED_TO_START,                  	//  2 - 1: Requests to start 0: no request to start (only to not stop the handlers)
	SR_FLAG_REQUESTED_TO_STOP,                      //  3 - 1: Requests to stop 0: no request to stop (only to not stop the handlers)

    SR_FLAG_PRERUN_TESTS_FINISHED,                  //  4 - 1: The prerun tests (i.e. plausibility check) are already finished
    SR_FLAG_MICRO_POWERING_UP,                      //  5 - 1: the micro is powering up (from reset or power up); 0: Micro is already at steady
 	SR_FLAG_ADC_PLAUSIB_CHECK_OK,                   //  6 - 1: ADC plausibility check performed and ok; 0 - ADC plausibility check not ok
	SR_FLAG_SHUTDOWN_PLAUSIB_CHECK_OK,              //  7 - 1: Shut down pin plausibility check performed and ok; 0 - Shut down check not ok
	
	SR_FLAG_DETECTION_PHASE_LOST,					//  8 - 1: Reports the motor is under risk of phase lost 0: no phase lost situation present
	SR_FLAG_MC_PARAMETERS_LOADED,					//  9 - 1: Motor control safery parameters loaded oo RAM; 0 - Parameters not loaded
	SR_FLAG_UNLOCK_DOOR_SPEED_CHECK_REQUESTED,      // 10 - 1: Unlock door speed after IDLE for some time is requested 0: do nothing
	SR_FLAG_FORCE_TO_STOP                           // 11 - 1: Requested to force to stop the motor
} SR_MOTOR_FLAGS_LIST_TYPE;


typedef enum
{
    OUTPUT_NO_PATTERN_DEFINED = 0,                  //  0 - the driver has no pattern defined in the pwm lines
    OUTPUT_PATTERN_LOWERS_ON,                       //  1 - only the lowers IGBTs are turned on
    OUTPUT_PATTERN_ALL_DEV_OFF                      //  2 - turn off all switching devices
} OUTPUT_PATTERN_TYPE;


// Safety Parameters Structures
typedef struct
{
    sint16 Tamb;
    sint16 SSpeed;

    float32 Cmot_Inv; //inverse of CMOT
    float32 Dt;

    // Steel Losses
    float32 A1;
    float32 A2;

    // Joule losses
    float32 Rwind;
    float32 Alfa_Winding;

    //thermal conductance
    float32 Gtumb;
    float32 Gtumb1;
    float32 Gspin;

} MOTOR_CLASS_B_RC_PARAMETERS;


typedef struct
{
    uint16 SrCrc;

    sint16 OverLoad_Temp_Init;
    sint16 OverLoad_Temp_Low;
    sint16 OverLoad_Temp_High;

    MOTOR_CLASS_B_RC_PARAMETERS Rc_Thermal;

    uint32 Avg_Fund_Period_Threshold_1;

} MOTOR_SAFETY_PARAMS_TYPE;

#define SAFETY_MOTOR_PARAMETERS_NUM sizeof(MOTOR_SAFETY_PARAMS_TYPE)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRMotorSafetyMgr__Initialize(void);
void SRMotorSafetyMgr__MCPwmHandler(void);
void SRMotorSafetyMgr__Handler250us(void);
void SRMotorSafetyMgr__Handler1ms(void);
void SRMotorSafetyMgr__Handler5ms(void);
void SRMotorSafetyMgr__SlowHandler(void);

unsigned char SRMotorSafetyMgr__AnyFaultFound(void);
unsigned char SRMotorSafetyMgr__GetFault(SR_MOTOR_FAULT_LIST_TYPE request_this_fault);
SR_MOTOR_FAULT_LIST_TYPE SRMotorSafetyMgr__GetListedError(void);
unsigned char SRMotorSafetyMgr__GetFaultHolder(SR_MOTOR_FAULT_LIST_TYPE request_this_fault);
unsigned char SRMotorSafetyMgr__GetFlag(SR_MOTOR_FLAGS_LIST_TYPE request_this_flag);
unsigned char SRMotorSafetyMgr__IsInIdle(void);


unsigned char SRMotorSafetyMgr__Prepare2StartMotor(void);
unsigned char SRMotorSafetyMgr__Prepare2StopMotor(void);
unsigned char SRMotorSafetyMgr__ForceStop(void);

void SRMotorSafetyMgr__UpdatePwm(signed long duty_a, signed long duty_b, signed long duty_c);
unsigned char SRMotorSafetyMgr__ConfigPwmPattern(OUTPUT_PATTERN_TYPE pattern);
void SRMotorSafetyMgr__UpdatePwmPeriod(uint32 pwm_period_perc);

float SRMotorSafetyMgr__SR_SquaredCurrentRmsA();
float SRMotorSafetyMgr__SR_SquaredCurrentRmsB();
float SRMotorSafetyMgr__SR_SquaredCurrentRmsC();

BOOL_TYPE SRMotorSafetyMgr__SetParams(void*  params_address);

unsigned char SRMotorSafetyMgr__GetInputCaptureMatchEvent(void);
unsigned int SRMotorSafetyMgr__GetInputCaptureCounterValue(void);

float SRMotorSafetyMgr__GetVoltageFrequency(void);
float SRMotorSafetyMgr__GetSquaredCurrents(void);
float SRMotorSafetyMgr__GetMotorTemperature(void);

#endif // SRMOTORSAFETYMGR_H_



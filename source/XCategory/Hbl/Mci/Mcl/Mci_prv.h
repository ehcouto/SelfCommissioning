/**
 *  @file		Mci_prv.h
 *
 *  @brief      Defintion of types and methods that Mci uses from Mcl
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCI_PRV_H_
	#define MCI_PRV_H_

#include "Mci_prm.h"
#include "C_Extensions.h"
#include "Mcl.h"
#ifdef MCI_MEASURE_INVERTER_TEMP
	#include "InverterTemp.h"
#endif

#ifdef SELF_COMMISSIONING
#include "SelfCommissioning.h"
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define RADS_TO_RPM_65536    (float32) (60.0f / 2.0f / PI *65536.0f)


#define MCI_DEFAULT_ERROR_REACTION()                    MCI_SET_FLOW_CTL(MCI_FORCE_LOWERS_ON)



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// STARTING state definitions

//! Define the starting handler. Its rate depend on definition of MCI_STARTING_2ND_PWM. If defined MCI_STARTING_2ND_PWM it runs at half pwm frequency.
#ifndef SELF_COMMISSIONING
#define MCI_STARTING_PWM()          Mcl__StartingHandler()

//! Define it and MCI_STARTING_PWM is called with half frequency of pwm frequency (16 kHz --> 8 kHz), otherwise MCI_STARTING_PWM is called at pwm frequency
#define MCI_STARTING_2ND_PWM()

//! Define to call entry method when entering "starting state"
#define MCI_STARTING_ENTRY()        Mcl__StartingReset()


//! Define to call exit method when leaving "starting state"
//#define MCI_STARTING_EXIT()

//! Define the guard condition to be met to leave starting state
#define MCI_STARTING_GUARD_EXIT()   Mcl__GetDigital(MCL_READ_STARTING_FINISHED)
#else
#define MCI_STARTING_PWM()
#define MCI_STARTING_GUARD_EXIT()   TRUE
#endif







///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// RUNNING state definitions
//! Define the running handler. Its rate depend on definition of MCI_RUNNING_2ND_PWM. If defined MCI_RUNNING_2ND_PWM it runs at half pwm frequency.
#ifndef SELF_COMMISSIONING
#define MCI_RUNNING_PWM()           Mcl__RunningHandler()


//! Define MCI_RUNNING_2ND_PWM and MCI_RUNNING_PWM is called with half frequency of pwm frequency (16 kHz --> 8 kHz), otherwise MCI_RUNNING_PWM is called at pwm frequency
#if defined(PWM_DOUBLE_SLOT) && defined(MCI_MEASURE_INVERTER_TEMP)
    #define MCI_RUNNING_2ND_PWM()       {Mcl__2ndRunningHandler(); InverterTemp__PwmHandler();}
#elif defined(PWM_DOUBLE_SLOT)
    #define MCI_RUNNING_2ND_PWM()       Mcl__2ndRunningHandler()
#elif defined(MCI_MEASURE_INVERTER_TEMP)
    #define MCI_RUNNING_2ND_PWM()       InverterTemp__PwmHandler()
#else
    #define MCI_RUNNING_2ND_PWM()
#endif

//! Define to call entry method when entering "running state"
#define MCI_RUNNING_ENTRY()         {Mcl__RunningReset();Mcl_Cs.flags.bit.wait_pll_engagement = 1;}

//! Define to call exit method when leaving "running state"
//#define MCI_RUNNING_EXIT()

//! Define the guard condition to be met to leave running state
#define MCI_RUNNING_GUARD_EXIT()    Mcl__GetDigital(MCL_READ_RUNNING_FINISHED)
#else
#define MCI_RUNNING_PWM()           SelfCommissioning__RunningHandler()


//! Define MCI_RUNNING_2ND_PWM and MCI_RUNNING_PWM is called with half frequency of pwm frequency (16 kHz --> 8 kHz), otherwise MCI_RUNNING_PWM is called at pwm frequency
#if defined(PWM_DOUBLE_SLOT) && defined(MCI_MEASURE_INVERTER_TEMP)
    #define MCI_RUNNING_2ND_PWM()       {Mcl__2ndRunningHandler(); InverterTemp__PwmHandler();}
#elif defined(PWM_DOUBLE_SLOT)
    #define MCI_RUNNING_2ND_PWM()       Mcl__2ndRunningHandler()
#elif defined(MCI_MEASURE_INVERTER_TEMP)
    #define MCI_RUNNING_2ND_PWM()       InverterTemp__PwmHandler()
#else
   // #define MCI_RUNNING_2ND_PWM()
#endif

//! Define to call entry method when entering "running state"
//#define MCI_RUNNING_ENTRY()         {Mcl__RunningReset();Mcl_Cs.flags.bit.wait_pll_engagement = 1;}

//! Define to call exit method when leaving "running state"
//#define MCI_RUNNING_EXIT()

//! Define the guard condition to be met to leave running state
#define MCI_RUNNING_GUARD_EXIT()   BIT_TEST(Mci_Flow_Control, MCI_REQUESTED_STOP)
#endif





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// STOPPING state definitions
//! Define the stopping handler. Its rate depend on definition of MCI_STOPPING_2ND_PWM. If defined MCI_STOPPING_2ND_PWM it runs at half pwm frequency.
#ifndef SELF_COMMISSIONING
#define MCI_STOPPING_PWM()          Mcl__StoppingHandler()

//! Define it and MCI_STOPPING_PWM is called with half frequency of pwm frequency (16 kHz --> 8 kHz), otherwise MCI_STOPPING_PWM is called at pwm frequency
#define MCI_STOPPING_2ND_PWM()

//! Define to call exit method when entering "stopping state"
#define MCI_STOPPING_ENTRY()        Mcl__StoppingReset()

//! Define to call exit method when leaving "stopping state"
#define MCI_STOPPING_EXIT()

//! Define the guard condition to be met to leave stopping state
#define MCI_STOPPING_GUARD_EXIT()   Mcl__GetDigital(MCL_READ_STOPPING_FINISHED)
#else
#define MCI_STOPPING_PWM()

//! Define it and MCI_STOPPING_PWM is called with half frequency of pwm frequency (16 kHz --> 8 kHz), otherwise MCI_STOPPING_PWM is called at pwm frequency
#define MCI_STOPPING_2ND_PWM()

//! Define to call exit method when entering "stopping state"
#define MCI_STOPPING_ENTRY()

//! Define to call exit method when leaving "stopping state"
#define MCI_STOPPING_EXIT()

//! Define the guard condition to be met to leave stopping state
#define MCI_STOPPING_GUARD_EXIT()   TRUE
#endif









///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// IDLE state definitions
//! Define the idling handler. Its rate depend on definition of MCI_IDLE_2ND_PWM. If defined MCI_IDLE_2ND_PWM it runs at half pwm frequency.
#if defined MCI_MEASURE_INVERTER_TEMP
    #define MCI_IDLE_PWM()              InverterTemp__PwmHandler()
#else
    //#define MCI_IDLE_PWM()
#endif
//! Define it and MCI_IDLE_PWM is called with half frequency of pwm frequency (16 kHz --> 8 kHz), otherwise MCI_IDLE_PWM is called at pwm frequency
//#define MCI_IDLE_2ND_PWM()

//! Define to call exit method when entering "idling state"
#define MCI_IDLE_ENTRY()            Mcl__ResetAll()

//! Define to call exit method when leaving "idling state"
//#define MCI_IDLE_EXIT()








///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// MANUAL_INJECTION state definitions
#if (MANUAL_INJECTION_FEATURE == ENABLED)
//! Define the manual injection handler. Its rate depend on definition of MCI_MANUAL_INJECTION_2ND_PWM. If defined MCI_MANUAL_INJECTION_2ND_PWM it runs at half pwm frequency.
#define MCI_MANUAL_INJECTION_PWM()              	Mcl__ManualHandler()

//! Define it and MCI_MANUAL_INJECTION_PWM is called with half frequency of pwm frequency (16 kHz --> 8 kHz), otherwise MCI_MANUAL_INJECTION_PWM is called at pwm frequency
#if defined MCI_MEASURE_INVERTER_TEMP
    #define MCI_MANUAL_INJECTION_2ND_PWM()          InverterTemp__PwmHandler()
#else
    #define MCI_MANUAL_INJECTION_2ND_PWM()
#endif

//! Define to call exit method when entering "manual injection state"
#define MCI_MANUAL_INJECTION_ENTRY()           Mcl__ManualReset()

//! Define to call exit method when leaving "manual injection state"
//#define MCI_MANUAL_INJECTION_EXIT()
#else
#define MCI_MANUAL_INJECTION_PWM()             
#define MCI_MANUAL_INJECTION_2ND_PWM()
#define MCI_MANUAL_INJECTION_ENTRY()
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// FAILED state definitions

//! Define to call method while running "failed state"
#define MCI_FAILED_PWM()

#if defined MCI_MEASURE_INVERTER_TEMP
    #define MCI_FAILED_2ND_PWM()          InverterTemp__PwmHandler()
#endif


//! Define to call exit method when leaving "failed state"
#define MCI_FAILED_EXIT()



#define MCI_POWER_UP_ERROR_TO_TEST                                                  \
	                                        (1UL<<MCI_ERROR_DCBUS_OVER_VOLTAGE) +   \
                                            (1UL<<MCI_ERROR_DCBUS_UNDER_VOLTAGE)+   \
                                            (1UL<<MCI_ERROR_OVER_LOAD)  +   \
                                            (1UL<<MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED) +   \
                                            0


#if (MCI_FVT_TESTS == ENABLED)

    #define MCI_FVT_ENTRY()                     MclFvt__Entry()
    #define MCI_FVT_EXIT()                      MclFvt__Exit()
    #define MCI_FVT_PMW()                       MclFvt__PwmHandler()
    #define MCI_FVT_2ND_PWM()
    #define MCI_FVT_GUARD_EXIT()                MclFvt__GuardCondition()

    #define MCI_FVT_GET_STATUS(this_status)     MclFvt__GetStatus(this_status)
    #define MCI_FVT_GET_RESULT(result)          MclFvt__GetResult(result)

#endif








//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the get digital from Mcl to Mci

//#define MCL_GET_OVER_TORQUE()                       0

// Get status if motor is considered as disconnected
#define MCL_GET_MOTOR_DISCONNECTED()                Mcl__GetDigital(MCL_READ_MOTOR_NOT_CONNECTED_ERROR)

// Define the command below to enable the plausiblity check from Class B
//#define MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED()   MotorSafetyMgr__GetFault(SR_FAULT_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED)
//#define MCL_GET_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED()   MotorSafetyMgr__GetFault(SR_FAULT_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED)

// Get status if motor is locked while running
#define MCL_GET_LOCKED_ROTOR_IN_RUNNING()           Mcl__GetDigital(MCL_READ_LOCKED_ROTOR_RUNNING)

// Get status if motor is locked while starting up
#define MCL_GET_LOCKED_ROTOR_AT_STARTUP()           Mcl__GetDigital(MCL_READ_LOCKED_ROTOR_STARTING)

// Get status if observer is lost somehow or *under torque* condition happened
//#define MCL_GET_OBSERVER_FAILURE()      0

// Status if the motor is at over temperature
//#define MCL_CLASS_A_MOTOR_OVER_TEMP()      0

// Status if the interrupt was disabled
//#define MCL_GET_INT_DISABLED()      0

// Get if the Mcl parameters are loaded
#define MCL_GET_PARAMETERS_LOADED()      Mcl_Cs.flags.bit.params_loaded






//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the GET ANALOG from MCL

////<  0 - Target rotor shaft speed in RPM - x65536 - (1<<16)
#define MCL_GET_TARGET_SPEED_RPM_S16()              (sint32)(SpeedRefGen__GetSpeedTarget() * RADS_TO_RPM_65536 )


////!<  1 - Speed reference in RPM - x65536 - (1<<16)
#define MCL_GET_SPEED_REFERENCE_RPM_S16()           (sint32)(Mcl_IO.Speed_Rot_Ref * RADS_TO_RPM_65536)

////!<  2 - Estimated rotor shaft speed in RPM - x65536 - (1<<16)
#define MCL_GET_ACTUAL_SPEED_S16()                  (sint32)(Mcl_Quantities.Speed_Rot_Mech * RADS_TO_RPM_65536)

////!<  3 - Difference between estimated and reference speeds in RPM - x65536 - (1<<16)
//#define MCL_GET_SPEED_ERROR_RPM_S16()          0

////!<  4 - Mean rotor speed calculation - x65536 - (1<<16)
#define MCL_GET_MEAN_SPEED_S16()                    (sint32)(Mcl_Quantities.Mean_Speed * RADS_TO_RPM_65536)

////!<  5 - RMS Motor current - x65536 - (1<<16)
#define MCL_GET_RMS_MOTOR_CURRENT_S16()             (sint32)(65536.0f * Mcl_Quantities.IsRMS)

////!<  6 - Measured active power in Watts - x65536 - (1<<16)
#define MCL_GET_ACTIVE_POWER_S16()                  (sint32)(65536.0f * Mcl_Quantities.Electrical_Power)

////!<  7 - Estimated shaft output power in Watts from estimated torque - x65536 - (1<<16)
#define MCL_GET_SHAFT_POWER_S16()          (sint32)(65536.0f * Mcl_Quantities.Shaft_Power)

////!<  8 - Motor temperature in Celsius - x65536 - (1<<16)
//#define MCL_GET_ROTOR_TEMP_S16()          0

////!<  9 - Read the load torque in Nm - x65536 - (1<<16)
#define MCL_GET_LOAD_TORQUE_S16()                   (sint32)(65536.0f * Mcl_Quantities.Torque_Application)

////!< 10 - Estimated rotor position in deg - x65536 - (1<<16)
//#define MCL_GET_SHAFT_POSITION_DEG_S16()          0

////!< 11 - Estimated Eletromagnetic torque in Nm - x65536 - (1<<16)
//#define MCL_GET_ELETROMAGNETIC_TORQUE_NM_S16()          0

////!< 12 - Bus current in Amps - x65536 - (1<<16)
//#define MCL_GET_BUS_CURRENT_S16()          0

////!< 13 - Bus voltage in Volts - x65536 - (1<<16)
#define MCL_GET_BUS_VOLTAGE_S16()                   (sint32)(65536.0f * Mcl_IO.Vdc)

////!< 14 - Inverter temperature in Celsius - x65536 - (1<<16)
// it is not managed by MCL, but directly by MCI (moved to mci.c)

////!< 15 - Inverter temperature in raw data [ADC counts]
// it is not managed by MCL, but directly by MCI (moved to mci.c)

////!< 16 - Motor phase A voltage in Volts - x65536 - (1<<16)
//#define MCL_GET_VOLTAGE_PHASE_A_S16()          0

////!< 17 - Motor phase B voltage in Volts - x65536 - (1<<16)
//#define MCL_GET_VOLTAGE_PHASE_B_S16()          0

////!< 18 - Motor phase C voltage in Volts - x65536 - (1<<16)
//#define MCL_GET_VOLTAGE_PHASE_C_S16()          0

////!< 19 - Select index of speed loop gain table
//#define MCL_GET_SPEED_LOOP_GAIN_TABLE_INDEX()          0

////!< 16 - Motor phase A voltage in Volts - x65536 - (1<<16)
#define MCL_GET_VOLTAGE_PHASE_A_S16()               (sint32)(65536.0f * Mcl_Quantities.Vabc_Rec.A)

////!< 17 - Motor phase B voltage in Volts - x65536 - (1<<16)
#define MCL_GET_VOLTAGE_PHASE_B_S16()               (sint32)(65536.0f * Mcl_Quantities.Vabc_Rec.B)

////!< 18 - Motor phase C voltage in Volts - x65536 - (1<<16)
#define MCL_GET_VOLTAGE_PHASE_C_S16()              (sint32)(65536.0f * Mcl_Quantities.Vabc_Rec.C)


////!< 20 - Phase current A in A - x65536 - (1<<16)
#define MCL_GET_CURR_PHASE_A_S16()                  (sint32)(65536.0f * Mcl_IO.Is_ABC.A)

////!< 21 - Phase current B in A - x65536 - (1<<16)
#define MCL_GET_CURR_PHASE_B_S16()                  (sint32)(65536.0f * Mcl_IO.Is_ABC.B)

////!< 22 - Phase current C in A - x65536 - (1<<16)
#define MCL_GET_CURR_PHASE_C_S16()                  (sint32)(65536.0f * Mcl_IO.Is_ABC.C)

////!< 23 - Stator resistance in Ohm - x65536 - (1<<16)
#define MCL_GET_STATOR_RESISTANCE_S16()          (sint32)(65536.0f * Mcl_Quantities.Stator_Resistance)

////!< 24 - Stator temperature calculated by stator resistance - x65536 - (1<<16)
#define MCL_GET_STATOR_TEMP_S16()                   (sint32)(65536.0f * Mcl_Quantities.Stator_Temperature)











//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the SET DIGITAL function in MCL


//!< Request the coast down while running motor
//#define MCL_SET_RUNNING_COAST_DOWN(value)           value

//!< Request to stop motor by coast down
#define MCL_SET_STOPPING_COAST_DOWN()               Mcl__SetDigital(MCL_WRITE_STOP_COAST_DOWN_REQ)

//!< Request stop motor by lowers on
#define MCL_SET_STOPPING_LOWERS_ON()                Mcl__SetDigital(MCL_WRITE_STOP_LOWERS_ON_REQ)

//!< Request stop motor by controlling the speed
#define MCL_SET_STOPPING_REQUEST()                  Mcl__SetDigital(MCL_WRITE_STOP_REQ)








//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the SET ANALOG to MCL

//!<  0 - Proportional speed loop gain - x65536 (1<<16)
//#define MCL_SET_SPEED_LOOP_PROP_GAIN_S16(value)       1*(value)

//!<  1 - Integral speed loop gain - x65536 (1<<16)
//#define MCL_SET_SPEED_LOOP_INT_GAIN_S16(value)        1*(value)

//!<  2 - Select index of speed loop gain table
#define MCL_SET_SPEED_LOOP_GAIN_TABLE_INDEX(value)      Mcl__SetAnalog(MCL_AO_SPEED_LOOP_GAIN_TABLE_INDEX, value)

//!<  3 - Select index of speed fir taps
//#define MCL_SET_SPEED_FIR_TAPS_TABLE_INDEX(value)     1*(value)

//!<  4 - Select index of soft start counter
//#define MCL_SET_SOFTSTART_COUNTER_INDEX(value)        1*(value)

//!<  5 - Select index of speed controller limit
#define MCL_SET_SPEED_LIMIT_INDEX(value)                Mcl__SetAnalog(MCL_AO_SPEED_LIMIT_INDEX,value)

//!<  6 - Set the address of Mcl parameters structure
#define MCL_SET_MCL_PARAMETERS_ADDRESS(value)           Mcl__SetParams((void*) value);

//!<  7 - Add (force) a delta speed on the speed reference - x65536 (1<<16)
#define MCL_SET_ADD_DELTA_SPEED_REF_S16(value)          Mcl__SetAnalog(MCL_AO_ADD_DELTA_SPEED_REF_S16, value)





//#define MAX_SPEED_RPM                       1400                //!< [rpm] Define the maximum allowed speed
//#define MIN_SPEED_RPM                       10                  //!< [rpm] Define the minimum allowed speed
//
//#define MAX_RPM_PER_SEC_ACCEL               100
//#define MAX_RPM_PER_SEC_DECEL               MAX_RPM_PER_SEC_ACCEL
//#define MIN_RPM_PER_SEC_ACCEL               1
//#define MIN_RPM_PER_SEC_DECEL               MIN_RPM_PER_SEC_ACCEL


////////////////////////////////////////////////////////////////////////////////////////////////
//                            Floating point \ Fixed Point MCL management                     //
////////////////////////////////////////////////////////////////////////////////////////////////

#define MCL_NOTATION_TYPE       float32
#define MCL_SPEED_REF_GEN()     (SpeedRefGen__Generator()+Mcl_Quantities.Delta_Speed_Rot_Ref)
#define MCL_CURRENT(arg)        (float32) (arg) * BASE_CURRENT / 32768.0f
#define MCL_VOLTAGE(arg)        (float32) (arg) * BASE_VOLTAGE_BUS / 32768.0f
#define MCL_DUTY(arg)           (sint32) ((arg) * 32768.0f)
#define MCL__ABS(value)         MATHCALC__FABS(value)

///////////////////////////////////////////////////////////////////////////////////////////////


//void Mci__Emulate_Error(uint8 emulate_this_error);

#endif // MCI_PRV_H_





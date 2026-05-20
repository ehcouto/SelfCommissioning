/**
 * @file
 * @brief       Private parameters for the API221MotionCtrl module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API221MOTIONCTRL_PRV_H_
#define API221MOTIONCTRL_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#if (API221_SERVER_FEATURE == ENABLED)
    #include "Mci.h"
#endif


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --

#if (API221_CLIENT_FEATURE == ENABLED)
    //! The maximum number of event handlers that can register for API221 events at the same time.
    #define API221_MAX_EVENT_HANDLERS 2

    //! The maximum number of analog input values that can be buffered by the API221_Client.
    //! Set this to zero if there is no intent on reading analog data from any motor.
    #define API221_STORED_ANALOG_INPUTS 10
#endif      // #if (API221_CLIENT_FEATURE == ENABLED)

#if (API221_SERVER_FEATURE == ENABLED)
    //! The default deceleration to use when the motor is stopped due to client inactivity.
    #define API221_DEFAULT_DECELERATION 50

    //! The average number of milliseconds between calls to the API221MotionCtrl__Background() function.
    #define API221_MS_PER_CALL_TO_BACKGROUND 25

    //! The time between a command from the client related to a specific motor and an automatic stop of that same motor.
    //! Set this value to 0 to disable this feature on the server.
    #define API221_MS_TIMEOUT_TO_STOP 0

    //! The number of motors supported by this API221 server implementation.
    #define API221_SERVER_MOTOR_COUNT 1
#endif      // #if (API221_SERVER_FEATURE == ENABLED)


// -- Private Enumerated Constant Declarations --


// -- Private Type Declarations --


// -- Private Flash-Based Constant Definitions --

#if (API221_CLIENT_FEATURE == ENABLED)
    //! An array of motor IDs to use in the API221 communications.
    //! This list has a one-to-one correspondence with the #API221_CLIENT_MOTOR_INDEX_TYPE.
    //! The first parameter of each entry is the communication node for the motor controller.
    //! The second parameter of each entry is the server-side motor index.
    static const API221_MOTOR_ID_TYPE API221_MOTOR_IDS[] =
    {
    //  Server    Server-Side
    //  {Node,    Motor Index   }
        {   0,              0   }
    };
#endif      // #if (API221_CLIENT_FEATURE == ENABLED)


//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Macro Declarations --

#if (API221_SERVER_FEATURE == ENABLED)
    //
    //  NOTE: Some of the following macros are optional. Read the comments below.
    //

    //! A command to the motor or motor-controlled device to perform a platform-specific behavior.
    //! @param motor = The server-based motor index.
    //! @param buffer = Pointer to an array of platform-specific parameters.
    //! @param size = Size of the 'buffer' array.
    //! @return TRUE if the requested motion behavior was accepted. Otherwise, FALSE.
    // Comment out or delete the API221_DEVICE_MOTION() macro if the feature is not supported.
    //#define API221_DEVICE_MOTION(motor, buffer, size)   (MotorActivity__Set(buffer, size, MOTION_REQUESTOR_API221))

    //! A command to run the motor or motor-controlled device.
    //! @param motor = The server-based motor index.
    //! @param velocity = The target velocity for the motor or motor-controlled device.
    //! @param accel = The acceleration rate for the motor or motor-controlled device.
    //! @return TRUE if the run command was accepted. Otherwise, FALSE.
    #define API221_DEVICE_RUN(motor, velocity, accel)   (Mci__Run(motor,  velocity,  accel))

    //! A command to stop the motor or motor-controlled device.
    //! @param motor = The server-based motor index.
    //! @param decel = The deceleration rate for the motor or the motor-controlled device.
    //! @return TRUE if the stop command was accepted. Otherwise, FALSE.
    #define API221_DEVICE_STOP(motor, decel)            (Mci__Stop(motor,decel))

    //! A command to clear any failures associated with the motor or motor-controlled device.
    //! @param motor = The server-based motor index.
    // Comment out or delete the API221_CLEAR_FAILURES() macro if the feature is not supported.
    #define API221_CLEAR_FAILURES(motor)               Mci__ClearError((MOTOR_ENUM_TYPE)motor)

    //! A command to get a specific analog input value.
    //! @param motor = The server-based motor index.
    //! @param chan = The analog input channel whose value is requested.
    //! @return The analog data value as a signed 32-bit value.
    // Comment out or delete the API221_READ_ANALOG_DATA() macro if the feature is not supported.
    #define API221_READ_ANALOG_DATA(motor, chan)        Mci__GetAnalog((MOTOR_ENUM_TYPE)motor, (MCI_AI_TYPE)chan)

    //! A command to get all failures associated with the motor or motor-controlled device.
    //! @param motor = The server-based motor index.
    //! @return The failure flags organized into a single unsigned 32-bit value.
    // Comment out or delete the API221_READ_FAILURE_FLAGS() macro if the feature is not supported.

    //Mci ERRORS not MAPPED into API221 FAILURES:
    //MCI_ERROR_INITIALIZATION_FAILED,                //!<  1 - [T/F] Mci Initialization phase failed
    //MCI_ERROR_INT_DISABLED,                         //!< 16 - [T/F] Interrupt was disabled
    //MCI_ERROR_CLASS_B_FORCE2STOP,                   //!< 18 - [T/F] Class B forced to stop without any Class B call
    //MCI_ERROR_DCBUS_OUT_OF_RANGE,                   //!< 21 - [T/F] From Class B - SR_FAULT_DC_VOLTAGE_OUT_OF_RANGE


    #define API221_READ_FAILURE_FLAGS(motor)  \
        (uint32)                                                                                                        \
        (                                                                                                               \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_ANY_ERROR_FOUND)           << API221_FF_ANY_ERROR_FOUND    ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_DCBUS_OVER_VOLTAGE)        << API221_FF_DCBUS_OVER_VOLTAGE ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_DCBUS_UNDER_VOLTAGE)       << API221_FF_DCBUS_UNDER_VOLTAGE) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_INVERTER_OVER_TEMP)        << API221_FF_INVERTER_OVER_TEMP ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_LOCKED_ROTOR_AT_STARTUP)   << API221_FF_LOCKED_ROTOR       ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_LOCKED_ROTOR_IN_RUNNING)   << API221_FF_LOCKED_ROTOR       ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_OVER_LOAD)                 << API221_FF_MOTOR_OVERLOADED  ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_CURRENT_SENSOR_FAILED)     << API221_FF_ELECTRONIC_FAILED  ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_VOLTAGE_SENSOR_FAILED)     << API221_FF_ELECTRONIC_FAILED  ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_SW_OVER_CURRENT)           << API221_FF_MOTOR_OVER_CURRENT ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_HW_OVER_CURRENT)           << API221_FF_DCBUS_OVER_CURRENT ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_UNLOCK_DOOR_SPEED_FAILED)  << API221_FF_SPEED_CHECK_FAILED ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_PHASE_LOST)                << API221_FF_PHASE_LOST         ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MIN_FAILED)  << API221_FF_ELECTRONIC_FAILED  ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_INPUTCAPTURE_PLAUSIBILITY_MAX_FAILED)  << API221_FF_ELECTRONIC_FAILED  ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_CLASS_A_MOTOR_OVER_TEMP)   << API221_FF_STATOR_OVER_TEMP   ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_PWM_SHUTDOWN_FAILED)       << API221_FF_ELECTRONIC_FAILED  ) | \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_OBSERVER_FAILURE)          << API221_FF_LOCKED_ROTOR       )\
        )

    //! A command to get the flags that indicate the current state of the motor.
    //! @param motor = The server-based motor index.
    //! @return The status flags organized into a single unsigned 8-bit value.
    #define API221_READ_MOTOR_STATUS(motor) \
        (uint8)                                                                                                         \
        (                                                                                                               \
            (Mci__GetError((MOTOR_ENUM_TYPE)motor, MCI_ERROR_ANY_ERROR_FOUND)       << API221_STS_ANY_ERROR_FOUND   ) | \
            (Mci__GetDigital((MOTOR_ENUM_TYPE)motor, MCI_DI_MOTOR_STOP)             << API221_STS_IS_MOTOR_STOPPED  ) | \
            (Mci__GetDigital((MOTOR_ENUM_TYPE)motor, MCI_DI_REACHED_TARGET_SPEED)   << API221_STS_REACHED_TARGET    ) | \
            (Mci__GetDigital((MOTOR_ENUM_TYPE)motor, MCI_DI_IDLE)                   << API221_STS_IS_IDLE           ) | \
            (Mci__GetDigital((MOTOR_ENUM_TYPE)motor, MCI_DI_MOTOR_RUNNING)          << API221_STS_MOTOR_RUNNING     )   \
        )
#endif      // #if (API221_SERVER_FEATURE == ENABLED)


#endif      // API221MOTIONCTRL_PRV_H_


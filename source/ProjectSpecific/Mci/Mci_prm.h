/**
 *  @file       Mci_prm.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2007.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef MCI_PRM_H_
	#define MCI_PRM_H_

#include "C_Extensions.h"
#include "SRMC_defs.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROJECT SPECIFIC: Windy sBPM Nucleus and PLT2.5 support //
/////////////////////////////////////////////////////////////
#define MOTOR_BPM_TYPE


// MOTOR COMMAND MODEs
#define MAIN_uC            0
#define MCI_BD             1


#define COMMAND_MODE                    MCI_BD
#define MCI_INTERNAL_PARAMS             // uncomment it to use internal parameters
//#define OTE_SET_PARAMETERS_INTERNAL     // uncomment it to use internal parameters

#ifdef OTE_SET_PARAMETERS_INTERNAL
    #include "SettingFile.h"

    //Thermal Model displacement definition
    //#define THERMALMODEL_SF_DISPL           SF_DISPL_MOTOR_OTE  //sBPM Motor
    #define THERMALMODEL_SF_DISPL           SF_DISPL_OTE_DD  //DD Motor
#endif
//***************** MCI_BD Mode - required steps ********************//
// in this file
// #define COMMAND_MODE     MCI_BD
// uncomment MCI_INTERNAL_PARAMS
// uncomment OTE_SET_PARAMETERS_INTERNAL
//
// in SurgeRelay_prm.h
// comment the line #define SRAPI20_CHECK()
//
// in SRMCUSpeedMonitor_prv.h
// leave empty the #define SRMCUSPEEDMONITOR__STOP_MOTOR()
//*******************************************************************//




// PROJECT SPECIFIC: Windy sBPM Nucleus and PLT2.5 support  - end //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PROJECT SPECIFIC PARAMETERS - begin /////////////////////////////////
#define MCI_PARAMETERS_OFFSET                   7


////////////////////////////////////////////////////////////////////////////////////////
//                         BOARD DEPENDENT BASE QUANTITIES                            //
////////////////////////////////////////////////////////////////////////////////////////
#if(WINDY_STRIP_BOARD == 1)
#elif(WINDY_INTERNATION_BOARD == 1)
    #define MCI_MEASURE_INVERTER_TEMP
  	#define MODULE_TEMP_TO_STOP     			110                 //!< [degC] maximum inverter temperature to disable motor driving
    #define MODULE_TEMP_TO_START     			88                  //!< [degC] minimum inverter temperature to enable motor driving

#else
    #error "Need to define a board"
#endif


///////////////////////////////////////////////////////////////////////////////////////
//                                  CURREN LIMITS                                     //
////////////////////////////////////////////////////////////////////////////////////////
#define MCI_MAXIMAL_MOTOR_CURRENT_ALLOWED       8.1f            //!< [A] Maximum allowed motor phase current

///////////////////////////// PROJECT SPECIFIC PARAMETERS - end /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////////////
//                         SWITCHING FREQUENCY AND CALL RATE                          //
////////////////////////////////////////////////////////////////////////////////////////
#define FSW                                  16000.0f            //!< [Hz] PWM Frequency and ISR call rate
#define TSW                                  0.000125f           //!< [s]  Control Call Rate Time

////////////////////////////////////////////////////////////////////////////////////////
//                         BOARD DEPENDENT BASES QUANTITIES                           //
////////////////////////////////////////////////////////////////////////////////////////
#define BASE_VOLTAGE                        (BASE_VOLTAGE_BUS/1.7320508075688772935274463415059f)  // < [V-phase] Base Phase Voltage
#define BASE_Z				                (float)(BASE_VOLTAGE/BASE_CURRENT)                     //!< [Ohm]     Base Resistance


////////////////////////////////////////////////////////////////////////////////////////
//                         SPEED REFERENCE PARAMETERS                                 //
////////////////////////////////////////////////////////////////////////////////////////
#define SPEEDREF__TS                        0.000125f     // [s] Speed reference generator call rate
#define SPEEDREF__MAX_RPM_PER_SEC_ACCEL()     (float32) Mci__GetParams(MOTOR0,MCI_PARAMS_MAX_RPM_PER_SEC_ACCEL) * SPEEDREF__TS * 2.0f * PI / 60.0f
#define SPEEDREF__MAX_RPM_PER_SEC_DECEL()     (float32) Mci__GetParams(MOTOR0,MCI_PARAMS_MAX_RPM_PER_SEC_DECEL) * SPEEDREF__TS * 2.0f * PI / 60.0f
#define SPEEDREF__MIN_RPM_PER_SEC_ACCEL()     (float32) Mci__GetParams(MOTOR0,MCI_PARAMS_MIN_RPM_PER_SEC_ACCEL) * SPEEDREF__TS * 2.0f * PI / 60.0f
#define SPEEDREF__MIN_RPM_PER_SEC_DECEL()     (float32) Mci__GetParams(MOTOR0,MCI_PARAMS_MIN_RPM_PER_SEC_DECEL) * SPEEDREF__TS * 2.0f * PI / 60.0f


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//                      ADDITIONAL COMPONENTS AND FEATURES                            //
////////////////////////////////////////////////////////////////////////////////////////
#define SURGE_RELAY_USED                            //!< uncomment To use SURGE RELAY feature
#define MANUAL_INJECTION_FEATURE            DISABLED
#define INVERTER_TEMP_SETTINGFILE_SUPPORT   DISABLED
#define POWERMODULE_MANAGER_USED            DISABLED


///////////// FVT SECTION  -begin //////////////////////////
#define MCI_FVT_TESTS                       ENABLED // DISABLED
// Injection parameters for FVT
#if(WINDY_INTERNATION_BOARD == 1)
#define INJECTION_LEVEL                     0.5f        //! Amps
#else
#define INJECTION_LEVEL                     0.6f        //! Amps
#endif
#define INJECTION_LEVEL_RATE                1.0f        //! Amps per second
#define INJECTION_LEVEL_POSITION            90.0f       //! Electrical Degrees
// Translating into MCLFVT notation
#define MCLFVT_INJECTION_LEVEL              (float32)(INJECTION_LEVEL);
#define MCLFVT_INJECTION_LEVEL_RATE         (float32)(INJECTION_LEVEL_RATE * TSW);
#define MCLFVT_INJECTION_POSITION           (float32)(INJECTION_LEVEL_POSITION /180.0f * PI);
#define MCLFVT_OC_VOLTAGE_LEVEL             -210.0f

#if(WINDY_INTERNATION_BOARD == 1)
#ifdef SURGE_RELAY_USED
#define SURGE_RELAY_FVT       //! uncomment to test the surge relay during the fvt procedure
#endif
//#define OC_FVT                //! uncomment to test the overcurrent complete circuit during the fvt procedure
#endif
///////////////  FVT SECTION  -end //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//                                MISCELLANEOUS DEFINES                               //
////////////////////////////////////////////////////////////////////////////////////////
#ifndef PI
    #define PI 3.1415926535897932384626433832795f
#endif

//! Converts a value into TRUE or FALSE; 0 is FALSE, otherwise is TRUE
#define GET_TRUE_FALSE(value)           (BOOL_TYPE)(((value) == 0) ? (FALSE) : (TRUE))





////////////////////////////////////////////////////////////////////////////////////////
//                                   DEBUG FEATURES                                   //
////////////////////////////////////////////////////////////////////////////////////////
//---------- BOARD DEBUGGING ------------//
#ifndef COMPILE_4_SIMULINK
#define BOARD_DEBUGGING_CALLBACKS   ENABLED
#else
#define BOARD_DEBUGGING_CALLBACKS   DISABLED
#endif

#if (BOARD_DEBUGGING_CALLBACKS == ENABLED)

    #include "BoardDebugging.h"
    #include "SelfCommissioning.h"

    // ---------------  DEBUGGING CALLBACKS ----------------//
    #define MCI_INITIALIZE_CALLBACK()       {BoardDebugging__Initialize();SelfCommissioning__Initialize();}
    #define MCI_END_PWM_CALLBACK()          BoardDebugging__PwmHandler()
    #define MCI_HALLS_CALLBACK()
    #define MCI_250US_CALLBACK()            BoardDebugging__250usHandler()
    #define MCI_1MS_CALLBACK()
    #define MCI_5MS_CALLBACK()
    #define MCI_25MS_CALLBACK()             {BoardDebugging__25msHandler();SelfCommissioning__25msHandler();}


    #define RESET_FROM_TOOLS            ENABLED

    //---------- MASTER COMMMANDER ------------//
    #define DEBUG_MASTERCOMMANDER       ENABLED
    #define SELF_COMMISSIONING

    //---------- SPI and BEAGLE ------------//
    #define FEATURE_MCI_CONTROLS_SPI DISABLED//ENABLED
    #if (FEATURE_MCI_CONTROLS_SPI == ENABLED)
        #define USE_BEAGLE
    #endif

#endif


//--------------------- SIMULINK ------------//
#ifdef COMPILE_4_SIMULINK
    #undef USE_BEAGLE
 //   #define MCI_INTERNAL_PARAMS
    #define BOARD_DEBUGGING_CALLBACKS      DISABLED

    #define SKIP_RESISTANCE_ESTIMATION    // uncomment it skip the first resistance estimation alignment

    #define MCI_INITIALIZE_CALLBACK()
    #define MCI_END_PWM_CALLBACK()
    #define MCI_HALLS_CALLBACK()
    #define MCI_250US_CALLBACK()
    #define MCI_1MS_CALLBACK()
    #define MCI_5MS_CALLBACK()
    #define MCI_25MS_CALLBACK()
#endif



#endif // MCI_PRM_H_




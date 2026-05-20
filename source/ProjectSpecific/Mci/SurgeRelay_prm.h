/**
 *  @file       SurgeRelay_prm.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2009.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SURGERELAY_PRM_H_
	#define SURGERELAY_PRM_H_

#include "Mci.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#if(WINDY_STRIP_BOARD == 1)                       // Windy Strip definition
	#define VBUS_RELAY_OPEN_VOLTAGE     140
	#define VBUS_RELAY_MIN_WAIT_VOLTAGE 200
	#define VBUS_RELAY_CLOSE_VOLTAGE    200
	
	#define VBUS_TOLERANCE_DV_DT        12              // this is dv/dt where dv is in V and dt is in seconds.
	#define VBUS_HANDLER_CALLRATE_MS    200             // call rate of Vbus handler, multiple of 25mS.
    #define SURGE_RELAY_PORT            PORTE
    #define SURGE_RELAY_PIN             24

	//#define SRAPI20_CHECK()				TRUE//(SRAPI020Safety__GetRxValue(API020_RX_SPEED_MONITOR_STATUS, &data_api20_rx))

#elif(WINDY_INTERNATION_BOARD == 1)                        // Windy international definition
    #define VBUS_RELAY_OPEN_VOLTAGE     140
	#define VBUS_RELAY_MIN_WAIT_VOLTAGE 200
	#define VBUS_RELAY_CLOSE_VOLTAGE    200

	#define VBUS_TOLERANCE_DV_DT        12              // this is dv/dt where dv is in V and dt is in seconds.
	#define VBUS_HANDLER_CALLRATE_MS    200             // call rate of Vbus handler, multiple of 25mS.

	#define SURGE_RELAY_PORT            PORTB
    #define SURGE_RELAY_PIN             3

	#include "SRAPI020Safety_prm.h"
	#include "SRAPI020Safety.h"                         //!< Class B API020 (Safety relevant communication)
	#define SRAPI20_CHECK()				(SRAPI020Safety__GetRxValue(API020_RX_SPEED_MONITOR_STATUS, &data_api20_rx))
#else
#error "Need to define a board"
#endif

#define SURGERELAY_READ_DCBUS_VOLTAGE() (Mci__GetAnalog(MOTOR0, MCI_AI_BUS_VOLTAGE_S16)>>16)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SURGERELAY_PRM_H_



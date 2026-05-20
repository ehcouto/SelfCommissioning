/**
 *  @file
 *  @brief      Interface of the public methods of API003Disc
 *
 *  $Header: API003Disc.h 1.2 2015/03/17 18:45:16EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/03/17 18:45:16EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API003DISC_H_
#define API003DISC_H_

#include "SystemConfig.h"

#ifndef API003DISC_FEATURE
    #define API003DISC_FEATURE      ENABLED
#endif

#if (API003DISC_FEATURE == ENABLED)
#include "Reveal.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define API003DISC_NUM           3
#define API003DISC_TYPE          3
#define API003DISC_VERSION       1
#define API003DISC_INSTANCES     1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void API003Disc__Initialize(void);
REVEAL_RECIPE_STATUS_TYPE API003Disc__CommandParser(REVEAL_MSG_TYPE * buffer);
void API003Disc__FeedbackParser(REVEAL_MSG_TYPE * buffer);
unsigned short int API003Disc__GetNodePassword(unsigned char node);

void API003Disc__CheckRevealNodes(unsigned char node);
void API003Disc__GetAPIs(unsigned char node);
void API003Disc__GetAPIInfo(unsigned char node,unsigned char api);
void API003Disc__GetAPIInstances(unsigned char node,unsigned char api);
void API003Disc__PublishAPIInfo(unsigned char index,unsigned char address);

#endif  //API003DISC_FEATURE

#endif // API003DISC_H_



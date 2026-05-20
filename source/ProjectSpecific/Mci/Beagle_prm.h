/**********************************************************************************************************************
 *  @file       Beagle_prm.h
 *  @brief      Beagle module sends high speed data through SPI port with DMA operation.
 *
 *  @details    The protocol does not implement handshake.
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2013.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

#ifndef BEAGLE_PRM_H_
    #define BEAGLE_PRM_H_

#include "Mci_prm.h"

typedef float BEAGLE_DATA_TYPE;                         // it might be "float" or "signed long"

#if(WINDY_STRIP_BOARD == 1)
    #define BEAGLE_PORT             PORTC
    #define BEAGLE_PIN              0
    #define BEAGLE_SPI_PORT         SPI0
    #define BEAGLE_BUFFER_SIZE      12                  // Number of elements sized in 32-bit
    /*---------------------------------------------
     *   BEAGLE_SKIP_CALLS       00    ->  250us
     *   BEAGLE_SKIP_CALLS       28    ->  10ms
     *   BEAGLE_SKIP_CALLS       98    ->  25ms
     --------------------------------------------*/
    #define BEAGLE_SKIP_CALLS       0                   // Number of skipping calls when sending (TX) beagle data

#elif(WINDY_INTERNATION_BOARD == 1)
    #define BEAGLE_PORT             PORTE
    #define BEAGLE_PIN              16
    #define BEAGLE_SPI_PORT         SPI0
    #define BEAGLE_BUFFER_SIZE      12                  // Number of elements sized in 32-bit
    /*---------------------------------------------
     *   BEAGLE_SKIP_CALLS       00    ->  250us
     *   BEAGLE_SKIP_CALLS       28    ->  10ms
     *   BEAGLE_SKIP_CALLS       98    ->  25ms
     --------------------------------------------*/
    #define BEAGLE_SKIP_CALLS       00                  // Number of skipping calls when sending (TX) beagle data

#else
    #error "Need to define ports for beagle"
#endif

#endif // BEAGLE_PRM_H_

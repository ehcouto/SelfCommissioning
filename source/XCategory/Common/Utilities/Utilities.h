/**
 *  @file
 *
 *  @brief      API of the Utilities modules which is a collection of commonly used operations with:\n
 *              Merging Bytes\n
 *              Manipulating Bits in arrays.
 *
 *  @section    Applicable_Documents
 *
 *
 *  $Header: Utilities.h 1.1 2015/03/17 16:33:04EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/03/17 16:33:04EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------



/**
 *  @file       Utilities.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef UTILITIES_H_
#define UTILITIES_H_
#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
unsigned long int Utilities__ConvertArrayTo32bits(unsigned char * array);
unsigned long int Utilities__ConvertArrayTo24bits(unsigned char * array);
unsigned short int Utilities__ConvertArrayTo16bits(unsigned char * array);
unsigned short int Utilities__Merg2BytesTo16bits(unsigned char msb, unsigned char lsb);

void Utilities__SetBitInArray(unsigned char * array, unsigned char bit, unsigned char value);
void Utilities__ToggleBitInArray(unsigned char * array, unsigned char bit);
unsigned char Utilities__GetBitInArray(unsigned char * array, unsigned char bit);
unsigned short int Utilities__CountOneBitsInArray(unsigned char * array, unsigned char size);

#endif // UTILITIES_H_



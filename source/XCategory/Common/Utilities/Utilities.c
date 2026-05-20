/**
 *  @file
 *
 *  @brief      modules which is a collection of commonly used operations with:\n
 *              Merging Bytes\n
 *              Manipulating Bits in arrays.
 *
 *  @details    The function sin this file are independent of microcontroller, endianess
 *
 *
 *  $Header: Utilities.c 1.1 2015/03/17 16:33:04EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/03/17 16:33:04EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Utilities.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

static const unsigned short BYTE_SET_MASK[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
static const unsigned short BYTE_CLR_MASK[] = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method extracts a 32bits unsigned long from an unsigned char array
 * @param array unsigned char * pointer to the source array
 * @return unsigned long resulting of the data extraction.
 */
unsigned long int Utilities__ConvertArrayTo32bits(unsigned char * array)
{
    unsigned long int retval;
    retval = ((unsigned long int)array[0]) << 24;
    retval += ((unsigned long int)array[1]) << 16;
    retval += ((unsigned long int)array[2]) << 8;
    retval += ((unsigned long int)array[3]);
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method extracts a 24bits unsigned long from an unsigned char array
 * @param array unsigned char *pointer to the source array.
 * @return unsigned long resulting of the data extraction.
 */
unsigned long int Utilities__ConvertArrayTo24bits(unsigned char * array)
{
    unsigned long int retval;
    retval = ((unsigned long int)array[0]) << 16;
    retval += ((unsigned long int)array[1]) << 8;
    retval += ((unsigned long int)array[2]);
    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method merges 2 bytes into a unsigned short int 16bits
 * @param msb most significant byte
 * @param lsb least significant byte
 * @return unsigned short with it the result of the merging.
 */
unsigned short int Utilities__Merg2BytesTo16bits(unsigned char msb, unsigned char lsb)
{
    unsigned short int retval;
    retval = ((unsigned short int)msb) << 8;
    retval += ((unsigned short int)lsb);
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method extracts a 16bits unsigned short from an unsigned char array
 * @param array unsigned char *pointer to the source array.
 * @return unsigned short resulting of the data extraction.
 */
unsigned short int Utilities__ConvertArrayTo16bits(unsigned char * array)
{
    return (Utilities__Merg2BytesTo16bits(array[0],array[1]));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to set a bit within an unsigned char array
 * @details the bit would vary from 0 to 255 which means the array can be only 32bytes long
 * @param array pointer to the unsigned char array
 * @param bit bit in the array (0 to 255)
 * @param value value to be set in the bit (0 / 1)
 */
void Utilities__SetBitInArray(unsigned char * array, unsigned char bit, unsigned char value)
{
    unsigned char s_byte;
    unsigned char s_bit;
    s_byte = bit / 8;
    s_bit = bit % 8;

    if (value)
    {
        array[s_byte] |= BYTE_SET_MASK[s_bit];
    }
    else
    {
        array[s_byte] &= BYTE_CLR_MASK[s_bit];
    }

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to set a bit within an unsigned char array
 * @details the bit would vary from 0 to 255 which means the array can be only 32bytes long
 * @param array pointer to the unsigned char array
 * @param bit bit in the array (0 to 255)
 * @return Value of the specific bit
 */
unsigned char Utilities__GetBitInArray(unsigned char * array, unsigned char bit)
{
    if (array[bit / 8] & BYTE_SET_MASK[bit % 8])
    {
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to toggle a bit within an unsigned char array
 * @details the bit would vary from 0 to 255 which means the array can be only 32bytes long
 * @param array pointer to the unsigned char array
 * @param bit bit in the array (0 to 255)
 */
void Utilities__ToggleBitInArray(unsigned char * array, unsigned char bit)
{
    array[bit / 8] ^= BYTE_SET_MASK[bit % 8];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method counts the number of bits with a value of one in an array
 * @param array unsigned char *pointer to the source array.
 * @param size number of bytes in the array.
 * @return The number of bits with a value of one in the array.
 */
unsigned short int Utilities__CountOneBitsInArray(unsigned char * array, unsigned char size)
{
    unsigned short int retval;
    unsigned char byte_index;
    unsigned char byte_value;
    unsigned char bit_mask;

    retval = 0;
    for (byte_index = 0; byte_index < size; byte_index++)
    {
        byte_value = array[byte_index];
        for (bit_mask = 0x80; bit_mask > 0; bit_mask >>= 1)
        {
            if ((byte_value & bit_mask) > 0)
            {
                retval++;
            }
        }
    }
    return retval;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



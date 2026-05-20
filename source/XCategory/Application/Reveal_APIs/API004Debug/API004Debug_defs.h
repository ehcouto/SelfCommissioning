/**
 * @brief       Cross category Class A definitions for the API004Debug module.
 *
 * @details     This file contains macros to create the Signature information easily and the
 *              definition of the "Debug function" data type
 *
 *
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API004DEBUG_DEFS_H_
#define API004DEBUG_DEFS_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --
#define DATATYPE_VOID               DATATYPE_INVALID
#define ELEMENT_BITS                4
#define ELEMENT_MASK                ((1 << ELEMENT_BITS) - 1)
#define PARAM(pos, datatype)        (uint32)(((datatype) & ELEMENT_MASK) << (pos * ELEMENT_BITS))
#define PARAM1(datatype)            PARAM(1, datatype)
#define PARAM2(datatype)            PARAM(2, datatype)
#define PARAM3(datatype)            PARAM(3, datatype)
#define PARAM4(datatype)            PARAM(4, datatype)
#define PARAM5(datatype)            PARAM(5, datatype)
#define PARAM6(datatype)            PARAM(6, datatype)
#define PARAM7(datatype)            PARAM(7, datatype)
#define RETURNS(datatype)           PARAM(0, datatype)

// -- Public Enumerated Constant Declarations --


// -- Public Type Declarations --
typedef struct DEBUG_FUNCTION_STRUCT
{
    char* Name;
    void* Pointer;
    uint32 Signature;
} DEBUG_FUNCTION_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // API004DEBUG_DEFS_H_

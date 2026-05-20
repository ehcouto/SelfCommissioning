/**
 *  @defgroup   CLASS_B
 *  @file       SRException.c
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2007.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SRException.h"
#include "SRMicro.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef struct
{
    EXCEPTION_EVENT_TYPE    event;
    uint16                  data1;
    uint16                  data2;
}EXEPTION_EVENT_TYPE;


#define EXCEPTION_BUFFER_SIZE       10

NO_INIT EXEPTION_EVENT_TYPE Exception_Buffer[EXCEPTION_BUFFER_SIZE];
NO_INIT unsigned char Exception_Write;
NO_INIT unsigned char Exception_Read;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Exception and its variables
 *
 */
void SRException__Initialize(void)
{
    Exception_Read = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Enter brief description of function.
 *
 *  @param
 *
 *  @return
 */
void SRException__Queue(EXCEPTION_EVENT_TYPE event, uint16 data1, uint16 data2)
{
    Exception_Buffer[Exception_Write].event = event;
    Exception_Buffer[Exception_Write].data1 = data1;
    Exception_Buffer[Exception_Write].data2 = data2;

    Exception_Write++;
    if (Exception_Write >= EXCEPTION_BUFFER_SIZE)
    {
        Exception_Write = 0;
    }

    switch (event)
    {
        case EXCEPTION_NONE:
            break;
        case EXCEPTION_MAIN_DATA_BYTE:
            break;
        case EXCEPTION_MAIN_DATA_SHORT:
            break;
        case EXCEPTION_MAIN_DATA_LONG:
            break;
        case EXCEPTION_MAIN_DATA_NOT_ALLOWED:
            break;
        case EXCEPTION_ISR_DATA_BYTE:
            break;
        case EXCEPTION_ISR_DATA_SHORT:
            break;
        case EXCEPTION_ISR_DATA_LONG:
            break;
        case EXCEPTION_ISR_DATA_NOT_ALLOWED:
            break;

        case EXCEPTION_MAIN_FLOW:               // -fallthrough - Lint Command
        case EXCEPTION_ISR_FLOW:                // -fallthrough - Lint Command
        case EXCEPTION_INIT_FLOW:
            SYSTEM_RESET();
            break;

        case EXCEPTION_SRGPIO_CONFIG:           // -fallthrough - Lint Command
        case EXCEPTION_SRGPIO_DATA:             // -fallthrough - Lint Command
        case EXCEPTION_SRGPIO_RANGE:
            SYSTEM_RESET();
            break;

        case SREXCEPTION_SRDATA_NOISE:          // -fallthrough - Lint Command
        case SREXCEPTION_SRDATA_BAD_MEMORY:     // -fallthrough - Lint Command
        case SREXCEPTION_SRDATA_NOT_ALLOWED:    // -fallthrough - Lint Command
        case EXCEPTION_DATA_BYTE:               // -fallthrough - Lint Command
        case EXCEPTION_DATA_SHORT:              // -fallthrough - Lint Command
        case EXCEPTION_DATA_LONG:
            SRMicro__ForceReset(MICRO_RESET_MODE_COLD);
            break;

        case EXCEPTION_MAX_ABSOLUT_CLOCK:       // -fallthrough - Lint Command
        case EXCEPTION_CLOCK_TEST:              // -fallthrough - Lint Command
        case SREXCEPTION_SRCLOCK_OVERRUN:       // -fallthrough - Lint Command
        case SREXCEPTION_SRCLOCK_TEST:
            SYSTEM_RESET();
            break;

        case EXCEPTION_CPU_REGISTER_ERROR:      // -fallthrough - Lint Command
        case EXCEPTION_STACK_OVERFLOW_ERROR:    // -fallthrough - Lint Command
        case EXCEPTION_STACK_UNDERFLOW_ERROR:   // -fallthrough - Lint Command
        case EXCEPTION_FLASH_ERROR:             // -fallthrough - Lint Command
        case EXCEPTION_RAM_RUNTIME_CORRUPTION:
            SRMicro__ForceReset(MICRO_RESET_MODE_COLD);
            break;

        case EXCEPTION_ATOD_STUCK:              // -fallthrough - Lint Command
        case EXCEPTION_SRATOD_CONFIG:
            SYSTEM_RESET();
            break;

        case EXCEPTION_MAIN_SLOT_VIOLATION:     // -fallthrough - Lint Command
        case EXCEPTION_DIVISION_BY_ZERO:
            SYSTEM_RESET();
            break;

        case SREXCEPTION_MICRO_RESET_SIGNATURE_CORUPTION:
            SRMicro__ForceReset(MICRO_RESET_MODE_COLD);
            break;

        default:
            SRMicro__ForceReset(MICRO_RESET_MODE_COLD);
            break;
    }
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



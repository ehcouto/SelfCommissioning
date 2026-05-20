/**
 *  @file       MclInputProc.h
 *
 *  @brief      Motor Control Loop 3-phase motors: Input Processing Macro Block.
 *
 *  @section
 *
 *  $Header:    MclInputProc.h
 *
 *  @copyright  Copyright 2020-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCL_INPUT_PROC_H_
#define MCL_INPUT_PROC_H_


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MclCrossTypes.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef struct
{
    float32 Current_Offset_Comp_Speed_Thr;              //!< [rad/s mech] - Speed threshold to activate the current offset compensation algorithm (0 if it is disabled)
} MCL_INPUT_PROC_PARAMS_TYPE;


typedef struct
{
    float32 Current_Offset_Comp_Speed_Hyst_Thr;         //!< [%] - hysteresis threshold to activate/disable the current offset compensation algorithm
    float32 Current_Offset_Comp_Current_Thr;            //!< [A] - current threshold to activate/disable the current offset compensation algorithm
    float32 Current_Offset_Comp_Max_Current_Comp;       //!< [A] - maximum compensation current applied
    uint32  Current_Offset_Comp_Full_Cycle_Cnt;         //!< [AD] - minimum full cycle count at maximum frequency(speed), Example 8000Hz/240Hz = 33
    float32 Dummy_1;
    float32 Dummy_2;
} MCL_INPUT_PROC_ADDITIONAL_PARAMS_TYPE;


typedef struct
{
    MCL_INPUT_PROC_PARAMS_TYPE                  *InputProcPrm;
    MCL_INPUT_PROC_ADDITIONAL_PARAMS_TYPE       *InputProcAddPrm;
}MCL_INPUT_PROC_JOINT_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


void MclInputProc__ResetState(void);
void MclInputProc__Initialize(MCL_INPUT_PROC_JOINT_PARAMS_TYPE *params);
void MclInputProc__RunningHandler(MCL_INPUT_PROC_IO_F_TYPE *io,MCL_INPUT_PROC_JOINT_PARAMS_TYPE *params);
void MclInputProc__1msHandler(MCL_INPUT_PROC_IO_F_TYPE *io, MCL_INPUT_PROC_JOINT_PARAMS_TYPE *params);


#endif // MCL_INPUT_PROC_H_

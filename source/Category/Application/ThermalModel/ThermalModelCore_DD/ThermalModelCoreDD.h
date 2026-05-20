/*
 * File: ThermalModelCore.h
 *
 * Code generated for Simulink model 'ThermalModelCore'.
 *
 * Model version                  : 1.6
 * Simulink Coder version         : 8.11 (R2016b) 25-Aug-2016
 * C/C++ source code generated on : Thu Nov 02 16:15:00 2017
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_ThermalModelCoreDD_h_
#define RTW_HEADER_ThermalModelCoreDD_h_
#include <string.h>
#ifndef ThermalModelCoreDD_COMMON_INCLUDES_
# define ThermalModelCoreDD_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* ThermalModelCoreDD_COMMON_INCLUDES_ */

#include "ThermalModelCore_types.h"
#include "Mci.h"
/* Macros for accessing real-time model data structure */

/* Block signals (auto storage) */
typedef struct {
  real32_T sfn[5];                     /* '<S1>/sfn' */
} B_ThermalModelCoreDD_T;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T sfn_DSTATE[6];                /* '<S1>/sfn' */
  real_T sfn_RWORK[71];                /* '<S1>/sfn' */
  int_T sfn_IWORK[13];                 /* '<S1>/sfn' */
} DW_ThermalModelCoreDD_T;

/* Constant parameters (auto storage) */
typedef struct {
  /* Computed Parameter: MapleSimICs_Value
   * Referenced by: '<S1>/MapleSimICs'
   */
  real32_T MapleSimICs_Value[6];

#ifdef OTE_SET_PARAMETERS_INTERNAL
  real32_T MapleSimParameters_Value[48];
#endif

} ConstP_ThermalModelCoreDD_T;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real32_T Temperature;                /* '<Root>/Temperature' */
  real32_T DriftCorrection;            /* '<Root>/DriftCorrection' */
  real32_T Reset;                      /* '<Root>/Reset' */
  real32_T Current;                    /* '<Root>/Current' */
  real32_T Speed;                      /* '<Root>/Speed' */
} ExtU_ThermalModelCoreDD_T;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real32_T MagnetTemperature;          /* '<Root>/MagnetTemperature' */
  real32_T StatorTempareture;          /* '<Root>/StatorTempareture' */
  real32_T WindingTemperature;         /* '<Root>/WindingTemperature' */
  real32_T AmbientTemperature;         /* '<Root>/AmbientTemperature' */
  real32_T error;                      /* '<Root>/error' */
} ExtY_ThermalModelCoreDD_T;

typedef real32_T OTE2_PARAMS_DD_TYPE[48];

extern OTE2_PARAMS_DD_TYPE* ThermalModelCoreDD_Params;

/* Block signals (auto storage) */
extern B_ThermalModelCoreDD_T ThermalModelCoreDD_B;

/* Block states (auto storage) */
extern DW_ThermalModelCoreDD_T ThermalModelCoreDD_DW;

/* External inputs (root inport signals with auto storage) */
extern ExtU_ThermalModelCoreDD_T ThermalModelCoreDD_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY_ThermalModelCoreDD_T ThermalModelCoreDD_Y;

/* Constant parameters (auto storage) */
extern const ConstP_ThermalModelCoreDD_T ThermalModelCoreDD_ConstP;

/* Model entry point functions */
extern void ThermalModelCoreDD_initialize(void);
extern void ThermalModelCoreDD_step(void);
extern void ThermalModelCoreDD_terminate(void);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'ThermalModelCore'
 * '<S1>'   : 'ThermalModelCore/MapleSim_OTE2_DDBPM_Single'
 */
#endif                                 /* RTW_HEADER_ThermalModelCoreDD_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

/*
 * File: ThermalModelCore_data.c
 *
 * Code generated for Simulink model 'ThermalModelCore'.
 *
 * Model version                  : 1.12
 * Simulink Coder version         : 8.11 (R2016b) 25-Aug-2016
 * C/C++ source code generated on : Thu Nov 30 10:43:35 2017
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "ThermalModelCore_private.h"
#include "ThermalModelCoreBPM.h"
#ifdef OTE_SET_PARAMETERS_INTERNAL
    #include "ThermalModelCore_prm.h"
#endif

/* Constant parameters (auto storage) */
const ConstP_ThermalModelCoreBPM_T ThermalModelCoreBPM_ConstP = {
  /* Computed Parameter: MapleSimICs_Value
   * Referenced by: '<S1>/MapleSimICs'
   */
  { 298.15F, 298.15F, 298.15F, 298.15F, 298.15F, 298.15F, 298.15F },

  /* Computed Parameter: MapleSimParameters_Value
   * Referenced by: '<S1>/MapleSimParameters'
   */
#ifdef OTE_SET_PARAMETERS_INTERNAL
  OTE_Params_BPM_SF,
#endif

};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

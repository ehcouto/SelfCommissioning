/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _POWERMODULEMANAGER_PRV_H_
#define _POWERMODULEMANAGER_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#define POWERMODULEMGR_INV_TEMP_THR_LOW          PowerModuleMgr_Params_Displ->Inv_Temp_Thr_Low
#define POWERMODULEMGR_INV_TEMP_THR_HIGH         PowerModuleMgr_Params_Displ->Inv_Temp_Thr_High

#define POWERMODULEMGR_MODULATION_WINDOW         PowerModuleMgr_Params_Displ->Modulation_Window

#define POWERMODULEMGR_I_RMS_THR_LOW_S16         PowerModuleMgr_Params_Displ->I_Rms_Thr_Low
#define POWERMODULEMGR_I_RMS_THR_HIGH_S16        PowerModuleMgr_Params_Displ->I_Rms_Thr_High

#define POWERMODULEMGR__GET_INVERTER_TEMP()      0      //(Mci__GetAnalog(MOTOR0, MCI_AI_INVERTER_TEMP_S16)>>16)
#define POWERMODULEMGR__GET_I_RMS_S16()          0      //Mci__GetAnalog(MOTOR0, MCI_AI_RMS_MOTOR_CURRENT_S16)

#define POWERMODULEMGR__IS_STABLE()              FALSE  //Mci__GetDigital(MOTOR0, MCI_DI_REACHED_TARGET_SPEED)
#define POWERMODULEMGR__IS_MOTOR_RUNNING()       FALSE  //Mci__GetDigital(MOTOR0, MCI_DI_MOTOR_RUNNING)
#define POWERMODULEMGR__IS_LOW_POWER()           FALSE  //Mcl__GetDigital(MCL_READ_LOW_POWER_MOD_ENABLED)
#define POWERMODULEMGR__ENTER_LOW_POWER()               //Mcl__SetDigital(MCL_WRITE_SET_LOW_POWER_MOD)
#define POWERMODULEMGR__EXIT_LOW_POWER()                //Mcl__ResetDigital(MCL_WRITE_SET_LOW_POWER_MOD)

#endif // _POWERMODULEMANAGER_PRV_H_

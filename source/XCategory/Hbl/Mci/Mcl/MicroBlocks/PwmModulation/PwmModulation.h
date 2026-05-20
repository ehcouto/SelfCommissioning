/**
 *  @file
 *  @brief       Space Vector Modulation Routines
 *  @details     This module calculates PWM duties to drive the inverter according
 *               to the voltage desired
 *  @author      alessio.beato/luigi.fagnano  (only temporary, since it is not integrated in MKS)
 *  $Header: FOC/PwmModulation.h 1.4 2016/02/02 17:26:40CET Luigi Fagnano (FAGNAL) Exp  $
 * @copyright Copyright 2012 - $Date: 2016/02/02 17:26:40CET $. Whirlpool Corporation. All rights reserved � CONFIDENTIAL
*/
/*
 *
 *---------------------------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef PWMMODULATION_H_
    #define PWMMODULATION_H_

#include "MclBasicTypes.h"
#include "McMathCalc_macros.h"
#include "PwmModulation_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
uint8 PwmModulation__SpaceVectorModulation_v2(mc_sint32 v_dc, ALPHA_BETA_COOR_SYST_TYPE* v_alphabeta_in, ABC_COOR_SYST_TYPE* duty_out);
uint8 PwmModulation__SpaceVectorModulationRectangular(mc_sint32 v_dc, ALPHA_BETA_COOR_SYST_TYPE* v_alphabeta_in, ABC_COOR_SYST_TYPE* duty_out);
void PwmModulation__DeadtimeCompensationGradient_v2(ABC_COOR_SYST_TYPE* i_abc_in, ABC_COOR_SYST_TYPE* duty);
void PwmModulation__InverterLossComp_v2(ABC_COOR_SYST_TYPE *i_abc_in, ABC_COOR_SYST_TYPE *duty, MATHCALC_LUT_EXT_TYPE *inverter_loss_params);

uint8 PwmModulation__SpaceVectorModulationF(float32 v_dc, ALPHA_BETA_COOR_SYST_F_TYPE* v_alphabeta_in, ABC_COOR_SYST_F_TYPE* duty_out, BOOL_TYPE enable_flat_bottom);
uint8 PwmModulation__SpaceVectorModulationRectangularF(float32 v_dc, ALPHA_BETA_COOR_SYST_F_TYPE* v_alphabeta_in, ABC_COOR_SYST_F_TYPE* duty_out);
void PwmModulation__InverterLossCompF(ABC_COOR_SYST_F_TYPE *i_abc_in, ABC_COOR_SYST_F_TYPE *duty, MATHCALC_LUT_EXT_F_TYPE *inverter_loss_params);
void PwmModulation__InverterLossOnStateCompF(ABC_COOR_SYST_F_TYPE *i_abc_in, ABC_COOR_SYST_F_TYPE *duty, MATHCALC_LUT_EXT_F_TYPE *inverter_loss_params, float32 v_dc);
void PwmModulation__DeadtimeCompensationGradientF(ABC_COOR_SYST_F_TYPE* i_abc_in, ABC_COOR_SYST_F_TYPE* duty);

uint8 PwmModulation__SpaceVectorModulationSixSteps(float32 v_dc, ALPHA_BETA_COOR_SYST_F_TYPE* v_alphabeta_in, ABC_COOR_SYST_F_TYPE* duty_out);
#endif // PWMMODULATION_H_

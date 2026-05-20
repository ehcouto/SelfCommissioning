/**
 *  @file       
 *
 *  @brief      Clarke/Park Transformation
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef CLARKEPARKTRANSFORM_H_
#define CLARKEPARKTRANSFORM_H_

#include "MclBasicTypes.h"
#include "McMathCalc_macros.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ClrkPark__DirectClarke(ABC_COOR_SYST_TYPE *input, ALPHA_BETA_COOR_SYST_TYPE *output);
void ClrkPark__InverseClarke(ALPHA_BETA_COOR_SYST_TYPE *input, ABC_COOR_SYST_TYPE *output);
void ClrkPark__DirectPark(ALPHA_BETA_COOR_SYST_TYPE *input, DQ_COOR_SYST_TYPE *output, SIN_COS_TYPE *sincos);
void ClrkPark__InversePark(DQ_COOR_SYST_TYPE *input, ALPHA_BETA_COOR_SYST_TYPE *output, SIN_COS_TYPE *sincos);

void ClrkPark__DirectClarkeF(ABC_COOR_SYST_F_TYPE *input, ALPHA_BETA_COOR_SYST_F_TYPE *output);
void ClrkPark__InverseClarkeF(ALPHA_BETA_COOR_SYST_F_TYPE *input, ABC_COOR_SYST_F_TYPE *output);
void ClrkPark__DirectParkF(ALPHA_BETA_COOR_SYST_F_TYPE *input, DQ_COOR_SYST_F_TYPE *output, SIN_COS_F_TYPE *sincos);
void ClrkPark__InverseParkF(DQ_COOR_SYST_F_TYPE *input, ALPHA_BETA_COOR_SYST_F_TYPE *output, SIN_COS_F_TYPE *sincos);

#endif // CLARKEPARKTRANSFORM_H_


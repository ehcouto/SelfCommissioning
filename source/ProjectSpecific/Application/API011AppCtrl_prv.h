/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: API011AppCtrl_prv.h 1.3 2015/07/17 13:44:31EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/17 13:44:31EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API011APPCTRL_PRV_H_
#define API011APPCTRL_PRV_H_
#include "Reveal.h"
#include "Mode.h"


//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#define API011APPCTRL__GET_PRODUCT_TYPE()  0

#define API011APPCTRL__RECEIVED_ERROR(node,error)

//! Macro called when the feedback message with the accepted Regulations is received from the ACU
#define API011APPCTRL_REGULATION_RX_NOTIFICATION()  RegulationFilter__RxRegulations()

#endif // API011APPCTRL_PRV_H_

/**
 *  @file       
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
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
#ifndef SRISRMONITOR_H_
#define SRISRMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRIsrMonitor__Initialize(void);
void SRIsrMonitor__EnterIsr(void);
void SRIsrMonitor__ExitIsr(void);
void SRIsrMonitor__Handler(void);

#endif // SRISRMONITOR_H_

/**
 *  @defgroup CLASS_B
 *  @file       
 *
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
#ifndef SRISRMONITOR_PRV_H_
#define SRISRMONITOR_PRV_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

// It defines maximum number of interrupts allowed that may occurs between two SRIsrMonitor__Handler() calls
// This value can be even an uint32; for diagnostic needs it can be set to a very high value to prevent exception.
// WARNING: If set to 0 disables SRIsrMonitor module at all - handle with care!
#define SRISRMONITOR_MAX				24 // 16 + 50 %

// It defines minimum number of interrupts that may occurs between two SRIsrMonitor__Handler() calls
// This value must be lower than SRISRMONITOR_MAX (except if module is disabled by setting SRISRMONITOR_MAX is set to 0)
#define SRISRMONITOR_MIN				8 // 16 - 50 %

// It defines maximum number of consecutive interrupts lower than minimum value are allowed before exception;
// 0 means an immediate action at first wrong value.
// Usually it's better to set it to 1 to prevent a false reset at startup - first iteration may not reach
// minimum number of interrupts set by SRISRMONITOR_MIN.
#define SRISRMONITOR_MAX_ATTEMPTS		1

// This option enables some statistic value - maximum and minimum interrupt counted and attempts done.
#define SRISRMONITOR_STATS				ENABLED

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRISRMONITOR_PRV_H_

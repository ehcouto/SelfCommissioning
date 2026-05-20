/**
 *  @file		ModeManagement.c
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "ModeManagement.h"
#include "SRMicro.h"
#include "Mode.h"
#include "SFUpdater.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

void ModeManagement__Handler(void)
{
	#if (SYSTEM_MCU_HANDLING == ENABLED)
	// Managing Mode Machine
	if (SettingFile__IsValid() == FALSE)            			// If Setting file is not consistent
	{
		Mode__SetMode(MODE_SETTINGFILE_NOT_CONSISTENT); 		// force Not consistent state
	}
	else
	{                                               			// If SettingFile is consistent
		if (Mode__GetMode() == MODE_SETTINGFILE_NOT_CONSISTENT) // If the Mode was set to not consistent
		{
			Mode__SetMode(MODE_NORMAL);             			// force Normal mode
		}
	}

	switch (Mode__GetMode())
	{
		case MODE_PROGRAMMING:
		case MODE_SETTINGFILE_NOT_CONSISTENT:
			if (SFUpdater__Handler() == TRUE)
			{
				SRMicro__ForceReset(MICRO_RESET_MODE_COLD);
			}
			break;

		default:
			Mode__SetMode(MODE_NORMAL);             			// force Normal mode
	}
	#endif
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


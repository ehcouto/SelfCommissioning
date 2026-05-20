/**
 * @file
 * @brief       Public parameters to the API013RemoteFunction module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API013REMOTEFUNCTION_PRM_H_
#define API013REMOTEFUNCTION_PRM_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
typedef enum EXTRACTION_STATUS_ERR_ENUM
{
	EXTRACTION_NO_ERR       		= 0,
	EXTRACTION_ERR_COMMAND_PARAM   	= 1,  // Size of command
	EXTRACTION_ERR_STATUS_BACKFUNC  = 2,  // Timeout
	EXTRACTION_STATUS_DISABLE		= 3,  // ACU Send Disable
	EXTRACTION_ERR_MOTION_BUSY		= 4,  // Motion is already in use
} EXTRACTION_STATUS_ERR_TYPE;

// -- Public Enumerated Constant Declarations --
#if (API013_CLIENT_FEATURE == ENABLED)
    //! An enumerated list of all client function names in the appliance system.
    //! This list has a one-to-one correspondence with the API013_CLIENT_FUNCTIONS[] array.
    typedef enum API013_CLIENT_FUNCTION_INDEX_ENUM
    {
        API013_CLIENT_FUNCTION_EXTRACTION = 0,
		API013_CLIENT_FUNCTION_CHILD_SAFETY,
    } API013_CLIENT_FUNCTION_INDEX_TYPE;
#else
    typedef enum API013_CLIENT_FUNCTION_INDEX_ENUM
    {
        API013_CLIENT_FUNCTION_EXTRACTION = 0,
		API013_CLIENT_FUNCTION_CHILD_SAFETY,
    } API013_CLIENT_FUNCTION_INDEX_TYPE;
#endif


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      /* API013REMOTEFUNCTION_PRM_H_ */

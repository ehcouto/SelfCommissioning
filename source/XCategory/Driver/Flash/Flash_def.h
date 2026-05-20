/**
 *  @file       
 *
 *  @brief      Controller specific definition for Flash GDM
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: Flash_def.h 1.1 2015/06/10 14:51:15EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2015/06/10 14:51:15EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FLASH_DEF_H_
#define FLASH_DEF_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * It defines the type of the address variable
 */
typedef uint32 FLASH_ADDRESS_TYPE;

/**
 * It defines the type of the size variable
 */
typedef uint32 FLASH_SIZE_TYPE;


/**
 * Controller supports only 4-byte alligned even addressed write. If the application tries to write on odd address gives error.
 */
#define FLASH_ALIGNMENT     4

#endif // FLASH_DEF_H_



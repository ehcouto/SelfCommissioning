/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: API007Data_prv.h 1.2 2015/06/18 18:19:38EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2015/06/18 18:19:38EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007DATA_PRV_H_
#define API007DATA_PRV_H_

#include "API007App.h"
#include "C_Extensions.h"
#include "SettingFile.h"
#include "Flash.h"
#include "Reveal.h"

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#define API007DATA_USE_PRIORITY_QUEUE                       REVEAL_QUEUE_PRIORITY

#define API007DATA__SF_CSUM()                               SettingFile__GetSettingFileCsum()

#define API007DATA_SF_UI_TABLE_START()                      SettingFile__GetSettingFileUITable()


#define API007DATA__SF_UI_START()                           SettingFile__GetUISettingFileStart()
#define API007DATA__SF_UI_SIZE()                            SettingFile__GetUISettingFileSize()

#define API007DATA__SF_UI_DATA_BY_INDEX(index)              SettingFile__GetUISettingFilePointerByIndex(index)


#define API007DATA__RECEIVED_SF_CSUM(acu_csum,ui_csum)
#define API007DATA__RECEIVED_SF_DATA(size,index,data_ptr)

#define API007DATA__FLASH_HANDLING_ALLOWED()                TRUE
#define API007DATA__ERASE_FLASH_PAGE(address)               (Flash__FormatPage(address) == FLASH_RESPONSE_DONE)
#define API007DATA__WRITE_FLASH(address,ptr_data,size)      (Flash__Write(address,ptr_data,size) == FLASH_RESPONSE_DONE)

#endif // API007DATA_PRV_H_

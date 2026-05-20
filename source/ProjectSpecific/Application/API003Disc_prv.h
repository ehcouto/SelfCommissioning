/**
 *  @file
 *
 *  @brief      Private configuration file for API003
 *
 *
 *  $Header: API003Disc_prv.h 1.2 2015/06/18 16:33:45EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/06/18 16:33:45EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API003DISC_PRV_H_
#define API003DISC_PRV_H_

#include "Reveal_def.h"
#include "API007Data.h"
#include "API009System.h"
#include "API010PollVar.h"
#include "API013RemoteFunction.h"
#include "API019ProductInfo.h"
#include "API221MotionCtrl.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//Test Comment
/**
 * @brief This parameter configures the firewall feature.
 * @note
 *      #define API003_FIREWALL_FEATURE DISABLED    // the firewall feature is disabled \n
 *      #define API003_FIREWALL_FEATURE ENABLED     // the firewall feature is enabled.
 */
#define API003_FIREWALL_FEATURE     DISABLED


/**
 * @brief This parameter defines the password of this node for the network
 * @details This password is a 16bits value from 0x0000 to 0xFFFF.
 * @note
 *      #define API003_FIREWALL_PWR     0xBEEF
 */
#define API003_FIREWALL_PWR         0x1234

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This parameter defines the description of each api in this system
 * @details The structure is defined in Reveal_defs.h is looks like:
 * <pre>
 *  typedef struct
 *  {
 *      unsigned char id;           // API number
 *      unsigned short int type;    // API type
 *      unsigned short int version; // API version
 *      unsigned char instances;    // API number of instances (1 means no other instances, 2 or more means other instances)
 *  } REVEAL_API_INFO_TYPE;
 * </pre>
 * @note
 * Example
 * <pre>
 *
 *      #define API003_API_LIST         {API001Core ,API001Core_Type    ,API001Core_Version     ,API001Core_Instances }, \
 *                                      {API002Daq  ,API002Daq_Type     ,API002Daq_Version      ,API002Daq_Instances }, \
 *                                      {API003Disc ,API003Disc_Type    ,API003Disc_Version     ,API003Disc_Instances }
 * </pre>
 */
                               // API                   ,TYPE                   ,VERSION
#define API003_API_LIST         {API001CORE_NUM         ,API001CORE_TYPE        ,API001CORE_VERSION         ,API001CORE_INSTANCES       }, \
                                {API003DISC_NUM         ,API003DISC_TYPE        ,API003DISC_VERSION         ,API003DISC_INSTANCES       }, \
                                {API007DATA_NUM         ,API007DATA_TYPE        ,API007DATA_VERSION         ,API007DATA_INSTANCES       }, \
                                {API009SYSTEM_NUM       ,API009SYSTEM_TYPE      ,API009SYSTEM_VERSION       ,API009SYSTEM_INSTANCES     }, \
                                {API010POLLVAR_NUM      ,API010POLLVAR_TYPE     ,API010POLLVAR_VERSION      ,API010POLLVAR_INSTANCES    }, \
                                {API013RMTFCN_NUM       ,API013RMTFCN_TYPE      ,API013RMTFCN_VERSION       ,API013RMTFCN_INSTANCES     }, \
                                {API019PRODUCTINFO_NUM  ,API019PRODUCTINFO_TYPE ,API019PRODUCTINFO_VERSION  ,API019PRODUCTINFO_INSTANCES}, \
                                {API221MOTIONCTRL_NUM   ,API221MOTIONCTRL_TYPE  ,API221MOTIONCTRL_VERSION   ,API221MOTIONCTRL_INSTANCES}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This parameter defines the number of instances of APIs in this system
 * @details This definition must match the number or items in the API003_INSTANCE_LIST list
 */
#define API003_NUM_INSTANCES    0


#if (API003_NUM_INSTANCES > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This parameter defines the description of the instances of the APIs in this system
 * @details The structure is defined in Reveal_defs.h is looks like:
 * <pre>
 *
 *  typedef struct
 *  {
 *      unsigned char api_id;
 *      unsigned char instance_id;
 *      unsigned short int type;
 *      unsigned short int version;
 *  } REVEAL_INSTACNE_INFO_TYPE;        // we noticed a misspelling here but this definition comes from Reveal
 *
 * </pre>
 */
#define API003_INSTANCE_LIST    {101          ,240                ,3                  ,4}, \
                                {101          ,241                ,3                  ,4}, \
                                {101          ,242                ,3                  ,4}, \
                                {102          ,243                ,3                  ,5 }

#endif


/**
 * @brief This call back is invoked when a node respond to the Reveal node command.
 * @details The password value is a 16bits data related to the published fire-wall password form the node in the call back.
 */
#define API003DISC__RECEIVED_REVEAL_NODES(node,password)


/**
 * @brief This call back is invoked when a message containing the information about a API is received
 */
#define API003DISC__RECEIVED_API_INFO(node,api,type_16bits,version_16bits,num_instances)


/**
 * @brief This call back is invoked when a message containing the APIs from one node is received.
 */
#define API003DISC__RECEIVED_APIS(node,api_array,api_array_size)

/**
 * @brief This call back is invoked when a message containing the information about instances of a API is received
 */
#define API003DISC__RECEIVED_API_INSTANCE_INFO(node, api, instance, type_16bits,version_16bits)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // API003DISC_PRV_H_

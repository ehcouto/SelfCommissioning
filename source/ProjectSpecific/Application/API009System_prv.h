/**
 *  @file       
 *
 *  @brief      API009System private parameters configuration
 *
 *
 *  $Header: API009System_prv.h 1.1 2015/06/11 16:54:14EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  *****  Copyright 2012.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL  *****
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API009SYSTEM_PRV_H_
#define API009SYSTEM_PRV_H_
#include "SRPowerMgr.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every a node which we sent a message to force a reset respond with ack reason code 1
 */
#define API009SYSTEM__RESET_ACK(node)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every time it receives a reset message for a node in the system
 * @brief
 *
 * @param reason        - This is of MICRO_LAST_RESET_TYPE
 * @param reset_mode    - This is of MICRO_RESET_MODE_TYPE
 */
#define API009SYSTEM__NODE_RESET_MSG(node, reason, reset_mode)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every time it receives a feedback from another node publishing his endianess
 * @details endianess = 1 means BIGENDIAN, 0 means LITTLEENDIAN
 */
#define API009SYSTEM__ENDIANESS_REQUESTG(node,endianess)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This directive defined a delay in 25ms time base to fire the call-back API009SYSTEM__GO_LOW_POWER_RECEIVED()
 * @details values range from 0 to 255 (in a 25ms time base), 0 disables the call-back
 */
#define API009SYSTEM_GO_LOW_POWER_TIMER			80

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called when a message other nodes indicate that they can't engage the low power mode.
 */
#define API009SYSTEM__CANCEL_LOW_POWER()

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called every time an ack is received for a go sleep command which means that the command was sent and received.
 * @details the parameter is the source of the ack message and the reason code.\n
 * reasoncode = NVO_RS_READY (0) means that the command was accepted and the node will go in LowPower. \n
 * reasoncode = NVO_RS_REJECTED(2) means that the node can not go in LowPower. \n
 */
#define API009SYSTEM__GO_LOW_POWER_ACK(address,reasoncode) //AppMgr__LowPowerAckReceived(node)


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This call-back is called when the command to force a watchdog reset is received.
 */
#define API009SYSTEM_WATCHDOG_RESET()       while (1)



#endif // API009SYSTEM_PRV_H_

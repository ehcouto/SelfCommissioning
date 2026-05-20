/**
 *  @file
 *
 *    @brief    The macro value in this file are configurable as per the user requirement.
 *    @details  It contains data that can be modified by the user in order to configure the Uart Tx, Rx buffer size,
 *              Enable/Disable the Uart Com Port.
 * <pre>
 * The UART0,UART1 and UART2 are available in the below port pins in MKV03x Controllers
 * -----------------------------------------------------------------
 * |                        MKV3x_FAMILY                           |
 * |---------------------------------------------------------------|
 * |         Uart COM      | REMAP 0 | REMAP 1 | REMAP 2 | REMAP 3 |
 * |-----------------------|---------|---------|---------|---------|
 * |    COM0     | Tx Pin  | PORTA 2 | PORTA 14| PORTB 1 | PORTB 17|
 * |   (UART0)   |---------|---------|---------|---------|---------|
 * |             | Rx Pin  | PORTA 1 | PORTA 15| PORTB 0 | PORTB 16|
 * |-------------|---------|---------|---------|--------- ---------
 * |    COM1     | Tx Pin  | PORTC 4 | PORTE 0 |
 * |   (UART1)   |---------|---------|---------|
 * |             | Rx pin  | PORTC 3 | PORTE 1 |
 * |-------------|---------|---------|---------|
 * |    COM2     | Tx Pin  | PORTD 3 | PORTE 16|
 * |   (UART2)   |---------|---------|---------|
 * |             | Rx pin  | PORTD 2 | PORTI 17|
 * -------------------------------------------- </pre>
 *
 *
 * @note: UART0 & UART1: Configures on System Clock
 * 				  UART2: Configured on Bus Clock
 *
 *  $Header: Uart_prm.h 1.2 2015/08/18 18:55:45EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/08/18 18:55:45EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef UART_PRM_H_
	#define UART_PRM_H_
#include "C_Extensions.h"
#include "uc.h"
#include "Mci_prm.h"

//=====================================================================================================================
//-------------------------------------- COM(UART) --------------------------------------------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 * @brief  - This macro Enables or Disables the COM1(UART1) and COM2(UART2).
 * @details- The COM port must be Enabled before using in the application.
 *
 * @param  - ENABLED(1) - Which Enables the COM
 *           DISABLED(0) – Which Disables the COM
 *
 * @note   - Following are the Examples\n
 *           "#define UART1_FEATURE       ENABLED"  //This Enables the COM1 Feature\n
 *           "#define UART1_FEATURE       DISABLED" // This Disables the COM1 Feature
 */
//=====================================================================================================================
#define UART0_FEATURE       ENABLED
#if (DEBUG_MASTERCOMMANDER == ENABLED)
    #define UART1_FEATURE       ENABLED
#else
    #define UART1_FEATURE       DISABLED
#endif
#define UART2_FEATURE       DISABLED

//=====================================================================================================================
/**
 * @brief  - This macros re-maps the UART0,UART1 and UART2 pins.
 * @details-
 *
 * <pre>
 * The UART0,UART1 and UART2 are available in the below port pins in MKV03x Controllers
 * -------------------------------------------------------------------
 * |                           MKV3x_FAMILY                          |
 * |-----------------------------------------------------------------|
 * |         Uart COM        |  REMAP 0| REMAP 1 | REMAP 2 | REMAP 3 |
 * |-------------------------|---------|---------|---------|---------|
 * |    COM0     |   Tx Pin  | PORTA 2 | PORTA 14| PORTB 1 | PORTB 17|
 * |   (UART0)   |-----------|---------|---------|---------|---------|
 * |             |   Rx Pin  | PORTA 1 | PORTA 15| PORTB 0 | PORTB 16|
 * |-------------|-----------|---------|---------|--------- ---------
 * |    COM1     |   Tx Pin  | PORTC 4 | PORTE 0 |
 * |   (UART1)   |-----------|---------|---------|
 * |             |   Rx pin  | PORTC 3 | PORTE 1 |
 * |-------------|-----------|---------|---------|
 * |    COM2     |   Tx Pin  | PORTD 3 | PORTE 16|
 * |   (UART2)   |-----------|---------|---------|
 * |             |   Rx pin  | PORTD 2 | PORTI 17|
 * ---------------------------------------------- </pre>
 *
 * @param  - 0/1     for UART0
 *           0/1     for UART1
 *           0/1/2/3 for UART2
 *
 * @note   - Following are the Examples\n
 *           "#define UART0_REMAP                    0"   // Tx = PORTA - Pin 2 ,   Rx = PORTA - Pin 1
 *           "#define UART0_REMAP                    1"   // Tx = PORTA - Pin 14,   Rx = PORTA - Pin 15
 *           "#define UART0_REMAP                    2"   // Tx = PORTB - Pin 1 ,   Rx = PORTB - Pin 0
 */
//=====================================================================================================================
#define UART0_REMAP         4
#define UART1_REMAP         0
#define UART2_REMAP         0

//=====================================================================================================================
/**
 *    @brief  - This macro defines the COM0(UART0),COM1(UART1) and COM2(UART2) Tx FIFO Buffer size.
 *    @details- The Tx FIFO buffer size should be non-zero value.\n
 *              For the Reveal Protocol application it’s recommended that the buffer size should be 22.
 *
 *    @param  - <1\n
 *              <22 for Reveal Application
 *
 *    @note   - Following are the Examples\n
 *              "#define UART2_TX_BUF_SIZE           10" // The Uart2 Tx buffer size is 10\n
 *              "#define UART2_TX_BUF_SIZE           22" // The Uart2 Tx buffer size is 22
 */
//=====================================================================================================================
#define UART0_TX_BUF_SIZE           22
#define UART1_TX_BUF_SIZE           22
#define UART2_TX_BUF_SIZE           22

//=====================================================================================================================
/**
 *    @brief  - This macro defines the COM0(UART0), COM1(UART1) and COM2(UART2)Rx FIFO Buffer size.
 *    @details- The Rx FIFO buffer size should be non-zero value.\n
 *              For the Reveal Protocol application it’s recommended that the buffer size should be 22.
 *
 *    @param  - <1\n
 *              <22 for Reveal Application
 *
 *    @note   - Following are the Examples\n
 *              "#define UART2_RX_BUF_SIZE           10" // The Uart2 Rx buffer size is 10\n
 *              "#define UART2_RX_BUF_SIZE           22" // The Uart2 Rx buffer size is 22
 */
//=====================================================================================================================
#define UART0_RX_BUF_SIZE           22
#define UART1_RX_BUF_SIZE           22
#define UART2_RX_BUF_SIZE           22

//=====================================================================================================================
/**
 * @brief  - This macro defines the Uart interrupt priority.
 * @details- Consists of 4 interrupt groups and each group consists of 4 sub-priorities.
 *           Only the group priority determines preemption of interrupt exceptions.
 *           When the processor is executing an interrupt exception handler, another interrupt with the
 *           same group priority as the interrupt being handled does not preempt the handler.
 *           If multiple pending interrupts have the same group priority, the sub-priority field determines
 *           the order in which they are processed. If multiple pending interrupts have the same group priority
 *           and sub-priority, the interrupt with the lowest IRQ number is processed first.
 *           The lower the Group priority the greater the priority of the corresponding interrupt group.
 *           The lower the sub-priority the greater the priorities of the corresponding interrupt within the group.
 *
 * @param  - 0/1/2/3
 *
 * @note   - Following are the Examples\n
 *           "#define COM1_PREEMPTION_PRIORITY 0"  //The UART1 is in interrupt  0 having highest priority
 *           "#define COM2_PREEMPTION_PRIORITY 1"  //The UART2 is in interrupt 1 having priority less than the UART1
 */
//=====================================================================================================================
#define COM0_PREEMPTION_PRIORITY    3
#define COM1_PREEMPTION_PRIORITY    3
#define COM2_PREEMPTION_PRIORITY    3

//=====================================================================================================================
/**
 * @brief  - This macro defines the UART interrupt sub-priority.
 * @details- Each interrupt group consists of 4 interrupt sub-priorities.
 *           If multiple pending interrupts have the same group priority, the sub-priority field determines the
 *           order in which they are processed. If multiple pending interrupts have the same group priority
 *           and sub-priority, the interrupt with the lowest IRQ number is processed first.
 *           The lower the sub-priority the greater the priorities of the corresponding interrupt within the group.
 *
 * @param  - 0/1/2/3
 *
 * @note   - Following are the Examples\n
 *           "#define COM1_CHANNEL_SUB_PRIORITY 0"  //The UART1 sub-priority is 0 having highest
 *                                                 //priority within the group
 *           "#define COM2_CHANNEL_SUB_PRIORITY 1"   //The UART2 sub-priority is 1
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define COM0_CHANNEL_SUB_PRIORITY   1
#define COM1_CHANNEL_SUB_PRIORITY   1
#define COM2_CHANNEL_SUB_PRIORITY   1
#elif(WINDY_INTERNATION_BOARD == 1)
#define COM0_CHANNEL_SUB_PRIORITY   3
#define COM1_CHANNEL_SUB_PRIORITY   3
#define COM2_CHANNEL_SUB_PRIORITY   3
#else
#error "Need to define ports for beagle"
#endif


//=====================================================================================================================
/**
 *    @brief  - This macro Enables or Disables the UART Polling Mode.
 *    @details- The UART Polling Mode must be Defined before using in the application.
 *    			There are some limitation on polling mode with Wide on long WIDE message packet with data length more than 20 used
 *	 	 	 	using polling - is not sufficient time for long message , so depend upon message length & project application user should select
 *	 	 	 	"#define UART_POLLING_MODE       ENABLED"
 *	 	 	 				or
 *              "#define UART_POLLING_MODE       DISABLED"
 *
 *    			Architecture suggest to use UART in Interrupt mode in application
 *    			Polling mode is recommended for boot loader application only'
 *
 *    @param  - ENABLED(1) - Which Enables the Polling Mode
 *              DISABLED(0) – Which Disables the Polling Mode
 *
 *    @note   - Following are the Examples\n
 *              "#define UART_POLLING_MODE       ENABLED"  // Which Enables the Polling Mode
 *              "#define UART_POLLING_MODE       DISABLED" // Which Disables the Polling Mode
 */
//---------------------------------------------------------------------------------------------------------------------
#define UART_POLLING_MODE           DISABLED

#endif

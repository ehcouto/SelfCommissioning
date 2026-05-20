/**
 *  @file
 *
 *  @brief    It contains data that can be modified by the user in order to adjust the Timer Interrupt period
 *
 *  @details
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: TimerInterrupt_prm.h 1.2 2015/08/11 08:08:34EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2015/08/11 08:08:34EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef TIMERINTERRUPT_PRM_H                        //Ensure this file is not compiled again
    #define TIMERINTERRUPT_PRM_H                    //After we compile it the first time

//=====================================================================================================================
/**
 *    @brief  - Defines the Interrupt Interval in us
 *    @details- Due to the WIDE implementation it is strongly recommended to use the timer interrupt interval of 250us.
 *              The macro value should not be changed
 *
 *    @param  - 250 // Do not change the value
 *
 *    @note   - Following is the Example\n
 *              "#define INTERRUPT_INTERVAL   250"
 */
//=====================================================================================================================
#define INTERRUPT_INTERVAL          250             //Interrupt interval in us


//=====================================================================================================================
/**
 * @brief  - This macro defines the Systik interrupt group priority.
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
 *           Note - Its recommended to configure the systik at highest priority.
 *
 *
 * @param  - 0/1/2/3
 *
 * @note   - Following are the Examples\n
 *           "#define SYSTICK_PREEMPTION_PRIORITY 0"  //The Systik is in interrupt group 0 having highest priority
 *           "#define SYSTICK_PREEMPTION_PRIORITY 1"  //The Systik is in interrupt group 1
 */
//=====================================================================================================================
#define SYSTICK_PREEMPTION_PRIORITY         1

//=====================================================================================================================
/**
 * @brief  - This macro defines the Systik interrupt sub-priority.
 * @details- Each interrupt group consists of 4 interrupt sub-priorities.
 *           If multiple pending interrupts have the same group priority, the sub-priority field determines the
 *           order in which they are processed. If multiple pending interrupts have the same group priority
 *           and sub-priority, the interrupt with the lowest IRQ number is processed first.
 *           The lower the sub-priority the greater the priorities of the corresponding interrupt within the group.
 *
 *           Note - Its recommended to configure the systik at highest priority.
 *
 * @param  - 0/1/2/3
 *
 * @note   - Following are the Examples\n
 *           "#define SYSTICK_SUB_PRIORITY 0"  //The Systik sub-priority is 0 having highest priority within the group
 *           "#define SYSTICK_SUB_PRIORITY 1"   //The Systik sub-priority is 1
 */
//=====================================================================================================================
#define SYSTICK_SUB_PRIORITY                1

//=====================================================================================================================
/**
 * @brief  - This macro defines long duration time interrupt
 * @details- The value should be in milli second.
 *            Long interval is configured only when the TimerInterrupt__SetLongInterval() is called during low power routine,
 *
 *           Note - User should take care of the long interval time and the watchdog time
 *
 * @param  - 1 to 1000
 *
 * @note   - Following are the Examples\n
 *           "#define TIMERITERUPT_LONG_INTERVAL_IN_MS 10000"      //systik interval tick sets for every 10ms
 *           "#define TIMERITERUPT_LONG_INTERVAL_IN_MS 50000"     //systik interval tick sets for every 50ms
 */
//=====================================================================================================================
#define TIMERITERUPT_LONG_INTERVAL_IN_MS     10000

#endif

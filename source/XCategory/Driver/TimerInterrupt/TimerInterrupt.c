/**
 *	  @file
 *    @brief    Timer Interrupt GDM module for MKV03X implements the
 *              Timer Interrupt Standard API (Application Programming Interface)
 *    @details  Timer Interrupt module gives the Timer interrupt service to the application,
 *              basically it is initialized and after that every defined interval an interruption is generated
 *              which drives the slotted Architecture where user can execute the Application
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: TimerInterrupt.c $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Micro.h"
#include "TimerInterrupt.h"
#include "string.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
/**
 *    @brief  - Defines the Interrupt Interval in us
 *    @details- Due to the Win implementation it is strongly recommended to use the timer interrupt interval of 1ms.
 *              The macro value should not be changed
 *
 *    @param  - 1000 // Do not change the value
 *
 *    @note   - Following is the Example\n
 *              "#define INTERRUPT_INTERVAL   1000"
 */
//=====================================================================================================================
#ifndef INTERRUPT_INTERVAL
#define INTERRUPT_INTERVAL          1000                    //Interrupt interval in us
#endif
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
#ifndef SYSTICK_PREEMPTION_PRIORITY
    #define SYSTICK_PREEMPTION_PRIORITY 3
#endif

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
#ifndef SYSTICK_SUB_PRIORITY
    #define SYSTICK_SUB_PRIORITY        3
#endif

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
 *           "#define TIMERITERUPT_LONG_INTERVAL_IN_US 25000"      //systick interval tick sets for every 25ms
 */
//=====================================================================================================================
#ifndef TIMERITERUPT_LONG_INTERVAL_IN_US
    #define TIMERITERUPT_LONG_INTERVAL_IN_US     25000
#endif

#define DEF_1MHZ                			1000000			//It generates a Timer clock freq = 1MHz
#define DEF_1KHZ                            1000			//It generates a Timer clock freq = 1KHz

#define TIMER_RELOAD_COUNT()            (Micro__GetClock()->HCLK_Frequency / (DEF_1MHZ/INTERRUPT_INTERVAL));
#define TIMER_RELOAD_LONG_COUNT()       (Micro__GetClock()->HCLK_Frequency / (DEF_1MHZ/TIMERITERUPT_LONG_INTERVAL_IN_MS));
#define HCLK_IN_MHZ()                   (Micro__GetClock()->HCLK_Frequency / DEF_1MHZ)
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the Timer interrupt module
 *    @details  1. This method should be called after initialization of all the modules like micro, Gpio, Hmi, Lsi,
 *    				Wide etc.
 *
 *              2. This function initializes the system tick timer and its interrupt and start the system tick timer.
 *		           Counter is in free running mode to generate periodical interrupts.
 *
 *		        3. You can change the SysTick IRQ priority by changing the below macro -
 *		        	#define SYSTICK_PREEMPTION_PRIORITY		0
 *
 *		        4. SysTick time base uses the below formula:
 *		                 Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)
 */
void TimerInterrupt__Initialize(void)
{

	//Disable the System Core timer
	SYSTICK.CTRL &= SYSTICK_COUNTER_DISABLE;

	 //AHB clock divided by 8 selected as SysTick clock source.
	SYSTICK.LOAD  = TIMER_RELOAD_COUNT();

	//Set Priority for SysTick_IRQ_CHANNEL
	Micro__NVICEnableIRQ (SYSTICK_IRQ_CHANNEL, SYSTICK_PREEMPTION_PRIORITY, SYSTICK_SUB_PRIORITY);

	SYSTICK.VAL   = 0;

	// Enable SysTick IRQ CLock source and SysTick Timer
	SYSTICK.CTRL  = SYSTICK_CTRL_CLKSOURCE_MSK | SYSTICK_CTRL_TICKINT_MSK | SYSTICK_CTRL_ENABLE_MSK;

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It sets the timer interrupt for long duration
 *    @details  Long interval value is depends on the 'TIMERITERUPT_LONG_INTERVAL_IN_MS' macro value
 */
void TimerInterrupt__SetLongInterval(void)
{
	//Disable the System Core timer
	SYSTICK.CTRL &= SYSTICK_COUNTER_DISABLE;

	 //AHB clock divided by 8 selected as SysTick clock source.
    SYSTICK.LOAD  = TIMER_RELOAD_LONG_COUNT();

    //Configure the SysTick handler priority
    Micro__NVICEnableIRQ (SYSTICK_IRQ_CHANNEL, SYSTICK_PREEMPTION_PRIORITY, SYSTICK_SUB_PRIORITY);

    SYSTICK.VAL   = 0;

    // Enable SysTick IRQ CLock source and SysTick Timer
    SYSTICK.CTRL  = SYSTICK_CTRL_CLKSOURCE_MSK | SYSTICK_CTRL_TICKINT_MSK | SYSTICK_CTRL_ENABLE_MSK;

    SERVICE_WATCHDOG();
}


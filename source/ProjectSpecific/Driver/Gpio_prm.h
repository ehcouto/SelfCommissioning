/**
 *  @file
 *  @brief  Thre's a default for each register which is applied to the register when Gpio__Initialize and
 *          Gpio__RefreshConfig are called.
 *  	 	\ If the DEFAULT register is not defined it the specific register is not affected.
 *  	 	\ If the DEFAULT register is defined and the MASK is not defined for that specific port the register will be
 *  		full overwrite by the default.
 *  		\ If the DEFAULT register is defined and the MASK is also defined for that specific port, the register will be
 *  		affected by the DEFAULT value only on the bist set to 1 in the MASK define.
 *
 *  $Header: Gpio_prm.h 1.2 2015/08/21 13:51:21EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2012-$Date: 2015/08/21 13:51:21EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef GPIO_PRM_H                                      // Ensure this file is not compiled again
    #define GPIO_PRM_H                                  // after we compile it the first time

//=====================================================================================================================
//-------------------------------------- GPIO configurable macros -----------------------------------------------------
//=====================================================================================================================
/**
 * Examples -
 *
 *  1. Configure the PORTE,pin0 and pin1 in OUTPUT_PUSHPULL mode
 * 		#define PORTA_MASK                  	0xFFFFFFFF		//Mask PORTE
 * 		#define PORTA_PDDR_DEFAULT           	0x00000003
 * 		#define PORTA_PCR			        	0x00000100		//Enable
 *
 *  2. Configure the PORTE,pin25 in INPUT_PULLUP mode
 * 		#define PORTE_MASK                  	0xFFFFFFFF		//Mask PORTE
 * 		#define PORTE_PDDR_DEFAULT           	0xFEFFFFFF		//bit25 is configured as general purpose input
 * 		#define PORTE_PCR[25]		        	0x00000103		//Enable Pullup for bit25
 *
 *  3. Configure the PORTE,pin25 in INPUT_FLOAT mode
 * 		#define PORTE_MASK                  	0xFFFFFFFF		//Mask PORTE
 * 		#define PORTE_PDDR_DEFAULT           	0xFEFFFFFF		//bit25 is configured as general purpose input
 * 		#define PORTE_PCR[25]		        	0x00000100		//Disable Pullup for bit0 and bit7
 *
 *	4.  The PORTA, pin 1 and 7 for in OUTPUT_OPEN_DRAIN state
 * 		#define PORTA_MASK                  	0xFFFFFFFF		//Mask PORTA
 * 		#define PORTA_PDDR_DEFAULT           	0x00000082		//bit0 and bit7 are configured as general purpose input
 * 		#define PORTE_PCR[25]		        	0x00000120		//Open drain output is enabled on the corresponding pin
 *
 *  Note: 1. If the DEFAULT register is not defined, the specific register is not affected.
 *        2. If the DEFAULT register is defined and the MASK is not defined for that specific port the register
 *           will be fully overwritten by the default.
 *        3. If the DEFAULT register is defined and MASK is also defined for that specific port,
 *           the register will be affected by the DEFAULT value only on the bits set to 1 in the MASK define.
 */

/**
 * GPIOA register bit assignments  (GPIOA_XXXX)(XXXX - GPIO registers)
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |PTA19|PTA18|PTA17|PTA16|
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |PTA15|PTA14|PTA13|PTA12|  NA |  NA |  NA |  NA |  NA |  NA |PTA5 |PTA4 |PTA3 |PTA2 |PTA1 |PTA0 |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *
 * GPIOB register bit assignments (GPIOB_XXXX)(XXXX - GPIO registers)
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |PTB23|PTB22|PTB21|PTB20|PTB19|PTB18|PTB17|PTB16|
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |PTB11|PTB10|PTB9 |  NA |  NA |  NA |  NA |  NA |PTB3 |PTB2 |PTB1 |PTB0 |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *
 * GPIOC register bit assignments (GPIOC_XXXX)(XXXX - GPIO registers)
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |PTC18|PTC17|PTC16|
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |PTC15|PTC14|PTC13|PTC12|PTC11|PTC10|PTC9 |PTC8 |PTC7 |PTC6 |PTC5 |PTC4 |PTC3 |PTC2 |PTC1 |PTC0 |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *
 * GPIOD register bit assignments  (GPIOA_XXXX)(XXXX - GPIO registers)
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |PTD7 |PTD6 |PTD5 |PTD4 |PTD3 |PTD2 |PTD1 |PTD0 |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *
 * GPIOE register bit assignments  (GPIOA_XXXX)(XXXX - GPIO registers)
 *
 *    31     30   29    28    27    26    25    24    23    22     21    20     19   18    17    16
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |  NA |PTE26|PTE25|PTE24|  NA |  NA |  NA |  NA |PTE19|PTE18|PTE17|PTE16|
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 *    15     14   13    12    11    10     9     8     7     6     5     4     3     2     1     0
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *  |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |  NA |PTE6 |PTE5 |PTE4 |PTE3 |PTE2 |PTE1 |PTE0 |
 *  |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
 *
 */
//=====================================================================================================================
//*********** PDDR - Port Data Direction Register ***********
// PDDR - 0 - pin configured as general purpose input, The Pin will be high-Z if the port i/p is disable in GPIOx_PIDR register.
// PDDR - 1 - pin configured as general purpose output
//*********** PIDR - Port Input Disable Register ************
// PIDR - 0 - Pin is configured for General purpose input
// PIDR - 1 - Pin is not configured for General purpose input
//*********** PDOR Port Data Output Register ****************
// PDOR - 0 - logic '0' driven on PIN
// PDOR - 1 - logic '1' driven on PIN

//PORTA
#define PORTA_MASK                  	0xFF		//Mask PORTA
#define PORTA_PDOR_DEFAULT           	0x00		//Default Logic '0' is driven on pin
#define PORTA_PDDR_DEFAULT           	0xFF		//Default All port pin's are configured as general purpose output
#define PORTA_PIDR_DEFAULT          	0xFF		//Default pin's are not configured as general purpose input
#define PORTA_PULLUP_DEFAULT            0x00        //Default Pullup is disabled
#define PORTA_PCR_DEFAULT               0x00000100

//PORTB
#define PORTB_MASK                  	0xFF		//Mask PORTA
#define PORTB_PDOR_DEFAULT            	0x00		//Logic '0' is driven on pin
#define PORTB_PDDR_DEFAULT           	0xFF
#define PORTB_PIDR_DEFAULT           	0xFF		//Default port pin's are not configured as general purpose input
#define PORTB_PULLUP_DEFAULT         	0x00		//Default Pullup is disabled

//PORTC
#define PORTC_MASK                  	0xFF		//Mask PORTC
#define PORTC_PDOR_DEFAULT            	0x00		//Logic '0' is driven on pin
#define PORTC_PDDR_DEFAULT           	0xFF
#define PORTC_PIDR_DEFAULT          	0xFF		//Default port pin's are not configured as general purpose input
#define PORTC_PULLUP_DEFAULT         	0x00		//Default Pullup is disabled

//PORTD
#define PORTD_MASK                  	0xFF		//Mask PORTD
#define PORTD_PDOR_DEFAULT            	0x00		//Logic '0' is driven on pin
#define PORTD_PDDR_DEFAULT           	0xFF
#define PORTD_PIDR_DEFAULT          	0xFF		//Default port pin's are not configured as general purpose input
#define PORTD_PULLUP_DEFAULT        	0x00		//Default Pullup is disabled

//PORTE
#define PORTE_MASK                  	0xFF		//Mask PORTE
#define PORTE_PDOR_DEFAULT            	0x00		//Logic '0' is driven on pin
#define PORTE_PDDR_DEFAULT           	0xFF
#define PORTE_PIDR_DEFAULT          	0xFF		//Default port pin's are not configured as general purpose input
#define PORTE_PULLUP_DEFAULT        	0x00		//Default Pullup is disabled

//PORTF
#define PORTF_MASK                  	0xFF		//Mask PORTF
#define PORTF_PDOR_DEFAULT            	0x00		//Logic '0' is driven on pin
#define PORTF_PDDR_DEFAULT           	0xFF
#define PORTF_PIDR_DEFAULT          	0xFF		//Default port pin's are not configured as general purpose input
#define PORTF_PULLUP_DEFAULT        	0x00		//Default Pullup is disabled

//PORTG
#define PORTG_MASK                  	0xFF		//Mask PORTG
#define PORTG_PDOR_DEFAULT            	0x00		//Logic '0' is driven on pin
#define PORTG_PDDR_DEFAULT           	0xFF
#define PORTG_PIDR_DEFAULT           	0xFF		//Default port pin's are not configured as general purpose input
#define PORTG_PULLUP_DEFAULT        	0x00		//Default Pullup is disabled

//PORTH
#define PORTH_MASK                  	0xFF		//Mask PORTH
#define PORTH_PDOR_DEFAULT            	0x00		//Logic '0' is driven on pin
#define PORTH_PDDR_DEFAULT           	0xFF
#define PORTH_PIDR_DEFAULT          	0xFF		//Default port pin's are not configured as general purpose input
#define PORTH_PULLUP_DEFAULT         	0x00		//Default Pullup is disabled

//PORTI
#define PORTI_MASK                  	0xFF		//Mask PORTI
#define PORTI_PDOR_DEFAULT            	0x00		//Logic '0' is driven on pin
#define PORTI_PDDR_DEFAULT           	0xFF
#define PORTI_PIDR_DEFAULT          	0xFF		//Default port pin's are not configured as general purpose input
#define PORTI_PULLUP_DEFAULT         	0x00		//Default Pullup is disabled

//This directive controls the code that protects some operations about being interrupted.
//IT accepts the options ENABLED / DISABLED
#define GPIO_PROTECTION_FEATURE   ENABLED

#endif

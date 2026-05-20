/**
 *  @file
 *  @brief      MKV3x Macros to fast access to the port bits
 *
 *  @details    NOTE ABOUT USE THE MACROS !!!
 *             READ THIS
 *
 *    The macros allow the application to access the registers related to the Gpio.
 *    This kind of access implies in operations like read, manipulate and write it back.
 *    It potentially is exposed to issues like conflict when called from Main and Interrupts since it's not an atomic instruction.
 *    It's fine to call it from processes handled from the timer interruption
 *
 *    It's NOT recommended use the macros for process from the Main code (background processes), use Gpio functions instead.
 *    To use the Macros from the background processes it recommend to do as atomic instructions, for that the code should follow the following example:
 *
 *    Micro__SaveInterruptEnableFlag();
 *    code....
 *    Micro__RestoreInterruptEnableFlag();
 *
 *  $Header: Gpio_macros.h 1.1 2015/06/10 14:02:50EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2012-$Date: 2015/06/10 14:02:50EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef GPIO_MACROS_H_
	#define GPIO_MACROS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
extern unsigned char const GPIO_SET_MASK[];
extern unsigned char const GPIO_CLR_MASK[];
extern volatile GPIO_DEF * const GPIO_PORT_LIST[];

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
//PORT = PORTA / PORTB / PORTC / PORTD / PORTE / PORTF / PORTG / PORTH / PORTI

#define GPIO__PIN_SET(PORT, PORTBIT)							{ GPIO_PORT_LIST[PORT]->PSOR = (1<<PORTBIT); }
#define GPIO__PIN_CLR(PORT, PORTBIT)							{ GPIO_PORT_LIST[PORT]->PCOR = (1<<PORTBIT); }
#define GPIO__PIN_TOGGLE(PORT, PORTBIT)							{ GPIO_PORT_LIST[PORT]->PTOR = (1<<PORTBIT); }
#define GPIO__PIN_WRITE(PORT, PORTBIT,VALUE)					{ if (VALUE == 0) { GPIO__PIN_CLR(PORT,PORTBIT); } else { GPIO__PIN_SET(PORT,PORTBIT); } }
#define GPIO__PIN_READ(PORT, PORTBIT)							( (GPIO_PORT_LIST[PORT]->PDIR >> PORTBIT) & 0x01 )

#define GPIO__PORT_SET(PORT, MASK)								{ GPIO_PORT_LIST[PORT]->PSOR = (MASK); }
#define GPIO__PORT_CLR(PORT, MASK)								{ GPIO_PORT_LIST[PORT]->PCOR = (MASK); }
#define GPIO__PORT_READ(PORT)									( GPIO_PORT_LIST[PORT]->PDIR )

#define GPIO__PIN_COFIG_I_FLOAT(PORT, PORTBIT)      			{ Gpio__PinConfig(PORT,PORTBIT,INPUT_FLOAT); }
#define GPIO__PIN_COFIG_I_PULLUP(PORT, PORTBIT)     			{ Gpio__PinConfig(PORT,PORTBIT,INPUT_PULLUP); }
#define GPIO__PIN_COFIG_O_PUSHPULL(PORT, PORTBIT)   			{ Gpio__PinConfig(PORT,PORTBIT,OUTPUT_PUSHPULL); }
#define GPIO__PIN_COFIG_O_OPENDRAIN(PORT, PORTBIT)  			{ Gpio__PinConfig(PORT,PORTBIT,OUTPUT_OPEN_DRAIN); }

#endif // GPIO_MACROS_H_

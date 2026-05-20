/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This file contains the array of interruptions functions
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2011-$Date: 2015/08/19 09:04:18EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "InterruptVector_prm.h"
 
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#pragma language=extended
#pragma segment="CSTACK"

void __iar_program_start( void );
#pragma location = ".intvec"

//=====================================================================================================================
//-------------------------------------- Interrupt Vector Table -------------------------------------------------------
//=====================================================================================================================
//MKV3x Vector Table entries
const intvec_elem __vector_table[] =
{
	{ .__ptr = __sfe( "CSTACK" ) },				//Start of ARM Core System Handler Vectors
	RESET_IRQHANDLER,
    NMI_HANDLER,
    HARDFAULT_HANDLER,
	MEMMANAGEFAULT_HANDLER,
	BUSFAULT_HANDLER,
	USAGEFAULT_HANDLER,
	0,
	0,
	0,
	0,											//Reserved
	SVC_HANDLER,
	DEBUGMONITOR_HANDLER,
	0,
	PENDSV_HANDLER,
	SYSTICK_HANDLER,							//End of ARM Core System Handler Vectors
	DMA_0_TRNASFER_COMPLETED_HANDLER,			//0:Start of Non-Core Vectors
	DMA_1_TRNASFER_COMPLETED_HANDLER,			//1:
	DMA_2_TRNASFER_COMPLETED_HANDLER,			//2:
	DMA_3_TRNASFER_COMPLETED_HANDLER,			//3:
	DMA_4_TRNASFER_COMPLETED_HANDLER,			//4:
	DMA_5_TRNASFER_COMPLETED_HANDLER,			//5:
	DMA_6_TRNASFER_COMPLETED_HANDLER,			//6:
	DMA_7_TRNASFER_COMPLETED_HANDLER,			//7:
	DMA_8_TRNASFER_COMPLETED_HANDLER,			//8:
	DMA_9_TRNASFER_COMPLETED_HANDLER,			//9
	DMA_10_TRNASFER_COMPLETED_HANDLER,			//10
	DMA_11_TRNASFER_COMPLETED_HANDLER,			//11:
	DMA_12_TRNASFER_COMPLETED_HANDLER,          //12:
	DMA_13_TRNASFER_COMPLETED_HANDLER,          //13:
	DMA_14_TRNASFER_COMPLETED_HANDLER,          //14:
	DMA_15_TRNASFER_COMPLETED_HANDLER,          //15:
	DMA_ERROR_INTERRUPT_HANDLER,                //16:
	MCM_HANDLER,                                //17:
	COMMAND_COMPLETE_HANDLER,                   //18:
	READ_COLLISION,                             //19:
	LBDLVM_HANLDER,                             //20:
	LOW_LEAKAGE_WAKEUP_HANDLER,                 //21:
	WDOG_HANDLER,								//22: WDOG or EWM (Both WD modules share this interrupt
	RNG_HANDLER,								//23: Random Number Generator
	I2C0_HANLDER,								//24:
	I2C1_HANLDER,                               //25:
	SPI0_HANDLER,                               //26:
	SPI1_HANDLER,                               //27:
	0,                                          //28:
	0,                                          //29:
	LPUART0_HANDLER,							//30: Status and Error
	UART0_HANDLER,								//31: Single Interrupt vector for UART status sources
	UART0_HANDLER,								//32: Single Interrupt vector for UART status sources
	UART1_HANDLER,								//33:
	UART1_HANDLER,                              //34:
	UART2_HANDLER,                              //35:
	UART2_HANDLER,                              //36:
	0,                                          //37:
	0,                                          //38:
	ADC_HANDLER,                                //39:
	ACMP0_HANDLER,                              //40:
	ACMP1_HANDLER,								//41:
	FLEXTIME0_HANDLER,                          //42:
	FLEXTIME1_HANDLER,                          //43:
	FLEXTIME2_HANDLER,                          //44:
	0,                                          //45:
	0,                                          //46:
	0,                                          //47:
	PIT_CH0_HANDLER,							//48: PIT Channel0
	PIT_CH1_HANDLER,							//49: PIT Channel1
	PIT_CH2_HANDLER,							//50: PIT Channel2
	PIT_CH3_HANDLER,							//51: PIT Channel3
	PDB_HANDLER,                                //52:
	0,                                          //53:
	0,                                          //54:
	0,                                          //55:
	DAC0_HANDLER,                               //56:
	MCG_HANDLER,                                //57:
	LOW_POWER_TIMER,                            //58:
//	EXTI_PORTA_CONTROLMODULE_HANDLER,				//59: Pin detect(Port A)
//	EXTI_PORTB_CONTROLMODULE_HANDLER,				//60: Pin detect(Port B)
//	EXTI_PORTC_CONTROLMODULE_HANDLER,				//61: Pin detect(Port C)
//	EXTI_PORTD_CONTROLMODULE_HANDLER,				//62: Pin detect(Port D)
//	EXTI_PORTE_CONTROLMODULE_HANDLER,				//63: Pin detect(Port E)
	PORTA_CONTROL_MODULE_HANDLER,				//59: Pin detect(Port A)
	PORTB_CONTROL_MODULE_HANDLER,				//60: Pin detect(Port B)
	PORTC_CONTROL_MODULE_HANDLER,				//61: Pin detect(Port C)
	PORTD_CONTROL_MODULE_HANDLER,				//62: Pin detect(Port D)
	PORTE_CONTROL_MODULE_HANDLER,				//63: Pin detect(Port E)
	SOFTWARE_HANDLER,							//64:
	0,                                          //65:
	0,                                          //66:
	0,                                          //67:
	0,                                          //68:
	0,                                          //69:
	0,                                          //70:
	FLEXTIME3_HANDLER,                          //71:
	DAC1_HANDLER,                               //72:
	ADC1_HANDLER                                //73:
};

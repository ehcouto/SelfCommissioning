/**
 *    @file     InterruptVector_prm.h
 *    @brief    This file contains configurable section for handling ISR routines in application.
 *
 *    $Header: SRInterruptVector_prm.h 1.4 2015/08/21 13:51:20EDT Natak Rahul (NATAKRB) Exp  $
 *
 *    @copyright  *****  Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL  *****
 */
#ifndef INTERRUPTVECTOR_PRM_H_
	#define INTERRUPTVECTOR_PRM_H_

#include "Micro.h"
#include "Uart.h"
#include "TimerInterrupt.h"
#include "Uart.h"
#include "SRMCAtod.h"
#include "SRMCPwm.h"
#include "SRMain.h"

extern void PendSV_Handler(void);
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//////////////////////////////////////////////////////////
// Include the header files for the interruption sources
//////////////////////////////////////////////////////////

	//!Interrupt Vectors

#define RESET_IRQHANDLER                            Micro__BootTest
#define NMI_HANDLER                                 Micro__Trap
#define HARDFAULT_HANDLER                           Micro__Trap
#define MEMMANAGEFAULT_HANDLER                      Micro__Trap
#define BUSFAULT_HANDLER                            Micro__Trap
#define USAGEFAULT_HANDLER                          Micro__Trap
#define SVC_HANDLER                                 Micro__Trap
#define DEBUGMONITOR_HANDLER                        Micro__Trap
#define PENDSV_HANDLER                              Micro__Trap
#define SYSTICK_HANDLER                             SRMain__TimerISR
#define DMA_0_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define DMA_1_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define DMA_2_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define DMA_3_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define DMA_4_TRNASFER_COMPLETED_HANDLER			Micro__Trap
#define DMA_5_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define	DMA_6_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define	DMA_7_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define	DMA_8_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define	DMA_9_TRNASFER_COMPLETED_HANDLER            Micro__Trap
#define	DMA_10_TRNASFER_COMPLETED_HANDLER           Micro__Trap
#define	DMA_11_TRNASFER_COMPLETED_HANDLER           Micro__Trap
#define	DMA_12_TRNASFER_COMPLETED_HANDLER           Micro__Trap
#define	DMA_13_TRNASFER_COMPLETED_HANDLER           Micro__Trap
#define	DMA_14_TRNASFER_COMPLETED_HANDLER           Micro__Trap
#define	DMA_15_TRNASFER_COMPLETED_HANDLER           Micro__Trap
#define	DMA_ERROR_INTERRUPT_HANDLER                 Micro__Trap
#define MCM_HANDLER                                 Micro__Trap
#define COMMAND_COMPLETE_HANDLER                    Micro__Trap
#define READ_COLLISION                              Micro__Trap
#define FTMRH_HANDLER                               Micro__Trap
#define LBDLVM_HANLDER                              Micro__Trap
#define LOW_LEAKAGE_WAKEUP_HANDLER                  Micro__Trap
#define RNG_HANDLER									Micro__Trap
#define I2C0_HANLDER                                Micro__Trap
#define I2C1_HANLDER                                Micro__Trap
#define SPI0_HANDLER                                Micro__Trap
#define SPI1_HANDLER                                Micro__Trap
#define LPUART0_HANDLER								Micro__Trap
#define UART0_HANDLER                               Uart__Interrupt0
#define UART1_HANDLER                               Uart__Interrupt1
#define UART2_HANDLER                               Uart__Interrupt2
#define ADC_HANDLER                                 Micro__Trap
#define ACMP0_HANDLER                               Micro__Trap
#define FLEXTIME0_HANDLER                           SRMCPwm__PwmISR
#define FLEXTIME1_HANDLER                           Micro__Trap
#define FLEXTIME2_HANDLER                           Micro__Trap
#define RTC_HANDLER                                 Micro__Trap
#define ACMP1_HANDLER                               Micro__Trap
#define PIT_CH0_HANDLER                             Micro__Trap
#define PIT_CH1_HANDLER                             Micro__Trap
#define PIT_CH2_HANDLER                             Micro__Trap
#define PIT_CH3_HANDLER                             Micro__Trap
#define PDB_HANDLER                                 SRMCPDB__ISR
#define DAC0_HANDLER                           		Micro__Trap
#define MCG_HANDLER                           		Micro__Trap
#define ICS_HANDLER                                 Micro__Trap
#define WDOG_HANDLER                                Micro__Trap
#define LOW_POWER_TIMER                             Micro__LPTimerInterrupt
#define PORTA_CONTROL_MODULE_HANDLER                Micro__Trap
#define PORTB_CONTROL_MODULE_HANDLER                Micro__Trap
#define PORTC_CONTROL_MODULE_HANDLER                Micro__Trap
#define PORTD_CONTROL_MODULE_HANDLER                Micro__Trap
#define PORTE_CONTROL_MODULE_HANDLER                Micro__Trap
#define SOFTWARE_HANDLER                            Micro__Trap
#define FLEXTIME3_HANDLER							Micro__Trap
#define DAC1_HANDLER								Micro__Trap
#define ADC1_HANDLER								SRMCAtod__ISR
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // INTERRUPTVECTOR_PRM_H_



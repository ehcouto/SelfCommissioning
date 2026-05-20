/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This file contains the uc_MKV3x controller related register definition.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: uc_MKV3x.h $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef UC_MKV30_H_
    #define UC_MKV30_H_      DISABLED      //CONFIGURE Micro as MKV30_H
#endif
#ifndef UC_MKV31_H_
    #define UC_MKV31_H_      ENABLED     //CONFIGURE Micro as MKV31_H


#include "uc_MKV3x_prm.h"
#include "Compiler_defs.h"
#include "C_Extensions.h"
#pragma language=extended
#pragma segment="CSTACK"

#pragma pack(push)
#pragma pack()


#define __MPU_PRESENT                  0         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               4         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined */
#define __FPU_PRESENT                  1         /**< Defines if an FPU is present or not */

//#include "core_cm4.h"                  /* Core Peripheral Access Layer */

// ============================================================
// --------------------- MEMORY SEGMENTS ----------------------
// ------------------------------------------------------------
#define FLASH_BASE              ((uint32)0x08000000) 					//! FLASH base address in the alias region
#define SRAM_BASE               ((uint32)0x20000000) 					//! SRAM base address in the alias region
//#define PERIPH_BASE           ((uint32)0x40000000) 					//! Peripheral base address in the alias region
#define CORE_PERIPH_BASE        ((uint32)0xE0000000) 					//! Peripheral base address in the alias region

//! Memory mapping of Cortex-M0 Hardware
#define SCS_BASE                (CORE_PERIPH_BASE + 0xE000)     		//! System Control Space Base Address
#define SYSTICK_BASE            (SCS_BASE +  0x0010UL)          		//! SysTick Base Address
#define NVIC_BASE               (SCS_BASE +  0x0100UL)          		//! NVIC Base Address
#define SCB_BASE                (SCS_BASE +  0x0D00UL)          		//! System Control Block Base Address

#define DBGMCU                  (*(DBGMCU_DEF *) DBGMCU_BASE)
#define SCnSCB                  (*(SCnSCB_DEF*) SCS_BASE      )     	//! System control Register not in SCB
#define SCB                     (*(SCB_DEF*) SCB_BASE      )        	//! SCB configuration struct
#define SYSTICK                 (*(SYSTICK_DEF *) SYSTICK_BASE)
#define NVIC                    (*(NVIC_DEF *) NVIC_BASE     )     	 	//! NVIC configuration struct
#define ITM                     (*(ITM_DEF *) ITM_BASE      )       	//! ITM configuration struct
#define CoreDebug               (*(COREDEBUG_DEF *) COREDEBUG_BASE) 	//! Core Debug configuration struct

#define AIRC_VECTKEY_MASK       ((uint32)0x05FA0000)            		// See section 2.7.4 STR750 Reference Manual
#define SCB_ADD                 (0xE0000000L + 0xE000L +0x0D00UL)
#define SCB_AIRCR               *((volatile uint32 *)(SCB_ADD + 12L))

// ============================================================
// ---------------------- BIT DEFINITIONS ---------------------
// ------------------------------------------------------------
#define BIT(bit)                (1<<(bit))

typedef enum
{
    RESET_HANDLER_INDEX                     = 1
} IRQ_INDEX_TYPE;



// ============================================================
// --------------------- TYPE DEFINITIONS ---------------------
// ------------------------------------------------------------
typedef union
{
    volatile uint32 dword;
    struct
    {
        volatile uint32 bit0              :1;
        volatile uint32 bit1              :1;
        volatile uint32 bit2              :1;
        volatile uint32 bit3              :1;
        volatile uint32 bit4              :1;
        volatile uint32 bit5              :1;
        volatile uint32 bit6              :1;
        volatile uint32 bit7              :1;
        volatile uint32 bit8              :1;
        volatile uint32 bit9              :1;
        volatile uint32 bit10             :1;
        volatile uint32 bit11             :1;
        volatile uint32 bit12             :1;
        volatile uint32 bit13             :1;
        volatile uint32 bit14             :1;
        volatile uint32 bit15             :1;
        volatile uint32 bit16             :1;
        volatile uint32 bit17             :1;
        volatile uint32 bit18             :1;
        volatile uint32 bit19             :1;
        volatile uint32 bit20             :1;
        volatile uint32 bit21             :1;
        volatile uint32 bit22             :1;
        volatile uint32 bit23             :1;
        volatile uint32 bit24             :1;
        volatile uint32 bit25             :1;
        volatile uint32 bit26             :1;
        volatile uint32 bit27             :1;
        volatile uint32 bit28             :1;
        volatile uint32 bit29             :1;
        volatile uint32 bit30             :1;
        volatile uint32 bit31             :1;
    } bit;
} REG_32BITS_DEF;

typedef union
{
    volatile uint16 word;
    struct
    {
        volatile uint16 bit0              :1;
        volatile uint16 bit1              :1;
        volatile uint16 bit2              :1;
        volatile uint16 bit3              :1;
        volatile uint16 bit4              :1;
        volatile uint16 bit5              :1;
        volatile uint16 bit6              :1;
        volatile uint16 bit7              :1;
        volatile uint16 bit8              :1;
        volatile uint16 bit9              :1;
        volatile uint16 bit10             :1;
        volatile uint16 bit11             :1;
        volatile uint16 bit12             :1;
        volatile uint16 bit13             :1;
        volatile uint16 bit14             :1;
        volatile uint16 bit15             :1;
    } bit;
} REG_16BITS_DEF;

//============================================================
/*
 * @brief General macro defines
 */
//MKV3x_FAMILY - Core/System Clock @48Mhz, 4-24Mhz(crystal)
                                                     //Core runs DC up to 48Mhz,
#define HSI_VALUE                           48000000               //For FEI: FLL Engaged Internal Oscillator:Pre factory trimmed internal reference for 48Mhz system clock
#define HSI_VALUE_SLOW                      48000000               //For FEI: FLL Engaged Internal Oscillator:Pre factory trimmed internal reference for 48Mhz system clock
#define MICRO_LSI_SLOW_MCGIRCLK_VALUE		32000
#define MICRO_LSI_FAST_MCGIRCLK_VALUE 		4000000


//=====================================================================================================================
//--------------------------------------------- MICROCONTROLLER REGISTERS ---------------------------------------------
//=====================================================================================================================


typedef enum IRQn {
  /* Auxiliary constants */
  NotAvail_IRQn                = -128,             /**< Not available device specific interrupt */

  /* Core interrupts */
  NonMaskableInt_IRQn          = -14,              /**< Non Maskable Interrupt */
  HardFault_IRQn               = -13,              /**< Cortex-M4 SV Hard Fault Interrupt */
  MemoryManagement_IRQn        = -12,              /**< Cortex-M4 Memory Management Interrupt */
  BusFault_IRQn                = -11,              /**< Cortex-M4 Bus Fault Interrupt */
  UsageFault_IRQn              = -10,              /**< Cortex-M4 Usage Fault Interrupt */
  SVCall_IRQn                  = -5,               /**< Cortex-M4 SV Call Interrupt */
  DebugMonitor_IRQn            = -4,               /**< Cortex-M4 Debug Monitor Interrupt */
  PendSV_IRQn                  = -2,               /**< Cortex-M4 Pend SV Interrupt */
  SysTick_IRQn                 = -1,               /**< Cortex-M4 System Tick Interrupt */

  /* Device specific interrupts */
  DMA0_IRQn                    = 0,                /**< DMA Channel 0 Transfer Complete */
  DMA1_IRQn                    = 1,                /**< DMA Channel 1 Transfer Complete */
  DMA2_IRQn                    = 2,                /**< DMA Channel 2 Transfer Complete */
  DMA3_IRQn                    = 3,                /**< DMA Channel 3 Transfer Complete */
  DMA4_IRQn                    = 4,                /**< DMA Channel 4 Transfer Complete */
  DMA5_IRQn                    = 5,                /**< DMA Channel 5 Transfer Complete */
  DMA6_IRQn                    = 6,                /**< DMA Channel 6 Transfer Complete */
  DMA7_IRQn                    = 7,                /**< DMA Channel 7 Transfer Complete */
  DMA8_IRQn                    = 8,                /**< DMA Channel 8 Transfer Complete */
  DMA9_IRQn                    = 9,                /**< DMA Channel 9 Transfer Complete */
  DMA10_IRQn                   = 10,               /**< DMA Channel 10 Transfer Complete */
  DMA11_IRQn                   = 11,               /**< DMA Channel 11 Transfer Complete */
  DMA12_IRQn                   = 12,               /**< DMA Channel 12 Transfer Complete */
  DMA13_IRQn                   = 13,               /**< DMA Channel 13 Transfer Complete */
  DMA14_IRQn                   = 14,               /**< DMA Channel 14 Transfer Complete */
  DMA15_IRQn                   = 15,               /**< DMA Channel 15 Transfer Complete */
  DMA_Error_IRQn               = 16,               /**< DMA Error Interrupt */
  MCM_IRQn                     = 17,               /**< Normal Interrupt */
  FTF_IRQn                     = 18,               /**< FTFA Command complete interrupt */
  Read_Collision_IRQn          = 19,               /**< Read Collision Interrupt */
  LVD_LVW_IRQn                 = 20,               /**< Low Voltage Detect, Low Voltage Warning */
  LLWU_IRQn                    = 21,               /**< Low Leakage Wakeup Unit */
  WDOG_EWM_IRQn                = 22,               /**< WDOG Interrupt */
  RNG_IRQn                     = 23,               /**< RNG Interrupt */
  I2C0_IRQn                    = 24,               /**< I2C0 interrupt */
  I2C1_IRQn                    = 25,               /**< I2C1 interrupt */
  SPI0_IRQn                    = 26,               /**< SPI0 Interrupt */
  SPI1_IRQn                    = 27,               /**< SPI1 Interrupt */
  Reserved44_IRQn              = 28,               /**< Reserved Interrupt 44 */
  Reserved45_IRQn              = 29,               /**< Reserved interrupt 45 */
  LPUART0_IRQn                 = 30,               /**< LPUART0 status/error interrupt */
  UART0_RX_TX_IRQn             = 31,               /**< UART0 Receive/Transmit interrupt */
  UART0_ERR_IRQn               = 32,               /**< UART0 Error interrupt */
  UART1_RX_TX_IRQn             = 33,               /**< UART1 Receive/Transmit interrupt */
  UART1_ERR_IRQn               = 34,               /**< UART1 Error interrupt */
  UART2_RX_TX_IRQn             = 35,               /**< UART2 Receive/Transmit interrupt */
  UART2_ERR_IRQn               = 36,               /**< UART2 Error interrupt */
  Reserved53_IRQn              = 37,               /**< Reserved interrupt 53 */
  Reserved54_IRQn              = 38,               /**< Reserved interrupt 54 */
  ADC0_IRQn                    = 39,               /**< ADC0 interrupt */
  CMP0_IRQn                    = 40,               /**< CMP0 interrupt */
  CMP1_IRQn                    = 41,               /**< CMP1 interrupt */
  FTM0_IRQn                    = 42,               /**< FTM0 fault, overflow and channels interrupt */
  FTM1_IRQn                    = 43,               /**< FTM1 fault, overflow and channels interrupt */
  FTM2_IRQn                    = 44,               /**< FTM2 fault, overflow and channels interrupt */
  Reserved61_IRQn              = 45,               /**< Reserved interrupt 61 */
  Reserved62_IRQn              = 46,               /**< Reserved interrupt 62 */
  Reserved63_IRQn              = 47,               /**< Reserved interrupt 63 */
  PIT0_IRQn                    = 48,               /**< PIT timer channel 0 interrupt */
  PIT1_IRQn                    = 49,               /**< PIT timer channel 1 interrupt */
  PIT2_IRQn                    = 50,               /**< PIT timer channel 2 interrupt */
  PIT3_IRQn                    = 51,               /**< PIT timer channel 3 interrupt */
  PDB0_IRQn                    = 52,               /**< PDB0 Interrupt */
  Reserved69_IRQn              = 53,               /**< Reserved interrupt 69 */
  Reserved70_IRQn              = 54,               /**< Reserved interrupt 70 */
  Reserved71_IRQn              = 55,               /**< Reserved interrupt 71 */
  DAC0_IRQn                    = 56,               /**< DAC0 interrupt */
  MCG_IRQn                     = 57,               /**< MCG Interrupt */
  LPTMR0_IRQn                  = 58,               /**< LPTimer interrupt */
  PORTA_IRQn                   = 59,               /**< Port A interrupt */
  PORTB_IRQn                   = 60,               /**< Port B interrupt */
  PORTC_IRQn                   = 61,               /**< Port C interrupt */
  PORTD_IRQn                   = 62,               /**< Port D interrupt */
  PORTE_IRQn                   = 63,               /**< Port E interrupt */
  SWI_IRQn                     = 64,               /**< Software interrupt */
  Reserved81_IRQn              = 65,               /**< Reserved interrupt 81 */
  Reserved82_IRQn              = 66,               /**< Reserved interrupt 82 */
  Reserved83_IRQn              = 67,               /**< Reserved interrupt 83 */
  Reserved84_IRQn              = 68,               /**< Reserved interrupt 84 */
  Reserved85_IRQn              = 69,               /**< Reserved interrupt 85 */
  Reserved86_IRQn              = 70,               /**< Reserved interrupt 86 */
  FTM3_IRQn                    = 71,               /**< FTM3 fault, overflow and channels interrupt */
  DAC1_IRQn                    = 72,               /**< DAC1 interrupt */
  ADC1_IRQn                    = 73,               /**< ADC1 interrupt */
  Reserved90_IRQn              = 74,               /**< Reserved Interrupt 90 */
  Reserved91_IRQn              = 75,               /**< Reserved Interrupt 91 */
  Reserved92_IRQn              = 76,               /**< Reserved Interrupt 92 */
  Reserved93_IRQn              = 77,               /**< Reserved Interrupt 93 */
  Reserved94_IRQn              = 78,               /**< Reserved Interrupt 94 */
  Reserved95_IRQn              = 79,               /**< Reserved Interrupt 95 */
  Reserved96_IRQn              = 80,               /**< Reserved Interrupt 96 */
  Reserved97_IRQn              = 81,               /**< Reserved Interrupt 97 */
  Reserved98_IRQn              = 82,               /**< Reserved Interrupt 98 */
  Reserved99_IRQn              = 83,               /**< Reserved Interrupt 99 */
  Reserved100_IRQn             = 84,               /**< Reserved Interrupt 100 */
  Reserved101_IRQn             = 85                /**< Reserved Interrupt 101 */
} IRQn_Type;

typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

//============================================================
/*
 * @brief Structure type to access the Nested Vectored Interrupt Controller (NVIC).
 */
typedef struct
{
    volatile uint32 Enable[8];           							//! Interrupt Set Enable Register
    volatile uint32 RESERVED0[24];
    volatile uint32 Disable[8];          							//! Interrupt Clear Enable Register
    volatile uint32 RSERVED1[24];
    volatile uint32 Set[8];              							//! Interrupt Set Pending Register
    volatile uint32 RESERVED2[24];
    volatile uint32 Clear[8];            							//! Interrupt Clear Pending Register
    volatile uint32 RESERVED3[24];
    volatile uint32 Active[8];           							//! Interrupt Active bit Register
    volatile uint32 RESERVED4[56];
    volatile uint8  Priority[240];      							//! Interrupt Priority Register (8Bit wide)
    volatile uint32 RESERVED5[644];
    volatile uint32 Trigger;             							//! Software Trigger Interrupt Register
} NVIC_DEF;

#define NVIC_PRIO_BITS                        4               		// STM32F4x uses 4 Bits for the Priority Levels
#define NVIC_INTERRUPT_ARRAY                  8
#define NVIC_INTERRUPT_PRIORITY_ARRAY         240
#define NVIC_INTERRUPT_PRIORITY_CLEAR         0x00000000

#define NVIC_PRIORITYGROUP_0                  ((uint32)0x700) 		// 0 bits for pre-emption priority 4 bits for sub-priority
#define NVIC_PRIORITYGROUP_1                  ((uint32)0x600) 		// 1 bits for pre-emption priority 3 bits for sub-priority
#define NVIC_PRIORITYGROUP_2                  ((uint32)0x500)		// 2 bits for pre-emption priority 2 bits for sub-priority
#define NVIC_PRIORITYGROUP_3                  ((uint32)0x400)		// 3 bits for pre-emption priority 1 bits for sub-priority
#define NVIC_PRIORITYGROUP_4                  ((uint32)0x300)		// 4 bits for pre-emption priority 0 bits for sub-priority

//============================================================
/*
 * @brief System Control block(SCB)
 */
typedef struct
{
    const uint32 CPUID;                                             //!  CPUID Base Register
    volatile uint32 ICSR;                                           //!  Interrupt Control and State Register
    volatile uint32 VTOR;                                           //!  Vector Table Offset Register
    volatile uint32 AIRCR;                                          //!  Application Interrupt and Reset Control Register
    volatile uint32 SCR;                                            //!  System Control Register
    volatile uint32 CCR;                                            //!  Configuration Control Register
    volatile uint8  SHP[12];                                        //!  System Handlers Priority Registers (4-7, 8-11, 12-15)
    volatile uint32 SHCSR;                                          //!  System Handler Control and State Register
    volatile uint32 CFSR;                                           //!  Configurable Fault Status Register
    volatile uint32 HFSR;                                           //!  HardFault Status Register
    volatile uint32 DFSR;                                           //!  Debug Fault Status Register
    volatile uint32 MMFAR;                                          //!  MemManage Fault Address Register
    volatile uint32 BFAR;                                           //!  BusFault Address Register
    volatile uint32 AFSR;                                           //!  Auxiliary Fault Status Register
    const uint32 PFR[2];                                            //!  Processor Feature Register
    const uint32 DFR;                                               //!  Debug Feature Register
    const uint32 ADR;                                               //!  Auxiliary Feature Register
    const uint32 MMFR[4];                                           //!  Memory Model Feature Register
    const uint32 ISAR[5];                                           //!  Instruction Set Attributes Register
    volatile uint32 RESERVED0[5];
    volatile uint32 CPACR;                                          //!  Co-processor Access Control Register
} SCB_DEF;

//! AIRCR Key for write access
#define NVIC_AIRCR_VECTKEY    (0x5FA << 16)

//! System Reset Request
#define NVIC_SYSRESETREQ            2

//! CPU id size
#define CPU_ID_SIZE                      12

//! SCB CPUID Register Definitions
#define SCB_CPUID_IMPLEMENTER_POS          24                                             //!  SCB CPUID: IMPLEMENTER Position
#define SCB_CPUID_IMPLEMENTER_MSK          (0xFFUL << SCB_CPUID_IMPLEMENTER_POS)          //!  SCB CPUID: IMPLEMENTER Mask

#define SCB_CPUID_VARIANT_POS              20                                             //!  SCB CPUID: VARIANT Position
#define SCB_CPUID_VARIANT_MSK              (0xFUL << SCB_CPUID_VARIANT_POS)               //!  SCB CPUID: VARIANT Mask

#define SCB_CPUID_ARCHITECTURE_POS         16                                             //!  SCB CPUID: ARCHITECTURE Position
#define SCB_CPUID_ARCHITECTURE_MSK         (0xFUL << SCB_CPUID_ARCHITECTURE_POS)          //!  SCB CPUID: ARCHITECTURE Mask

#define SCB_CPUID_PARTNO_POS                4                                             //!  SCB CPUID: PARTNO Position
#define SCB_CPUID_PARTNO_MSK               (0xFFFUL << SCB_CPUID_PARTNO_POS)              //!  SCB CPUID: PARTNO Mask

#define SCB_CPUID_REVISION_POS              0                                             //!  SCB CPUID: REVISION Position
#define SCB_CPUID_REVISION_MSK             (0xFUL << SCB_CPUID_REVISION_POS)              //!  SCB CPUID: REVISION Mask

//!  SCB Interrupt Control State Register Definitions
#define SCB_ICSR_NMIPENDSET_POS            31                                             //!  SCB ICSR: NMIPENDSET Position
#define SCB_ICSR_NMIPENDSET_MSK            (1UL << SCB_ICSR_NMIPENDSET_POS)               //!  SCB ICSR: NMIPENDSET Mask

#define SCB_ICSR_PENDSVSET_POS             28                                             //!  SCB ICSR: PENDSVSET Position
#define SCB_ICSR_PENDSVSET_MSK             (1UL << SCB_ICSR_PENDSVSET_POS)                //!  SCB ICSR: PENDSVSET Mask

#define SCB_ICSR_PENDSVCLR_POS             27                                             //!  SCB ICSR: PENDSVCLR Position
#define SCB_ICSR_PENDSVCLR_MSK             (1UL << SCB_ICSR_PENDSVCLR_POS)                //!  SCB ICSR: PENDSVCLR Mask

#define SCB_ICSR_PENDSTSET_POS             26                                             //!  SCB ICSR: PENDSTSET Position
#define SCB_ICSR_PENDSTSET_MSK             (1UL << SCB_ICSR_PENDSTSET_POS)                //!  SCB ICSR: PENDSTSET Mask

#define SCB_ICSR_PENDSTCLR_POS             25                                             //!  SCB ICSR: PENDSTCLR Position
#define SCB_ICSR_PENDSTCLR_MSK             (1UL << SCB_ICSR_PENDSTCLR_POS)                //!  SCB ICSR: PENDSTCLR Mask

#define SCB_ICSR_ISRPREEMPT_POS            23                                             //!  SCB ICSR: ISRPREEMPT Position
#define SCB_ICSR_ISRPREEMPT_MSK            (1UL << SCB_ICSR_ISRPREEMPT_POS)               //!  SCB ICSR: ISRPREEMPT Mask

#define SCB_ICSR_ISRPENDING_POS            22                                             //!  SCB ICSR: ISRPENDING Position
#define SCB_ICSR_ISRPENDING_MSK            (1UL << SCB_ICSR_ISRPENDING_POS)               //!  SCB ICSR: ISRPENDING Mask

#define SCB_ICSR_VECTPENDING_POS           12                                             //!  SCB ICSR: VECTPENDING Position
#define SCB_ICSR_VECTPENDING_MSK           (0x1FFUL << SCB_ICSR_VECTPENDING_POS)          //!  SCB ICSR: VECTPENDING Mask

#define SCB_ICSR_RETTOBASE_POS             11                                             //!  SCB ICSR: RETTOBASE Position
#define SCB_ICSR_RETTOBASE_MSK             (1UL << SCB_ICSR_RETTOBASE_POS)                //!  SCB ICSR: RETTOBASE Mask

#define SCB_ICSR_VECTACTIVE_POS             0                                             //!  SCB ICSR: VECTACTIVE Position
#define SCB_ICSR_VECTACTIVE_MSK            (0x1FFUL << SCB_ICSR_VECTACTIVE_POS)           //!  SCB ICSR: VECTACTIVE Mask

//!  SCB Vector Table Offset Register Definitions
#define SCB_VTOR_TBLOFF_POS                 7                                             //!  SCB VTOR: TBLOFF Position
#define SCB_VTOR_TBLOFF_MSK                (0x1FFFFFFUL << SCB_VTOR_TBLOFF_POS)           //!  SCB VTOR: TBLOFF Mask

//!  SCB Application Interrupt and Reset Control Register Definitions
#define SCB_AIRCR_VECTKEY_POS              16                                             //!  SCB AIRCR: VECTKEY Position
#define SCB_AIRCR_VECTKEY_MSK              (0xFFFFUL << SCB_AIRCR_VECTKEY_POS)            //!  SCB AIRCR: VECTKEY Mask

#define SCB_AIRCR_VECTKEYSTAT_POS          16                                             //!  SCB AIRCR: VECTKEYSTAT Position
#define SCB_AIRCR_VECTKEYSTAT_MSK          (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_POS)        //!  SCB AIRCR: VECTKEYSTAT Mask

#define SCB_AIRCR_ENDIANESS_POS            15                                             //!  SCB AIRCR: ENDIANESS Position
#define SCB_AIRCR_ENDIANESS_MSK            (1UL << SCB_AIRCR_ENDIANESS_POS)               //!  SCB AIRCR: ENDIANESS Mask

#define SCB_AIRCR_PRIGROUP_POS              8                                             //!  SCB AIRCR: PRIGROUP Position
#define SCB_AIRCR_PRIGROUP_MSK             (7UL << SCB_AIRCR_PRIGROUP_POS)                //!  SCB AIRCR: PRIGROUP Mask

#define SCB_AIRCR_SYSRESETREQ_POS           2                                             //!  SCB AIRCR: SYSRESETREQ Position
#define SCB_AIRCR_SYSRESETREQ_MSK          (1UL << SCB_AIRCR_SYSRESETREQ_POS)             //!  SCB AIRCR: SYSRESETREQ Mask

#define SCB_AIRCR_VECTCLRACTIVE_POS         1                                             //!  SCB AIRCR: VECTCLRACTIVE Position
#define SCB_AIRCR_VECTCLRACTIVE_MSK        (1UL << SCB_AIRCR_VECTCLRACTIVE_POS)           //!  SCB AIRCR: VECTCLRACTIVE Mask

#define SCB_AIRCR_VECTRESET_POS             0                                             //!  SCB AIRCR: VECTRESET Position
#define SCB_AIRCR_VECTRESET_MSK            (1UL << SCB_AIRCR_VECTRESET_POS)               //!  SCB AIRCR: VECTRESET Mask

//!  SCB System Control Register Definitions
#define SCB_SCR_SEVONPEND_POS               4                                             //!  SCB SCR: SEVONPEND Position
#define SCB_SCR_SEVONPEND_MSK              (1UL << SCB_SCR_SEVONPEND_POS)                 //!  SCB SCR: SEVONPEND Mask

#define SCB_SCR_SLEEPDEEP_POS               2                                             //!  SCB SCR: SLEEPDEEP Position
#define SCB_SCR_SLEEPDEEP_MSK              (1UL << SCB_SCR_SLEEPDEEP_POS)                 //!  SCB SCR: SLEEPDEEP Mask

#define SCB_SCR_SLEEPONEXIT_POS             1                                             //!  SCB SCR: SLEEPONEXIT Position
#define SCB_SCR_SLEEPONEXIT_MSK            (1UL << SCB_SCR_SLEEPONEXIT_POS)               //!  SCB SCR: SLEEPONEXIT Mask

//!  SCB Configuration Control Register Definitions
#define SCB_CCR_STKALIGN_POS                9                                             //!  SCB CCR: STKALIGN Position
#define SCB_CCR_STKALIGN_MSK               (1UL << SCB_CCR_STKALIGN_POS)                  //!  SCB CCR: STKALIGN Mask

#define SCB_CCR_BFHFNMIGN_POS               8                                             //!  SCB CCR: BFHFNMIGN Position
#define SCB_CCR_BFHFNMIGN_MSK              (1UL << SCB_CCR_BFHFNMIGN_POS)                 //!  SCB CCR: BFHFNMIGN Mask

#define SCB_CCR_DIV_0_TRP_POS               4                                             //!  SCB CCR: DIV_0_TRP Position
#define SCB_CCR_DIV_0_TRP_MSK              (1UL << SCB_CCR_DIV_0_TRP_POS)                 //!  SCB CCR: DIV_0_TRP Mask

#define SCB_CCR_UNALIGN_TRP_POS             3                                             //!  SCB CCR: UNALIGN_TRP Position
#define SCB_CCR_UNALIGN_TRP_MSK            (1UL << SCB_CCR_UNALIGN_TRP_POS)               //!  SCB CCR: UNALIGN_TRP Mask

#define SCB_CCR_USERSETMPEND_POS            1                                             //!  SCB CCR: USERSETMPEND Position
#define SCB_CCR_USERSETMPEND_MSK           (1UL << SCB_CCR_USERSETMPEND_POS)              //!  SCB CCR: USERSETMPEND Mask

#define SCB_CCR_NONBASETHRDENA_POS          0                                             //!  SCB CCR: NONBASETHRDENA Position
#define SCB_CCR_NONBASETHRDENA_MSK         (1UL << SCB_CCR_NONBASETHRDENA_POS)            //!  SCB CCR: NONBASETHRDENA Mask

//!  SCB System Handler Control and State Register Definitions
#define SCB_SHCSR_USGFAULTENA_POS          18                                             //!  SCB SHCSR: USGFAULTENA Position
#define SCB_SHCSR_USGFAULTENA_MSK          (1UL << SCB_SHCSR_USGFAULTENA_POS)             //!  SCB SHCSR: USGFAULTENA Mask

#define SCB_SHCSR_BUSFAULTENA_POS          17                                             //!  SCB SHCSR: BUSFAULTENA Position
#define SCB_SHCSR_BUSFAULTENA_MSK          (1UL << SCB_SHCSR_BUSFAULTENA_POS)             //!  SCB SHCSR: BUSFAULTENA Mask

#define SCB_SHCSR_MEMFAULTENA_POS          16                                             //!  SCB SHCSR: MEMFAULTENA Position
#define SCB_SHCSR_MEMFAULTENA_MSK          (1UL << SCB_SHCSR_MEMFAULTENA_POS)             //!  SCB SHCSR: MEMFAULTENA Mask

#define SCB_SHCSR_SVCALLPENDED_POS         15                                             //!  SCB SHCSR: SVCALLPENDED Position
#define SCB_SHCSR_SVCALLPENDED_MSK         (1UL << SCB_SHCSR_SVCALLPENDED_POS)            //!  SCB SHCSR: SVCALLPENDED Mask

#define SCB_SHCSR_BUSFAULTPENDED_POS       14                                             //!  SCB SHCSR: BUSFAULTPENDED Position
#define SCB_SHCSR_BUSFAULTPENDED_MSK       (1UL << SCB_SHCSR_BUSFAULTPENDED_POS)          //!  SCB SHCSR: BUSFAULTPENDED Mask

#define SCB_SHCSR_MEMFAULTPENDED_POS       13                                             //!  SCB SHCSR: MEMFAULTPENDED Position
#define SCB_SHCSR_MEMFAULTPENDED_MSK       (1UL << SCB_SHCSR_MEMFAULTPENDED_POS)          //!  SCB SHCSR: MEMFAULTPENDED Mask

#define SCB_SHCSR_USGFAULTPENDED_POS       12                                             //!  SCB SHCSR: USGFAULTPENDED Position
#define SCB_SHCSR_USGFAULTPENDED_MSK       (1UL << SCB_SHCSR_USGFAULTPENDED_POS)          //!  SCB SHCSR: USGFAULTPENDED Mask

#define SCB_SHCSR_SYSTICKACT_POS           11                                             //!  SCB SHCSR: SYSTICKACT Position
#define SCB_SHCSR_SYSTICKACT_MSK           (1UL << SCB_SHCSR_SYSTICKACT_POS)              //!  SCB SHCSR: SYSTICKACT Mask

#define SCB_SHCSR_PENDSVACT_POS            10                                             //!  SCB SHCSR: PENDSVACT Position
#define SCB_SHCSR_PENDSVACT_MSK            (1UL << SCB_SHCSR_PENDSVACT_POS)               //!  SCB SHCSR: PENDSVACT Mask

#define SCB_SHCSR_MONITORACT_POS            8                                             //!  SCB SHCSR: MONITORACT Position
#define SCB_SHCSR_MONITORACT_MSK           (1UL << SCB_SHCSR_MONITORACT_POS)              //!  SCB SHCSR: MONITORACT Mask

#define SCB_SHCSR_SVCALLACT_POS             7                                             //!  SCB SHCSR: SVCALLACT Position
#define SCB_SHCSR_SVCALLACT_MSK            (1UL << SCB_SHCSR_SVCALLACT_POS)               //!  SCB SHCSR: SVCALLACT Mask

#define SCB_SHCSR_USGFAULTACT_POS           3                                             //!  SCB SHCSR: USGFAULTACT Position
#define SCB_SHCSR_USGFAULTACT_MSK          (1UL << SCB_SHCSR_USGFAULTACT_POS)             //!  SCB SHCSR: USGFAULTACT Mask

#define SCB_SHCSR_BUSFAULTACT_POS           1                                             //!  SCB SHCSR: BUSFAULTACT Position
#define SCB_SHCSR_BUSFAULTACT_MSK          (1UL << SCB_SHCSR_BUSFAULTACT_POS)             //!  SCB SHCSR: BUSFAULTACT Mask

#define SCB_SHCSR_MEMFAULTACT_POS           0                                             //!  SCB SHCSR: MEMFAULTACT Position
#define SCB_SHCSR_MEMFAULTACT_MSK          (1UL << SCB_SHCSR_MEMFAULTACT_POS)             //!  SCB SHCSR: MEMFAULTACT Mask

//!  SCB Configurable Fault Status Registers Definitions
#define SCB_CFSR_USGFAULTSR_POS            16                                             //!  SCB CFSR: Usage Fault Status Register Position
#define SCB_CFSR_USGFAULTSR_MSK            (0xFFFFUL << SCB_CFSR_USGFAULTSR_POS)          //!  SCB CFSR: Usage Fault Status Register Mask

#define SCB_CFSR_BUSFAULTSR_POS             8                                             //!  SCB CFSR: Bus Fault Status Register Position
#define SCB_CFSR_BUSFAULTSR_MSK            (0xFFUL << SCB_CFSR_BUSFAULTSR_POS)            //!  SCB CFSR: Bus Fault Status Register Mask

#define SCB_CFSR_MEMFAULTSR_POS             0                                             //!  SCB CFSR: Memory Manage Fault Status Register Position
#define SCB_CFSR_MEMFAULTSR_MSK            (0xFFUL << SCB_CFSR_MEMFAULTSR_POS)            //!  SCB CFSR: Memory Manage Fault Status Register Mask

//!  SCB Hard Fault Status Registers Definitions
#define SCB_HFSR_DEBUGEVT_POS              31                                             //!  SCB HFSR: DEBUGEVT Position
#define SCB_HFSR_DEBUGEVT_MSK              (1UL << SCB_HFSR_DEBUGEVT_POS)                 //!  SCB HFSR: DEBUGEVT Mask

#define SCB_HFSR_FORCED_POS                30                                             //!  SCB HFSR: FORCED Position
#define SCB_HFSR_FORCED_MSK                (1UL << SCB_HFSR_FORCED_POS)                   //!  SCB HFSR: FORCED Mask

#define SCB_HFSR_VECTTBL_POS                1                                             //!  SCB HFSR: VECTTBL Position
#define SCB_HFSR_VECTTBL_MSK               (1UL << SCB_HFSR_VECTTBL_POS)                  //!  SCB HFSR: VECTTBL Mask

//!  SCB Debug Fault Status Register Definitions
#define SCB_DFSR_EXTERNAL_POS               4                                             //!  SCB DFSR: EXTERNAL Position
#define SCB_DFSR_EXTERNAL_MSK              (1UL << SCB_DFSR_EXTERNAL_POS)                 //!  SCB DFSR: EXTERNAL Mask

#define SCB_DFSR_VCATCH_POS                 3                                             //!  SCB DFSR: VCATCH Position
#define SCB_DFSR_VCATCH_MSK                (1UL << SCB_DFSR_VCATCH_POS)                   //!  SCB DFSR: VCATCH Mask

#define SCB_DFSR_DWTTRAP_POS                2                                             //!  SCB DFSR: DWTTRAP Position
#define SCB_DFSR_DWTTRAP_MSK               (1UL << SCB_DFSR_DWTTRAP_POS)                  //!  SCB DFSR: DWTTRAP Mask

#define SCB_DFSR_BKPT_POS                   1                                             //!  SCB DFSR: BKPT Position
#define SCB_DFSR_BKPT_MSK                  (1UL << SCB_DFSR_BKPT_POS)                     //!  SCB DFSR: BKPT Mask

#define SCB_DFSR_HALTED_POS                 0                                             //!  SCB DFSR: HALTED Position
#define SCB_DFSR_HALTED_MSK                (1UL << SCB_DFSR_HALTED_POS)                   //!  SCB DFSR: HALTED Mask

//----------------------------------------
// IRQ Channels For Cortex M4 MKV3x Family
//----------------------------------------
//! Interrupt Number Definition
typedef enum
{
    //! *****  Cortex-M4 Processor Exceptions Numbers ***************************************************************
    NONMASKABLEINT_IRQ_CHANNEL         = -14,    //!  2 Non Maskable Interrupt
    MEMORYMANAGEMENT_IRQ_CHANNEL       = -12,    //!  4 Cortex-M4 Memory Management Interrupt
    BUSFAULT_IRQ_CHANNEL               = -11,    //!  5 Cortex-M4 Bus Fault Interrupt
    USAGEFAULT_IRQ_CHANNEL             = -10,    //!  6 Cortex-M4 Usage Fault Interrupt
    SVCALL_IRQ_CHANNEL                 = -5,     //!  11 Cortex-M4 SV Call Interrupt
    DEBUGMONITOR_IRQ_CHANNEL           = -4,     //!  12 Cortex-M4 Debug Monitor Interrupt
    PENDSV_IRQ_CHANNEL                 = -2,     //!  14 Cortex-M4 Pend SV Interrupt
    SYSTICK_IRQ_CHANNEL                = -1,     //!  15 Cortex-M4 System Tick Interrupt

    //! *****  Kinetis MKV3x specific Interrupt Numbers *********************************************************************
    DMA0_IRQ_CHANNEL                   = 0,      //!  DMA channel 0 transfer complete
    DMA1_IRQ_CHANNEL                   = 1,      //!  DMA channel 1 transfer complete
    DMA2_IRQ_CHANNEL                   = 2,      //!  DMA channel 2 transfer complete
    DMA3_IRQ_CHANNEL                   = 3,      //!  DMA channel 3 transfer complete
    DMA4_IRQ_CHANNEL                   = 4,      //!  DMA channel 4 transfer complete
    DMA5_IRQ_CHANNEL                   = 5,      //!  DMA channel 5 transfer complete
    DMA6_IRQ_CHANNEL                   = 6,      //!  DMA channel 6 transfer complete
    DMA7_IRQ_CHANNEL                   = 7,      //!  DMA channel 7 transfer complete
    DMA8_IRQ_CHANNEL                   = 8,      //!  DMA channel 8 transfer complete
    DMA9_IRQ_CHANNEL                   = 9,      //!  DMA channel 9 transfer complete
    DMA10_IRQ_CHANNEL                  = 10,     //!  DMA channel 10 transfer complete
    DMA11_IRQ_CHANNEL                  = 11,     //!  DMA Stream 11 global Interrupt
    DMA12_IRQ_CHANNEL                  = 12,     //!  DMA Stream 12 global Interrupt
    DMA13_IRQ_CHANNEL                  = 13,     //!  DMA Stream 13 global Interrupt
    DMA14_IRQ_CHANNEL                  = 14,     //!  DMA Stream 14 global Interrupt
    DMA15_IRQ_CHANNEL                  = 15,     //!  DMA Stream 15 global Interrupt
    DMA_ERROR_IRQ_CHANNEL              = 16,     //!  DMA error interrupt channels 0-15
    MCM_IRQ_CHANNEL                    = 17,     //!  FPU sources
    FLASH_CMD_COMPLETE_IRQ_CHANNEL     = 18,     //!  Command complete
    FLASH_READ_COLLISION_IRQ_CHANNEL   = 19,     //!  Read collision
	LVD_LVW_IRQ_CHANNEL                = 20,     //!  Low-voltage detect, low-voltage warning
	LLWU_IRQ_CHANNEL                   = 21,     //!  Low Leakage Wakeup
	Watchdog_IRQ_CHANNEL               = 22,     //!  Both watchdog modules share this interrupt.
    RNG_IRQ_CHANNEL                    = 23,     //!  Randon Number Generator
	I2C0_IRQ_CHANNEL                   = 24,     //!  I2C0 Event Interrupt
	I2C1_IRQ_CHANNEL                   = 25,     //!  I2C1 Event Interruptf
	SPI0_IRQ_CHANNEL                   = 26,     //!  Single interrupt vector for all sources
	SPI1_IRQ_CHANNEL                   = 27,     //!  Single interrupt vector for all sources
	LPUARTO                            = 30,     //!  Status and error
	UART0_STATUS_IRQ_CHANNEL           = 31,     //!  Single interrupt vector for UART status sources
	UART0_ERROR_IRQ_CHANNEL            = 32,     //!  Single interrupt vector for UART error sources
	UART1_STATUS_IRQ_CHANNEL           = 33,     //!  Single interrupt vector for UART status sources
	UART1_ERROR_IRQ_CHANNEL            = 34,     //!  Single interrupt vector for UART error sources
	UART2_STATUS_IRQ_CHANNEL           = 35,     //!  Single interrupt vector for UART status sources
	UART2_ERROR_IRQ_CHANNEL            = 36,     //!  Single interrupt vector for UART error sources
	ADC0_IRQ_CHANNEL                   = 39,     //!  USART3 global Interrupt
	ACMP0_IRQ_CHANNEL                  = 40,     //!  External Line[15:10] Interrupts
	ACMP1_IRQ_CHANNEL                  = 41,     //!  RTC Alarm (A and B) through EXTI Line Interrupt
	FTM0_IRQ_CHANNEL                   = 42,     //!  FTM0 Single interrupt vector for all sources
	FTM1_IRQ_CHANNEL                   = 43,     //!  FTM1 Single interrupt vector for all sources
	FTM2_IRQ_CHANNEL                   = 44,     //!  FTM2 Single interrupt vector for all sources
	PIT_CH0_IRQ_CHANNEL                = 48,     //!  FSMC global Interrupt
	PIT_CH1_IRQ_CHANNEL                = 49,     //!  SDIO global Interrupt
	PIT_CH2_IRQ_CHANNEL                = 50,     //!  TIM5 global Interrupt
	PIT_CH3_IRQ_CHANNEL                = 51,     //!  SPI3 global Interrupt
	PDB_IRQ_CHANNEL                    = 52,     //!  UART4 global Interrupt
    DAC_IRQ_CHANNEL                    = 56,     //!  UART5 global Interrupt
    MCG_DAC_IRQ_CHANNEL                = 57,     //!  TIM6 global and DAC1&2 underrun error  interrupts
    LOW_POWER_TIMER_IRQ_CHANNEL        = 58,     //!  TIM7 global interrupt
    PORTA_IRQ_CHANNEL                  = 59,     //!  DMA2 Stream 0 global Interrupt
    PORTB_IRQ_CHANNEL                  = 60,     //!  DMA2 Stream 1 global Interrupt
	PORTC_IRQ_CHANNEL                  = 61,     //!  DMA2 Stream 2 global Interrupt
	PORTD_IRQ_CHANNEL                  = 62,     //!  DMA2 Stream 3 global Interrupt
	PORTE_IRQ_CHANNEL                  = 63,     //!  DMA2 Stream 4 global Interrupt
    SOFTWARE_IRQ_CHANNEL               = 64,     //!  Ethernet global Interrupt
    FTM3_IRQ_CHANNEL                   = 71,     //!  USART6 global interrupt
    DAC1_IRQ_CHANNEL                   = 72,     //!  I2C3 event interrupt
    ADC1_IRQ_CHANNEL                   = 73,     //!  I2C3 error interrupt
	INVALID_IRQ_CHANNEL                = 255
} IRQ_CHANNEL_TYPE;

//Interrupt vector table type
typedef void( *interruptfunc )( void );

typedef union
{
    interruptfunc __fun;
    void * __ptr;
} INTERRUPT_TABLE_TYPE;

//-------------------------- Start of Watchdog Configuration register -----------------------------------------------------
/** Peripheral WDOG base address */
#define WDOG_BASE                                (0x40052000u)
#define WDOG                                     (*(WDOG_DEF *)WDOG_BASE)

/** WDOG - Register Layout Typedef */
typedef struct
 {
	volatile uint16 STCTRLH; 					// Watchdog Status and Control Register High, offset: 0x0
	volatile uint16 STCTRLL; 					// Watchdog Status and Control Register Low,  offset: 0x2
	volatile uint16 TOVALH; 					// Watchdog Time-out Value Register High,     offset: 0x4
	volatile uint16 TOVALL; 					// Watchdog Time-out Value Register Low,      offset: 0x6
	volatile uint16 WINH; 						// Watchdog Window Register High,             offset: 0x8
	volatile uint16 WINL; 						// Watchdog Window Register Low,              offset: 0xA
	volatile uint16 REFRESH; 					// Watchdog Refresh register,                 offset: 0xC
	volatile uint16 UNLOCK; 					// Watchdog Unlock register,                  offset: 0xE
	volatile uint16 TMROUTH; 					// Watchdog Timer Output Register High,       offset: 0x10
	volatile uint16 TMROUTL; 					// Watchdog Timer Output Register Low,        offset: 0x12
	volatile uint16 RSTCNT; 					// Watchdog Reset Count register,             offset: 0x14
	volatile uint16 PRESC; 						// Watchdog Prescaler register,               offset: 0x16

} WDOG_DEF;

// WDOG Register Masks

/* STCTRLH Bit Fields */
#define WDOG_STCTRLH_WDOGEN_MASK                 0x1u
#define WDOG_STCTRLH_WDOGEN_SHIFT                0
#define WDOG_STCTRLH_CLKSRC_MASK                 0x2u
#define WDOG_STCTRLH_CLKSRC_SHIFT                1
#define WDOG_STCTRLH_IRQRSTEN_MASK               0x4u
#define WDOG_STCTRLH_IRQRSTEN_SHIFT              2
#define WDOG_STCTRLH_WINEN_MASK                  0x8u
#define WDOG_STCTRLH_WINEN_SHIFT                 3
#define WDOG_STCTRLH_ALLOWUPDATE_MASK            0x10u
#define WDOG_STCTRLH_ALLOWUPDATE_SHIFT           4
#define WDOG_STCTRLH_DBGEN_MASK                  0x20u
#define WDOG_STCTRLH_DBGEN_SHIFT                 5
#define WDOG_STCTRLH_STOPEN_MASK                 0x40u
#define WDOG_STCTRLH_STOPEN_SHIFT                6
#define WDOG_STCTRLH_WAITEN_MASK                 0x80u
#define WDOG_STCTRLH_WAITEN_SHIFT                7
#define WDOG_STCTRLH_TESTWDOG_MASK               0x400u
#define WDOG_STCTRLH_TESTWDOG_SHIFT              10
#define WDOG_STCTRLH_TESTSEL_MASK                0x800u
#define WDOG_STCTRLH_TESTSEL_SHIFT               11
#define WDOG_STCTRLH_BYTESEL_MASK                0x3000u
#define WDOG_STCTRLH_BYTESEL_SHIFT               12
#define WDOG_STCTRLH_BYTESEL(x)                  (((uint16)(((uint16)(x))<<WDOG_STCTRLH_BYTESEL_SHIFT))&WDOG_STCTRLH_BYTESEL_MASK)
#define WDOG_STCTRLH_DISTESTWDOG_MASK            0x4000u
#define WDOG_STCTRLH_DISTESTWDOG_SHIFT           14
/* STCTRLL Bit Fields */
#define WDOG_STCTRLL_INTFLG_MASK                 0x8000u
#define WDOG_STCTRLL_INTFLG_SHIFT                15
/* TOVALH Bit Fields */
#define WDOG_TOVALH_TOVALHIGH_MASK               0xFFFFu
#define WDOG_TOVALH_TOVALHIGH_SHIFT              0
#define WDOG_TOVALH_TOVALHIGH(x)                 (((uint16)(((uint16)(x))<<WDOG_TOVALH_TOVALHIGH_SHIFT))&WDOG_TOVALH_TOVALHIGH_MASK)
/* TOVALL Bit Fields */
#define WDOG_TOVALL_TOVALLOW_MASK                0xFFFFu
#define WDOG_TOVALL_TOVALLOW_SHIFT               0
#define WDOG_TOVALL_TOVALLOW(x)                  (((uint16)(((uint16)(x))<<WDOG_TOVALL_TOVALLOW_SHIFT))&WDOG_TOVALL_TOVALLOW_MASK)
/* WINH Bit Fields */
#define WDOG_WINH_WINHIGH_MASK                   0xFFFFu
#define WDOG_WINH_WINHIGH_SHIFT                  0
#define WDOG_WINH_WINHIGH(x)                     (((uint16)(((uint16)(x))<<WDOG_WINH_WINHIGH_SHIFT))&WDOG_WINH_WINHIGH_MASK)
/* WINL Bit Fields */
#define WDOG_WINL_WINLOW_MASK                    0xFFFFu
#define WDOG_WINL_WINLOW_SHIFT                   0
#define WDOG_WINL_WINLOW(x)                      (((uint16)(((uint16)(x))<<WDOG_WINL_WINLOW_SHIFT))&WDOG_WINL_WINLOW_MASK)
/* REFRESH Bit Fields */
#define WDOG_REFRESH_WDOGREFRESH_MASK            0xFFFFu
#define WDOG_REFRESH_WDOGREFRESH_SHIFT           0
#define WDOG_REFRESH_WDOGREFRESH(x)              (((uint16)(((uint16)(x))<<WDOG_REFRESH_WDOGREFRESH_SHIFT))&WDOG_REFRESH_WDOGREFRESH_MASK)
/* UNLOCK Bit Fields */
#define WDOG_UNLOCK_WDOGUNLOCK_MASK              0xFFFFu
#define WDOG_UNLOCK_WDOGUNLOCK_SHIFT             0
#define WDOG_UNLOCK_WDOGUNLOCK(x)                (((uint16)(((uint16)(x))<<WDOG_UNLOCK_WDOGUNLOCK_SHIFT))&WDOG_UNLOCK_WDOGUNLOCK_MASK)
/* TMROUTH Bit Fields */
#define WDOG_TMROUTH_TIMEROUTHIGH_MASK           0xFFFFu
#define WDOG_TMROUTH_TIMEROUTHIGH_SHIFT          0
#define WDOG_TMROUTH_TIMEROUTHIGH(x)             (((uint16)(((uint16)(x))<<WDOG_TMROUTH_TIMEROUTHIGH_SHIFT))&WDOG_TMROUTH_TIMEROUTHIGH_MASK)
/* TMROUTL Bit Fields */
#define WDOG_TMROUTL_TIMEROUTLOW_MASK            0xFFFFu
#define WDOG_TMROUTL_TIMEROUTLOW_SHIFT           0
#define WDOG_TMROUTL_TIMEROUTLOW(x)              (((uint16)(((uint16)(x))<<WDOG_TMROUTL_TIMEROUTLOW_SHIFT))&WDOG_TMROUTL_TIMEROUTLOW_MASK)
/* RSTCNT Bit Fields */
#define WDOG_RSTCNT_RSTCNT_MASK                  0xFFFFu
#define WDOG_RSTCNT_RSTCNT_SHIFT                 0
#define WDOG_RSTCNT_RSTCNT(x)                    (((uint16)(((uint16)(x))<<WDOG_RSTCNT_RSTCNT_SHIFT))&WDOG_RSTCNT_RSTCNT_MASK)
/* PRESC Bit Fields */
#define WDOG_PRESC_PRESCVAL_MASK                 0x700u
#define WDOG_PRESC_PRESCVAL_SHIFT                8
#define WDOG_PRESC_PRESCVAL(x)                   (((uint16)(((uint16)(x))<<WDOG_PRESC_PRESCVAL_SHIFT))&WDOG_PRESC_PRESCVAL_MASK)

//end of group WDOG_Register_Masks

//-------------------------- Start of PORT Configuration register -----------------------------------------------------

/* PORT - Peripheral instance base addresses */
/** Peripheral PORTA base pointer */
#define PORTA_BASE                              (0x40049000u)
#define PCIPORTA                                (*(PORTx_DEF *)PORTA_BASE)
/** Peripheral PORTB base pointer */
#define PORTB_BASE                              (0x4004A000u)
#define PCIPORTB                                (*(PORTx_DEF *)PORTB_BASE)
/** Peripheral PORTC base pointer */
#define PORTC_BASE                              (0x4004B000u)
#define PCIPORTC                                (*(PORTx_DEF *)PORTC_BASE)
/** Peripheral PORTD base pointer */
#define PORTD_BASE                              (0x4004C000u)
#define PCIPORTD                                (*(PORTx_DEF *)PORTD_BASE)
/** Peripheral PORTE base pointer */
#define PORTE_BASE                              (0x4004D000u)
#define PCIPORTE                                (*(PORTx_DEF *)PORTE_BASE)


#define MAX_PIN 								32
#define PORT_DATA_TYPE  						uint32              // This define is mandatory, it defines the register's type

/** PORT - Peripheral register structure */
typedef struct PORT_MemMap
{
  volatile uint32 PCR[32];                                /**< Pin Control Register n, array offset: 0x0, array step: 0x4 */
  volatile uint32 GPCLR;                                  /**< Global Pin Control Low Register, offset: 0x80 */
  volatile uint32 GPCHR;                                  /**< Global Pin Control High Register, offset: 0x84 */
  volatile uint8  RESERVED_0[24];
  volatile uint32 ISFR;                                   /**< Interrupt Status Flag Register, offset: 0xA0 */
  volatile uint8  RESERVED_1[28];
  volatile uint32 DFER;                                   /**< Digital Filter Enable Register, offset: 0xC0 */
  volatile uint32 DFCR;                                   /**< Digital Filter Clock Register, offset: 0xC4 */
  volatile uint32 DFWR;                                   /**< Digital Filter Width Register, offset: 0xC8 */
} volatile PORTx_DEF;

//PORT Register Masks

/* PCR Bit Fields */
#define PORT_PCR_PS_MASK                         0x1u
#define PORT_PCR_PS_SHIFT                        0
#define PORT_PCR_PE_MASK                         0x2u
#define PORT_PCR_PE_SHIFT                        1
#define PORT_PCR_SRE_MASK                        0x4u
#define PORT_PCR_SRE_SHIFT                       2
#define PORT_PCR_PFE_MASK                        0x10u
#define PORT_PCR_PFE_SHIFT                       4
#define PORT_PCR_ODE_MASK                        0x20u
#define PORT_PCR_ODE_SHIFT                       5
#define PORT_PCR_DSE_MASK                        0x40u
#define PORT_PCR_DSE_SHIFT                       6
#define PORT_PCR_MUX_MASK                        0x700u
#define PORT_PCR_MUX_SHIFT                       8
#define PORT_PCR_MUX(x)                          (((uint32)(((uint32)(x))<<PORT_PCR_MUX_SHIFT))&PORT_PCR_MUX_MASK)
#define PORT_PCR_LK_MASK                         0x8000u
#define PORT_PCR_LK_SHIFT                        15
#define PORT_PCR_IRQC_MASK                       0xF0000u
#define PORT_PCR_IRQC_SHIFT                      16
#define PORT_PCR_IRQC(x)                         (((uint32)(((uint32)(x))<<PORT_PCR_IRQC_SHIFT))&PORT_PCR_IRQC_MASK)
#define PORT_PCR_ISF_MASK                        0x1000000u
#define PORT_PCR_ISF_SHIFT                       24
/* GPCLR Bit Fields */
#define PORT_GPCLR_GPWD_MASK                     0xFFFFu
#define PORT_GPCLR_GPWD_SHIFT                    0
#define PORT_GPCLR_GPWD(x)                       (((uint32)(((uint32)(x))<<PORT_GPCLR_GPWD_SHIFT))&PORT_GPCLR_GPWD_MASK)
#define PORT_GPCLR_GPWE_MASK                     0xFFFF0000u
#define PORT_GPCLR_GPWE_SHIFT                    16
#define PORT_GPCLR_GPWE(x)                       (((uint32)(((uint32)(x))<<PORT_GPCLR_GPWE_SHIFT))&PORT_GPCLR_GPWE_MASK)
/* GPCHR Bit Fields */
#define PORT_GPCHR_GPWD_MASK                     0xFFFFu
#define PORT_GPCHR_GPWD_SHIFT                    0
#define PORT_GPCHR_GPWD(x)                       (((uint32)(((uint32)(x))<<PORT_GPCHR_GPWD_SHIFT))&PORT_GPCHR_GPWD_MASK)
#define PORT_GPCHR_GPWE_MASK                     0xFFFF0000u
#define PORT_GPCHR_GPWE_SHIFT                    16
#define PORT_GPCHR_GPWE(x)                       (((uint32)(((uint32)(x))<<PORT_GPCHR_GPWE_SHIFT))&PORT_GPCHR_GPWE_MASK)
/* ISFR Bit Fields */
#define PORT_ISFR_ISF_MASK                       0xFFFFFFFFu
#define PORT_ISFR_ISF_SHIFT                      0
#define PORT_ISFR_ISF(x)                         (((uint32)(((uint32)(x))<<PORT_ISFR_ISF_SHIFT))&PORT_ISFR_ISF_MASK)
/* DFER Bit Fields */
#define PORT_DFER_DFE_MASK                       0xFFFFFFFFu
#define PORT_DFER_DFE_SHIFT                      0
#define PORT_DFER_DFE(x)                         (((uint32)(((uint32)(x))<<PORT_DFER_DFE_SHIFT))&PORT_DFER_DFE_MASK)
/* DFCR Bit Fields */
#define PORT_DFCR_CS_MASK                        0x1u
#define PORT_DFCR_CS_SHIFT                       0
/* DFWR Bit Fields */
#define PORT_DFWR_FILT_MASK                      0x1Fu
#define PORT_DFWR_FILT_SHIFT                     0
#define PORT_DFWR_FILT(x)                        (((uint32)(((uint32)(x))<<PORT_DFWR_FILT_SHIFT))&PORT_DFWR_FILT_MASK)


#define EXTERNAL_INTERRUPT_NUM					MAX_PIN

//-------------------------- Start of GPIO Configuration register -----------------------------------------------------
/* PORT - Peripheral instance base addresses */
/** Peripheral PORTA base pointer */
#define GPIOA_BASE                           (0x400FF000u)
#define GPIOA                                (*(GPIO_DEF *)GPIOA_BASE)
/** Peripheral PORTB base pointer */
#define GPIOB_BASE                           (0x400FF040u)
#define GPIOB                                (*(GPIO_DEF *)GPIOB_BASE)
/** Peripheral PORTC base pointer */
#define GPIOC_BASE                           (0x400FF080u)
#define GPIOC                                (*(GPIO_DEF *)GPIOC_BASE)
/** Peripheral PORTD base pointer */
#define GPIOD_BASE                           (0x400FF0C0u)
#define GPIOD                                (*(GPIO_DEF *)GPIOD_BASE)
/** Peripheral PORTE base pointer */
#define GPIOE_BASE                           (0x400FF100u)
#define GPIOE                                (*(GPIO_DEF *)GPIOE_BASE)


/** GPIO - Peripheral register structure */
typedef struct GPIO_MemMap
{
  uint32 PDOR;                                   //Port Data Output Register,   offset: 0x0
  uint32 PSOR;                                   //Port Set Output Register,    offset: 0x4
  uint32 PCOR;                                   //Port Clear Output Register,  offset: 0x8
  uint32 PTOR;                                   //Port Toggle Output Register, offset: 0xC
  uint32 PDIR;                                   //Port Data Input Register,    offset: 0x10
  uint32 PDDR;                                   //Port Data Direction Register,offset: 0x14
} volatile GPIO_DEF;

/* ----------------------------------------------------------------------------
   -- GPIO - Register accessor macros
   ---------------------------------------------------------------------------- */
/* GPIO - Register accessors */
#define GPIO_PDOR_REG(base)                      ((base)->PDOR)
#define GPIO_PSOR_REG(base)                      ((base)->PSOR)
#define GPIO_PCOR_REG(base)                      ((base)->PCOR)
#define GPIO_PTOR_REG(base)                      ((base)->PTOR)
#define GPIO_PDIR_REG(base)                      ((base)->PDIR)
#define GPIO_PDDR_REG(base)                      ((base)->PDDR)

//GPIO Register Masks

/* PDOR Bit Fields */
#define GPIO_PDOR_PDO_MASK                       0xFFFFFFFFu
#define GPIO_PDOR_PDO_SHIFT                      0
#define GPIO_PDOR_PDO(x)                         (((uint32)(((uint32)(x))<<GPIO_PDOR_PDO_SHIFT))&GPIO_PDOR_PDO_MASK)
/* PSOR Bit Fields */
#define GPIO_PSOR_PTSO_MASK                      0xFFFFFFFFu
#define GPIO_PSOR_PTSO_SHIFT                     0
#define GPIO_PSOR_PTSO(x)                        (((uint32)(((uint32)(x))<<GPIO_PSOR_PTSO_SHIFT))&GPIO_PSOR_PTSO_MASK)
/* PCOR Bit Fields */
#define GPIO_PCOR_PTCO_MASK                      0xFFFFFFFFu
#define GPIO_PCOR_PTCO_SHIFT                     0
#define GPIO_PCOR_PTCO(x)                        (((uint32)(((uint32)(x))<<GPIO_PCOR_PTCO_SHIFT))&GPIO_PCOR_PTCO_MASK)
/* PTOR Bit Fields */
#define GPIO_PTOR_PTTO_MASK                      0xFFFFFFFFu
#define GPIO_PTOR_PTTO_SHIFT                     0
#define GPIO_PTOR_PTTO(x)                        (((uint32)(((uint32)(x))<<GPIO_PTOR_PTTO_SHIFT))&GPIO_PTOR_PTTO_MASK)
/* PDIR Bit Fields */
#define GPIO_PDIR_PDI_MASK                       0xFFFFFFFFu
#define GPIO_PDIR_PDI_SHIFT                      0
#define GPIO_PDIR_PDI(x)                         (((uint32)(((uint32)(x))<<GPIO_PDIR_PDI_SHIFT))&GPIO_PDIR_PDI_MASK)
/* PDDR Bit Fields */
#define GPIO_PDDR_PDD_MASK                       0xFFFFFFFFu
#define GPIO_PDDR_PDD_SHIFT                      0
#define GPIO_PDDR_PDD(x)                         (((uint32)(((uint32)(x))<<GPIO_PDDR_PDD_SHIFT))&GPIO_PDDR_PDD_MASK)

/* GPIO - Peripheral instance base addresses */
/** Peripheral PTA base pointer */
#define PTA_BASE_PTR                             ((GPIO_MemMapPtr)0x400FF000u)
/** Peripheral PTB base pointer */
#define PTB_BASE_PTR                             ((GPIO_MemMapPtr)0x400FF040u)
/** Peripheral PTC base pointer */
#define PTC_BASE_PTR                             ((GPIO_MemMapPtr)0x400FF080u)
/** Peripheral PTD base pointer */
#define PTD_BASE_PTR                             ((GPIO_MemMapPtr)0x400FF0C0u)
/** Peripheral PTE base pointer */
#define PTE_BASE_PTR                             ((GPIO_MemMapPtr)0x400FF100u)
/** Array initializer of GPIO peripheral base pointers */
#define GPIO_BASE_PTRS                           { PTA_BASE_PTR, PTB_BASE_PTR, PTC_BASE_PTR, PTD_BASE_PTR, PTE_BASE_PTR }

//GPIO - Register accessor macros
/* PTA */
#define GPIOA_PDOR                               GPIO_PDOR_REG(PTA_BASE_PTR)
#define GPIOA_PSOR                               GPIO_PSOR_REG(PTA_BASE_PTR)
#define GPIOA_PCOR                               GPIO_PCOR_REG(PTA_BASE_PTR)
#define GPIOA_PTOR                               GPIO_PTOR_REG(PTA_BASE_PTR)
#define GPIOA_PDIR                               GPIO_PDIR_REG(PTA_BASE_PTR)
#define GPIOA_PDDR                               GPIO_PDDR_REG(PTA_BASE_PTR)
/* PTB */
#define GPIOB_PDOR                               GPIO_PDOR_REG(PTB_BASE_PTR)
#define GPIOB_PSOR                               GPIO_PSOR_REG(PTB_BASE_PTR)
#define GPIOB_PCOR                               GPIO_PCOR_REG(PTB_BASE_PTR)
#define GPIOB_PTOR                               GPIO_PTOR_REG(PTB_BASE_PTR)
#define GPIOB_PDIR                               GPIO_PDIR_REG(PTB_BASE_PTR)
#define GPIOB_PDDR                               GPIO_PDDR_REG(PTB_BASE_PTR)
/* PTC */
#define GPIOC_PDOR                               GPIO_PDOR_REG(PTC_BASE_PTR)
#define GPIOC_PSOR                               GPIO_PSOR_REG(PTC_BASE_PTR)
#define GPIOC_PCOR                               GPIO_PCOR_REG(PTC_BASE_PTR)
#define GPIOC_PTOR                               GPIO_PTOR_REG(PTC_BASE_PTR)
#define GPIOC_PDIR                               GPIO_PDIR_REG(PTC_BASE_PTR)
#define GPIOC_PDDR                               GPIO_PDDR_REG(PTC_BASE_PTR)
/* PTD */
#define GPIOD_PDOR                               GPIO_PDOR_REG(PTD_BASE_PTR)
#define GPIOD_PSOR                               GPIO_PSOR_REG(PTD_BASE_PTR)
#define GPIOD_PCOR                               GPIO_PCOR_REG(PTD_BASE_PTR)
#define GPIOD_PTOR                               GPIO_PTOR_REG(PTD_BASE_PTR)
#define GPIOD_PDIR                               GPIO_PDIR_REG(PTD_BASE_PTR)
#define GPIOD_PDDR                               GPIO_PDDR_REG(PTD_BASE_PTR)
/* PTE */
#define GPIOE_PDOR                               GPIO_PDOR_REG(PTE_BASE_PTR)
#define GPIOE_PSOR                               GPIO_PSOR_REG(PTE_BASE_PTR)
#define GPIOE_PCOR                               GPIO_PCOR_REG(PTE_BASE_PTR)
#define GPIOE_PTOR                               GPIO_PTOR_REG(PTE_BASE_PTR)
#define GPIOE_PDIR                               GPIO_PDIR_REG(PTE_BASE_PTR)
#define GPIOE_PDDR                               GPIO_PDDR_REG(PTE_BASE_PTR)

//-------------------------- End of GPIO Configuration register -----------------------------------------------------

//============================================================
/*
 * For testing of clock and board alive purpose macros are added
 */

/*! USER LED definitions */
/*! LED maps
 * code		    KE06
 * RED   <----> PTG5
 * GREEN <----> PTG6
 * BLUE  <----> PTG7
 *
 */
#define LED0_Init()				GPIOB.PDDR |= (1<<21)    /*! RED Light */
#define LED0_Toggle()		    GPIOB.PTOR = (1<<21)
#define LED0_On()		        GPIOB.PCOR = (1<<21)
#define LED0_Off()		        GPIOB.PSOR = (1<<21)

#define LED1_Init()				GPIOB.PDDR |= (1<<22)    /*! Green */
#define LED1_Toggle()		    GPIOB.PTOR = (1<<22)
#define LED1_On()		        GPIOB.PCOR = (1<<22)
#define LED1_Off()		        GPIOB.PSOR = (1<<22)

#define LED2_Init()				GPIOB.PDDR |= (0x01<<23)  /*! Blue */
#define LED2_Toggle()		    GPIOB.PTOR = (0x01<<23)
#define LED2_On()		        GPIOB.PCOR = (0x01<<23)
#define LED2_Off()		        GPIOB.PSOR = (0x01<<23)

//Pin Toggel
#define CLK_PIN_PTF6_INIT()			GPIOB.PDDR |= (1<<14)    /*! Out pin to check clock signal for 250us */
#define CLK_PIN_PTF6_Toggle()		GPIOB.PTOR = (1<<14)
#define CLK_PIN_PTF6_HIGH()		    GPIOB.PCOR = (1<<14)
#define CLK_PIN_PTF6_LOW()		    GPIOB.PSOR = (1<<14)
//============================================================

//-------------------------- Start of RCM (Reset Control Module) Configuration register -----------------------------------------------------

#define RCM_BASE				((uint32)0x4007F000u)

#define RCM						(*(RCM_DEF *)RCM_BASE)

/** RCM - Peripheral register structure */
typedef struct
{
  volatile uint8 SRS0;                                    //System Reset Status Register 0,        offset: 0x0
  volatile uint8 SRS1;                                    //System Reset Status Register 1,        offset: 0x1
  volatile uint8 RESERVED_0[2];
  volatile uint8 RPFC;                                    //Reset Pin Filter Control register,     offset: 0x4
  volatile uint8 RPFW;                                    //Reset Pin Filter Width register,       offset: 0x5
  volatile uint8 RESERVED_1[1];
  volatile uint8 MR;                                      //Mode Register,                         offset: 0x7
  volatile uint8 SSRS0;                                   //Sticky System Reset Status Register 0, offset: 0x8
  volatile uint8 SSRS1;                                   //Sticky System Reset Status Register 1, offset: 0x9
} RCM_DEF;

/* SRS0 Bit Fields */
#define RCM_SRS0_WAKEUP_MASK                     0x1u
#define RCM_SRS0_WAKEUP_SHIFT                    0
#define RCM_SRS0_LVD_MASK                        0x2u
#define RCM_SRS0_LVD_SHIFT                       1
#define RCM_SRS0_LOC_MASK                        0x4u
#define RCM_SRS0_LOC_SHIFT                       2
#define RCM_SRS0_LOL_MASK                        0x8u
#define RCM_SRS0_LOL_SHIFT                       3
#define RCM_SRS0_WDOG_MASK                       0x20u
#define RCM_SRS0_WDOG_SHIFT                      5
#define RCM_SRS0_PIN_MASK                        0x40u
#define RCM_SRS0_PIN_SHIFT                       6
#define RCM_SRS0_POR_MASK                        0x80u
#define RCM_SRS0_POR_SHIFT                       7
/* SRS1 Bit Fields */
#define RCM_SRS1_JTAG_MASK                       0x1u
#define RCM_SRS1_JTAG_SHIFT                      0
#define RCM_SRS1_LOCKUP_MASK                     0x2u
#define RCM_SRS1_LOCKUP_SHIFT                    1
#define RCM_SRS1_SW_MASK                         0x4u
#define RCM_SRS1_SW_SHIFT                        2
#define RCM_SRS1_MDM_AP_MASK                     0x8u
#define RCM_SRS1_MDM_AP_SHIFT                    3
#define RCM_SRS1_EZPT_MASK                       0x10u
#define RCM_SRS1_EZPT_SHIFT                      4
#define RCM_SRS1_SACKERR_MASK                    0x20u
#define RCM_SRS1_SACKERR_SHIFT                   5
/* RPFC Bit Fields */
#define RCM_RPFC_RSTFLTSRW_MASK                  0x3u
#define RCM_RPFC_RSTFLTSRW_SHIFT                 0
#define RCM_RPFC_RSTFLTSRW(x)                    (((uint8)(((uint8)(x))<<RCM_RPFC_RSTFLTSRW_SHIFT))&RCM_RPFC_RSTFLTSRW_MASK)
#define RCM_RPFC_RSTFLTSS_MASK                   0x4u
#define RCM_RPFC_RSTFLTSS_SHIFT                  2
/* RPFW Bit Fields */
#define RCM_RPFW_RSTFLTSEL_MASK                  0x1Fu
#define RCM_RPFW_RSTFLTSEL_SHIFT                 0
#define RCM_RPFW_RSTFLTSEL(x)                    (((uint8)(((uint8)(x))<<RCM_RPFW_RSTFLTSEL_SHIFT))&RCM_RPFW_RSTFLTSEL_MASK)
/* MR Bit Fields */
#define RCM_MR_EZP_MS_MASK                       0x2u
#define RCM_MR_EZP_MS_SHIFT                      1
/* SSRS0 Bit Fields */
#define RCM_SSRS0_SWAKEUP_MASK                   0x1u
#define RCM_SSRS0_SWAKEUP_SHIFT                  0
#define RCM_SSRS0_SLVD_MASK                      0x2u
#define RCM_SSRS0_SLVD_SHIFT                     1
#define RCM_SSRS0_SLOC_MASK                      0x4u
#define RCM_SSRS0_SLOC_SHIFT                     2
#define RCM_SSRS0_SLOL_MASK                      0x8u
#define RCM_SSRS0_SLOL_SHIFT                     3
#define RCM_SSRS0_SWDOG_MASK                     0x20u
#define RCM_SSRS0_SWDOG_SHIFT                    5
#define RCM_SSRS0_SPIN_MASK                      0x40u
#define RCM_SSRS0_SPIN_SHIFT                     6
#define RCM_SSRS0_SPOR_MASK                      0x80u
#define RCM_SSRS0_SPOR_SHIFT                     7
/* SSRS1 Bit Fields */
#define RCM_SSRS1_SJTAG_MASK                     0x1u
#define RCM_SSRS1_SJTAG_SHIFT                    0
#define RCM_SSRS1_SLOCKUP_MASK                   0x2u
#define RCM_SSRS1_SLOCKUP_SHIFT                  1
#define RCM_SSRS1_SSW_MASK                       0x4u
#define RCM_SSRS1_SSW_SHIFT                      2
#define RCM_SSRS1_SMDM_AP_MASK                   0x8u
#define RCM_SSRS1_SMDM_AP_SHIFT                  3
#define RCM_SSRS1_SEZPT_MASK                     0x10u
#define RCM_SSRS1_SEZPT_SHIFT                    4
#define RCM_SSRS1_SSACKERR_MASK                  0x20u
#define RCM_SSRS1_SSACKERR_SHIFT                 5

//-------------------------- End of RCM Configuration register -----------------------------------------------------

//-------------------------- Start of SIM System Integration Module Register Configuration -------------------------

#define SIM_BASE              ((uint32)0x40047000)

#define SIM                   (*(SIM_DEF *)SIM_BASE)

typedef struct
{
	  volatile uint32 SOPT1;                                  //System Options Register 1,              offset: 0x0
	  volatile uint32 SOPT1CFG;                               //SOPT1 Configuration Register,           offset: 0x4
	  volatile uint8  RESERVED_0[4092];
	  volatile uint32 SOPT2;                                  //System Options Register 2,              offset: 0x1004
	  volatile uint8  RESERVED_1[4];
	  volatile uint32 SOPT4;                                  //System Options Register 4,              offset: 0x100C
	  volatile uint32 SOPT5;                                  //System Options Register 5,              offset: 0x1010
	  volatile uint8  RESERVED_2[4];
	  volatile uint32 SOPT7;                                  //System Options Register 7,              offset: 0x1018
	  volatile uint32 SOPT8;                                  //System Options Register 8,              offset: 0x101C
	  volatile uint8  RESERVED_3[4];
	  volatile uint32 SDID;                                   //System Device Identification Register,  offset: 0x1024
	  volatile uint8  RESERVED_4[12];
	  volatile uint32 SCGC4;                                  //System Clock Gating Control Register 4, offset: 0x1034
	  volatile uint32 SCGC5;                                  //System Clock Gating Control Register 5, offset: 0x1038
	  volatile uint32 SCGC6;                                  //System Clock Gating Control Register 6, offset: 0x103C
	  volatile uint32 SCGC7;                                  //System Clock Gating Control Register 7, offset: 0x1040
	  volatile uint32 CLKDIV1;                                //System Clock Divider Register 1,        offset: 0x1044
	  volatile uint8  RESERVED_5[4];
	  volatile uint32 FCFG1;                                  //Flash Configuration Register 1,         offset: 0x104C
	  volatile uint32 FCFG2;                                  //Flash Configuration Register 2,         offset: 0x1050
	  volatile uint32 UIDH;                                   //Unique Identification Register High,    offset: 0x1054
	  volatile uint32 UIDMH;                                  //Unique Identification Register Mid-High,offset: 0x1058
	  volatile uint32 UIDML;                                  //Unique Identification Register Mid Low, offset: 0x105C
	  volatile uint32 UIDL;                                   //Unique Identification Register Low,     offset: 0x1060

}SIM_DEF;


/* ----------------------------------------------------------------------------
   -- SIM Register Masks
   ---------------------------------------------------------------------------- */

/* SOPT1 Bit Fields */
#define SIM_SOPT1_RAMSIZE_MASK                   0xF000u
#define SIM_SOPT1_RAMSIZE_SHIFT                  12
#define SIM_SOPT1_RAMSIZE(x)                     (((uint32)(((uint32)(x))<<SIM_SOPT1_RAMSIZE_SHIFT))&SIM_SOPT1_RAMSIZE_MASK)
#define SIM_SOPT1_OSC32KOUT_MASK                 0x30000u
#define SIM_SOPT1_OSC32KOUT_SHIFT                16
#define SIM_SOPT1_OSC32KOUT(x)                   (((uint32)(((uint32)(x))<<SIM_SOPT1_OSC32KOUT_SHIFT))&SIM_SOPT1_OSC32KOUT_MASK)
#define SIM_SOPT1_OSC32KSEL_MASK                 0xC0000u
#define SIM_SOPT1_OSC32KSEL_SHIFT                18
#define SIM_SOPT1_OSC32KSEL(x)                   (((uint32)(((uint32)(x))<<SIM_SOPT1_OSC32KSEL_SHIFT))&SIM_SOPT1_OSC32KSEL_MASK)
/* SOPT2 Bit Fields */
#define SIM_SOPT2_CLKOUTSEL_MASK                 0xE0u
#define SIM_SOPT2_CLKOUTSEL_SHIFT                5
#define SIM_SOPT2_CLKOUTSEL(x)                   (((uint32)(((uint32)(x))<<SIM_SOPT2_CLKOUTSEL_SHIFT))&SIM_SOPT2_CLKOUTSEL_MASK)
#define SIM_SOPT2_FBSL_MASK                      0x300u
#define SIM_SOPT2_FBSL_SHIFT                     8
#define SIM_SOPT2_FBSL(x)                        (((uint32)(((uint32)(x))<<SIM_SOPT2_FBSL_SHIFT))&SIM_SOPT2_FBSL_MASK)
#define SIM_SOPT2_TRACECLKSEL_MASK               0x1000u
#define SIM_SOPT2_TRACECLKSEL_SHIFT              12
#define SIM_SOPT2_PLLFLLSEL_MASK                 0x30000u
#define SIM_SOPT2_PLLFLLSEL_SHIFT                16
#define SIM_SOPT2_PLLFLLSEL(x)                   (((uint32)(((uint32)(x))<<SIM_SOPT2_PLLFLLSEL_SHIFT))&SIM_SOPT2_PLLFLLSEL_MASK)
#define SIM_SOPT2_LPUARTSRC_MASK                 0xC000000u
#define SIM_SOPT2_LPUARTSRC_SHIFT                26
#define SIM_SOPT2_LPUARTSRC(x)                   (((uint32)(((uint32)(x))<<SIM_SOPT2_LPUARTSRC_SHIFT))&SIM_SOPT2_LPUARTSRC_MASK)
/* SOPT4 Bit Fields */
#define SIM_SOPT4_FTM0FLT0_MASK                  0x1u
#define SIM_SOPT4_FTM0FLT0_SHIFT                 0
#define SIM_SOPT4_FTM0FLT1_MASK                  0x2u
#define SIM_SOPT4_FTM0FLT1_SHIFT                 1
#define SIM_SOPT4_FTM1FLT0_MASK                  0x10u
#define SIM_SOPT4_FTM1FLT0_SHIFT                 4
#define SIM_SOPT4_FTM2FLT0_MASK                  0x100u
#define SIM_SOPT4_FTM2FLT0_SHIFT                 8
#define SIM_SOPT4_FTM3FLT0_MASK                  0x1000u
#define SIM_SOPT4_FTM3FLT0_SHIFT                 12
#define SIM_SOPT4_FTM1CH0SRC_MASK                0xC0000u
#define SIM_SOPT4_FTM1CH0SRC_SHIFT               18
#define SIM_SOPT4_FTM1CH0SRC(x)                  (((uint32)(((uint32)(x))<<SIM_SOPT4_FTM1CH0SRC_SHIFT))&SIM_SOPT4_FTM1CH0SRC_MASK)
#define SIM_SOPT4_FTM2CH0SRC_MASK                0x300000u
#define SIM_SOPT4_FTM2CH0SRC_SHIFT               20
#define SIM_SOPT4_FTM2CH0SRC(x)                  (((uint32)(((uint32)(x))<<SIM_SOPT4_FTM2CH0SRC_SHIFT))&SIM_SOPT4_FTM2CH0SRC_MASK)
#define SIM_SOPT4_FTM2CH1SRC_MASK                0x400000u
#define SIM_SOPT4_FTM2CH1SRC_SHIFT               22
#define SIM_SOPT4_FTM0CLKSEL_MASK                0x1000000u
#define SIM_SOPT4_FTM0CLKSEL_SHIFT               24
#define SIM_SOPT4_FTM1CLKSEL_MASK                0x2000000u
#define SIM_SOPT4_FTM1CLKSEL_SHIFT               25
#define SIM_SOPT4_FTM2CLKSEL_MASK                0x4000000u
#define SIM_SOPT4_FTM2CLKSEL_SHIFT               26
#define SIM_SOPT4_FTM3CLKSEL_MASK                0x8000000u
#define SIM_SOPT4_FTM3CLKSEL_SHIFT               27
#define SIM_SOPT4_FTM0TRG0SRC_MASK               0x10000000u
#define SIM_SOPT4_FTM0TRG0SRC_SHIFT              28
#define SIM_SOPT4_FTM0TRG1SRC_MASK               0x20000000u
#define SIM_SOPT4_FTM0TRG1SRC_SHIFT              29
#define SIM_SOPT4_FTM3TRG0SRC_MASK               0x40000000u
#define SIM_SOPT4_FTM3TRG0SRC_SHIFT              30
#define SIM_SOPT4_FTM3TRG1SRC_MASK               0x80000000u
#define SIM_SOPT4_FTM3TRG1SRC_SHIFT              31
/* SOPT5 Bit Fields */
#define SIM_SOPT5_UART0TXSRC_MASK                0x3u
#define SIM_SOPT5_UART0TXSRC_SHIFT               0
#define SIM_SOPT5_UART0TXSRC(x)                  (((uint32)(((uint32)(x))<<SIM_SOPT5_UART0TXSRC_SHIFT))&SIM_SOPT5_UART0TXSRC_MASK)
#define SIM_SOPT5_UART0RXSRC_MASK                0xCu
#define SIM_SOPT5_UART0RXSRC_SHIFT               2
#define SIM_SOPT5_UART0RXSRC(x)                  (((uint32)(((uint32)(x))<<SIM_SOPT5_UART0RXSRC_SHIFT))&SIM_SOPT5_UART0RXSRC_MASK)
#define SIM_SOPT5_UART1TXSRC_MASK                0x30u
#define SIM_SOPT5_UART1TXSRC_SHIFT               4
#define SIM_SOPT5_UART1TXSRC(x)                  (((uint32)(((uint32)(x))<<SIM_SOPT5_UART1TXSRC_SHIFT))&SIM_SOPT5_UART1TXSRC_MASK)
#define SIM_SOPT5_UART1RXSRC_MASK                0xC0u
#define SIM_SOPT5_UART1RXSRC_SHIFT               6
#define SIM_SOPT5_UART1RXSRC(x)                  (((uint32)(((uint32)(x))<<SIM_SOPT5_UART1RXSRC_SHIFT))&SIM_SOPT5_UART1RXSRC_MASK)
#define SIM_SOPT5_LPUART0RXSRC_MASK              0xC0000u
#define SIM_SOPT5_LPUART0RXSRC_SHIFT             18
#define SIM_SOPT5_LPUART0RXSRC(x)                (((uint32)(((uint32)(x))<<SIM_SOPT5_LPUART0RXSRC_SHIFT))&SIM_SOPT5_LPUART0RXSRC_MASK)
/* SOPT7 Bit Fields */
#define SIM_SOPT7_ADC0TRGSEL_MASK                0xFu
#define SIM_SOPT7_ADC0TRGSEL_SHIFT               0
#define SIM_SOPT7_ADC0TRGSEL(x)                  (((uint32)(((uint32)(x))<<SIM_SOPT7_ADC0TRGSEL_SHIFT))&SIM_SOPT7_ADC0TRGSEL_MASK)
#define SIM_SOPT7_ADC0PRETRGSEL_MASK             0x10u
#define SIM_SOPT7_ADC0PRETRGSEL_SHIFT            4
#define SIM_SOPT7_ADC0ALTTRGEN_MASK              0x80u
#define SIM_SOPT7_ADC0ALTTRGEN_SHIFT             7
#define SIM_SOPT7_ADC1TRGSEL_MASK                0xF00u
#define SIM_SOPT7_ADC1TRGSEL_SHIFT               8
#define SIM_SOPT7_ADC1TRGSEL(x)                  (((uint32)(((uint32)(x))<<SIM_SOPT7_ADC1TRGSEL_SHIFT))&SIM_SOPT7_ADC1TRGSEL_MASK)
#define SIM_SOPT7_ADC1PRETRGSEL_MASK             0x1000u
#define SIM_SOPT7_ADC1PRETRGSEL_SHIFT            12
#define SIM_SOPT7_ADC1ALTTRGEN_MASK              0x8000u
#define SIM_SOPT7_ADC1ALTTRGEN_SHIFT             15
/* SOPT8 Bit Fields */
#define SIM_SOPT8_FTM0SYNCBIT_MASK               0x1u
#define SIM_SOPT8_FTM0SYNCBIT_SHIFT              0
#define SIM_SOPT8_FTM1SYNCBIT_MASK               0x2u
#define SIM_SOPT8_FTM1SYNCBIT_SHIFT              1
#define SIM_SOPT8_FTM2SYNCBIT_MASK               0x4u
#define SIM_SOPT8_FTM2SYNCBIT_SHIFT              2
#define SIM_SOPT8_FTM3SYNCBIT_MASK               0x8u
#define SIM_SOPT8_FTM3SYNCBIT_SHIFT              3
#define SIM_SOPT8_FTM0OCH0SRC_MASK               0x10000u
#define SIM_SOPT8_FTM0OCH0SRC_SHIFT              16
#define SIM_SOPT8_FTM0OCH1SRC_MASK               0x20000u
#define SIM_SOPT8_FTM0OCH1SRC_SHIFT              17
#define SIM_SOPT8_FTM0OCH2SRC_MASK               0x40000u
#define SIM_SOPT8_FTM0OCH2SRC_SHIFT              18
#define SIM_SOPT8_FTM0OCH3SRC_MASK               0x80000u
#define SIM_SOPT8_FTM0OCH3SRC_SHIFT              19
#define SIM_SOPT8_FTM0OCH4SRC_MASK               0x100000u
#define SIM_SOPT8_FTM0OCH4SRC_SHIFT              20
#define SIM_SOPT8_FTM0OCH5SRC_MASK               0x200000u
#define SIM_SOPT8_FTM0OCH5SRC_SHIFT              21
#define SIM_SOPT8_FTM0OCH6SRC_MASK               0x400000u
#define SIM_SOPT8_FTM0OCH6SRC_SHIFT              22
#define SIM_SOPT8_FTM0OCH7SRC_MASK               0x800000u
#define SIM_SOPT8_FTM0OCH7SRC_SHIFT              23
#define SIM_SOPT8_FTM3OCH0SRC_MASK               0x1000000u
#define SIM_SOPT8_FTM3OCH0SRC_SHIFT              24
#define SIM_SOPT8_FTM3OCH1SRC_MASK               0x2000000u
#define SIM_SOPT8_FTM3OCH1SRC_SHIFT              25
#define SIM_SOPT8_FTM3OCH2SRC_MASK               0x4000000u
#define SIM_SOPT8_FTM3OCH2SRC_SHIFT              26
#define SIM_SOPT8_FTM3OCH3SRC_MASK               0x8000000u
#define SIM_SOPT8_FTM3OCH3SRC_SHIFT              27
#define SIM_SOPT8_FTM3OCH4SRC_MASK               0x10000000u
#define SIM_SOPT8_FTM3OCH4SRC_SHIFT              28
#define SIM_SOPT8_FTM3OCH5SRC_MASK               0x20000000u
#define SIM_SOPT8_FTM3OCH5SRC_SHIFT              29
#define SIM_SOPT8_FTM3OCH6SRC_MASK               0x40000000u
#define SIM_SOPT8_FTM3OCH6SRC_SHIFT              30
#define SIM_SOPT8_FTM3OCH7SRC_MASK               0x80000000u
#define SIM_SOPT8_FTM3OCH7SRC_SHIFT              31
/* SDID Bit Fields */
#define SIM_SDID_PINID_MASK                      0xFu
#define SIM_SDID_PINID_SHIFT                     0
#define SIM_SDID_PINID(x)                        (((uint32)(((uint32)(x))<<SIM_SDID_PINID_SHIFT))&SIM_SDID_PINID_MASK)
#define SIM_SDID_DIEID_MASK                      0xF80u
#define SIM_SDID_DIEID_SHIFT                     7
#define SIM_SDID_DIEID(x)                        (((uint32)(((uint32)(x))<<SIM_SDID_DIEID_SHIFT))&SIM_SDID_DIEID_MASK)
#define SIM_SDID_REVID_MASK                      0xF000u
#define SIM_SDID_REVID_SHIFT                     12
#define SIM_SDID_REVID(x)                        (((uint32)(((uint32)(x))<<SIM_SDID_REVID_SHIFT))&SIM_SDID_REVID_MASK)
#define SIM_SDID_SERIESID_MASK                   0xF00000u
#define SIM_SDID_SERIESID_SHIFT                  20
#define SIM_SDID_SERIESID(x)                     (((uint32)(((uint32)(x))<<SIM_SDID_SERIESID_SHIFT))&SIM_SDID_SERIESID_MASK)
#define SIM_SDID_SUBFAMID_MASK                   0xF000000u
#define SIM_SDID_SUBFAMID_SHIFT                  24
#define SIM_SDID_SUBFAMID(x)                     (((uint32)(((uint32)(x))<<SIM_SDID_SUBFAMID_SHIFT))&SIM_SDID_SUBFAMID_MASK)
#define SIM_SDID_FAMILYID_MASK                   0xF0000000u
#define SIM_SDID_FAMILYID_SHIFT                  28
#define SIM_SDID_FAMILYID(x)                     (((uint32)(((uint32)(x))<<SIM_SDID_FAMILYID_SHIFT))&SIM_SDID_FAMILYID_MASK)
/* SCGC4 Bit Fields */
#define SIM_SCGC4_EWM_MASK                       0x2u
#define SIM_SCGC4_EWM_SHIFT                      1
#define SIM_SCGC4_I2C0_MASK                      0x40u
#define SIM_SCGC4_I2C0_SHIFT                     6
#define SIM_SCGC4_I2C1_MASK                      0x80u
#define SIM_SCGC4_I2C1_SHIFT                     7
#define SIM_SCGC4_UART0_MASK                     0x400u
#define SIM_SCGC4_UART0_SHIFT                    10
#define SIM_SCGC4_UART1_MASK                     0x800u
#define SIM_SCGC4_UART1_SHIFT                    11
#define SIM_SCGC4_UART2_MASK                     0x1000u
#define SIM_SCGC4_UART2_SHIFT                    12
#define SIM_SCGC4_CMP_MASK                       0x80000u
#define SIM_SCGC4_CMP_SHIFT                      19
#define SIM_SCGC4_VREF_MASK                      0x100000u
#define SIM_SCGC4_VREF_SHIFT                     20
/* SCGC5 Bit Fields */
#define SIM_SCGC5_LPTMR_MASK                     0x1u
#define SIM_SCGC5_LPTMR_SHIFT                    0
#define SIM_SCGC5_PORTA_MASK                     0x200u
#define SIM_SCGC5_PORTA_SHIFT                    9
#define SIM_SCGC5_PORTB_MASK                     0x400u
#define SIM_SCGC5_PORTB_SHIFT                    10
#define SIM_SCGC5_PORTC_MASK                     0x800u
#define SIM_SCGC5_PORTC_SHIFT                    11
#define SIM_SCGC5_PORTD_MASK                     0x1000u
#define SIM_SCGC5_PORTD_SHIFT                    12
#define SIM_SCGC5_PORTE_MASK                     0x2000u
#define SIM_SCGC5_PORTE_SHIFT                    13
/* SCGC6 Bit Fields */
#define SIM_SCGC6_FTF_MASK                       0x1u
#define SIM_SCGC6_FTF_SHIFT                      0
#define SIM_SCGC6_DMAMUX_MASK                    0x2u
#define SIM_SCGC6_DMAMUX_SHIFT                   1
#define SIM_SCGC6_FTM3_MASK                      0x40u
#define SIM_SCGC6_FTM3_SHIFT                     6
#define SIM_SCGC6_ADC1_MASK                      0x80u
#define SIM_SCGC6_ADC1_SHIFT                     7
#define SIM_SCGC6_DAC1_MASK                      0x100u
#define SIM_SCGC6_DAC1_SHIFT                     8
#define SIM_SCGC6_RNGA_MASK                      0x200u
#define SIM_SCGC6_RNGA_SHIFT                     9
#define SIM_SCGC6_LPUART0_MASK                   0x400u
#define SIM_SCGC6_LPUART0_SHIFT                  10
#define SIM_SCGC6_SPI0_MASK                      0x1000u
#define SIM_SCGC6_SPI0_SHIFT                     12
#define SIM_SCGC6_SPI1_MASK                      0x2000u
#define SIM_SCGC6_SPI1_SHIFT                     13
#define SIM_SCGC6_CRC_MASK                       0x40000u
#define SIM_SCGC6_CRC_SHIFT                      18
#define SIM_SCGC6_PDB_MASK                       0x400000u
#define SIM_SCGC6_PDB_SHIFT                      22
#define SIM_SCGC6_PIT_MASK                       0x800000u
#define SIM_SCGC6_PIT_SHIFT                      23
#define SIM_SCGC6_FTM0_MASK                      0x1000000u
#define SIM_SCGC6_FTM0_SHIFT                     24
#define SIM_SCGC6_FTM1_MASK                      0x2000000u
#define SIM_SCGC6_FTM1_SHIFT                     25
#define SIM_SCGC6_FTM2_MASK                      0x4000000u
#define SIM_SCGC6_FTM2_SHIFT                     26
#define SIM_SCGC6_ADC0_MASK                      0x8000000u
#define SIM_SCGC6_ADC0_SHIFT                     27
#define SIM_SCGC6_DAC0_MASK                      0x80000000u
#define SIM_SCGC6_DAC0_SHIFT                     31
/* SCGC7 Bit Fields */
#define SIM_SCGC7_FLEXBUS_MASK                   0x1u
#define SIM_SCGC7_FLEXBUS_SHIFT                  0
#define SIM_SCGC7_DMA_MASK                       0x2u
#define SIM_SCGC7_DMA_SHIFT                      1
/* CLKDIV1 Bit Fields */
#define SIM_CLKDIV1_OUTDIV4_MASK                 0xF0000u
#define SIM_CLKDIV1_OUTDIV4_SHIFT                16
#define SIM_CLKDIV1_OUTDIV4(x)                   (((uint32)(((uint32)(x))<<SIM_CLKDIV1_OUTDIV4_SHIFT))&SIM_CLKDIV1_OUTDIV4_MASK)
#define SIM_CLKDIV1_OUTDIV3_MASK                 0xF00000u
#define SIM_CLKDIV1_OUTDIV3_SHIFT                20
#define SIM_CLKDIV1_OUTDIV3(x)                   (((uint32)(((uint32)(x))<<SIM_CLKDIV1_OUTDIV3_SHIFT))&SIM_CLKDIV1_OUTDIV3_MASK)
#define SIM_CLKDIV1_OUTDIV2_MASK                 0xF000000u
#define SIM_CLKDIV1_OUTDIV2_SHIFT                24
#define SIM_CLKDIV1_OUTDIV2(x)                   (((uint32)(((uint32)(x))<<SIM_CLKDIV1_OUTDIV2_SHIFT))&SIM_CLKDIV1_OUTDIV2_MASK)
#define SIM_CLKDIV1_OUTDIV1_MASK                 0xF0000000u
#define SIM_CLKDIV1_OUTDIV1_SHIFT                28
#define SIM_CLKDIV1_OUTDIV1(x)                   (((uint32)(((uint32)(x))<<SIM_CLKDIV1_OUTDIV1_SHIFT))&SIM_CLKDIV1_OUTDIV1_MASK)
/* FCFG1 Bit Fields */
#define SIM_FCFG1_FLASHDIS_MASK                  0x1u
#define SIM_FCFG1_FLASHDIS_SHIFT                 0
#define SIM_FCFG1_FLASHDOZE_MASK                 0x2u
#define SIM_FCFG1_FLASHDOZE_SHIFT                1
#define SIM_FCFG1_PFSIZE_MASK                    0xF000000u
#define SIM_FCFG1_PFSIZE_SHIFT                   24
#define SIM_FCFG1_PFSIZE(x)                      (((uint32)(((uint32)(x))<<SIM_FCFG1_PFSIZE_SHIFT))&SIM_FCFG1_PFSIZE_MASK)
/* FCFG2 Bit Fields */
#define SIM_FCFG2_MAXADDR1_MASK                  0x7F0000u
#define SIM_FCFG2_MAXADDR1_SHIFT                 16
#define SIM_FCFG2_MAXADDR1(x)                    (((uint32)(((uint32)(x))<<SIM_FCFG2_MAXADDR1_SHIFT))&SIM_FCFG2_MAXADDR1_MASK)
#define SIM_FCFG2_MAXADDR0_MASK                  0x7F000000u
#define SIM_FCFG2_MAXADDR0_SHIFT                 24
#define SIM_FCFG2_MAXADDR0(x)                    (((uint32)(((uint32)(x))<<SIM_FCFG2_MAXADDR0_SHIFT))&SIM_FCFG2_MAXADDR0_MASK)
/* UIDH Bit Fields */
#define SIM_UIDH_UID_MASK                        0xFFFFFFFFu
#define SIM_UIDH_UID_SHIFT                       0
#define SIM_UIDH_UID(x)                          (((uint32)(((uint32)(x))<<SIM_UIDH_UID_SHIFT))&SIM_UIDH_UID_MASK)
/* UIDMH Bit Fields */
#define SIM_UIDMH_UID_MASK                       0xFFFFFFFFu
#define SIM_UIDMH_UID_SHIFT                      0
#define SIM_UIDMH_UID(x)                         (((uint32)(((uint32)(x))<<SIM_UIDMH_UID_SHIFT))&SIM_UIDMH_UID_MASK)
/* UIDML Bit Fields */
#define SIM_UIDML_UID_MASK                       0xFFFFFFFFu
#define SIM_UIDML_UID_SHIFT                      0
#define SIM_UIDML_UID(x)                         (((uint32)(((uint32)(x))<<SIM_UIDML_UID_SHIFT))&SIM_UIDML_UID_MASK)
/* UIDL Bit Fields */
#define SIM_UIDL_UID_MASK                        0xFFFFFFFFu
#define SIM_UIDL_UID_SHIFT                       0
#define SIM_UIDL_UID(x)                          (((uint32)(((uint32)(x))<<SIM_UIDL_UID_SHIFT))&SIM_UIDL_UID_MASK)

//*******************  Bit definition for SIM_CLKDIV register  *****************
#define SIM_PRE_OUTDIV1_DIV0                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 1
#define SIM_PRE_OUTDIV1_DIV1                   ((uint32) 0x10000000)             //01 - MCGOUTCLK divides by 2
#define SIM_PRE_OUTDIV1_DIV2                   ((uint32) 0x20000000)             //01 - MCGOUTCLK divides by 3
#define SIM_PRE_OUTDIV1_DIV3                   ((uint32) 0x30000000)             //01 - MCGOUTCLK divides by 4
#define SIM_PRE_OUTDIV1_DIV4                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 5
#define SIM_PRE_OUTDIV1_DIV5                   ((uint32) 0x10000000)             //01 - MCGOUTCLK divides by 6
#define SIM_PRE_OUTDIV1_DIV6                   ((uint32) 0x20000000)             //01 - MCGOUTCLK divides by 7
#define SIM_PRE_OUTDIV1_DIV7                   ((uint32) 0x30000000)             //01 - MCGOUTCLK divides by 8
#define SIM_PRE_OUTDIV1_DIV8                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 9
#define SIM_PRE_OUTDIV1_DIV9                   ((uint32) 0x10000000)             //01 - MCGOUTCLK divides by 10
#define SIM_PRE_OUTDIV1_DIV10                  ((uint32) 0x20000000)             //01 - MCGOUTCLK divides by 11
#define SIM_PRE_OUTDIV1_DIV11                  ((uint32) 0x30000000)             //01 - MCGOUTCLK divides by 12
#define SIM_PRE_OUTDIV1_DIV12                  ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 13
#define SIM_PRE_OUTDIV1_DIV13                  ((uint32) 0x10000000)             //01 - MCGOUTCLK divides by 14
#define SIM_PRE_OUTDIV1_DIV14                  ((uint32) 0x20000000)             //01 - MCGOUTCLK divides by 15
#define SIM_PRE_OUTDIV1_DIV15                  ((uint32) 0x30000000)             //01 - MCGOUTCLK divides by 16

#define SIM_PRE_OUTDIV2_DIV0                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 1
#define SIM_PRE_OUTDIV2_DIV1                   ((uint32) 0x01000000)             //01 - MCGOUTCLK divides by 2
#define SIM_PRE_OUTDIV2_DIV2                   ((uint32) 0x02000000)             //01 - MCGOUTCLK divides by 3
#define SIM_PRE_OUTDIV2_DIV3                   ((uint32) 0x03000000)             //01 - MCGOUTCLK divides by 4
#define SIM_PRE_OUTDIV2_DIV4                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 5
#define SIM_PRE_OUTDIV2_DIV5                   ((uint32) 0x01000000)             //01 - MCGOUTCLK divides by 6
#define SIM_PRE_OUTDIV2_DIV6                   ((uint32) 0x02000000)             //01 - MCGOUTCLK divides by 7
#define SIM_PRE_OUTDIV2_DIV7                   ((uint32) 0x03000000)             //01 - MCGOUTCLK divides by 8
#define SIM_PRE_OUTDIV2_DIV8                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 9
#define SIM_PRE_OUTDIV2_DIV9                   ((uint32) 0x01000000)             //01 - MCGOUTCLK divides by 10
#define SIM_PRE_OUTDIV2_DIV10                  ((uint32) 0x02000000)             //01 - MCGOUTCLK divides by 11
#define SIM_PRE_OUTDIV2_DIV11                  ((uint32) 0x03000000)             //01 - MCGOUTCLK divides by 12
#define SIM_PRE_OUTDIV2_DIV12                  ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 13
#define SIM_PRE_OUTDIV2_DIV13                  ((uint32) 0x01000000)             //01 - MCGOUTCLK divides by 14
#define SIM_PRE_OUTDIV2_DIV14                  ((uint32) 0x02000000)             //01 - MCGOUTCLK divides by 15
#define SIM_PRE_OUTDIV2_DIV15                  ((uint32) 0x03000000)             //01 - MCGOUTCLK divides by 16

#define SIM_PRE_OUTDIV3_DIV0                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 1
#define SIM_PRE_OUTDIV3_DIV1                   ((uint32) 0x00100000)             //01 - MCGOUTCLK divides by 2
#define SIM_PRE_OUTDIV3_DIV2                   ((uint32) 0x00200000)             //01 - MCGOUTCLK divides by 3
#define SIM_PRE_OUTDIV3_DIV3                   ((uint32) 0x00300000)             //01 - MCGOUTCLK divides by 4
#define SIM_PRE_OUTDIV3_DIV4                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 5
#define SIM_PRE_OUTDIV3_DIV5                   ((uint32) 0x00100000)             //01 - MCGOUTCLK divides by 6
#define SIM_PRE_OUTDIV3_DIV6                   ((uint32) 0x00200000)             //01 - MCGOUTCLK divides by 7
#define SIM_PRE_OUTDIV3_DIV7                   ((uint32) 0x00300000)             //01 - MCGOUTCLK divides by 8
#define SIM_PRE_OUTDIV3_DIV8                   ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 9
#define SIM_PRE_OUTDIV3_DIV9                   ((uint32) 0x00100000)             //01 - MCGOUTCLK divides by 10
#define SIM_PRE_OUTDIV3_DIV10                  ((uint32) 0x00200000)             //01 - MCGOUTCLK divides by 11
#define SIM_PRE_OUTDIV3_DIV11                  ((uint32) 0x00300000)             //01 - MCGOUTCLK divides by 12
#define SIM_PRE_OUTDIV3_DIV12                  ((uint32) 0x00000000)             //00 - MCGOUTCLK divides by 13
#define SIM_PRE_OUTDIV3_DIV13                  ((uint32) 0x00100000)             //01 - MCGOUTCLK divides by 14
#define SIM_PRE_OUTDIV3_DIV14                  ((uint32) 0x00200000)             //01 - MCGOUTCLK divides by 15
#define SIM_PRE_OUTDIV3_DIV15                  ((uint32) 0x00300000)             //01 - MCGOUTCLK divides by 16

#define SIM_PRE_OUTDIV4_DIV0                   ((uint32) 0x00 00 0000)             //00 - MCGOUTCLK divides by 1
#define SIM_PRE_OUTDIV4_DIV1                   ((uint32) 0x00 01 0000)             //01 - MCGOUTCLK divides by 2
#define SIM_PRE_OUTDIV4_DIV2                   ((uint32) 0x00 02 0000)             //01 - MCGOUTCLK divides by 3
#define SIM_PRE_OUTDIV4_DIV3                   ((uint32) 0x00 03 0000)             //01 - MCGOUTCLK divides by 4
#define SIM_PRE_OUTDIV4_DIV4                   ((uint32) 0x00 04 0000)             //00 - MCGOUTCLK divides by 5
#define SIM_PRE_OUTDIV4_DIV5                   ((uint32) 0x00 05 0000)             //01 - MCGOUTCLK divides by 6
#define SIM_PRE_OUTDIV4_DIV6                   ((uint32) 0x00 06 0000)             //01 - MCGOUTCLK divides by 7
#define SIM_PRE_OUTDIV4_DIV7                   ((uint32) 0x00 07 0000)             //01 - MCGOUTCLK divides by 8
#define SIM_PRE_OUTDIV4_DIV8                   ((uint32) 0x00 08 0000)             //00 - MCGOUTCLK divides by 9
#define SIM_PRE_OUTDIV4_DIV9                   ((uint32) 0x00 09 0000)             //01 - MCGOUTCLK divides by 10
#define SIM_PRE_OUTDIV4_DIV10                  ((uint32) 0x00 0A 0000)             //01 - MCGOUTCLK divides by 11
#define SIM_PRE_OUTDIV4_DIV11                  ((uint32) 0x00 0B 0000)             //01 - MCGOUTCLK divides by 12
#define SIM_PRE_OUTDIV4_DIV12                  ((uint32) 0x00 0C 0000)             //00 - MCGOUTCLK divides by 13
#define SIM_PRE_OUTDIV4_DIV13                  ((uint32) 0x00 0D 0000)             //01 - MCGOUTCLK divides by 14
#define SIM_PRE_OUTDIV4_DIV14                  ((uint32) 0x00 0E 0000)             //01 - MCGOUTCLK divides by 15
#define SIM_PRE_OUTDIV4_DIV15                  ((uint32) 0x00 0F 0000)             //01 - MCGOUTCLK divides by 16


//*******************  Bit definition for SIM_SRSID register for reset flag  *****************
////!*******************  Bit definition for RCC_CSR register  *******************
#define SIM_SRSID_LVDRSTF					 	((uint32) 0x00000002)			//! LVD Reset flag
#define SIM_SRSID_LOCRSTF						((uint32) 0x00000004)			//! Internal clock source module Reset flag
#define SIM_SRSID_WDOGRSTF						((uint32) 0x00000020)			//! Watchdog reset flag
#define SIM_SRSID_PINRSTF						((uint32) 0x00000040)			//! External Reset PIN flag
#define SIM_SRSID_PORRSTF						((uint32) 0x00000080)			//! POR reset flag
#define SIM_SRSID_LOCKUP						((uint32) 0x00000200)			//! Software Reset flag
#define SIM_SRSID_SFTRSTF						((uint32) 0x00000400)			//! Software Reset flag
#define SIM_SRSID_MDMAP							((uint32) 0x00000800)			//! Software Reset flag
#define SIM_SRSID_SACKERR						((uint32) 0x00002000)


//============================================================
/*
 * Oscillator Register
 */


/** OSC - Peripheral register structure */
#define OSC_BASE              ((uint32)0x40065000)

typedef struct
{
	volatile uint8 OSC_CR;                         //Oscillator Control Register, offset: 0x0
	volatile uint8 RESERVED_0[1];
	volatile uint8 OSC_DIV;                        //Oscillator Clock Divider, offset: 0x02
}OSC_DEF;

#define OSC               (*(OSC_DEF *)OSC_BASE)


/** OSC - Register Layout Typedef */
typedef struct {
  volatile uint8 CR;                                 /**< OSC Control Register, offset: 0x0 */
       uint8 RESERVED_0[1];
       volatile uint8 DIV;                                /**< OSC_DIV, offset: 0x2 */
} OSC_Type;
/* ----------------------------------------------------------------------------
   -- OSC Register Masks
   ---------------------------------------------------------------------------- */

/* CR Bit Fields */
#define OSC_CR_SC16P_MASK                        0x1u
#define OSC_CR_SC16P_SHIFT                       0
#define OSC_CR_SC8P_MASK                         0x2u
#define OSC_CR_SC8P_SHIFT                        1
#define OSC_CR_SC4P_MASK                         0x4u
#define OSC_CR_SC4P_SHIFT                        2
#define OSC_CR_SC2P_MASK                         0x8u
#define OSC_CR_SC2P_SHIFT                        3
#define OSC_CR_EREFSTEN_MASK                     0x20u
#define OSC_CR_EREFSTEN_SHIFT                    5
#define OSC_CR_ERCLKEN_MASK                      0x80u
#define OSC_CR_ERCLKEN_SHIFT                     7
/* DIV Bit Fields */
#define OSC_DIV_ERPS_MASK                        0xC0u
#define OSC_DIV_ERPS_SHIFT                       6
#define OSC_DIV_ERPS(x)                          (((uint8)(((uint8)(x))<<OSC_DIV_ERPS_SHIFT))&OSC_DIV_ERPS_MASK)


//=============================================================================================
/*
 * MCG: Multiple Clock Generator
 */

#define MCG_BASE              ((uint32)0x40064000)

/** MCG - Peripheral register structure */
typedef struct
{
	union
	{
		uint8 BYTE;
		struct
		{
			uint8 IREFSTEN	: 1;
			uint8 IRCLKEN  	: 1;  //MCGIRCLK:Internal Reference Clock Enable
			uint8 IREFS   	: 1;
			uint8 FRDIV   	: 3;
			uint8 CLKS 		: 2;

		}BIT;
	}MCG_C1;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 LOCRE0: 1;
			uint8 FCTRIM: 1;
			uint8 RANGE:  2;
			uint8 HGO   : 1;
			uint8 EREFS : 1;
			uint8 LP    : 1;
			uint8 IRCS  : 1;

		}BIT;
	}MCG_C2;

	volatile uint8 MCG_C3;                          // MCG Control Register 3, offset:0x2

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 SCFTRIM	: 1;
			uint8 FCTRIM	: 4;
			uint8 DRST_DRS	: 2;
			uint8 DMX32		: 1;

		}BIT;
	}volatile MCG_C4;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 PRDIV0	: 5;
			uint8 PLLSTEN0	: 1;
			uint8 PLLCLKEN0	: 1;
			uint8 RES0		: 1;

		}BIT;
	}volatile MCG_C5;								// MCG Control Register 5, offset:0x4

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 VDIV0	: 5;
			uint8 CME0	: 1;
			uint8 PLLS	: 1;
			uint8 LOLIE0: 1;

		}BIT;
	}MCG_C6;										// MCG Control Register 6, offset:0x5

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 IRCST		: 1;
			uint8 OSCINIT0	: 1;
			uint8 CLKST		: 2;
			uint8 IREFST	: 1;
			uint8 PLLST		: 1;
			uint8 LOCK0		: 1;
			uint8 LOLS0		: 1;

		}BIT;
	}MCG_S;

	volatile uint8 RESERVED_0[1];
	//volatile uint8 MCG_SC;                          // MCG Status and Control Register, offset:0x8

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 LOCS0		:1;
			uint8 FCRDIV	:3;
			uint8 FLTPRSRV	:1;
			uint8 ATMF		:1;
			uint8 ATMS		:1;
			uint8 ATME		:1;

		}BIT;
	}MCG_SC;
	volatile uint8 RESERVED_1[1];
	volatile uint8 MCG_ATCVH;                       // MCG Auto Trim Compare Value High Register, offset:0xA
	volatile uint8 MCG_ATCVL;                       // MCG Auto Trim Compare Value Low Register, offset:0xB
	volatile uint8 RESERVED_2[1];
	//volatile uint8 MCG_C8;                          // MCG Control 8 Register, offset:0xD
	union
	{
		uint8 BYTE;
		struct
		{
			uint8 LOCS1		: 1;
			uint8 RES_1_4	: 4;
			uint8 CME1		: 2;
			uint8 LOLRE   	: 1;
			uint8 LOCRE1 	: 1;

		}BIT;
	}MCG_C8;

}MCG_DEF;

#define MCG             (*(MCG_DEF *)MCG_BASE)





/*!
 * @addtogroup MCG_Peripheral_Access_Layer MCG Peripheral Access Layer
 * @{
 */

/** MCG - Register Layout Typedef */
typedef struct {
  volatile uint8 C1;                                 /**< MCG Control 1 Register, offset: 0x0 */
  volatile uint8 C2;                                 /**< MCG Control 2 Register, offset: 0x1 */
  volatile uint8 C3;                                 /**< MCG Control 3 Register, offset: 0x2 */
  volatile uint8 C4;                                 /**< MCG Control 4 Register, offset: 0x3 */
  volatile uint8 C5;                                 /**< MCG Control 5 Register, offset: 0x4 */
  volatile uint8 C6;                                 /**< MCG Control 6 Register, offset: 0x5 */
  volatile uint8 S;                                  /**< MCG Status Register, offset: 0x6 */
       uint8 RESERVED_0[1];
  volatile uint8 SC;                                 /**< MCG Status and Control Register, offset: 0x8 */
       uint8 RESERVED_1[1];
  volatile uint8 ATCVH;                              /**< MCG Auto Trim Compare Value High Register, offset: 0xA */
  volatile uint8 ATCVL;                              /**< MCG Auto Trim Compare Value Low Register, offset: 0xB */
  volatile uint8 C7;                                 /**< MCG Control 7 Register, offset: 0xC */
  volatile uint8 C8;                                 /**< MCG Control 8 Register, offset: 0xD */
} MCG_Type;


#define MCG_HD             ((MCG_Type *)MCG_BASE)


/*!
 * @addtogroup MCG_Register_Masks MCG Register Masks
 * @{
 */

/*! @name C1 - MCG Control 1 Register */
#define MCG_C1_IREFSTEN_MASK                     (0x1U)
#define MCG_C1_IREFSTEN_SHIFT                    (0U)
#define MCG_C1_IREFSTEN(x)                       (((uint8)(((uint8)(x)) << MCG_C1_IREFSTEN_SHIFT)) & MCG_C1_IREFSTEN_MASK)
#define MCG_C1_IRCLKEN_MASK                      (0x2U)
#define MCG_C1_IRCLKEN_SHIFT                     (1U)
#define MCG_C1_IRCLKEN(x)                        (((uint8)(((uint8)(x)) << MCG_C1_IRCLKEN_SHIFT)) & MCG_C1_IRCLKEN_MASK)
#define MCG_C1_IREFS_MASK                        (0x4U)
#define MCG_C1_IREFS_SHIFT                       (2U)
#define MCG_C1_IREFS(x)                          (((uint8)(((uint8)(x)) << MCG_C1_IREFS_SHIFT)) & MCG_C1_IREFS_MASK)
#define MCG_C1_FRDIV_MASK                        (0x38U)
#define MCG_C1_FRDIV_SHIFT                       (3U)
#define MCG_C1_FRDIV(x)                          (((uint8)(((uint8)(x)) << MCG_C1_FRDIV_SHIFT)) & MCG_C1_FRDIV_MASK)
#define MCG_C1_CLKS_MASK                         (0xC0U)
#define MCG_C1_CLKS_SHIFT                        (6U)
#define MCG_C1_CLKS(x)                           (((uint8)(((uint8)(x)) << MCG_C1_CLKS_SHIFT)) & MCG_C1_CLKS_MASK)

/*! @name C2 - MCG Control 2 Register */
#define MCG_C2_IRCS_MASK                         (0x1U)
#define MCG_C2_IRCS_SHIFT                        (0U)
#define MCG_C2_IRCS(x)                           (((uint8)(((uint8)(x)) << MCG_C2_IRCS_SHIFT)) & MCG_C2_IRCS_MASK)
#define MCG_C2_LP_MASK                           (0x2U)
#define MCG_C2_LP_SHIFT                          (1U)
#define MCG_C2_LP(x)                             (((uint8)(((uint8)(x)) << MCG_C2_LP_SHIFT)) & MCG_C2_LP_MASK)
#define MCG_C2_EREFS_MASK                        (0x4U)
#define MCG_C2_EREFS_SHIFT                       (2U)
#define MCG_C2_EREFS(x)                          (((uint8)(((uint8)(x)) << MCG_C2_EREFS_SHIFT)) & MCG_C2_EREFS_MASK)
#define MCG_C2_HGO_MASK                          (0x8U)
#define MCG_C2_HGO_SHIFT                         (3U)
#define MCG_C2_HGO(x)                            (((uint8)(((uint8)(x)) << MCG_C2_HGO_SHIFT)) & MCG_C2_HGO_MASK)
#define MCG_C2_RANGE_MASK                        (0x30U)
#define MCG_C2_RANGE_SHIFT                       (4U)
#define MCG_C2_RANGE(x)                          (((uint8)(((uint8)(x)) << MCG_C2_RANGE_SHIFT)) & MCG_C2_RANGE_MASK)
#define MCG_C2_FCFTRIM_MASK                      (0x40U)
#define MCG_C2_FCFTRIM_SHIFT                     (6U)
#define MCG_C2_FCFTRIM(x)                        (((uint8)(((uint8)(x)) << MCG_C2_FCFTRIM_SHIFT)) & MCG_C2_FCFTRIM_MASK)
#define MCG_C2_LOCRE0_MASK                       (0x80U)
#define MCG_C2_LOCRE0_SHIFT                      (7U)
#define MCG_C2_LOCRE0(x)                         (((uint8)(((uint8)(x)) << MCG_C2_LOCRE0_SHIFT)) & MCG_C2_LOCRE0_MASK)

/*! @name C3 - MCG Control 3 Register */
#define MCG_C3_SCTRIM_MASK                       (0xFFU)
#define MCG_C3_SCTRIM_SHIFT                      (0U)
#define MCG_C3_SCTRIM(x)                         (((uint8)(((uint8)(x)) << MCG_C3_SCTRIM_SHIFT)) & MCG_C3_SCTRIM_MASK)

/*! @name C4 - MCG Control 4 Register */
#define MCG_C4_SCFTRIM_MASK                      (0x1U)
#define MCG_C4_SCFTRIM_SHIFT                     (0U)
#define MCG_C4_SCFTRIM(x)                        (((uint8)(((uint8)(x)) << MCG_C4_SCFTRIM_SHIFT)) & MCG_C4_SCFTRIM_MASK)
#define MCG_C4_FCTRIM_MASK                       (0x1EU)
#define MCG_C4_FCTRIM_SHIFT                      (1U)
#define MCG_C4_FCTRIM(x)                         (((uint8)(((uint8)(x)) << MCG_C4_FCTRIM_SHIFT)) & MCG_C4_FCTRIM_MASK)
#define MCG_C4_DRST_DRS_MASK                     (0x60U)
#define MCG_C4_DRST_DRS_SHIFT                    (5U)
#define MCG_C4_DRST_DRS(x)                       (((uint8)(((uint8)(x)) << MCG_C4_DRST_DRS_SHIFT)) & MCG_C4_DRST_DRS_MASK)
#define MCG_C4_DMX32_MASK                        (0x80U)
#define MCG_C4_DMX32_SHIFT                       (7U)
#define MCG_C4_DMX32(x)                          (((uint8)(((uint8)(x)) << MCG_C4_DMX32_SHIFT)) & MCG_C4_DMX32_MASK)

/*! @name C5 - MCG Control 5 Register */
#define MCG_C5_PRDIV0_MASK                       (0x1FU)
#define MCG_C5_PRDIV0_SHIFT                      (0U)
#define MCG_C5_PRDIV0(x)                         (((uint8)(((uint8)(x)) << MCG_C5_PRDIV0_SHIFT)) & MCG_C5_PRDIV0_MASK)
#define MCG_C5_PLLSTEN0_MASK                     (0x20U)
#define MCG_C5_PLLSTEN0_SHIFT                    (5U)
#define MCG_C5_PLLSTEN0(x)                       (((uint8)(((uint8)(x)) << MCG_C5_PLLSTEN0_SHIFT)) & MCG_C5_PLLSTEN0_MASK)
#define MCG_C5_PLLCLKEN0_MASK                    (0x40U)
#define MCG_C5_PLLCLKEN0_SHIFT                   (6U)
#define MCG_C5_PLLCLKEN0(x)                      (((uint8)(((uint8)(x)) << MCG_C5_PLLCLKEN0_SHIFT)) & MCG_C5_PLLCLKEN0_MASK)

/*! @name C6 - MCG Control 6 Register */
#define MCG_C6_VDIV0_MASK                        (0x1FU)
#define MCG_C6_VDIV0_SHIFT                       (0U)
#define MCG_C6_VDIV0(x)                          (((uint8)(((uint8)(x)) << MCG_C6_VDIV0_SHIFT)) & MCG_C6_VDIV0_MASK)
#define MCG_C6_CME0_MASK                         (0x20U)
#define MCG_C6_CME0_SHIFT                        (5U)
#define MCG_C6_CME0(x)                           (((uint8)(((uint8)(x)) << MCG_C6_CME0_SHIFT)) & MCG_C6_CME0_MASK)
#define MCG_C6_PLLS_MASK                         (0x40U)
#define MCG_C6_PLLS_SHIFT                        (6U)
#define MCG_C6_PLLS(x)                           (((uint8)(((uint8)(x)) << MCG_C6_PLLS_SHIFT)) & MCG_C6_PLLS_MASK)
#define MCG_C6_LOLIE0_MASK                       (0x80U)
#define MCG_C6_LOLIE0_SHIFT                      (7U)
#define MCG_C6_LOLIE0(x)                         (((uint8)(((uint8)(x)) << MCG_C6_LOLIE0_SHIFT)) & MCG_C6_LOLIE0_MASK)

/*! @name S - MCG Status Register */
#define MCG_S_IRCST_MASK                         (0x1U)
#define MCG_S_IRCST_SHIFT                        (0U)
#define MCG_S_IRCST(x)                           (((uint8)(((uint8)(x)) << MCG_S_IRCST_SHIFT)) & MCG_S_IRCST_MASK)
#define MCG_S_OSCINIT0_MASK                      (0x2U)
#define MCG_S_OSCINIT0_SHIFT                     (1U)
#define MCG_S_OSCINIT0(x)                        (((uint8)(((uint8)(x)) << MCG_S_OSCINIT0_SHIFT)) & MCG_S_OSCINIT0_MASK)
#define MCG_S_CLKST_MASK                         (0xCU)
#define MCG_S_CLKST_SHIFT                        (2U)
#define MCG_S_CLKST(x)                           (((uint8)(((uint8)(x)) << MCG_S_CLKST_SHIFT)) & MCG_S_CLKST_MASK)
#define MCG_S_IREFST_MASK                        (0x10U)
#define MCG_S_IREFST_SHIFT                       (4U)
#define MCG_S_IREFST(x)                          (((uint8)(((uint8)(x)) << MCG_S_IREFST_SHIFT)) & MCG_S_IREFST_MASK)
#define MCG_S_PLLST_MASK                         (0x20U)
#define MCG_S_PLLST_SHIFT                        (5U)
#define MCG_S_PLLST(x)                           (((uint8)(((uint8)(x)) << MCG_S_PLLST_SHIFT)) & MCG_S_PLLST_MASK)
#define MCG_S_LOCK0_MASK                         (0x40U)
#define MCG_S_LOCK0_SHIFT                        (6U)
#define MCG_S_LOCK0(x)                           (((uint8)(((uint8)(x)) << MCG_S_LOCK0_SHIFT)) & MCG_S_LOCK0_MASK)
#define MCG_S_LOLS0_MASK                         (0x80U)
#define MCG_S_LOLS0_SHIFT                        (7U)
#define MCG_S_LOLS0(x)                           (((uint8)(((uint8)(x)) << MCG_S_LOLS0_SHIFT)) & MCG_S_LOLS0_MASK)

/*! @name SC - MCG Status and Control Register */
#define MCG_SC_LOCS0_MASK                        (0x1U)
#define MCG_SC_LOCS0_SHIFT                       (0U)
#define MCG_SC_LOCS0(x)                          (((uint8)(((uint8)(x)) << MCG_SC_LOCS0_SHIFT)) & MCG_SC_LOCS0_MASK)
#define MCG_SC_FCRDIV_MASK                       (0xEU)
#define MCG_SC_FCRDIV_SHIFT                      (1U)
#define MCG_SC_FCRDIV(x)                         (((uint8)(((uint8)(x)) << MCG_SC_FCRDIV_SHIFT)) & MCG_SC_FCRDIV_MASK)
#define MCG_SC_FLTPRSRV_MASK                     (0x10U)
#define MCG_SC_FLTPRSRV_SHIFT                    (4U)
#define MCG_SC_FLTPRSRV(x)                       (((uint8)(((uint8)(x)) << MCG_SC_FLTPRSRV_SHIFT)) & MCG_SC_FLTPRSRV_MASK)
#define MCG_SC_ATMF_MASK                         (0x20U)
#define MCG_SC_ATMF_SHIFT                        (5U)
#define MCG_SC_ATMF(x)                           (((uint8)(((uint8)(x)) << MCG_SC_ATMF_SHIFT)) & MCG_SC_ATMF_MASK)
#define MCG_SC_ATMS_MASK                         (0x40U)
#define MCG_SC_ATMS_SHIFT                        (6U)
#define MCG_SC_ATMS(x)                           (((uint8)(((uint8)(x)) << MCG_SC_ATMS_SHIFT)) & MCG_SC_ATMS_MASK)
#define MCG_SC_ATME_MASK                         (0x80U)
#define MCG_SC_ATME_SHIFT                        (7U)
#define MCG_SC_ATME(x)                           (((uint8)(((uint8)(x)) << MCG_SC_ATME_SHIFT)) & MCG_SC_ATME_MASK)

/*! @name ATCVH - MCG Auto Trim Compare Value High Register */
#define MCG_ATCVH_ATCVH_MASK                     (0xFFU)
#define MCG_ATCVH_ATCVH_SHIFT                    (0U)
#define MCG_ATCVH_ATCVH(x)                       (((uint8)(((uint8)(x)) << MCG_ATCVH_ATCVH_SHIFT)) & MCG_ATCVH_ATCVH_MASK)

/*! @name ATCVL - MCG Auto Trim Compare Value Low Register */
#define MCG_ATCVL_ATCVL_MASK                     (0xFFU)
#define MCG_ATCVL_ATCVL_SHIFT                    (0U)
#define MCG_ATCVL_ATCVL(x)                       (((uint8)(((uint8)(x)) << MCG_ATCVL_ATCVL_SHIFT)) & MCG_ATCVL_ATCVL_MASK)

/*! @name C7 - MCG Control 7 Register */
#define MCG_C7_OSCSEL_MASK                       (0x3U)
#define MCG_C7_OSCSEL_SHIFT                      (0U)
#define MCG_C7_OSCSEL(x)                         (((uint8)(((uint8)(x)) << MCG_C7_OSCSEL_SHIFT)) & MCG_C7_OSCSEL_MASK)

/*! @name C8 - MCG Control 8 Register */
#define MCG_C8_LOCS1_MASK                        (0x1U)
#define MCG_C8_LOCS1_SHIFT                       (0U)
#define MCG_C8_LOCS1(x)                          (((uint8)(((uint8)(x)) << MCG_C8_LOCS1_SHIFT)) & MCG_C8_LOCS1_MASK)
#define MCG_C8_CME1_MASK                         (0x20U)
#define MCG_C8_CME1_SHIFT                        (5U)
#define MCG_C8_CME1(x)                           (((uint8)(((uint8)(x)) << MCG_C8_CME1_SHIFT)) & MCG_C8_CME1_MASK)
#define MCG_C8_LOLRE_MASK                        (0x40U)
#define MCG_C8_LOLRE_SHIFT                       (6U)
#define MCG_C8_LOLRE(x)                          (((uint8)(((uint8)(x)) << MCG_C8_LOLRE_SHIFT)) & MCG_C8_LOLRE_MASK)
#define MCG_C8_LOCRE1_MASK                       (0x80U)
#define MCG_C8_LOCRE1_SHIFT                      (7U)
#define MCG_C8_LOCRE1(x)                         (((uint8)(((uint8)(x)) << MCG_C8_LOCRE1_SHIFT)) & MCG_C8_LOCRE1_MASK)

/* ----------------------------------------------------------------------------
   -- MCG Register Masks
   ---------------------------------------------------------------------------- */

/* C5 Bit Fields */
#define MCG_C5_PRDIV_MASK                        0x7u
#define MCG_C5_PRDIV_SHIFT                       0
#define MCG_C5_PRDIV(x)                          (((uint8)(((uint8)(x))<<MCG_C5_PRDIV_SHIFT))&MCG_C5_PRDIV_MASK)
#define MCG_C5_PLLSTEN_MASK                      0x20u
#define MCG_C5_PLLSTEN_SHIFT                     5
#define MCG_C5_PLLCLKEN_MASK                     0x40u
#define MCG_C5_PLLCLKEN_SHIFT                    6
/* C6 Bit Fields */
#define MCG_C6_VDIV_MASK                         0x1Fu
#define MCG_C6_VDIV_SHIFT                        0
#define MCG_C6_VDIV(x)                           (((uint8)(((uint8)(x))<<MCG_C6_VDIV_SHIFT))&MCG_C6_VDIV_MASK)


//============================================================
/*
 * @brief ICS Internal Clock source mapping of Cortex-M0
 */
#define ICS_BASE              ((uint32)0x40064000)

typedef struct
{
	volatile uint8  ICS_C1;                         //! ICS Control Register 1
	volatile uint8  ICS_C2;                         //! ICS Control Register 2
	volatile uint8  ICS_C3;                         //! ICS Control Register 3
	volatile uint8  ICS_C4;                         //! ICS Control Register 4
	volatile uint8  ICS_S;                          //! ICS Status Register

}ICS_DEF;

#define ICS             (*(ICS_DEF *)ICS_BASE)

// Default value of ICS and OSC registers after reset
#define ICS_C1_DEFAULT           0x04               //! Internal ref clock selected
#define ICS_C2_DEFAULT           0x20               //! Divide select clock by 2
#define ICS_C3_DEFAULT           0x5B
#define ICS_C4_DEFAULT           0x01
#define ICS_S_DEFAULT            0x50
#define OSC_CR_DEFAULT           0

//! Internal Clock Status Register 1 => ICS_C1
#define ICS_C1_IREFSTEN_POS               0
#define ICS_C1_IREFSTEN_MSK               0x01
#define ICS_C1_IRCLKEN_POS                1
#define ICS_C1_IRCLKEN_MSK                0x02
#define ICS_C1_IREFS_POS                  2
#define ICS_C1_IREFS_MSK                  0x04
#define ICS_C1_RDIV_POS                   3
#define ICS_C1_RDIV_MSK                   0x38
#define ICS_C1_CLKS_POS                   6
#define ICS_C1_CLKS_MSK                   0xC0
#define ICS_C1_CLKS(x)                    (((uint8)(((uint8)(x))<<ICS_C1_CLKS_POS))&ICS_C1_CLKS_MSK)
//! Internal Clock Status Register 2 => ICS_C2
#define ICS_C2_LP_POS                      4
#define ICS_C2_LP_MSK                     (0x10)
#define ICS_C2_BDIV_POS                    5
#define ICS_C2_BDIV_MSK                   (0xE0)
#define ICS_C2_BDIV(x)                    (((uint8)(((uint8)(x))<<ICS_C2_BDIV_POS))&ICS_C2_BDIV_MSK)
//! Internal Clock Status Register 3 => ICS_C3
#define ICS_C3_SCTRIM_POS                  0
#define ICS_C3_SCTRIM_MSK                  0xFF
//! Internal Clock Status Register 4 => ICS_C4
#define ICS_C4_SCFTRIM_POS                 0
#define ICS_C4_SCFTRIM_MSK                 0x01
#define ICS_C4_CME_POS                     5
#define ICS_C4_CME_MSK                     0x20
#define ICS_C4_LOLIE_POS                   7
#define ICS_C4_LOLIE_MSK                   0x80
//! ICS Status Register => ICS_S
#define ICS_S_CLKST_POS                    2
#define ICS_S_CLKST_MSK                    0x0C
#define ICS_S_IREFST_POS                   4
#define ICS_S_IREFST_MSK                   0x10
#define ICS_S_LOCK_POS                     6
#define ICS_S_LOCK_MSK                     0x40
#define ICS_S_LOLS_POS                     7
#define ICS_S_LOLS_MSK                     0x80

//*******************  Bit definition for ICS_C1 register  *****************
//FEE mode only                                                   OSC_CR[RANGE] = 0     OSC_CR[RANGE] = 0
#define ICS_C1_RDIV_DIV0                   ((uint8) 0x00)       //        1                   32
#define ICS_C1_RDIV_DIV1                   ((uint8) 0x08)       //        2                   64
#define ICS_C1_RDIV_DIV2                   ((uint8) 0x10)       //        4                   128
#define ICS_C1_RDIV_DIV3                   ((uint8) 0x18)       //        8                   256
#define ICS_C1_RDIV_DIV4                   ((uint8) 0x20)       //        16                  512
#define ICS_C1_RDIV_DIV5                   ((uint8) 0x28)       //        32                  1024
#define ICS_C1_RDIV_DIV6                   ((uint8) 0x30)       //        64                  Reserved
#define ICS_C1_RDIV_DIV7                   ((uint8) 0x38)       //        128                 Reserved

//*******************  Bit definition for ICS_C2 register  *****************
//FEI (High range)
#define ICS_C2_PREBDIV_DIV0                   ((uint8) 0x00)             //40Mhz ~ 50Mhz
#define ICS_C2_PREBDIV_DIV1                   ((uint8) 0x20)             //20Mhz ~ 25Mhz
#define ICS_C2_PREBDIV_DIV2                   ((uint8) 0x40)             //10Mhz ~ 12.5Mhz
#define ICS_C2_PREBDIV_DIV4                   ((uint8) 0x60)             //5Mhz ~ 6.25Mhz
#define ICS_C2_PREBDIV_DIV8                   ((uint8) 0x80)             //2.5Mhz ~ 3.125Mhz
#define ICS_C2_PREBDIV_DIV16                  ((uint8) 0xA0)             //1.25Mhz ~ 1.5625Mhz
#define ICS_C2_PREBDIV_DIV32                  ((uint8) 0xC0)             //625Khz ~ 781.25Khz
#define ICS_C2_PREBDIV_DIV64                  ((uint8) 0xE0)             //312.5Khz ~ 390.625Khz

// ============================================================
// ---------------------- TIMER SECTION -----------------------
// ------------------------------------------------------------
//============================================================
/*
 * @brief System Tick
 */
typedef struct
{
    volatile uint32 CTRL;        //! Offset: 0x000 (R/W)  SysTick Control and Status Register
    volatile uint32 LOAD;        //! Offset: 0x004 (R/W)  SysTick Reload Value Register
    volatile uint32 VAL;         //! Offset: 0x008 (R/W)  SysTick Current Value Register
    const uint32 CALIB;          //! Offset: 0x00C (R/ )  SysTick Calibration Register
} SYSTICK_DEF;

#define SYSTICK_COUNTER_DISABLE             ((uint32)0xFFFFFFFE)
#define SYSTICK_COUNTER_ENABLE              ((uint32)0x00000001)
#define SYSTICK_COUNTER_CLEAR               ((uint32)0x00000000)

//!  SYSTICK Control / Status Register Definitions
#define SYSTICK_CTRL_COUNTFLAG              (uint32)(0x00010000)

#define SYSTICK_CTRL_COUNTFLAG_POS         16                                             //! SYSTICK CTRL: COUNTFLAG POSition
#define SYSTICK_CTRL_COUNTFLAG_MSK         (1UL << SYSTICK_CTRL_COUNTFLAG_POS)            //! SYSTICK CTRL: COUNTFLAG Mask

#define SYSTICK_CTRL_CLKSOURCE_POS          2                                             //! SYSTICK CTRL: CLKSOURCE POSition
#define SYSTICK_CTRL_CLKSOURCE_MSK         (1UL << SYSTICK_CTRL_CLKSOURCE_POS)            //! SYSTICK CTRL: CLKSOURCE Mask

#define SYSTICK_CTRL_TICKINT_POS            1                                             //! SYSTICK CTRL: TICKINT POSition
#define SYSTICK_CTRL_TICKINT_MSK           (1UL << SYSTICK_CTRL_TICKINT_POS)              //! SYSTICK CTRL: TICKINT Mask

#define SYSTICK_CTRL_ENABLE_POS             0                                             //! SYSTICK CTRL: ENABLE POSition
#define SYSTICK_CTRL_ENABLE_MSK            (1UL << SYSTICK_CTRL_ENABLE_POS)               //! SYSTICK CTRL: ENABLE Mask

//!  SYSTICK Reload Register Definitions
#define SYSTICK_LOAD_RELOAD_POS             0                                             //! SYSTICK LOAD: RELOAD POSition
#define SYSTICK_LOAD_RELOAD_MSK            (0xFFFFFFUL << SYSTICK_LOAD_RELOAD_POS)        //! SYSTICK LOAD: RELOAD Mask

//!  SYSTICK Current Register Definitions
#define SYSTICK_VAL_CURRENT_POS             0                                             //! SYSTICK VAL: CURRENT POSition
#define SYSTICK_VAL_CURRENT_MSK            (0xFFFFFFUL << SYSTICK_VAL_CURRENT_POS)        //! SYSTICK VAL: CURRENT Mask

//!  SYSTICK Calibration Register Definitions
#define SYSTICK_CALIB_NOREF_POS            31                                             //! SYSTICK CALIB: NOREF POSition
#define SYSTICK_CALIB_NOREF_MSK            (1UL << SYSTICK_CALIB_NOREF_POS)               //! SYSTICK CALIB: NOREF Mask

#define SYSTICK_CALIB_SKEW_POS             30                                             //! SYSTICK CALIB: SKEW POSition
#define SYSTICK_CALIB_SKEW_MSK             (1UL << SYSTICK_CALIB_SKEW_POS)                //! SYSTICK CALIB: SKEW Mask

#define SYSTICK_CALIB_TENMS_POS             0                                             //! SYSTICK CALIB: TENMS POSition
#define SYSTICK_CALIB_TENMS_MSK            (0xFFFFFFUL << SYSTICK_VAL_CURRENT_POS)        //! SYSTICK CALIB: TENMS Mask

//============================================================
/**
 * @brief Universal synchronous asynchronous receiver transmitter (USART)
 */
#define UART0_BASE                               0x4006A000
#define UART1_BASE                               0x4006B000
#define UART2_BASE                               0x4006C000
typedef struct
{
	volatile uint8 UART_BRRH;                    //Baud Rate Register HIGH
	volatile uint8 UART_BRRL;                    //Baud Rate Register LOW
	volatile uint8 UART_C1;                      //UART Control Register 1
	volatile uint8 UART_C2;                      //UART Control Register 2
	volatile uint8 UART_S1;                      //UART Status Register 1
	volatile uint8 UART_S2;                      //UART Status Register 2
	volatile uint8 UART_C3;                      //UART Control Register 3
	volatile uint8 UART_DATA;                    //UART Data Register
	volatile uint8 UART_MA1;                     //UART Match Address Register 1
	volatile uint8 UART_MA2;                     //UART Match Address Register 2
	volatile uint8 UART_C4;                      //UART Control Register 4
	volatile uint8 UART_C5;                      //UART Control Register 5
	volatile uint8 UART_ED;                      //UART Extended Data Register
}UART_BASIC_DEF;

#define UART0                                    (*(UART_BASIC_DEF *)UART0_BASE)
#define UART1                                    (*(UART_BASIC_DEF *)UART1_BASE)
#define UART2                                    (*(UART_BASIC_DEF *)UART2_BASE)

//! *****************  Bit definition for USART_CR1 register  ******************
/* BDH Bit Fields */
#define UART_BDH_SBR_MASK                        0x1Fu
#define UART_BDH_SBR_SHIFT                       0
#define UART_BDH_SBR(x)                          (((uint8)(((uint8)(x))<<UART_BDH_SBR_SHIFT))&UART_BDH_SBR_MASK)
#define UART_BDH_SBNS_MASK                       0x20u
#define UART_BDH_SBNS_SHIFT                      5
#define UART_BDH_RXEDGIE_MASK                    0x40u
#define UART_BDH_RXEDGIE_SHIFT                   6
#define UART_BDH_LBKDIE_MASK                     0x80u
#define UART_BDH_LBKDIE_SHIFT                    7
/* BDL Bit Fields */
#define UART_BDL_SBR_MASK                        0xFFu
#define UART_BDL_SBR_SHIFT                       0
#define UART_BDL_SBR(x)                          (((uint8)(((uint8)(x))<<UART_BDL_SBR_SHIFT))&UART_BDL_SBR_MASK)
/* C1 Bit Fields */
#define UART_C1_PT_MASK                          0x1u
#define UART_C1_PT_SHIFT                         0
#define UART_C1_PE_MASK                          0x2u
#define UART_C1_PE_SHIFT                         1
#define UART_C1_ILT_MASK                         0x4u
#define UART_C1_ILT_SHIFT                        2
#define UART_C1_WAKE_MASK                        0x8u
#define UART_C1_WAKE_SHIFT                       3
#define UART_C1_M_MASK                           0x10u
#define UART_C1_M_SHIFT                          4
#define UART_C1_RSRC_MASK                        0x20u
#define UART_C1_RSRC_SHIFT                       5
#define UART_C1_UARTSWAI_MASK                    0x40u
#define UART_C1_UARTSWAI_SHIFT                   6
#define UART_C1_LOOPS_MASK                       0x80u
#define UART_C1_LOOPS_SHIFT                      7
/* C2 Bit Fields */
#define UART_C2_SBK_MASK                         0x1u
#define UART_C2_SBK_SHIFT                        0
#define UART_C2_RWU_MASK                         0x2u
#define UART_C2_RWU_SHIFT                        1
#define UART_C2_RE_MASK                          0x4u
#define UART_C2_RE_SHIFT                         2
#define UART_C2_TE_MASK                          0x8u
#define UART_C2_TE_SHIFT                         3
#define UART_C2_ILIE_MASK                        0x10u
#define UART_C2_ILIE_SHIFT                       4
#define UART_C2_RIE_MASK                         0x20u
#define UART_C2_RIE_SHIFT                        5
#define UART_C2_TCIE_MASK                        0x40u
#define UART_C2_TCIE_SHIFT                       6
#define UART_C2_TIE_MASK                         0x80u
#define UART_C2_TIE_SHIFT                        7
/* S1 Bit Fields */
#define UART_S1_PF_MASK                          0x1u
#define UART_S1_PF_SHIFT                         0
#define UART_S1_FE_MASK                          0x2u
#define UART_S1_FE_SHIFT                         1
#define UART_S1_NF_MASK                          0x4u
#define UART_S1_NF_SHIFT                         2
#define UART_S1_OR_MASK                          0x8u
#define UART_S1_OR_SHIFT                         3
#define UART_S1_IDLE_MASK                        0x10u
#define UART_S1_IDLE_SHIFT                       4
#define UART_S1_RDRF_MASK                        0x20u
#define UART_S1_RDRF_SHIFT                       5
#define UART_S1_TC_MASK                          0x40u
#define UART_S1_TC_SHIFT                         6
#define UART_S1_TDRE_MASK                        0x80u
#define UART_S1_TDRE_SHIFT                       7
/* S2 Bit Fields */
#define UART_S2_RAF_MASK                         0x1u
#define UART_S2_RAF_SHIFT                        0
#define UART_S2_LBKDE_MASK                       0x2u
#define UART_S2_LBKDE_SHIFT                      1
#define UART_S2_BRK13_MASK                       0x4u
#define UART_S2_BRK13_SHIFT                      2
#define UART_S2_RWUID_MASK                       0x8u
#define UART_S2_RWUID_SHIFT                      3
#define UART_S2_RXINV_MASK                       0x10u
#define UART_S2_RXINV_SHIFT                      4
#define UART_S2_RXEDGIF_MASK                     0x40u
#define UART_S2_RXEDGIF_SHIFT                    6
#define UART_S2_LBKDIF_MASK                      0x80u
#define UART_S2_LBKDIF_SHIFT                     7
/* C3 Bit Fields */
#define UART_C3_PEIE_MASK                        0x1u
#define UART_C3_PEIE_SHIFT                       0
#define UART_C3_FEIE_MASK                        0x2u
#define UART_C3_FEIE_SHIFT                       1
#define UART_C3_NEIE_MASK                        0x4u
#define UART_C3_NEIE_SHIFT                       2
#define UART_C3_ORIE_MASK                        0x8u
#define UART_C3_ORIE_SHIFT                       3
#define UART_C3_TXINV_MASK                       0x10u
#define UART_C3_TXINV_SHIFT                      4
#define UART_C3_TXDIR_MASK                       0x20u
#define UART_C3_TXDIR_SHIFT                      5
#define UART_C3_T8_MASK                          0x40u
#define UART_C3_T8_SHIFT                         6
#define UART_C3_R8_MASK                          0x80u
#define UART_C3_R8_SHIFT                         7
/* D Bit Fields */
#define UART_D_R0T0_MASK                         0x1u
#define UART_D_R0T0_SHIFT                        0
#define UART_D_R1T1_MASK                         0x2u
#define UART_D_R1T1_SHIFT                        1
#define UART_D_R2T2_MASK                         0x4u
#define UART_D_R2T2_SHIFT                        2
#define UART_D_R3T3_MASK                         0x8u
#define UART_D_R3T3_SHIFT                        3
#define UART_D_R4T4_MASK                         0x10u
#define UART_D_R4T4_SHIFT                        4
#define UART_D_R5T5_MASK                         0x20u
#define UART_D_R5T5_SHIFT                        5
#define UART_D_R6T6_MASK                         0x40u
#define UART_D_R6T6_SHIFT                        6
#define UART_D_R7T7_MASK                         0x80u
#define UART_D_R7T7_SHIFT                        7


/* MA1 Bit Fields */
#define UART_MA1_MA_MASK                         0xFFu
#define UART_MA1_MA_SHIFT                        0
#define UART_MA1_MA(x)                           (((uint8)(((uint8)(x))<<UART_MA1_MA_SHIFT))&UART_MA1_MA_MASK)
/* MA2 Bit Fields */
#define UART_MA2_MA_MASK                         0xFFu
#define UART_MA2_MA_SHIFT                        0
#define UART_MA2_MA(x)                           (((uint8)(((uint8)(x))<<UART_MA2_MA_SHIFT))&UART_MA2_MA_MASK)
/* C4 Bit Fields */
#define UART_C4_BRFA_MASK                        0x1Fu
#define UART_C4_BRFA_SHIFT                       0
#define UART_C4_BRFA(x)                          (((uint8)(((uint8)(x))<<UART_C4_BRFA_SHIFT))&UART_C4_BRFA_MASK)
#define UART_C4_M10_MASK                         0x20u
#define UART_C4_M10_SHIFT                        5
#define UART_C4_MAEN2_MASK                       0x40u
#define UART_C4_MAEN2_SHIFT                      6
#define UART_C4_MAEN1_MASK                       0x80u
#define UART_C4_MAEN1_SHIFT                      7
/* C5 Bit Fields */
#define UART_C5_RDMAS_MASK                       0x20u
#define UART_C5_RDMAS_SHIFT                      5
#define UART_C5_TDMAS_MASK                       0x80u
#define UART_C5_TDMAS_SHIFT                      7
/* ED Bit Fields */
#define UART_ED_PARITYE_MASK                     0x40u
#define UART_ED_PARITYE_SHIFT                    6
#define UART_ED_NOISY_MASK                       0x80u
#define UART_ED_NOISY_SHIFT                      7
/* MODEM Bit Fields */
#define UART_MODEM_TXCTSE_MASK                   0x1u
#define UART_MODEM_TXCTSE_SHIFT                  0
#define UART_MODEM_TXRTSE_MASK                   0x2u
#define UART_MODEM_TXRTSE_SHIFT                  1
#define UART_MODEM_TXRTSPOL_MASK                 0x4u
#define UART_MODEM_TXRTSPOL_SHIFT                2
#define UART_MODEM_RXRTSE_MASK                   0x8u
#define UART_MODEM_RXRTSE_SHIFT                  3
/* IR Bit Fields */
#define UART_IR_TNP_MASK                         0x3u
#define UART_IR_TNP_SHIFT                        0
#define UART_IR_TNP(x)                           (((uint8)(((uint8)(x))<<UART_IR_TNP_SHIFT))&UART_IR_TNP_MASK)
#define UART_IR_IREN_MASK                        0x4u
#define UART_IR_IREN_SHIFT                       2
/* PFIFO Bit Fields */
#define UART_PFIFO_RXFIFOSIZE_MASK               0x7u
#define UART_PFIFO_RXFIFOSIZE_SHIFT              0
#define UART_PFIFO_RXFIFOSIZE(x)                 (((uint8)(((uint8)(x))<<UART_PFIFO_RXFIFOSIZE_SHIFT))&UART_PFIFO_RXFIFOSIZE_MASK)
#define UART_PFIFO_RXFE_MASK                     0x8u
#define UART_PFIFO_RXFE_SHIFT                    3
#define UART_PFIFO_TXFIFOSIZE_MASK               0x70u
#define UART_PFIFO_TXFIFOSIZE_SHIFT              4
#define UART_PFIFO_TXFIFOSIZE(x)                 (((uint8)(((uint8)(x))<<UART_PFIFO_TXFIFOSIZE_SHIFT))&UART_PFIFO_TXFIFOSIZE_MASK)
#define UART_PFIFO_TXFE_MASK                     0x80u
#define UART_PFIFO_TXFE_SHIFT                    7
/* CFIFO Bit Fields */
#define UART_CFIFO_RXUFE_MASK                    0x1u
#define UART_CFIFO_RXUFE_SHIFT                   0
#define UART_CFIFO_TXOFE_MASK                    0x2u
#define UART_CFIFO_TXOFE_SHIFT                   1
#define UART_CFIFO_RXOFE_MASK                    0x4u
#define UART_CFIFO_RXOFE_SHIFT                   2
#define UART_CFIFO_RXFLUSH_MASK                  0x40u
#define UART_CFIFO_RXFLUSH_SHIFT                 6
#define UART_CFIFO_TXFLUSH_MASK                  0x80u
#define UART_CFIFO_TXFLUSH_SHIFT                 7
/* SFIFO Bit Fields */
#define UART_SFIFO_RXUF_MASK                     0x1u
#define UART_SFIFO_RXUF_SHIFT                    0
#define UART_SFIFO_TXOF_MASK                     0x2u
#define UART_SFIFO_TXOF_SHIFT                    1
#define UART_SFIFO_RXOF_MASK                     0x4u
#define UART_SFIFO_RXOF_SHIFT                    2
#define UART_SFIFO_RXEMPT_MASK                   0x40u
#define UART_SFIFO_RXEMPT_SHIFT                  6
#define UART_SFIFO_TXEMPT_MASK                   0x80u
#define UART_SFIFO_TXEMPT_SHIFT                  7
/* TWFIFO Bit Fields */
#define UART_TWFIFO_TXWATER_MASK                 0xFFu
#define UART_TWFIFO_TXWATER_SHIFT                0
#define UART_TWFIFO_TXWATER(x)                   (((uint8)(((uint8)(x))<<UART_TWFIFO_TXWATER_SHIFT))&UART_TWFIFO_TXWATER_MASK)
/* TCFIFO Bit Fields */
#define UART_TCFIFO_TXCOUNT_MASK                 0xFFu
#define UART_TCFIFO_TXCOUNT_SHIFT                0
#define UART_TCFIFO_TXCOUNT(x)                   (((uint8)(((uint8)(x))<<UART_TCFIFO_TXCOUNT_SHIFT))&UART_TCFIFO_TXCOUNT_MASK)
/* RWFIFO Bit Fields */
#define UART_RWFIFO_RXWATER_MASK                 0xFFu
#define UART_RWFIFO_RXWATER_SHIFT                0
#define UART_RWFIFO_RXWATER(x)                   (((uint8)(((uint8)(x))<<UART_RWFIFO_RXWATER_SHIFT))&UART_RWFIFO_RXWATER_MASK)
/* RCFIFO Bit Fields */
#define UART_RCFIFO_RXCOUNT_MASK                 0xFFu
#define UART_RCFIFO_RXCOUNT_SHIFT                0
#define UART_RCFIFO_RXCOUNT(x)                   (((uint8)(((uint8)(x))<<UART_RCFIFO_RXCOUNT_SHIFT))&UART_RCFIFO_RXCOUNT_MASK)
/* C7816 Bit Fields */
#define UART_C7816_ISO_7816E_MASK                0x1u
#define UART_C7816_ISO_7816E_SHIFT               0
#define UART_C7816_TTYPE_MASK                    0x2u
#define UART_C7816_TTYPE_SHIFT                   1
#define UART_C7816_INIT_MASK                     0x4u
#define UART_C7816_INIT_SHIFT                    2
#define UART_C7816_ANACK_MASK                    0x8u
#define UART_C7816_ANACK_SHIFT                   3
#define UART_C7816_ONACK_MASK                    0x10u
#define UART_C7816_ONACK_SHIFT                   4
/* IE7816 Bit Fields */
#define UART_IE7816_RXTE_MASK                    0x1u
#define UART_IE7816_RXTE_SHIFT                   0
#define UART_IE7816_TXTE_MASK                    0x2u
#define UART_IE7816_TXTE_SHIFT                   1
#define UART_IE7816_GTVE_MASK                    0x4u
#define UART_IE7816_GTVE_SHIFT                   2
#define UART_IE7816_ADTE_MASK                    0x8u
#define UART_IE7816_ADTE_SHIFT                   3
#define UART_IE7816_INITDE_MASK                  0x10u
#define UART_IE7816_INITDE_SHIFT                 4
#define UART_IE7816_BWTE_MASK                    0x20u
#define UART_IE7816_BWTE_SHIFT                   5
#define UART_IE7816_CWTE_MASK                    0x40u
#define UART_IE7816_CWTE_SHIFT                   6
#define UART_IE7816_WTE_MASK                     0x80u
#define UART_IE7816_WTE_SHIFT                    7
/* IS7816 Bit Fields */
#define UART_IS7816_RXT_MASK                     0x1u
#define UART_IS7816_RXT_SHIFT                    0
#define UART_IS7816_TXT_MASK                     0x2u
#define UART_IS7816_TXT_SHIFT                    1
#define UART_IS7816_GTV_MASK                     0x4u
#define UART_IS7816_GTV_SHIFT                    2
#define UART_IS7816_ADT_MASK                     0x8u
#define UART_IS7816_ADT_SHIFT                    3
#define UART_IS7816_INITD_MASK                   0x10u
#define UART_IS7816_INITD_SHIFT                  4
#define UART_IS7816_BWT_MASK                     0x20u
#define UART_IS7816_BWT_SHIFT                    5
#define UART_IS7816_CWT_MASK                     0x40u
#define UART_IS7816_CWT_SHIFT                    6
#define UART_IS7816_WT_MASK                      0x80u
#define UART_IS7816_WT_SHIFT                     7
/* WP7816 Bit Fields */
#define UART_WP7816_WTX_MASK                     0xFFu
#define UART_WP7816_WTX_SHIFT                    0
#define UART_WP7816_WTX(x)                       (((uint8)(((uint8)(x))<<UART_WP7816_WTX_SHIFT))&UART_WP7816_WTX_MASK)
/* WN7816 Bit Fields */
#define UART_WN7816_GTN_MASK                     0xFFu
#define UART_WN7816_GTN_SHIFT                    0
#define UART_WN7816_GTN(x)                       (((uint8)(((uint8)(x))<<UART_WN7816_GTN_SHIFT))&UART_WN7816_GTN_MASK)
/* WF7816 Bit Fields */
#define UART_WF7816_GTFD_MASK                    0xFFu
#define UART_WF7816_GTFD_SHIFT                   0
#define UART_WF7816_GTFD(x)                      (((uint8)(((uint8)(x))<<UART_WF7816_GTFD_SHIFT))&UART_WF7816_GTFD_MASK)
/* ET7816 Bit Fields */
#define UART_ET7816_RXTHRESHOLD_MASK             0xFu
#define UART_ET7816_RXTHRESHOLD_SHIFT            0
#define UART_ET7816_RXTHRESHOLD(x)               (((uint8)(((uint8)(x))<<UART_ET7816_RXTHRESHOLD_SHIFT))&UART_ET7816_RXTHRESHOLD_MASK)
#define UART_ET7816_TXTHRESHOLD_MASK             0xF0u
#define UART_ET7816_TXTHRESHOLD_SHIFT            4
#define UART_ET7816_TXTHRESHOLD(x)               (((uint8)(((uint8)(x))<<UART_ET7816_TXTHRESHOLD_SHIFT))&UART_ET7816_TXTHRESHOLD_MASK)
/* TL7816 Bit Fields */
#define UART_TL7816_TLEN_MASK                    0xFFu
#define UART_TL7816_TLEN_SHIFT                   0
#define UART_TL7816_TLEN(x)                      (((uint8)(((uint8)(x))<<UART_TL7816_TLEN_SHIFT))&UART_TL7816_TLEN_MASK)
/* AP7816A_T0 Bit Fields */
#define UART_AP7816A_T0_ADTI_H_MASK              0xFFu
#define UART_AP7816A_T0_ADTI_H_SHIFT             0
#define UART_AP7816A_T0_ADTI_H(x)                (((uint8)(((uint8)(x))<<UART_AP7816A_T0_ADTI_H_SHIFT))&UART_AP7816A_T0_ADTI_H_MASK)
/* AP7816B_T0 Bit Fields */
#define UART_AP7816B_T0_ADTI_L_MASK              0xFFu
#define UART_AP7816B_T0_ADTI_L_SHIFT             0
#define UART_AP7816B_T0_ADTI_L(x)                (((uint8)(((uint8)(x))<<UART_AP7816B_T0_ADTI_L_SHIFT))&UART_AP7816B_T0_ADTI_L_MASK)
/* WP7816A_T0 Bit Fields */
#define UART_WP7816A_T0_WI_H_MASK                0xFFu
#define UART_WP7816A_T0_WI_H_SHIFT               0
#define UART_WP7816A_T0_WI_H(x)                  (((uint8)(((uint8)(x))<<UART_WP7816A_T0_WI_H_SHIFT))&UART_WP7816A_T0_WI_H_MASK)
/* WP7816B_T0 Bit Fields */
#define UART_WP7816B_T0_WI_L_MASK                0xFFu
#define UART_WP7816B_T0_WI_L_SHIFT               0
#define UART_WP7816B_T0_WI_L(x)                  (((uint8)(((uint8)(x))<<UART_WP7816B_T0_WI_L_SHIFT))&UART_WP7816B_T0_WI_L_MASK)
/* WP7816A_T1 Bit Fields */
#define UART_WP7816A_T1_BWI_H_MASK               0xFFu
#define UART_WP7816A_T1_BWI_H_SHIFT              0
#define UART_WP7816A_T1_BWI_H(x)                 (((uint8)(((uint8)(x))<<UART_WP7816A_T1_BWI_H_SHIFT))&UART_WP7816A_T1_BWI_H_MASK)
/* WP7816B_T1 Bit Fields */
#define UART_WP7816B_T1_BWI_L_MASK               0xFFu
#define UART_WP7816B_T1_BWI_L_SHIFT              0
#define UART_WP7816B_T1_BWI_L(x)                 (((uint8)(((uint8)(x))<<UART_WP7816B_T1_BWI_L_SHIFT))&UART_WP7816B_T1_BWI_L_MASK)
/* WGP7816_T1 Bit Fields */
#define UART_WGP7816_T1_BGI_MASK                 0xFu
#define UART_WGP7816_T1_BGI_SHIFT                0
#define UART_WGP7816_T1_BGI(x)                   (((uint8)(((uint8)(x))<<UART_WGP7816_T1_BGI_SHIFT))&UART_WGP7816_T1_BGI_MASK)
#define UART_WGP7816_T1_CWI1_MASK                0xF0u
#define UART_WGP7816_T1_CWI1_SHIFT               4
#define UART_WGP7816_T1_CWI1(x)                  (((uint8)(((uint8)(x))<<UART_WGP7816_T1_CWI1_SHIFT))&UART_WGP7816_T1_CWI1_MASK)
/* WP7816C_T1 Bit Fields */
#define UART_WP7816C_T1_CWI2_MASK                0x1Fu
#define UART_WP7816C_T1_CWI2_SHIFT               0
#define UART_WP7816C_T1_CWI2(x)                  (((uint8)(((uint8)(x))<<UART_WP7816C_T1_CWI2_SHIFT))&UART_WP7816C_T1_CWI2_MASK)






/* UART - Peripheral instance base addresses */
/** Peripheral UART0 base pointer */
#define UART0_BASE_PTR                           ((UART_MemMapPtr)0x4006A000u)
/** Peripheral UART1 base pointer */
#define UART1_BASE_PTR                           ((UART_MemMapPtr)0x4006B000u)
/** Peripheral UART2 base pointer */
#define UART2_BASE_PTR                           ((UART_MemMapPtr)0x4006C000u)
/** Array initializer of UART peripheral base pointers */
#define UART_BASE_PTRS                           { UART0_BASE_PTR, UART1_BASE_PTR, UART2_BASE_PTR }

/* ----------------------------------------------------------------------------
   -- UART - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UART_Register_Accessor_Macros UART - Register accessor macros
 * @{
 */


/* UART - Register instance definitions */
/* UART0 */
#define UART0_BDH                                UART_BDH_REG(UART0_BASE_PTR)
#define UART0_BDL                                UART_BDL_REG(UART0_BASE_PTR)
#define UART0_C1                                 UART_C1_REG(UART0_BASE_PTR)
#define UART0_C2                                 UART_C2_REG(UART0_BASE_PTR)
#define UART0_S1                                 UART_S1_REG(UART0_BASE_PTR)
#define UART0_S2                                 UART_S2_REG(UART0_BASE_PTR)
#define UART0_C3                                 UART_C3_REG(UART0_BASE_PTR)
#define UART0_D                                  UART_D_REG(UART0_BASE_PTR)
#define UART0_MA1                                UART_MA1_REG(UART0_BASE_PTR)
#define UART0_MA2                                UART_MA2_REG(UART0_BASE_PTR)
#define UART0_C4                                 UART_C4_REG(UART0_BASE_PTR)
#define UART0_C5                                 UART_C5_REG(UART0_BASE_PTR)
#define UART0_ED                                 UART_ED_REG(UART0_BASE_PTR)
#define UART0_MODEM                              UART_MODEM_REG(UART0_BASE_PTR)
#define UART0_IR                                 UART_IR_REG(UART0_BASE_PTR)
#define UART0_PFIFO                              UART_PFIFO_REG(UART0_BASE_PTR)
#define UART0_CFIFO                              UART_CFIFO_REG(UART0_BASE_PTR)
#define UART0_SFIFO                              UART_SFIFO_REG(UART0_BASE_PTR)
#define UART0_TWFIFO                             UART_TWFIFO_REG(UART0_BASE_PTR)
#define UART0_TCFIFO                             UART_TCFIFO_REG(UART0_BASE_PTR)
#define UART0_RWFIFO                             UART_RWFIFO_REG(UART0_BASE_PTR)
#define UART0_RCFIFO                             UART_RCFIFO_REG(UART0_BASE_PTR)
#define UART0_C7816                              UART_C7816_REG(UART0_BASE_PTR)
#define UART0_IE7816                             UART_IE7816_REG(UART0_BASE_PTR)
#define UART0_IS7816                             UART_IS7816_REG(UART0_BASE_PTR)
#define UART0_WP7816                             UART_WP7816_REG(UART0_BASE_PTR)
#define UART0_WN7816                             UART_WN7816_REG(UART0_BASE_PTR)
#define UART0_WF7816                             UART_WF7816_REG(UART0_BASE_PTR)
#define UART0_ET7816                             UART_ET7816_REG(UART0_BASE_PTR)
#define UART0_TL7816                             UART_TL7816_REG(UART0_BASE_PTR)
#define UART0_AP7816A_T0                         UART_AP7816A_T0_REG(UART0_BASE_PTR)
#define UART0_AP7816B_T0                         UART_AP7816B_T0_REG(UART0_BASE_PTR)
#define UART0_WP7816A_T0                         UART_WP7816A_T0_REG(UART0_BASE_PTR)
#define UART0_WP7816A_T1                         UART_WP7816A_T1_REG(UART0_BASE_PTR)
#define UART0_WP7816B_T0                         UART_WP7816B_T0_REG(UART0_BASE_PTR)
#define UART0_WP7816B_T1                         UART_WP7816B_T1_REG(UART0_BASE_PTR)
#define UART0_WGP7816_T1                         UART_WGP7816_T1_REG(UART0_BASE_PTR)
#define UART0_WP7816C_T1                         UART_WP7816C_T1_REG(UART0_BASE_PTR)
/* UART1 */
#define UART1_BDH                                UART_BDH_REG(UART1_BASE_PTR)
#define UART1_BDL                                UART_BDL_REG(UART1_BASE_PTR)
#define UART1_C1                                 UART_C1_REG(UART1_BASE_PTR)
#define UART1_C2                                 UART_C2_REG(UART1_BASE_PTR)
#define UART1_S1                                 UART_S1_REG(UART1_BASE_PTR)
#define UART1_S2                                 UART_S2_REG(UART1_BASE_PTR)
#define UART1_C3                                 UART_C3_REG(UART1_BASE_PTR)
#define UART1_D                                  UART_D_REG(UART1_BASE_PTR)
#define UART1_MA1                                UART_MA1_REG(UART1_BASE_PTR)
#define UART1_MA2                                UART_MA2_REG(UART1_BASE_PTR)
#define UART1_C4                                 UART_C4_REG(UART1_BASE_PTR)
#define UART1_C5                                 UART_C5_REG(UART1_BASE_PTR)
#define UART1_ED                                 UART_ED_REG(UART1_BASE_PTR)
#define UART1_MODEM                              UART_MODEM_REG(UART1_BASE_PTR)
#define UART1_IR                                 UART_IR_REG(UART1_BASE_PTR)
#define UART1_PFIFO                              UART_PFIFO_REG(UART1_BASE_PTR)
#define UART1_CFIFO                              UART_CFIFO_REG(UART1_BASE_PTR)
#define UART1_SFIFO                              UART_SFIFO_REG(UART1_BASE_PTR)
#define UART1_TWFIFO                             UART_TWFIFO_REG(UART1_BASE_PTR)
#define UART1_TCFIFO                             UART_TCFIFO_REG(UART1_BASE_PTR)
#define UART1_RWFIFO                             UART_RWFIFO_REG(UART1_BASE_PTR)
#define UART1_RCFIFO                             UART_RCFIFO_REG(UART1_BASE_PTR)
/* UART2 */
#define UART2_BDH                                UART_BDH_REG(UART2_BASE_PTR)
#define UART2_BDL                                UART_BDL_REG(UART2_BASE_PTR)
#define UART2_C1                                 UART_C1_REG(UART2_BASE_PTR)
#define UART2_C2                                 UART_C2_REG(UART2_BASE_PTR)
#define UART2_S1                                 UART_S1_REG(UART2_BASE_PTR)
#define UART2_S2                                 UART_S2_REG(UART2_BASE_PTR)
#define UART2_C3                                 UART_C3_REG(UART2_BASE_PTR)
#define UART2_D                                  UART_D_REG(UART2_BASE_PTR)
#define UART2_MA1                                UART_MA1_REG(UART2_BASE_PTR)
#define UART2_MA2                                UART_MA2_REG(UART2_BASE_PTR)
#define UART2_C4                                 UART_C4_REG(UART2_BASE_PTR)
#define UART2_C5                                 UART_C5_REG(UART2_BASE_PTR)
#define UART2_ED                                 UART_ED_REG(UART2_BASE_PTR)
#define UART2_MODEM                              UART_MODEM_REG(UART2_BASE_PTR)
#define UART2_IR                                 UART_IR_REG(UART2_BASE_PTR)
#define UART2_PFIFO                              UART_PFIFO_REG(UART2_BASE_PTR)
#define UART2_CFIFO                              UART_CFIFO_REG(UART2_BASE_PTR)
#define UART2_SFIFO                              UART_SFIFO_REG(UART2_BASE_PTR)
#define UART2_TWFIFO                             UART_TWFIFO_REG(UART2_BASE_PTR)
#define UART2_TCFIFO                             UART_TCFIFO_REG(UART2_BASE_PTR)
#define UART2_RWFIFO                             UART_RWFIFO_REG(UART2_BASE_PTR)
#define UART2_RCFIFO                             UART_RCFIFO_REG(UART2_BASE_PTR)


// ============================================================
// -------------------- PERIPHERAL SECTION --------------------
// ------------------------------------------------------------
//============================================================

/* ----------------------------------------------------------------------------
   -- DMA
   ---------------------------------------------------------------------------- */
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Structure for the DMA hardware request
 *
 * Defines the structure for the DMA hardware request collections. The user can configure the
 * hardware request into DMAMUX to trigger the DMA transfer accordingly. The index
 * of the hardware request varies according  to the to SoC.
 */
typedef enum _dma_request_source
{
    kDmaRequestMux0Disable          = 0|0x100U,    /**< DMAMUX TriggerDisabled. */
    kDmaRequestMux0Reserved1        = 1|0x100U,    /**< Reserved1 */
    kDmaRequestMux0UART0Rx          = 2|0x100U,    /**< UART0 Receive. */
    kDmaRequestMux0UART0Tx          = 3|0x100U,    /**< UART0 Transmit. */
    kDmaRequestMux0UART1Rx          = 4|0x100U,    /**< UART1 Receive. */
    kDmaRequestMux0UART1Tx          = 5|0x100U,    /**< UART1 Transmit. */
    kDmaRequestMux0UART2Rx          = 6|0x100U,    /**< UART2 Receive. */
    kDmaRequestMux0UART2Tx          = 7|0x100U,    /**< UART2 Transmit. */
    kDmaRequestMux0Reserved8        = 8|0x100U,    /**< Reserved8 */
    kDmaRequestMux0Reserved9        = 9|0x100U,    /**< Reserved9 */
    kDmaRequestMux0Reserved10       = 10|0x100U,   /**< Reserved10 */
    kDmaRequestMux0Reserved11       = 11|0x100U,   /**< Reserved11 */
    kDmaRequestMux0Reserved12       = 12|0x100U,   /**< Reserved12 */
    kDmaRequestMux0Reserved13       = 13|0x100U,   /**< Reserved13 */
    kDmaRequestMux0SPI0Rx           = 14|0x100U,   /**< SPI0 Receive. */
    kDmaRequestMux0SPI0Tx           = 15|0x100U,   /**< SPI0 Transmit. */
    kDmaRequestMux0SPI1             = 16|0x100U,   /**< SPI1 Transmit or Receive. */
    kDmaRequestMux0Reserved17       = 17|0x100U,   /**< Reserved17 */
    kDmaRequestMux0I2C0             = 18|0x100U,   /**< I2C0. */
    kDmaRequestMux0I2C1             = 19|0x100U,   /**< I2C1. */
    kDmaRequestMux0FTM0Channel0     = 20|0x100U,   /**< FTM0 C0V. */
    kDmaRequestMux0FTM0Channel1     = 21|0x100U,   /**< FTM0 C1V. */
    kDmaRequestMux0FTM0Channel2     = 22|0x100U,   /**< FTM0 C2V. */
    kDmaRequestMux0FTM0Channel3     = 23|0x100U,   /**< FTM0 C3V. */
    kDmaRequestMux0FTM0Channel4     = 24|0x100U,   /**< FTM0 C4V. */
    kDmaRequestMux0FTM0Channel5     = 25|0x100U,   /**< FTM0 C5V. */
    kDmaRequestMux0FTM0Channel6     = 26|0x100U,   /**< FTM0 C6V. */
    kDmaRequestMux0FTM0Channel7     = 27|0x100U,   /**< FTM0 C7V. */
    kDmaRequestMux0FTM1Channel0     = 28|0x100U,   /**< FTM1 C0V. */
    kDmaRequestMux0FTM1Channel1     = 29|0x100U,   /**< FTM1 C1V. */
    kDmaRequestMux0FTM2Channel0     = 30|0x100U,   /**< FTM2 C0V. */
    kDmaRequestMux0FTM2Channel1     = 31|0x100U,   /**< FTM2 C1V. */
    kDmaRequestMux0FTM3Channel0     = 32|0x100U,   /**< FTM3 C0V. */
    kDmaRequestMux0FTM3Channel1     = 33|0x100U,   /**< FTM3 C1V. */
    kDmaRequestMux0FTM3Channel2     = 34|0x100U,   /**< FTM3 C2V. */
    kDmaRequestMux0FTM3Channel3     = 35|0x100U,   /**< FTM3 C3V. */
    kDmaRequestMux0FTM3Channel4     = 36|0x100U,   /**< FTM3 C4V. */
    kDmaRequestMux0FTM3Channel5     = 37|0x100U,   /**< FTM3 C5V. */
    kDmaRequestMux0FTM3Channel6     = 38|0x100U,   /**< FTM3 C6V. */
    kDmaRequestMux0FTM3Channel7     = 39|0x100U,   /**< FTM3 C7V. */
    kDmaRequestMux0ADC0             = 40|0x100U,   /**< ADC0. */
    kDmaRequestMux0ADC1             = 41|0x100U,   /**< ADC1. */
    kDmaRequestMux0CMP0             = 42|0x100U,   /**< CMP0. */
    kDmaRequestMux0CMP1             = 43|0x100U,   /**< CMP1. */
    kDmaRequestMux0Reserved44       = 44|0x100U,   /**< Reserved44 */
    kDmaRequestMux0DAC0             = 45|0x100U,   /**< DAC0. */
    kDmaRequestMux0DAC1             = 46|0x100U,   /**< DAC1. */
    kDmaRequestMux0Reserved47       = 47|0x100U,   /**< Reserved47 */
    kDmaRequestMux0PDB              = 48|0x100U,   /**< PDB0. */
    kDmaRequestMux0PortA            = 49|0x100U,   /**< PTA. */
    kDmaRequestMux0PortB            = 50|0x100U,   /**< PTB. */
    kDmaRequestMux0PortC            = 51|0x100U,   /**< PTC. */
    kDmaRequestMux0PortD            = 52|0x100U,   /**< PTD. */
    kDmaRequestMux0PortE            = 53|0x100U,   /**< PTE. */
    kDmaRequestMux0Reserved54       = 54|0x100U,   /**< Reserved54 */
    kDmaRequestMux0Reserved55       = 55|0x100U,   /**< Reserved55 */
    kDmaRequestMux0Reserved56       = 56|0x100U,   /**< Reserved56 */
    kDmaRequestMux0Reserved57       = 57|0x100U,   /**< Reserved57 */
    kDmaRequestMux0LPUART0Rx        = 58|0x100U,   /**< LPUART0 Receive. */
    kDmaRequestMux0LPUART0Tx        = 59|0x100U,   /**< LPUART0 Transmit. */
    kDmaRequestMux0AlwaysOn60       = 60|0x100U,   /**< DMAMUX Always Enabled slot. */
    kDmaRequestMux0AlwaysOn61       = 61|0x100U,   /**< DMAMUX Always Enabled slot. */
    kDmaRequestMux0AlwaysOn62       = 62|0x100U,   /**< DMAMUX Always Enabled slot. */
    kDmaRequestMux0AlwaysOn63       = 63|0x100U,   /**< DMAMUX Always Enabled slot. */
} dma_request_source_t;


/* ----------------------------------------------------------------------------
   -- DMA Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DMA_Peripheral_Access_Layer DMA Peripheral Access Layer
 * @{
 */

/** DMA - Register Layout Typedef */
typedef struct {
  volatile uint32 CR;                                /**< Control Register, offset: 0x0 */
  volatile  uint32 ES;                                /**< Error Status Register, offset: 0x4 */
       uint8 RESERVED_0[4];
  volatile uint32 ERQ;                               /**< Enable Request Register, offset: 0xC */
       uint8 RESERVED_1[4];
  volatile uint32 EEI;                               /**< Enable Error Interrupt Register, offset: 0x14 */
  volatile  uint8 CEEI;                               /**< Clear Enable Error Interrupt Register, offset: 0x18 */
  volatile  uint8 SEEI;                               /**< Set Enable Error Interrupt Register, offset: 0x19 */
  volatile  uint8 CERQ;                               /**< Clear Enable Request Register, offset: 0x1A */
  volatile  uint8 SERQ;                               /**< Set Enable Request Register, offset: 0x1B */
  volatile  uint8 CDNE;                               /**< Clear DONE Status Bit Register, offset: 0x1C */
  volatile  uint8 SSRT;                               /**< Set START Bit Register, offset: 0x1D */
  volatile  uint8 CERR;                               /**< Clear Error Register, offset: 0x1E */
  volatile  uint8 CINT;                               /**< Clear Interrupt Request Register, offset: 0x1F */
       uint8 RESERVED_2[4];
  volatile uint32 INT;                               /**< Interrupt Request Register, offset: 0x24 */
       uint8 RESERVED_3[4];
  volatile uint32 ERR;                               /**< Error Register, offset: 0x2C */
       uint8 RESERVED_4[4];
  volatile  uint32 HRS;                               /**< Hardware Request Status Register, offset: 0x34 */
       uint8 RESERVED_5[12];
  volatile uint32 EARS;                              /**< Enable Asynchronous Request in Stop Register, offset: 0x44 */
       uint8 RESERVED_6[184];
  volatile uint8 DCHPRI3;                            /**< Channel n Priority Register, offset: 0x100 */
  volatile uint8 DCHPRI2;                            /**< Channel n Priority Register, offset: 0x101 */
  volatile uint8 DCHPRI1;                            /**< Channel n Priority Register, offset: 0x102 */
  volatile uint8 DCHPRI0;                            /**< Channel n Priority Register, offset: 0x103 */
  volatile uint8 DCHPRI7;                            /**< Channel n Priority Register, offset: 0x104 */
  volatile uint8 DCHPRI6;                            /**< Channel n Priority Register, offset: 0x105 */
  volatile uint8 DCHPRI5;                            /**< Channel n Priority Register, offset: 0x106 */
  volatile uint8 DCHPRI4;                            /**< Channel n Priority Register, offset: 0x107 */
  volatile uint8 DCHPRI11;                           /**< Channel n Priority Register, offset: 0x108 */
  volatile uint8 DCHPRI10;                           /**< Channel n Priority Register, offset: 0x109 */
  volatile uint8 DCHPRI9;                            /**< Channel n Priority Register, offset: 0x10A */
  volatile uint8 DCHPRI8;                            /**< Channel n Priority Register, offset: 0x10B */
  volatile uint8 DCHPRI15;                           /**< Channel n Priority Register, offset: 0x10C */
  volatile uint8 DCHPRI14;                           /**< Channel n Priority Register, offset: 0x10D */
  volatile uint8 DCHPRI13;                           /**< Channel n Priority Register, offset: 0x10E */
  volatile uint8 DCHPRI12;                           /**< Channel n Priority Register, offset: 0x10F */
       uint8 RESERVED_7[3824];
  struct {                                         /* offset: 0x1000, array step: 0x20 */
    volatile uint32 SADDR;                             /**< TCD Source Address, array offset: 0x1000, array step: 0x20 */
    volatile uint16 SOFF;                              /**< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
    volatile uint16 ATTR;                              /**< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
    union {                                          /* offset: 0x1008, array step: 0x20 */
      volatile uint32 NBYTES_MLNO;                       /**< TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20 */
      volatile uint32 NBYTES_MLOFFNO;                    /**< TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
      volatile uint32 NBYTES_MLOFFYES;                   /**< TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20 */
    };
    volatile uint32 SLAST;                             /**< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
    volatile uint32 DADDR;                             /**< TCD Destination Address, array offset: 0x1010, array step: 0x20 */
    volatile uint16 DOFF;                              /**< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
    union {                                          /* offset: 0x1016, array step: 0x20 */
      volatile uint16 CITER_ELINKNO;                     /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
      volatile uint16 CITER_ELINKYES;                    /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
    };
    volatile uint32 DLAST_SGA;                         /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
    volatile uint16 CSR;                               /**< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
    union {                                          /* offset: 0x101E, array step: 0x20 */
      volatile uint16 BITER_ELINKNO;                     /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
      volatile uint16 BITER_ELINKYES;                    /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
    };
  } TCD[16];
} DMA_TYPE;

/* ----------------------------------------------------------------------------
   -- DMA Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DMA_Register_Masks DMA Register Masks
 * @{
 */

/*! @name CR - Control Register */
#define DMA_CR_EDBG_MASK                         (0x2U)
#define DMA_CR_EDBG_SHIFT                        (1U)
#define DMA_CR_EDBG(x)                           (((uint32)(((uint32)(x)) << DMA_CR_EDBG_SHIFT)) & DMA_CR_EDBG_MASK)
#define DMA_CR_ERCA_MASK                         (0x4U)
#define DMA_CR_ERCA_SHIFT                        (2U)
#define DMA_CR_ERCA(x)                           (((uint32)(((uint32)(x)) << DMA_CR_ERCA_SHIFT)) & DMA_CR_ERCA_MASK)
#define DMA_CR_HOE_MASK                          (0x10U)
#define DMA_CR_HOE_SHIFT                         (4U)
#define DMA_CR_HOE(x)                            (((uint32)(((uint32)(x)) << DMA_CR_HOE_SHIFT)) & DMA_CR_HOE_MASK)
#define DMA_CR_HALT_MASK                         (0x20U)
#define DMA_CR_HALT_SHIFT                        (5U)
#define DMA_CR_HALT(x)                           (((uint32)(((uint32)(x)) << DMA_CR_HALT_SHIFT)) & DMA_CR_HALT_MASK)
#define DMA_CR_CLM_MASK                          (0x40U)
#define DMA_CR_CLM_SHIFT                         (6U)
#define DMA_CR_CLM(x)                            (((uint32)(((uint32)(x)) << DMA_CR_CLM_SHIFT)) & DMA_CR_CLM_MASK)
#define DMA_CR_EMLM_MASK                         (0x80U)
#define DMA_CR_EMLM_SHIFT                        (7U)
#define DMA_CR_EMLM(x)                           (((uint32)(((uint32)(x)) << DMA_CR_EMLM_SHIFT)) & DMA_CR_EMLM_MASK)
#define DMA_CR_ECX_MASK                          (0x10000U)
#define DMA_CR_ECX_SHIFT                         (16U)
#define DMA_CR_ECX(x)                            (((uint32)(((uint32)(x)) << DMA_CR_ECX_SHIFT)) & DMA_CR_ECX_MASK)
#define DMA_CR_CX_MASK                           (0x20000U)
#define DMA_CR_CX_SHIFT                          (17U)
#define DMA_CR_CX(x)                             (((uint32)(((uint32)(x)) << DMA_CR_CX_SHIFT)) & DMA_CR_CX_MASK)

/*! @name ES - Error Status Register */
#define DMA_ES_DBE_MASK                          (0x1U)
#define DMA_ES_DBE_SHIFT                         (0U)
#define DMA_ES_DBE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_DBE_SHIFT)) & DMA_ES_DBE_MASK)
#define DMA_ES_SBE_MASK                          (0x2U)
#define DMA_ES_SBE_SHIFT                         (1U)
#define DMA_ES_SBE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_SBE_SHIFT)) & DMA_ES_SBE_MASK)
#define DMA_ES_SGE_MASK                          (0x4U)
#define DMA_ES_SGE_SHIFT                         (2U)
#define DMA_ES_SGE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_SGE_SHIFT)) & DMA_ES_SGE_MASK)
#define DMA_ES_NCE_MASK                          (0x8U)
#define DMA_ES_NCE_SHIFT                         (3U)
#define DMA_ES_NCE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_NCE_SHIFT)) & DMA_ES_NCE_MASK)
#define DMA_ES_DOE_MASK                          (0x10U)
#define DMA_ES_DOE_SHIFT                         (4U)
#define DMA_ES_DOE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_DOE_SHIFT)) & DMA_ES_DOE_MASK)
#define DMA_ES_DAE_MASK                          (0x20U)
#define DMA_ES_DAE_SHIFT                         (5U)
#define DMA_ES_DAE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_DAE_SHIFT)) & DMA_ES_DAE_MASK)
#define DMA_ES_SOE_MASK                          (0x40U)
#define DMA_ES_SOE_SHIFT                         (6U)
#define DMA_ES_SOE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_SOE_SHIFT)) & DMA_ES_SOE_MASK)
#define DMA_ES_SAE_MASK                          (0x80U)
#define DMA_ES_SAE_SHIFT                         (7U)
#define DMA_ES_SAE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_SAE_SHIFT)) & DMA_ES_SAE_MASK)
#define DMA_ES_ERRCHN_MASK                       (0xF00U)
#define DMA_ES_ERRCHN_SHIFT                      (8U)
#define DMA_ES_ERRCHN(x)                         (((uint32)(((uint32)(x)) << DMA_ES_ERRCHN_SHIFT)) & DMA_ES_ERRCHN_MASK)
#define DMA_ES_CPE_MASK                          (0x4000U)
#define DMA_ES_CPE_SHIFT                         (14U)
#define DMA_ES_CPE(x)                            (((uint32)(((uint32)(x)) << DMA_ES_CPE_SHIFT)) & DMA_ES_CPE_MASK)
#define DMA_ES_ECX_MASK                          (0x10000U)
#define DMA_ES_ECX_SHIFT                         (16U)
#define DMA_ES_ECX(x)                            (((uint32)(((uint32)(x)) << DMA_ES_ECX_SHIFT)) & DMA_ES_ECX_MASK)
#define DMA_ES_VLD_MASK                          (0x80000000U)
#define DMA_ES_VLD_SHIFT                         (31U)
#define DMA_ES_VLD(x)                            (((uint32)(((uint32)(x)) << DMA_ES_VLD_SHIFT)) & DMA_ES_VLD_MASK)

/*! @name ERQ - Enable Request Register */
#define DMA_ERQ_ERQ0_MASK                        (0x1U)
#define DMA_ERQ_ERQ0_SHIFT                       (0U)
#define DMA_ERQ_ERQ0(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ0_SHIFT)) & DMA_ERQ_ERQ0_MASK)
#define DMA_ERQ_ERQ1_MASK                        (0x2U)
#define DMA_ERQ_ERQ1_SHIFT                       (1U)
#define DMA_ERQ_ERQ1(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ1_SHIFT)) & DMA_ERQ_ERQ1_MASK)
#define DMA_ERQ_ERQ2_MASK                        (0x4U)
#define DMA_ERQ_ERQ2_SHIFT                       (2U)
#define DMA_ERQ_ERQ2(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ2_SHIFT)) & DMA_ERQ_ERQ2_MASK)
#define DMA_ERQ_ERQ3_MASK                        (0x8U)
#define DMA_ERQ_ERQ3_SHIFT                       (3U)
#define DMA_ERQ_ERQ3(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ3_SHIFT)) & DMA_ERQ_ERQ3_MASK)
#define DMA_ERQ_ERQ4_MASK                        (0x10U)
#define DMA_ERQ_ERQ4_SHIFT                       (4U)
#define DMA_ERQ_ERQ4(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ4_SHIFT)) & DMA_ERQ_ERQ4_MASK)
#define DMA_ERQ_ERQ5_MASK                        (0x20U)
#define DMA_ERQ_ERQ5_SHIFT                       (5U)
#define DMA_ERQ_ERQ5(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ5_SHIFT)) & DMA_ERQ_ERQ5_MASK)
#define DMA_ERQ_ERQ6_MASK                        (0x40U)
#define DMA_ERQ_ERQ6_SHIFT                       (6U)
#define DMA_ERQ_ERQ6(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ6_SHIFT)) & DMA_ERQ_ERQ6_MASK)
#define DMA_ERQ_ERQ7_MASK                        (0x80U)
#define DMA_ERQ_ERQ7_SHIFT                       (7U)
#define DMA_ERQ_ERQ7(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ7_SHIFT)) & DMA_ERQ_ERQ7_MASK)
#define DMA_ERQ_ERQ8_MASK                        (0x100U)
#define DMA_ERQ_ERQ8_SHIFT                       (8U)
#define DMA_ERQ_ERQ8(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ8_SHIFT)) & DMA_ERQ_ERQ8_MASK)
#define DMA_ERQ_ERQ9_MASK                        (0x200U)
#define DMA_ERQ_ERQ9_SHIFT                       (9U)
#define DMA_ERQ_ERQ9(x)                          (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ9_SHIFT)) & DMA_ERQ_ERQ9_MASK)
#define DMA_ERQ_ERQ10_MASK                       (0x400U)
#define DMA_ERQ_ERQ10_SHIFT                      (10U)
#define DMA_ERQ_ERQ10(x)                         (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ10_SHIFT)) & DMA_ERQ_ERQ10_MASK)
#define DMA_ERQ_ERQ11_MASK                       (0x800U)
#define DMA_ERQ_ERQ11_SHIFT                      (11U)
#define DMA_ERQ_ERQ11(x)                         (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ11_SHIFT)) & DMA_ERQ_ERQ11_MASK)
#define DMA_ERQ_ERQ12_MASK                       (0x1000U)
#define DMA_ERQ_ERQ12_SHIFT                      (12U)
#define DMA_ERQ_ERQ12(x)                         (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ12_SHIFT)) & DMA_ERQ_ERQ12_MASK)
#define DMA_ERQ_ERQ13_MASK                       (0x2000U)
#define DMA_ERQ_ERQ13_SHIFT                      (13U)
#define DMA_ERQ_ERQ13(x)                         (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ13_SHIFT)) & DMA_ERQ_ERQ13_MASK)
#define DMA_ERQ_ERQ14_MASK                       (0x4000U)
#define DMA_ERQ_ERQ14_SHIFT                      (14U)
#define DMA_ERQ_ERQ14(x)                         (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ14_SHIFT)) & DMA_ERQ_ERQ14_MASK)
#define DMA_ERQ_ERQ15_MASK                       (0x8000U)
#define DMA_ERQ_ERQ15_SHIFT                      (15U)
#define DMA_ERQ_ERQ15(x)                         (((uint32)(((uint32)(x)) << DMA_ERQ_ERQ15_SHIFT)) & DMA_ERQ_ERQ15_MASK)

/*! @name EEI - Enable Error Interrupt Register */
#define DMA_EEI_EEI0_MASK                        (0x1U)
#define DMA_EEI_EEI0_SHIFT                       (0U)
#define DMA_EEI_EEI0(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI0_SHIFT)) & DMA_EEI_EEI0_MASK)
#define DMA_EEI_EEI1_MASK                        (0x2U)
#define DMA_EEI_EEI1_SHIFT                       (1U)
#define DMA_EEI_EEI1(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI1_SHIFT)) & DMA_EEI_EEI1_MASK)
#define DMA_EEI_EEI2_MASK                        (0x4U)
#define DMA_EEI_EEI2_SHIFT                       (2U)
#define DMA_EEI_EEI2(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI2_SHIFT)) & DMA_EEI_EEI2_MASK)
#define DMA_EEI_EEI3_MASK                        (0x8U)
#define DMA_EEI_EEI3_SHIFT                       (3U)
#define DMA_EEI_EEI3(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI3_SHIFT)) & DMA_EEI_EEI3_MASK)
#define DMA_EEI_EEI4_MASK                        (0x10U)
#define DMA_EEI_EEI4_SHIFT                       (4U)
#define DMA_EEI_EEI4(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI4_SHIFT)) & DMA_EEI_EEI4_MASK)
#define DMA_EEI_EEI5_MASK                        (0x20U)
#define DMA_EEI_EEI5_SHIFT                       (5U)
#define DMA_EEI_EEI5(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI5_SHIFT)) & DMA_EEI_EEI5_MASK)
#define DMA_EEI_EEI6_MASK                        (0x40U)
#define DMA_EEI_EEI6_SHIFT                       (6U)
#define DMA_EEI_EEI6(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI6_SHIFT)) & DMA_EEI_EEI6_MASK)
#define DMA_EEI_EEI7_MASK                        (0x80U)
#define DMA_EEI_EEI7_SHIFT                       (7U)
#define DMA_EEI_EEI7(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI7_SHIFT)) & DMA_EEI_EEI7_MASK)
#define DMA_EEI_EEI8_MASK                        (0x100U)
#define DMA_EEI_EEI8_SHIFT                       (8U)
#define DMA_EEI_EEI8(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI8_SHIFT)) & DMA_EEI_EEI8_MASK)
#define DMA_EEI_EEI9_MASK                        (0x200U)
#define DMA_EEI_EEI9_SHIFT                       (9U)
#define DMA_EEI_EEI9(x)                          (((uint32)(((uint32)(x)) << DMA_EEI_EEI9_SHIFT)) & DMA_EEI_EEI9_MASK)
#define DMA_EEI_EEI10_MASK                       (0x400U)
#define DMA_EEI_EEI10_SHIFT                      (10U)
#define DMA_EEI_EEI10(x)                         (((uint32)(((uint32)(x)) << DMA_EEI_EEI10_SHIFT)) & DMA_EEI_EEI10_MASK)
#define DMA_EEI_EEI11_MASK                       (0x800U)
#define DMA_EEI_EEI11_SHIFT                      (11U)
#define DMA_EEI_EEI11(x)                         (((uint32)(((uint32)(x)) << DMA_EEI_EEI11_SHIFT)) & DMA_EEI_EEI11_MASK)
#define DMA_EEI_EEI12_MASK                       (0x1000U)
#define DMA_EEI_EEI12_SHIFT                      (12U)
#define DMA_EEI_EEI12(x)                         (((uint32)(((uint32)(x)) << DMA_EEI_EEI12_SHIFT)) & DMA_EEI_EEI12_MASK)
#define DMA_EEI_EEI13_MASK                       (0x2000U)
#define DMA_EEI_EEI13_SHIFT                      (13U)
#define DMA_EEI_EEI13(x)                         (((uint32)(((uint32)(x)) << DMA_EEI_EEI13_SHIFT)) & DMA_EEI_EEI13_MASK)
#define DMA_EEI_EEI14_MASK                       (0x4000U)
#define DMA_EEI_EEI14_SHIFT                      (14U)
#define DMA_EEI_EEI14(x)                         (((uint32)(((uint32)(x)) << DMA_EEI_EEI14_SHIFT)) & DMA_EEI_EEI14_MASK)
#define DMA_EEI_EEI15_MASK                       (0x8000U)
#define DMA_EEI_EEI15_SHIFT                      (15U)
#define DMA_EEI_EEI15(x)                         (((uint32)(((uint32)(x)) << DMA_EEI_EEI15_SHIFT)) & DMA_EEI_EEI15_MASK)

/*! @name CEEI - Clear Enable Error Interrupt Register */
#define DMA_CEEI_CEEI_MASK                       (0xFU)
#define DMA_CEEI_CEEI_SHIFT                      (0U)
#define DMA_CEEI_CEEI(x)                         (((uint8)(((uint8)(x)) << DMA_CEEI_CEEI_SHIFT)) & DMA_CEEI_CEEI_MASK)
#define DMA_CEEI_CAEE_MASK                       (0x40U)
#define DMA_CEEI_CAEE_SHIFT                      (6U)
#define DMA_CEEI_CAEE(x)                         (((uint8)(((uint8)(x)) << DMA_CEEI_CAEE_SHIFT)) & DMA_CEEI_CAEE_MASK)
#define DMA_CEEI_NOP_MASK                        (0x80U)
#define DMA_CEEI_NOP_SHIFT                       (7U)
#define DMA_CEEI_NOP(x)                          (((uint8)(((uint8)(x)) << DMA_CEEI_NOP_SHIFT)) & DMA_CEEI_NOP_MASK)

/*! @name SEEI - Set Enable Error Interrupt Register */
#define DMA_SEEI_SEEI_MASK                       (0xFU)
#define DMA_SEEI_SEEI_SHIFT                      (0U)
#define DMA_SEEI_SEEI(x)                         (((uint8)(((uint8)(x)) << DMA_SEEI_SEEI_SHIFT)) & DMA_SEEI_SEEI_MASK)
#define DMA_SEEI_SAEE_MASK                       (0x40U)
#define DMA_SEEI_SAEE_SHIFT                      (6U)
#define DMA_SEEI_SAEE(x)                         (((uint8)(((uint8)(x)) << DMA_SEEI_SAEE_SHIFT)) & DMA_SEEI_SAEE_MASK)
#define DMA_SEEI_NOP_MASK                        (0x80U)
#define DMA_SEEI_NOP_SHIFT                       (7U)
#define DMA_SEEI_NOP(x)                          (((uint8)(((uint8)(x)) << DMA_SEEI_NOP_SHIFT)) & DMA_SEEI_NOP_MASK)

/*! @name CERQ - Clear Enable Request Register */
#define DMA_CERQ_CERQ_MASK                       (0xFU)
#define DMA_CERQ_CERQ_SHIFT                      (0U)
#define DMA_CERQ_CERQ(x)                         (((uint8)(((uint8)(x)) << DMA_CERQ_CERQ_SHIFT)) & DMA_CERQ_CERQ_MASK)
#define DMA_CERQ_CAER_MASK                       (0x40U)
#define DMA_CERQ_CAER_SHIFT                      (6U)
#define DMA_CERQ_CAER(x)                         (((uint8)(((uint8)(x)) << DMA_CERQ_CAER_SHIFT)) & DMA_CERQ_CAER_MASK)
#define DMA_CERQ_NOP_MASK                        (0x80U)
#define DMA_CERQ_NOP_SHIFT                       (7U)
#define DMA_CERQ_NOP(x)                          (((uint8)(((uint8)(x)) << DMA_CERQ_NOP_SHIFT)) & DMA_CERQ_NOP_MASK)

/*! @name SERQ - Set Enable Request Register */
#define DMA_SERQ_SERQ_MASK                       (0xFU)
#define DMA_SERQ_SERQ_SHIFT                      (0U)
#define DMA_SERQ_SERQ(x)                         (((uint8)(((uint8)(x)) << DMA_SERQ_SERQ_SHIFT)) & DMA_SERQ_SERQ_MASK)
#define DMA_SERQ_SAER_MASK                       (0x40U)
#define DMA_SERQ_SAER_SHIFT                      (6U)
#define DMA_SERQ_SAER(x)                         (((uint8)(((uint8)(x)) << DMA_SERQ_SAER_SHIFT)) & DMA_SERQ_SAER_MASK)
#define DMA_SERQ_NOP_MASK                        (0x80U)
#define DMA_SERQ_NOP_SHIFT                       (7U)
#define DMA_SERQ_NOP(x)                          (((uint8)(((uint8)(x)) << DMA_SERQ_NOP_SHIFT)) & DMA_SERQ_NOP_MASK)

/*! @name CDNE - Clear DONE Status Bit Register */
#define DMA_CDNE_CDNE_MASK                       (0xFU)
#define DMA_CDNE_CDNE_SHIFT                      (0U)
#define DMA_CDNE_CDNE(x)                         (((uint8)(((uint8)(x)) << DMA_CDNE_CDNE_SHIFT)) & DMA_CDNE_CDNE_MASK)
#define DMA_CDNE_CADN_MASK                       (0x40U)
#define DMA_CDNE_CADN_SHIFT                      (6U)
#define DMA_CDNE_CADN(x)                         (((uint8)(((uint8)(x)) << DMA_CDNE_CADN_SHIFT)) & DMA_CDNE_CADN_MASK)
#define DMA_CDNE_NOP_MASK                        (0x80U)
#define DMA_CDNE_NOP_SHIFT                       (7U)
#define DMA_CDNE_NOP(x)                          (((uint8)(((uint8)(x)) << DMA_CDNE_NOP_SHIFT)) & DMA_CDNE_NOP_MASK)

/*! @name SSRT - Set START Bit Register */
#define DMA_SSRT_SSRT_MASK                       (0xFU)
#define DMA_SSRT_SSRT_SHIFT                      (0U)
#define DMA_SSRT_SSRT(x)                         (((uint8)(((uint8)(x)) << DMA_SSRT_SSRT_SHIFT)) & DMA_SSRT_SSRT_MASK)
#define DMA_SSRT_SAST_MASK                       (0x40U)
#define DMA_SSRT_SAST_SHIFT                      (6U)
#define DMA_SSRT_SAST(x)                         (((uint8)(((uint8)(x)) << DMA_SSRT_SAST_SHIFT)) & DMA_SSRT_SAST_MASK)
#define DMA_SSRT_NOP_MASK                        (0x80U)
#define DMA_SSRT_NOP_SHIFT                       (7U)
#define DMA_SSRT_NOP(x)                          (((uint8)(((uint8)(x)) << DMA_SSRT_NOP_SHIFT)) & DMA_SSRT_NOP_MASK)

/*! @name CERR - Clear Error Register */
#define DMA_CERR_CERR_MASK                       (0xFU)
#define DMA_CERR_CERR_SHIFT                      (0U)
#define DMA_CERR_CERR(x)                         (((uint8)(((uint8)(x)) << DMA_CERR_CERR_SHIFT)) & DMA_CERR_CERR_MASK)
#define DMA_CERR_CAEI_MASK                       (0x40U)
#define DMA_CERR_CAEI_SHIFT                      (6U)
#define DMA_CERR_CAEI(x)                         (((uint8)(((uint8)(x)) << DMA_CERR_CAEI_SHIFT)) & DMA_CERR_CAEI_MASK)
#define DMA_CERR_NOP_MASK                        (0x80U)
#define DMA_CERR_NOP_SHIFT                       (7U)
#define DMA_CERR_NOP(x)                          (((uint8)(((uint8)(x)) << DMA_CERR_NOP_SHIFT)) & DMA_CERR_NOP_MASK)

/*! @name CINT - Clear Interrupt Request Register */
#define DMA_CINT_CINT_MASK                       (0xFU)
#define DMA_CINT_CINT_SHIFT                      (0U)
#define DMA_CINT_CINT(x)                         (((uint8)(((uint8)(x)) << DMA_CINT_CINT_SHIFT)) & DMA_CINT_CINT_MASK)
#define DMA_CINT_CAIR_MASK                       (0x40U)
#define DMA_CINT_CAIR_SHIFT                      (6U)
#define DMA_CINT_CAIR(x)                         (((uint8)(((uint8)(x)) << DMA_CINT_CAIR_SHIFT)) & DMA_CINT_CAIR_MASK)
#define DMA_CINT_NOP_MASK                        (0x80U)
#define DMA_CINT_NOP_SHIFT                       (7U)
#define DMA_CINT_NOP(x)                          (((uint8)(((uint8)(x)) << DMA_CINT_NOP_SHIFT)) & DMA_CINT_NOP_MASK)

/*! @name INT - Interrupt Request Register */
#define DMA_INT_INT0_MASK                        (0x1U)
#define DMA_INT_INT0_SHIFT                       (0U)
#define DMA_INT_INT0(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT0_SHIFT)) & DMA_INT_INT0_MASK)
#define DMA_INT_INT1_MASK                        (0x2U)
#define DMA_INT_INT1_SHIFT                       (1U)
#define DMA_INT_INT1(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT1_SHIFT)) & DMA_INT_INT1_MASK)
#define DMA_INT_INT2_MASK                        (0x4U)
#define DMA_INT_INT2_SHIFT                       (2U)
#define DMA_INT_INT2(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT2_SHIFT)) & DMA_INT_INT2_MASK)
#define DMA_INT_INT3_MASK                        (0x8U)
#define DMA_INT_INT3_SHIFT                       (3U)
#define DMA_INT_INT3(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT3_SHIFT)) & DMA_INT_INT3_MASK)
#define DMA_INT_INT4_MASK                        (0x10U)
#define DMA_INT_INT4_SHIFT                       (4U)
#define DMA_INT_INT4(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT4_SHIFT)) & DMA_INT_INT4_MASK)
#define DMA_INT_INT5_MASK                        (0x20U)
#define DMA_INT_INT5_SHIFT                       (5U)
#define DMA_INT_INT5(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT5_SHIFT)) & DMA_INT_INT5_MASK)
#define DMA_INT_INT6_MASK                        (0x40U)
#define DMA_INT_INT6_SHIFT                       (6U)
#define DMA_INT_INT6(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT6_SHIFT)) & DMA_INT_INT6_MASK)
#define DMA_INT_INT7_MASK                        (0x80U)
#define DMA_INT_INT7_SHIFT                       (7U)
#define DMA_INT_INT7(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT7_SHIFT)) & DMA_INT_INT7_MASK)
#define DMA_INT_INT8_MASK                        (0x100U)
#define DMA_INT_INT8_SHIFT                       (8U)
#define DMA_INT_INT8(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT8_SHIFT)) & DMA_INT_INT8_MASK)
#define DMA_INT_INT9_MASK                        (0x200U)
#define DMA_INT_INT9_SHIFT                       (9U)
#define DMA_INT_INT9(x)                          (((uint32)(((uint32)(x)) << DMA_INT_INT9_SHIFT)) & DMA_INT_INT9_MASK)
#define DMA_INT_INT10_MASK                       (0x400U)
#define DMA_INT_INT10_SHIFT                      (10U)
#define DMA_INT_INT10(x)                         (((uint32)(((uint32)(x)) << DMA_INT_INT10_SHIFT)) & DMA_INT_INT10_MASK)
#define DMA_INT_INT11_MASK                       (0x800U)
#define DMA_INT_INT11_SHIFT                      (11U)
#define DMA_INT_INT11(x)                         (((uint32)(((uint32)(x)) << DMA_INT_INT11_SHIFT)) & DMA_INT_INT11_MASK)
#define DMA_INT_INT12_MASK                       (0x1000U)
#define DMA_INT_INT12_SHIFT                      (12U)
#define DMA_INT_INT12(x)                         (((uint32)(((uint32)(x)) << DMA_INT_INT12_SHIFT)) & DMA_INT_INT12_MASK)
#define DMA_INT_INT13_MASK                       (0x2000U)
#define DMA_INT_INT13_SHIFT                      (13U)
#define DMA_INT_INT13(x)                         (((uint32)(((uint32)(x)) << DMA_INT_INT13_SHIFT)) & DMA_INT_INT13_MASK)
#define DMA_INT_INT14_MASK                       (0x4000U)
#define DMA_INT_INT14_SHIFT                      (14U)
#define DMA_INT_INT14(x)                         (((uint32)(((uint32)(x)) << DMA_INT_INT14_SHIFT)) & DMA_INT_INT14_MASK)
#define DMA_INT_INT15_MASK                       (0x8000U)
#define DMA_INT_INT15_SHIFT                      (15U)
#define DMA_INT_INT15(x)                         (((uint32)(((uint32)(x)) << DMA_INT_INT15_SHIFT)) & DMA_INT_INT15_MASK)

/*! @name ERR - Error Register */
#define DMA_ERR_ERR0_MASK                        (0x1U)
#define DMA_ERR_ERR0_SHIFT                       (0U)
#define DMA_ERR_ERR0(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR0_SHIFT)) & DMA_ERR_ERR0_MASK)
#define DMA_ERR_ERR1_MASK                        (0x2U)
#define DMA_ERR_ERR1_SHIFT                       (1U)
#define DMA_ERR_ERR1(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR1_SHIFT)) & DMA_ERR_ERR1_MASK)
#define DMA_ERR_ERR2_MASK                        (0x4U)
#define DMA_ERR_ERR2_SHIFT                       (2U)
#define DMA_ERR_ERR2(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR2_SHIFT)) & DMA_ERR_ERR2_MASK)
#define DMA_ERR_ERR3_MASK                        (0x8U)
#define DMA_ERR_ERR3_SHIFT                       (3U)
#define DMA_ERR_ERR3(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR3_SHIFT)) & DMA_ERR_ERR3_MASK)
#define DMA_ERR_ERR4_MASK                        (0x10U)
#define DMA_ERR_ERR4_SHIFT                       (4U)
#define DMA_ERR_ERR4(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR4_SHIFT)) & DMA_ERR_ERR4_MASK)
#define DMA_ERR_ERR5_MASK                        (0x20U)
#define DMA_ERR_ERR5_SHIFT                       (5U)
#define DMA_ERR_ERR5(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR5_SHIFT)) & DMA_ERR_ERR5_MASK)
#define DMA_ERR_ERR6_MASK                        (0x40U)
#define DMA_ERR_ERR6_SHIFT                       (6U)
#define DMA_ERR_ERR6(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR6_SHIFT)) & DMA_ERR_ERR6_MASK)
#define DMA_ERR_ERR7_MASK                        (0x80U)
#define DMA_ERR_ERR7_SHIFT                       (7U)
#define DMA_ERR_ERR7(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR7_SHIFT)) & DMA_ERR_ERR7_MASK)
#define DMA_ERR_ERR8_MASK                        (0x100U)
#define DMA_ERR_ERR8_SHIFT                       (8U)
#define DMA_ERR_ERR8(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR8_SHIFT)) & DMA_ERR_ERR8_MASK)
#define DMA_ERR_ERR9_MASK                        (0x200U)
#define DMA_ERR_ERR9_SHIFT                       (9U)
#define DMA_ERR_ERR9(x)                          (((uint32)(((uint32)(x)) << DMA_ERR_ERR9_SHIFT)) & DMA_ERR_ERR9_MASK)
#define DMA_ERR_ERR10_MASK                       (0x400U)
#define DMA_ERR_ERR10_SHIFT                      (10U)
#define DMA_ERR_ERR10(x)                         (((uint32)(((uint32)(x)) << DMA_ERR_ERR10_SHIFT)) & DMA_ERR_ERR10_MASK)
#define DMA_ERR_ERR11_MASK                       (0x800U)
#define DMA_ERR_ERR11_SHIFT                      (11U)
#define DMA_ERR_ERR11(x)                         (((uint32)(((uint32)(x)) << DMA_ERR_ERR11_SHIFT)) & DMA_ERR_ERR11_MASK)
#define DMA_ERR_ERR12_MASK                       (0x1000U)
#define DMA_ERR_ERR12_SHIFT                      (12U)
#define DMA_ERR_ERR12(x)                         (((uint32)(((uint32)(x)) << DMA_ERR_ERR12_SHIFT)) & DMA_ERR_ERR12_MASK)
#define DMA_ERR_ERR13_MASK                       (0x2000U)
#define DMA_ERR_ERR13_SHIFT                      (13U)
#define DMA_ERR_ERR13(x)                         (((uint32)(((uint32)(x)) << DMA_ERR_ERR13_SHIFT)) & DMA_ERR_ERR13_MASK)
#define DMA_ERR_ERR14_MASK                       (0x4000U)
#define DMA_ERR_ERR14_SHIFT                      (14U)
#define DMA_ERR_ERR14(x)                         (((uint32)(((uint32)(x)) << DMA_ERR_ERR14_SHIFT)) & DMA_ERR_ERR14_MASK)
#define DMA_ERR_ERR15_MASK                       (0x8000U)
#define DMA_ERR_ERR15_SHIFT                      (15U)
#define DMA_ERR_ERR15(x)                         (((uint32)(((uint32)(x)) << DMA_ERR_ERR15_SHIFT)) & DMA_ERR_ERR15_MASK)

/*! @name HRS - Hardware Request Status Register */
#define DMA_HRS_HRS0_MASK                        (0x1U)
#define DMA_HRS_HRS0_SHIFT                       (0U)
#define DMA_HRS_HRS0(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS0_SHIFT)) & DMA_HRS_HRS0_MASK)
#define DMA_HRS_HRS1_MASK                        (0x2U)
#define DMA_HRS_HRS1_SHIFT                       (1U)
#define DMA_HRS_HRS1(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS1_SHIFT)) & DMA_HRS_HRS1_MASK)
#define DMA_HRS_HRS2_MASK                        (0x4U)
#define DMA_HRS_HRS2_SHIFT                       (2U)
#define DMA_HRS_HRS2(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS2_SHIFT)) & DMA_HRS_HRS2_MASK)
#define DMA_HRS_HRS3_MASK                        (0x8U)
#define DMA_HRS_HRS3_SHIFT                       (3U)
#define DMA_HRS_HRS3(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS3_SHIFT)) & DMA_HRS_HRS3_MASK)
#define DMA_HRS_HRS4_MASK                        (0x10U)
#define DMA_HRS_HRS4_SHIFT                       (4U)
#define DMA_HRS_HRS4(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS4_SHIFT)) & DMA_HRS_HRS4_MASK)
#define DMA_HRS_HRS5_MASK                        (0x20U)
#define DMA_HRS_HRS5_SHIFT                       (5U)
#define DMA_HRS_HRS5(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS5_SHIFT)) & DMA_HRS_HRS5_MASK)
#define DMA_HRS_HRS6_MASK                        (0x40U)
#define DMA_HRS_HRS6_SHIFT                       (6U)
#define DMA_HRS_HRS6(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS6_SHIFT)) & DMA_HRS_HRS6_MASK)
#define DMA_HRS_HRS7_MASK                        (0x80U)
#define DMA_HRS_HRS7_SHIFT                       (7U)
#define DMA_HRS_HRS7(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS7_SHIFT)) & DMA_HRS_HRS7_MASK)
#define DMA_HRS_HRS8_MASK                        (0x100U)
#define DMA_HRS_HRS8_SHIFT                       (8U)
#define DMA_HRS_HRS8(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS8_SHIFT)) & DMA_HRS_HRS8_MASK)
#define DMA_HRS_HRS9_MASK                        (0x200U)
#define DMA_HRS_HRS9_SHIFT                       (9U)
#define DMA_HRS_HRS9(x)                          (((uint32)(((uint32)(x)) << DMA_HRS_HRS9_SHIFT)) & DMA_HRS_HRS9_MASK)
#define DMA_HRS_HRS10_MASK                       (0x400U)
#define DMA_HRS_HRS10_SHIFT                      (10U)
#define DMA_HRS_HRS10(x)                         (((uint32)(((uint32)(x)) << DMA_HRS_HRS10_SHIFT)) & DMA_HRS_HRS10_MASK)
#define DMA_HRS_HRS11_MASK                       (0x800U)
#define DMA_HRS_HRS11_SHIFT                      (11U)
#define DMA_HRS_HRS11(x)                         (((uint32)(((uint32)(x)) << DMA_HRS_HRS11_SHIFT)) & DMA_HRS_HRS11_MASK)
#define DMA_HRS_HRS12_MASK                       (0x1000U)
#define DMA_HRS_HRS12_SHIFT                      (12U)
#define DMA_HRS_HRS12(x)                         (((uint32)(((uint32)(x)) << DMA_HRS_HRS12_SHIFT)) & DMA_HRS_HRS12_MASK)
#define DMA_HRS_HRS13_MASK                       (0x2000U)
#define DMA_HRS_HRS13_SHIFT                      (13U)
#define DMA_HRS_HRS13(x)                         (((uint32)(((uint32)(x)) << DMA_HRS_HRS13_SHIFT)) & DMA_HRS_HRS13_MASK)
#define DMA_HRS_HRS14_MASK                       (0x4000U)
#define DMA_HRS_HRS14_SHIFT                      (14U)
#define DMA_HRS_HRS14(x)                         (((uint32)(((uint32)(x)) << DMA_HRS_HRS14_SHIFT)) & DMA_HRS_HRS14_MASK)
#define DMA_HRS_HRS15_MASK                       (0x8000U)
#define DMA_HRS_HRS15_SHIFT                      (15U)
#define DMA_HRS_HRS15(x)                         (((uint32)(((uint32)(x)) << DMA_HRS_HRS15_SHIFT)) & DMA_HRS_HRS15_MASK)

/*! @name EARS - Enable Asynchronous Request in Stop Register */
#define DMA_EARS_EDREQ_0_MASK                    (0x1U)
#define DMA_EARS_EDREQ_0_SHIFT                   (0U)
#define DMA_EARS_EDREQ_0(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_0_SHIFT)) & DMA_EARS_EDREQ_0_MASK)
#define DMA_EARS_EDREQ_1_MASK                    (0x2U)
#define DMA_EARS_EDREQ_1_SHIFT                   (1U)
#define DMA_EARS_EDREQ_1(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_1_SHIFT)) & DMA_EARS_EDREQ_1_MASK)
#define DMA_EARS_EDREQ_2_MASK                    (0x4U)
#define DMA_EARS_EDREQ_2_SHIFT                   (2U)
#define DMA_EARS_EDREQ_2(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_2_SHIFT)) & DMA_EARS_EDREQ_2_MASK)
#define DMA_EARS_EDREQ_3_MASK                    (0x8U)
#define DMA_EARS_EDREQ_3_SHIFT                   (3U)
#define DMA_EARS_EDREQ_3(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_3_SHIFT)) & DMA_EARS_EDREQ_3_MASK)
#define DMA_EARS_EDREQ_4_MASK                    (0x10U)
#define DMA_EARS_EDREQ_4_SHIFT                   (4U)
#define DMA_EARS_EDREQ_4(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_4_SHIFT)) & DMA_EARS_EDREQ_4_MASK)
#define DMA_EARS_EDREQ_5_MASK                    (0x20U)
#define DMA_EARS_EDREQ_5_SHIFT                   (5U)
#define DMA_EARS_EDREQ_5(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_5_SHIFT)) & DMA_EARS_EDREQ_5_MASK)
#define DMA_EARS_EDREQ_6_MASK                    (0x40U)
#define DMA_EARS_EDREQ_6_SHIFT                   (6U)
#define DMA_EARS_EDREQ_6(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_6_SHIFT)) & DMA_EARS_EDREQ_6_MASK)
#define DMA_EARS_EDREQ_7_MASK                    (0x80U)
#define DMA_EARS_EDREQ_7_SHIFT                   (7U)
#define DMA_EARS_EDREQ_7(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_7_SHIFT)) & DMA_EARS_EDREQ_7_MASK)
#define DMA_EARS_EDREQ_8_MASK                    (0x100U)
#define DMA_EARS_EDREQ_8_SHIFT                   (8U)
#define DMA_EARS_EDREQ_8(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_8_SHIFT)) & DMA_EARS_EDREQ_8_MASK)
#define DMA_EARS_EDREQ_9_MASK                    (0x200U)
#define DMA_EARS_EDREQ_9_SHIFT                   (9U)
#define DMA_EARS_EDREQ_9(x)                      (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_9_SHIFT)) & DMA_EARS_EDREQ_9_MASK)
#define DMA_EARS_EDREQ_10_MASK                   (0x400U)
#define DMA_EARS_EDREQ_10_SHIFT                  (10U)
#define DMA_EARS_EDREQ_10(x)                     (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_10_SHIFT)) & DMA_EARS_EDREQ_10_MASK)
#define DMA_EARS_EDREQ_11_MASK                   (0x800U)
#define DMA_EARS_EDREQ_11_SHIFT                  (11U)
#define DMA_EARS_EDREQ_11(x)                     (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_11_SHIFT)) & DMA_EARS_EDREQ_11_MASK)
#define DMA_EARS_EDREQ_12_MASK                   (0x1000U)
#define DMA_EARS_EDREQ_12_SHIFT                  (12U)
#define DMA_EARS_EDREQ_12(x)                     (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_12_SHIFT)) & DMA_EARS_EDREQ_12_MASK)
#define DMA_EARS_EDREQ_13_MASK                   (0x2000U)
#define DMA_EARS_EDREQ_13_SHIFT                  (13U)
#define DMA_EARS_EDREQ_13(x)                     (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_13_SHIFT)) & DMA_EARS_EDREQ_13_MASK)
#define DMA_EARS_EDREQ_14_MASK                   (0x4000U)
#define DMA_EARS_EDREQ_14_SHIFT                  (14U)
#define DMA_EARS_EDREQ_14(x)                     (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_14_SHIFT)) & DMA_EARS_EDREQ_14_MASK)
#define DMA_EARS_EDREQ_15_MASK                   (0x8000U)
#define DMA_EARS_EDREQ_15_SHIFT                  (15U)
#define DMA_EARS_EDREQ_15(x)                     (((uint32)(((uint32)(x)) << DMA_EARS_EDREQ_15_SHIFT)) & DMA_EARS_EDREQ_15_MASK)

/*! @name DCHPRI3 - Channel n Priority Register */
#define DMA_DCHPRI3_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI3_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI3_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI3_CHPRI_SHIFT)) & DMA_DCHPRI3_CHPRI_MASK)
#define DMA_DCHPRI3_DPA_MASK                     (0x40U)
#define DMA_DCHPRI3_DPA_SHIFT                    (6U)
#define DMA_DCHPRI3_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI3_DPA_SHIFT)) & DMA_DCHPRI3_DPA_MASK)
#define DMA_DCHPRI3_ECP_MASK                     (0x80U)
#define DMA_DCHPRI3_ECP_SHIFT                    (7U)
#define DMA_DCHPRI3_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI3_ECP_SHIFT)) & DMA_DCHPRI3_ECP_MASK)

/*! @name DCHPRI2 - Channel n Priority Register */
#define DMA_DCHPRI2_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI2_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI2_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI2_CHPRI_SHIFT)) & DMA_DCHPRI2_CHPRI_MASK)
#define DMA_DCHPRI2_DPA_MASK                     (0x40U)
#define DMA_DCHPRI2_DPA_SHIFT                    (6U)
#define DMA_DCHPRI2_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI2_DPA_SHIFT)) & DMA_DCHPRI2_DPA_MASK)
#define DMA_DCHPRI2_ECP_MASK                     (0x80U)
#define DMA_DCHPRI2_ECP_SHIFT                    (7U)
#define DMA_DCHPRI2_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI2_ECP_SHIFT)) & DMA_DCHPRI2_ECP_MASK)

/*! @name DCHPRI1 - Channel n Priority Register */
#define DMA_DCHPRI1_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI1_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI1_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI1_CHPRI_SHIFT)) & DMA_DCHPRI1_CHPRI_MASK)
#define DMA_DCHPRI1_DPA_MASK                     (0x40U)
#define DMA_DCHPRI1_DPA_SHIFT                    (6U)
#define DMA_DCHPRI1_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI1_DPA_SHIFT)) & DMA_DCHPRI1_DPA_MASK)
#define DMA_DCHPRI1_ECP_MASK                     (0x80U)
#define DMA_DCHPRI1_ECP_SHIFT                    (7U)
#define DMA_DCHPRI1_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI1_ECP_SHIFT)) & DMA_DCHPRI1_ECP_MASK)

/*! @name DCHPRI0 - Channel n Priority Register */
#define DMA_DCHPRI0_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI0_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI0_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI0_CHPRI_SHIFT)) & DMA_DCHPRI0_CHPRI_MASK)
#define DMA_DCHPRI0_DPA_MASK                     (0x40U)
#define DMA_DCHPRI0_DPA_SHIFT                    (6U)
#define DMA_DCHPRI0_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI0_DPA_SHIFT)) & DMA_DCHPRI0_DPA_MASK)
#define DMA_DCHPRI0_ECP_MASK                     (0x80U)
#define DMA_DCHPRI0_ECP_SHIFT                    (7U)
#define DMA_DCHPRI0_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI0_ECP_SHIFT)) & DMA_DCHPRI0_ECP_MASK)

/*! @name DCHPRI7 - Channel n Priority Register */
#define DMA_DCHPRI7_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI7_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI7_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI7_CHPRI_SHIFT)) & DMA_DCHPRI7_CHPRI_MASK)
#define DMA_DCHPRI7_DPA_MASK                     (0x40U)
#define DMA_DCHPRI7_DPA_SHIFT                    (6U)
#define DMA_DCHPRI7_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI7_DPA_SHIFT)) & DMA_DCHPRI7_DPA_MASK)
#define DMA_DCHPRI7_ECP_MASK                     (0x80U)
#define DMA_DCHPRI7_ECP_SHIFT                    (7U)
#define DMA_DCHPRI7_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI7_ECP_SHIFT)) & DMA_DCHPRI7_ECP_MASK)

/*! @name DCHPRI6 - Channel n Priority Register */
#define DMA_DCHPRI6_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI6_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI6_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI6_CHPRI_SHIFT)) & DMA_DCHPRI6_CHPRI_MASK)
#define DMA_DCHPRI6_DPA_MASK                     (0x40U)
#define DMA_DCHPRI6_DPA_SHIFT                    (6U)
#define DMA_DCHPRI6_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI6_DPA_SHIFT)) & DMA_DCHPRI6_DPA_MASK)
#define DMA_DCHPRI6_ECP_MASK                     (0x80U)
#define DMA_DCHPRI6_ECP_SHIFT                    (7U)
#define DMA_DCHPRI6_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI6_ECP_SHIFT)) & DMA_DCHPRI6_ECP_MASK)

/*! @name DCHPRI5 - Channel n Priority Register */
#define DMA_DCHPRI5_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI5_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI5_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI5_CHPRI_SHIFT)) & DMA_DCHPRI5_CHPRI_MASK)
#define DMA_DCHPRI5_DPA_MASK                     (0x40U)
#define DMA_DCHPRI5_DPA_SHIFT                    (6U)
#define DMA_DCHPRI5_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI5_DPA_SHIFT)) & DMA_DCHPRI5_DPA_MASK)
#define DMA_DCHPRI5_ECP_MASK                     (0x80U)
#define DMA_DCHPRI5_ECP_SHIFT                    (7U)
#define DMA_DCHPRI5_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI5_ECP_SHIFT)) & DMA_DCHPRI5_ECP_MASK)

/*! @name DCHPRI4 - Channel n Priority Register */
#define DMA_DCHPRI4_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI4_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI4_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI4_CHPRI_SHIFT)) & DMA_DCHPRI4_CHPRI_MASK)
#define DMA_DCHPRI4_DPA_MASK                     (0x40U)
#define DMA_DCHPRI4_DPA_SHIFT                    (6U)
#define DMA_DCHPRI4_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI4_DPA_SHIFT)) & DMA_DCHPRI4_DPA_MASK)
#define DMA_DCHPRI4_ECP_MASK                     (0x80U)
#define DMA_DCHPRI4_ECP_SHIFT                    (7U)
#define DMA_DCHPRI4_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI4_ECP_SHIFT)) & DMA_DCHPRI4_ECP_MASK)

/*! @name DCHPRI11 - Channel n Priority Register */
#define DMA_DCHPRI11_CHPRI_MASK                  (0xFU)
#define DMA_DCHPRI11_CHPRI_SHIFT                 (0U)
#define DMA_DCHPRI11_CHPRI(x)                    (((uint8)(((uint8)(x)) << DMA_DCHPRI11_CHPRI_SHIFT)) & DMA_DCHPRI11_CHPRI_MASK)
#define DMA_DCHPRI11_DPA_MASK                    (0x40U)
#define DMA_DCHPRI11_DPA_SHIFT                   (6U)
#define DMA_DCHPRI11_DPA(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI11_DPA_SHIFT)) & DMA_DCHPRI11_DPA_MASK)
#define DMA_DCHPRI11_ECP_MASK                    (0x80U)
#define DMA_DCHPRI11_ECP_SHIFT                   (7U)
#define DMA_DCHPRI11_ECP(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI11_ECP_SHIFT)) & DMA_DCHPRI11_ECP_MASK)

/*! @name DCHPRI10 - Channel n Priority Register */
#define DMA_DCHPRI10_CHPRI_MASK                  (0xFU)
#define DMA_DCHPRI10_CHPRI_SHIFT                 (0U)
#define DMA_DCHPRI10_CHPRI(x)                    (((uint8)(((uint8)(x)) << DMA_DCHPRI10_CHPRI_SHIFT)) & DMA_DCHPRI10_CHPRI_MASK)
#define DMA_DCHPRI10_DPA_MASK                    (0x40U)
#define DMA_DCHPRI10_DPA_SHIFT                   (6U)
#define DMA_DCHPRI10_DPA(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI10_DPA_SHIFT)) & DMA_DCHPRI10_DPA_MASK)
#define DMA_DCHPRI10_ECP_MASK                    (0x80U)
#define DMA_DCHPRI10_ECP_SHIFT                   (7U)
#define DMA_DCHPRI10_ECP(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI10_ECP_SHIFT)) & DMA_DCHPRI10_ECP_MASK)

/*! @name DCHPRI9 - Channel n Priority Register */
#define DMA_DCHPRI9_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI9_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI9_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI9_CHPRI_SHIFT)) & DMA_DCHPRI9_CHPRI_MASK)
#define DMA_DCHPRI9_DPA_MASK                     (0x40U)
#define DMA_DCHPRI9_DPA_SHIFT                    (6U)
#define DMA_DCHPRI9_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI9_DPA_SHIFT)) & DMA_DCHPRI9_DPA_MASK)
#define DMA_DCHPRI9_ECP_MASK                     (0x80U)
#define DMA_DCHPRI9_ECP_SHIFT                    (7U)
#define DMA_DCHPRI9_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI9_ECP_SHIFT)) & DMA_DCHPRI9_ECP_MASK)

/*! @name DCHPRI8 - Channel n Priority Register */
#define DMA_DCHPRI8_CHPRI_MASK                   (0xFU)
#define DMA_DCHPRI8_CHPRI_SHIFT                  (0U)
#define DMA_DCHPRI8_CHPRI(x)                     (((uint8)(((uint8)(x)) << DMA_DCHPRI8_CHPRI_SHIFT)) & DMA_DCHPRI8_CHPRI_MASK)
#define DMA_DCHPRI8_DPA_MASK                     (0x40U)
#define DMA_DCHPRI8_DPA_SHIFT                    (6U)
#define DMA_DCHPRI8_DPA(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI8_DPA_SHIFT)) & DMA_DCHPRI8_DPA_MASK)
#define DMA_DCHPRI8_ECP_MASK                     (0x80U)
#define DMA_DCHPRI8_ECP_SHIFT                    (7U)
#define DMA_DCHPRI8_ECP(x)                       (((uint8)(((uint8)(x)) << DMA_DCHPRI8_ECP_SHIFT)) & DMA_DCHPRI8_ECP_MASK)

/*! @name DCHPRI15 - Channel n Priority Register */
#define DMA_DCHPRI15_CHPRI_MASK                  (0xFU)
#define DMA_DCHPRI15_CHPRI_SHIFT                 (0U)
#define DMA_DCHPRI15_CHPRI(x)                    (((uint8)(((uint8)(x)) << DMA_DCHPRI15_CHPRI_SHIFT)) & DMA_DCHPRI15_CHPRI_MASK)
#define DMA_DCHPRI15_DPA_MASK                    (0x40U)
#define DMA_DCHPRI15_DPA_SHIFT                   (6U)
#define DMA_DCHPRI15_DPA(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI15_DPA_SHIFT)) & DMA_DCHPRI15_DPA_MASK)
#define DMA_DCHPRI15_ECP_MASK                    (0x80U)
#define DMA_DCHPRI15_ECP_SHIFT                   (7U)
#define DMA_DCHPRI15_ECP(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI15_ECP_SHIFT)) & DMA_DCHPRI15_ECP_MASK)

/*! @name DCHPRI14 - Channel n Priority Register */
#define DMA_DCHPRI14_CHPRI_MASK                  (0xFU)
#define DMA_DCHPRI14_CHPRI_SHIFT                 (0U)
#define DMA_DCHPRI14_CHPRI(x)                    (((uint8)(((uint8)(x)) << DMA_DCHPRI14_CHPRI_SHIFT)) & DMA_DCHPRI14_CHPRI_MASK)
#define DMA_DCHPRI14_DPA_MASK                    (0x40U)
#define DMA_DCHPRI14_DPA_SHIFT                   (6U)
#define DMA_DCHPRI14_DPA(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI14_DPA_SHIFT)) & DMA_DCHPRI14_DPA_MASK)
#define DMA_DCHPRI14_ECP_MASK                    (0x80U)
#define DMA_DCHPRI14_ECP_SHIFT                   (7U)
#define DMA_DCHPRI14_ECP(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI14_ECP_SHIFT)) & DMA_DCHPRI14_ECP_MASK)

/*! @name DCHPRI13 - Channel n Priority Register */
#define DMA_DCHPRI13_CHPRI_MASK                  (0xFU)
#define DMA_DCHPRI13_CHPRI_SHIFT                 (0U)
#define DMA_DCHPRI13_CHPRI(x)                    (((uint8)(((uint8)(x)) << DMA_DCHPRI13_CHPRI_SHIFT)) & DMA_DCHPRI13_CHPRI_MASK)
#define DMA_DCHPRI13_DPA_MASK                    (0x40U)
#define DMA_DCHPRI13_DPA_SHIFT                   (6U)
#define DMA_DCHPRI13_DPA(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI13_DPA_SHIFT)) & DMA_DCHPRI13_DPA_MASK)
#define DMA_DCHPRI13_ECP_MASK                    (0x80U)
#define DMA_DCHPRI13_ECP_SHIFT                   (7U)
#define DMA_DCHPRI13_ECP(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI13_ECP_SHIFT)) & DMA_DCHPRI13_ECP_MASK)

/*! @name DCHPRI12 - Channel n Priority Register */
#define DMA_DCHPRI12_CHPRI_MASK                  (0xFU)
#define DMA_DCHPRI12_CHPRI_SHIFT                 (0U)
#define DMA_DCHPRI12_CHPRI(x)                    (((uint8)(((uint8)(x)) << DMA_DCHPRI12_CHPRI_SHIFT)) & DMA_DCHPRI12_CHPRI_MASK)
#define DMA_DCHPRI12_DPA_MASK                    (0x40U)
#define DMA_DCHPRI12_DPA_SHIFT                   (6U)
#define DMA_DCHPRI12_DPA(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI12_DPA_SHIFT)) & DMA_DCHPRI12_DPA_MASK)
#define DMA_DCHPRI12_ECP_MASK                    (0x80U)
#define DMA_DCHPRI12_ECP_SHIFT                   (7U)
#define DMA_DCHPRI12_ECP(x)                      (((uint8)(((uint8)(x)) << DMA_DCHPRI12_ECP_SHIFT)) & DMA_DCHPRI12_ECP_MASK)

/*! @name SADDR - TCD Source Address */
#define DMA_SADDR_SADDR_MASK                     (0xFFFFFFFFU)
#define DMA_SADDR_SADDR_SHIFT                    (0U)
#define DMA_SADDR_SADDR(x)                       (((uint32)(((uint32)(x)) << DMA_SADDR_SADDR_SHIFT)) & DMA_SADDR_SADDR_MASK)

/* The count of DMA_SADDR */
#define DMA_SADDR_COUNT                          (16U)

/*! @name SOFF - TCD Signed Source Address Offset */
#define DMA_SOFF_SOFF_MASK                       (0xFFFFU)
#define DMA_SOFF_SOFF_SHIFT                      (0U)
#define DMA_SOFF_SOFF(x)                         (((uint16)(((uint16)(x)) << DMA_SOFF_SOFF_SHIFT)) & DMA_SOFF_SOFF_MASK)

/* The count of DMA_SOFF */
#define DMA_SOFF_COUNT                           (16U)

/*! @name ATTR - TCD Transfer Attributes */
#define DMA_ATTR_DSIZE_MASK                      (0x7U)
#define DMA_ATTR_DSIZE_SHIFT                     (0U)
#define DMA_ATTR_DSIZE(x)                        (((uint16)(((uint16)(x)) << DMA_ATTR_DSIZE_SHIFT)) & DMA_ATTR_DSIZE_MASK)
#define DMA_ATTR_DMOD_MASK                       (0xF8U)
#define DMA_ATTR_DMOD_SHIFT                      (3U)
#define DMA_ATTR_DMOD(x)                         (((uint16)(((uint16)(x)) << DMA_ATTR_DMOD_SHIFT)) & DMA_ATTR_DMOD_MASK)
#define DMA_ATTR_SSIZE_MASK                      (0x700U)
#define DMA_ATTR_SSIZE_SHIFT                     (8U)
#define DMA_ATTR_SSIZE(x)                        (((uint16)(((uint16)(x)) << DMA_ATTR_SSIZE_SHIFT)) & DMA_ATTR_SSIZE_MASK)
#define DMA_ATTR_SMOD_MASK                       (0xF800U)
#define DMA_ATTR_SMOD_SHIFT                      (11U)
#define DMA_ATTR_SMOD(x)                         (((uint16)(((uint16)(x)) << DMA_ATTR_SMOD_SHIFT)) & DMA_ATTR_SMOD_MASK)

/* The count of DMA_ATTR */
#define DMA_ATTR_COUNT                           (16U)

/*! @name NBYTES_MLNO - TCD Minor Byte Count (Minor Loop Disabled) */
#define DMA_NBYTES_MLNO_NBYTES_MASK              (0xFFFFFFFFU)
#define DMA_NBYTES_MLNO_NBYTES_SHIFT             (0U)
#define DMA_NBYTES_MLNO_NBYTES(x)                (((uint32)(((uint32)(x)) << DMA_NBYTES_MLNO_NBYTES_SHIFT)) & DMA_NBYTES_MLNO_NBYTES_MASK)

/* The count of DMA_NBYTES_MLNO */
#define DMA_NBYTES_MLNO_COUNT                    (16U)

/*! @name NBYTES_MLOFFNO - TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled) */
#define DMA_NBYTES_MLOFFNO_NBYTES_MASK           (0x3FFFFFFFU)
#define DMA_NBYTES_MLOFFNO_NBYTES_SHIFT          (0U)
#define DMA_NBYTES_MLOFFNO_NBYTES(x)             (((uint32)(((uint32)(x)) << DMA_NBYTES_MLOFFNO_NBYTES_SHIFT)) & DMA_NBYTES_MLOFFNO_NBYTES_MASK)
#define DMA_NBYTES_MLOFFNO_DMLOE_MASK            (0x40000000U)
#define DMA_NBYTES_MLOFFNO_DMLOE_SHIFT           (30U)
#define DMA_NBYTES_MLOFFNO_DMLOE(x)              (((uint32)(((uint32)(x)) << DMA_NBYTES_MLOFFNO_DMLOE_SHIFT)) & DMA_NBYTES_MLOFFNO_DMLOE_MASK)
#define DMA_NBYTES_MLOFFNO_SMLOE_MASK            (0x80000000U)
#define DMA_NBYTES_MLOFFNO_SMLOE_SHIFT           (31U)
#define DMA_NBYTES_MLOFFNO_SMLOE(x)              (((uint32)(((uint32)(x)) << DMA_NBYTES_MLOFFNO_SMLOE_SHIFT)) & DMA_NBYTES_MLOFFNO_SMLOE_MASK)

/* The count of DMA_NBYTES_MLOFFNO */
#define DMA_NBYTES_MLOFFNO_COUNT                 (16U)

/*! @name NBYTES_MLOFFYES - TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled) */
#define DMA_NBYTES_MLOFFYES_NBYTES_MASK          (0x3FFU)
#define DMA_NBYTES_MLOFFYES_NBYTES_SHIFT         (0U)
#define DMA_NBYTES_MLOFFYES_NBYTES(x)            (((uint32)(((uint32)(x)) << DMA_NBYTES_MLOFFYES_NBYTES_SHIFT)) & DMA_NBYTES_MLOFFYES_NBYTES_MASK)
#define DMA_NBYTES_MLOFFYES_MLOFF_MASK           (0x3FFFFC00U)
#define DMA_NBYTES_MLOFFYES_MLOFF_SHIFT          (10U)
#define DMA_NBYTES_MLOFFYES_MLOFF(x)             (((uint32)(((uint32)(x)) << DMA_NBYTES_MLOFFYES_MLOFF_SHIFT)) & DMA_NBYTES_MLOFFYES_MLOFF_MASK)
#define DMA_NBYTES_MLOFFYES_DMLOE_MASK           (0x40000000U)
#define DMA_NBYTES_MLOFFYES_DMLOE_SHIFT          (30U)
#define DMA_NBYTES_MLOFFYES_DMLOE(x)             (((uint32)(((uint32)(x)) << DMA_NBYTES_MLOFFYES_DMLOE_SHIFT)) & DMA_NBYTES_MLOFFYES_DMLOE_MASK)
#define DMA_NBYTES_MLOFFYES_SMLOE_MASK           (0x80000000U)
#define DMA_NBYTES_MLOFFYES_SMLOE_SHIFT          (31U)
#define DMA_NBYTES_MLOFFYES_SMLOE(x)             (((uint32)(((uint32)(x)) << DMA_NBYTES_MLOFFYES_SMLOE_SHIFT)) & DMA_NBYTES_MLOFFYES_SMLOE_MASK)

/* The count of DMA_NBYTES_MLOFFYES */
#define DMA_NBYTES_MLOFFYES_COUNT                (16U)

/*! @name SLAST - TCD Last Source Address Adjustment */
#define DMA_SLAST_SLAST_MASK                     (0xFFFFFFFFU)
#define DMA_SLAST_SLAST_SHIFT                    (0U)
#define DMA_SLAST_SLAST(x)                       (((uint32)(((uint32)(x)) << DMA_SLAST_SLAST_SHIFT)) & DMA_SLAST_SLAST_MASK)

/* The count of DMA_SLAST */
#define DMA_SLAST_COUNT                          (16U)

/*! @name DADDR - TCD Destination Address */
#define DMA_DADDR_DADDR_MASK                     (0xFFFFFFFFU)
#define DMA_DADDR_DADDR_SHIFT                    (0U)
#define DMA_DADDR_DADDR(x)                       (((uint32)(((uint32)(x)) << DMA_DADDR_DADDR_SHIFT)) & DMA_DADDR_DADDR_MASK)

/* The count of DMA_DADDR */
#define DMA_DADDR_COUNT                          (16U)

/*! @name DOFF - TCD Signed Destination Address Offset */
#define DMA_DOFF_DOFF_MASK                       (0xFFFFU)
#define DMA_DOFF_DOFF_SHIFT                      (0U)
#define DMA_DOFF_DOFF(x)                         (((uint16)(((uint16)(x)) << DMA_DOFF_DOFF_SHIFT)) & DMA_DOFF_DOFF_MASK)

/* The count of DMA_DOFF */
#define DMA_DOFF_COUNT                           (16U)

/*! @name CITER_ELINKNO - TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled) */
#define DMA_CITER_ELINKNO_CITER_MASK             (0x7FFFU)
#define DMA_CITER_ELINKNO_CITER_SHIFT            (0U)
#define DMA_CITER_ELINKNO_CITER(x)               (((uint16)(((uint16)(x)) << DMA_CITER_ELINKNO_CITER_SHIFT)) & DMA_CITER_ELINKNO_CITER_MASK)
#define DMA_CITER_ELINKNO_ELINK_MASK             (0x8000U)
#define DMA_CITER_ELINKNO_ELINK_SHIFT            (15U)
#define DMA_CITER_ELINKNO_ELINK(x)               (((uint16)(((uint16)(x)) << DMA_CITER_ELINKNO_ELINK_SHIFT)) & DMA_CITER_ELINKNO_ELINK_MASK)

/* The count of DMA_CITER_ELINKNO */
#define DMA_CITER_ELINKNO_COUNT                  (16U)

/*! @name CITER_ELINKYES - TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled) */
#define DMA_CITER_ELINKYES_CITER_MASK            (0x1FFU)
#define DMA_CITER_ELINKYES_CITER_SHIFT           (0U)
#define DMA_CITER_ELINKYES_CITER(x)              (((uint16)(((uint16)(x)) << DMA_CITER_ELINKYES_CITER_SHIFT)) & DMA_CITER_ELINKYES_CITER_MASK)
#define DMA_CITER_ELINKYES_LINKCH_MASK           (0x1E00U)
#define DMA_CITER_ELINKYES_LINKCH_SHIFT          (9U)
#define DMA_CITER_ELINKYES_LINKCH(x)             (((uint16)(((uint16)(x)) << DMA_CITER_ELINKYES_LINKCH_SHIFT)) & DMA_CITER_ELINKYES_LINKCH_MASK)
#define DMA_CITER_ELINKYES_ELINK_MASK            (0x8000U)
#define DMA_CITER_ELINKYES_ELINK_SHIFT           (15U)
#define DMA_CITER_ELINKYES_ELINK(x)              (((uint16)(((uint16)(x)) << DMA_CITER_ELINKYES_ELINK_SHIFT)) & DMA_CITER_ELINKYES_ELINK_MASK)

/* The count of DMA_CITER_ELINKYES */
#define DMA_CITER_ELINKYES_COUNT                 (16U)

/*! @name DLAST_SGA - TCD Last Destination Address Adjustment/Scatter Gather Address */
#define DMA_DLAST_SGA_DLASTSGA_MASK              (0xFFFFFFFFU)
#define DMA_DLAST_SGA_DLASTSGA_SHIFT             (0U)
#define DMA_DLAST_SGA_DLASTSGA(x)                (((uint32)(((uint32)(x)) << DMA_DLAST_SGA_DLASTSGA_SHIFT)) & DMA_DLAST_SGA_DLASTSGA_MASK)

/* The count of DMA_DLAST_SGA */
#define DMA_DLAST_SGA_COUNT                      (16U)

/*! @name CSR - TCD Control and Status */
#define DMA_CSR_START_MASK                       (0x1U)
#define DMA_CSR_START_SHIFT                      (0U)
#define DMA_CSR_START(x)                         (((uint16)(((uint16)(x)) << DMA_CSR_START_SHIFT)) & DMA_CSR_START_MASK)
#define DMA_CSR_INTMAJOR_MASK                    (0x2U)
#define DMA_CSR_INTMAJOR_SHIFT                   (1U)
#define DMA_CSR_INTMAJOR(x)                      (((uint16)(((uint16)(x)) << DMA_CSR_INTMAJOR_SHIFT)) & DMA_CSR_INTMAJOR_MASK)
#define DMA_CSR_INTHALF_MASK                     (0x4U)
#define DMA_CSR_INTHALF_SHIFT                    (2U)
#define DMA_CSR_INTHALF(x)                       (((uint16)(((uint16)(x)) << DMA_CSR_INTHALF_SHIFT)) & DMA_CSR_INTHALF_MASK)
#define DMA_CSR_DREQ_MASK                        (0x8U)
#define DMA_CSR_DREQ_SHIFT                       (3U)
#define DMA_CSR_DREQ(x)                          (((uint16)(((uint16)(x)) << DMA_CSR_DREQ_SHIFT)) & DMA_CSR_DREQ_MASK)
#define DMA_CSR_ESG_MASK                         (0x10U)
#define DMA_CSR_ESG_SHIFT                        (4U)
#define DMA_CSR_ESG(x)                           (((uint16)(((uint16)(x)) << DMA_CSR_ESG_SHIFT)) & DMA_CSR_ESG_MASK)
#define DMA_CSR_MAJORELINK_MASK                  (0x20U)
#define DMA_CSR_MAJORELINK_SHIFT                 (5U)
#define DMA_CSR_MAJORELINK(x)                    (((uint16)(((uint16)(x)) << DMA_CSR_MAJORELINK_SHIFT)) & DMA_CSR_MAJORELINK_MASK)
#define DMA_CSR_ACTIVE_MASK                      (0x40U)
#define DMA_CSR_ACTIVE_SHIFT                     (6U)
#define DMA_CSR_ACTIVE(x)                        (((uint16)(((uint16)(x)) << DMA_CSR_ACTIVE_SHIFT)) & DMA_CSR_ACTIVE_MASK)
#define DMA_CSR_DONE_MASK                        (0x80U)
#define DMA_CSR_DONE_SHIFT                       (7U)
#define DMA_CSR_DONE(x)                          (((uint16)(((uint16)(x)) << DMA_CSR_DONE_SHIFT)) & DMA_CSR_DONE_MASK)
#define DMA_CSR_MAJORLINKCH_MASK                 (0xF00U)
#define DMA_CSR_MAJORLINKCH_SHIFT                (8U)
#define DMA_CSR_MAJORLINKCH(x)                   (((uint16)(((uint16)(x)) << DMA_CSR_MAJORLINKCH_SHIFT)) & DMA_CSR_MAJORLINKCH_MASK)
#define DMA_CSR_BWC_MASK                         (0xC000U)
#define DMA_CSR_BWC_SHIFT                        (14U)
#define DMA_CSR_BWC(x)                           (((uint16)(((uint16)(x)) << DMA_CSR_BWC_SHIFT)) & DMA_CSR_BWC_MASK)

/* The count of DMA_CSR */
#define DMA_CSR_COUNT                            (16U)

/*! @name BITER_ELINKNO - TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled) */
#define DMA_BITER_ELINKNO_BITER_MASK             (0x7FFFU)
#define DMA_BITER_ELINKNO_BITER_SHIFT            (0U)
#define DMA_BITER_ELINKNO_BITER(x)               (((uint16)(((uint16)(x)) << DMA_BITER_ELINKNO_BITER_SHIFT)) & DMA_BITER_ELINKNO_BITER_MASK)
#define DMA_BITER_ELINKNO_ELINK_MASK             (0x8000U)
#define DMA_BITER_ELINKNO_ELINK_SHIFT            (15U)
#define DMA_BITER_ELINKNO_ELINK(x)               (((uint16)(((uint16)(x)) << DMA_BITER_ELINKNO_ELINK_SHIFT)) & DMA_BITER_ELINKNO_ELINK_MASK)

/* The count of DMA_BITER_ELINKNO */
#define DMA_BITER_ELINKNO_COUNT                  (16U)

/*! @name BITER_ELINKYES - TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled) */
#define DMA_BITER_ELINKYES_BITER_MASK            (0x1FFU)
#define DMA_BITER_ELINKYES_BITER_SHIFT           (0U)
#define DMA_BITER_ELINKYES_BITER(x)              (((uint16)(((uint16)(x)) << DMA_BITER_ELINKYES_BITER_SHIFT)) & DMA_BITER_ELINKYES_BITER_MASK)
#define DMA_BITER_ELINKYES_LINKCH_MASK           (0x1E00U)
#define DMA_BITER_ELINKYES_LINKCH_SHIFT          (9U)
#define DMA_BITER_ELINKYES_LINKCH(x)             (((uint16)(((uint16)(x)) << DMA_BITER_ELINKYES_LINKCH_SHIFT)) & DMA_BITER_ELINKYES_LINKCH_MASK)
#define DMA_BITER_ELINKYES_ELINK_MASK            (0x8000U)
#define DMA_BITER_ELINKYES_ELINK_SHIFT           (15U)
#define DMA_BITER_ELINKYES_ELINK(x)              (((uint16)(((uint16)(x)) << DMA_BITER_ELINKYES_ELINK_SHIFT)) & DMA_BITER_ELINKYES_ELINK_MASK)

/* The count of DMA_BITER_ELINKYES */
#define DMA_BITER_ELINKYES_COUNT                 (16U)


/*!
 * @}
 */ /* end of group DMA_Register_Masks */


/* DMA - Peripheral instance base addresses */
/** Peripheral DMA base address */
#define DMA_BASE                                 (0x40008000u)
/** Peripheral DMA base pointer */
#define DMA0                                     ((DMA_TYPE *)DMA_BASE)
/** Array initializer of DMA peripheral base addresses */
#define DMA_BASE_ADDRS                           { DMA_BASE }
/** Array initializer of DMA peripheral base pointers */
#define DMA_BASE_PTRS                            { DMA0 }
/** Interrupt vectors for the DMA peripheral type */
#define DMA_CHN_IRQS                             { DMA0_IRQn, DMA1_IRQn, DMA2_IRQn, DMA3_IRQn, DMA4_IRQn, DMA5_IRQn, DMA6_IRQn, DMA7_IRQn, DMA8_IRQn, DMA9_IRQn, DMA10_IRQn, DMA11_IRQn, DMA12_IRQn, DMA13_IRQn, DMA14_IRQn, DMA15_IRQn }
#define DMA_ERROR_IRQS                           { DMA_Error_IRQn }

/*!
 * @}
 */ /* end of group DMA_Peripheral_Access_Layer */


/* ----------------------------------------------------------------------------
   -- DMAMUX Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DMAMUX_Peripheral_Access_Layer DMAMUX Peripheral Access Layer
 * @{
 */

/** DMAMUX - Register Layout Typedef */
typedef struct {
  volatile uint8 CHCFG[16];                          /**< Channel Configuration register, array offset: 0x0, array step: 0x1 */
} DMAMUX_Type;

/* ----------------------------------------------------------------------------
   -- DMAMUX Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup DMAMUX_Register_Masks DMAMUX Register Masks
 * @{
 */

/*! @name CHCFG - Channel Configuration register */
#define DMAMUX_CHCFG_SOURCE_MASK                 (0x3FU)
#define DMAMUX_CHCFG_SOURCE_SHIFT                (0U)
#define DMAMUX_CHCFG_SOURCE(x)                   (((uint8)(((uint8)(x)) << DMAMUX_CHCFG_SOURCE_SHIFT)) & DMAMUX_CHCFG_SOURCE_MASK)
#define DMAMUX_CHCFG_TRIG_MASK                   (0x40U)
#define DMAMUX_CHCFG_TRIG_SHIFT                  (6U)
#define DMAMUX_CHCFG_TRIG(x)                     (((uint8)(((uint8)(x)) << DMAMUX_CHCFG_TRIG_SHIFT)) & DMAMUX_CHCFG_TRIG_MASK)
#define DMAMUX_CHCFG_ENBL_MASK                   (0x80U)
#define DMAMUX_CHCFG_ENBL_SHIFT                  (7U)
#define DMAMUX_CHCFG_ENBL(x)                     (((uint8)(((uint8)(x)) << DMAMUX_CHCFG_ENBL_SHIFT)) & DMAMUX_CHCFG_ENBL_MASK)

/* The count of DMAMUX_CHCFG */
#define DMAMUX_CHCFG_COUNT                       (16U)


/*!
 * @}
 */ /* end of group DMAMUX_Register_Masks */


/* DMAMUX - Peripheral instance base addresses */
/** Peripheral DMAMUX base address */
#define DMAMUX_BASE                              (0x40021000u)
/** Peripheral DMAMUX base pointer */
#define DMAMUX                                   ((DMAMUX_Type *)DMAMUX_BASE)
/** Array initializer of DMAMUX peripheral base addresses */
#define DMAMUX_BASE_ADDRS                        { DMAMUX_BASE }
/** Array initializer of DMAMUX peripheral base pointers */
#define DMAMUX_BASE_PTRS                         { DMAMUX }





//
///*!
// * @addtogroup DMA_Peripheral DMA
// * @{
// */
//
///** DMA - Peripheral register structure */
//typedef struct
//{
//  uint32 CR;                                     //Control Register, offset: 0x0
//  uint32 ES;                                     //Error Status Register, offset: 0x4
//  uint8 RESERVED_0[4];
//  uint32 ERQ;                                    //Enable Request Register, offset: 0xC
//  uint8 RESERVED_1[4];
//  uint32 EEI;                                    //Enable Error Interrupt Register, offset: 0x14
//  uint8 CEEI;                                    //Clear Enable Error Interrupt Register, offset: 0x18
//  uint8 SEEI;                                    //Set Enable Error Interrupt Register, offset: 0x19
//  uint8 CERQ;                                    //Clear Enable Request Register, offset: 0x1A
//  uint8 SERQ;                                    //Set Enable Request Register, offset: 0x1B
//  uint8 CDNE;                                    //Clear DONE Status Bit Register, offset: 0x1C
//  uint8 SSRT;                                    //Set START Bit Register, offset: 0x1D
//  uint8 CERR;                                    //Clear Error Register, offset: 0x1E
//  uint8 CINT;                                    //Clear Interrupt Request Register, offset: 0x1F
//  uint8 RESERVED_2[4];
//  uint32 INT;                                    //Interrupt Request Register, offset: 0x24
//  uint8 RESERVED_3[4];
//  uint32 ERR;                                    //Error Register, offset: 0x2C
//  uint8 RESERVED_4[4];
//  uint32 HRS;                                    //Hardware Request Status Register, offset: 0x34
//  uint8 RESERVED_5[12];
//  uint32 EARS;                                   //Enable Asynchronous Request in Stop Register, offset: 0x44
//  uint8 RESERVED_6[184];
//  uint8 DCHPRI3;                                 //Channel n Priority Register, offset: 0x100
//  uint8 DCHPRI2;                                 //Channel n Priority Register, offset: 0x101
//  uint8 DCHPRI1;                                 //Channel n Priority Register, offset: 0x102
//  uint8 DCHPRI0;                                 //Channel n Priority Register, offset: 0x103
//  uint8 DCHPRI7;                                 //Channel n Priority Register, offset: 0x104
//  uint8 DCHPRI6;                                 //Channel n Priority Register, offset: 0x105
//  uint8 DCHPRI5;                                 //Channel n Priority Register, offset: 0x106
//  uint8 DCHPRI4;                                 //Channel n Priority Register, offset: 0x107
//  uint8 DCHPRI11;                                //Channel n Priority Register, offset: 0x108
//  uint8 DCHPRI10;                                //Channel n Priority Register, offset: 0x109
//  uint8 DCHPRI9;                                 //Channel n Priority Register, offset: 0x10A
//  uint8 DCHPRI8;                                 //Channel n Priority Register, offset: 0x10B
//  uint8 DCHPRI15;                                //Channel n Priority Register, offset: 0x10C
//  uint8 DCHPRI14;                                //Channel n Priority Register, offset: 0x10D
//  uint8 DCHPRI13;                                //Channel n Priority Register, offset: 0x10E
//  uint8 DCHPRI12;                                //Channel n Priority Register, offset: 0x10F
//  uint8 RESERVED_7[3824];
//  struct
//  {                                    			 //offset: 0x1000, array step: 0x20
//    uint32 SADDR;                                //TCD Source Address, array offset: 0x1000, array step: 0x20
//    uint16 SOFF;                                 //TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20
//    uint16 ATTR;                                 //TCD Transfer Attributes, array offset: 0x1006, array step: 0x20
//    union
//	{                                          	 //offset: 0x1008, array step: 0x20
//      uint32 NBYTES_MLNO;                        //TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20
//      uint32 NBYTES_MLOFFNO;                     //TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20
//      uint32 NBYTES_MLOFFYES;                    //TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20
//    };
//    uint32 SLAST;                                //TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20
//    uint32 DADDR;                                //TCD Destination Address, array offset: 0x1010, array step: 0x20
//    uint16 DOFF;                                 //TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20
//    union
//	{                                            //offset: 0x1016, array step: 0x20
//      uint16 CITER_ELINKNO;                      //TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20
//      uint16 CITER_ELINKYES;                     //TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20
//    };
//    uint32 DLAST_SGA;                            //TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20
//    uint16 CSR;                                  //TCD Control and Status, array offset: 0x101C, array step: 0x20
//    union
//	{                                            //offset: 0x101E, array step: 0x20 */
//      uint16 BITER_ELINKNO;                      //TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20
//      uint16 BITER_ELINKYES;                     //TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20
//    };
//  } TCD[16];
//} volatile DMA_BASIC_DEF;
//
//#define DMA_BASE                                 0x40008000
//#define DMA                                      (*(DMA_BASIC_DEF *)DMA_BASE)
//
///* ----------------------------------------------------------------------------
//   -- DMA - Register accessor macros
//   ---------------------------------------------------------------------------- */
//
///*!
// * @addtogroup DMA_Register_Accessor_Macros DMA - Register accessor macros
// * @{
// */
//
//
///* DMA - Register accessors */
//#define DMA_CR_REG(base)                         ((base)->CR)
//#define DMA_ES_REG(base)                         ((base)->ES)
//#define DMA_ERQ_REG(base)                        ((base)->ERQ)
//#define DMA_EEI_REG(base)                        ((base)->EEI)
//#define DMA_CEEI_REG(base)                       ((base)->CEEI)
//#define DMA_SEEI_REG(base)                       ((base)->SEEI)
//#define DMA_CERQ_REG(base)                       ((base)->CERQ)
//#define DMA_SERQ_REG(base)                       ((base)->SERQ)
//#define DMA_CDNE_REG(base)                       ((base)->CDNE)
//#define DMA_SSRT_REG(base)                       ((base)->SSRT)
//#define DMA_CERR_REG(base)                       ((base)->CERR)
//#define DMA_CINT_REG(base)                       ((base)->CINT)
//#define DMA_INT_REG(base)                        ((base)->INT)
//#define DMA_ERR_REG(base)                        ((base)->ERR)
//#define DMA_HRS_REG(base)                        ((base)->HRS)
//#define DMA_EARS_REG(base)                       ((base)->EARS)
//#define DMA_DCHPRI3_REG(base)                    ((base)->DCHPRI3)
//#define DMA_DCHPRI2_REG(base)                    ((base)->DCHPRI2)
//#define DMA_DCHPRI1_REG(base)                    ((base)->DCHPRI1)
//#define DMA_DCHPRI0_REG(base)                    ((base)->DCHPRI0)
//#define DMA_DCHPRI7_REG(base)                    ((base)->DCHPRI7)
//#define DMA_DCHPRI6_REG(base)                    ((base)->DCHPRI6)
//#define DMA_DCHPRI5_REG(base)                    ((base)->DCHPRI5)
//#define DMA_DCHPRI4_REG(base)                    ((base)->DCHPRI4)
//#define DMA_DCHPRI11_REG(base)                   ((base)->DCHPRI11)
//#define DMA_DCHPRI10_REG(base)                   ((base)->DCHPRI10)
//#define DMA_DCHPRI9_REG(base)                    ((base)->DCHPRI9)
//#define DMA_DCHPRI8_REG(base)                    ((base)->DCHPRI8)
//#define DMA_DCHPRI15_REG(base)                   ((base)->DCHPRI15)
//#define DMA_DCHPRI14_REG(base)                   ((base)->DCHPRI14)
//#define DMA_DCHPRI13_REG(base)                   ((base)->DCHPRI13)
//#define DMA_DCHPRI12_REG(base)                   ((base)->DCHPRI12)
//#define DMA_SADDR_REG(base,index)                ((base)->TCD[index].SADDR)
//#define DMA_SOFF_REG(base,index)                 ((base)->TCD[index].SOFF)
//#define DMA_ATTR_REG(base,index)                 ((base)->TCD[index].ATTR)
//#define DMA_NBYTES_MLNO_REG(base,index)          ((base)->TCD[index].NBYTES_MLNO)
//#define DMA_NBYTES_MLOFFNO_REG(base,index)       ((base)->TCD[index].NBYTES_MLOFFNO)
//#define DMA_NBYTES_MLOFFYES_REG(base,index)      ((base)->TCD[index].NBYTES_MLOFFYES)
//#define DMA_SLAST_REG(base,index)                ((base)->TCD[index].SLAST)
//#define DMA_DADDR_REG(base,index)                ((base)->TCD[index].DADDR)
//#define DMA_DOFF_REG(base,index)                 ((base)->TCD[index].DOFF)
//#define DMA_CITER_ELINKNO_REG(base,index)        ((base)->TCD[index].CITER_ELINKNO)
//#define DMA_CITER_ELINKYES_REG(base,index)       ((base)->TCD[index].CITER_ELINKYES)
//#define DMA_DLAST_SGA_REG(base,index)            ((base)->TCD[index].DLAST_SGA)
//#define DMA_CSR_REG(base,index)                  ((base)->TCD[index].CSR)
//#define DMA_BITER_ELINKNO_REG(base,index)        ((base)->TCD[index].BITER_ELINKNO)
//#define DMA_BITER_ELINKYES_REG(base,index)       ((base)->TCD[index].BITER_ELINKYES)
///* ----------------------------------------------------------------------------
//   -- DMA Register Masks
//   ---------------------------------------------------------------------------- */
//
///* CR Bit Fields */
//#define DMA_CR_EDBG_MASK                         0x2u
//#define DMA_CR_EDBG_SHIFT                        1
//#define DMA_CR_ERCA_MASK                         0x4u
//#define DMA_CR_ERCA_SHIFT                        2
//#define DMA_CR_HOE_MASK                          0x10u
//#define DMA_CR_HOE_SHIFT                         4
//#define DMA_CR_HALT_MASK                         0x20u
//#define DMA_CR_HALT_SHIFT                        5
//#define DMA_CR_CLM_MASK                          0x40u
//#define DMA_CR_CLM_SHIFT                         6
//#define DMA_CR_EMLM_MASK                         0x80u
//#define DMA_CR_EMLM_SHIFT                        7
//#define DMA_CR_ECX_MASK                          0x10000u
//#define DMA_CR_ECX_SHIFT                         16
//#define DMA_CR_CX_MASK                           0x20000u
//#define DMA_CR_CX_SHIFT                          17
///* ES Bit Fields */
//#define DMA_ES_DBE_MASK                          0x1u
//#define DMA_ES_DBE_SHIFT                         0
//#define DMA_ES_SBE_MASK                          0x2u
//#define DMA_ES_SBE_SHIFT                         1
//#define DMA_ES_SGE_MASK                          0x4u
//#define DMA_ES_SGE_SHIFT                         2
//#define DMA_ES_NCE_MASK                          0x8u
//#define DMA_ES_NCE_SHIFT                         3
//#define DMA_ES_DOE_MASK                          0x10u
//#define DMA_ES_DOE_SHIFT                         4
//#define DMA_ES_DAE_MASK                          0x20u
//#define DMA_ES_DAE_SHIFT                         5
//#define DMA_ES_SOE_MASK                          0x40u
//#define DMA_ES_SOE_SHIFT                         6
//#define DMA_ES_SAE_MASK                          0x80u
//#define DMA_ES_SAE_SHIFT                         7
//#define DMA_ES_ERRCHN_MASK                       0xF00u
//#define DMA_ES_ERRCHN_SHIFT                      8
//#define DMA_ES_ERRCHN(x)                         (((uint32)(((uint32)(x))<<DMA_ES_ERRCHN_SHIFT))&DMA_ES_ERRCHN_MASK)
//#define DMA_ES_CPE_MASK                          0x4000u
//#define DMA_ES_CPE_SHIFT                         14
//#define DMA_ES_ECX_MASK                          0x10000u
//#define DMA_ES_ECX_SHIFT                         16
//#define DMA_ES_VLD_MASK                          0x80000000u
//#define DMA_ES_VLD_SHIFT                         31
///* ERQ Bit Fields */
//#define DMA_ERQ_ERQ0_MASK                        0x1u
//#define DMA_ERQ_ERQ0_SHIFT                       0
//#define DMA_ERQ_ERQ1_MASK                        0x2u
//#define DMA_ERQ_ERQ1_SHIFT                       1
//#define DMA_ERQ_ERQ2_MASK                        0x4u
//#define DMA_ERQ_ERQ2_SHIFT                       2
//#define DMA_ERQ_ERQ3_MASK                        0x8u
//#define DMA_ERQ_ERQ3_SHIFT                       3
//#define DMA_ERQ_ERQ4_MASK                        0x10u
//#define DMA_ERQ_ERQ4_SHIFT                       4
//#define DMA_ERQ_ERQ5_MASK                        0x20u
//#define DMA_ERQ_ERQ5_SHIFT                       5
//#define DMA_ERQ_ERQ6_MASK                        0x40u
//#define DMA_ERQ_ERQ6_SHIFT                       6
//#define DMA_ERQ_ERQ7_MASK                        0x80u
//#define DMA_ERQ_ERQ7_SHIFT                       7
//#define DMA_ERQ_ERQ8_MASK                        0x100u
//#define DMA_ERQ_ERQ8_SHIFT                       8
//#define DMA_ERQ_ERQ9_MASK                        0x200u
//#define DMA_ERQ_ERQ9_SHIFT                       9
//#define DMA_ERQ_ERQ10_MASK                       0x400u
//#define DMA_ERQ_ERQ10_SHIFT                      10
//#define DMA_ERQ_ERQ11_MASK                       0x800u
//#define DMA_ERQ_ERQ11_SHIFT                      11
//#define DMA_ERQ_ERQ12_MASK                       0x1000u
//#define DMA_ERQ_ERQ12_SHIFT                      12
//#define DMA_ERQ_ERQ13_MASK                       0x2000u
//#define DMA_ERQ_ERQ13_SHIFT                      13
//#define DMA_ERQ_ERQ14_MASK                       0x4000u
//#define DMA_ERQ_ERQ14_SHIFT                      14
//#define DMA_ERQ_ERQ15_MASK                       0x8000u
//#define DMA_ERQ_ERQ15_SHIFT                      15
///* EEI Bit Fields */
//#define DMA_EEI_EEI0_MASK                        0x1u
//#define DMA_EEI_EEI0_SHIFT                       0
//#define DMA_EEI_EEI1_MASK                        0x2u
//#define DMA_EEI_EEI1_SHIFT                       1
//#define DMA_EEI_EEI2_MASK                        0x4u
//#define DMA_EEI_EEI2_SHIFT                       2
//#define DMA_EEI_EEI3_MASK                        0x8u
//#define DMA_EEI_EEI3_SHIFT                       3
//#define DMA_EEI_EEI4_MASK                        0x10u
//#define DMA_EEI_EEI4_SHIFT                       4
//#define DMA_EEI_EEI5_MASK                        0x20u
//#define DMA_EEI_EEI5_SHIFT                       5
//#define DMA_EEI_EEI6_MASK                        0x40u
//#define DMA_EEI_EEI6_SHIFT                       6
//#define DMA_EEI_EEI7_MASK                        0x80u
//#define DMA_EEI_EEI7_SHIFT                       7
//#define DMA_EEI_EEI8_MASK                        0x100u
//#define DMA_EEI_EEI8_SHIFT                       8
//#define DMA_EEI_EEI9_MASK                        0x200u
//#define DMA_EEI_EEI9_SHIFT                       9
//#define DMA_EEI_EEI10_MASK                       0x400u
//#define DMA_EEI_EEI10_SHIFT                      10
//#define DMA_EEI_EEI11_MASK                       0x800u
//#define DMA_EEI_EEI11_SHIFT                      11
//#define DMA_EEI_EEI12_MASK                       0x1000u
//#define DMA_EEI_EEI12_SHIFT                      12
//#define DMA_EEI_EEI13_MASK                       0x2000u
//#define DMA_EEI_EEI13_SHIFT                      13
//#define DMA_EEI_EEI14_MASK                       0x4000u
//#define DMA_EEI_EEI14_SHIFT                      14
//#define DMA_EEI_EEI15_MASK                       0x8000u
//#define DMA_EEI_EEI15_SHIFT                      15
///* CEEI Bit Fields */
//#define DMA_CEEI_CEEI_MASK                       0xFu
//#define DMA_CEEI_CEEI_SHIFT                      0
//#define DMA_CEEI_CEEI(x)                         (((uint8)(((uint8)(x))<<DMA_CEEI_CEEI_SHIFT))&DMA_CEEI_CEEI_MASK)
//#define DMA_CEEI_CAEE_MASK                       0x40u
//#define DMA_CEEI_CAEE_SHIFT                      6
//#define DMA_CEEI_NOP_MASK                        0x80u
//#define DMA_CEEI_NOP_SHIFT                       7
///* SEEI Bit Fields */
//#define DMA_SEEI_SEEI_MASK                       0xFu
//#define DMA_SEEI_SEEI_SHIFT                      0
//#define DMA_SEEI_SEEI(x)                         (((uint8)(((uint8)(x))<<DMA_SEEI_SEEI_SHIFT))&DMA_SEEI_SEEI_MASK)
//#define DMA_SEEI_SAEE_MASK                       0x40u
//#define DMA_SEEI_SAEE_SHIFT                      6
//#define DMA_SEEI_NOP_MASK                        0x80u
//#define DMA_SEEI_NOP_SHIFT                       7
///* CERQ Bit Fields */
//#define DMA_CERQ_CERQ_MASK                       0xFu
//#define DMA_CERQ_CERQ_SHIFT                      0
//#define DMA_CERQ_CERQ(x)                         (((uint8)(((uint8)(x))<<DMA_CERQ_CERQ_SHIFT))&DMA_CERQ_CERQ_MASK)
//#define DMA_CERQ_CAER_MASK                       0x40u
//#define DMA_CERQ_CAER_SHIFT                      6
//#define DMA_CERQ_NOP_MASK                        0x80u
//#define DMA_CERQ_NOP_SHIFT                       7
///* SERQ Bit Fields */
//#define DMA_SERQ_SERQ_MASK                       0xFu
//#define DMA_SERQ_SERQ_SHIFT                      0
//#define DMA_SERQ_SERQ(x)                         (((uint8)(((uint8)(x))<<DMA_SERQ_SERQ_SHIFT))&DMA_SERQ_SERQ_MASK)
//#define DMA_SERQ_SAER_MASK                       0x40u
//#define DMA_SERQ_SAER_SHIFT                      6
//#define DMA_SERQ_NOP_MASK                        0x80u
//#define DMA_SERQ_NOP_SHIFT                       7
///* CDNE Bit Fields */
//#define DMA_CDNE_CDNE_MASK                       0xFu
//#define DMA_CDNE_CDNE_SHIFT                      0
//#define DMA_CDNE_CDNE(x)                         (((uint8)(((uint8)(x))<<DMA_CDNE_CDNE_SHIFT))&DMA_CDNE_CDNE_MASK)
//#define DMA_CDNE_CADN_MASK                       0x40u
//#define DMA_CDNE_CADN_SHIFT                      6
//#define DMA_CDNE_NOP_MASK                        0x80u
//#define DMA_CDNE_NOP_SHIFT                       7
///* SSRT Bit Fields */
//#define DMA_SSRT_SSRT_MASK                       0xFu
//#define DMA_SSRT_SSRT_SHIFT                      0
//#define DMA_SSRT_SSRT(x)                         (((uint8)(((uint8)(x))<<DMA_SSRT_SSRT_SHIFT))&DMA_SSRT_SSRT_MASK)
//#define DMA_SSRT_SAST_MASK                       0x40u
//#define DMA_SSRT_SAST_SHIFT                      6
//#define DMA_SSRT_NOP_MASK                        0x80u
//#define DMA_SSRT_NOP_SHIFT                       7
///* CERR Bit Fields */
//#define DMA_CERR_CERR_MASK                       0xFu
//#define DMA_CERR_CERR_SHIFT                      0
//#define DMA_CERR_CERR(x)                         (((uint8)(((uint8)(x))<<DMA_CERR_CERR_SHIFT))&DMA_CERR_CERR_MASK)
//#define DMA_CERR_CAEI_MASK                       0x40u
//#define DMA_CERR_CAEI_SHIFT                      6
//#define DMA_CERR_NOP_MASK                        0x80u
//#define DMA_CERR_NOP_SHIFT                       7
///* CINT Bit Fields */
//#define DMA_CINT_CINT_MASK                       0xFu
//#define DMA_CINT_CINT_SHIFT                      0
//#define DMA_CINT_CINT(x)                         (((uint8)(((uint8)(x))<<DMA_CINT_CINT_SHIFT))&DMA_CINT_CINT_MASK)
//#define DMA_CINT_CAIR_MASK                       0x40u
//#define DMA_CINT_CAIR_SHIFT                      6
//#define DMA_CINT_NOP_MASK                        0x80u
//#define DMA_CINT_NOP_SHIFT                       7
///* INT Bit Fields */
//#define DMA_INT_INT0_MASK                        0x1u
//#define DMA_INT_INT0_SHIFT                       0
//#define DMA_INT_INT1_MASK                        0x2u
//#define DMA_INT_INT1_SHIFT                       1
//#define DMA_INT_INT2_MASK                        0x4u
//#define DMA_INT_INT2_SHIFT                       2
//#define DMA_INT_INT3_MASK                        0x8u
//#define DMA_INT_INT3_SHIFT                       3
//#define DMA_INT_INT4_MASK                        0x10u
//#define DMA_INT_INT4_SHIFT                       4
//#define DMA_INT_INT5_MASK                        0x20u
//#define DMA_INT_INT5_SHIFT                       5
//#define DMA_INT_INT6_MASK                        0x40u
//#define DMA_INT_INT6_SHIFT                       6
//#define DMA_INT_INT7_MASK                        0x80u
//#define DMA_INT_INT7_SHIFT                       7
//#define DMA_INT_INT8_MASK                        0x100u
//#define DMA_INT_INT8_SHIFT                       8
//#define DMA_INT_INT9_MASK                        0x200u
//#define DMA_INT_INT9_SHIFT                       9
//#define DMA_INT_INT10_MASK                       0x400u
//#define DMA_INT_INT10_SHIFT                      10
//#define DMA_INT_INT11_MASK                       0x800u
//#define DMA_INT_INT11_SHIFT                      11
//#define DMA_INT_INT12_MASK                       0x1000u
//#define DMA_INT_INT12_SHIFT                      12
//#define DMA_INT_INT13_MASK                       0x2000u
//#define DMA_INT_INT13_SHIFT                      13
//#define DMA_INT_INT14_MASK                       0x4000u
//#define DMA_INT_INT14_SHIFT                      14
//#define DMA_INT_INT15_MASK                       0x8000u
//#define DMA_INT_INT15_SHIFT                      15
///* ERR Bit Fields */
//#define DMA_ERR_ERR0_MASK                        0x1u
//#define DMA_ERR_ERR0_SHIFT                       0
//#define DMA_ERR_ERR1_MASK                        0x2u
//#define DMA_ERR_ERR1_SHIFT                       1
//#define DMA_ERR_ERR2_MASK                        0x4u
//#define DMA_ERR_ERR2_SHIFT                       2
//#define DMA_ERR_ERR3_MASK                        0x8u
//#define DMA_ERR_ERR3_SHIFT                       3
//#define DMA_ERR_ERR4_MASK                        0x10u
//#define DMA_ERR_ERR4_SHIFT                       4
//#define DMA_ERR_ERR5_MASK                        0x20u
//#define DMA_ERR_ERR5_SHIFT                       5
//#define DMA_ERR_ERR6_MASK                        0x40u
//#define DMA_ERR_ERR6_SHIFT                       6
//#define DMA_ERR_ERR7_MASK                        0x80u
//#define DMA_ERR_ERR7_SHIFT                       7
//#define DMA_ERR_ERR8_MASK                        0x100u
//#define DMA_ERR_ERR8_SHIFT                       8
//#define DMA_ERR_ERR9_MASK                        0x200u
//#define DMA_ERR_ERR9_SHIFT                       9
//#define DMA_ERR_ERR10_MASK                       0x400u
//#define DMA_ERR_ERR10_SHIFT                      10
//#define DMA_ERR_ERR11_MASK                       0x800u
//#define DMA_ERR_ERR11_SHIFT                      11
//#define DMA_ERR_ERR12_MASK                       0x1000u
//#define DMA_ERR_ERR12_SHIFT                      12
//#define DMA_ERR_ERR13_MASK                       0x2000u
//#define DMA_ERR_ERR13_SHIFT                      13
//#define DMA_ERR_ERR14_MASK                       0x4000u
//#define DMA_ERR_ERR14_SHIFT                      14
//#define DMA_ERR_ERR15_MASK                       0x8000u
//#define DMA_ERR_ERR15_SHIFT                      15
///* HRS Bit Fields */
//#define DMA_HRS_HRS0_MASK                        0x1u
//#define DMA_HRS_HRS0_SHIFT                       0
//#define DMA_HRS_HRS1_MASK                        0x2u
//#define DMA_HRS_HRS1_SHIFT                       1
//#define DMA_HRS_HRS2_MASK                        0x4u
//#define DMA_HRS_HRS2_SHIFT                       2
//#define DMA_HRS_HRS3_MASK                        0x8u
//#define DMA_HRS_HRS3_SHIFT                       3
//#define DMA_HRS_HRS4_MASK                        0x10u
//#define DMA_HRS_HRS4_SHIFT                       4
//#define DMA_HRS_HRS5_MASK                        0x20u
//#define DMA_HRS_HRS5_SHIFT                       5
//#define DMA_HRS_HRS6_MASK                        0x40u
//#define DMA_HRS_HRS6_SHIFT                       6
//#define DMA_HRS_HRS7_MASK                        0x80u
//#define DMA_HRS_HRS7_SHIFT                       7
//#define DMA_HRS_HRS8_MASK                        0x100u
//#define DMA_HRS_HRS8_SHIFT                       8
//#define DMA_HRS_HRS9_MASK                        0x200u
//#define DMA_HRS_HRS9_SHIFT                       9
//#define DMA_HRS_HRS10_MASK                       0x400u
//#define DMA_HRS_HRS10_SHIFT                      10
//#define DMA_HRS_HRS11_MASK                       0x800u
//#define DMA_HRS_HRS11_SHIFT                      11
//#define DMA_HRS_HRS12_MASK                       0x1000u
//#define DMA_HRS_HRS12_SHIFT                      12
//#define DMA_HRS_HRS13_MASK                       0x2000u
//#define DMA_HRS_HRS13_SHIFT                      13
//#define DMA_HRS_HRS14_MASK                       0x4000u
//#define DMA_HRS_HRS14_SHIFT                      14
//#define DMA_HRS_HRS15_MASK                       0x8000u
//#define DMA_HRS_HRS15_SHIFT                      15
///* EARS Bit Fields */
//#define DMA_EARS_EDREQ_0_MASK                    0x1u
//#define DMA_EARS_EDREQ_0_SHIFT                   0
//#define DMA_EARS_EDREQ_1_MASK                    0x2u
//#define DMA_EARS_EDREQ_1_SHIFT                   1
//#define DMA_EARS_EDREQ_2_MASK                    0x4u
//#define DMA_EARS_EDREQ_2_SHIFT                   2
//#define DMA_EARS_EDREQ_3_MASK                    0x8u
//#define DMA_EARS_EDREQ_3_SHIFT                   3
//#define DMA_EARS_EDREQ_4_MASK                    0x10u
//#define DMA_EARS_EDREQ_4_SHIFT                   4
//#define DMA_EARS_EDREQ_5_MASK                    0x20u
//#define DMA_EARS_EDREQ_5_SHIFT                   5
//#define DMA_EARS_EDREQ_6_MASK                    0x40u
//#define DMA_EARS_EDREQ_6_SHIFT                   6
//#define DMA_EARS_EDREQ_7_MASK                    0x80u
//#define DMA_EARS_EDREQ_7_SHIFT                   7
//#define DMA_EARS_EDREQ_8_MASK                    0x100u
//#define DMA_EARS_EDREQ_8_SHIFT                   8
//#define DMA_EARS_EDREQ_9_MASK                    0x200u
//#define DMA_EARS_EDREQ_9_SHIFT                   9
//#define DMA_EARS_EDREQ_10_MASK                   0x400u
//#define DMA_EARS_EDREQ_10_SHIFT                  10
//#define DMA_EARS_EDREQ_11_MASK                   0x800u
//#define DMA_EARS_EDREQ_11_SHIFT                  11
//#define DMA_EARS_EDREQ_12_MASK                   0x1000u
//#define DMA_EARS_EDREQ_12_SHIFT                  12
//#define DMA_EARS_EDREQ_13_MASK                   0x2000u
//#define DMA_EARS_EDREQ_13_SHIFT                  13
//#define DMA_EARS_EDREQ_14_MASK                   0x4000u
//#define DMA_EARS_EDREQ_14_SHIFT                  14
//#define DMA_EARS_EDREQ_15_MASK                   0x8000u
//#define DMA_EARS_EDREQ_15_SHIFT                  15
///* DCHPRI3 Bit Fields */
//#define DMA_DCHPRI3_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI3_CHPRI_SHIFT                  0
//#define DMA_DCHPRI3_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI3_CHPRI_SHIFT))&DMA_DCHPRI3_CHPRI_MASK)
//#define DMA_DCHPRI3_DPA_MASK                     0x40u
//#define DMA_DCHPRI3_DPA_SHIFT                    6
//#define DMA_DCHPRI3_ECP_MASK                     0x80u
//#define DMA_DCHPRI3_ECP_SHIFT                    7
///* DCHPRI2 Bit Fields */
//#define DMA_DCHPRI2_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI2_CHPRI_SHIFT                  0
//#define DMA_DCHPRI2_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI2_CHPRI_SHIFT))&DMA_DCHPRI2_CHPRI_MASK)
//#define DMA_DCHPRI2_DPA_MASK                     0x40u
//#define DMA_DCHPRI2_DPA_SHIFT                    6
//#define DMA_DCHPRI2_ECP_MASK                     0x80u
//#define DMA_DCHPRI2_ECP_SHIFT                    7
///* DCHPRI1 Bit Fields */
//#define DMA_DCHPRI1_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI1_CHPRI_SHIFT                  0
//#define DMA_DCHPRI1_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI1_CHPRI_SHIFT))&DMA_DCHPRI1_CHPRI_MASK)
//#define DMA_DCHPRI1_DPA_MASK                     0x40u
//#define DMA_DCHPRI1_DPA_SHIFT                    6
//#define DMA_DCHPRI1_ECP_MASK                     0x80u
//#define DMA_DCHPRI1_ECP_SHIFT                    7
///* DCHPRI0 Bit Fields */
//#define DMA_DCHPRI0_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI0_CHPRI_SHIFT                  0
//#define DMA_DCHPRI0_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI0_CHPRI_SHIFT))&DMA_DCHPRI0_CHPRI_MASK)
//#define DMA_DCHPRI0_DPA_MASK                     0x40u
//#define DMA_DCHPRI0_DPA_SHIFT                    6
//#define DMA_DCHPRI0_ECP_MASK                     0x80u
//#define DMA_DCHPRI0_ECP_SHIFT                    7
///* DCHPRI7 Bit Fields */
//#define DMA_DCHPRI7_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI7_CHPRI_SHIFT                  0
//#define DMA_DCHPRI7_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI7_CHPRI_SHIFT))&DMA_DCHPRI7_CHPRI_MASK)
//#define DMA_DCHPRI7_DPA_MASK                     0x40u
//#define DMA_DCHPRI7_DPA_SHIFT                    6
//#define DMA_DCHPRI7_ECP_MASK                     0x80u
//#define DMA_DCHPRI7_ECP_SHIFT                    7
///* DCHPRI6 Bit Fields */
//#define DMA_DCHPRI6_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI6_CHPRI_SHIFT                  0
//#define DMA_DCHPRI6_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI6_CHPRI_SHIFT))&DMA_DCHPRI6_CHPRI_MASK)
//#define DMA_DCHPRI6_DPA_MASK                     0x40u
//#define DMA_DCHPRI6_DPA_SHIFT                    6
//#define DMA_DCHPRI6_ECP_MASK                     0x80u
//#define DMA_DCHPRI6_ECP_SHIFT                    7
///* DCHPRI5 Bit Fields */
//#define DMA_DCHPRI5_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI5_CHPRI_SHIFT                  0
//#define DMA_DCHPRI5_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI5_CHPRI_SHIFT))&DMA_DCHPRI5_CHPRI_MASK)
//#define DMA_DCHPRI5_DPA_MASK                     0x40u
//#define DMA_DCHPRI5_DPA_SHIFT                    6
//#define DMA_DCHPRI5_ECP_MASK                     0x80u
//#define DMA_DCHPRI5_ECP_SHIFT                    7
///* DCHPRI4 Bit Fields */
//#define DMA_DCHPRI4_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI4_CHPRI_SHIFT                  0
//#define DMA_DCHPRI4_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI4_CHPRI_SHIFT))&DMA_DCHPRI4_CHPRI_MASK)
//#define DMA_DCHPRI4_DPA_MASK                     0x40u
//#define DMA_DCHPRI4_DPA_SHIFT                    6
//#define DMA_DCHPRI4_ECP_MASK                     0x80u
//#define DMA_DCHPRI4_ECP_SHIFT                    7
///* DCHPRI11 Bit Fields */
//#define DMA_DCHPRI11_CHPRI_MASK                  0xFu
//#define DMA_DCHPRI11_CHPRI_SHIFT                 0
//#define DMA_DCHPRI11_CHPRI(x)                    (((uint8)(((uint8)(x))<<DMA_DCHPRI11_CHPRI_SHIFT))&DMA_DCHPRI11_CHPRI_MASK)
//#define DMA_DCHPRI11_DPA_MASK                    0x40u
//#define DMA_DCHPRI11_DPA_SHIFT                   6
//#define DMA_DCHPRI11_ECP_MASK                    0x80u
//#define DMA_DCHPRI11_ECP_SHIFT                   7
///* DCHPRI10 Bit Fields */
//#define DMA_DCHPRI10_CHPRI_MASK                  0xFu
//#define DMA_DCHPRI10_CHPRI_SHIFT                 0
//#define DMA_DCHPRI10_CHPRI(x)                    (((uint8)(((uint8)(x))<<DMA_DCHPRI10_CHPRI_SHIFT))&DMA_DCHPRI10_CHPRI_MASK)
//#define DMA_DCHPRI10_DPA_MASK                    0x40u
//#define DMA_DCHPRI10_DPA_SHIFT                   6
//#define DMA_DCHPRI10_ECP_MASK                    0x80u
//#define DMA_DCHPRI10_ECP_SHIFT                   7
///* DCHPRI9 Bit Fields */
//#define DMA_DCHPRI9_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI9_CHPRI_SHIFT                  0
//#define DMA_DCHPRI9_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI9_CHPRI_SHIFT))&DMA_DCHPRI9_CHPRI_MASK)
//#define DMA_DCHPRI9_DPA_MASK                     0x40u
//#define DMA_DCHPRI9_DPA_SHIFT                    6
//#define DMA_DCHPRI9_ECP_MASK                     0x80u
//#define DMA_DCHPRI9_ECP_SHIFT                    7
///* DCHPRI8 Bit Fields */
//#define DMA_DCHPRI8_CHPRI_MASK                   0xFu
//#define DMA_DCHPRI8_CHPRI_SHIFT                  0
//#define DMA_DCHPRI8_CHPRI(x)                     (((uint8)(((uint8)(x))<<DMA_DCHPRI8_CHPRI_SHIFT))&DMA_DCHPRI8_CHPRI_MASK)
//#define DMA_DCHPRI8_DPA_MASK                     0x40u
//#define DMA_DCHPRI8_DPA_SHIFT                    6
//#define DMA_DCHPRI8_ECP_MASK                     0x80u
//#define DMA_DCHPRI8_ECP_SHIFT                    7
///* DCHPRI15 Bit Fields */
//#define DMA_DCHPRI15_CHPRI_MASK                  0xFu
//#define DMA_DCHPRI15_CHPRI_SHIFT                 0
//#define DMA_DCHPRI15_CHPRI(x)                    (((uint8)(((uint8)(x))<<DMA_DCHPRI15_CHPRI_SHIFT))&DMA_DCHPRI15_CHPRI_MASK)
//#define DMA_DCHPRI15_DPA_MASK                    0x40u
//#define DMA_DCHPRI15_DPA_SHIFT                   6
//#define DMA_DCHPRI15_ECP_MASK                    0x80u
//#define DMA_DCHPRI15_ECP_SHIFT                   7
///* DCHPRI14 Bit Fields */
//#define DMA_DCHPRI14_CHPRI_MASK                  0xFu
//#define DMA_DCHPRI14_CHPRI_SHIFT                 0
//#define DMA_DCHPRI14_CHPRI(x)                    (((uint8)(((uint8)(x))<<DMA_DCHPRI14_CHPRI_SHIFT))&DMA_DCHPRI14_CHPRI_MASK)
//#define DMA_DCHPRI14_DPA_MASK                    0x40u
//#define DMA_DCHPRI14_DPA_SHIFT                   6
//#define DMA_DCHPRI14_ECP_MASK                    0x80u
//#define DMA_DCHPRI14_ECP_SHIFT                   7
///* DCHPRI13 Bit Fields */
//#define DMA_DCHPRI13_CHPRI_MASK                  0xFu
//#define DMA_DCHPRI13_CHPRI_SHIFT                 0
//#define DMA_DCHPRI13_CHPRI(x)                    (((uint8)(((uint8)(x))<<DMA_DCHPRI13_CHPRI_SHIFT))&DMA_DCHPRI13_CHPRI_MASK)
//#define DMA_DCHPRI13_DPA_MASK                    0x40u
//#define DMA_DCHPRI13_DPA_SHIFT                   6
//#define DMA_DCHPRI13_ECP_MASK                    0x80u
//#define DMA_DCHPRI13_ECP_SHIFT                   7
///* DCHPRI12 Bit Fields */
//#define DMA_DCHPRI12_CHPRI_MASK                  0xFu
//#define DMA_DCHPRI12_CHPRI_SHIFT                 0
//#define DMA_DCHPRI12_CHPRI(x)                    (((uint8)(((uint8)(x))<<DMA_DCHPRI12_CHPRI_SHIFT))&DMA_DCHPRI12_CHPRI_MASK)
//#define DMA_DCHPRI12_DPA_MASK                    0x40u
//#define DMA_DCHPRI12_DPA_SHIFT                   6
//#define DMA_DCHPRI12_ECP_MASK                    0x80u
//#define DMA_DCHPRI12_ECP_SHIFT                   7
///* SADDR Bit Fields */
//#define DMA_SADDR_SADDR_MASK                     0xFFFFFFFFu
//#define DMA_SADDR_SADDR_SHIFT                    0
//#define DMA_SADDR_SADDR(x)                       (((uint32)(((uint32)(x))<<DMA_SADDR_SADDR_SHIFT))&DMA_SADDR_SADDR_MASK)
///* SOFF Bit Fields */
//#define DMA_SOFF_SOFF_MASK                       0xFFFFu
//#define DMA_SOFF_SOFF_SHIFT                      0
//#define DMA_SOFF_SOFF(x)                         (((uint16)(((uint16)(x))<<DMA_SOFF_SOFF_SHIFT))&DMA_SOFF_SOFF_MASK)
///* ATTR Bit Fields */
//#define DMA_ATTR_DSIZE_MASK                      0x7u
//#define DMA_ATTR_DSIZE_SHIFT                     0
//#define DMA_ATTR_DSIZE(x)                        (((uint16)(((uint16)(x))<<DMA_ATTR_DSIZE_SHIFT))&DMA_ATTR_DSIZE_MASK)
//#define DMA_ATTR_DMOD_MASK                       0xF8u
//#define DMA_ATTR_DMOD_SHIFT                      3
//#define DMA_ATTR_DMOD(x)                         (((uint16)(((uint16)(x))<<DMA_ATTR_DMOD_SHIFT))&DMA_ATTR_DMOD_MASK)
//#define DMA_ATTR_SSIZE_MASK                      0x700u
//#define DMA_ATTR_SSIZE_SHIFT                     8
//#define DMA_ATTR_SSIZE(x)                        (((uint16)(((uint16)(x))<<DMA_ATTR_SSIZE_SHIFT))&DMA_ATTR_SSIZE_MASK)
//#define DMA_ATTR_SMOD_MASK                       0xF800u
//#define DMA_ATTR_SMOD_SHIFT                      11
//#define DMA_ATTR_SMOD(x)                         (((uint16)(((uint16)(x))<<DMA_ATTR_SMOD_SHIFT))&DMA_ATTR_SMOD_MASK)
///* NBYTES_MLNO Bit Fields */
//#define DMA_NBYTES_MLNO_NBYTES_MASK              0xFFFFFFFFu
//#define DMA_NBYTES_MLNO_NBYTES_SHIFT             0
//#define DMA_NBYTES_MLNO_NBYTES(x)                (((uint32)(((uint32)(x))<<DMA_NBYTES_MLNO_NBYTES_SHIFT))&DMA_NBYTES_MLNO_NBYTES_MASK)
///* NBYTES_MLOFFNO Bit Fields */
//#define DMA_NBYTES_MLOFFNO_NBYTES_MASK           0x3FFFFFFFu
//#define DMA_NBYTES_MLOFFNO_NBYTES_SHIFT          0
//#define DMA_NBYTES_MLOFFNO_NBYTES(x)             (((uint32)(((uint32)(x))<<DMA_NBYTES_MLOFFNO_NBYTES_SHIFT))&DMA_NBYTES_MLOFFNO_NBYTES_MASK)
//#define DMA_NBYTES_MLOFFNO_DMLOE_MASK            0x40000000u
//#define DMA_NBYTES_MLOFFNO_DMLOE_SHIFT           30
//#define DMA_NBYTES_MLOFFNO_SMLOE_MASK            0x80000000u
//#define DMA_NBYTES_MLOFFNO_SMLOE_SHIFT           31
///* NBYTES_MLOFFYES Bit Fields */
//#define DMA_NBYTES_MLOFFYES_NBYTES_MASK          0x3FFu
//#define DMA_NBYTES_MLOFFYES_NBYTES_SHIFT         0
//#define DMA_NBYTES_MLOFFYES_NBYTES(x)            (((uint32)(((uint32)(x))<<DMA_NBYTES_MLOFFYES_NBYTES_SHIFT))&DMA_NBYTES_MLOFFYES_NBYTES_MASK)
//#define DMA_NBYTES_MLOFFYES_MLOFF_MASK           0x3FFFFC00u
//#define DMA_NBYTES_MLOFFYES_MLOFF_SHIFT          10
//#define DMA_NBYTES_MLOFFYES_MLOFF(x)             (((uint32)(((uint32)(x))<<DMA_NBYTES_MLOFFYES_MLOFF_SHIFT))&DMA_NBYTES_MLOFFYES_MLOFF_MASK)
//#define DMA_NBYTES_MLOFFYES_DMLOE_MASK           0x40000000u
//#define DMA_NBYTES_MLOFFYES_DMLOE_SHIFT          30
//#define DMA_NBYTES_MLOFFYES_SMLOE_MASK           0x80000000u
//#define DMA_NBYTES_MLOFFYES_SMLOE_SHIFT          31
///* SLAST Bit Fields */
//#define DMA_SLAST_SLAST_MASK                     0xFFFFFFFFu
//#define DMA_SLAST_SLAST_SHIFT                    0
//#define DMA_SLAST_SLAST(x)                       (((uint32)(((uint32)(x))<<DMA_SLAST_SLAST_SHIFT))&DMA_SLAST_SLAST_MASK)
///* DADDR Bit Fields */
//#define DMA_DADDR_DADDR_MASK                     0xFFFFFFFFu
//#define DMA_DADDR_DADDR_SHIFT                    0
//#define DMA_DADDR_DADDR(x)                       (((uint32)(((uint32)(x))<<DMA_DADDR_DADDR_SHIFT))&DMA_DADDR_DADDR_MASK)
///* DOFF Bit Fields */
//#define DMA_DOFF_DOFF_MASK                       0xFFFFu
//#define DMA_DOFF_DOFF_SHIFT                      0
//#define DMA_DOFF_DOFF(x)                         (((uint16)(((uint16)(x))<<DMA_DOFF_DOFF_SHIFT))&DMA_DOFF_DOFF_MASK)
///* CITER_ELINKNO Bit Fields */
//#define DMA_CITER_ELINKNO_CITER_MASK             0x7FFFu
//#define DMA_CITER_ELINKNO_CITER_SHIFT            0
//#define DMA_CITER_ELINKNO_CITER(x)               (((uint16)(((uint16)(x))<<DMA_CITER_ELINKNO_CITER_SHIFT))&DMA_CITER_ELINKNO_CITER_MASK)
//#define DMA_CITER_ELINKNO_ELINK_MASK             0x8000u
//#define DMA_CITER_ELINKNO_ELINK_SHIFT            15
///* CITER_ELINKYES Bit Fields */
//#define DMA_CITER_ELINKYES_CITER_MASK            0x1FFu
//#define DMA_CITER_ELINKYES_CITER_SHIFT           0
//#define DMA_CITER_ELINKYES_CITER(x)              (((uint16)(((uint16)(x))<<DMA_CITER_ELINKYES_CITER_SHIFT))&DMA_CITER_ELINKYES_CITER_MASK)
//#define DMA_CITER_ELINKYES_LINKCH_MASK           0x1E00u
//#define DMA_CITER_ELINKYES_LINKCH_SHIFT          9
//#define DMA_CITER_ELINKYES_LINKCH(x)             (((uint16)(((uint16)(x))<<DMA_CITER_ELINKYES_LINKCH_SHIFT))&DMA_CITER_ELINKYES_LINKCH_MASK)
//#define DMA_CITER_ELINKYES_ELINK_MASK            0x8000u
//#define DMA_CITER_ELINKYES_ELINK_SHIFT           15
///* DLAST_SGA Bit Fields */
//#define DMA_DLAST_SGA_DLASTSGA_MASK              0xFFFFFFFFu
//#define DMA_DLAST_SGA_DLASTSGA_SHIFT             0
//#define DMA_DLAST_SGA_DLASTSGA(x)                (((uint32)(((uint32)(x))<<DMA_DLAST_SGA_DLASTSGA_SHIFT))&DMA_DLAST_SGA_DLASTSGA_MASK)
///* CSR Bit Fields */
//#define DMA_CSR_START_MASK                       0x1u
//#define DMA_CSR_START_SHIFT                      0
//#define DMA_CSR_INTMAJOR_MASK                    0x2u
//#define DMA_CSR_INTMAJOR_SHIFT                   1
//#define DMA_CSR_INTHALF_MASK                     0x4u
//#define DMA_CSR_INTHALF_SHIFT                    2
//#define DMA_CSR_DREQ_MASK                        0x8u
//#define DMA_CSR_DREQ_SHIFT                       3
//#define DMA_CSR_ESG_MASK                         0x10u
//#define DMA_CSR_ESG_SHIFT                        4
//#define DMA_CSR_MAJORELINK_MASK                  0x20u
//#define DMA_CSR_MAJORELINK_SHIFT                 5
//#define DMA_CSR_ACTIVE_MASK                      0x40u
//#define DMA_CSR_ACTIVE_SHIFT                     6
//#define DMA_CSR_DONE_MASK                        0x80u
//#define DMA_CSR_DONE_SHIFT                       7
//#define DMA_CSR_MAJORLINKCH_MASK                 0xF00u
//#define DMA_CSR_MAJORLINKCH_SHIFT                8
//#define DMA_CSR_MAJORLINKCH(x)                   (((uint16)(((uint16)(x))<<DMA_CSR_MAJORLINKCH_SHIFT))&DMA_CSR_MAJORLINKCH_MASK)
//#define DMA_CSR_BWC_MASK                         0xC000u
//#define DMA_CSR_BWC_SHIFT                        14
//#define DMA_CSR_BWC(x)                           (((uint16)(((uint16)(x))<<DMA_CSR_BWC_SHIFT))&DMA_CSR_BWC_MASK)
///* BITER_ELINKNO Bit Fields */
//#define DMA_BITER_ELINKNO_BITER_MASK             0x7FFFu
//#define DMA_BITER_ELINKNO_BITER_SHIFT            0
//#define DMA_BITER_ELINKNO_BITER(x)               (((uint16)(((uint16)(x))<<DMA_BITER_ELINKNO_BITER_SHIFT))&DMA_BITER_ELINKNO_BITER_MASK)
//#define DMA_BITER_ELINKNO_ELINK_MASK             0x8000u
//#define DMA_BITER_ELINKNO_ELINK_SHIFT            15
///* BITER_ELINKYES Bit Fields */
//#define DMA_BITER_ELINKYES_BITER_MASK            0x1FFu
//#define DMA_BITER_ELINKYES_BITER_SHIFT           0
//#define DMA_BITER_ELINKYES_BITER(x)              (((uint16)(((uint16)(x))<<DMA_BITER_ELINKYES_BITER_SHIFT))&DMA_BITER_ELINKYES_BITER_MASK)
//#define DMA_BITER_ELINKYES_LINKCH_MASK           0x1E00u
//#define DMA_BITER_ELINKYES_LINKCH_SHIFT          9
//#define DMA_BITER_ELINKYES_LINKCH(x)             (((uint16)(((uint16)(x))<<DMA_BITER_ELINKYES_LINKCH_SHIFT))&DMA_BITER_ELINKYES_LINKCH_MASK)
//#define DMA_BITER_ELINKYES_ELINK_MASK            0x8000u
//#define DMA_BITER_ELINKYES_ELINK_SHIFT           15
//
//
//
//
///* ----------------------------------------------------------------------------
//   -- DMA
//   ---------------------------------------------------------------------------- */
//
///*!
// * @addtogroup DMA_Peripheral DMA
// * @{
// */
//
///** DMA - Peripheral register structure */
//typedef struct DMA_MemMap {
//  uint32 CR;                                     /**< Control Register, offset: 0x0 */
//  uint32 ES;                                     /**< Error Status Register, offset: 0x4 */
//  uint8 RESERVED_0[4];
//  uint32 ERQ;                                    /**< Enable Request Register, offset: 0xC */
//  uint8 RESERVED_1[4];
//  uint32 EEI;                                    /**< Enable Error Interrupt Register, offset: 0x14 */
//  uint8 CEEI;                                    /**< Clear Enable Error Interrupt Register, offset: 0x18 */
//  uint8 SEEI;                                    /**< Set Enable Error Interrupt Register, offset: 0x19 */
//  uint8 CERQ;                                    /**< Clear Enable Request Register, offset: 0x1A */
//  uint8 SERQ;                                    /**< Set Enable Request Register, offset: 0x1B */
//  uint8 CDNE;                                    /**< Clear DONE Status Bit Register, offset: 0x1C */
//  uint8 SSRT;                                    /**< Set START Bit Register, offset: 0x1D */
//  uint8 CERR;                                    /**< Clear Error Register, offset: 0x1E */
//  uint8 CINT;                                    /**< Clear Interrupt Request Register, offset: 0x1F */
//  uint8 RESERVED_2[4];
//  uint32 INT;                                    /**< Interrupt Request Register, offset: 0x24 */
//  uint8 RESERVED_3[4];
//  uint32 ERR;                                    /**< Error Register, offset: 0x2C */
//  uint8 RESERVED_4[4];
//  uint32 HRS;                                    /**< Hardware Request Status Register, offset: 0x34 */
//  uint8 RESERVED_5[12];
//  uint32 EARS;                                   /**< Enable Asynchronous Request in Stop Register, offset: 0x44 */
//  uint8 RESERVED_6[184];
//  uint8 DCHPRI3;                                 /**< Channel n Priority Register, offset: 0x100 */
//  uint8 DCHPRI2;                                 /**< Channel n Priority Register, offset: 0x101 */
//  uint8 DCHPRI1;                                 /**< Channel n Priority Register, offset: 0x102 */
//  uint8 DCHPRI0;                                 /**< Channel n Priority Register, offset: 0x103 */
//  uint8 DCHPRI7;                                 /**< Channel n Priority Register, offset: 0x104 */
//  uint8 DCHPRI6;                                 /**< Channel n Priority Register, offset: 0x105 */
//  uint8 DCHPRI5;                                 /**< Channel n Priority Register, offset: 0x106 */
//  uint8 DCHPRI4;                                 /**< Channel n Priority Register, offset: 0x107 */
//  uint8 DCHPRI11;                                /**< Channel n Priority Register, offset: 0x108 */
//  uint8 DCHPRI10;                                /**< Channel n Priority Register, offset: 0x109 */
//  uint8 DCHPRI9;                                 /**< Channel n Priority Register, offset: 0x10A */
//  uint8 DCHPRI8;                                 /**< Channel n Priority Register, offset: 0x10B */
//  uint8 DCHPRI15;                                /**< Channel n Priority Register, offset: 0x10C */
//  uint8 DCHPRI14;                                /**< Channel n Priority Register, offset: 0x10D */
//  uint8 DCHPRI13;                                /**< Channel n Priority Register, offset: 0x10E */
//  uint8 DCHPRI12;                                /**< Channel n Priority Register, offset: 0x10F */
//  uint8 RESERVED_7[3824];
//  struct {                                         /* offset: 0x1000, array step: 0x20 */
//    uint32 SADDR;                                  /**< TCD Source Address, array offset: 0x1000, array step: 0x20 */
//    uint16 SOFF;                                   /**< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
//    uint16 ATTR;                                   /**< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
//    union {                                          /* offset: 0x1008, array step: 0x20 */
//      uint32 NBYTES_MLNO;                            /**< TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20 */
//      uint32 NBYTES_MLOFFNO;                         /**< TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
//      uint32 NBYTES_MLOFFYES;                        /**< TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20 */
//    };
//    uint32 SLAST;                                  /**< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
//    uint32 DADDR;                                  /**< TCD Destination Address, array offset: 0x1010, array step: 0x20 */
//    uint16 DOFF;                                   /**< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
//    union {                                          /* offset: 0x1016, array step: 0x20 */
//      uint16 CITER_ELINKNO;                          /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
//      uint16 CITER_ELINKYES;                         /**< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
//    };
//    uint32 DLAST_SGA;                              /**< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
//    uint16 CSR;                                    /**< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
//    union {                                          /* offset: 0x101E, array step: 0x20 */
//      uint16 BITER_ELINKNO;                          /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
//      uint16 BITER_ELINKYES;                         /**< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
//    };
//  } TCD[16];
//} volatile *DMA_MemMapPtr;
//
//
//
//
///* DMA - Peripheral instance base addresses */
///** Peripheral DMA base pointer */
//#define DMA_BASE_PTR                             ((DMA_MemMapPtr)0x40008000u)
///** Array initializer of DMA peripheral base pointers */
//#define DMA_BASE_PTRS                            { DMA_BASE_PTR }
//
///* ----------------------------------------------------------------------------
//   -- DMA - Register accessor macros
//   ---------------------------------------------------------------------------- */
//
///* DMA - Register instance definitions */
///* DMA */
//#define DMA_CR                                   DMA_CR_REG(DMA_BASE_PTR)
//#define DMA_ES                                   DMA_ES_REG(DMA_BASE_PTR)
//#define DMA_ERQ                                  DMA_ERQ_REG(DMA_BASE_PTR)
//#define DMA_EEI                                  DMA_EEI_REG(DMA_BASE_PTR)
//#define DMA_CEEI                                 DMA_CEEI_REG(DMA_BASE_PTR)
//#define DMA_SEEI                                 DMA_SEEI_REG(DMA_BASE_PTR)
//#define DMA_CERQ                                 DMA_CERQ_REG(DMA_BASE_PTR)
//#define DMA_SERQ                                 DMA_SERQ_REG(DMA_BASE_PTR)
//#define DMA_CDNE                                 DMA_CDNE_REG(DMA_BASE_PTR)
//#define DMA_SSRT                                 DMA_SSRT_REG(DMA_BASE_PTR)
//#define DMA_CERR                                 DMA_CERR_REG(DMA_BASE_PTR)
//#define DMA_CINT                                 DMA_CINT_REG(DMA_BASE_PTR)
//#define DMA_INT                                  DMA_INT_REG(DMA_BASE_PTR)
//#define DMA_ERR                                  DMA_ERR_REG(DMA_BASE_PTR)
//#define DMA_HRS                                  DMA_HRS_REG(DMA_BASE_PTR)
//#define DMA_EARS                                 DMA_EARS_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI3                              DMA_DCHPRI3_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI2                              DMA_DCHPRI2_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI1                              DMA_DCHPRI1_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI0                              DMA_DCHPRI0_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI7                              DMA_DCHPRI7_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI6                              DMA_DCHPRI6_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI5                              DMA_DCHPRI5_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI4                              DMA_DCHPRI4_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI11                             DMA_DCHPRI11_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI10                             DMA_DCHPRI10_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI9                              DMA_DCHPRI9_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI8                              DMA_DCHPRI8_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI15                             DMA_DCHPRI15_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI14                             DMA_DCHPRI14_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI13                             DMA_DCHPRI13_REG(DMA_BASE_PTR)
//#define DMA_DCHPRI12                             DMA_DCHPRI12_REG(DMA_BASE_PTR)
//#define DMA_TCD0_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_CSR                             DMA_CSR_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,0)
//#define DMA_TCD0_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,0)
//#define DMA_TCD1_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_CSR                             DMA_CSR_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,1)
//#define DMA_TCD1_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,1)
//#define DMA_TCD2_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_CSR                             DMA_CSR_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,2)
//#define DMA_TCD2_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,2)
//#define DMA_TCD3_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_CSR                             DMA_CSR_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,3)
//#define DMA_TCD3_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,3)
//#define DMA_TCD4_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_CSR                             DMA_CSR_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,4)
//#define DMA_TCD4_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,4)
//#define DMA_TCD5_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_CSR                             DMA_CSR_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,5)
//#define DMA_TCD5_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,5)
//#define DMA_TCD6_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_CSR                             DMA_CSR_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,6)
//#define DMA_TCD6_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,6)
//#define DMA_TCD7_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_CSR                             DMA_CSR_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,7)
//#define DMA_TCD7_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,7)
//#define DMA_TCD8_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_CSR                             DMA_CSR_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,8)
//#define DMA_TCD8_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,8)
//#define DMA_TCD9_SADDR                           DMA_SADDR_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_SOFF                            DMA_SOFF_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_ATTR                            DMA_ATTR_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_NBYTES_MLNO                     DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_NBYTES_MLOFFNO                  DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_NBYTES_MLOFFYES                 DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_SLAST                           DMA_SLAST_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_DADDR                           DMA_DADDR_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_DOFF                            DMA_DOFF_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_CITER_ELINKNO                   DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_CITER_ELINKYES                  DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_DLASTSGA                        DMA_DLAST_SGA_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_CSR                             DMA_CSR_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_BITER_ELINKNO                   DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,9)
//#define DMA_TCD9_BITER_ELINKYES                  DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,9)
//#define DMA_TCD10_SADDR                          DMA_SADDR_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_SOFF                           DMA_SOFF_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_ATTR                           DMA_ATTR_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_NBYTES_MLNO                    DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_NBYTES_MLOFFNO                 DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_NBYTES_MLOFFYES                DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_SLAST                          DMA_SLAST_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_DADDR                          DMA_DADDR_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_DOFF                           DMA_DOFF_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_CITER_ELINKNO                  DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_CITER_ELINKYES                 DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_DLASTSGA                       DMA_DLAST_SGA_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_CSR                            DMA_CSR_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_BITER_ELINKNO                  DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,10)
//#define DMA_TCD10_BITER_ELINKYES                 DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,10)
//#define DMA_TCD11_SADDR                          DMA_SADDR_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_SOFF                           DMA_SOFF_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_ATTR                           DMA_ATTR_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_NBYTES_MLNO                    DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_NBYTES_MLOFFNO                 DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_NBYTES_MLOFFYES                DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_SLAST                          DMA_SLAST_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_DADDR                          DMA_DADDR_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_DOFF                           DMA_DOFF_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_CITER_ELINKNO                  DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_CITER_ELINKYES                 DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_DLASTSGA                       DMA_DLAST_SGA_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_CSR                            DMA_CSR_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_BITER_ELINKNO                  DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,11)
//#define DMA_TCD11_BITER_ELINKYES                 DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,11)
//#define DMA_TCD12_SADDR                          DMA_SADDR_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_SOFF                           DMA_SOFF_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_ATTR                           DMA_ATTR_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_NBYTES_MLNO                    DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_NBYTES_MLOFFNO                 DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_NBYTES_MLOFFYES                DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_SLAST                          DMA_SLAST_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_DADDR                          DMA_DADDR_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_DOFF                           DMA_DOFF_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_CITER_ELINKNO                  DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_CITER_ELINKYES                 DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_DLASTSGA                       DMA_DLAST_SGA_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_CSR                            DMA_CSR_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_BITER_ELINKNO                  DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,12)
//#define DMA_TCD12_BITER_ELINKYES                 DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,12)
//#define DMA_TCD13_SADDR                          DMA_SADDR_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_SOFF                           DMA_SOFF_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_ATTR                           DMA_ATTR_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_NBYTES_MLNO                    DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_NBYTES_MLOFFNO                 DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_NBYTES_MLOFFYES                DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_SLAST                          DMA_SLAST_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_DADDR                          DMA_DADDR_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_DOFF                           DMA_DOFF_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_CITER_ELINKNO                  DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_CITER_ELINKYES                 DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_DLASTSGA                       DMA_DLAST_SGA_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_CSR                            DMA_CSR_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_BITER_ELINKNO                  DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,13)
//#define DMA_TCD13_BITER_ELINKYES                 DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,13)
//#define DMA_TCD14_SADDR                          DMA_SADDR_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_SOFF                           DMA_SOFF_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_ATTR                           DMA_ATTR_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_NBYTES_MLNO                    DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_NBYTES_MLOFFNO                 DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_NBYTES_MLOFFYES                DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_SLAST                          DMA_SLAST_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_DADDR                          DMA_DADDR_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_DOFF                           DMA_DOFF_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_CITER_ELINKNO                  DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_CITER_ELINKYES                 DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_DLASTSGA                       DMA_DLAST_SGA_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_CSR                            DMA_CSR_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_BITER_ELINKNO                  DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,14)
//#define DMA_TCD14_BITER_ELINKYES                 DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,14)
//#define DMA_TCD15_SADDR                          DMA_SADDR_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_SOFF                           DMA_SOFF_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_ATTR                           DMA_ATTR_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_NBYTES_MLNO                    DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_NBYTES_MLOFFNO                 DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_NBYTES_MLOFFYES                DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_SLAST                          DMA_SLAST_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_DADDR                          DMA_DADDR_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_DOFF                           DMA_DOFF_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_CITER_ELINKNO                  DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_CITER_ELINKYES                 DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_DLASTSGA                       DMA_DLAST_SGA_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_CSR                            DMA_CSR_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_BITER_ELINKNO                  DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,15)
//#define DMA_TCD15_BITER_ELINKYES                 DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,15)
//
///* DMA - Register array accessors */
//#define DMA_SADDR(index)                         DMA_SADDR_REG(DMA_BASE_PTR,index)
//#define DMA_SOFF(index)                          DMA_SOFF_REG(DMA_BASE_PTR,index)
//#define DMA_ATTR(index)                          DMA_ATTR_REG(DMA_BASE_PTR,index)
//#define DMA_NBYTES_MLNO(index)                   DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,index)
//#define DMA_NBYTES_MLOFFNO(index)                DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,index)
//#define DMA_NBYTES_MLOFFYES(index)               DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,index)
//#define DMA_SLAST(index)                         DMA_SLAST_REG(DMA_BASE_PTR,index)
//#define DMA_DADDR(index)                         DMA_DADDR_REG(DMA_BASE_PTR,index)
//#define DMA_DOFF(index)                          DMA_DOFF_REG(DMA_BASE_PTR,index)
//#define DMA_CITER_ELINKNO(index)                 DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,index)
//#define DMA_CITER_ELINKYES(index)                DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,index)
//#define DMA_DLAST_SGA(index)                     DMA_DLAST_SGA_REG(DMA_BASE_PTR,index)
//#define DMA_CSR(index)                           DMA_CSR_REG(DMA_BASE_PTR,index)
//#define DMA_BITER_ELINKNO(index)                 DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,index)
//#define DMA_BITER_ELINKYES(index)                DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,index)
//
////end of group DMA_Peripheral
//
//
///* ----------------------------------------------------------------------------
//   -- DMAMUX
//   ---------------------------------------------------------------------------- */
//typedef struct
//{
//	union
//	{
//		uint8 BYTE;
//		struct
//		{
//			uint8 SOURCE :6;
//			uint8 TRIG :1;
//			uint8 ENBL:1;
//
//		}BIT;
//
//	}CHCFG[16];                               /**< Channel Configuration register, array offset: 0x0, array step: 0x1 */
//
//} volatile DMAMUX_BASIC_DEF;
//
//#define DMAMUX_BASE                                    0x40021000
//#define DMAMUX                                         (*(DMAMUX_BASIC_DEF *)DMAMUX_BASE)
//
//
///** DMAMUX - Peripheral register structure */
//typedef struct DMAMUX_MemMap {
//  uint8 CHCFG[16];                               /**< Channel Configuration register, array offset: 0x0, array step: 0x1 */
//} volatile *DMAMUX_MemMapPtr;
//
//
///* ----------------------------------------------------------------------------
//   -- DMAMUX - Register accessor macros
//   ---------------------------------------------------------------------------- */
//
///* DMAMUX - Register accessors */
//#define DMAMUX_CHCFG_REG(base,index)             ((base)->CHCFG[index])
//
///* CHCFG Bit Fields */
//#define DMAMUX_CHCFG_SOURCE_MASK                 0x3Fu
//#define DMAMUX_CHCFG_SOURCE_SHIFT                0
//#define DMAMUX_CHCFG_SOURCE(x)                   (((uint8)(((uint8)(x))<<DMAMUX_CHCFG_SOURCE_SHIFT))&DMAMUX_CHCFG_SOURCE_MASK)
//#define DMAMUX_CHCFG_TRIG_MASK                   0x40u
//#define DMAMUX_CHCFG_TRIG_SHIFT                  6
//#define DMAMUX_CHCFG_ENBL_MASK                   0x80u
//#define DMAMUX_CHCFG_ENBL_SHIFT                  7
//
///* DMAMUX - Peripheral instance base addresses */
///** Peripheral DMAMUX base pointer */
//#define DMAMUX_BASE_PTR                          ((DMAMUX_MemMapPtr)0x40021000u)
///** Array initializer of DMAMUX peripheral base pointers */
//#define DMAMUX_BASE_PTRS                         { DMAMUX_BASE_PTR }
//
///* DMAMUX - Register instance definitions */
///* DMAMUX */
//#define DMAMUX_CHCFG0                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,0)
//#define DMAMUX_CHCFG1                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,1)
//#define DMAMUX_CHCFG2                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,2)
//#define DMAMUX_CHCFG3                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,3)
//#define DMAMUX_CHCFG4                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,4)
//#define DMAMUX_CHCFG5                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,5)
//#define DMAMUX_CHCFG6                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,6)
//#define DMAMUX_CHCFG7                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,7)
//#define DMAMUX_CHCFG8                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,8)
//#define DMAMUX_CHCFG9                            DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,9)
//#define DMAMUX_CHCFG10                           DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,10)
//#define DMAMUX_CHCFG11                           DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,11)
//#define DMAMUX_CHCFG12                           DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,12)
//#define DMAMUX_CHCFG13                           DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,13)
//#define DMAMUX_CHCFG14                           DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,14)
//#define DMAMUX_CHCFG15                           DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,15)
//
///* DMAMUX - Register array accessors */
//#define DMAMUX_CHCFG(index)                      DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,index)

/* ----------------------------------------------------------------------------
 *  @brief Analog to Digital Converter (ADC)
 *  ----------------------------------------------------------------------------
*/
/** Peripheral ADC0 base pointer */
#define ADC0_BASE								0x4003B000

/** Peripheral ADC1 base pointer */
#define ADC1_BASE								0x40027000

typedef struct
{
   volatile uint32 ADC_SC1[2];                   // ADC Status and Control Registers 1, 		  array offset: 0x0, array step: 0x4 */
   volatile uint32 ADC_CFG1;                     // ADC Configuration Register 1, 					offset: 0x8 */
   volatile uint32 ADC_CFG2;                     // ADC Configuration Register 2, 					offset: 0xC */
   volatile uint32 ADC_R[2];                     // ADC Data Result Register, 				  array offset: 0x10, array step: 0x4 */
   volatile uint32 ADC_CV1;                      // Compare Value Registers, 							offset: 0x18 */
   volatile uint32 ADC_CV2;                      // Compare Value Registers, 							offset: 0x1C */
   volatile uint32 ADC_SC2;                      // Status and Control Register 2, 					offset: 0x20 */
   volatile uint32 ADC_SC3;                      // Status and Control Register 3, 					offset: 0x24 */
   volatile uint32 ADC_OFS;                      // ADC Offset Correction Register, 					offset: 0x28 */
   volatile uint32 ADC_PG;                       // ADC Plus-Side Gain Register, 						offset: 0x2C */
   volatile uint32 ADC_MG;                       // ADC Minus-Side Gain Register, 					offset: 0x30 */
   volatile uint32 ADC_CLPD;                     // ADC Plus-Side General Calibration Value Register, offset: 0x34 */
   volatile uint32 ADC_CLPS;                     // ADC Plus-Side General Calibration Value Register, offset: 0x38 */
   volatile uint32 ADC_CLP4;                     // ADC Plus-Side General Calibration Value Register, offset: 0x3C */
   volatile uint32 ADC_CLP3;                     // ADC Plus-Side General Calibration Value Register, offset: 0x40 */
   volatile uint32 ADC_CLP2;                     // ADC Plus-Side General Calibration Value Register, offset: 0x44 */
   volatile uint32 ADC_CLP1;                     // ADC Plus-Side General Calibration Value Register, offset: 0x48 */
   volatile uint32 ADC_CLP0;                     // ADC Plus-Side General Calibration Value Register, offset: 0x4C */
   volatile uint8  RESERVED_0[4];
   volatile uint32 ADC_CLMD;                     // ADC Minus-Side General Calibration Value Register, offset: 0x54 */
   volatile uint32 ADC_CLMS;                     // ADC Minus-Side General Calibration Value Register, offset: 0x58 */
   volatile uint32 ADC_CLM4;                     // ADC Minus-Side General Calibration Value Register, offset: 0x5C */
   volatile uint32 ADC_CLM3;                     // ADC Minus-Side General Calibration Value Register, offset: 0x60 */
   volatile uint32 ADC_CLM2;                     // ADC Minus-Side General Calibration Value Register, offset: 0x64 */
   volatile uint32 ADC_CLM1;                     // ADC Minus-Side General Calibration Value Register, offset: 0x68 */
   volatile uint32 ADC_CLM0;                     // ADC Minus-Side General Calibration Value Register, offset: 0x6C */
} ADC_BASIC_DEF;

#define ADC0                             		(*(ADC_BASIC_DEF *)ADC0_BASE)

#define ADC1                             		(*(ADC_BASIC_DEF *)ADC1_BASE)

/* SC1 Bit Fields */
#define ADC_SC1_ADCH_MASK                        0x1Fu
#define ADC_SC1_ADCH_SHIFT                       0
#define ADC_SC1_ADCH(x)                          (((uint32)(((uint32)(x))<<ADC_SC1_ADCH_SHIFT))&ADC_SC1_ADCH_MASK)
#define ADC_SC1_DIFF_MASK                        0x20u
#define ADC_SC1_DIFF_SHIFT                       5
#define ADC_SC1_AIEN_MASK                        0x40u
#define ADC_SC1_AIEN_SHIFT                       6
#define ADC_SC1_COCO_MASK                        0x80u
#define ADC_SC1_COCO_SHIFT                       7
/* CFG1 Bit Fields */
#define ADC_CFG1_ADICLK_MASK                     0x3u
#define ADC_CFG1_ADICLK_SHIFT                    0
#define ADC_CFG1_ADICLK(x)                       (((uint32)(((uint32)(x))<<ADC_CFG1_ADICLK_SHIFT))&ADC_CFG1_ADICLK_MASK)
#define ADC_CFG1_MODE_MASK                       0xCu
#define ADC_CFG1_MODE_SHIFT                      2
#define ADC_CFG1_MODE(x)                         (((uint32)(((uint32)(x))<<ADC_CFG1_MODE_SHIFT))&ADC_CFG1_MODE_MASK)
#define ADC_CFG1_ADLSMP_MASK                     0x10u
#define ADC_CFG1_ADLSMP_SHIFT                    4
#define ADC_CFG1_ADIV_MASK                       0x60u
#define ADC_CFG1_ADIV_SHIFT                      5
#define ADC_CFG1_ADIV(x)                         (((uint32)(((uint32)(x))<<ADC_CFG1_ADIV_SHIFT))&ADC_CFG1_ADIV_MASK)
#define ADC_CFG1_ADLPC_MASK                      0x80u
#define ADC_CFG1_ADLPC_SHIFT                     7
/* CFG2 Bit Fields */
#define ADC_CFG2_ADLSTS_MASK                     0x3u
#define ADC_CFG2_ADLSTS_SHIFT                    0
#define ADC_CFG2_ADLSTS(x)                       (((uint32)(((uint32)(x))<<ADC_CFG2_ADLSTS_SHIFT))&ADC_CFG2_ADLSTS_MASK)
#define ADC_CFG2_ADHSC_MASK                      0x4u
#define ADC_CFG2_ADHSC_SHIFT                     2
#define ADC_CFG2_ADACKEN_MASK                    0x8u
#define ADC_CFG2_ADACKEN_SHIFT                   3
#define ADC_CFG2_MUXSEL_MASK                     0x10u
#define ADC_CFG2_MUXSEL_SHIFT                    4
/* R Bit Fields */
#define ADC_R_D_MASK                             0xFFFFu
#define ADC_R_D_SHIFT                            0
#define ADC_R_D(x)                               (((uint32)(((uint32)(x))<<ADC_R_D_SHIFT))&ADC_R_D_MASK)
/* CV1 Bit Fields */
#define ADC_CV1_CV_MASK                          0xFFFFu
#define ADC_CV1_CV_SHIFT                         0
#define ADC_CV1_CV(x)                            (((uint32)(((uint32)(x))<<ADC_CV1_CV_SHIFT))&ADC_CV1_CV_MASK)
/* CV2 Bit Fields */
#define ADC_CV2_CV_MASK                          0xFFFFu
#define ADC_CV2_CV_SHIFT                         0
#define ADC_CV2_CV(x)                            (((uint32)(((uint32)(x))<<ADC_CV2_CV_SHIFT))&ADC_CV2_CV_MASK)
/* SC2 Bit Fields */
#define ADC_SC2_REFSEL_MASK                      0x3u
#define ADC_SC2_REFSEL_SHIFT                     0
#define ADC_SC2_REFSEL(x)                        (((uint32)(((uint32)(x))<<ADC_SC2_REFSEL_SHIFT))&ADC_SC2_REFSEL_MASK)
#define ADC_SC2_DMAEN_MASK                       0x4u
#define ADC_SC2_DMAEN_SHIFT                      2
#define ADC_SC2_ACREN_MASK                       0x8u
#define ADC_SC2_ACREN_SHIFT                      3
#define ADC_SC2_ACFGT_MASK                       0x10u
#define ADC_SC2_ACFGT_SHIFT                      4
#define ADC_SC2_ACFE_MASK                        0x20u
#define ADC_SC2_ACFE_SHIFT                       5
#define ADC_SC2_ADTRG_MASK                       0x40u
#define ADC_SC2_ADTRG_SHIFT                      6
#define ADC_SC2_ADACT_MASK                       0x80u
#define ADC_SC2_ADACT_SHIFT                      7
/* SC3 Bit Fields */
#define ADC_SC3_AVGS_MASK                        0x3u
#define ADC_SC3_AVGS_SHIFT                       0
#define ADC_SC3_AVGS(x)                          (((uint32)(((uint32)(x))<<ADC_SC3_AVGS_SHIFT))&ADC_SC3_AVGS_MASK)
#define ADC_SC3_AVGE_MASK                        0x4u
#define ADC_SC3_AVGE_SHIFT                       2
#define ADC_SC3_ADCO_MASK                        0x8u
#define ADC_SC3_ADCO_SHIFT                       3
#define ADC_SC3_CALF_MASK                        0x40u
#define ADC_SC3_CALF_SHIFT                       6
#define ADC_SC3_CAL_MASK                         0x80u
#define ADC_SC3_CAL_SHIFT                        7
/* OFS Bit Fields */
#define ADC_OFS_OFS_MASK                         0xFFFFu
#define ADC_OFS_OFS_SHIFT                        0
#define ADC_OFS_OFS(x)                           (((uint32)(((uint32)(x))<<ADC_OFS_OFS_SHIFT))&ADC_OFS_OFS_MASK)
/* PG Bit Fields */
#define ADC_PG_PG_MASK                           0xFFFFu
#define ADC_PG_PG_SHIFT                          0
#define ADC_PG_PG(x)                             (((uint32)(((uint32)(x))<<ADC_PG_PG_SHIFT))&ADC_PG_PG_MASK)
/* MG Bit Fields */
#define ADC_MG_MG_MASK                           0xFFFFu
#define ADC_MG_MG_SHIFT                          0
#define ADC_MG_MG(x)                             (((uint32)(((uint32)(x))<<ADC_MG_MG_SHIFT))&ADC_MG_MG_MASK)
/* CLPD Bit Fields */
#define ADC_CLPD_CLPD_MASK                       0x3Fu
#define ADC_CLPD_CLPD_SHIFT                      0
#define ADC_CLPD_CLPD(x)                         (((uint32)(((uint32)(x))<<ADC_CLPD_CLPD_SHIFT))&ADC_CLPD_CLPD_MASK)
/* CLPS Bit Fields */
#define ADC_CLPS_CLPS_MASK                       0x3Fu
#define ADC_CLPS_CLPS_SHIFT                      0
#define ADC_CLPS_CLPS(x)                         (((uint32)(((uint32)(x))<<ADC_CLPS_CLPS_SHIFT))&ADC_CLPS_CLPS_MASK)
/* CLP4 Bit Fields */
#define ADC_CLP4_CLP4_MASK                       0x3FFu
#define ADC_CLP4_CLP4_SHIFT                      0
#define ADC_CLP4_CLP4(x)                         (((uint32)(((uint32)(x))<<ADC_CLP4_CLP4_SHIFT))&ADC_CLP4_CLP4_MASK)
/* CLP3 Bit Fields */
#define ADC_CLP3_CLP3_MASK                       0x1FFu
#define ADC_CLP3_CLP3_SHIFT                      0
#define ADC_CLP3_CLP3(x)                         (((uint32)(((uint32)(x))<<ADC_CLP3_CLP3_SHIFT))&ADC_CLP3_CLP3_MASK)
/* CLP2 Bit Fields */
#define ADC_CLP2_CLP2_MASK                       0xFFu
#define ADC_CLP2_CLP2_SHIFT                      0
#define ADC_CLP2_CLP2(x)                         (((uint32)(((uint32)(x))<<ADC_CLP2_CLP2_SHIFT))&ADC_CLP2_CLP2_MASK)
/* CLP1 Bit Fields */
#define ADC_CLP1_CLP1_MASK                       0x7Fu
#define ADC_CLP1_CLP1_SHIFT                      0
#define ADC_CLP1_CLP1(x)                         (((uint32)(((uint32)(x))<<ADC_CLP1_CLP1_SHIFT))&ADC_CLP1_CLP1_MASK)
/* CLP0 Bit Fields */
#define ADC_CLP0_CLP0_MASK                       0x3Fu
#define ADC_CLP0_CLP0_SHIFT                      0
#define ADC_CLP0_CLP0(x)                         (((uint32)(((uint32)(x))<<ADC_CLP0_CLP0_SHIFT))&ADC_CLP0_CLP0_MASK)
/* CLMD Bit Fields */
#define ADC_CLMD_CLMD_MASK                       0x3Fu
#define ADC_CLMD_CLMD_SHIFT                      0
#define ADC_CLMD_CLMD(x)                         (((uint32)(((uint32)(x))<<ADC_CLMD_CLMD_SHIFT))&ADC_CLMD_CLMD_MASK)
/* CLMS Bit Fields */
#define ADC_CLMS_CLMS_MASK                       0x3Fu
#define ADC_CLMS_CLMS_SHIFT                      0
#define ADC_CLMS_CLMS(x)                         (((uint32)(((uint32)(x))<<ADC_CLMS_CLMS_SHIFT))&ADC_CLMS_CLMS_MASK)
/* CLM4 Bit Fields */
#define ADC_CLM4_CLM4_MASK                       0x3FFu
#define ADC_CLM4_CLM4_SHIFT                      0
#define ADC_CLM4_CLM4(x)                         (((uint32)(((uint32)(x))<<ADC_CLM4_CLM4_SHIFT))&ADC_CLM4_CLM4_MASK)
/* CLM3 Bit Fields */
#define ADC_CLM3_CLM3_MASK                       0x1FFu
#define ADC_CLM3_CLM3_SHIFT                      0
#define ADC_CLM3_CLM3(x)                         (((uint32)(((uint32)(x))<<ADC_CLM3_CLM3_SHIFT))&ADC_CLM3_CLM3_MASK)
/* CLM2 Bit Fields */
#define ADC_CLM2_CLM2_MASK                       0xFFu
#define ADC_CLM2_CLM2_SHIFT                      0
#define ADC_CLM2_CLM2(x)                         (((uint32)(((uint32)(x))<<ADC_CLM2_CLM2_SHIFT))&ADC_CLM2_CLM2_MASK)
/* CLM1 Bit Fields */
#define ADC_CLM1_CLM1_MASK                       0x7Fu
#define ADC_CLM1_CLM1_SHIFT                      0
#define ADC_CLM1_CLM1(x)                         (((uint32)(((uint32)(x))<<ADC_CLM1_CLM1_SHIFT))&ADC_CLM1_CLM1_MASK)
/* CLM0 Bit Fields */
#define ADC_CLM0_CLM0_MASK                       0x3Fu
#define ADC_CLM0_CLM0_SHIFT                      0
#define ADC_CLM0_CLM0(x)                         (((uint32)(((uint32)(x))<<ADC_CLM0_CLM0_SHIFT))&ADC_CLM0_CLM0_MASK)

//************** end of group ADC_Register_Masks ********************


typedef enum
{
    TIMER_FROZEN,
    TIMER_ACTIVE_IN_MATCH,
    TIMER_INACTIVE_IN_MATCH,
    TIMER_TOGGLE,
    TIMER_FORCE_INACTIVE,
    TIMER_FORCE_ACTIVE,
    TIMER_PWM1,
    TIMER_PWM2
} TIMER_MODE_DEF;


/* ----------------------------------------------------------------------------
   -- FTM
   ---------------------------------------------------------------------------- */

#define FTM0_BASE								 0x40038000
#define FTM1_BASE								 0x40039000
#define FTM2_BASE								 0x4003A000
#define FTM3_BASE								 0x40026000

/** FTM - Peripheral register structure */
typedef struct
{
  uint32 SC;                                     //Status And Control, 						offset: 0x0
  uint32 CNT;                                    //Counter, 								offset: 0x4
  uint32 MOD;                                    //Modulo, 									offset: 0x8
  struct
  {                                             //offset: 0xC, array step: 0x8
    uint32 CnSC;                                //Channel (n) Status And Control,	 array offset: 0xC, array step: 0x8
    uint32 CnV;                                 //Channel (n) Value, 				array offset: 0x10, array step: 0x8
  } CONTROLS[8];
  uint32 CNTIN;                                  //Counter Initial Value, 					offset: 0x4C
  uint32 STATUS;                                 //Capture And Compare Status, 				offset: 0x50
  uint32 MODE;                                   //Features Mode Selection, 				offset: 0x54
  uint32 SYNC;                                   //Synchronization, 						offset: 0x58
  uint32 OUTINIT;                                //Initial State For Channels Output, 		offset: 0x5C
  uint32 OUTMASK;                                //Output Mask, 							offset: 0x60
  uint32 COMBINE;                                //Function For Linked Channels, 			offset: 0x64
  uint32 DEADTIME;                               //Deadtime Insertion Control, 				offset: 0x68
  uint32 EXTTRIG;                                //FTM External Trigger, 					offset: 0x6C
  uint32 POL;                                    //Channels Polarity, 						offset: 0x70
  uint32 FMS;                                    //Fault Mode Status, 						offset: 0x74
  uint32 FILTER;                                 //Input Capture Filter Control, 			offset: 0x78
  uint32 FLTCTRL;                                //Fault Control, 							offset: 0x7C
  uint32 QDCTRL;                                 //Quadrature Decoder Control And Status, 	offset: 0x80
  uint32 CONF;                                   //Configuration, 							offset: 0x84
  uint32 FLTPOL;                                 //FTM Fault Input Polarity, 				offset: 0x88
  uint32 SYNCONF;                                //Synchronization Configuration, 			offset: 0x8C
  uint32 INVCTRL;                                //FTM Inverting Control, 					offset: 0x90
  uint32 SWOCTRL;                                //FTM Software Output Control, 			offset: 0x94
  uint32 PWMLOAD;                                //FTM PWM Load, 							offset: 0x98
} FTM_BASIC_DEF;


#define FTM0	                                 (*(FTM_BASIC_DEF *)FTM0_BASE)
#define FTM1	                                 (*(FTM_BASIC_DEF *)FTM1_BASE)
#define FTM2	                                 (*(FTM_BASIC_DEF *)FTM2_BASE)
#define FTM3	                                 (*(FTM_BASIC_DEF *)FTM3_BASE)

/* ----------------------------------------------------------------------------
   -- FTM - Register accessor macros
   ---------------------------------------------------------------------------- */

/* FTM - Register accessors */
#define FTM_SC_REG(base)                         ((base)->SC)
#define FTM_CNT_REG(base)                        ((base)->CNT)
#define FTM_MOD_REG(base)                        ((base)->MOD)
#define FTM_CnSC_REG(base,index)                 ((base)->CONTROLS[index].CnSC)
#define FTM_CnV_REG(base,index)                  ((base)->CONTROLS[index].CnV)
#define FTM_CNTIN_REG(base)                      ((base)->CNTIN)
#define FTM_STATUS_REG(base)                     ((base)->STATUS)
#define FTM_MODE_REG(base)                       ((base)->MODE)
#define FTM_SYNC_REG(base)                       ((base)->SYNC)
#define FTM_OUTINIT_REG(base)                    ((base)->OUTINIT)
#define FTM_OUTMASK_REG(base)                    ((base)->OUTMASK)
#define FTM_COMBINE_REG(base)                    ((base)->COMBINE)
#define FTM_DEADTIME_REG(base)                   ((base)->DEADTIME)
#define FTM_EXTTRIG_REG(base)                    ((base)->EXTTRIG)
#define FTM_POL_REG(base)                        ((base)->POL)
#define FTM_FMS_REG(base)                        ((base)->FMS)
#define FTM_FILTER_REG(base)                     ((base)->FILTER)
#define FTM_FLTCTRL_REG(base)                    ((base)->FLTCTRL)
#define FTM_QDCTRL_REG(base)                     ((base)->QDCTRL)
#define FTM_CONF_REG(base)                       ((base)->CONF)
#define FTM_FLTPOL_REG(base)                     ((base)->FLTPOL)
#define FTM_SYNCONF_REG(base)                    ((base)->SYNCONF)
#define FTM_INVCTRL_REG(base)                    ((base)->INVCTRL)
#define FTM_SWOCTRL_REG(base)                    ((base)->SWOCTRL)
#define FTM_PWMLOAD_REG(base)                    ((base)->PWMLOAD)

/* ----------------------------------------------------------------------------
   -- FTM Register Masks
   ---------------------------------------------------------------------------- */

/* SC Bit Fields */
#define FTM_SC_PS_MASK                           0x7u
#define FTM_SC_PS_SHIFT                          0
#define FTM_SC_PS(x)                             (((uint32)(((uint32)(x))<<FTM_SC_PS_SHIFT))&FTM_SC_PS_MASK)
#define FTM_SC_CLKS_MASK                         0x18u
#define FTM_SC_CLKS_SHIFT                        3
#define FTM_SC_CLKS(x)                           (((uint32)(((uint32)(x))<<FTM_SC_CLKS_SHIFT))&FTM_SC_CLKS_MASK)
#define FTM_SC_CPWMS_MASK                        0x20u
#define FTM_SC_CPWMS_SHIFT                       5
#define FTM_SC_TOIE_MASK                         0x40u
#define FTM_SC_TOIE_SHIFT                        6
#define FTM_SC_TOF_MASK                          0x80u
#define FTM_SC_TOF_SHIFT                         7
/* CNT Bit Fields */
#define FTM_CNT_COUNT_MASK                       0xFFFFu
#define FTM_CNT_COUNT_SHIFT                      0
#define FTM_CNT_COUNT(x)                         (((uint32)(((uint32)(x))<<FTM_CNT_COUNT_SHIFT))&FTM_CNT_COUNT_MASK)
/* MOD Bit Fields */
#define FTM_MOD_MOD_MASK                         0xFFFFu
#define FTM_MOD_MOD_SHIFT                        0
#define FTM_MOD_MOD(x)                           (((uint32)(((uint32)(x))<<FTM_MOD_MOD_SHIFT))&FTM_MOD_MOD_MASK)
/* CnSC Bit Fields */
#define FTM_CnSC_DMA_MASK                        0x1u
#define FTM_CnSC_DMA_SHIFT                       0
#define FTM_CnSC_ICRST_MASK                      0x2u
#define FTM_CnSC_ICRST_SHIFT                     1
#define FTM_CnSC_ELSA_MASK                       0x4u
#define FTM_CnSC_ELSA_SHIFT                      2
#define FTM_CnSC_ELSB_MASK                       0x8u
#define FTM_CnSC_ELSB_SHIFT                      3
#define FTM_CnSC_MSA_MASK                        0x10u
#define FTM_CnSC_MSA_SHIFT                       4
#define FTM_CnSC_MSB_MASK                        0x20u
#define FTM_CnSC_MSB_SHIFT                       5
#define FTM_CnSC_CHIE_MASK                       0x40u
#define FTM_CnSC_CHIE_SHIFT                      6
#define FTM_CnSC_CHF_MASK                        0x80u
#define FTM_CnSC_CHF_SHIFT                       7
/* CnV Bit Fields */
#define FTM_CnV_VAL_MASK                         0xFFFFu
#define FTM_CnV_VAL_SHIFT                        0
#define FTM_CnV_VAL(x)                           (((uint32)(((uint32)(x))<<FTM_CnV_VAL_SHIFT))&FTM_CnV_VAL_MASK)
/* CNTIN Bit Fields */
#define FTM_CNTIN_INIT_MASK                      0xFFFFu
#define FTM_CNTIN_INIT_SHIFT                     0
#define FTM_CNTIN_INIT(x)                        (((uint32)(((uint32)(x))<<FTM_CNTIN_INIT_SHIFT))&FTM_CNTIN_INIT_MASK)
/* STATUS Bit Fields */
#define FTM_STATUS_CH0F_MASK                     0x1u
#define FTM_STATUS_CH0F_SHIFT                    0
#define FTM_STATUS_CH1F_MASK                     0x2u
#define FTM_STATUS_CH1F_SHIFT                    1
#define FTM_STATUS_CH2F_MASK                     0x4u
#define FTM_STATUS_CH2F_SHIFT                    2
#define FTM_STATUS_CH3F_MASK                     0x8u
#define FTM_STATUS_CH3F_SHIFT                    3
#define FTM_STATUS_CH4F_MASK                     0x10u
#define FTM_STATUS_CH4F_SHIFT                    4
#define FTM_STATUS_CH5F_MASK                     0x20u
#define FTM_STATUS_CH5F_SHIFT                    5
#define FTM_STATUS_CH6F_MASK                     0x40u
#define FTM_STATUS_CH6F_SHIFT                    6
#define FTM_STATUS_CH7F_MASK                     0x80u
#define FTM_STATUS_CH7F_SHIFT                    7
/* MODE Bit Fields */
#define FTM_MODE_FTMEN_MASK                      0x1u
#define FTM_MODE_FTMEN_SHIFT                     0
#define FTM_MODE_INIT_MASK                       0x2u
#define FTM_MODE_INIT_SHIFT                      1
#define FTM_MODE_WPDIS_MASK                      0x4u
#define FTM_MODE_WPDIS_SHIFT                     2
#define FTM_MODE_PWMSYNC_MASK                    0x8u
#define FTM_MODE_PWMSYNC_SHIFT                   3
#define FTM_MODE_CAPTEST_MASK                    0x10u
#define FTM_MODE_CAPTEST_SHIFT                   4
#define FTM_MODE_FAULTM_MASK                     0x60u
#define FTM_MODE_FAULTM_SHIFT                    5
#define FTM_MODE_FAULTM(x)                       (((uint32)(((uint32)(x))<<FTM_MODE_FAULTM_SHIFT))&FTM_MODE_FAULTM_MASK)
#define FTM_MODE_FAULTIE_MASK                    0x80u
#define FTM_MODE_FAULTIE_SHIFT                   7
/* SYNC Bit Fields */
#define FTM_SYNC_CNTMIN_MASK                     0x1u
#define FTM_SYNC_CNTMIN_SHIFT                    0
#define FTM_SYNC_CNTMAX_MASK                     0x2u
#define FTM_SYNC_CNTMAX_SHIFT                    1
#define FTM_SYNC_REINIT_MASK                     0x4u
#define FTM_SYNC_REINIT_SHIFT                    2
#define FTM_SYNC_SYNCHOM_MASK                    0x8u
#define FTM_SYNC_SYNCHOM_SHIFT                   3
#define FTM_SYNC_TRIG0_MASK                      0x10u
#define FTM_SYNC_TRIG0_SHIFT                     4
#define FTM_SYNC_TRIG1_MASK                      0x20u
#define FTM_SYNC_TRIG1_SHIFT                     5
#define FTM_SYNC_TRIG2_MASK                      0x40u
#define FTM_SYNC_TRIG2_SHIFT                     6
#define FTM_SYNC_SWSYNC_MASK                     0x80u
#define FTM_SYNC_SWSYNC_SHIFT                    7
/* OUTINIT Bit Fields */
#define FTM_OUTINIT_CH0OI_MASK                   0x1u
#define FTM_OUTINIT_CH0OI_SHIFT                  0
#define FTM_OUTINIT_CH1OI_MASK                   0x2u
#define FTM_OUTINIT_CH1OI_SHIFT                  1
#define FTM_OUTINIT_CH2OI_MASK                   0x4u
#define FTM_OUTINIT_CH2OI_SHIFT                  2
#define FTM_OUTINIT_CH3OI_MASK                   0x8u
#define FTM_OUTINIT_CH3OI_SHIFT                  3
#define FTM_OUTINIT_CH4OI_MASK                   0x10u
#define FTM_OUTINIT_CH4OI_SHIFT                  4
#define FTM_OUTINIT_CH5OI_MASK                   0x20u
#define FTM_OUTINIT_CH5OI_SHIFT                  5
#define FTM_OUTINIT_CH6OI_MASK                   0x40u
#define FTM_OUTINIT_CH6OI_SHIFT                  6
#define FTM_OUTINIT_CH7OI_MASK                   0x80u
#define FTM_OUTINIT_CH7OI_SHIFT                  7
/* OUTMASK Bit Fields */
#define FTM_OUTMASK_CH0OM_MASK                   0x1u
#define FTM_OUTMASK_CH0OM_SHIFT                  0
#define FTM_OUTMASK_CH1OM_MASK                   0x2u
#define FTM_OUTMASK_CH1OM_SHIFT                  1
#define FTM_OUTMASK_CH2OM_MASK                   0x4u
#define FTM_OUTMASK_CH2OM_SHIFT                  2
#define FTM_OUTMASK_CH3OM_MASK                   0x8u
#define FTM_OUTMASK_CH3OM_SHIFT                  3
#define FTM_OUTMASK_CH4OM_MASK                   0x10u
#define FTM_OUTMASK_CH4OM_SHIFT                  4
#define FTM_OUTMASK_CH5OM_MASK                   0x20u
#define FTM_OUTMASK_CH5OM_SHIFT                  5
#define FTM_OUTMASK_CH6OM_MASK                   0x40u
#define FTM_OUTMASK_CH6OM_SHIFT                  6
#define FTM_OUTMASK_CH7OM_MASK                   0x80u
#define FTM_OUTMASK_CH7OM_SHIFT                  7
/* COMBINE Bit Fields */
#define FTM_COMBINE_COMBINE0_MASK                0x1u
#define FTM_COMBINE_COMBINE0_SHIFT               0
#define FTM_COMBINE_COMP0_MASK                   0x2u
#define FTM_COMBINE_COMP0_SHIFT                  1
#define FTM_COMBINE_DECAPEN0_MASK                0x4u
#define FTM_COMBINE_DECAPEN0_SHIFT               2
#define FTM_COMBINE_DECAP0_MASK                  0x8u
#define FTM_COMBINE_DECAP0_SHIFT                 3
#define FTM_COMBINE_DTEN0_MASK                   0x10u
#define FTM_COMBINE_DTEN0_SHIFT                  4
#define FTM_COMBINE_SYNCEN0_MASK                 0x20u
#define FTM_COMBINE_SYNCEN0_SHIFT                5
#define FTM_COMBINE_FAULTEN0_MASK                0x40u
#define FTM_COMBINE_FAULTEN0_SHIFT               6
#define FTM_COMBINE_COMBINE1_MASK                0x100u
#define FTM_COMBINE_COMBINE1_SHIFT               8
#define FTM_COMBINE_COMP1_MASK                   0x200u
#define FTM_COMBINE_COMP1_SHIFT                  9
#define FTM_COMBINE_DECAPEN1_MASK                0x400u
#define FTM_COMBINE_DECAPEN1_SHIFT               10
#define FTM_COMBINE_DECAP1_MASK                  0x800u
#define FTM_COMBINE_DECAP1_SHIFT                 11
#define FTM_COMBINE_DTEN1_MASK                   0x1000u
#define FTM_COMBINE_DTEN1_SHIFT                  12
#define FTM_COMBINE_SYNCEN1_MASK                 0x2000u
#define FTM_COMBINE_SYNCEN1_SHIFT                13
#define FTM_COMBINE_FAULTEN1_MASK                0x4000u
#define FTM_COMBINE_FAULTEN1_SHIFT               14
#define FTM_COMBINE_COMBINE2_MASK                0x10000u
#define FTM_COMBINE_COMBINE2_SHIFT               16
#define FTM_COMBINE_COMP2_MASK                   0x20000u
#define FTM_COMBINE_COMP2_SHIFT                  17
#define FTM_COMBINE_DECAPEN2_MASK                0x40000u
#define FTM_COMBINE_DECAPEN2_SHIFT               18
#define FTM_COMBINE_DECAP2_MASK                  0x80000u
#define FTM_COMBINE_DECAP2_SHIFT                 19
#define FTM_COMBINE_DTEN2_MASK                   0x100000u
#define FTM_COMBINE_DTEN2_SHIFT                  20
#define FTM_COMBINE_SYNCEN2_MASK                 0x200000u
#define FTM_COMBINE_SYNCEN2_SHIFT                21
#define FTM_COMBINE_FAULTEN2_MASK                0x400000u
#define FTM_COMBINE_FAULTEN2_SHIFT               22
#define FTM_COMBINE_COMBINE3_MASK                0x1000000u
#define FTM_COMBINE_COMBINE3_SHIFT               24
#define FTM_COMBINE_COMP3_MASK                   0x2000000u
#define FTM_COMBINE_COMP3_SHIFT                  25
#define FTM_COMBINE_DECAPEN3_MASK                0x4000000u
#define FTM_COMBINE_DECAPEN3_SHIFT               26
#define FTM_COMBINE_DECAP3_MASK                  0x8000000u
#define FTM_COMBINE_DECAP3_SHIFT                 27
#define FTM_COMBINE_DTEN3_MASK                   0x10000000u
#define FTM_COMBINE_DTEN3_SHIFT                  28
#define FTM_COMBINE_SYNCEN3_MASK                 0x20000000u
#define FTM_COMBINE_SYNCEN3_SHIFT                29
#define FTM_COMBINE_FAULTEN3_MASK                0x40000000u
#define FTM_COMBINE_FAULTEN3_SHIFT               30
/* DEADTIME Bit Fields */
#define FTM_DEADTIME_DTVAL_MASK                  0x3Fu
#define FTM_DEADTIME_DTVAL_SHIFT                 0
#define FTM_DEADTIME_DTVAL(x)                    (((uint32)(((uint32)(x))<<FTM_DEADTIME_DTVAL_SHIFT))&FTM_DEADTIME_DTVAL_MASK)
#define FTM_DEADTIME_DTPS_MASK                   0xC0u
#define FTM_DEADTIME_DTPS_SHIFT                  6
#define FTM_DEADTIME_DTPS(x)                     (((uint32)(((uint32)(x))<<FTM_DEADTIME_DTPS_SHIFT))&FTM_DEADTIME_DTPS_MASK)
/* EXTTRIG Bit Fields */
#define FTM_EXTTRIG_CH2TRIG_MASK                 0x1u
#define FTM_EXTTRIG_CH2TRIG_SHIFT                0
#define FTM_EXTTRIG_CH3TRIG_MASK                 0x2u
#define FTM_EXTTRIG_CH3TRIG_SHIFT                1
#define FTM_EXTTRIG_CH4TRIG_MASK                 0x4u
#define FTM_EXTTRIG_CH4TRIG_SHIFT                2
#define FTM_EXTTRIG_CH5TRIG_MASK                 0x8u
#define FTM_EXTTRIG_CH5TRIG_SHIFT                3
#define FTM_EXTTRIG_CH0TRIG_MASK                 0x10u
#define FTM_EXTTRIG_CH0TRIG_SHIFT                4
#define FTM_EXTTRIG_CH1TRIG_MASK                 0x20u
#define FTM_EXTTRIG_CH1TRIG_SHIFT                5
#define FTM_EXTTRIG_INITTRIGEN_MASK              0x40u
#define FTM_EXTTRIG_INITTRIGEN_SHIFT             6
#define FTM_EXTTRIG_TRIGF_MASK                   0x80u
#define FTM_EXTTRIG_TRIGF_SHIFT                  7
/* POL Bit Fields */
#define FTM_POL_POL0_MASK                        0x1u
#define FTM_POL_POL0_SHIFT                       0
#define FTM_POL_POL1_MASK                        0x2u
#define FTM_POL_POL1_SHIFT                       1
#define FTM_POL_POL2_MASK                        0x4u
#define FTM_POL_POL2_SHIFT                       2
#define FTM_POL_POL3_MASK                        0x8u
#define FTM_POL_POL3_SHIFT                       3
#define FTM_POL_POL4_MASK                        0x10u
#define FTM_POL_POL4_SHIFT                       4
#define FTM_POL_POL5_MASK                        0x20u
#define FTM_POL_POL5_SHIFT                       5
#define FTM_POL_POL6_MASK                        0x40u
#define FTM_POL_POL6_SHIFT                       6
#define FTM_POL_POL7_MASK                        0x80u
#define FTM_POL_POL7_SHIFT                       7
/* FMS Bit Fields */
#define FTM_FMS_FAULTF0_MASK                     0x1u
#define FTM_FMS_FAULTF0_SHIFT                    0
#define FTM_FMS_FAULTF1_MASK                     0x2u
#define FTM_FMS_FAULTF1_SHIFT                    1
#define FTM_FMS_FAULTF2_MASK                     0x4u
#define FTM_FMS_FAULTF2_SHIFT                    2
#define FTM_FMS_FAULTF3_MASK                     0x8u
#define FTM_FMS_FAULTF3_SHIFT                    3
#define FTM_FMS_FAULTIN_MASK                     0x20u
#define FTM_FMS_FAULTIN_SHIFT                    5
#define FTM_FMS_WPEN_MASK                        0x40u
#define FTM_FMS_WPEN_SHIFT                       6
#define FTM_FMS_FAULTF_MASK                      0x80u
#define FTM_FMS_FAULTF_SHIFT                     7
/* FILTER Bit Fields */
#define FTM_FILTER_CH0FVAL_MASK                  0xFu
#define FTM_FILTER_CH0FVAL_SHIFT                 0
#define FTM_FILTER_CH0FVAL(x)                    (((uint32)(((uint32)(x))<<FTM_FILTER_CH0FVAL_SHIFT))&FTM_FILTER_CH0FVAL_MASK)
#define FTM_FILTER_CH1FVAL_MASK                  0xF0u
#define FTM_FILTER_CH1FVAL_SHIFT                 4
#define FTM_FILTER_CH1FVAL(x)                    (((uint32)(((uint32)(x))<<FTM_FILTER_CH1FVAL_SHIFT))&FTM_FILTER_CH1FVAL_MASK)
#define FTM_FILTER_CH2FVAL_MASK                  0xF00u
#define FTM_FILTER_CH2FVAL_SHIFT                 8
#define FTM_FILTER_CH2FVAL(x)                    (((uint32)(((uint32)(x))<<FTM_FILTER_CH2FVAL_SHIFT))&FTM_FILTER_CH2FVAL_MASK)
#define FTM_FILTER_CH3FVAL_MASK                  0xF000u
#define FTM_FILTER_CH3FVAL_SHIFT                 12
#define FTM_FILTER_CH3FVAL(x)                    (((uint32)(((uint32)(x))<<FTM_FILTER_CH3FVAL_SHIFT))&FTM_FILTER_CH3FVAL_MASK)
/* FLTCTRL Bit Fields */
#define FTM_FLTCTRL_FAULT0EN_MASK                0x1u
#define FTM_FLTCTRL_FAULT0EN_SHIFT               0
#define FTM_FLTCTRL_FAULT1EN_MASK                0x2u
#define FTM_FLTCTRL_FAULT1EN_SHIFT               1
#define FTM_FLTCTRL_FAULT2EN_MASK                0x4u
#define FTM_FLTCTRL_FAULT2EN_SHIFT               2
#define FTM_FLTCTRL_FAULT3EN_MASK                0x8u
#define FTM_FLTCTRL_FAULT3EN_SHIFT               3
#define FTM_FLTCTRL_FFLTR0EN_MASK                0x10u
#define FTM_FLTCTRL_FFLTR0EN_SHIFT               4
#define FTM_FLTCTRL_FFLTR1EN_MASK                0x20u
#define FTM_FLTCTRL_FFLTR1EN_SHIFT               5
#define FTM_FLTCTRL_FFLTR2EN_MASK                0x40u
#define FTM_FLTCTRL_FFLTR2EN_SHIFT               6
#define FTM_FLTCTRL_FFLTR3EN_MASK                0x80u
#define FTM_FLTCTRL_FFLTR3EN_SHIFT               7
#define FTM_FLTCTRL_FFVAL_MASK                   0xF00u
#define FTM_FLTCTRL_FFVAL_SHIFT                  8
#define FTM_FLTCTRL_FFVAL(x)                     (((uint32)(((uint32)(x))<<FTM_FLTCTRL_FFVAL_SHIFT))&FTM_FLTCTRL_FFVAL_MASK)
/* QDCTRL Bit Fields */
#define FTM_QDCTRL_QUADEN_MASK                   0x1u
#define FTM_QDCTRL_QUADEN_SHIFT                  0
#define FTM_QDCTRL_TOFDIR_MASK                   0x2u
#define FTM_QDCTRL_TOFDIR_SHIFT                  1
#define FTM_QDCTRL_QUADIR_MASK                   0x4u
#define FTM_QDCTRL_QUADIR_SHIFT                  2
#define FTM_QDCTRL_QUADMODE_MASK                 0x8u
#define FTM_QDCTRL_QUADMODE_SHIFT                3
#define FTM_QDCTRL_PHBPOL_MASK                   0x10u
#define FTM_QDCTRL_PHBPOL_SHIFT                  4
#define FTM_QDCTRL_PHAPOL_MASK                   0x20u
#define FTM_QDCTRL_PHAPOL_SHIFT                  5
#define FTM_QDCTRL_PHBFLTREN_MASK                0x40u
#define FTM_QDCTRL_PHBFLTREN_SHIFT               6
#define FTM_QDCTRL_PHAFLTREN_MASK                0x80u
#define FTM_QDCTRL_PHAFLTREN_SHIFT               7
/* CONF Bit Fields */
#define FTM_CONF_NUMTOF_MASK                     0x1Fu
#define FTM_CONF_NUMTOF_SHIFT                    0
#define FTM_CONF_NUMTOF(x)                       (((uint32)(((uint32)(x))<<FTM_CONF_NUMTOF_SHIFT))&FTM_CONF_NUMTOF_MASK)
#define FTM_CONF_BDMMODE_MASK                    0xC0u
#define FTM_CONF_BDMMODE_SHIFT                   6
#define FTM_CONF_BDMMODE(x)                      (((uint32)(((uint32)(x))<<FTM_CONF_BDMMODE_SHIFT))&FTM_CONF_BDMMODE_MASK)
#define FTM_CONF_GTBEEN_MASK                     0x200u
#define FTM_CONF_GTBEEN_SHIFT                    9
#define FTM_CONF_GTBEOUT_MASK                    0x400u
#define FTM_CONF_GTBEOUT_SHIFT                   10
/* FLTPOL Bit Fields */
#define FTM_FLTPOL_FLT0POL_MASK                  0x1u
#define FTM_FLTPOL_FLT0POL_SHIFT                 0
#define FTM_FLTPOL_FLT1POL_MASK                  0x2u
#define FTM_FLTPOL_FLT1POL_SHIFT                 1
#define FTM_FLTPOL_FLT2POL_MASK                  0x4u
#define FTM_FLTPOL_FLT2POL_SHIFT                 2
#define FTM_FLTPOL_FLT3POL_MASK                  0x8u
#define FTM_FLTPOL_FLT3POL_SHIFT                 3
/* SYNCONF Bit Fields */
#define FTM_SYNCONF_HWTRIGMODE_MASK              0x1u
#define FTM_SYNCONF_HWTRIGMODE_SHIFT             0
#define FTM_SYNCONF_CNTINC_MASK                  0x4u
#define FTM_SYNCONF_CNTINC_SHIFT                 2
#define FTM_SYNCONF_INVC_MASK                    0x10u
#define FTM_SYNCONF_INVC_SHIFT                   4
#define FTM_SYNCONF_SWOC_MASK                    0x20u
#define FTM_SYNCONF_SWOC_SHIFT                   5
#define FTM_SYNCONF_SYNCMODE_MASK                0x80u
#define FTM_SYNCONF_SYNCMODE_SHIFT               7
#define FTM_SYNCONF_SWRSTCNT_MASK                0x100u
#define FTM_SYNCONF_SWRSTCNT_SHIFT               8
#define FTM_SYNCONF_SWWRBUF_MASK                 0x200u
#define FTM_SYNCONF_SWWRBUF_SHIFT                9
#define FTM_SYNCONF_SWOM_MASK                    0x400u
#define FTM_SYNCONF_SWOM_SHIFT                   10
#define FTM_SYNCONF_SWINVC_MASK                  0x800u
#define FTM_SYNCONF_SWINVC_SHIFT                 11
#define FTM_SYNCONF_SWSOC_MASK                   0x1000u
#define FTM_SYNCONF_SWSOC_SHIFT                  12
#define FTM_SYNCONF_HWRSTCNT_MASK                0x10000u
#define FTM_SYNCONF_HWRSTCNT_SHIFT               16
#define FTM_SYNCONF_HWWRBUF_MASK                 0x20000u
#define FTM_SYNCONF_HWWRBUF_SHIFT                17
#define FTM_SYNCONF_HWOM_MASK                    0x40000u
#define FTM_SYNCONF_HWOM_SHIFT                   18
#define FTM_SYNCONF_HWINVC_MASK                  0x80000u
#define FTM_SYNCONF_HWINVC_SHIFT                 19
#define FTM_SYNCONF_HWSOC_MASK                   0x100000u
#define FTM_SYNCONF_HWSOC_SHIFT                  20
/* INVCTRL Bit Fields */
#define FTM_INVCTRL_INV0EN_MASK                  0x1u
#define FTM_INVCTRL_INV0EN_SHIFT                 0
#define FTM_INVCTRL_INV1EN_MASK                  0x2u
#define FTM_INVCTRL_INV1EN_SHIFT                 1
#define FTM_INVCTRL_INV2EN_MASK                  0x4u
#define FTM_INVCTRL_INV2EN_SHIFT                 2
#define FTM_INVCTRL_INV3EN_MASK                  0x8u
#define FTM_INVCTRL_INV3EN_SHIFT                 3
/* SWOCTRL Bit Fields */
#define FTM_SWOCTRL_CH0OC_MASK                   0x1u
#define FTM_SWOCTRL_CH0OC_SHIFT                  0
#define FTM_SWOCTRL_CH1OC_MASK                   0x2u
#define FTM_SWOCTRL_CH1OC_SHIFT                  1
#define FTM_SWOCTRL_CH2OC_MASK                   0x4u
#define FTM_SWOCTRL_CH2OC_SHIFT                  2
#define FTM_SWOCTRL_CH3OC_MASK                   0x8u
#define FTM_SWOCTRL_CH3OC_SHIFT                  3
#define FTM_SWOCTRL_CH4OC_MASK                   0x10u
#define FTM_SWOCTRL_CH4OC_SHIFT                  4
#define FTM_SWOCTRL_CH5OC_MASK                   0x20u
#define FTM_SWOCTRL_CH5OC_SHIFT                  5
#define FTM_SWOCTRL_CH6OC_MASK                   0x40u
#define FTM_SWOCTRL_CH6OC_SHIFT                  6
#define FTM_SWOCTRL_CH7OC_MASK                   0x80u
#define FTM_SWOCTRL_CH7OC_SHIFT                  7
#define FTM_SWOCTRL_CH0OCV_MASK                  0x100u
#define FTM_SWOCTRL_CH0OCV_SHIFT                 8
#define FTM_SWOCTRL_CH1OCV_MASK                  0x200u
#define FTM_SWOCTRL_CH1OCV_SHIFT                 9
#define FTM_SWOCTRL_CH2OCV_MASK                  0x400u
#define FTM_SWOCTRL_CH2OCV_SHIFT                 10
#define FTM_SWOCTRL_CH3OCV_MASK                  0x800u
#define FTM_SWOCTRL_CH3OCV_SHIFT                 11
#define FTM_SWOCTRL_CH4OCV_MASK                  0x1000u
#define FTM_SWOCTRL_CH4OCV_SHIFT                 12
#define FTM_SWOCTRL_CH5OCV_MASK                  0x2000u
#define FTM_SWOCTRL_CH5OCV_SHIFT                 13
#define FTM_SWOCTRL_CH6OCV_MASK                  0x4000u
#define FTM_SWOCTRL_CH6OCV_SHIFT                 14
#define FTM_SWOCTRL_CH7OCV_MASK                  0x8000u
#define FTM_SWOCTRL_CH7OCV_SHIFT                 15
/* PWMLOAD Bit Fields */
#define FTM_PWMLOAD_CH0SEL_MASK                  0x1u
#define FTM_PWMLOAD_CH0SEL_SHIFT                 0
#define FTM_PWMLOAD_CH1SEL_MASK                  0x2u
#define FTM_PWMLOAD_CH1SEL_SHIFT                 1
#define FTM_PWMLOAD_CH2SEL_MASK                  0x4u
#define FTM_PWMLOAD_CH2SEL_SHIFT                 2
#define FTM_PWMLOAD_CH3SEL_MASK                  0x8u
#define FTM_PWMLOAD_CH3SEL_SHIFT                 3
#define FTM_PWMLOAD_CH4SEL_MASK                  0x10u
#define FTM_PWMLOAD_CH4SEL_SHIFT                 4
#define FTM_PWMLOAD_CH5SEL_MASK                  0x20u
#define FTM_PWMLOAD_CH5SEL_SHIFT                 5
#define FTM_PWMLOAD_CH6SEL_MASK                  0x40u
#define FTM_PWMLOAD_CH6SEL_SHIFT                 6
#define FTM_PWMLOAD_CH7SEL_MASK                  0x80u
#define FTM_PWMLOAD_CH7SEL_SHIFT                 7
#define FTM_PWMLOAD_LDOK_MASK                    0x200u
#define FTM_PWMLOAD_LDOK_SHIFT                   9

/* FTM - Peripheral instance base addresses */
/** Peripheral FTM0 base pointer */
#define FTM0_BASE_PTR                            ((FTM_MemMapPtr)0x40038000u)
/** Peripheral FTM1 base pointer */
#define FTM1_BASE_PTR                            ((FTM_MemMapPtr)0x40039000u)
/** Peripheral FTM2 base pointer */
#define FTM2_BASE_PTR                            ((FTM_MemMapPtr)0x4003A000u)
/** Peripheral FTM3 base pointer */
#define FTM3_BASE_PTR                            ((FTM_MemMapPtr)0x40026000u)
/** Array initializer of FTM peripheral base pointers */
#define FTM_BASE_PTRS                            { FTM0_BASE_PTR, FTM1_BASE_PTR, FTM2_BASE_PTR, FTM3_BASE_PTR }

/* ----------------------------------------------------------------------------
   -- FTM - Register accessor macros
   ---------------------------------------------------------------------------- */

/* FTM - Register instance definitions */
/* FTM0 */
#define FTM0_SC                                  FTM_SC_REG(FTM0_BASE_PTR)
#define FTM0_CNT                                 FTM_CNT_REG(FTM0_BASE_PTR)
#define FTM0_MOD                                 FTM_MOD_REG(FTM0_BASE_PTR)
#define FTM0_C0SC                                FTM_CnSC_REG(FTM0_BASE_PTR,0)
#define FTM0_C0V                                 FTM_CnV_REG(FTM0_BASE_PTR,0)
#define FTM0_C1SC                                FTM_CnSC_REG(FTM0_BASE_PTR,1)
#define FTM0_C1V                                 FTM_CnV_REG(FTM0_BASE_PTR,1)
#define FTM0_C2SC                                FTM_CnSC_REG(FTM0_BASE_PTR,2)
#define FTM0_C2V                                 FTM_CnV_REG(FTM0_BASE_PTR,2)
#define FTM0_C3SC                                FTM_CnSC_REG(FTM0_BASE_PTR,3)
#define FTM0_C3V                                 FTM_CnV_REG(FTM0_BASE_PTR,3)
#define FTM0_C4SC                                FTM_CnSC_REG(FTM0_BASE_PTR,4)
#define FTM0_C4V                                 FTM_CnV_REG(FTM0_BASE_PTR,4)
#define FTM0_C5SC                                FTM_CnSC_REG(FTM0_BASE_PTR,5)
#define FTM0_C5V                                 FTM_CnV_REG(FTM0_BASE_PTR,5)
#define FTM0_C6SC                                FTM_CnSC_REG(FTM0_BASE_PTR,6)
#define FTM0_C6V                                 FTM_CnV_REG(FTM0_BASE_PTR,6)
#define FTM0_C7SC                                FTM_CnSC_REG(FTM0_BASE_PTR,7)
#define FTM0_C7V                                 FTM_CnV_REG(FTM0_BASE_PTR,7)
#define FTM0_CNTIN                               FTM_CNTIN_REG(FTM0_BASE_PTR)
#define FTM0_STATUS                              FTM_STATUS_REG(FTM0_BASE_PTR)
#define FTM0_MODE                                FTM_MODE_REG(FTM0_BASE_PTR)
#define FTM0_SYNC                                FTM_SYNC_REG(FTM0_BASE_PTR)
#define FTM0_OUTINIT                             FTM_OUTINIT_REG(FTM0_BASE_PTR)
#define FTM0_OUTMASK                             FTM_OUTMASK_REG(FTM0_BASE_PTR)
#define FTM0_COMBINE                             FTM_COMBINE_REG(FTM0_BASE_PTR)
#define FTM0_DEADTIME                            FTM_DEADTIME_REG(FTM0_BASE_PTR)
#define FTM0_EXTTRIG                             FTM_EXTTRIG_REG(FTM0_BASE_PTR)
#define FTM0_POL                                 FTM_POL_REG(FTM0_BASE_PTR)
#define FTM0_FMS                                 FTM_FMS_REG(FTM0_BASE_PTR)
#define FTM0_FILTER                              FTM_FILTER_REG(FTM0_BASE_PTR)
#define FTM0_FLTCTRL                             FTM_FLTCTRL_REG(FTM0_BASE_PTR)
#define FTM0_QDCTRL                              FTM_QDCTRL_REG(FTM0_BASE_PTR)
#define FTM0_CONF                                FTM_CONF_REG(FTM0_BASE_PTR)
#define FTM0_FLTPOL                              FTM_FLTPOL_REG(FTM0_BASE_PTR)
#define FTM0_SYNCONF                             FTM_SYNCONF_REG(FTM0_BASE_PTR)
#define FTM0_INVCTRL                             FTM_INVCTRL_REG(FTM0_BASE_PTR)
#define FTM0_SWOCTRL                             FTM_SWOCTRL_REG(FTM0_BASE_PTR)
#define FTM0_PWMLOAD                             FTM_PWMLOAD_REG(FTM0_BASE_PTR)
/* FTM1 */
#define FTM1_SC                                  FTM_SC_REG(FTM1_BASE_PTR)
#define FTM1_CNT                                 FTM_CNT_REG(FTM1_BASE_PTR)
#define FTM1_MOD                                 FTM_MOD_REG(FTM1_BASE_PTR)
#define FTM1_C0SC                                FTM_CnSC_REG(FTM1_BASE_PTR,0)
#define FTM1_C0V                                 FTM_CnV_REG(FTM1_BASE_PTR,0)
#define FTM1_C1SC                                FTM_CnSC_REG(FTM1_BASE_PTR,1)
#define FTM1_C1V                                 FTM_CnV_REG(FTM1_BASE_PTR,1)
#define FTM1_CNTIN                               FTM_CNTIN_REG(FTM1_BASE_PTR)
#define FTM1_STATUS                              FTM_STATUS_REG(FTM1_BASE_PTR)
#define FTM1_MODE                                FTM_MODE_REG(FTM1_BASE_PTR)
#define FTM1_SYNC                                FTM_SYNC_REG(FTM1_BASE_PTR)
#define FTM1_OUTINIT                             FTM_OUTINIT_REG(FTM1_BASE_PTR)
#define FTM1_OUTMASK                             FTM_OUTMASK_REG(FTM1_BASE_PTR)
#define FTM1_COMBINE                             FTM_COMBINE_REG(FTM1_BASE_PTR)
#define FTM1_DEADTIME                            FTM_DEADTIME_REG(FTM1_BASE_PTR)
#define FTM1_EXTTRIG                             FTM_EXTTRIG_REG(FTM1_BASE_PTR)
#define FTM1_POL                                 FTM_POL_REG(FTM1_BASE_PTR)
#define FTM1_FMS                                 FTM_FMS_REG(FTM1_BASE_PTR)
#define FTM1_FILTER                              FTM_FILTER_REG(FTM1_BASE_PTR)
#define FTM1_FLTCTRL                             FTM_FLTCTRL_REG(FTM1_BASE_PTR)
#define FTM1_QDCTRL                              FTM_QDCTRL_REG(FTM1_BASE_PTR)
#define FTM1_CONF                                FTM_CONF_REG(FTM1_BASE_PTR)
#define FTM1_FLTPOL                              FTM_FLTPOL_REG(FTM1_BASE_PTR)
#define FTM1_SYNCONF                             FTM_SYNCONF_REG(FTM1_BASE_PTR)
#define FTM1_INVCTRL                             FTM_INVCTRL_REG(FTM1_BASE_PTR)
#define FTM1_SWOCTRL                             FTM_SWOCTRL_REG(FTM1_BASE_PTR)
#define FTM1_PWMLOAD                             FTM_PWMLOAD_REG(FTM1_BASE_PTR)
/* FTM2 */
#define FTM2_SC                                  FTM_SC_REG(FTM2_BASE_PTR)
#define FTM2_CNT                                 FTM_CNT_REG(FTM2_BASE_PTR)
#define FTM2_MOD                                 FTM_MOD_REG(FTM2_BASE_PTR)
#define FTM2_C0SC                                FTM_CnSC_REG(FTM2_BASE_PTR,0)
#define FTM2_C0V                                 FTM_CnV_REG(FTM2_BASE_PTR,0)
#define FTM2_C1SC                                FTM_CnSC_REG(FTM2_BASE_PTR,1)
#define FTM2_C1V                                 FTM_CnV_REG(FTM2_BASE_PTR,1)
#define FTM2_CNTIN                               FTM_CNTIN_REG(FTM2_BASE_PTR)
#define FTM2_STATUS                              FTM_STATUS_REG(FTM2_BASE_PTR)
#define FTM2_MODE                                FTM_MODE_REG(FTM2_BASE_PTR)
#define FTM2_SYNC                                FTM_SYNC_REG(FTM2_BASE_PTR)
#define FTM2_OUTINIT                             FTM_OUTINIT_REG(FTM2_BASE_PTR)
#define FTM2_OUTMASK                             FTM_OUTMASK_REG(FTM2_BASE_PTR)
#define FTM2_COMBINE                             FTM_COMBINE_REG(FTM2_BASE_PTR)
#define FTM2_DEADTIME                            FTM_DEADTIME_REG(FTM2_BASE_PTR)
#define FTM2_EXTTRIG                             FTM_EXTTRIG_REG(FTM2_BASE_PTR)
#define FTM2_POL                                 FTM_POL_REG(FTM2_BASE_PTR)
#define FTM2_FMS                                 FTM_FMS_REG(FTM2_BASE_PTR)
#define FTM2_FILTER                              FTM_FILTER_REG(FTM2_BASE_PTR)
#define FTM2_FLTCTRL                             FTM_FLTCTRL_REG(FTM2_BASE_PTR)
#define FTM2_QDCTRL                              FTM_QDCTRL_REG(FTM2_BASE_PTR)
#define FTM2_CONF                                FTM_CONF_REG(FTM2_BASE_PTR)
#define FTM2_FLTPOL                              FTM_FLTPOL_REG(FTM2_BASE_PTR)
#define FTM2_SYNCONF                             FTM_SYNCONF_REG(FTM2_BASE_PTR)
#define FTM2_INVCTRL                             FTM_INVCTRL_REG(FTM2_BASE_PTR)
#define FTM2_SWOCTRL                             FTM_SWOCTRL_REG(FTM2_BASE_PTR)
#define FTM2_PWMLOAD                             FTM_PWMLOAD_REG(FTM2_BASE_PTR)
/* FTM3 */
#define FTM3_SC                                  FTM_SC_REG(FTM3_BASE_PTR)
#define FTM3_CNT                                 FTM_CNT_REG(FTM3_BASE_PTR)
#define FTM3_MOD                                 FTM_MOD_REG(FTM3_BASE_PTR)
#define FTM3_C0SC                                FTM_CnSC_REG(FTM3_BASE_PTR,0)
#define FTM3_C0V                                 FTM_CnV_REG(FTM3_BASE_PTR,0)
#define FTM3_C1SC                                FTM_CnSC_REG(FTM3_BASE_PTR,1)
#define FTM3_C1V                                 FTM_CnV_REG(FTM3_BASE_PTR,1)
#define FTM3_C2SC                                FTM_CnSC_REG(FTM3_BASE_PTR,2)
#define FTM3_C2V                                 FTM_CnV_REG(FTM3_BASE_PTR,2)
#define FTM3_C3SC                                FTM_CnSC_REG(FTM3_BASE_PTR,3)
#define FTM3_C3V                                 FTM_CnV_REG(FTM3_BASE_PTR,3)
#define FTM3_C4SC                                FTM_CnSC_REG(FTM3_BASE_PTR,4)
#define FTM3_C4V                                 FTM_CnV_REG(FTM3_BASE_PTR,4)
#define FTM3_C5SC                                FTM_CnSC_REG(FTM3_BASE_PTR,5)
#define FTM3_C5V                                 FTM_CnV_REG(FTM3_BASE_PTR,5)
#define FTM3_C6SC                                FTM_CnSC_REG(FTM3_BASE_PTR,6)
#define FTM3_C6V                                 FTM_CnV_REG(FTM3_BASE_PTR,6)
#define FTM3_C7SC                                FTM_CnSC_REG(FTM3_BASE_PTR,7)
#define FTM3_C7V                                 FTM_CnV_REG(FTM3_BASE_PTR,7)
#define FTM3_CNTIN                               FTM_CNTIN_REG(FTM3_BASE_PTR)
#define FTM3_STATUS                              FTM_STATUS_REG(FTM3_BASE_PTR)
#define FTM3_MODE                                FTM_MODE_REG(FTM3_BASE_PTR)
#define FTM3_SYNC                                FTM_SYNC_REG(FTM3_BASE_PTR)
#define FTM3_OUTINIT                             FTM_OUTINIT_REG(FTM3_BASE_PTR)
#define FTM3_OUTMASK                             FTM_OUTMASK_REG(FTM3_BASE_PTR)
#define FTM3_COMBINE                             FTM_COMBINE_REG(FTM3_BASE_PTR)
#define FTM3_DEADTIME                            FTM_DEADTIME_REG(FTM3_BASE_PTR)
#define FTM3_EXTTRIG                             FTM_EXTTRIG_REG(FTM3_BASE_PTR)
#define FTM3_POL                                 FTM_POL_REG(FTM3_BASE_PTR)
#define FTM3_FMS                                 FTM_FMS_REG(FTM3_BASE_PTR)
#define FTM3_FILTER                              FTM_FILTER_REG(FTM3_BASE_PTR)
#define FTM3_FLTCTRL                             FTM_FLTCTRL_REG(FTM3_BASE_PTR)
#define FTM3_QDCTRL                              FTM_QDCTRL_REG(FTM3_BASE_PTR)
#define FTM3_CONF                                FTM_CONF_REG(FTM3_BASE_PTR)
#define FTM3_FLTPOL                              FTM_FLTPOL_REG(FTM3_BASE_PTR)
#define FTM3_SYNCONF                             FTM_SYNCONF_REG(FTM3_BASE_PTR)
#define FTM3_INVCTRL                             FTM_INVCTRL_REG(FTM3_BASE_PTR)
#define FTM3_SWOCTRL                             FTM_SWOCTRL_REG(FTM3_BASE_PTR)
#define FTM3_PWMLOAD                             FTM_PWMLOAD_REG(FTM3_BASE_PTR)

/* FTM - Register array accessors */
#define FTM0_CnSC(index)                         FTM_CnSC_REG(FTM0_BASE_PTR,index)
#define FTM1_CnSC(index)                         FTM_CnSC_REG(FTM1_BASE_PTR,index)
#define FTM2_CnSC(index)                         FTM_CnSC_REG(FTM2_BASE_PTR,index)
#define FTM3_CnSC(index)                         FTM_CnSC_REG(FTM3_BASE_PTR,index)
#define FTM0_CnV(index)                          FTM_CnV_REG(FTM0_BASE_PTR,index)
#define FTM1_CnV(index)                          FTM_CnV_REG(FTM1_BASE_PTR,index)
#define FTM2_CnV(index)                          FTM_CnV_REG(FTM2_BASE_PTR,index)
#define FTM3_CnV(index)                          FTM_CnV_REG(FTM3_BASE_PTR,index)

/* ----------------------------------------------------------------------------
   -- SPI
   ---------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
   -- SPI Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Peripheral_Access_Layer SPI Peripheral Access Layer
 * @{
 */

/** SPI - Register Layout Typedef */
typedef struct {
  volatile uint32 MCR;                               /**< Module Configuration Register, offset: 0x0 */
       uint8 RESERVED_0[4];
  volatile uint32 TCR;                               /**< Transfer Count Register, offset: 0x8 */
  union {                                          /* offset: 0xC */
    volatile uint32 CTAR[2];                           /**< Clock and Transfer Attributes Register (In Master Mode), array offset: 0xC, array step: 0x4 */
    volatile uint32 CTAR_SLAVE[1];                     /**< Clock and Transfer Attributes Register (In Slave Mode), array offset: 0xC, array step: 0x4 */
  };
       uint8 RESERVED_1[24];
  volatile uint32 SR;                                /**< Status Register, offset: 0x2C */
  volatile uint32 RSER;                              /**< DMA/Interrupt Request Select and Enable Register, offset: 0x30 */
  union {                                          /* offset: 0x34 */
    volatile uint32 PUSHR;                             /**< PUSH TX FIFO Register In Master Mode, offset: 0x34 */
    volatile uint32 PUSHR_SLAVE;                       /**< PUSH TX FIFO Register In Slave Mode, offset: 0x34 */
  };
  volatile  uint32 POPR;                              /**< POP RX FIFO Register, offset: 0x38 */
  volatile  uint32 TXFR0;                             /**< Transmit FIFO Registers, offset: 0x3C */
  volatile  uint32 TXFR1;                             /**< Transmit FIFO Registers, offset: 0x40 */
  volatile  uint32 TXFR2;                             /**< Transmit FIFO Registers, offset: 0x44 */
  volatile  uint32 TXFR3;                             /**< Transmit FIFO Registers, offset: 0x48 */
       uint8 RESERVED_2[48];
  volatile  uint32 RXFR0;                             /**< Receive FIFO Registers, offset: 0x7C */
  volatile  uint32 RXFR1;                             /**< Receive FIFO Registers, offset: 0x80 */
  volatile  uint32 RXFR2;                             /**< Receive FIFO Registers, offset: 0x84 */
  volatile  uint32 RXFR3;                             /**< Receive FIFO Registers, offset: 0x88 */
} SPI_TYPE;

/* ----------------------------------------------------------------------------
   -- SPI Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_Register_Masks SPI Register Masks
 * @{
 */

/*! @name MCR - Module Configuration Register */
#define SPI_MCR_HALT_MASK                        (0x1U)
#define SPI_MCR_HALT_SHIFT                       (0U)
#define SPI_MCR_HALT(x)                          (((uint32)(((uint32)(x)) << SPI_MCR_HALT_SHIFT)) & SPI_MCR_HALT_MASK)
#define SPI_MCR_SMPL_PT_MASK                     (0x300U)
#define SPI_MCR_SMPL_PT_SHIFT                    (8U)
#define SPI_MCR_SMPL_PT(x)                       (((uint32)(((uint32)(x)) << SPI_MCR_SMPL_PT_SHIFT)) & SPI_MCR_SMPL_PT_MASK)
#define SPI_MCR_CLR_RXF_MASK                     (0x400U)
#define SPI_MCR_CLR_RXF_SHIFT                    (10U)
#define SPI_MCR_CLR_RXF(x)                       (((uint32)(((uint32)(x)) << SPI_MCR_CLR_RXF_SHIFT)) & SPI_MCR_CLR_RXF_MASK)
#define SPI_MCR_CLR_TXF_MASK                     (0x800U)
#define SPI_MCR_CLR_TXF_SHIFT                    (11U)
#define SPI_MCR_CLR_TXF(x)                       (((uint32)(((uint32)(x)) << SPI_MCR_CLR_TXF_SHIFT)) & SPI_MCR_CLR_TXF_MASK)
#define SPI_MCR_DIS_RXF_MASK                     (0x1000U)
#define SPI_MCR_DIS_RXF_SHIFT                    (12U)
#define SPI_MCR_DIS_RXF(x)                       (((uint32)(((uint32)(x)) << SPI_MCR_DIS_RXF_SHIFT)) & SPI_MCR_DIS_RXF_MASK)
#define SPI_MCR_DIS_TXF_MASK                     (0x2000U)
#define SPI_MCR_DIS_TXF_SHIFT                    (13U)
#define SPI_MCR_DIS_TXF(x)                       (((uint32)(((uint32)(x)) << SPI_MCR_DIS_TXF_SHIFT)) & SPI_MCR_DIS_TXF_MASK)
#define SPI_MCR_MDIS_MASK                        (0x4000U)
#define SPI_MCR_MDIS_SHIFT                       (14U)
#define SPI_MCR_MDIS(x)                          (((uint32)(((uint32)(x)) << SPI_MCR_MDIS_SHIFT)) & SPI_MCR_MDIS_MASK)
#define SPI_MCR_DOZE_MASK                        (0x8000U)
#define SPI_MCR_DOZE_SHIFT                       (15U)
#define SPI_MCR_DOZE(x)                          (((uint32)(((uint32)(x)) << SPI_MCR_DOZE_SHIFT)) & SPI_MCR_DOZE_MASK)
#define SPI_MCR_PCSIS_MASK                       (0x3F0000U)
#define SPI_MCR_PCSIS_SHIFT                      (16U)
#define SPI_MCR_PCSIS(x)                         (((uint32)(((uint32)(x)) << SPI_MCR_PCSIS_SHIFT)) & SPI_MCR_PCSIS_MASK)
#define SPI_MCR_ROOE_MASK                        (0x1000000U)
#define SPI_MCR_ROOE_SHIFT                       (24U)
#define SPI_MCR_ROOE(x)                          (((uint32)(((uint32)(x)) << SPI_MCR_ROOE_SHIFT)) & SPI_MCR_ROOE_MASK)
#define SPI_MCR_PCSSE_MASK                       (0x2000000U)
#define SPI_MCR_PCSSE_SHIFT                      (25U)
#define SPI_MCR_PCSSE(x)                         (((uint32)(((uint32)(x)) << SPI_MCR_PCSSE_SHIFT)) & SPI_MCR_PCSSE_MASK)
#define SPI_MCR_MTFE_MASK                        (0x4000000U)
#define SPI_MCR_MTFE_SHIFT                       (26U)
#define SPI_MCR_MTFE(x)                          (((uint32)(((uint32)(x)) << SPI_MCR_MTFE_SHIFT)) & SPI_MCR_MTFE_MASK)
#define SPI_MCR_FRZ_MASK                         (0x8000000U)
#define SPI_MCR_FRZ_SHIFT                        (27U)
#define SPI_MCR_FRZ(x)                           (((uint32)(((uint32)(x)) << SPI_MCR_FRZ_SHIFT)) & SPI_MCR_FRZ_MASK)
#define SPI_MCR_DCONF_MASK                       (0x30000000U)
#define SPI_MCR_DCONF_SHIFT                      (28U)
#define SPI_MCR_DCONF(x)                         (((uint32)(((uint32)(x)) << SPI_MCR_DCONF_SHIFT)) & SPI_MCR_DCONF_MASK)
#define SPI_MCR_CONT_SCKE_MASK                   (0x40000000U)
#define SPI_MCR_CONT_SCKE_SHIFT                  (30U)
#define SPI_MCR_CONT_SCKE(x)                     (((uint32)(((uint32)(x)) << SPI_MCR_CONT_SCKE_SHIFT)) & SPI_MCR_CONT_SCKE_MASK)
#define SPI_MCR_MSTR_MASK                        (0x80000000U)
#define SPI_MCR_MSTR_SHIFT                       (31U)
#define SPI_MCR_MSTR(x)                          (((uint32)(((uint32)(x)) << SPI_MCR_MSTR_SHIFT)) & SPI_MCR_MSTR_MASK)

/*! @name TCR - Transfer Count Register */
#define SPI_TCR_SPI_TCNT_MASK                    (0xFFFF0000U)
#define SPI_TCR_SPI_TCNT_SHIFT                   (16U)
#define SPI_TCR_SPI_TCNT(x)                      (((uint32)(((uint32)(x)) << SPI_TCR_SPI_TCNT_SHIFT)) & SPI_TCR_SPI_TCNT_MASK)

/*! @name CTAR - Clock and Transfer Attributes Register (In Master Mode) */
#define SPI_CTAR_BR_MASK                         (0xFU)
#define SPI_CTAR_BR_SHIFT                        (0U)
#define SPI_CTAR_BR(x)                           (((uint32)(((uint32)(x)) << SPI_CTAR_BR_SHIFT)) & SPI_CTAR_BR_MASK)
#define SPI_CTAR_DT_MASK                         (0xF0U)
#define SPI_CTAR_DT_SHIFT                        (4U)
#define SPI_CTAR_DT(x)                           (((uint32)(((uint32)(x)) << SPI_CTAR_DT_SHIFT)) & SPI_CTAR_DT_MASK)
#define SPI_CTAR_ASC_MASK                        (0xF00U)
#define SPI_CTAR_ASC_SHIFT                       (8U)
#define SPI_CTAR_ASC(x)                          (((uint32)(((uint32)(x)) << SPI_CTAR_ASC_SHIFT)) & SPI_CTAR_ASC_MASK)
#define SPI_CTAR_CSSCK_MASK                      (0xF000U)
#define SPI_CTAR_CSSCK_SHIFT                     (12U)
#define SPI_CTAR_CSSCK(x)                        (((uint32)(((uint32)(x)) << SPI_CTAR_CSSCK_SHIFT)) & SPI_CTAR_CSSCK_MASK)
#define SPI_CTAR_PBR_MASK                        (0x30000U)
#define SPI_CTAR_PBR_SHIFT                       (16U)
#define SPI_CTAR_PBR(x)                          (((uint32)(((uint32)(x)) << SPI_CTAR_PBR_SHIFT)) & SPI_CTAR_PBR_MASK)
#define SPI_CTAR_PDT_MASK                        (0xC0000U)
#define SPI_CTAR_PDT_SHIFT                       (18U)
#define SPI_CTAR_PDT(x)                          (((uint32)(((uint32)(x)) << SPI_CTAR_PDT_SHIFT)) & SPI_CTAR_PDT_MASK)
#define SPI_CTAR_PASC_MASK                       (0x300000U)
#define SPI_CTAR_PASC_SHIFT                      (20U)
#define SPI_CTAR_PASC(x)                         (((uint32)(((uint32)(x)) << SPI_CTAR_PASC_SHIFT)) & SPI_CTAR_PASC_MASK)
#define SPI_CTAR_PCSSCK_MASK                     (0xC00000U)
#define SPI_CTAR_PCSSCK_SHIFT                    (22U)
#define SPI_CTAR_PCSSCK(x)                       (((uint32)(((uint32)(x)) << SPI_CTAR_PCSSCK_SHIFT)) & SPI_CTAR_PCSSCK_MASK)
#define SPI_CTAR_LSBFE_MASK                      (0x1000000U)
#define SPI_CTAR_LSBFE_SHIFT                     (24U)
#define SPI_CTAR_LSBFE(x)                        (((uint32)(((uint32)(x)) << SPI_CTAR_LSBFE_SHIFT)) & SPI_CTAR_LSBFE_MASK)
#define SPI_CTAR_CPHA_MASK                       (0x2000000U)
#define SPI_CTAR_CPHA_SHIFT                      (25U)
#define SPI_CTAR_CPHA(x)                         (((uint32)(((uint32)(x)) << SPI_CTAR_CPHA_SHIFT)) & SPI_CTAR_CPHA_MASK)
#define SPI_CTAR_CPOL_MASK                       (0x4000000U)
#define SPI_CTAR_CPOL_SHIFT                      (26U)
#define SPI_CTAR_CPOL(x)                         (((uint32)(((uint32)(x)) << SPI_CTAR_CPOL_SHIFT)) & SPI_CTAR_CPOL_MASK)
#define SPI_CTAR_FMSZ_MASK                       (0x78000000U)
#define SPI_CTAR_FMSZ_SHIFT                      (27U)
#define SPI_CTAR_FMSZ(x)                         (((uint32)(((uint32)(x)) << SPI_CTAR_FMSZ_SHIFT)) & SPI_CTAR_FMSZ_MASK)
#define SPI_CTAR_DBR_MASK                        (0x80000000U)
#define SPI_CTAR_DBR_SHIFT                       (31U)
#define SPI_CTAR_DBR(x)                          (((uint32)(((uint32)(x)) << SPI_CTAR_DBR_SHIFT)) & SPI_CTAR_DBR_MASK)

/* The count of SPI_CTAR */
#define SPI_CTAR_COUNT                           (2U)

/*! @name CTAR_SLAVE - Clock and Transfer Attributes Register (In Slave Mode) */
#define SPI_CTAR_SLAVE_CPHA_MASK                 (0x2000000U)
#define SPI_CTAR_SLAVE_CPHA_SHIFT                (25U)
#define SPI_CTAR_SLAVE_CPHA(x)                   (((uint32)(((uint32)(x)) << SPI_CTAR_SLAVE_CPHA_SHIFT)) & SPI_CTAR_SLAVE_CPHA_MASK)
#define SPI_CTAR_SLAVE_CPOL_MASK                 (0x4000000U)
#define SPI_CTAR_SLAVE_CPOL_SHIFT                (26U)
#define SPI_CTAR_SLAVE_CPOL(x)                   (((uint32)(((uint32)(x)) << SPI_CTAR_SLAVE_CPOL_SHIFT)) & SPI_CTAR_SLAVE_CPOL_MASK)
#define SPI_CTAR_SLAVE_FMSZ_MASK                 (0xF8000000U)
#define SPI_CTAR_SLAVE_FMSZ_SHIFT                (27U)
#define SPI_CTAR_SLAVE_FMSZ(x)                   (((uint32)(((uint32)(x)) << SPI_CTAR_SLAVE_FMSZ_SHIFT)) & SPI_CTAR_SLAVE_FMSZ_MASK)

/* The count of SPI_CTAR_SLAVE */
#define SPI_CTAR_SLAVE_COUNT                     (1U)

/*! @name SR - Status Register */
#define SPI_SR_POPNXTPTR_MASK                    (0xFU)
#define SPI_SR_POPNXTPTR_SHIFT                   (0U)
#define SPI_SR_POPNXTPTR(x)                      (((uint32)(((uint32)(x)) << SPI_SR_POPNXTPTR_SHIFT)) & SPI_SR_POPNXTPTR_MASK)
#define SPI_SR_RXCTR_MASK                        (0xF0U)
#define SPI_SR_RXCTR_SHIFT                       (4U)
#define SPI_SR_RXCTR(x)                          (((uint32)(((uint32)(x)) << SPI_SR_RXCTR_SHIFT)) & SPI_SR_RXCTR_MASK)
#define SPI_SR_TXNXTPTR_MASK                     (0xF00U)
#define SPI_SR_TXNXTPTR_SHIFT                    (8U)
#define SPI_SR_TXNXTPTR(x)                       (((uint32)(((uint32)(x)) << SPI_SR_TXNXTPTR_SHIFT)) & SPI_SR_TXNXTPTR_MASK)
#define SPI_SR_TXCTR_MASK                        (0xF000U)
#define SPI_SR_TXCTR_SHIFT                       (12U)
#define SPI_SR_TXCTR(x)                          (((uint32)(((uint32)(x)) << SPI_SR_TXCTR_SHIFT)) & SPI_SR_TXCTR_MASK)
#define SPI_SR_RFDF_MASK                         (0x20000U)
#define SPI_SR_RFDF_SHIFT                        (17U)
#define SPI_SR_RFDF(x)                           (((uint32)(((uint32)(x)) << SPI_SR_RFDF_SHIFT)) & SPI_SR_RFDF_MASK)
#define SPI_SR_RFOF_MASK                         (0x80000U)
#define SPI_SR_RFOF_SHIFT                        (19U)
#define SPI_SR_RFOF(x)                           (((uint32)(((uint32)(x)) << SPI_SR_RFOF_SHIFT)) & SPI_SR_RFOF_MASK)
#define SPI_SR_TFFF_MASK                         (0x2000000U)
#define SPI_SR_TFFF_SHIFT                        (25U)
#define SPI_SR_TFFF(x)                           (((uint32)(((uint32)(x)) << SPI_SR_TFFF_SHIFT)) & SPI_SR_TFFF_MASK)
#define SPI_SR_TFUF_MASK                         (0x8000000U)
#define SPI_SR_TFUF_SHIFT                        (27U)
#define SPI_SR_TFUF(x)                           (((uint32)(((uint32)(x)) << SPI_SR_TFUF_SHIFT)) & SPI_SR_TFUF_MASK)
#define SPI_SR_EOQF_MASK                         (0x10000000U)
#define SPI_SR_EOQF_SHIFT                        (28U)
#define SPI_SR_EOQF(x)                           (((uint32)(((uint32)(x)) << SPI_SR_EOQF_SHIFT)) & SPI_SR_EOQF_MASK)
#define SPI_SR_TXRXS_MASK                        (0x40000000U)
#define SPI_SR_TXRXS_SHIFT                       (30U)
#define SPI_SR_TXRXS(x)                          (((uint32)(((uint32)(x)) << SPI_SR_TXRXS_SHIFT)) & SPI_SR_TXRXS_MASK)
#define SPI_SR_TCF_MASK                          (0x80000000U)
#define SPI_SR_TCF_SHIFT                         (31U)
#define SPI_SR_TCF(x)                            (((uint32)(((uint32)(x)) << SPI_SR_TCF_SHIFT)) & SPI_SR_TCF_MASK)

/*! @name RSER - DMA/Interrupt Request Select and Enable Register */
#define SPI_RSER_RFDF_DIRS_MASK                  (0x10000U)
#define SPI_RSER_RFDF_DIRS_SHIFT                 (16U)
#define SPI_RSER_RFDF_DIRS(x)                    (((uint32)(((uint32)(x)) << SPI_RSER_RFDF_DIRS_SHIFT)) & SPI_RSER_RFDF_DIRS_MASK)
#define SPI_RSER_RFDF_RE_MASK                    (0x20000U)
#define SPI_RSER_RFDF_RE_SHIFT                   (17U)
#define SPI_RSER_RFDF_RE(x)                      (((uint32)(((uint32)(x)) << SPI_RSER_RFDF_RE_SHIFT)) & SPI_RSER_RFDF_RE_MASK)
#define SPI_RSER_RFOF_RE_MASK                    (0x80000U)
#define SPI_RSER_RFOF_RE_SHIFT                   (19U)
#define SPI_RSER_RFOF_RE(x)                      (((uint32)(((uint32)(x)) << SPI_RSER_RFOF_RE_SHIFT)) & SPI_RSER_RFOF_RE_MASK)
#define SPI_RSER_TFFF_DIRS_MASK                  (0x1000000U)
#define SPI_RSER_TFFF_DIRS_SHIFT                 (24U)
#define SPI_RSER_TFFF_DIRS(x)                    (((uint32)(((uint32)(x)) << SPI_RSER_TFFF_DIRS_SHIFT)) & SPI_RSER_TFFF_DIRS_MASK)
#define SPI_RSER_TFFF_RE_MASK                    (0x2000000U)
#define SPI_RSER_TFFF_RE_SHIFT                   (25U)
#define SPI_RSER_TFFF_RE(x)                      (((uint32)(((uint32)(x)) << SPI_RSER_TFFF_RE_SHIFT)) & SPI_RSER_TFFF_RE_MASK)
#define SPI_RSER_TFUF_RE_MASK                    (0x8000000U)
#define SPI_RSER_TFUF_RE_SHIFT                   (27U)
#define SPI_RSER_TFUF_RE(x)                      (((uint32)(((uint32)(x)) << SPI_RSER_TFUF_RE_SHIFT)) & SPI_RSER_TFUF_RE_MASK)
#define SPI_RSER_EOQF_RE_MASK                    (0x10000000U)
#define SPI_RSER_EOQF_RE_SHIFT                   (28U)
#define SPI_RSER_EOQF_RE(x)                      (((uint32)(((uint32)(x)) << SPI_RSER_EOQF_RE_SHIFT)) & SPI_RSER_EOQF_RE_MASK)
#define SPI_RSER_TCF_RE_MASK                     (0x80000000U)
#define SPI_RSER_TCF_RE_SHIFT                    (31U)
#define SPI_RSER_TCF_RE(x)                       (((uint32)(((uint32)(x)) << SPI_RSER_TCF_RE_SHIFT)) & SPI_RSER_TCF_RE_MASK)

/*! @name PUSHR - PUSH TX FIFO Register In Master Mode */
#define SPI_PUSHR_TXDATA_MASK                    (0xFFFFU)
#define SPI_PUSHR_TXDATA_SHIFT                   (0U)
#define SPI_PUSHR_TXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_PUSHR_TXDATA_SHIFT)) & SPI_PUSHR_TXDATA_MASK)
#define SPI_PUSHR_PCS_MASK                       (0x3F0000U)
#define SPI_PUSHR_PCS_SHIFT                      (16U)
#define SPI_PUSHR_PCS(x)                         (((uint32)(((uint32)(x)) << SPI_PUSHR_PCS_SHIFT)) & SPI_PUSHR_PCS_MASK)
#define SPI_PUSHR_CTCNT_MASK                     (0x4000000U)
#define SPI_PUSHR_CTCNT_SHIFT                    (26U)
#define SPI_PUSHR_CTCNT(x)                       (((uint32)(((uint32)(x)) << SPI_PUSHR_CTCNT_SHIFT)) & SPI_PUSHR_CTCNT_MASK)
#define SPI_PUSHR_EOQ_MASK                       (0x8000000U)
#define SPI_PUSHR_EOQ_SHIFT                      (27U)
#define SPI_PUSHR_EOQ(x)                         (((uint32)(((uint32)(x)) << SPI_PUSHR_EOQ_SHIFT)) & SPI_PUSHR_EOQ_MASK)
#define SPI_PUSHR_CTAS_MASK                      (0x70000000U)
#define SPI_PUSHR_CTAS_SHIFT                     (28U)
#define SPI_PUSHR_CTAS(x)                        (((uint32)(((uint32)(x)) << SPI_PUSHR_CTAS_SHIFT)) & SPI_PUSHR_CTAS_MASK)
#define SPI_PUSHR_CONT_MASK                      (0x80000000U)
#define SPI_PUSHR_CONT_SHIFT                     (31U)
#define SPI_PUSHR_CONT(x)                        (((uint32)(((uint32)(x)) << SPI_PUSHR_CONT_SHIFT)) & SPI_PUSHR_CONT_MASK)

/*! @name PUSHR_SLAVE - PUSH TX FIFO Register In Slave Mode */
#define SPI_PUSHR_SLAVE_TXDATA_MASK              (0xFFFFFFFFU)
#define SPI_PUSHR_SLAVE_TXDATA_SHIFT             (0U)
#define SPI_PUSHR_SLAVE_TXDATA(x)                (((uint32)(((uint32)(x)) << SPI_PUSHR_SLAVE_TXDATA_SHIFT)) & SPI_PUSHR_SLAVE_TXDATA_MASK)

/*! @name POPR - POP RX FIFO Register */
#define SPI_POPR_RXDATA_MASK                     (0xFFFFFFFFU)
#define SPI_POPR_RXDATA_SHIFT                    (0U)
#define SPI_POPR_RXDATA(x)                       (((uint32)(((uint32)(x)) << SPI_POPR_RXDATA_SHIFT)) & SPI_POPR_RXDATA_MASK)

/*! @name TXFR0 - Transmit FIFO Registers */
#define SPI_TXFR0_TXDATA_MASK                    (0xFFFFU)
#define SPI_TXFR0_TXDATA_SHIFT                   (0U)
#define SPI_TXFR0_TXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_TXFR0_TXDATA_SHIFT)) & SPI_TXFR0_TXDATA_MASK)
#define SPI_TXFR0_TXCMD_TXDATA_MASK              (0xFFFF0000U)
#define SPI_TXFR0_TXCMD_TXDATA_SHIFT             (16U)
#define SPI_TXFR0_TXCMD_TXDATA(x)                (((uint32)(((uint32)(x)) << SPI_TXFR0_TXCMD_TXDATA_SHIFT)) & SPI_TXFR0_TXCMD_TXDATA_MASK)

/*! @name TXFR1 - Transmit FIFO Registers */
#define SPI_TXFR1_TXDATA_MASK                    (0xFFFFU)
#define SPI_TXFR1_TXDATA_SHIFT                   (0U)
#define SPI_TXFR1_TXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_TXFR1_TXDATA_SHIFT)) & SPI_TXFR1_TXDATA_MASK)
#define SPI_TXFR1_TXCMD_TXDATA_MASK              (0xFFFF0000U)
#define SPI_TXFR1_TXCMD_TXDATA_SHIFT             (16U)
#define SPI_TXFR1_TXCMD_TXDATA(x)                (((uint32)(((uint32)(x)) << SPI_TXFR1_TXCMD_TXDATA_SHIFT)) & SPI_TXFR1_TXCMD_TXDATA_MASK)

/*! @name TXFR2 - Transmit FIFO Registers */
#define SPI_TXFR2_TXDATA_MASK                    (0xFFFFU)
#define SPI_TXFR2_TXDATA_SHIFT                   (0U)
#define SPI_TXFR2_TXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_TXFR2_TXDATA_SHIFT)) & SPI_TXFR2_TXDATA_MASK)
#define SPI_TXFR2_TXCMD_TXDATA_MASK              (0xFFFF0000U)
#define SPI_TXFR2_TXCMD_TXDATA_SHIFT             (16U)
#define SPI_TXFR2_TXCMD_TXDATA(x)                (((uint32)(((uint32)(x)) << SPI_TXFR2_TXCMD_TXDATA_SHIFT)) & SPI_TXFR2_TXCMD_TXDATA_MASK)

/*! @name TXFR3 - Transmit FIFO Registers */
#define SPI_TXFR3_TXDATA_MASK                    (0xFFFFU)
#define SPI_TXFR3_TXDATA_SHIFT                   (0U)
#define SPI_TXFR3_TXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_TXFR3_TXDATA_SHIFT)) & SPI_TXFR3_TXDATA_MASK)
#define SPI_TXFR3_TXCMD_TXDATA_MASK              (0xFFFF0000U)
#define SPI_TXFR3_TXCMD_TXDATA_SHIFT             (16U)
#define SPI_TXFR3_TXCMD_TXDATA(x)                (((uint32)(((uint32)(x)) << SPI_TXFR3_TXCMD_TXDATA_SHIFT)) & SPI_TXFR3_TXCMD_TXDATA_MASK)

/*! @name RXFR0 - Receive FIFO Registers */
#define SPI_RXFR0_RXDATA_MASK                    (0xFFFFFFFFU)
#define SPI_RXFR0_RXDATA_SHIFT                   (0U)
#define SPI_RXFR0_RXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_RXFR0_RXDATA_SHIFT)) & SPI_RXFR0_RXDATA_MASK)

/*! @name RXFR1 - Receive FIFO Registers */
#define SPI_RXFR1_RXDATA_MASK                    (0xFFFFFFFFU)
#define SPI_RXFR1_RXDATA_SHIFT                   (0U)
#define SPI_RXFR1_RXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_RXFR1_RXDATA_SHIFT)) & SPI_RXFR1_RXDATA_MASK)

/*! @name RXFR2 - Receive FIFO Registers */
#define SPI_RXFR2_RXDATA_MASK                    (0xFFFFFFFFU)
#define SPI_RXFR2_RXDATA_SHIFT                   (0U)
#define SPI_RXFR2_RXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_RXFR2_RXDATA_SHIFT)) & SPI_RXFR2_RXDATA_MASK)

/*! @name RXFR3 - Receive FIFO Registers */
#define SPI_RXFR3_RXDATA_MASK                    (0xFFFFFFFFU)
#define SPI_RXFR3_RXDATA_SHIFT                   (0U)
#define SPI_RXFR3_RXDATA(x)                      (((uint32)(((uint32)(x)) << SPI_RXFR3_RXDATA_SHIFT)) & SPI_RXFR3_RXDATA_MASK)


/*!
 * @}
 */ /* end of group SPI_Register_Masks */


/* SPI - Peripheral instance base addresses */
/** Peripheral SPI0 base address */
#define SPI0_BASE                                (0x4002C000u)
/** Peripheral SPI0 base pointer */
#define SPI_0                                     ((SPI_TYPE *)SPI0_BASE)
/** Peripheral SPI1 base address */
#define SPI1_BASE                                (0x4002D000u)
/** Peripheral SPI1 base pointer */
#define SPI_1                                     ((SPI_TYPE *)SPI1_BASE)
/** Array initializer of SPI peripheral base addresses */
#define SPI_BASE_ADDRS                           { SPI0_BASE, SPI1_BASE }
/** Array initializer of SPI peripheral base pointers */
#define SPI_BASE_PTRS                            { SPI_0, SPI_1 }
/** Interrupt vectors for the SPI peripheral type */
#define SPI_IRQS                                 { SPI0_IRQn, SPI1_IRQn }

#define DSPI0                        SPI_0
#define DSPI1                        SPI_1

/*!
 * @}
 */ /* end of group SPI_Peripheral_Access_Layer */























//#define SPI0_BASE								0x4002C000
//#define SPI1_BASE								0x4002D000
//
//#define SPI_0	                            (*(volatile SPI_BASIC_DEF *)SPI0_BASE)
//#define SPI_1	                            (*(volatile SPI_BASIC_DEF *)SPI1_BASE)

///** SPI - Peripheral register structure */
//typedef struct SPI_MemMap
//{
//  uint32 MCR;                                    // Module Configuration Register, offset: 0x0
//  uint8 RESERVED_0[4];
//  uint32 TCR;                                    // Transfer Count Register, offset: 0x8
//  union
//  {                                              // offset: 0xC
//    uint32 CTAR[2];                              // Clock and Transfer Attributes Register (In Master Mode), array offset: 0xC, array step: 0x4 */
//    uint32 CTAR_SLAVE[1];                        // Clock and Transfer Attributes Register (In Slave Mode), array offset: 0xC, array step: 0x4 */
//  };
//  uint8 RESERVED_1[24];
//  uint32 SR;                                     // Status Register, offset: 0x2C
//  uint32 RSER;                                   // DMA/Interrupt Request Select and Enable Register, offset: 0x30
//  union
//  {                                              // offset: 0x34
//    uint32 PUSHR;                                // PUSH TX FIFO Register In Master Mode, offset: 0x34
//    uint32 PUSHR_SLAVE;                          // PUSH TX FIFO Register In Slave Mode, offset: 0x34
//  };
//  uint32 POPR;                                   // POP RX FIFO Register, offset: 0x38
//  uint32 TXFR0;                                  // Transmit FIFO Registers, offset: 0x3C
//  uint32 TXFR1;                                  // Transmit FIFO Registers, offset: 0x40
//  uint32 TXFR2;                                  // Transmit FIFO Registers, offset: 0x44
//  uint32 TXFR3;                                  // Transmit FIFO Registers, offset: 0x48
//  uint8 RESERVED_2[48];
//  uint32 RXFR0;                                  // Receive FIFO Registers, offset: 0x7C
//  uint32 RXFR1;                                  // Receive FIFO Registers, offset: 0x80
//  uint32 RXFR2;                                  // Receive FIFO Registers, offset: 0x84
//  uint32 RXFR3;                                  // Receive FIFO Registers, offset: 0x88
//} volatile SPI_BASIC_DEF;
//
///* ----------------------------------------------------------------------------
//   -- SPI - Register accessor macros
//   ---------------------------------------------------------------------------- */
//
///* SPI - Register accessors */
//#define SPI_MCR_REG(base)                        ((base)->MCR)
//#define SPI_TCR_REG(base)                        ((base)->TCR)
//#define SPI_CTAR_REG(base,index2)                ((base)->CTAR[index2])
//#define SPI_CTAR_SLAVE_REG(base,index2)          ((base)->CTAR_SLAVE[index2])
//#define SPI_SR_REG(base)                         ((base)->SR)
//#define SPI_RSER_REG(base)                       ((base)->RSER)
//#define SPI_PUSHR_REG(base)                      ((base)->PUSHR)
//#define SPI_PUSHR_SLAVE_REG(base)                ((base)->PUSHR_SLAVE)
//#define SPI_POPR_REG(base)                       ((base)->POPR)
//#define SPI_TXFR0_REG(base)                      ((base)->TXFR0)
//#define SPI_TXFR1_REG(base)                      ((base)->TXFR1)
//#define SPI_TXFR2_REG(base)                      ((base)->TXFR2)
//#define SPI_TXFR3_REG(base)                      ((base)->TXFR3)
//#define SPI_RXFR0_REG(base)                      ((base)->RXFR0)
//#define SPI_RXFR1_REG(base)                      ((base)->RXFR1)
//#define SPI_RXFR2_REG(base)                      ((base)->RXFR2)
//#define SPI_RXFR3_REG(base)                      ((base)->RXFR3)
//
///* ----------------------------------------------------------------------------
//   -- SPI Register Masks
//   ---------------------------------------------------------------------------- */
//
//
///* MCR Bit Fields */
//#define SPI_MCR_HALT_MASK                        0x1u
//#define SPI_MCR_HALT_SHIFT                       0
//#define SPI_MCR_SMPL_PT_MASK                     0x300u
//#define SPI_MCR_SMPL_PT_SHIFT                    8
//#define SPI_MCR_SMPL_PT(x)                       (((uint32)(((uint32)(x))<<SPI_MCR_SMPL_PT_SHIFT))&SPI_MCR_SMPL_PT_MASK)
//#define SPI_MCR_CLR_RXF_MASK                     0x400u
//#define SPI_MCR_CLR_RXF_SHIFT                    10
//#define SPI_MCR_CLR_TXF_MASK                     0x800u
//#define SPI_MCR_CLR_TXF_SHIFT                    11
//#define SPI_MCR_DIS_RXF_MASK                     0x1000u
//#define SPI_MCR_DIS_RXF_SHIFT                    12
//#define SPI_MCR_DIS_TXF_MASK                     0x2000u
//#define SPI_MCR_DIS_TXF_SHIFT                    13
//#define SPI_MCR_MDIS_MASK                        0x4000u
//#define SPI_MCR_MDIS_SHIFT                       14
//#define SPI_MCR_DOZE_MASK                        0x8000u
//#define SPI_MCR_DOZE_SHIFT                       15
//#define SPI_MCR_PCSIS_MASK                       0x3F0000u
//#define SPI_MCR_PCSIS_SHIFT                      16
//#define SPI_MCR_PCSIS(x)                         (((uint32)(((uint32)(x))<<SPI_MCR_PCSIS_SHIFT))&SPI_MCR_PCSIS_MASK)
//#define SPI_MCR_ROOE_MASK                        0x1000000u
//#define SPI_MCR_ROOE_SHIFT                       24
//#define SPI_MCR_PCSSE_MASK                       0x2000000u
//#define SPI_MCR_PCSSE_SHIFT                      25
//#define SPI_MCR_MTFE_MASK                        0x4000000u
//#define SPI_MCR_MTFE_SHIFT                       26
//#define SPI_MCR_FRZ_MASK                         0x8000000u
//#define SPI_MCR_FRZ_SHIFT                        27
//#define SPI_MCR_DCONF_MASK                       0x30000000u
//#define SPI_MCR_DCONF_SHIFT                      28
//#define SPI_MCR_DCONF(x)                         (((uint32)(((uint32)(x))<<SPI_MCR_DCONF_SHIFT))&SPI_MCR_DCONF_MASK)
//#define SPI_MCR_CONT_SCKE_MASK                   0x40000000u
//#define SPI_MCR_CONT_SCKE_SHIFT                  30
//#define SPI_MCR_MSTR_MASK                        0x80000000u
//#define SPI_MCR_MSTR_SHIFT                       31
///* TCR Bit Fields */
//#define SPI_TCR_SPI_TCNT_MASK                    0xFFFF0000u
//#define SPI_TCR_SPI_TCNT_SHIFT                   16
//#define SPI_TCR_SPI_TCNT(x)                      (((uint32)(((uint32)(x))<<SPI_TCR_SPI_TCNT_SHIFT))&SPI_TCR_SPI_TCNT_MASK)
///* CTAR Bit Fields */
//#define SPI_CTAR_BR_MASK                         0xFu
//#define SPI_CTAR_BR_SHIFT                        0
//#define SPI_CTAR_BR(x)                           (((uint32)(((uint32)(x))<<SPI_CTAR_BR_SHIFT))&SPI_CTAR_BR_MASK)
//#define SPI_CTAR_DT_MASK                         0xF0u
//#define SPI_CTAR_DT_SHIFT                        4
//#define SPI_CTAR_DT(x)                           (((uint32)(((uint32)(x))<<SPI_CTAR_DT_SHIFT))&SPI_CTAR_DT_MASK)
//#define SPI_CTAR_ASC_MASK                        0xF00u
//#define SPI_CTAR_ASC_SHIFT                       8
//#define SPI_CTAR_ASC(x)                          (((uint32)(((uint32)(x))<<SPI_CTAR_ASC_SHIFT))&SPI_CTAR_ASC_MASK)
//#define SPI_CTAR_CSSCK_MASK                      0xF000u
//#define SPI_CTAR_CSSCK_SHIFT                     12
//#define SPI_CTAR_CSSCK(x)                        (((uint32)(((uint32)(x))<<SPI_CTAR_CSSCK_SHIFT))&SPI_CTAR_CSSCK_MASK)
//#define SPI_CTAR_PBR_MASK                        0x30000u
//#define SPI_CTAR_PBR_SHIFT                       16
//#define SPI_CTAR_PBR(x)                          (((uint32)(((uint32)(x))<<SPI_CTAR_PBR_SHIFT))&SPI_CTAR_PBR_MASK)
//#define SPI_CTAR_PDT_MASK                        0xC0000u
//#define SPI_CTAR_PDT_SHIFT                       18
//#define SPI_CTAR_PDT(x)                          (((uint32)(((uint32)(x))<<SPI_CTAR_PDT_SHIFT))&SPI_CTAR_PDT_MASK)
//#define SPI_CTAR_PASC_MASK                       0x300000u
//#define SPI_CTAR_PASC_SHIFT                      20
//#define SPI_CTAR_PASC(x)                         (((uint32)(((uint32)(x))<<SPI_CTAR_PASC_SHIFT))&SPI_CTAR_PASC_MASK)
//#define SPI_CTAR_PCSSCK_MASK                     0xC00000u
//#define SPI_CTAR_PCSSCK_SHIFT                    22
//#define SPI_CTAR_PCSSCK(x)                       (((uint32)(((uint32)(x))<<SPI_CTAR_PCSSCK_SHIFT))&SPI_CTAR_PCSSCK_MASK)
//#define SPI_CTAR_LSBFE_MASK                      0x1000000u
//#define SPI_CTAR_LSBFE_SHIFT                     24
//#define SPI_CTAR_CPHA_MASK                       0x2000000u
//#define SPI_CTAR_CPHA_SHIFT                      25
//#define SPI_CTAR_CPOL_MASK                       0x4000000u
//#define SPI_CTAR_CPOL_SHIFT                      26
//#define SPI_CTAR_FMSZ_MASK                       0x78000000u
//#define SPI_CTAR_FMSZ_SHIFT                      27
//#define SPI_CTAR_FMSZ(x)                         (((uint32)(((uint32)(x))<<SPI_CTAR_FMSZ_SHIFT))&SPI_CTAR_FMSZ_MASK)
//#define SPI_CTAR_DBR_MASK                        0x80000000u
//#define SPI_CTAR_DBR_SHIFT                       31
///* CTAR_SLAVE Bit Fields */
//#define SPI_CTAR_SLAVE_CPHA_MASK                 0x2000000u
//#define SPI_CTAR_SLAVE_CPHA_SHIFT                25
//#define SPI_CTAR_SLAVE_CPOL_MASK                 0x4000000u
//#define SPI_CTAR_SLAVE_CPOL_SHIFT                26
//#define SPI_CTAR_SLAVE_FMSZ_MASK                 0xF8000000u
//#define SPI_CTAR_SLAVE_FMSZ_SHIFT                27
//#define SPI_CTAR_SLAVE_FMSZ(x)                   (((uint32)(((uint32)(x))<<SPI_CTAR_SLAVE_FMSZ_SHIFT))&SPI_CTAR_SLAVE_FMSZ_MASK)
///* SR Bit Fields */
//#define SPI_SR_POPNXTPTR_MASK                    0xFu
//#define SPI_SR_POPNXTPTR_SHIFT                   0
//#define SPI_SR_POPNXTPTR(x)                      (((uint32)(((uint32)(x))<<SPI_SR_POPNXTPTR_SHIFT))&SPI_SR_POPNXTPTR_MASK)
//#define SPI_SR_RXCTR_MASK                        0xF0u
//#define SPI_SR_RXCTR_SHIFT                       4
//#define SPI_SR_RXCTR(x)                          (((uint32)(((uint32)(x))<<SPI_SR_RXCTR_SHIFT))&SPI_SR_RXCTR_MASK)
//#define SPI_SR_TXNXTPTR_MASK                     0xF00u
//#define SPI_SR_TXNXTPTR_SHIFT                    8
//#define SPI_SR_TXNXTPTR(x)                       (((uint32)(((uint32)(x))<<SPI_SR_TXNXTPTR_SHIFT))&SPI_SR_TXNXTPTR_MASK)
//#define SPI_SR_TXCTR_MASK                        0xF000u
//#define SPI_SR_TXCTR_SHIFT                       12
//#define SPI_SR_TXCTR(x)                          (((uint32)(((uint32)(x))<<SPI_SR_TXCTR_SHIFT))&SPI_SR_TXCTR_MASK)
//#define SPI_SR_RFDF_MASK                         0x20000u
//#define SPI_SR_RFDF_SHIFT                        17
//#define SPI_SR_RFOF_MASK                         0x80000u
//#define SPI_SR_RFOF_SHIFT                        19
//#define SPI_SR_TFFF_MASK                         0x2000000u
//#define SPI_SR_TFFF_SHIFT                        25
//#define SPI_SR_TFUF_MASK                         0x8000000u
//#define SPI_SR_TFUF_SHIFT                        27
//#define SPI_SR_EOQF_MASK                         0x10000000u
//#define SPI_SR_EOQF_SHIFT                        28
//#define SPI_SR_TXRXS_MASK                        0x40000000u
//#define SPI_SR_TXRXS_SHIFT                       30
//#define SPI_SR_TCF_MASK                          0x80000000u
//#define SPI_SR_TCF_SHIFT                         31
///* RSER Bit Fields */
//#define SPI_RSER_RFDF_DIRS_MASK                  0x10000u
//#define SPI_RSER_RFDF_DIRS_SHIFT                 16
//#define SPI_RSER_RFDF_RE_MASK                    0x20000u
//#define SPI_RSER_RFDF_RE_SHIFT                   17
//#define SPI_RSER_RFOF_RE_MASK                    0x80000u
//#define SPI_RSER_RFOF_RE_SHIFT                   19
//#define SPI_RSER_TFFF_DIRS_MASK                  0x1000000u
//#define SPI_RSER_TFFF_DIRS_SHIFT                 24
//#define SPI_RSER_TFFF_RE_MASK                    0x2000000u
//#define SPI_RSER_TFFF_RE_SHIFT                   25
//#define SPI_RSER_TFUF_RE_MASK                    0x8000000u
//#define SPI_RSER_TFUF_RE_SHIFT                   27
//#define SPI_RSER_EOQF_RE_MASK                    0x10000000u
//#define SPI_RSER_EOQF_RE_SHIFT                   28
//#define SPI_RSER_TCF_RE_MASK                     0x80000000u
//#define SPI_RSER_TCF_RE_SHIFT                    31
///* PUSHR Bit Fields */
//#define SPI_PUSHR_TXDATA_MASK                    0xFFFFu
//#define SPI_PUSHR_TXDATA_SHIFT                   0
//#define SPI_PUSHR_TXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_PUSHR_TXDATA_SHIFT))&SPI_PUSHR_TXDATA_MASK)
//#define SPI_PUSHR_PCS_MASK                       0x3F0000u
//#define SPI_PUSHR_PCS_SHIFT                      16
//#define SPI_PUSHR_PCS(x)                         (((uint32)(((uint32)(x))<<SPI_PUSHR_PCS_SHIFT))&SPI_PUSHR_PCS_MASK)
//#define SPI_PUSHR_CTCNT_MASK                     0x4000000u
//#define SPI_PUSHR_CTCNT_SHIFT                    26
//#define SPI_PUSHR_EOQ_MASK                       0x8000000u
//#define SPI_PUSHR_EOQ_SHIFT                      27
//#define SPI_PUSHR_CTAS_MASK                      0x70000000u
//#define SPI_PUSHR_CTAS_SHIFT                     28
//#define SPI_PUSHR_CTAS(x)                        (((uint32)(((uint32)(x))<<SPI_PUSHR_CTAS_SHIFT))&SPI_PUSHR_CTAS_MASK)
//#define SPI_PUSHR_CONT_MASK                      0x80000000u
//#define SPI_PUSHR_CONT_SHIFT                     31
///* PUSHR_SLAVE Bit Fields */
//#define SPI_PUSHR_SLAVE_TXDATA_MASK              0xFFFFFFFFu
//#define SPI_PUSHR_SLAVE_TXDATA_SHIFT             0
//#define SPI_PUSHR_SLAVE_TXDATA(x)                (((uint32)(((uint32)(x))<<SPI_PUSHR_SLAVE_TXDATA_SHIFT))&SPI_PUSHR_SLAVE_TXDATA_MASK)
///* POPR Bit Fields */
//#define SPI_POPR_RXDATA_MASK                     0xFFFFFFFFu
//#define SPI_POPR_RXDATA_SHIFT                    0
//#define SPI_POPR_RXDATA(x)                       (((uint32)(((uint32)(x))<<SPI_POPR_RXDATA_SHIFT))&SPI_POPR_RXDATA_MASK)
///* TXFR0 Bit Fields */
//#define SPI_TXFR0_TXDATA_MASK                    0xFFFFu
//#define SPI_TXFR0_TXDATA_SHIFT                   0
//#define SPI_TXFR0_TXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_TXFR0_TXDATA_SHIFT))&SPI_TXFR0_TXDATA_MASK)
//#define SPI_TXFR0_TXCMD_TXDATA_MASK              0xFFFF0000u
//#define SPI_TXFR0_TXCMD_TXDATA_SHIFT             16
//#define SPI_TXFR0_TXCMD_TXDATA(x)                (((uint32)(((uint32)(x))<<SPI_TXFR0_TXCMD_TXDATA_SHIFT))&SPI_TXFR0_TXCMD_TXDATA_MASK)
///* TXFR1 Bit Fields */
//#define SPI_TXFR1_TXDATA_MASK                    0xFFFFu
//#define SPI_TXFR1_TXDATA_SHIFT                   0
//#define SPI_TXFR1_TXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_TXFR1_TXDATA_SHIFT))&SPI_TXFR1_TXDATA_MASK)
//#define SPI_TXFR1_TXCMD_TXDATA_MASK              0xFFFF0000u
//#define SPI_TXFR1_TXCMD_TXDATA_SHIFT             16
//#define SPI_TXFR1_TXCMD_TXDATA(x)                (((uint32)(((uint32)(x))<<SPI_TXFR1_TXCMD_TXDATA_SHIFT))&SPI_TXFR1_TXCMD_TXDATA_MASK)
///* TXFR2 Bit Fields */
//#define SPI_TXFR2_TXDATA_MASK                    0xFFFFu
//#define SPI_TXFR2_TXDATA_SHIFT                   0
//#define SPI_TXFR2_TXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_TXFR2_TXDATA_SHIFT))&SPI_TXFR2_TXDATA_MASK)
//#define SPI_TXFR2_TXCMD_TXDATA_MASK              0xFFFF0000u
//#define SPI_TXFR2_TXCMD_TXDATA_SHIFT             16
//#define SPI_TXFR2_TXCMD_TXDATA(x)                (((uint32)(((uint32)(x))<<SPI_TXFR2_TXCMD_TXDATA_SHIFT))&SPI_TXFR2_TXCMD_TXDATA_MASK)
///* TXFR3 Bit Fields */
//#define SPI_TXFR3_TXDATA_MASK                    0xFFFFu
//#define SPI_TXFR3_TXDATA_SHIFT                   0
//#define SPI_TXFR3_TXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_TXFR3_TXDATA_SHIFT))&SPI_TXFR3_TXDATA_MASK)
//#define SPI_TXFR3_TXCMD_TXDATA_MASK              0xFFFF0000u
//#define SPI_TXFR3_TXCMD_TXDATA_SHIFT             16
//#define SPI_TXFR3_TXCMD_TXDATA(x)                (((uint32)(((uint32)(x))<<SPI_TXFR3_TXCMD_TXDATA_SHIFT))&SPI_TXFR3_TXCMD_TXDATA_MASK)
///* RXFR0 Bit Fields */
//#define SPI_RXFR0_RXDATA_MASK                    0xFFFFFFFFu
//#define SPI_RXFR0_RXDATA_SHIFT                   0
//#define SPI_RXFR0_RXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_RXFR0_RXDATA_SHIFT))&SPI_RXFR0_RXDATA_MASK)
///* RXFR1 Bit Fields */
//#define SPI_RXFR1_RXDATA_MASK                    0xFFFFFFFFu
//#define SPI_RXFR1_RXDATA_SHIFT                   0
//#define SPI_RXFR1_RXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_RXFR1_RXDATA_SHIFT))&SPI_RXFR1_RXDATA_MASK)
///* RXFR2 Bit Fields */
//#define SPI_RXFR2_RXDATA_MASK                    0xFFFFFFFFu
//#define SPI_RXFR2_RXDATA_SHIFT                   0
//#define SPI_RXFR2_RXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_RXFR2_RXDATA_SHIFT))&SPI_RXFR2_RXDATA_MASK)
///* RXFR3 Bit Fields */
//#define SPI_RXFR3_RXDATA_MASK                    0xFFFFFFFFu
//#define SPI_RXFR3_RXDATA_SHIFT                   0
//#define SPI_RXFR3_RXDATA(x)                      (((uint32)(((uint32)(x))<<SPI_RXFR3_RXDATA_SHIFT))&SPI_RXFR3_RXDATA_MASK)
//
///* SPI - Peripheral instance base addresses */
///** Peripheral SPI0 base pointer */
//#define SPI0_BASE_PTR                            ((SPI_MemMapPtr)0x4002C000u)
///** Peripheral SPI1 base pointer */
//#define SPI1_BASE_PTR                            ((SPI_MemMapPtr)0x4002D000u)
///** Array initializer of SPI peripheral base pointers */
//#define SPI_BASE_PTRS                            { SPI0_BASE_PTR, SPI1_BASE_PTR }
//
///* ----------------------------------------------------------------------------
//   -- SPI - Register accessor macros
//   ---------------------------------------------------------------------------- */
//
///* SPI - Register instance definitions */
///* SPI0 */
//#define SPI0_MCR                                 SPI_MCR_REG(SPI0_BASE_PTR)
//#define SPI0_TCR                                 SPI_TCR_REG(SPI0_BASE_PTR)
//#define SPI0_CTAR0                               SPI_CTAR_REG(SPI0_BASE_PTR,0)
//#define SPI0_CTAR0_SLAVE                         SPI_CTAR_SLAVE_REG(SPI0_BASE_PTR,0)
//#define SPI0_CTAR1                               SPI_CTAR_REG(SPI0_BASE_PTR,1)
//#define SPI0_SR                                  SPI_SR_REG(SPI0_BASE_PTR)
//#define SPI0_RSER                                SPI_RSER_REG(SPI0_BASE_PTR)
//#define SPI0_PUSHR                               SPI_PUSHR_REG(SPI0_BASE_PTR)
//#define SPI0_PUSHR_SLAVE                         SPI_PUSHR_SLAVE_REG(SPI0_BASE_PTR)
//#define SPI0_POPR                                SPI_POPR_REG(SPI0_BASE_PTR)
//#define SPI0_TXFR0                               SPI_TXFR0_REG(SPI0_BASE_PTR)
//#define SPI0_TXFR1                               SPI_TXFR1_REG(SPI0_BASE_PTR)
//#define SPI0_TXFR2                               SPI_TXFR2_REG(SPI0_BASE_PTR)
//#define SPI0_TXFR3                               SPI_TXFR3_REG(SPI0_BASE_PTR)
//#define SPI0_RXFR0                               SPI_RXFR0_REG(SPI0_BASE_PTR)
//#define SPI0_RXFR1                               SPI_RXFR1_REG(SPI0_BASE_PTR)
//#define SPI0_RXFR2                               SPI_RXFR2_REG(SPI0_BASE_PTR)
//#define SPI0_RXFR3                               SPI_RXFR3_REG(SPI0_BASE_PTR)
///* SPI1 */
//#define SPI1_MCR                                 SPI_MCR_REG(SPI1_BASE_PTR)
//#define SPI1_TCR                                 SPI_TCR_REG(SPI1_BASE_PTR)
//#define SPI1_CTAR0                               SPI_CTAR_REG(SPI1_BASE_PTR,0)
//#define SPI1_CTAR0_SLAVE                         SPI_CTAR_SLAVE_REG(SPI1_BASE_PTR,0)
//#define SPI1_CTAR1                               SPI_CTAR_REG(SPI1_BASE_PTR,1)
//#define SPI1_SR                                  SPI_SR_REG(SPI1_BASE_PTR)
//#define SPI1_RSER                                SPI_RSER_REG(SPI1_BASE_PTR)
//#define SPI1_PUSHR                               SPI_PUSHR_REG(SPI1_BASE_PTR)
//#define SPI1_PUSHR_SLAVE                         SPI_PUSHR_SLAVE_REG(SPI1_BASE_PTR)
//#define SPI1_POPR                                SPI_POPR_REG(SPI1_BASE_PTR)
//#define SPI1_TXFR0                               SPI_TXFR0_REG(SPI1_BASE_PTR)
//#define SPI1_TXFR1                               SPI_TXFR1_REG(SPI1_BASE_PTR)
//#define SPI1_TXFR2                               SPI_TXFR2_REG(SPI1_BASE_PTR)
//#define SPI1_TXFR3                               SPI_TXFR3_REG(SPI1_BASE_PTR)
//#define SPI1_RXFR0                               SPI_RXFR0_REG(SPI1_BASE_PTR)
//#define SPI1_RXFR1                               SPI_RXFR1_REG(SPI1_BASE_PTR)
//#define SPI1_RXFR2                               SPI_RXFR2_REG(SPI1_BASE_PTR)
//#define SPI1_RXFR3                               SPI_RXFR3_REG(SPI1_BASE_PTR)
//
///* SPI - Register array accessors */
//#define SPI0_CTAR(index2)                        SPI_CTAR_REG(SPI0_BASE_PTR,index2)
//#define SPI1_CTAR(index2)                        SPI_CTAR_REG(SPI1_BASE_PTR,index2)
//#define SPI0_CTAR_SLAVE(index2)                  SPI_CTAR_SLAVE_REG(SPI0_BASE_PTR,index2)
//#define SPI1_CTAR_SLAVE(index2)                  SPI_CTAR_SLAVE_REG(SPI1_BASE_PTR,index2)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief I2C registers
 */

#define I2C0_BASE								 0x40066000
#define I2C1_BASE								 0x40067000

typedef struct
{
	union
	{
		uint8 BYTE;
		struct
		{
			uint8 Reserved  :1;
			uint8 AD	    :7;

		}BIT;

	}I2C_A ;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 ICR	:6;							//Clock Rate
			uint8 MULT	:2;							//Multiple Factor
		}BIT;
	}I2C_F;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 Reserved	:1;
			uint8 WUEN		:1;						//Wakeup Enable
			uint8 RSTA		:1;						//Repeat START
			uint8 TXAK		:1;						//Transmit Acknowledge Enable
			uint8 TX		:1;						//Transmit Mode Select
			uint8 MST		:1;						//Master Mode Select
			uint8 IICIE		:1;						//I2C Interrupt Enable
			uint8 IICEN		:1;						//I2C Enable
		}BIT;							//I2C Control Register 1
	}I2C_C1;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 RXAK		:1;						//Receive Acknowledge
			uint8 IICIF		:1;						//Interrupt Flag
			uint8 SRW		:1;						//Slave Read/Write
			uint8 RAM		:1;						//Range Address Match
			uint8 ARBL		:1;						//Arbitration Lost
			uint8 BUSY		:1;						//Bus Busy
			uint8 IAAS		:1;						//Addressed As a Slave
			uint8 TCF		:1;						//Transmit Complete Flag
		}BIT;
	}I2C_S1;

	uint8 I2C_D;							//I2C Data I/O Register

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 AD		:3;						//Slave Address
			uint8 RMEN		:1;						//Range Address Matching Enable
			uint8 SBRC		:1;						//Slave Baud Rate Control
			uint8 HDRS		:1;						//High drive select
			uint8 ADEXT		:1;						//Address Extension
			uint8 GCAEN		:1;						//General Call Address Enable
		}BIT;
	}I2C_C2;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 FLT		:4;
			uint8 STARTF	:1;
			uint8 SSIE		:1;
			uint8 STOPF		:1;
			uint8 SHEN		:1;
		}BIT;
	}I2C_FLT;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 Reserved 	:1;
			uint8 RAD		:7;						//Range Slave Address

		}BIT;
	}I2C_RA;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 SHTF2IE	:1;
			uint8 SHTF2		:1;
			uint8 SHTF1		:1;
			uint8 SLTF		:1;
			uint8 TCKSEL	:1;						//
			uint8 SIICAEN	:1;						//Second I2C Address Enable
			uint8 ALERTEN	:1;						//SMBus Alert Response Address Enable
			uint8 FACK		:1;						//Fast NACK/ACKEnable

		}BIT;
	}I2C_SMB;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 Reserved  :1;
			uint8 SAD		:7; 					//SMB Bus Address

		}BIT;
	}I2C_A2;

	uint8 SLTH;
	uint8 SLTL;

}I2C_BASIC_DEF;



#define I2C_0	                            (*(volatile I2C_BASIC_DEF *)I2C0_BASE)
#define I2C_1	                            (*(volatile I2C_BASIC_DEF *)I2C1_BASE)

/* ----------------------------------------------------------------------------
   -- I2C Register Masks
   ---------------------------------------------------------------------------- */

/* A1 Bit Fields */
#define I2C_A1_AD_MASK                           0xFEu
#define I2C_A1_AD_SHIFT                          1
#define I2C_A1_AD(x)                             (((uint8)(((uint8)(x))<<I2C_A1_AD_SHIFT))&I2C_A1_AD_MASK)
/* F Bit Fields */
#define I2C_F_ICR_MASK                           0x3Fu
#define I2C_F_ICR_SHIFT                          0
#define I2C_F_ICR(x)                             (((uint8)(((uint8)(x))<<I2C_F_ICR_SHIFT))&I2C_F_ICR_MASK)
#define I2C_F_MULT_MASK                          0xC0u
#define I2C_F_MULT_SHIFT                         6
#define I2C_F_MULT(x)                            (((uint8)(((uint8)(x))<<I2C_F_MULT_SHIFT))&I2C_F_MULT_MASK)
/* C1 Bit Fields */
#define I2C_C1_WUEN_MASK                         0x2u
#define I2C_C1_WUEN_SHIFT                        1
#define I2C_C1_RSTA_MASK                         0x4u
#define I2C_C1_RSTA_SHIFT                        2
#define I2C_C1_TXAK_MASK                         0x8u
#define I2C_C1_TXAK_SHIFT                        3
#define I2C_C1_TX_MASK                           0x10u
#define I2C_C1_TX_SHIFT                          4
#define I2C_C1_MST_MASK                          0x20u
#define I2C_C1_MST_SHIFT                         5
#define I2C_C1_IICIE_MASK                        0x40u
#define I2C_C1_IICIE_SHIFT                       6
#define I2C_C1_IICEN_MASK                        0x80u
#define I2C_C1_IICEN_SHIFT                       7
/* S Bit Fields */
#define I2C_S_RXAK_MASK                          0x1u
#define I2C_S_RXAK_SHIFT                         0
#define I2C_S_IICIF_MASK                         0x2u
#define I2C_S_IICIF_SHIFT                        1
#define I2C_S_SRW_MASK                           0x4u
#define I2C_S_SRW_SHIFT                          2
#define I2C_S_RAM_MASK                           0x8u
#define I2C_S_RAM_SHIFT                          3
#define I2C_S_ARBL_MASK                          0x10u
#define I2C_S_ARBL_SHIFT                         4
#define I2C_S_BUSY_MASK                          0x20u
#define I2C_S_BUSY_SHIFT                         5
#define I2C_S_IAAS_MASK                          0x40u
#define I2C_S_IAAS_SHIFT                         6
#define I2C_S_TCF_MASK                           0x80u
#define I2C_S_TCF_SHIFT                          7
/* D Bit Fields */
#define I2C_D_DATA_MASK                          0xFFu
#define I2C_D_DATA_SHIFT                         0
#define I2C_D_DATA(x)                            (((uint8)(((uint8)(x))<<I2C_D_DATA_SHIFT))&I2C_D_DATA_MASK)
/* C2 Bit Fields */
#define I2C_C2_AD_MASK                           0x7u
#define I2C_C2_AD_SHIFT                          0
#define I2C_C2_AD(x)                             (((uint8)(((uint8)(x))<<I2C_C2_AD_SHIFT))&I2C_C2_AD_MASK)
#define I2C_C2_RMEN_MASK                         0x8u
#define I2C_C2_RMEN_SHIFT                        3
#define I2C_C2_SBRC_MASK                         0x10u
#define I2C_C2_SBRC_SHIFT                        4
#define I2C_C2_ADEXT_MASK                        0x40u
#define I2C_C2_ADEXT_SHIFT                       6
#define I2C_C2_GCAEN_MASK                        0x80u
#define I2C_C2_GCAEN_SHIFT                       7
/* FLT Bit Fields */
#define I2C_FLT_FLT_MASK                         0xFu
#define I2C_FLT_FLT_SHIFT                        0
#define I2C_FLT_FLT(x)                           (((uint8)(((uint8)(x))<<I2C_FLT_FLT_SHIFT))&I2C_FLT_FLT_MASK)
#define I2C_FLT_STARTF_MASK                      0x10u
#define I2C_FLT_STARTF_SHIFT                     4
#define I2C_FLT_SSIE_MASK                        0x20u
#define I2C_FLT_SSIE_SHIFT                       5
#define I2C_FLT_STOPF_MASK                       0x40u
#define I2C_FLT_STOPF_SHIFT                      6
#define I2C_FLT_SHEN_MASK                        0x80u
#define I2C_FLT_SHEN_SHIFT                       7
/* RA Bit Fields */
#define I2C_RA_RAD_MASK                          0xFEu
#define I2C_RA_RAD_SHIFT                         1
#define I2C_RA_RAD(x)                            (((uint8)(((uint8)(x))<<I2C_RA_RAD_SHIFT))&I2C_RA_RAD_MASK)
/* SMB Bit Fields */
#define I2C_SMB_SHTF2IE_MASK                     0x1u
#define I2C_SMB_SHTF2IE_SHIFT                    0
#define I2C_SMB_SHTF2_MASK                       0x2u
#define I2C_SMB_SHTF2_SHIFT                      1
#define I2C_SMB_SHTF1_MASK                       0x4u
#define I2C_SMB_SHTF1_SHIFT                      2
#define I2C_SMB_SLTF_MASK                        0x8u
#define I2C_SMB_SLTF_SHIFT                       3
#define I2C_SMB_TCKSEL_MASK                      0x10u
#define I2C_SMB_TCKSEL_SHIFT                     4
#define I2C_SMB_SIICAEN_MASK                     0x20u
#define I2C_SMB_SIICAEN_SHIFT                    5
#define I2C_SMB_ALERTEN_MASK                     0x40u
#define I2C_SMB_ALERTEN_SHIFT                    6
#define I2C_SMB_FACK_MASK                        0x80u
#define I2C_SMB_FACK_SHIFT                       7
/* A2 Bit Fields */
#define I2C_A2_SAD_MASK                          0xFEu
#define I2C_A2_SAD_SHIFT                         1
#define I2C_A2_SAD(x)                            (((uint8)(((uint8)(x))<<I2C_A2_SAD_SHIFT))&I2C_A2_SAD_MASK)
/* SLTH Bit Fields */
#define I2C_SLTH_SSLT_MASK                       0xFFu
#define I2C_SLTH_SSLT_SHIFT                      0
#define I2C_SLTH_SSLT(x)                         (((uint8)(((uint8)(x))<<I2C_SLTH_SSLT_SHIFT))&I2C_SLTH_SSLT_MASK)
/* SLTL Bit Fields */
#define I2C_SLTL_SSLT_MASK                       0xFFu
#define I2C_SLTL_SSLT_SHIFT                      0
#define I2C_SLTL_SSLT(x)                         (((uint8)(((uint8)(x))<<I2C_SLTL_SSLT_SHIFT))&I2C_SLTL_SSLT_MASK)

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Flash registers
 */
/* ----------------------------------------------------------------------------
   -- FTFA
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTFA_Peripheral FTFA
 * @{
 */

#define FTFA_BASE								0x40020000

#define FTFA	                            	(*(volatile FLASH_BASIC_DEF *)FTFA_BASE)

/** FTFA - Peripheral register structure */
typedef struct
{
  uint8 FSTAT;                                   // Flash Status Register, offset: 0x0 */
  uint8 FCNFG;                                   // Flash Configuration Register, offset: 0x1 */
  uint8 FSEC;                                    // Flash Security Register, offset: 0x2 */
  uint8 FOPT;                                    // Flash Option Register, offset: 0x3 */
  uint8 FCCOB3;                                  // Flash Common Command Object Registers, offset: 0x4
  uint8 FCCOB2;                                  // Flash Common Command Object Registers, offset: 0x5
  uint8 FCCOB1;                                  // Flash Common Command Object Registers, offset: 0x6
  uint8 FCCOB0;                                  // Flash Common Command Object Registers, offset: 0x7
  uint8 FCCOB7;                                  // Flash Common Command Object Registers, offset: 0x8
  uint8 FCCOB6;                                  // Flash Common Command Object Registers, offset: 0x9
  uint8 FCCOB5;                                  // Flash Common Command Object Registers, offset: 0xA
  uint8 FCCOB4;                                  // Flash Common Command Object Registers, offset: 0xB
  uint8 FCCOBB;                                  // Flash Common Command Object Registers, offset: 0xC
  uint8 FCCOBA;                                  // Flash Common Command Object Registers, offset: 0xD
  uint8 FCCOB9;                                  // Flash Common Command Object Registers, offset: 0xE
  uint8 FCCOB8;                                  // Flash Common Command Object Registers, offset: 0xF
  uint8 FPROT3;                                  // Program Flash Protection Registers, offset: 0x10
  uint8 FPROT2;                                  // Program Flash Protection Registers, offset: 0x11
  uint8 FPROT1;                                  // Program Flash Protection Registers, offset: 0x12
  uint8 FPROT0;                                  // Program Flash Protection Registers, offset: 0x13
  uint8 RESERVED_0[4];
  uint8 XACCH3;                                  // Execute-only Access Registers, offset: 0x18
  uint8 XACCH2;                                  // Execute-only Access Registers, offset: 0x19
  uint8 XACCH1;                                  // Execute-only Access Registers, offset: 0x1A
  uint8 XACCH0;                                  // Execute-only Access Registers, offset: 0x1B
  uint8 XACCL3;                                  // Execute-only Access Registers, offset: 0x1C
  uint8 XACCL2;                                  // Execute-only Access Registers, offset: 0x1D
  uint8 XACCL1;                                  // Execute-only Access Registers, offset: 0x1E
  uint8 XACCL0;                                  // Execute-only Access Registers, offset: 0x1F
  uint8 SACCH3;                                  // Supervisor-only Access Registers, offset: 0x20
  uint8 SACCH2;                                  // Supervisor-only Access Registers, offset: 0x21
  uint8 SACCH1;                                  // Supervisor-only Access Registers, offset: 0x22
  uint8 SACCH0;                                  // Supervisor-only Access Registers, offset: 0x23
  uint8 SACCL3;                                  // Supervisor-only Access Registers, offset: 0x24
  uint8 SACCL2;                                  // Supervisor-only Access Registers, offset: 0x25
  uint8 SACCL1;                                  // Supervisor-only Access Registers, offset: 0x26
  uint8 SACCL0;                                  // Supervisor-only Access Registers, offset: 0x27
  uint8 FACSS;                                   // Flash Access Segment Size Register, offset: 0x28
  uint8 RESERVED_1[2];
  uint8 FACSN;                                   // Flash Access Segment Number Register, offset: 0x2B
} volatile FLASH_BASIC_DEF;

/* ----------------------------------------------------------------------------
   -- FTFA - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTFA_Register_Accessor_Macros FTFA - Register accessor macros
 * @{
 */


/* FTFA - Register accessors */
#define FTFA_FSTAT_REG(base)                     ((base)->FSTAT)
#define FTFA_FCNFG_REG(base)                     ((base)->FCNFG)
#define FTFA_FSEC_REG(base)                      ((base)->FSEC)
#define FTFA_FOPT_REG(base)                      ((base)->FOPT)
#define FTFA_FCCOB3_REG(base)                    ((base)->FCCOB3)
#define FTFA_FCCOB2_REG(base)                    ((base)->FCCOB2)
#define FTFA_FCCOB1_REG(base)                    ((base)->FCCOB1)
#define FTFA_FCCOB0_REG(base)                    ((base)->FCCOB0)
#define FTFA_FCCOB7_REG(base)                    ((base)->FCCOB7)
#define FTFA_FCCOB6_REG(base)                    ((base)->FCCOB6)
#define FTFA_FCCOB5_REG(base)                    ((base)->FCCOB5)
#define FTFA_FCCOB4_REG(base)                    ((base)->FCCOB4)
#define FTFA_FCCOBB_REG(base)                    ((base)->FCCOBB)
#define FTFA_FCCOBA_REG(base)                    ((base)->FCCOBA)
#define FTFA_FCCOB9_REG(base)                    ((base)->FCCOB9)
#define FTFA_FCCOB8_REG(base)                    ((base)->FCCOB8)
#define FTFA_FPROT3_REG(base)                    ((base)->FPROT3)
#define FTFA_FPROT2_REG(base)                    ((base)->FPROT2)
#define FTFA_FPROT1_REG(base)                    ((base)->FPROT1)
#define FTFA_FPROT0_REG(base)                    ((base)->FPROT0)
#define FTFA_XACCH3_REG(base)                    ((base)->XACCH3)
#define FTFA_XACCH2_REG(base)                    ((base)->XACCH2)
#define FTFA_XACCH1_REG(base)                    ((base)->XACCH1)
#define FTFA_XACCH0_REG(base)                    ((base)->XACCH0)
#define FTFA_XACCL3_REG(base)                    ((base)->XACCL3)
#define FTFA_XACCL2_REG(base)                    ((base)->XACCL2)
#define FTFA_XACCL1_REG(base)                    ((base)->XACCL1)
#define FTFA_XACCL0_REG(base)                    ((base)->XACCL0)
#define FTFA_SACCH3_REG(base)                    ((base)->SACCH3)
#define FTFA_SACCH2_REG(base)                    ((base)->SACCH2)
#define FTFA_SACCH1_REG(base)                    ((base)->SACCH1)
#define FTFA_SACCH0_REG(base)                    ((base)->SACCH0)
#define FTFA_SACCL3_REG(base)                    ((base)->SACCL3)
#define FTFA_SACCL2_REG(base)                    ((base)->SACCL2)
#define FTFA_SACCL1_REG(base)                    ((base)->SACCL1)
#define FTFA_SACCL0_REG(base)                    ((base)->SACCL0)
#define FTFA_FACSS_REG(base)                     ((base)->FACSS)
#define FTFA_FACSN_REG(base)                     ((base)->FACSN)

/*!
 * @}
 */ /* end of group FTFA_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- FTFA Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FTFA_Register_Masks FTFA Register Masks
 * @{
 */

/* FSTAT Bit Fields */
#define FTFA_FSTAT_MGSTAT0_MASK                  0x1u
#define FTFA_FSTAT_MGSTAT0_SHIFT                 0
#define FTFA_FSTAT_FPVIOL_MASK                   0x10u
#define FTFA_FSTAT_FPVIOL_SHIFT                  4
#define FTFA_FSTAT_ACCERR_MASK                   0x20u
#define FTFA_FSTAT_ACCERR_SHIFT                  5
#define FTFA_FSTAT_RDCOLERR_MASK                 0x40u
#define FTFA_FSTAT_RDCOLERR_SHIFT                6
#define FTFA_FSTAT_CCIF_MASK                     0x80u
#define FTFA_FSTAT_CCIF_SHIFT                    7
/* FCNFG Bit Fields */
#define FTFA_FCNFG_ERSSUSP_MASK                  0x10u
#define FTFA_FCNFG_ERSSUSP_SHIFT                 4
#define FTFA_FCNFG_ERSAREQ_MASK                  0x20u
#define FTFA_FCNFG_ERSAREQ_SHIFT                 5
#define FTFA_FCNFG_RDCOLLIE_MASK                 0x40u
#define FTFA_FCNFG_RDCOLLIE_SHIFT                6
#define FTFA_FCNFG_CCIE_MASK                     0x80u
#define FTFA_FCNFG_CCIE_SHIFT                    7
/* FSEC Bit Fields */
#define FTFA_FSEC_SEC_MASK                       0x3u
#define FTFA_FSEC_SEC_SHIFT                      0
#define FTFA_FSEC_SEC(x)                         (((uint8)(((uint8)(x))<<FTFA_FSEC_SEC_SHIFT))&FTFA_FSEC_SEC_MASK)
#define FTFA_FSEC_FSLACC_MASK                    0xCu
#define FTFA_FSEC_FSLACC_SHIFT                   2
#define FTFA_FSEC_FSLACC(x)                      (((uint8)(((uint8)(x))<<FTFA_FSEC_FSLACC_SHIFT))&FTFA_FSEC_FSLACC_MASK)
#define FTFA_FSEC_MEEN_MASK                      0x30u
#define FTFA_FSEC_MEEN_SHIFT                     4
#define FTFA_FSEC_MEEN(x)                        (((uint8)(((uint8)(x))<<FTFA_FSEC_MEEN_SHIFT))&FTFA_FSEC_MEEN_MASK)
#define FTFA_FSEC_KEYEN_MASK                     0xC0u
#define FTFA_FSEC_KEYEN_SHIFT                    6
#define FTFA_FSEC_KEYEN(x)                       (((uint8)(((uint8)(x))<<FTFA_FSEC_KEYEN_SHIFT))&FTFA_FSEC_KEYEN_MASK)
/* FOPT Bit Fields */
#define FTFA_FOPT_OPT_MASK                       0xFFu
#define FTFA_FOPT_OPT_SHIFT                      0
#define FTFA_FOPT_OPT(x)                         (((uint8)(((uint8)(x))<<FTFA_FOPT_OPT_SHIFT))&FTFA_FOPT_OPT_MASK)
/* FCCOB3 Bit Fields */
#define FTFA_FCCOB3_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB3_CCOBn_SHIFT                  0
#define FTFA_FCCOB3_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB3_CCOBn_SHIFT))&FTFA_FCCOB3_CCOBn_MASK)
/* FCCOB2 Bit Fields */
#define FTFA_FCCOB2_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB2_CCOBn_SHIFT                  0
#define FTFA_FCCOB2_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB2_CCOBn_SHIFT))&FTFA_FCCOB2_CCOBn_MASK)
/* FCCOB1 Bit Fields */
#define FTFA_FCCOB1_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB1_CCOBn_SHIFT                  0
#define FTFA_FCCOB1_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB1_CCOBn_SHIFT))&FTFA_FCCOB1_CCOBn_MASK)
/* FCCOB0 Bit Fields */
#define FTFA_FCCOB0_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB0_CCOBn_SHIFT                  0
#define FTFA_FCCOB0_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB0_CCOBn_SHIFT))&FTFA_FCCOB0_CCOBn_MASK)
/* FCCOB7 Bit Fields */
#define FTFA_FCCOB7_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB7_CCOBn_SHIFT                  0
#define FTFA_FCCOB7_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB7_CCOBn_SHIFT))&FTFA_FCCOB7_CCOBn_MASK)
/* FCCOB6 Bit Fields */
#define FTFA_FCCOB6_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB6_CCOBn_SHIFT                  0
#define FTFA_FCCOB6_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB6_CCOBn_SHIFT))&FTFA_FCCOB6_CCOBn_MASK)
/* FCCOB5 Bit Fields */
#define FTFA_FCCOB5_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB5_CCOBn_SHIFT                  0
#define FTFA_FCCOB5_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB5_CCOBn_SHIFT))&FTFA_FCCOB5_CCOBn_MASK)
/* FCCOB4 Bit Fields */
#define FTFA_FCCOB4_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB4_CCOBn_SHIFT                  0
#define FTFA_FCCOB4_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB4_CCOBn_SHIFT))&FTFA_FCCOB4_CCOBn_MASK)
/* FCCOBB Bit Fields */
#define FTFA_FCCOBB_CCOBn_MASK                   0xFFu
#define FTFA_FCCOBB_CCOBn_SHIFT                  0
#define FTFA_FCCOBB_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOBB_CCOBn_SHIFT))&FTFA_FCCOBB_CCOBn_MASK)
/* FCCOBA Bit Fields */
#define FTFA_FCCOBA_CCOBn_MASK                   0xFFu
#define FTFA_FCCOBA_CCOBn_SHIFT                  0
#define FTFA_FCCOBA_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOBA_CCOBn_SHIFT))&FTFA_FCCOBA_CCOBn_MASK)
/* FCCOB9 Bit Fields */
#define FTFA_FCCOB9_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB9_CCOBn_SHIFT                  0
#define FTFA_FCCOB9_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB9_CCOBn_SHIFT))&FTFA_FCCOB9_CCOBn_MASK)
/* FCCOB8 Bit Fields */
#define FTFA_FCCOB8_CCOBn_MASK                   0xFFu
#define FTFA_FCCOB8_CCOBn_SHIFT                  0
#define FTFA_FCCOB8_CCOBn(x)                     (((uint8)(((uint8)(x))<<FTFA_FCCOB8_CCOBn_SHIFT))&FTFA_FCCOB8_CCOBn_MASK)
/* FPROT3 Bit Fields */
#define FTFA_FPROT3_PROT_MASK                    0xFFu
#define FTFA_FPROT3_PROT_SHIFT                   0
#define FTFA_FPROT3_PROT(x)                      (((uint8)(((uint8)(x))<<FTFA_FPROT3_PROT_SHIFT))&FTFA_FPROT3_PROT_MASK)
/* FPROT2 Bit Fields */
#define FTFA_FPROT2_PROT_MASK                    0xFFu
#define FTFA_FPROT2_PROT_SHIFT                   0
#define FTFA_FPROT2_PROT(x)                      (((uint8)(((uint8)(x))<<FTFA_FPROT2_PROT_SHIFT))&FTFA_FPROT2_PROT_MASK)
/* FPROT1 Bit Fields */
#define FTFA_FPROT1_PROT_MASK                    0xFFu
#define FTFA_FPROT1_PROT_SHIFT                   0
#define FTFA_FPROT1_PROT(x)                      (((uint8)(((uint8)(x))<<FTFA_FPROT1_PROT_SHIFT))&FTFA_FPROT1_PROT_MASK)
/* FPROT0 Bit Fields */
#define FTFA_FPROT0_PROT_MASK                    0xFFu
#define FTFA_FPROT0_PROT_SHIFT                   0
#define FTFA_FPROT0_PROT(x)                      (((uint8)(((uint8)(x))<<FTFA_FPROT0_PROT_SHIFT))&FTFA_FPROT0_PROT_MASK)
/* XACCH3 Bit Fields */
#define FTFA_XACCH3_XA_MASK                      0xFFu
#define FTFA_XACCH3_XA_SHIFT                     0
#define FTFA_XACCH3_XA(x)                        (((uint8)(((uint8)(x))<<FTFA_XACCH3_XA_SHIFT))&FTFA_XACCH3_XA_MASK)
/* XACCH2 Bit Fields */
#define FTFA_XACCH2_XA_MASK                      0xFFu
#define FTFA_XACCH2_XA_SHIFT                     0
#define FTFA_XACCH2_XA(x)                        (((uint8)(((uint8)(x))<<FTFA_XACCH2_XA_SHIFT))&FTFA_XACCH2_XA_MASK)
/* XACCH1 Bit Fields */
#define FTFA_XACCH1_XA_MASK                      0xFFu
#define FTFA_XACCH1_XA_SHIFT                     0
#define FTFA_XACCH1_XA(x)                        (((uint8)(((uint8)(x))<<FTFA_XACCH1_XA_SHIFT))&FTFA_XACCH1_XA_MASK)
/* XACCH0 Bit Fields */
#define FTFA_XACCH0_XA_MASK                      0xFFu
#define FTFA_XACCH0_XA_SHIFT                     0
#define FTFA_XACCH0_XA(x)                        (((uint8)(((uint8)(x))<<FTFA_XACCH0_XA_SHIFT))&FTFA_XACCH0_XA_MASK)
/* XACCL3 Bit Fields */
#define FTFA_XACCL3_XA_MASK                      0xFFu
#define FTFA_XACCL3_XA_SHIFT                     0
#define FTFA_XACCL3_XA(x)                        (((uint8)(((uint8)(x))<<FTFA_XACCL3_XA_SHIFT))&FTFA_XACCL3_XA_MASK)
/* XACCL2 Bit Fields */
#define FTFA_XACCL2_XA_MASK                      0xFFu
#define FTFA_XACCL2_XA_SHIFT                     0
#define FTFA_XACCL2_XA(x)                        (((uint8)(((uint8)(x))<<FTFA_XACCL2_XA_SHIFT))&FTFA_XACCL2_XA_MASK)
/* XACCL1 Bit Fields */
#define FTFA_XACCL1_XA_MASK                      0xFFu
#define FTFA_XACCL1_XA_SHIFT                     0
#define FTFA_XACCL1_XA(x)                        (((uint8)(((uint8)(x))<<FTFA_XACCL1_XA_SHIFT))&FTFA_XACCL1_XA_MASK)
/* XACCL0 Bit Fields */
#define FTFA_XACCL0_XA_MASK                      0xFFu
#define FTFA_XACCL0_XA_SHIFT                     0
#define FTFA_XACCL0_XA(x)                        (((uint8)(((uint8)(x))<<FTFA_XACCL0_XA_SHIFT))&FTFA_XACCL0_XA_MASK)
/* SACCH3 Bit Fields */
#define FTFA_SACCH3_SA_MASK                      0xFFu
#define FTFA_SACCH3_SA_SHIFT                     0
#define FTFA_SACCH3_SA(x)                        (((uint8)(((uint8)(x))<<FTFA_SACCH3_SA_SHIFT))&FTFA_SACCH3_SA_MASK)
/* SACCH2 Bit Fields */
#define FTFA_SACCH2_SA_MASK                      0xFFu
#define FTFA_SACCH2_SA_SHIFT                     0
#define FTFA_SACCH2_SA(x)                        (((uint8)(((uint8)(x))<<FTFA_SACCH2_SA_SHIFT))&FTFA_SACCH2_SA_MASK)
/* SACCH1 Bit Fields */
#define FTFA_SACCH1_SA_MASK                      0xFFu
#define FTFA_SACCH1_SA_SHIFT                     0
#define FTFA_SACCH1_SA(x)                        (((uint8)(((uint8)(x))<<FTFA_SACCH1_SA_SHIFT))&FTFA_SACCH1_SA_MASK)
/* SACCH0 Bit Fields */
#define FTFA_SACCH0_SA_MASK                      0xFFu
#define FTFA_SACCH0_SA_SHIFT                     0
#define FTFA_SACCH0_SA(x)                        (((uint8)(((uint8)(x))<<FTFA_SACCH0_SA_SHIFT))&FTFA_SACCH0_SA_MASK)
/* SACCL3 Bit Fields */
#define FTFA_SACCL3_SA_MASK                      0xFFu
#define FTFA_SACCL3_SA_SHIFT                     0
#define FTFA_SACCL3_SA(x)                        (((uint8)(((uint8)(x))<<FTFA_SACCL3_SA_SHIFT))&FTFA_SACCL3_SA_MASK)
/* SACCL2 Bit Fields */
#define FTFA_SACCL2_SA_MASK                      0xFFu
#define FTFA_SACCL2_SA_SHIFT                     0
#define FTFA_SACCL2_SA(x)                        (((uint8)(((uint8)(x))<<FTFA_SACCL2_SA_SHIFT))&FTFA_SACCL2_SA_MASK)
/* SACCL1 Bit Fields */
#define FTFA_SACCL1_SA_MASK                      0xFFu
#define FTFA_SACCL1_SA_SHIFT                     0
#define FTFA_SACCL1_SA(x)                        (((uint8)(((uint8)(x))<<FTFA_SACCL1_SA_SHIFT))&FTFA_SACCL1_SA_MASK)
/* SACCL0 Bit Fields */
#define FTFA_SACCL0_SA_MASK                      0xFFu
#define FTFA_SACCL0_SA_SHIFT                     0
#define FTFA_SACCL0_SA(x)                        (((uint8)(((uint8)(x))<<FTFA_SACCL0_SA_SHIFT))&FTFA_SACCL0_SA_MASK)
/* FACSS Bit Fields */
#define FTFA_FACSS_SGSIZE_MASK                   0xFFu
#define FTFA_FACSS_SGSIZE_SHIFT                  0
#define FTFA_FACSS_SGSIZE(x)                     (((uint8)(((uint8)(x))<<FTFA_FACSS_SGSIZE_SHIFT))&FTFA_FACSS_SGSIZE_MASK)
/* FACSN Bit Fields */
#define FTFA_FACSN_NUMSG_MASK                    0xFFu
#define FTFA_FACSN_NUMSG_SHIFT                   0
#define FTFA_FACSN_NUMSG(x)                      (((uint8)(((uint8)(x))<<FTFA_FACSN_NUMSG_SHIFT))&FTFA_FACSN_NUMSG_MASK)

/*!
 * @}
 */ /* end of group FTFA_Register_Masks */


/* FTFA - Peripheral instance base addresses */
/** Peripheral FTFA base pointer */
#define FTFA_BASE_PTR                            ((FTFA_MemMapPtr)0x40020000u)
/** Array initializer of FTFA peripheral base pointers */
#define FTFA_BASE_PTRS                           { FTFA_BASE_PTR }

/* ----------------------------------------------------------------------------
   -- FTFA - Register accessor macros
   ---------------------------------------------------------------------------- */

/* FTFA - Register instance definitions */
/* FTFA */
#define FTFA_FSTAT                               FTFA_FSTAT_REG(FTFA_BASE_PTR)
#define FTFA_FCNFG                               FTFA_FCNFG_REG(FTFA_BASE_PTR)
#define FTFA_FSEC                                FTFA_FSEC_REG(FTFA_BASE_PTR)
#define FTFA_FOPT                                FTFA_FOPT_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB3                              FTFA_FCCOB3_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB2                              FTFA_FCCOB2_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB1                              FTFA_FCCOB1_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB0                              FTFA_FCCOB0_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB7                              FTFA_FCCOB7_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB6                              FTFA_FCCOB6_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB5                              FTFA_FCCOB5_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB4                              FTFA_FCCOB4_REG(FTFA_BASE_PTR)
#define FTFA_FCCOBB                              FTFA_FCCOBB_REG(FTFA_BASE_PTR)
#define FTFA_FCCOBA                              FTFA_FCCOBA_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB9                              FTFA_FCCOB9_REG(FTFA_BASE_PTR)
#define FTFA_FCCOB8                              FTFA_FCCOB8_REG(FTFA_BASE_PTR)
#define FTFA_FPROT3                              FTFA_FPROT3_REG(FTFA_BASE_PTR)
#define FTFA_FPROT2                              FTFA_FPROT2_REG(FTFA_BASE_PTR)
#define FTFA_FPROT1                              FTFA_FPROT1_REG(FTFA_BASE_PTR)
#define FTFA_FPROT0                              FTFA_FPROT0_REG(FTFA_BASE_PTR)
#define FTFA_XACCH3                              FTFA_XACCH3_REG(FTFA_BASE_PTR)
#define FTFA_XACCH2                              FTFA_XACCH2_REG(FTFA_BASE_PTR)
#define FTFA_XACCH1                              FTFA_XACCH1_REG(FTFA_BASE_PTR)
#define FTFA_XACCH0                              FTFA_XACCH0_REG(FTFA_BASE_PTR)
#define FTFA_XACCL3                              FTFA_XACCL3_REG(FTFA_BASE_PTR)
#define FTFA_XACCL2                              FTFA_XACCL2_REG(FTFA_BASE_PTR)
#define FTFA_XACCL1                              FTFA_XACCL1_REG(FTFA_BASE_PTR)
#define FTFA_XACCL0                              FTFA_XACCL0_REG(FTFA_BASE_PTR)
#define FTFA_SACCH3                              FTFA_SACCH3_REG(FTFA_BASE_PTR)
#define FTFA_SACCH2                              FTFA_SACCH2_REG(FTFA_BASE_PTR)
#define FTFA_SACCH1                              FTFA_SACCH1_REG(FTFA_BASE_PTR)
#define FTFA_SACCH0                              FTFA_SACCH0_REG(FTFA_BASE_PTR)
#define FTFA_SACCL3                              FTFA_SACCL3_REG(FTFA_BASE_PTR)
#define FTFA_SACCL2                              FTFA_SACCL2_REG(FTFA_BASE_PTR)
#define FTFA_SACCL1                              FTFA_SACCL1_REG(FTFA_BASE_PTR)
#define FTFA_SACCL0                              FTFA_SACCL0_REG(FTFA_BASE_PTR)
#define FTFA_FACSS                               FTFA_FACSS_REG(FTFA_BASE_PTR)
#define FTFA_FACSN                               FTFA_FACSN_REG(FTFA_BASE_PTR)

/* FCCOBIX Bit Fields */
#define FTMRE_FCCOBIX_CCOBIX_MASK                0x7u
#define FTMRE_FCCOBIX_CCOBIX_SHIFT               0
#define FTMRE_FCCOBIX_CCOBIX(x)                  (((uint8)(((uint8)(x))<<FTMRE_FCCOBIX_CCOBIX_SHIFT))&FTMRE_FCCOBIX_CCOBIX_MASK)
/* FSEC Bit Fields */
#define FTMRE_FSEC_SEC_MASK                      0x3u
#define FTMRE_FSEC_SEC_SHIFT                     0
#define FTMRE_FSEC_SEC(x)                        (((uint8)(((uint8)(x))<<FTMRE_FSEC_SEC_SHIFT))&FTMRE_FSEC_SEC_MASK)
#define FTMRE_FSEC_KEYEN_MASK                    0xC0u
#define FTMRE_FSEC_KEYEN_SHIFT                   6
#define FTMRE_FSEC_KEYEN(x)                      (((uint8)(((uint8)(x))<<FTMRE_FSEC_KEYEN_SHIFT))&FTMRE_FSEC_KEYEN_MASK)
/* FCLKDIV Bit Fields */
#define FTMRE_FCLKDIV_FDIV_MASK                  0x3Fu
#define FTMRE_FCLKDIV_FDIV_SHIFT                 0
#define FTMRE_FCLKDIV_FDIV(x)                    (((uint8)(((uint8)(x))<<FTMRE_FCLKDIV_FDIV_SHIFT))&FTMRE_FCLKDIV_FDIV_MASK)
#define FTMRE_FCLKDIV_FDIVLCK_MASK               0x40u
#define FTMRE_FCLKDIV_FDIVLCK_SHIFT              6
#define FTMRE_FCLKDIV_FDIVLD_MASK                0x80u
#define FTMRE_FCLKDIV_FDIVLD_SHIFT               7
/* FSTAT Bit Fields */
#define FTMRE_FSTAT_MGSTAT_MASK                  0x3u
#define FTMRE_FSTAT_MGSTAT_SHIFT                 0
#define FTMRE_FSTAT_MGSTAT(x)                    (((uint8)(((uint8)(x))<<FTMRE_FSTAT_MGSTAT_SHIFT))&FTMRE_FSTAT_MGSTAT_MASK)
#define FTMRE_FSTAT_MGBUSY_MASK                  0x8u
#define FTMRE_FSTAT_MGBUSY_SHIFT                 3
#define FTMRE_FSTAT_FPVIOL_MASK                  0x10u
#define FTMRE_FSTAT_FPVIOL_SHIFT                 4
#define FTMRE_FSTAT_ACCERR_MASK                  0x20u
#define FTMRE_FSTAT_ACCERR_SHIFT                 5
#define FTMRE_FSTAT_CCIF_MASK                    0x80u
#define FTMRE_FSTAT_CCIF_SHIFT                   7
/* FCNFG Bit Fields */
#define FTMRE_FCNFG_ERSAREQ_MASK                 0x20u
#define FTMRE_FCNFG_ERSAREQ_SHIFT                5
#define FTMRE_FCNFG_CCIE_MASK                    0x80u
#define FTMRE_FCNFG_CCIE_SHIFT                   7
/* FCCOBLO Bit Fields */
#define FTMRE_FCCOBLO_CCOB_MASK                  0xFFu
#define FTMRE_FCCOBLO_CCOB_SHIFT                 0
#define FTMRE_FCCOBLO_CCOB(x)                    (((uint8)(((uint8)(x))<<FTMRE_FCCOBLO_CCOB_SHIFT))&FTMRE_FCCOBLO_CCOB_MASK)
/* FCCOBHI Bit Fields */
#define FTMRE_FCCOBHI_CCOB_MASK                  0xFFu
#define FTMRE_FCCOBHI_CCOB_SHIFT                 0
#define FTMRE_FCCOBHI_CCOB(x)                    (((uint8)(((uint8)(x))<<FTMRE_FCCOBHI_CCOB_SHIFT))&FTMRE_FCCOBHI_CCOB_MASK)
/* FPROT Bit Fields */
#define FTMRE_FPROT_FPLS_MASK                    0x3u
#define FTMRE_FPROT_FPLS_SHIFT                   0
#define FTMRE_FPROT_FPLS(x)                      (((uint8)(((uint8)(x))<<FTMRE_FPROT_FPLS_SHIFT))&FTMRE_FPROT_FPLS_MASK)
#define FTMRE_FPROT_FPLDIS_MASK                  0x4u
#define FTMRE_FPROT_FPLDIS_SHIFT                 2
#define FTMRE_FPROT_FPHS_MASK                    0x18u
#define FTMRE_FPROT_FPHS_SHIFT                   3
#define FTMRE_FPROT_FPHS(x)                      (((uint8)(((uint8)(x))<<FTMRE_FPROT_FPHS_SHIFT))&FTMRE_FPROT_FPHS_MASK)
#define FTMRE_FPROT_FPHDIS_MASK                  0x20u
#define FTMRE_FPROT_FPHDIS_SHIFT                 5
#define FTMRE_FPROT_RNV6_MASK                    0x40u
#define FTMRE_FPROT_RNV6_SHIFT                   6
#define FTMRE_FPROT_FPOPEN_MASK                  0x80u
#define FTMRE_FPROT_FPOPEN_SHIFT                 7
/* FOPT Bit Fields */
#define FTMRE_FOPT_NV_MASK                       0xFFu
#define FTMRE_FOPT_NV_SHIFT                      0
//-------------------------- End of Flash Configuration register -----------------------------------------------------

/* ----------------------------------------------------------------------------
   -- MCM Peripheral Access Layer
   ---------------------------------------------------------------------------- */

#define MCM_BASE								 0xF0003008

typedef struct
{
	uint16 PLASC;
	uint16 PLAMC;
	uint32 PLACR;
}MCM_BASIC_DEF;

#define MCM										(*(volatile MCM_BASIC_DEF *)MCM_BASE)

/* ----------------------------------------------------------------------------
   -- MCM Register Masks
   ---------------------------------------------------------------------------- */


/* PLASC Bit Fields */
#define MCM_PLASC_ASC_MASK                       0xFFu
#define MCM_PLASC_ASC_SHIFT                      0
#define MCM_PLASC_ASC(x)                         (((uint16)(((uint16)(x))<<MCM_PLASC_ASC_SHIFT))&MCM_PLASC_ASC_MASK)
/* PLAMC Bit Fields */
#define MCM_PLAMC_AMC_MASK                       0xFFu
#define MCM_PLAMC_AMC_SHIFT                      0
#define MCM_PLAMC_AMC(x)                         (((uint16)(((uint16)(x))<<MCM_PLAMC_AMC_SHIFT))&MCM_PLAMC_AMC_MASK)
/* PLACR Bit Fields */
#define MCM_PLACR_CFCC_MASK                      0x400u
#define MCM_PLACR_CFCC_SHIFT                     10
#define MCM_PLACR_DFCDA_MASK                     0x800u
#define MCM_PLACR_DFCDA_SHIFT                    11
#define MCM_PLACR_DFCIC_MASK                     0x1000u
#define MCM_PLACR_DFCIC_SHIFT                    12
#define MCM_PLACR_DFCC_MASK                      0x2000u
#define MCM_PLACR_DFCC_SHIFT                     13
#define MCM_PLACR_EFDS_MASK                      0x4000u
#define MCM_PLACR_EFDS_SHIFT                     14
#define MCM_PLACR_DFCS_MASK                      0x8000u
#define MCM_PLACR_DFCS_SHIFT                     15
#define MCM_PLACR_ESFC_MASK                      0x10000u
#define MCM_PLACR_ESFC_SHIFT                     16

//-------------------------- Start of PMC Configuration register -----------------------------------------------------

//-- PMC  Power Management Controller (PMC) registers

#define PMC_BASE								0x4007D000u
#define PMC	                            		(*(volatile PMC_BASIC_DEF *)PMC_BASE)

///** PMC - Peripheral register structure */
typedef struct
{
union
{
	uint8 BYTE;
	struct
	{
		uint8 LVDV      :2;                      //Low-Voltage Detect Voltage Select
		uint8 RESERVE23 :2;                      //This field is reserved
		uint8 LVDRE     :1;                      //Low-Voltage Detect Reset Enable
		uint8 LVDIE     :1;                      //Low-Voltage Detect Interrupt Enable
		uint8 LVDACK    :1;                      //Low-Voltage Detect Acknowledge
		uint8 LVDF      :1;                      //Low-Voltage Detect Flag
	}BIT;
}LVDSC1;

union
{
	uint8 BYTE;
	struct
	{
		uint8 LVWV   	:2;							//Low-Voltage Warning Voltage Select
		uint8 RESERVE  	:3;							//This field is reserved
		uint8 LVWIE  	:1;							//Low-Voltage Warning Interrupt Enable
		uint8 LVWACK	:1;							//Low-Voltage Warning Acknowledge
		uint8 LVWF      :1;                         //Low-Voltage Warning Voltage Flag
	}BIT;
}LVDSC2;

union
{
	uint8 BYTE;
	struct
	{
		uint8 BGBE   	:1;							//Bandgap Buffer Enable
		uint8 RESERVE  	:1;							//This field is reserved
		uint8 REGONS  	:1;							//Regulator In Run Regulation Status
		uint8 ACKISO	:1;							//Acknowledge Isolation
		uint8 BGEN      :1;                         //Bandgap Enable In VLPx Operation
		uint8 RESERVED  :3;                         //This field is reserved
	}BIT;
}REGSC;

}PMC_BASIC_DEF;

/* ----------------------------------------------------------------------------
   -- PMC - Register accessor macros
   ---------------------------------------------------------------------------- */
/* PMC - Register accessors */
#define PMC_LVDSC1_REG(base)                     ((base)->LVDSC1)
#define PMC_LVDSC2_REG(base)                     ((base)->LVDSC2)
#define PMC_REGSC_REG(base)                      ((base)->REGSC)

/* ----------------------------------------------------------------------------
   -- PMC Register Masks
   ---------------------------------------------------------------------------- */

/* LVDSC1 Bit Fields */
#define PMC_LVDSC1_LVDV_MASK                     0x3u
#define PMC_LVDSC1_LVDV_SHIFT                    0
#define PMC_LVDSC1_LVDV(x)                       (((uint8)(((uint8)(x))<<PMC_LVDSC1_LVDV_SHIFT))&PMC_LVDSC1_LVDV_MASK)
#define PMC_LVDSC1_LVDRE_MASK                    0x10u
#define PMC_LVDSC1_LVDRE_SHIFT                   4
#define PMC_LVDSC1_LVDIE_MASK                    0x20u
#define PMC_LVDSC1_LVDIE_SHIFT                   5
#define PMC_LVDSC1_LVDACK_MASK                   0x40u
#define PMC_LVDSC1_LVDACK_SHIFT                  6
#define PMC_LVDSC1_LVDF_MASK                     0x80u
#define PMC_LVDSC1_LVDF_SHIFT                    7
/* LVDSC2 Bit Fields */
#define PMC_LVDSC2_LVWV_MASK                     0x3u
#define PMC_LVDSC2_LVWV_SHIFT                    0
#define PMC_LVDSC2_LVWV(x)                       (((uint8)(((uint8)(x))<<PMC_LVDSC2_LVWV_SHIFT))&PMC_LVDSC2_LVWV_MASK)
#define PMC_LVDSC2_LVWIE_MASK                    0x20u
#define PMC_LVDSC2_LVWIE_SHIFT                   5
#define PMC_LVDSC2_LVWACK_MASK                   0x40u
#define PMC_LVDSC2_LVWACK_SHIFT                  6
#define PMC_LVDSC2_LVWF_MASK                     0x80u
#define PMC_LVDSC2_LVWF_SHIFT                    7
/* REGSC Bit Fields */
#define PMC_REGSC_BGBE_MASK                      0x1u
#define PMC_REGSC_BGBE_SHIFT                     0
#define PMC_REGSC_REGONS_MASK                    0x4u
#define PMC_REGSC_REGONS_SHIFT                   2
#define PMC_REGSC_ACKISO_MASK                    0x8u
#define PMC_REGSC_ACKISO_SHIFT                   3
#define PMC_REGSC_BGEN_MASK                      0x10u
#define PMC_REGSC_BGEN_SHIFT                     4

/* PMC - Peripheral instance base addresses */
/** Peripheral PMC base pointer */
#define PMC_BASE_PTR                             ((PMC_MemMapPtr)0x4007D000u)
/** Array initializer of PMC peripheral base pointers */
#define PMC_BASE_PTRS                            { PMC_BASE_PTR }

/* ----------------------------------------------------------------------------
   -- PMC - Register accessor macros
   ---------------------------------------------------------------------------- */

/* PMC - Register instance definitions */
/* PMC */
#define PMC_LVDSC1                               PMC_LVDSC1_REG(PMC_BASE_PTR)
#define PMC_LVDSC2                               PMC_LVDSC2_REG(PMC_BASE_PTR)
#define PMC_REGSC                                PMC_REGSC_REG(PMC_BASE_PTR)


//-------------------------- End of PMC Configuration register -----------------------------------------------------

/* ----------------------------------------------------------------------------
   -- SMC
   ---------------------------------------------------------------------------- */

#define SMC_BASE								0x4007E000u
#define SMC	                            		(*(volatile SMC_BASIC_DEF *)SMC_BASE)

/** SMC - Peripheral register structure */
typedef struct
{
	union
	{
		uint8 BYTE;
		struct
		{
			uint8 RESERVED0: 1;
			uint8 AVLLS    : 1;
			uint8 RESERVED2: 1;
			uint8 ALLS     : 1;
			uint8 RESERVED4: 1;
			uint8 AVLP     : 1;
			uint8 RESERVED6: 1;
			uint8 AHSRUN   : 1;

		}BIT;

	}PMPROT;                                     //Power Mode Protection register, offset: 0x0

  uint8 PMCTRL;                                  //Power Mode Control register, offset: 0x1
  uint8 STOPCTRL;                                //Stop Control Register, offset: 0x2
  uint8 PMSTAT;                                  //Power Mode Status register, offset: 0x3
} volatile SMC_BASIC_DEF;

/* ----------------------------------------------------------------------------
   -- SMC - Register accessor macros
   ---------------------------------------------------------------------------- */

/* SMC - Register accessors */
#define SMC_PMPROT_REG(base)                     ((base)->PMPROT)
#define SMC_PMCTRL_REG(base)                     ((base)->PMCTRL)
#define SMC_STOPCTRL_REG(base)                   ((base)->STOPCTRL)
#define SMC_PMSTAT_REG(base)                     ((base)->PMSTAT)

/* ----------------------------------------------------------------------------
   -- SMC Register Masks
   ---------------------------------------------------------------------------- */

/* PMPROT Bit Fields */
#define SMC_PMPROT_AVLLS_MASK                    0x2u
#define SMC_PMPROT_AVLLS_SHIFT                   1
#define SMC_PMPROT_ALLS_MASK                     0x8u
#define SMC_PMPROT_ALLS_SHIFT                    3
#define SMC_PMPROT_AVLP_MASK                     0x20u
#define SMC_PMPROT_AVLP_SHIFT                    5
#define SMC_PMPROT_AHSRUN_MASK                   0x80u
#define SMC_PMPROT_AHSRUN_SHIFT                  7
/* PMCTRL Bit Fields */
#define SMC_PMCTRL_STOPM_MASK                    0x7u
#define SMC_PMCTRL_STOPM_SHIFT                   0
#define SMC_PMCTRL_STOPM(x)                      (((uint8)(((uint8)(x))<<SMC_PMCTRL_STOPM_SHIFT))&SMC_PMCTRL_STOPM_MASK)
#define SMC_PMCTRL_STOPA_MASK                    0x8u
#define SMC_PMCTRL_STOPA_SHIFT                   3
#define SMC_PMCTRL_RUNM_MASK                     0x60u
#define SMC_PMCTRL_RUNM_SHIFT                    5
#define SMC_PMCTRL_RUNM(x)                       (((uint8)(((uint8)(x))<<SMC_PMCTRL_RUNM_SHIFT))&SMC_PMCTRL_RUNM_MASK)
/* STOPCTRL Bit Fields */
#define SMC_STOPCTRL_LLSM_MASK                   0x7u
#define SMC_STOPCTRL_LLSM_SHIFT                  0
#define SMC_STOPCTRL_LLSM(x)                     (((uint8)(((uint8)(x))<<SMC_STOPCTRL_LLSM_SHIFT))&SMC_STOPCTRL_LLSM_MASK)
#define SMC_STOPCTRL_PORPO_MASK                  0x20u
#define SMC_STOPCTRL_PORPO_SHIFT                 5
#define SMC_STOPCTRL_PSTOPO_MASK                 0xC0u
#define SMC_STOPCTRL_PSTOPO_SHIFT                6
#define SMC_STOPCTRL_PSTOPO(x)                   (((uint8)(((uint8)(x))<<SMC_STOPCTRL_PSTOPO_SHIFT))&SMC_STOPCTRL_PSTOPO_MASK)
/* PMSTAT Bit Fields */
#define SMC_PMSTAT_PMSTAT_MASK                   0xFFu
#define SMC_PMSTAT_PMSTAT_SHIFT                  0
#define SMC_PMSTAT_PMSTAT(x)                     (((uint8)(((uint8)(x))<<SMC_PMSTAT_PMSTAT_SHIFT))&SMC_PMSTAT_PMSTAT_MASK)

/* SMC - Peripheral instance base addresses */
/** Peripheral SMC base pointer */
#define SMC_BASE_PTR                             ((SMC_MemMapPtr)0x4007E000u)
/** Array initializer of SMC peripheral base pointers */
#define SMC_BASE_PTRS                            { SMC_BASE_PTR }

/* ----------------------------------------------------------------------------
   -- SMC - Register accessor macros
   ---------------------------------------------------------------------------- */

/* SMC - Register instance definitions */
/* SMC */
#define SMC_PMPROT                               SMC_PMPROT_REG(SMC_BASE_PTR)
#define SMC_PMCTRL                               SMC_PMCTRL_REG(SMC_BASE_PTR)
#define SMC_STOPCTRL                             SMC_STOPCTRL_REG(SMC_BASE_PTR)
#define SMC_PMSTAT                               SMC_PMSTAT_REG(SMC_BASE_PTR)

//-------------------------- End of SMC Configuration register -----------------------------------------------------

//-------------------------- Start of IRQ Configuration register -----------------------------------------------------
#define IRQ_BASE		0x40031000

typedef union
{
	uint8 BYTE;
	struct
	{
		uint8 IRQMOD	:1;							//IRQ Detection Mode
		uint8 IRQIE 	:1;							//IRQ Interrupt Enable
		uint8 IRQACK	:1;							//IRQ Acknowledge
		uint8 IRQF  	:1;							//IRQ Flag
		uint8 IRQPE 	:1;							//IRQ Pin Enable
		uint8 IRQEDG	:1;							//IRQ Edge Select
		uint8 IRQPDD	:1;							//IRQ Pull Device Disable
		uint8 RES_bit7	:1;
	};
}IRQ_DEF;

#define IRQ		(*(volatile IRQ_DEF *)IRQ_BASE)
//-------------------------- End of IRQ Configuration register -----------------------------------------------------

/* ----------------------------------------------------------------------------
   -- Backward Compatibility
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Backward_Compatibility_Symbols Backward Compatibility
 * @{
 */

#define  MCG_C2_EREFS0_MASK          MCG_C2_EREFS_MASK
#define  MCG_C2_EREFS0_SHIFT         MCG_C2_EREFS_SHIFT
#define  MCG_C2_HGO0_MASK            MCG_C2_HGO_MASK
#define  MCG_C2_HGO0_SHIFT           MCG_C2_HGO_SHIFT
#define  MCG_C2_RANGE0_MASK          MCG_C2_RANGE_MASK
#define  MCG_C2_RANGE0_SHIFT         MCG_C2_RANGE_SHIFT
#define  MCG_C2_RANGE0(x)            MCG_C2_RANGE(x)

/*!
 * @}
 */ /* end of group Backward_Compatibility_Symbols */

//-------------------------- Start of PIT Peripherial Interrupt Timer ----------------------------------------------
#define PIT_BASE								 0x40037000

typedef struct
{
	volatile uint32 MCR;
	uint8 RESREVED_0[252];
	struct
	{
		volatile uint32 LDVAL;
		volatile uint32 CVAL;
		volatile uint32 TCTRL;
		volatile uint32 TFLG;
	}CHANNEL[4];
}PIT_BASIC_DEF;

#define PIT		                                (*(PIT_BASIC_DEF *)PIT_BASE)

/* MCR Bit Fields */
#define PIT_MCR_FRZ_MASK                         0x1u
#define PIT_MCR_FRZ_POS                          0
#define PIT_MCR_MDIS_MASK                        0x2u
#define PIT_MCR_MDIS_POS                         1
/* LTMR64H Bit Fields */
#define PIT_LTMR64H_LTH_MASK                     0xFFFFFFFFu
#define PIT_LTMR64H_LTH_POS                      0
/* LTMR64L Bit Fields */
#define PIT_LTMR64L_LTL_MASK                     0xFFFFFFFFu
#define PIT_LTMR64L_LTL_POS                      0
/* LDVAL Bit Fields */
#define PIT_LDVAL_TSV_MASK                       0xFFFFFFFFu
#define PIT_LDVAL_TSV_POS                        0
/* CVAL Bit Fields */
#define PIT_CVAL_TVL_MASK                        0xFFFFFFFFu
#define PIT_CVAL_TVL_POS                         0
/* TCTRL Bit Fields */
#define PIT_TCTRL_TEN_MASK                       0x1u
#define PIT_TCTRL_TEN_POS                        0
#define PIT_TCTRL_TIE_MASK                       0x2u
#define PIT_TCTRL_TIE_POS                        1
#define PIT_TCTRL_CHN_MASK                       0x4u
#define PIT_TCTRL_CHN_POS                        2
/* TFLG Bit Fields */
#define PIT_TFLG_TIF_MASK                        0x1u
#define PIT_TFLG_TIF_POS                         0
//-------------------------- End of PIT Peripherial Interrupt Timer ------------------------------------------------


//-------------------------- Start of Programmable Delay Block (PDB) ----------------------------------------------

#define PDB_BASE		0x40036000

/** PDB - Peripheral register structure */
typedef struct
{
  uint32 SC;                                     //Status and Control register, offset: 0x0
  uint32 MOD;                                    //Modulus register, offset: 0x4
  uint32 CNT;                                    //Counter register, offset: 0x8
  uint32 IDLY;                                   //Interrupt Delay register, offset: 0xC
  struct
  {                                              //offset: 0x10, array step: 0x28
    uint32 C1;                                   //Channel n Control register 1, array offset: 0x10, array step: 0x28
    uint32 S;                                    //Channel n Status register, array offset: 0x14, array step: 0x28
    uint32 DLY[2];                               //Channel n Delay 0 register..Channel n Delay 1 register, array offset: 0x18, array step: index*0x28, index2*0x4
    uint8 RESERVED_0[24];
  } CH[2];
  uint8 RESERVED_0[240];
  struct
  {                                               //offset: 0x150, array step: 0x8
    uint32 INTC;                                  //DAC Interval Trigger n Control register, array offset: 0x150, array step: 0x8
    uint32 INT;                                   //DAC Interval n register, array offset: 0x154, array step: 0x8
  } DAC[2];
  uint8 RESERVED_1[48];
  uint32 POEN;                                    //Pulse-Out n Enable register, offset: 0x190
  uint32 PODLY[2];                                //Pulse-Out n Delay register, array offset: 0x194, array step: 0x4
} volatile PDB_DEF;

#define PDB		(*(volatile PDB_DEF *)PDB_BASE)


/* ----------------------------------------------------------------------------
   -- PDB - Register accessor macros
   ---------------------------------------------------------------------------- */
/* PDB - Register accessors */
#define PDB_SC_REG(base)                         ((base)->SC)
#define PDB_MOD_REG(base)                        ((base)->MOD)
#define PDB_CNT_REG(base)                        ((base)->CNT)
#define PDB_IDLY_REG(base)                       ((base)->IDLY)
#define PDB_C1_REG(base,index)                   ((base)->CH[index].C1)
#define PDB_S_REG(base,index)                    ((base)->CH[index].S)
#define PDB_DLY_REG(base,index,index2)           ((base)->CH[index].DLY[index2])
#define PDB_INTC_REG(base,index)                 ((base)->DAC[index].INTC)
#define PDB_INT_REG(base,index)                  ((base)->DAC[index].INT)
#define PDB_POEN_REG(base)                       ((base)->POEN)
#define PDB_PODLY_REG(base,index)                ((base)->PODLY[index])

/* ----------------------------------------------------------------------------
   -- PDB Register Masks
   ---------------------------------------------------------------------------- */
/* SC Bit Fields */
#define PDB_SC_LDOK_MASK                         0x1u
#define PDB_SC_LDOK_SHIFT                        0
#define PDB_SC_CONT_MASK                         0x2u
#define PDB_SC_CONT_SHIFT                        1
#define PDB_SC_MULT_MASK                         0xCu
#define PDB_SC_MULT_SHIFT                        2
#define PDB_SC_MULT(x)                           (((uint32)(((uint32)(x))<<PDB_SC_MULT_SHIFT))&PDB_SC_MULT_MASK)
#define PDB_SC_PDBIE_MASK                        0x20u
#define PDB_SC_PDBIE_SHIFT                       5
#define PDB_SC_PDBIF_MASK                        0x40u
#define PDB_SC_PDBIF_SHIFT                       6
#define PDB_SC_PDBEN_MASK                        0x80u
#define PDB_SC_PDBEN_SHIFT                       7
#define PDB_SC_TRGSEL_MASK                       0xF00u
#define PDB_SC_TRGSEL_SHIFT                      8
#define PDB_SC_TRGSEL(x)                         (((uint32)(((uint32)(x))<<PDB_SC_TRGSEL_SHIFT))&PDB_SC_TRGSEL_MASK)
#define PDB_SC_PRESCALER_MASK                    0x7000u
#define PDB_SC_PRESCALER_SHIFT                   12
#define PDB_SC_PRESCALER(x)                      (((uint32)(((uint32)(x))<<PDB_SC_PRESCALER_SHIFT))&PDB_SC_PRESCALER_MASK)
#define PDB_SC_DMAEN_MASK                        0x8000u
#define PDB_SC_DMAEN_SHIFT                       15
#define PDB_SC_SWTRIG_MASK                       0x10000u
#define PDB_SC_SWTRIG_SHIFT                      16
#define PDB_SC_PDBEIE_MASK                       0x20000u
#define PDB_SC_PDBEIE_SHIFT                      17
#define PDB_SC_LDMOD_MASK                        0xC0000u
#define PDB_SC_LDMOD_SHIFT                       18
#define PDB_SC_LDMOD(x)                          (((uint32)(((uint32)(x))<<PDB_SC_LDMOD_SHIFT))&PDB_SC_LDMOD_MASK)
/* MOD Bit Fields */
#define PDB_MOD_MOD_MASK                         0xFFFFu
#define PDB_MOD_MOD_SHIFT                        0
#define PDB_MOD_MOD(x)                           (((uint32)(((uint32)(x))<<PDB_MOD_MOD_SHIFT))&PDB_MOD_MOD_MASK)
/* CNT Bit Fields */
#define PDB_CNT_CNT_MASK                         0xFFFFu
#define PDB_CNT_CNT_SHIFT                        0
#define PDB_CNT_CNT(x)                           (((uint32)(((uint32)(x))<<PDB_CNT_CNT_SHIFT))&PDB_CNT_CNT_MASK)
/* IDLY Bit Fields */
#define PDB_IDLY_IDLY_MASK                       0xFFFFu
#define PDB_IDLY_IDLY_SHIFT                      0
#define PDB_IDLY_IDLY(x)                         (((uint32)(((uint32)(x))<<PDB_IDLY_IDLY_SHIFT))&PDB_IDLY_IDLY_MASK)
/* C1 Bit Fields */
#define PDB_C1_EN_MASK                           0xFFu
#define PDB_C1_EN_SHIFT                          0
#define PDB_C1_EN(x)                             (((uint32)(((uint32)(x))<<PDB_C1_EN_SHIFT))&PDB_C1_EN_MASK)
#define PDB_C1_TOS_MASK                          0xFF00u
#define PDB_C1_TOS_SHIFT                         8
#define PDB_C1_TOS(x)                            (((uint32)(((uint32)(x))<<PDB_C1_TOS_SHIFT))&PDB_C1_TOS_MASK)
#define PDB_C1_BB_MASK                           0xFF0000u
#define PDB_C1_BB_SHIFT                          16
#define PDB_C1_BB(x)                             (((uint32)(((uint32)(x))<<PDB_C1_BB_SHIFT))&PDB_C1_BB_MASK)
/* S Bit Fields */
#define PDB_S_ERR_MASK                           0xFFu
#define PDB_S_ERR_SHIFT                          0
#define PDB_S_ERR(x)                             (((uint32)(((uint32)(x))<<PDB_S_ERR_SHIFT))&PDB_S_ERR_MASK)
#define PDB_S_CF_MASK                            0xFF0000u
#define PDB_S_CF_SHIFT                           16
#define PDB_S_CF(x)                              (((uint32)(((uint32)(x))<<PDB_S_CF_SHIFT))&PDB_S_CF_MASK)
/* DLY Bit Fields */
#define PDB_DLY_DLY_MASK                         0xFFFFu
#define PDB_DLY_DLY_SHIFT                        0
#define PDB_DLY_DLY(x)                           (((uint32)(((uint32)(x))<<PDB_DLY_DLY_SHIFT))&PDB_DLY_DLY_MASK)
/* INTC Bit Fields */
#define PDB_INTC_TOE_MASK                        0x1u
#define PDB_INTC_TOE_SHIFT                       0
#define PDB_INTC_EXT_MASK                        0x2u
#define PDB_INTC_EXT_SHIFT                       1
/* INT Bit Fields */
#define PDB_INT_INT_MASK                         0xFFFFu
#define PDB_INT_INT_SHIFT                        0
#define PDB_INT_INT(x)                           (((uint32)(((uint32)(x))<<PDB_INT_INT_SHIFT))&PDB_INT_INT_MASK)
/* POEN Bit Fields */
#define PDB_POEN_POEN_MASK                       0xFFu
#define PDB_POEN_POEN_SHIFT                      0
#define PDB_POEN_POEN(x)                         (((uint32)(((uint32)(x))<<PDB_POEN_POEN_SHIFT))&PDB_POEN_POEN_MASK)
/* PODLY Bit Fields */
#define PDB_PODLY_DLY2_MASK                      0xFFFFu
#define PDB_PODLY_DLY2_SHIFT                     0
#define PDB_PODLY_DLY2(x)                        (((uint32)(((uint32)(x))<<PDB_PODLY_DLY2_SHIFT))&PDB_PODLY_DLY2_MASK)
#define PDB_PODLY_DLY1_MASK                      0xFFFF0000u
#define PDB_PODLY_DLY1_SHIFT                     16
#define PDB_PODLY_DLY1(x)                        (((uint32)(((uint32)(x))<<PDB_PODLY_DLY1_SHIFT))&PDB_PODLY_DLY1_MASK)

/* ----------------------------------------------------------------------------
   -- PDB - Register accessor macros
   ---------------------------------------------------------------------------- */
/* PDB - Register instance definitions */
/* PDB0 */
#define PDB0_SC                                  PDB_SC_REG(PDB0_BASE_PTR)
#define PDB0_MOD                                 PDB_MOD_REG(PDB0_BASE_PTR)
#define PDB0_CNT                                 PDB_CNT_REG(PDB0_BASE_PTR)
#define PDB0_IDLY                                PDB_IDLY_REG(PDB0_BASE_PTR)
#define PDB0_CH0C1                               PDB_C1_REG(PDB0_BASE_PTR,0)
#define PDB0_CH0S                                PDB_S_REG(PDB0_BASE_PTR,0)
#define PDB0_CH0DLY0                             PDB_DLY_REG(PDB0_BASE_PTR,0,0)
#define PDB0_CH0DLY1                             PDB_DLY_REG(PDB0_BASE_PTR,0,1)
#define PDB0_CH1C1                               PDB_C1_REG(PDB0_BASE_PTR,1)
#define PDB0_CH1S                                PDB_S_REG(PDB0_BASE_PTR,1)
#define PDB0_CH1DLY0                             PDB_DLY_REG(PDB0_BASE_PTR,1,0)
#define PDB0_CH1DLY1                             PDB_DLY_REG(PDB0_BASE_PTR,1,1)
#define PDB0_DACINTC0                            PDB_INTC_REG(PDB0_BASE_PTR,0)
#define PDB0_DACINT0                             PDB_INT_REG(PDB0_BASE_PTR,0)
#define PDB0_DACINTC1                            PDB_INTC_REG(PDB0_BASE_PTR,1)
#define PDB0_DACINT1                             PDB_INT_REG(PDB0_BASE_PTR,1)
#define PDB0_POEN                                PDB_POEN_REG(PDB0_BASE_PTR)
#define PDB0_PO0DLY                              PDB_PODLY_REG(PDB0_BASE_PTR,0)
#define PDB0_PO1DLY                              PDB_PODLY_REG(PDB0_BASE_PTR,1)

/* PDB - Register array accessors */
#define PDB0_C1(index)                           PDB_C1_REG(PDB0_BASE_PTR,index)
#define PDB0_S(index)                            PDB_S_REG(PDB0_BASE_PTR,index)
#define PDB0_DLY(index,index2)                   PDB_DLY_REG(PDB0_BASE_PTR,index,index2)
#define PDB0_INTC(index)                         PDB_INTC_REG(PDB0_BASE_PTR,index)
#define PDB0_INT(index)                          PDB_INT_REG(PDB0_BASE_PTR,index)
#define PDB0_PODLY(index)                        PDB_PODLY_REG(PDB0_BASE_PTR,index)

//-------------------------- End of Programmable Delay Block (PDB) ----------------------------------------------


//-------------------------- Start of Comparator ----------------------------------------------------------------

#define CMP0_BASE                              (0x40073000u)                 //Absolute address of CMP0_CR0 register
#define CMP1_BASE                              (0x40073008u)

#define CMP0	                               (*(volatile CMP_BASIC_DEF *)CMP0_BASE)
#define CMP1	                               (*(volatile CMP_BASIC_DEF *)CMP1_BASE)

typedef struct
 {
	//uint8 CR0;                                     /**< CMP Control Register 0, offset: 0x0 */
	union
	{
		uint8 BYTE;
		struct
		{
			uint8 HYSTCTR 	:2;					//Comparator hard block hysteresis control
			uint8 RESERVE23 :2;					//Reserved Bits
			uint8 FILTER_CNT:3;					//filter sample count
			uint8 RESERVE7 	:1;

		} BIT;

	} CR0;

	//uint8 CR1;                                     /**< CMP Control Register 1, offset: 0x1 */
	union
	{
		uint8 BYTE;
		struct
		{
			uint8 EN :1;						//Comparator module enable
			uint8 OPE :1;						//Comparator output pin enable
			uint8 COS :1;						//Comparator output select
			uint8 INV :1;						//Comparator output invert
			uint8 PMODE :1;						//Power mode select
			uint8 TRIGM :1;						//Trigger mode enable
			uint8 WE :1;						//Windowing mode enable
			uint8 SE :1;						//Sample enable
		} BIT;

	}CR1;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 FILT_PER :8;					//filter sample period

		} BIT;

	}FPR;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 COUT :1;						//Analog Comparator output
			uint8 CFF :1;						//Analog Comparator flag falling
			uint8 CFR :1;						//Analog Comparator flag rising
			uint8 IEF :1;						//Comparator interrupt enable falling
			uint8 IER :1;						//Comparator  interrupt enable rising
			uint8 Reserved_05 :1;				//Reserved Bit
			uint8 DMAEN :1;						//DMA enable control
			uint8 Reserved_07 :1;				//Reserved Bit

		} BIT;

	} SCR;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 VOSEL :6;						//DAC output voltage select
			uint8 VRSEL :1;						//Supply voltage reference source select
			uint8 DACEN :1;						//DAC enable

		}BIT;

	} DACCR;

	union
	{
		uint8 BYTE;
		struct
		{
			uint8 MSEL :3;						//Minus input mux control
			uint8 PSEL :3;						//Plus  input mux control
			uint8 Reserved_bits :2;

		}BIT;

	}MUXCR;

} CMP_BASIC_DEF;

/* CMP - Register accessors */
#define CMP_CR0_REG(base)                        ((base)->CR0)
#define CMP_CR1_REG(base)                        ((base)->CR1)
#define CMP_FPR_REG(base)                        ((base)->FPR)
#define CMP_SCR_REG(base)                        ((base)->SCR)
#define CMP_DACCR_REG(base)                      ((base)->DACCR)
#define CMP_MUXCR_REG(base)                      ((base)->MUXCR)

/* ----------------------------------------------------------------------------
   -- CMP Register Masks
   ---------------------------------------------------------------------------- */

/* CR0 Bit Fields */
#define CMP_CR0_HYSTCTR_MASK                     0x3u
#define CMP_CR0_HYSTCTR_SHIFT                    0
#define CMP_CR0_HYSTCTR(x)                       (((uint8)(((uint8)(x))<<CMP_CR0_HYSTCTR_SHIFT))&CMP_CR0_HYSTCTR_MASK)
#define CMP_CR0_FILTER_CNT_MASK                  0x70u
#define CMP_CR0_FILTER_CNT_SHIFT                 4
#define CMP_CR0_FILTER_CNT(x)                    (((uint8)(((uint8)(x))<<CMP_CR0_FILTER_CNT_SHIFT))&CMP_CR0_FILTER_CNT_MASK)
/* CR1 Bit Fields */
#define CMP_CR1_EN_MASK                          0x1u
#define CMP_CR1_EN_SHIFT                         0
#define CMP_CR1_OPE_MASK                         0x2u
#define CMP_CR1_OPE_SHIFT                        1
#define CMP_CR1_COS_MASK                         0x4u
#define CMP_CR1_COS_SHIFT                        2
#define CMP_CR1_INV_MASK                         0x8u
#define CMP_CR1_INV_SHIFT                        3
#define CMP_CR1_PMODE_MASK                       0x10u
#define CMP_CR1_PMODE_SHIFT                      4
#define CMP_CR1_TRIGM_MASK                       0x20u
#define CMP_CR1_TRIGM_SHIFT                      5
#define CMP_CR1_WE_MASK                          0x40u
#define CMP_CR1_WE_SHIFT                         6
#define CMP_CR1_SE_MASK                          0x80u
#define CMP_CR1_SE_SHIFT                         7
/* FPR Bit Fields */
#define CMP_FPR_FILT_PER_MASK                    0xFFu
#define CMP_FPR_FILT_PER_SHIFT                   0
#define CMP_FPR_FILT_PER(x)                      (((uint8)(((uint8)(x))<<CMP_FPR_FILT_PER_SHIFT))&CMP_FPR_FILT_PER_MASK)
/* SCR Bit Fields */
#define CMP_SCR_COUT_MASK                        0x1u
#define CMP_SCR_COUT_SHIFT                       0
#define CMP_SCR_CFF_MASK                         0x2u
#define CMP_SCR_CFF_SHIFT                        1
#define CMP_SCR_CFR_MASK                         0x4u
#define CMP_SCR_CFR_SHIFT                        2
#define CMP_SCR_IEF_MASK                         0x8u
#define CMP_SCR_IEF_SHIFT                        3
#define CMP_SCR_IER_MASK                         0x10u
#define CMP_SCR_IER_SHIFT                        4
#define CMP_SCR_DMAEN_MASK                       0x40u
#define CMP_SCR_DMAEN_SHIFT                      6
/* DACCR Bit Fields */
#define CMP_DACCR_VOSEL_MASK                     0x3Fu
#define CMP_DACCR_VOSEL_SHIFT                    0
#define CMP_DACCR_VOSEL(x)                       (((uint8)(((uint8)(x))<<CMP_DACCR_VOSEL_SHIFT))&CMP_DACCR_VOSEL_MASK)
#define CMP_DACCR_VRSEL_MASK                     0x40u
#define CMP_DACCR_VRSEL_SHIFT                    6
#define CMP_DACCR_DACEN_MASK                     0x80u
#define CMP_DACCR_DACEN_SHIFT                    7
/* MUXCR Bit Fields */
#define CMP_MUXCR_MSEL_MASK                      0x7u
#define CMP_MUXCR_MSEL_SHIFT                     0
#define CMP_MUXCR_MSEL(x)                        (((uint8)(((uint8)(x))<<CMP_MUXCR_MSEL_SHIFT))&CMP_MUXCR_MSEL_MASK)
#define CMP_MUXCR_PSEL_MASK                      0x38u
#define CMP_MUXCR_PSEL_SHIFT                     3
#define CMP_MUXCR_PSEL(x)                        (((uint8)(((uint8)(x))<<CMP_MUXCR_PSEL_SHIFT))&CMP_MUXCR_PSEL_MASK)

//-------------------------- End of Comparator  ------------------------------------------------------------

//-------------------------- Start of Low Power Timer ------------------------------------------------------------

#define LPTMR_BASE                              (0x40040000u)                 //Absolute address of LPTMR register

#define LPTMR0	                               (*(volatile LPTMR_DEF *)LPTMR_BASE)

/** LPTMR - Peripheral register structure */
typedef struct
{
  uint32 CSR;                                    /**< Low Power Timer Control Status Register, offset: 0x0 */
  uint32 PSR;                                    /**< Low Power Timer Prescale Register, offset: 0x4 */
  uint32 CMR;                                    /**< Low Power Timer Compare Register, offset: 0x8 */
  uint32 CNR;                                    /**< Low Power Timer Counter Register, offset: 0xC */
} volatile LPTMR_DEF;

/* ----------------------------------------------------------------------------
   -- LPTMR - Register accessor macros
   ---------------------------------------------------------------------------- */

/* LPTMR - Register accessors */
#define LPTMR_CSR_REG(base)                      ((base)->CSR)
#define LPTMR_PSR_REG(base)                      ((base)->PSR)
#define LPTMR_CMR_REG(base)                      ((base)->CMR)
#define LPTMR_CNR_REG(base)                      ((base)->CNR)

/* ----------------------------------------------------------------------------
   -- LPTMR Register Masks
   ---------------------------------------------------------------------------- */
/* CSR Bit Fields */
#define LPTMR_CSR_TEN_MASK                       0x1u
#define LPTMR_CSR_TEN_SHIFT                      0
#define LPTMR_CSR_TMS_MASK                       0x2u
#define LPTMR_CSR_TMS_SHIFT                      1
#define LPTMR_CSR_TFC_MASK                       0x4u
#define LPTMR_CSR_TFC_SHIFT                      2
#define LPTMR_CSR_TPP_MASK                       0x8u
#define LPTMR_CSR_TPP_SHIFT                      3
#define LPTMR_CSR_TPS_MASK                       0x30u
#define LPTMR_CSR_TPS_SHIFT                      4
#define LPTMR_CSR_TPS(x)                         (((uint32)(((uint32)(x))<<LPTMR_CSR_TPS_SHIFT))&LPTMR_CSR_TPS_MASK)
#define LPTMR_CSR_TIE_MASK                       0x40u
#define LPTMR_CSR_TIE_SHIFT                      6
#define LPTMR_CSR_TCF_MASK                       0x80u
#define LPTMR_CSR_TCF_SHIFT                      7
/* PSR Bit Fields */
#define LPTMR_PSR_PCS_MASK                       0x3u
#define LPTMR_PSR_PCS_SHIFT                      0
#define LPTMR_PSR_PCS(x)                         (((uint32)(((uint32)(x))<<LPTMR_PSR_PCS_SHIFT))&LPTMR_PSR_PCS_MASK)
#define LPTMR_PSR_PBYP_MASK                      0x4u
#define LPTMR_PSR_PBYP_SHIFT                     2
#define LPTMR_PSR_PRESCALE_MASK                  0x78u
#define LPTMR_PSR_PRESCALE_SHIFT                 3
#define LPTMR_PSR_PRESCALE(x)                    (((uint32)(((uint32)(x))<<LPTMR_PSR_PRESCALE_SHIFT))&LPTMR_PSR_PRESCALE_MASK)
/* CMR Bit Fields */
#define LPTMR_CMR_COMPARE_MASK                   0xFFFFu
#define LPTMR_CMR_COMPARE_SHIFT                  0
#define LPTMR_CMR_COMPARE(x)                     (((uint32)(((uint32)(x))<<LPTMR_CMR_COMPARE_SHIFT))&LPTMR_CMR_COMPARE_MASK)
/* CNR Bit Fields */
#define LPTMR_CNR_COUNTER_MASK                   0xFFFFu
#define LPTMR_CNR_COUNTER_SHIFT                  0
#define LPTMR_CNR_COUNTER(x)                     (((uint32)(((uint32)(x))<<LPTMR_CNR_COUNTER_SHIFT))&LPTMR_CNR_COUNTER_MASK)

/* LPTMR - Register instance definitions */
/* LPTMR0 */
#define LPTMR0_CSR                               LPTMR_CSR_REG(LPTMR0_BASE_PTR)
#define LPTMR0_PSR                               LPTMR_PSR_REG(LPTMR0_BASE_PTR)
#define LPTMR0_CMR                               LPTMR_CMR_REG(LPTMR0_BASE_PTR)
#define LPTMR0_CNR                               LPTMR_CNR_REG(LPTMR0_BASE_PTR)

//-------------------------- End of LPTMR (Low Power Timer) ----------------------------------------------


#pragma pack(pop)

#endif

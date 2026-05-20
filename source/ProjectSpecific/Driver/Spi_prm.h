/**
 *  @file
 *  @brief    The macro value in this file are configurable as per the user requirement.
 *  @details  It contains data that can be modified by the user in order to configure the Spi Tx, Rx buffer size,
 *              Spi Clock polarity, Baud rate, interrupt priority etc
 * <pre>
 * The Spi0 and Spi1 is available in the below port pins in MKV3x Controllers
 * ------------------------------------------------------         ----------------------------------------
 * |                    MKV03x_FAMILY                   |        |              MKV03x_FAMILY            |
 * |----------------------------------------------------|        |---------------------------------------|
 * |SPI com      | REMAP 0 | REMAP 1 | REMAP 2 |REMAP 3 |        |SPI com     |REMAP 0| REMAP 1 |   NA   |
 * |-------------|---------|---------|---------|--------|        |------------|-------|---------|--------|
 * |      | MOSI |PORTE 19 |PORTA 16 | PORTC 6 |PORTD 2 |        |     | MOSI |PORTE 1| PORTB 16|PORTD 7 |
 * |      |------|---------|---------|---------|--------|        |     |------|-------|---------|--------|
 * |      | MISO |PORTE 18 |PORTA 17 | PORTC 7 |PORTD 3 |        |     | MISO |PORTE 3| PORTB 17|PORTD 6 |
 * | SPI0 |------|---------|---------|---------|--------|        |SPI1 |------|-------|---------|--------|
 * |      | SCK  |PORTE 17 |PORTA 15 | PORTC 5 |PORTD 1 |        |     | SCK  |PORTE 2| PORTB 1 |PORTD 5 |
 * |      |------|--------------------------------------|        |     |------|--------------------------|
 * |      | SS   |          UNUSED                      |        |     | SS   |         UNUSED           |
 * -----------------------------------------------------          --------------------------------------- </pre>
 *
 *  $Header: Spi_prm.h 1.5 2014/12/11 21:45:05IST Natak Rahul (NATAKRB) Exp  $
 *
 *   @copyright  Copyright 2007-2012-$Date: 2014/12/11 21:45:05IST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef SPI_PRM_H_
	#define SPI_PRM_H_
#include "C_Extensions.h"
#include "Mci_prm.h"
//=====================================================================================================================
//-------------------------------------- SPI1 and SPI2-------------------------------------------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 *    @brief  - This macro Enables or Disables the Spi modules.
 *    @details- The macro must be Enabled before using in the application.
 *
 *    @param  - ENABLED(1)  - Which Enables the Specified Spi
 *              DISABLED(0) – Which Disables the Specified Spi
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI0_FEATURE        ENABLED"  //This Enables the Spi2 Feature\n
 *              "#define SPI0_FEATURE        DISABLED" // This Disables the Spi2 Feature\n
 *              "#define SPI1_FEATURE        ENABLED"  //This Enables the Spi1 Feature\n
 *              "#define SPI1_FEATURE        DISABLED" // This Disables the Spi1 Feature\n

 */
//=====================================================================================================================
#define SPI0_FEATURE        ENABLED
#define SPI1_FEATURE        DISABLED

//=====================================================================================================================
/**
 *    @brief  - This macro defines the SPI mode either Full duplex (Tx and Rx are ON) or Half duplex (Tx only ON).
 *    @details- If the SPI0_TX_RX_MODE is Enabled then MISO pin could be free to use as Gpio pin for other application.
 *    @param  - DISABLED(0) –  Half duplex (Tx only ON)
 *              ENABLED(1) -   Full duplex (Tx and Rx are ON)
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI0_TX_RX_MODE        DISABLED" // Only Transmitter ON
 *              "#define SPI0_TX_RX_MODE        ENABLED"  // Both Transmitter and Receiver are ON\n
 */
//=====================================================================================================================
#define SPI0_TX_RX_MODE        ENABLED
#define SPI1_TX_RX_MODE        ENABLED

//=====================================================================================================================
/**
 *    @brief  - This macro defines the Tx and Rx FIFO Buffer size.
 *    @details- Tx and Rx FIFO buffer size should be big enough so that the application can insert the data into it.
 *              Rx buffer is not allocated if the SPIx_TX_ONLY mode is Enabled.
 *
 *    @param  - n    Where n=1,2,3…
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI0_BUFFER_SIZE         5"  // The Spi Tx and Rx buffer size is 5\n
 *              "#define SPI0_BUFFER_SIZE         10" // The Spi Tx and Rx buffer size is 10
 */
//=====================================================================================================================
#define SPI0_BUFFER_SIZE     50
#define SPI1_BUFFER_SIZE     50

//=====================================================================================================================
/**
 *    @brief  - This macro remaps the Spi0 pins.
 *    @details- The SPI0 is available in the below port pins as per the Remap configuration value
 * <pre>
 * The Spi0 is available in the below port pins in MKE02x/MKE04x/MKE06x Controllers
 * ------------------------------------------------------
 * |                    MKV03x_FAMILY                   |
 * |----------------------------------------------------|
 * |SPI com      | REMAP 0 | REMAP 1 | REMAP 2 |REMAP 3 |
 * |-------------|---------|---------|---------|--------|
 * |      | MOSI |PORTE 19 |PORTA 16 | PORTC 6 |PORTD 2 |
 * |      |------|---------|---------|---------|--------|
 * |      | MISO |PORTE 18 |PORTA 17 | PORTC 7 |PORTD 3 |
 * | SPI0 |------|---------|---------|---------|--------|
 * |      | SCK  |PORTE 17 |PORTA 15 | PORTC 5 |PORTD 1 |
 * |      |------|--------------------------------------|
 * |      | SS   |          UNUSED                      |
 * -----------------------------------------------------   </pre>
 *
 *    @param  - 0/1
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI0_REMAP                    0"   // MOSI->PB3, MISO->PB4, SCK->PB2\n
 *              "#define SPI0_REMAP                    1"   // MOSI->PE1, MISO->PE2, SCK->PE0
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
	#define SPI0_REMAP          	2
#elif(WINDY_INTERNATION_BOARD == 1)
	#define SPI0_REMAP          	0
#else
	#error "Need to define a board"
#endif

//=====================================================================================================================
/**
 *    @brief  - This macro remaps the Spi1 pins.
 *    @details- The SPI1 is available in the below port pins as per the Remap configuration value
 * <pre>
 * The Spi1 is available in the below port pins in MKE02x/MKE04x/MKE06x  Controllers
 *  ----------------------------------------
 * |              MKV03x_FAMILY            |
 * |---------------------------------------|
 * |SPI com     |REMAP 0| REMAP 1 |   NA   |
 * |------------|-------|---------|--------|
 * |     | MOSI |PORTE 1| PORTB 16|PORTD 7 |
 * |     |------|-------|---------|--------|
 * |     | MISO |PORTE 3| PORTB 17|PORTD 6 |
 * |SPI1 |------|-------|---------|--------|
 * |     | SCK  |PORTE 2| PORTB 1 |PORTD 5 |
 * |     |------|--------------------------|
 * |     | SS   |         UNUSED           |
 *  --------------------------------------- <</pre>
 *
 *    @param  - 0/1
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI1_REMAP                    0"   // MOSI->PD1, MISO->PD2, SCK->PD0\n
 *              "#define SPI1_REMAP                    1"   // MOSI->PG5, MISO->PG6, SCK->PG4
 */
//=====================================================================================================================
#define SPI1_REMAP         	 0

//=====================================================================================================================
/**
 *    @brief  - This macro defines Spi bit rate.
 *    @details-
 *  ---------------------------------------------------------------------------------------------------------------
 *</pre>
 *
 *  Bus clock is the source of SPI baud rate generator
 *	For example - To set 10Mbps speed
 *				  SCK Baudrate = (fp/PBR) * [(1+DBR)/BR]
 *				  where,
 *				  fp = Bus Clock frequency
 *				  PBR = BaudRate Prescaler
 *				  BR  = Baudrate Scaler
 *				  DBR = Double BaudRate
 *
 *				  SCK Baudrate = (fp/PBR) * [(1+DBR)/BR]
 *
 *				               = (20000000/2)*[(1+1)/2]
 *				               = 10000000
 *				               = 10Mb/s
 *
 *				  fp = 20Mhz
 *				  PBR = 0 i.e Prescaler value is '2'
 *				  BR  = 0 i.e Scaler value is '2'
 *
 *   Table: Double BaudRate                   Table: BaudRate PreScaler             Table: BaudRate Scaler
 *   ----------------------------------        -------------------------------       -------------------------------
 *	|DBR  | CPHA | PBR  | SCK DutyCycle|      | PBR  |BaudRate PreScaler Value|     |  BR     |BaudRate Scaler Value|
 *	|-------------------|--------------|      |------|------------------------|     |---------|---------------------|
 *	| 0	  | any  | any  |     50/50    |      | 0000 |   2                    |     |  0000	  |   2                 |
 *	| 1   |  0   |  00  |     50/50    |      | 0001 |   3                    |     |  0001   |   4                 |
 *	| 1   |  0   |  00  |     36/66    |      | 0010 |   5                    |     |  0010   |   6                 |
 *	| 1   |  0   |  00  |     40/60    |      | 0011 |   7                    |     |  0011   |   8                 |
 *	| 1   |  0   |  00  |     43/57    |       -------------------------------      |  0100   |   16                |
 *	| 1   |  1   |  01  |     50/50    |                                            |  0101   |   32                |
 *	| 0   |  1   |  01  |     66/33    |                                            |  0110   |   64                |
 *	| 1   |  1   |  01  |     60/40    |                                            |  0111   |   128               |
 *	| 0   |  1   |  01  |     57/43    |                                            |  1000   |   256               |
 *   ----------------------------------                                             |  1001   |   512               |
 *                                                                                  |  1010   |   1024              |
 *                                                                                  |  1011   |   2048              |
 *                                                                                  |  1100   |   4096              |
 *                                                                                  |  1101   |   8192              |
 *                                                                                  |  1110   |   16384             |
 *                                                                                  |  1111   |   32768             |
 *                                                                                   -------------------------------
 *   PBR: SPI_x_BAUDRATE_PRESCALER
 *   BR : SPI_x_BAUDRATE_SCALER
 *   DBR: SPI_x_DOUBLE_BAUDRATE
 *
 *    @param  - 0/1/2/3/4/5/6/7
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_0_BAUDRATE_PRESCALER        4" //Fill PBR with 4 i.e Busclk/5     \n
 *              "#define SPI_0_BAUDRATE_SCALER           0" //Fill  BR with 0 i.e SPPR/2
 */
//=====================================================================================================================
#define SPI_0_BAUDRATE_PRESCALER         1
#define SPI_0_BAUDRATE_SCALER            0
#define SPI_0_DOUBLE_BAUDRATE            1

#define SPI_1_BAUDRATE_PRESCALER         0
#define SPI_1_BAUDRATE_SCALER            2
#define SPI_1_DOUBLE_BAUDRATE            1
//=====================================================================================================================
/**
 *    @brief  - This macro defines the clock polarity when clock stops (i.e. Spi device is idle).
 *
 *    @param  - 0 - Low (L) Clock line is in Low state when Spi becomes idle.
 *              1 – High (H) Clock line is in High state when Spi becomes idle.
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI0_POL       0"  //Clock line is in Low state when Spi1 becomes idle\n
 *              "#define SPI0_POL       1"  //Clock line is in High state when Spi2 becomes idle
 */
//=====================================================================================================================
#define SPI0_POL               1
#define SPI1_POL               0

//=====================================================================================================================
/**
 *    @brief  - This macro defines the SPI clock phase select.
 *    @details- This macro selects one of two clock format for different kinds of synchronous serial peripheral devices
 *    If CPHA (clock phase) bit is set, the second edge on the SCK pin (falling edge if the CPOL bit is reset,
 *              rising edge if the CPOL bit is set) is the MSBit capture strobe. Data is latched on the occurrence of
 *              the first clock transition.
 *              If CPHA bit is reset, the first edge on the SCK pin (falling edge if CPOL bit is set, rising edge if CPOL
 *              bit is reset) is the MSBit capture strobe. Data is latched on the occurrence of the second clock transition.
 *
 *    @param  - 1 - First edge on SCK occurs at the middle of the first cycle of a data transfer.
 *              0 - First edge on SCK occurs at the start of the first cycle of a data transfer.
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_0_CPHA_EDGE      0" // The first clock transition at the middle of first cycle of data transfer\n
 *              "#define SPI_1_CPHA_EDGE      1" // The first clock transition at the start of first cycle of data transfer
 */
//=====================================================================================================================
#define SPI0_CPHA_EDGE         1
#define SPI1_CPHA_EDGE         0

//=====================================================================================================================
/**
 *    @brief  - This macro Defines the SPI data transfer order MSB first or LSB first.
 *
 *    @param  - 1 - LSB first
 *              0 - MSB first
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_0_LSBFIRST      0"  //Data order is MSB first\n
 *              "#define SPI_1_LSBFIRST      1"  //Data order is LSB first
 */
//=====================================================================================================================
#define SPI0_LSBFIRST          0
#define SPI1_LSBFIRST          0

//=====================================================================================================================
/**
 *    @brief  - This macro Enables or Disables SPI Transmission by Reference or by buffer.
 *    @details- When this feature is Disabled the Spi Driver Copies the data byte (and String) sent by the Application
 *              and stores it in the Tx Buffer and initiates the Spi Communication.
 *              But when this feature is Enabled the Spi Driver uses the Address of the application buffer
 *              as reference and initiates the Spi Communication.
 *              Advantage - This feature saves memory(by not defining the Tx Buffer)
 *                          As well as speed of execution is also fast.
 *              Disadvantage – The Application buffer value (reference buffer) should not be altered
 *                             until the current transmission is successful.
 *
 *    @param  - ENABLED(1) - Which Enables the SPI Transmission by Reference.
 *              DISABLED(0) – Which Disables the SPI Transmission by Reference.
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_TX_BY_REFERENCE  ENABLED" //This Enables the SPI0 Transmission by Reference\n
 *              "#define SPI_1_TX_BY_REFERENCE  DISABLED" // This Disables the SPI0 Transmission by Reference
 */
//=====================================================================================================================
#define SPI_0_TX_BY_REFERENCE      DISABLED
#define SPI_1_TX_BY_REFERENCE      DISABLED

////=====================================================================================================================
///**
// *    @brief  - Call back that is called every time the Spi starts a write operation
// *
// *    @param  - Defines the port pin which state is high in every Spi write operation
// *
// *              This features uses the Spi interrupt to indicate the end of transmission.
// *              1. Include #include "Spi.h" into Interrupt_prm.h file
// *              2. If needed change the Interrupt priority by default it has the highest priority
// *                  For SPI1
// *                  #define SPI_1_INTERRUPT_PREEMPTION_PRIORITY     0
// *                  #define SPI_1_INTERRUPT_PRIORITY                0
// *
// *                  For SPI2
// *                  #define SPI_2_INTERRUPT_PREEMPTION_PRIORITY     0
// *                  #define SPI_2_INTERRUPT_PRIORITY                0
// *
// *    @note   - Following is the Examples\n
// *              "#define SPI1_START_CALLBACK()  {GPIO__PIN_COFIG_O_PUSHPULL(PORTD,15);GPIO__PIN_SET(PORTD, 15);};"
// */
////=====================================================================================================================
////#define SPI1_START_CALLBACK()  {GPIO__PIN_COFIG_O_PUSHPULL(PORTD,15);GPIO__PIN_SET(PORTD, 15);};
////#define SPI2_START_CALLBACK()  {GPIO__PIN_COFIG_O_PUSHPULL(PORTC,2);GPIO__PIN_SET(PORTC, 2);};
//
////=====================================================================================================================
///**
// *    @brief  - Call back that is called every time when the Spi ends a transmission operation
// *
// *    @param  - Defines the port pin which state is Low in every Spi transmission operation
// *
// *    @note   - Following is the Examples\n
// *              "#define SPI1_END_CALLBACK()  {GPIO__PIN_CLR(PORTD,15);}"
// */
////=====================================================================================================================
////#define SPI1_END_CALLBACK()  {GPIO__PIN_CLR(PORTD,15);}
////#define SPI2_END_CALLBACK()  {GPIO__PIN_CLR(PORTC,2);}
//
//=====================================================================================================================
/**
 *    @brief  - This macro defines the SPI interrupt group priority.
 *    @details- Consists of 4 interrupt groups and each group consists of 4 sub-priorities.
 *              Only the group priority determines preemption of interrupt exceptions.
 *              When the processor is executing an interrupt exception handler, another interrupt with the
 *              same group priority as the interrupt being handled does not preempt the handler.
 *              If multiple pending interrupts have the same group priority, the sub-priority field determines
 *              the order in which they are processed. If multiple pending interrupts have the same group priority
 *              and sub-priority, the interrupt with the lowest IRQ number is processed first.
 *              The lower the Group priority the greater the priority of the corresponding interrupt group.
 *              The lower the sub-priority the greater the priorities of the corresponding interrupt within the group.
 *
 *             Note - The Interrupt priority is valid only when chip select feature is enable
 *    @param  - 0/1/2/3
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_INTERRUPT_PREEMPTION_PRIORITY 0"  //The Spi0 is in interrupt group 0
 *                                                               //having highest priority\n
 *              "#define SPI_2_INTERRUPT_PREEMPTION_PRIORITY 1"  //The Spi1 is in interrupt
 *                                                               //group 1 having priority less than the Spi0
 */
//=====================================================================================================================
#define SPI0_INTERRUPT_PREEMPTION_PRIORITY    0
#define SPI1_INTERRUPT_PREEMPTION_PRIORITY    2

//=====================================================================================================================
/**
 *    @brief  - This macro defines the SPI interrupt sub-priority.
 *    @details- Each interrupt group consists of 4 interrupt sub-priorities.
 *              If multiple pending interrupts have the same group priority, the sub-priority field determines the
 *              order in which they are processed. If multiple pending interrupts have the same group priority
 *              and sub-priority, the interrupt with the lowest IRQ number is processed first.
 *              The lower the sub-priority the greater the priorities of the corresponding interrupt within the group.
 *
 *             Note - The Interrupt priority is valid only when chip select feature is enable
 *
 *    @param  - 0/1/2/3
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_INTERRUPT_PRIORITY 0"  //The Spi0 sub-priority is 0 having highest
 *                                                    //priority within the group\n
 *              "#define SPI_2_INTERRUPT_PRIORITY 1"   //The Spi1 sub-priority is 1
 */
//=====================================================================================================================
#define SPI0_INTERRUPT_SUB_PRIORITY    0
#define SPI1_INTERRUPT_SUB_PRIORITY    2


//=====================================================================================================================
/**
 *    @brief  - This macro defines the SPI CSSCK.
 *    @details-
 *
 *             Note - The Interrupt priority is valid only when chip select feature is enable
 *
 *    @param  - 0/1/2/3
 *
 *    @note   -
 */
//=====================================================================================================================
#define SPI0_CSSCK    0
#define SPI1_CSSCK    0


//=====================================================================================================================
/**
 *    @brief  - This macro to enable or disbale SPI with or without DMA feature
 *    @details-
 *    #define DMA_FEATURE_FOR_SPI			ENABLED //SPI with DMA
 *    #define DMA_FEATURE_FOR_SPI			DIABLED //Only SPI
 */
//=====================================================================================================================

#define DMA_FEATURE_FOR_SPI			DISABLED

#endif // SPI_PRM_H_

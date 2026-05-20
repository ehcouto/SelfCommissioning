/**
 *  @file
 *  @brief    Uart GDM module for Kinetis MKV3x family implements the Uart Standard API (Application Programming Interface).
 *  @details  This module provides the interface to the application such as initialization of Uart,\n
 *              transmission and reception of serial data.
 *
 * <pre>
 * The UART0,UART1 and UART2 are available in the below port pins in MKV3x Controllers
 * ---------------------------------------------------------------------------------
 * |                           MKV30_FAMILY                               |         |
 * |----------------------------------------------------------------------|-------- |
 * |         Uart COM            |  REMAP 0 | REMAP 1 | REMAP 2 | REMAP 3 | REMAP 4 |
 * |-----------------------------|----------|---------|---------|---------|---------|
 * |    COM0     |   Tx Pin      | PORTA 2  | PORTA 14| PORTB 1 | PORTD 6 | PORTB 17|
 * |   (UART0)   |---------------|----------|---------|---------|---------|-------- |
 * |             |   Rx Pin      | PORTA 1  | PORTA 15| PORTB 0 | PORTD 7 | PORTB 16|
 * |-------------|---------------|----------|---------|--------- -------------------
 * |    COM1     |   Tx Pin      | PORTC 4  | PORTE 16| PORTE 0 |
 * |   (UART1)   |---------------|----------|---------|---------
 * |             |   Rx pin      | PORTC 3  | PORTE 17| PORTE 1 |
 * |-------------|---------------|----------|---------|---------
 * |    COM2     |   Tx Pin      | PORTD 3  |
 * |   (UART2)   |---------------|----------|
 * |             |   Rx pin      | PORTD 2  |
 * ------------------------------------------ </pre>
 *  * <pre>
 * The UART0,UART1 and UART2 are available in the below port pins in MKV3x Controllers
 * ---------------------------------------------------------------------------------
 * |                           MKV31_FAMILY                               |         |
 * |----------------------------------------------------------------------|-------- |
 * |         Uart COM            |  REMAP 0 | REMAP 1 | REMAP 2 | REMAP 3 | REMAP 4 |
 * |-----------------------------|----------|---------|---------|---------|---------|
 * |    COM0     |   Tx Pin      | PORTA 2  | PORTA 14| PORTB 1 | PORTD 6 | PORTB 17|
 * |   (UART0)   |---------------|----------|---------|---------|---------|-------- |
 * |             |   Rx Pin      | PORTA 1  | PORTA 15| PORTB 0 | PORTD 7 | PORTB 16|
 * |-------------|---------------|----------|---------|--------- -------------------
 * |    COM1     |   Tx Pin      | PORTC 4  | PORTE 0 |
 * |   (UART1)   |---------------|----------|---------|
 * |             |   Rx pin      | PORTC 3  | PORTE 1 |
 * |-------------|---------------|----------|---------|
 * |    COM2     |   Tx Pin      | PORTD 3  | PORTE 16|
 * |   (UART2)   |---------------|----------|---------|
 * |             |   Rx pin      | PORTD 2  | PORTE 17|
 * --------------------------------------------------- </pre>
 *
 * @note: UART0 & UART1: Configures on System Clock
 *                UART2: Configured on Bus Clock
 *
 *  $Header: Uart.c $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Micro.h"
#include "Gpio.h"
#include "Uart.h"

//=====================================================================================================================
//Compiler directive
//=====================================================================================================================
#ifndef UART_POLLING_MODE
    #define UART_POLLING_MODE				DISABLED
#endif

//! UART0 means COM0
#ifndef UART0_FEATURE
    #define UART0_FEATURE					ENABLED
#endif

//! UART1 means COM1
#ifndef UART1_FEATURE
    #define UART1_FEATURE					DISABLED
#endif

//! UART2 means COM2
#ifndef UART2_FEATURE
    #define UART2_FEATURE					DISABLED
#endif


//! Enable the Variables, Constants & Defines only if the either one of the UART is enabled
#if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
    #if (UART0_FEATURE == ENABLED)
        #ifndef UART0_TX_BUF_SIZE
            #define UART0_TX_BUF_SIZE						22
        #elif (UART0_TX_BUF_SIZE > 255 )
            #error " UART0_TX_BUF_SIZE should not be more than 255 in Uart_prm.h file"
        #endif

        #ifndef UART0_RX_BUF_SIZE
            #define UART0_RX_BUF_SIZE						22
        #elif (UART0_RX_BUF_SIZE > 255 )
            #error " UART0_RX_BUF_SIZE should not be more than 255 in Uart_prm.h file"
        #endif

        #ifndef COM0_PREEMPTION_PRIORITY
            #define COM0_PREEMPTION_PRIORITY				0
        #elif (COM0_PREEMPTION_PRIORITY > 3)
            #error " COM0_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
        #endif
    #endif

    #if (UART1_FEATURE == ENABLED)
        #ifndef UART1_TX_BUF_SIZE
            #define UART1_TX_BUF_SIZE						22
        #elif (UART1_TX_BUF_SIZE > 255 )
            #error " UART1_TX_BUF_SIZE should not be more than 255 in Uart_prm.h file"
        #endif

        #ifndef UART1_RX_BUF_SIZE
            #define UART1_RX_BUF_SIZE						22
        #elif (UART1_RX_BUF_SIZE > 255 )
            #error " UART1_RX_BUF_SIZE should not be more than 255 in Uart_prm.h file"
        #endif

        #ifndef COM1_PREEMPTION_PRIORITY
            #define COM1_PREEMPTION_PRIORITY                0
        #elif (COM1_PREEMPTION_PRIORITY > 3)
            #error " COM1_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
        #endif
    #endif

    #if (UART2_FEATURE == ENABLED)
        #ifndef UART2_TX_BUF_SIZE
            #define UART2_TX_BUF_SIZE						22
        #elif (UART2_TX_BUF_SIZE > 255 )
            #error " UART2_TX_BUF_SIZE should not be more than 255 in Uart_prm.h file"
        #endif

        #ifndef UART2_RX_BUF_SIZE
            #define UART2_RX_BUF_SIZE						22
        #elif (UART2_RX_BUF_SIZE > 255 )
            #error " UART2_RX_BUF_SIZE should not be more than 255 in Uart_prm.h file"
        #endif

        #ifndef COM2_PREEMPTION_PRIORITY
            #define COM2_PREEMPTION_PRIORITY                0
        #elif (COM2_PREEMPTION_PRIORITY > 3)
            #error " COM2_PREEMPTION_PRIORITY should be either 0,1,2,3. Where 0 is the Highest priority and 3 is the lowest priority."
        #endif
    #endif

#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! UART CR1 register clear Mask
#define CR1_CLEAR_MASK                          			((uint8)(UART_C1_LOOPS_MASK| UART_C1_M_MASK |  \
                                                                     UART_C1_WAKE_MASK |UART_C1_ILT_MASK |  \
                                                                     UART_C1_PE_MASK |UART_C1_PT_MASK))

//! UART CR2 register clock bits clear Mask
#define CR2_CLEAR_MASK                          			((uint8)(UART_C2_TIE_MASK| UART_C2_TCIE_MASK |  \
																	 UART_C2_RIE_MASK| UART_C2_ILIE_MASK |  \
                                                                     UART_C2_TE_MASK |UART_C2_RE_MASK |  \
                                                                     UART_C2_RWU_MASK |UART_C2_SBK_MASK))

//! UART CR3 register clear Mask
#define CR3_CLEAR_MASK                          			((uint8)( UART_C3_TXINV_MASK | UART_C3_ORIE_MASK | \
                                                                      UART_C3_NEIE_MASK | UART_C3_FEIE_MASK | \
                                                                      UART_C3_PEIE_MASK))

//UART Stop bit
#define  USART_CR2_1_STOPBIT                    			((uint8)0x00)        //One Stop bit
#define  USART_CR2_2_STOPBIT                    			((uint8)0x20)        //Two Stop bit

#if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED)||(UART4_FEATURE == ENABLED))
    static const uint32 BDRATE[] = {1200,2400,4800,9600,19200,38400,57600,115200,230400,9600,9600,300,600};

    static UART_BASIC_DEF * const UART_PTR[] =
    {
        #if (UART0_FEATURE == ENABLED)
            &UART0,
        #else
            NULL,
        #endif

        #if (UART1_FEATURE == ENABLED)
            &UART1,
        #else
            NULL,
        #endif

        #if (UART2_FEATURE == ENABLED)
            &UART2,
        #else
            NULL,
        #endif

     };

    typedef struct
    {
        uint16 parity_err      :1;
        uint16 frame_err       :1;
        uint16 noise_err       :1;
        uint16 overrun_err     :1;
        uint16 incomming_break :1;
        uint16 data            :8;
        uint16 unused12_to_15  :3;
    } DATA_REGISTER_TYPE;

    #if (UART0_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART0_Rx_Buffer[UART0_RX_BUF_SIZE];
        static uint8 UART0_Tx_Buffer[UART0_TX_BUF_SIZE];
        static uint8 UART0_Tx_Write_Index;
        static uint8 UART0_Tx_Read_Index;
        static uint8 UART0_Rx_Write_Index;
        static uint8 UART0_Rx_Read_Index;
        static uint8 UART0_Break;
        static DATA_REGISTER_TYPE UART0_LastData;
    #endif

    #if (UART1_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART1_Rx_Buffer[UART1_RX_BUF_SIZE];
        static uint8 UART1_Tx_Buffer[UART1_TX_BUF_SIZE];
        static uint8 UART1_Tx_Write_Index;
        static uint8 UART1_Tx_Read_Index;
        static uint8 UART1_Rx_Write_Index;
        static uint8 UART1_Rx_Read_Index;
        static uint8 UART1_Break;
        static DATA_REGISTER_TYPE  UART1_LastData;
    #endif

    #if (UART2_FEATURE == ENABLED)
        static DATA_REGISTER_TYPE UART2_Rx_Buffer[UART2_RX_BUF_SIZE];
        static uint8 UART2_Tx_Buffer[UART2_TX_BUF_SIZE];
        static uint8 UART2_Tx_Write_Index;
        static uint8 UART2_Tx_Read_Index;
        static uint8 UART2_Rx_Write_Index;
        static uint8 UART2_Rx_Read_Index;
        static uint8 UART2_Break;
        static DATA_REGISTER_TYPE UART2_LastData;
    #endif

#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes a specific Uart with given configuration such as baud rate (i.e. speed),\n
 *              stop bits and parity. It also initializes the Uart module variables.
 *    @details  Before using Uart module this method should be called to initialize specific Uart.
 *    @param    uart    : Defines the Uart COM port
 *    @param    speed   : Defines the baud rate for Uart
 *    @param    stops   : Defines the stop bit for Uart
 *    @param    parity  : Defines the parity bit for Uart
 *    @return   TRUE (1)    :  If the Uart is successfully initialized.
 *              FALSE (0)   : Failed to initialize the Uart.
 *
 *    @image    html    Uart__Initialize.jpg
 */
BOOL_TYPE Uart__Initialize(UART_ENUM_DEF uart, UART_SPEED_DEF speed,UART_STOP_DEF stops, UART_PARITY_DEF parity)
{
#if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
	uint8 u8tmpreg;
	uint16 u16tmpreg;
	uint32 busclock;
	BOOL_TYPE response;
	UART_BASIC_DEF * com;

	u8tmpreg = 0;
	u16tmpreg = 0;
	response = FALSE;
	com = NULL;

	if (uart < COM3)
	{
		com = UART_PTR[uart];

		if (com != NULL)
		{
			if (speed <= (B230400))
			{
				Uart__Clear(uart);

			#if(UART0_FEATURE == ENABLED)
				if (uart == COM0)
				{
					SIM.SCGC4 |= SIM_SCGC4_UART0_MASK;											//Clock Enabled for UART0 module
					#if (UART0_REMAP == 0)														//Configure the Uart0 Tx and Rx port-pin
						PCIPORTA.PCR[2] = PORT_PCR_MUX(2);										//PTE0 pin - UART Tx
						PCIPORTA.PCR[1] = PORT_PCR_MUX(2);										//PTE1 pin - UART Rx
					#elif(UART0_REMAP == 1)
						PCIPORTA.PCR[14] = PORT_PCR_MUX(3);										//PTE0 pin - UART Tx
						PCIPORTA.PCR[15] = PORT_PCR_MUX(3);										//PTE1 pin - UART Rx
					#elif(UART0_REMAP == 2)
						PCIPORTB.PCR[1] = PORT_PCR_MUX(7);										//PTB1 pin - UART Tx
						PCIPORTB.PCR[0] = PORT_PCR_MUX(7);										//PTB0 pin - UART Rx
                    #elif(UART0_REMAP == 3)
                        PCIPORTD.PCR[7] = PORT_PCR_MUX(3);                                      //PTD6 pin - UART Tx
                        PCIPORTD.PCR[6] = PORT_PCR_MUX(3);                                      //PTD7 pin - UART Rx
                	#else
						PCIPORTB.PCR[17] = PORT_PCR_MUX(3);										//PTB17 pin - UART Tx
						PCIPORTB.PCR[16] = PORT_PCR_MUX(3);										//PTB16 pin - UART Rx
				#endif

				}
			#endif

						#if(UART1_FEATURE == ENABLED)
							if(uart == COM1)
							{
								SIM.SCGC4 |= SIM_SCGC4_UART1_MASK;											//Clock Enabled for UART1 module
								#if (UART1_REMAP == 0)														//Configure UART1 Tx and Rx port-pin
									PCIPORTC.PCR[4] = PORT_PCR_MUX(3);										//PTE0 pin - UART Tx
									PCIPORTC.PCR[3] = PORT_PCR_MUX(3);										//PTE1 pin - UART Rx
							   #elif(UART1_REMAP == 1) || (UC_MKV30_H_ == ENABLED)
									PCIPORTE.PCR[16] = PORT_PCR_MUX(3);										//PTE16 pin - UART Tx
									PCIPORTE.PCR[17] = PORT_PCR_MUX(3);										//PTE17 pin - UART Rx
								#else
									PCIPORTE.PCR[0] = PORT_PCR_MUX(3);										//PTE0 pin - UART Tx
									PCIPORTE.PCR[1] = PORT_PCR_MUX(3);										//PTE1 pin - UART Rx
								#endif
							}
						#endif

						#if(UART2_FEATURE == ENABLED)
							if(uart == COM2)
							{
								SIM_CLK.SIM_SCGC |= SIM_SCGC_UART2_MASK;									//Clock Enabled for UART2 module
								#if (UART2_REMAP == 0)
									PCIPORTD.PCR[3] = PORT_PCR_MUX(3);										//PTD3 pin - UART Tx
									PCIPORTD.PCR[2] = PORT_PCR_MUX(3);										//PTD2 pin - UART Rx
                                #elif(UART2_REMAP == 1) || (UC_MKV31_H_ == ENABLED)
		                            PCIPORTE.PCR[16] = PORT_PCR_MUX(3);										//PTE16 pin - UART Tx
		                            PCIPORTE.PCR[17] = PORT_PCR_MUX(3);
								#endif

							}
						#endif



				//Make sure that the transmitter and receiver are disabled while changing the settings
				com->UART_C2 = (uint8) ~(CR2_CLEAR_MASK);

				//---------------------------- UART C1 Configuration -----------------------
				u8tmpreg = com->UART_C1;
				u8tmpreg &= (uint8) ~(CR1_CLEAR_MASK);      // Clear M, PCE, PS, TE and RE bits

				// Configure the UART Word Length, Parity and mode -----------------------
				// Set the M bits When the parity is ON
				// Set PCE(Parity Control) and PS(Parity selection) as per the request
				// Over sampling by 16 method is selected
				if (parity == UART_PARITY_EVEN)
				{
					u8tmpreg |= (UART_C1_PE_MASK | UART_C1_M_MASK);
				}
				else if (parity == UART_PARITY_ODD)
				{
					u8tmpreg |= (UART_C1_PE_MASK | UART_C1_M_MASK | UART_C1_PT_MASK);
				}

				com->UART_C1 = u8tmpreg;                                  		// Write to USART CR1
				com->UART_C3 = 0x00;                                      		//UART C3 Configuration
				//---------------------------- UART BRRH Configuration ----------------------
				u8tmpreg = com->UART_BRRH;
				// Clear STOP[13:12] bits
				u8tmpreg &=	(uint32) ~((uint32) UART_BDH_SBNS_MASK);

				// Configure the UART Stop Bits ------------
				// Set STOP bit according to UART_StopBits value i.e SBSN bit value of BRR register
				if (stops == UART_STOP_BIT_2)
				{
					u8tmpreg |= (uint32) USART_CR2_2_STOPBIT;             		// 1 Stop Bit by default
				}

				com->UART_BRRH = u8tmpreg;                                		// Write Stop bit to UART CR2
				//---------------------------- UART BRRH & BRRL Configuration ------------------

				//Baudrate = UART Module Clock/(SBR[12:0] * 16)
				//         Kinteis - Calculate baud settings
				//         u16Sbr = (((u32SysClk)>>4) + (u32Baud>>1))/u32Baud;

				if((UART0_FEATURE == ENABLED)|(UART1_FEATURE == ENABLED))
				{
					busclock = Micro__GetClock()->SYSCLK_Frequency;			//System clock: UART0 & UART1
				}
				else
				{
					busclock = Micro__GetClock()->BUSCLK_Frequency;			//System clock: UART2
				}

				u16tmpreg = ((busclock) / (BDRATE[speed] * 16));
				u8tmpreg = com->UART_BRRH & ~(UART_BDH_SBR_MASK);            	//Save current value of UART0 BDH except for the SBR field
				com->UART_BRRH = u8tmpreg | (u16tmpreg >> 8);                	//Write Calculated Baud Rate in the Baudrate register
				com->UART_BRRL = (uint8) (u16tmpreg & UART_BDL_SBR_MASK);

			#if (UART_POLLING_MODE == DISABLED)

				#if (UART0_FEATURE == ENABLED)									//Enable UART0 Interrupt and Set Priority for UART0
					if (uart == COM0)
					{
						Micro__NVICEnableIRQ(UART0_STATUS_IRQ_CHANNEL,COM0_PREEMPTION_PRIORITY, COM0_CHANNEL_SUB_PRIORITY);
						Micro__NVICEnableIRQ(UART0_ERROR_IRQ_CHANNEL,	COM0_PREEMPTION_PRIORITY, COM0_CHANNEL_SUB_PRIORITY);
					}
				#endif

				#if (UART1_FEATURE == ENABLED)									//Enable UART1 Interrupt and Set Priority for UART1
					if (uart == COM1)
					{
						Micro__NVICEnableIRQ(UART1_STATUS_IRQ_CHANNEL,COM1_PREEMPTION_PRIORITY, COM1_CHANNEL_SUB_PRIORITY);
						Micro__NVICEnableIRQ(UART1_ERROR_IRQ_CHANNEL,	COM1_PREEMPTION_PRIORITY, COM1_CHANNEL_SUB_PRIORITY);
					}

				#endif

				#if (UART2_FEATURE == ENABLED)									//Enable UART2 Interrupt and Set Priority for UART2
					if (uart == COM2)
					{
						Micro__NVICEnableIRQ(UART2_STATUS_IRQ_CHANNEL,COM2_PREEMPTION_PRIORITY, COM2_CHANNEL_SUB_PRIORITY);
						Micro__NVICEnableIRQ(UART2_ERROR_IRQ_CHANNEL,	COM2_PREEMPTION_PRIORITY, COM2_CHANNEL_SUB_PRIORITY);
					}
				#endif

			#endif

				Uart__Enable(uart, UART_OPTION_ALL);                    		//Enable UARTx module
				response = TRUE;
			}
		}
	}
#else
	uint8 response;
	response = FALSE;
#endif

	return response;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This method clears transmit and receive FIFO buffers of a specific uart.
 *
 *  @param      uart: uart enumerator
 *    @image    html    Uart__Clear.jpg
 */
void Uart__Clear(UART_ENUM_DEF uart)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        #if (UART0_FEATURE == ENABLED)
            if (uart == COM0)
            {
                UART0_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART0_Tx_Write_Index = 0;
                UART0_Tx_Read_Index = 0;
                UART0_Rx_Write_Index = 0;
                UART0_Rx_Read_Index = 0;
                *((uint16 *)(&UART0_LastData)) = 0;
            }
        #endif

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                UART1_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART1_Tx_Write_Index = 0;
                UART1_Tx_Read_Index = 0;
                UART1_Rx_Write_Index = 0;
                UART1_Rx_Read_Index = 0;
                *((uint16 *)(&UART1_LastData)) = 0;
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                UART2_Break = FALSE;

                //Clear the Read and Write Index of Tx and Rx FIFO
                UART2_Tx_Write_Index = 0;
                UART2_Tx_Read_Index = 0;
                UART2_Rx_Write_Index = 0;
                UART2_Rx_Read_Index = 0;
                *((uint16 *)(&UART2_LastData)) = 0;
            }
        #endif

        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method clears the transmit FIFO buffers of a specific Uart.
 * @param uart    : Defines the Uart COM port
 */
void Uart__ClearTx(UART_ENUM_DEF uart)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        #if (UART0_FEATURE == ENABLED)
            if (uart == COM0)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART0_Tx_Write_Index = 0;
                UART0_Tx_Read_Index = 0;
            }
        #endif

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART1_Tx_Write_Index = 0;
                UART1_Tx_Read_Index = 0;
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                //Clear the Read and Write Index of Tx and Rx FIFO
                UART2_Tx_Write_Index = 0;
                UART2_Tx_Read_Index = 0;
            }
        #endif

        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method enables transmission, reception or both depending on option for
 *              specific uart.
 *    @details  This method enables an option (UART_OPTION_ALL,UART_OPTION_TX_EN,UART_OPTION_RX_EN)
 *              for a specific Uart.
 *    @param    uart   : Defines the Specific Uart COM port
 *    @param    option : Defines the option to enable the transmitter, receiver or both of the
 *                       specific Uart.
 *
 *    @image    html    Uart__Enable.jpg
 */
void Uart__Enable(UART_ENUM_DEF uart,UART_ENABLE_OPTIONS_DEF option)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
	UART_BASIC_DEF  *com;

	com = NULL;
    if (uart < COM3 )
    {
        com =  UART_PTR[uart];

        if (com != NULL)
        {
            switch (option)
            {
                case UART_OPTION_ALL:
                    #if (UART_POLLING_MODE == ENABLED)
                		com->UART_C2 |= UART_C2_TE_MASK;
                		com->UART_C2 |= UART_C2_RE_MASK;
                    #else
                		com->UART_C2 |= (UART_C2_RIE_MASK | UART_C2_RE_MASK | UART_C2_TIE_MASK | UART_C2_TE_MASK );
                		com->UART_C3 |= (UART_C3_ORIE_MASK |UART_C3_NEIE_MASK | UART_C3_FEIE_MASK | UART_C3_PEIE_MASK);
                    #endif
                    break;

                case UART_OPTION_TX_EN:
                    #if (UART_POLLING_MODE == ENABLED)
            			com->UART_C2 |= UART_C2_TE_MASK;
                    #else
            			com->UART_C2 |= (UART_C2_TIE_MASK | UART_C2_TE_MASK);
            			com->UART_C3 |= (UART_C3_ORIE_MASK |UART_C3_NEIE_MASK | UART_C3_FEIE_MASK | UART_C3_PEIE_MASK);
                    #endif

                    break;
                case UART_OPTION_RX_EN:
                    #if (UART_POLLING_MODE == ENABLED)
            			com->UART_C2 |= UART_C2_RE_MASK;
                    #else
            			com->UART_C2 |= (UART_C2_RIE_MASK |UART_C2_RE_MASK );
            			com->UART_C3 |= (UART_C3_ORIE_MASK |UART_C3_NEIE_MASK | UART_C3_FEIE_MASK | UART_C3_PEIE_MASK);
                    #endif
                    break;
                default:
                    break;
            }
        }
    }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method disables transmission, reception or both depending on option for
 *              specific uart.
 *    @details  This method disables an option (UART_OPTION_ALL,UART_OPTION_TX_EN,UART_OPTION_RX_EN)
 *              for a specific Uart.
 *    @param    uart   : Defines the Specific Uart COM port
 *    @param    option : Defines the option to disable the transmitter, receiver or both of the
 *                       specific Uart.
 *
 *    @image    html    Uart__Disable.jpg
 */
void Uart__Disable(UART_ENUM_DEF uart,UART_ENABLE_OPTIONS_DEF option)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
    UART_BASIC_DEF * com;

    if (uart < COM3 )
    {
        com = UART_PTR[uart];

        if (com != NULL)
        {
            switch (option)
            {
                case UART_OPTION_ALL:
                	com->UART_C2 &= (uint8)~((uint8)(UART_C2_RIE_MASK | UART_C2_RE_MASK | UART_C2_TE_MASK | UART_C2_TIE_MASK));
                	com->UART_C3 &= (uint8)~((uint8)(UART_C3_ORIE_MASK |UART_C3_NEIE_MASK | UART_C3_FEIE_MASK | UART_C3_PEIE_MASK));
                    break;

                case UART_OPTION_TX_EN:
                	com->UART_C2 &= (uint8)~((uint8)(UART_C2_TE_MASK | UART_C2_TIE_MASK));
                	com->UART_C3 &= (uint8)~((uint8)(UART_C3_ORIE_MASK |UART_C3_NEIE_MASK | UART_C3_FEIE_MASK | UART_C3_PEIE_MASK));
                    break;

                case UART_OPTION_RX_EN:
                	com->UART_C2 &= (uint8)~((uint8)(UART_C2_RE_MASK | UART_C2_RIE_MASK));
                	com->UART_C3 &= (uint8)~((uint8)(UART_C3_ORIE_MASK |UART_C3_NEIE_MASK | UART_C3_FEIE_MASK | UART_C3_PEIE_MASK));
                    break;

                default:
                    break;
            }
        }
    }
   #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes data from ‘source’ (buffer address mentioned by the Application) to\n
 *              the ‘destination’ (Transmitter FIFO) as per the size mentioned to the specific Uart\n
 *              and also returns the response for the write operation.
 *    @details  The data in the Transmitter FIFO is sent to other Uart node through the\n
 *              specified Uart COM port whenever the Transmitter is free.\n
 *              Before writing into the Transmitter FIFO check for the FIFO availability using Uart__GetStatus() API\n
 *              to avoid the over writing on the existing data.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @param    buf : Indicates the buffer’s address which contains the data to be sent.
 *    @param    size : Defines the number of bytes to be sent.
 *    @return   TRUE (1): If the data written in the transmission FIFO is successful.
 *              FALSE (0): If data written in the transmission FIFO is failed.
 *
 *    @image    html    Uart__Write.jpg
 */
BOOL_TYPE Uart__Write(UART_ENUM_DEF uart, uint8 * buf,uint8 size)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
        BOOL_TYPE response;
        uint8 count;
        uint8 pt;

        response = TRUE;

        #if (UART0_FEATURE == ENABLED)
            if (uart == COM0)
            {
                pt = UART0_Tx_Write_Index;
                for (count = 0; count < size; count++)
                {
                    UART0_Tx_Buffer[pt] = *buf;
                    buf++;
                    pt++;
                    if (pt >= UART0_TX_BUF_SIZE)
                    {
                        pt = 0;
                    }
                }
                UART0_Tx_Write_Index = pt;
            }
        #endif

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                pt = UART1_Tx_Write_Index;
                for (count = 0; count < size; count++)
                {
                    UART1_Tx_Buffer[pt] = *buf;
                    buf++;
                    pt++;
                    if (pt >= UART1_TX_BUF_SIZE)
                    {
                        pt = 0;
                    }
                }
                UART1_Tx_Write_Index = pt;
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                pt = UART2_Tx_Write_Index;
                for (count = 0; count < size; count++)
                {
                    UART2_Tx_Buffer[pt] = *buf;
                    buf++;
                    pt++;
                    if (pt >= UART2_TX_BUF_SIZE)
                    {
                        pt = 0;
                    }
                }
                UART2_Tx_Write_Index = pt;
            }
        #endif

    #else
        BOOL_TYPE response;
        response = FALSE;
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads data from ‘receive FIFO’ and\n
 *              copies to the ‘address of the buffer’ mentioned by the Application as per the size and\n
 *              also returns the response for the read operation.
 *    @details  Before reading the Receiver FIFO check for the data availability in the\n
 *              Receiver FIFO using Uart__GetStatus() API.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @param    buf : Indicates the buffer’s address
 *    @param    size : Defines the number of bytes to be sent.
 *    @return   TRUE (1): If data reading from receive FIFO is successful. The amount of bytes
 *                        requested was available to read
 *              FALSE (0): If data reading from receive FIFO is unsuccessful. There was not
 *                         enough data to be read, fewer bytes than the size requested
 *
 *    @image    html    Uart__Read.jpg
 */
BOOL_TYPE Uart__Read(UART_ENUM_DEF uart, uint8 * buf,uint8 size)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
        BOOL_TYPE response;
        uint8 count;
        uint8 pt;

        response = TRUE;

        #if (UART0_FEATURE == ENABLED)
            if (uart == COM0)
            {
                pt = UART0_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART0_Rx_Write_Index)
                    {
                        *buf = UART0_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART0_LastData)) = *((uint16 *)(&UART0_Rx_Buffer[pt]));

                        buf++;
                        pt++;

                        if (pt >= UART0_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART0_Rx_Read_Index;
                        break;
                    }
                }

                UART0_Rx_Read_Index = pt;
            }
            #endif

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                pt = UART1_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART1_Rx_Write_Index)
                    {
                        *buf = UART1_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART1_LastData)) = *((uint16 *)(&UART1_Rx_Buffer[pt]));
                        buf++;
                        pt++;

                        if (pt >= UART1_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART1_Rx_Read_Index;
                        break;
                    }

                    UART1_Rx_Read_Index = pt;
                }
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                pt = UART2_Rx_Read_Index;

                for (count = 0; count < size; count++)
                {
                    if (pt != UART2_Rx_Write_Index)
                    {
                        *buf = UART2_Rx_Buffer[pt].data;
                        *((uint16 *)(&UART2_LastData)) = *((uint16 *)(&UART2_Rx_Buffer[pt]));

                        buf++;
                        pt++;

                        if (pt >= UART2_RX_BUF_SIZE)
                        {
                            pt = 0;
                        }
                    }
                    else
                    {
                        response = FALSE;
                        pt = UART2_Rx_Read_Index;
                        break;
                    }
                }

                UART2_Rx_Read_Index = pt;
            }
            #endif
    #else
        BOOL_TYPE response;
        response = FALSE;
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes one byte of data into the Uart transmitter FIFO.
 *    @details  The data in the Transmitter FIFO is sent to other Uart node through the specified Uart COM port\n
 *              whenever the Transmitter is free.\n
 *              Before writing into the Transmitter FIFO check for the FIFO availability using Uart__GetStatus() API\n
 *              to avoid the over writing on the existing data.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @param    data : data to be written into the transmission FIFO.
 *    @return   TRUE (1): If data writing into transmission FIFO is successful.
 *              FALSE (0): If data writing into transmission FIFO is unsuccessful.
 *
 *    @image    html    Uart__WriteByte.jpg
 */
BOOL_TYPE Uart__WriteByte(UART_ENUM_DEF uart,uint8 data)
{
	BOOL_TYPE response;
    response = FALSE;

    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
        response = Uart__Write(uart,&data,1);
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads one byte of data from Uart receiver FIFO.
 *    @details  Before reading the Receiver FIFO check for the data availability in the Receiver FIFO
 *              using Uart__GetStatus() API.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @return   uint8: data read from UART receive FIFO
 *
 *    @image    html    Uart__ReadByte.jpg
 */
uint8 Uart__ReadByte(UART_ENUM_DEF uart)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
        uint8 response;

        response = FALSE;

        Uart__Read(uart,&response,1);
    #else
        uint8 response;
        response = FALSE;
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method starts sending break signal through a specific uart.
 *    @details  Break signal should be sent If the Uart Node requires more time to participate in the Uart communication.
 *              The break signal should be released once the node is ready to participate in the communication.
 *              The Break signal should not be hold for the long time.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @image    html    Uart__SendBreak.jpg
 */
void Uart__SendBreak(UART_ENUM_DEF uart)
{
	#if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
		Uart__Disable(uart, UART_OPTION_TX_EN);

		#if (UART0_FEATURE == ENABLED)
			if (uart == COM0)
			{
				UART0_Break = TRUE;
				//Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
				#if(UART0_REMAP == 0)
					Gpio__PinConfig(PORTB,1,OUTPUT_PUSHPULL);
					Gpio__PinWrite(PORTB,1,0);
				#else
					Gpio__PinConfig(PORTA,3,OUTPUT_PUSHPULL);
					Gpio__PinWrite(PORTA,3,0);
				#endif
			}
		#endif

		#if (UART1_FEATURE == ENABLED)
			if (uart == COM1)
			{
				UART1_Break = TRUE;
				//Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
				#if(UART1_REMAP == 0)
					Gpio__PinConfig(PORTC,7,OUTPUT_PUSHPULL);
					Gpio__PinWrite(PORTC,7,0);
				#else
//					Gpio__PinConfig(PORTF,3,OUTPUT_PUSHPULL);
//					Gpio__PinWrite(PORTF,3,0);
				#endif
			}
		#endif

		#if (UART2_FEATURE == ENABLED)
			if (uart == COM2)
			{
				UART2_Break = TRUE;
				//Configure the Uart Tx pin as OUTPUT PUSHPULL and drive LOW on the pin
				#if(UART2_REMAP == 0)
					Gpio__PinConfig(PORTD, 7, OUTPUT_PUSHPULL);
					Gpio__PinWrite(PORTD, 7, 0);
				#else
					Gpio__PinConfig(PORTI,1,OUTPUT_PUSHPULL);
					Gpio__PinWrite(PORTI,1,0);
				#endif
			}
		#endif

	#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This procedure stops sending a break signal through a specific Uart.
 *    @details  The break signal should be release once the node is ready to participate in the communication.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @image    html    Uart__SendBreak_Verify.jpg
 */
void Uart__StopBreak(UART_ENUM_DEF uart)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))

		#if (UART0_FEATURE == ENABLED)
			if (uart == COM0)
			{
				UART0_Break = FALSE;                     //Configure the Uart Tx port-pin
			}
		#endif

		#if (UART1_FEATURE == ENABLED)
			if (uart == COM1)
			{
				UART1_Break = FALSE;                     //Configure the Uart Tx port-pin
			}
		#endif

		#if (UART2_FEATURE == ENABLED)
			if (uart == COM2)
			{
				UART2_Break = FALSE;                     //Configure the Uart Tx port-pin
			}
		#endif

		Uart__Enable(uart,UART_OPTION_TX_EN);            //Uart Tx Pin configuration

    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method checks the status of the specific Uart.
 *    @details  This method checks the break in, break out signal, transmit and receive FIFO availability for\n
 *              writing and reading, transmit FIFO empty and error status of received frame.
 *    @param    uart : Defines the Specific Uart COM port.
 *    @param    index : Index enumerator defines which status should be checked for specific Uart.
 *
 *    @return   UART_STATUS_BREAK_IN:\n
 *              Response:   TRUE (1) = If it detects the Break signal sent by other node.\n
 *                          FALSE (0) = No break signal is detected.\n
 *              UART_STATUS_BREAK_OUT:\n
 *              Response:   TRUE (1) = A break signal is sent;\n
 *                          FALSE (0) = No break signal is sent;\n
 *              UART_STATUS_TX_EMPTY:\n
 *              Response:   TRUE (1) = The transmission FIFO is empty;\n
 *                          FALSE (0) = The transmission FIFO is not empty;\n
 *              UART_STATUS_TX_AVAILABLE:\n
 *              Response:   The response is the free space in the Transmission FIFO\n
 *                          If the response is 1 means the only one byte is free in the Tx FIFO,\n
 *                          if response is 10 means only 10 bytes are free in the Tx FIFO.\n
 *              UART_STATUS_RX_AVAILABLE:\n
 *              Response:   The amount of bytes available in the Reception FIFO;\n
 *                          If the response is 1 means the only one byte is available to read the Rx FIFO,\n
 *                          If response is 10 means only 10 bytes are available to read the the Rx FIFO.\n
 *              UART_STATUS_ERROR:\n
 *              Response:   TRUE (1) = Error detected (Over run, Framing, Parity or noise_err error)\n
 *                          FALSE (0) = No error detected.
 *
 */
uint8 Uart__GetStatus(UART_ENUM_DEF uart,UART_STATUS_DEF index)
{
    #if ((UART0_FEATURE == ENABLED)||(UART1_FEATURE == ENABLED)||(UART2_FEATURE == ENABLED))
        uint8 response;

        response = FALSE;

        // UART_Rx_Write_Index and UART_Tx_Read_Index variables values gets updated from the Uart Interrupt which runs at the highest priority in asynchronous way
        // The value shall be copied to the local variable and perform the operation instead of relying on the variable which is changing its value based on the interrupt
        uint8 local_uart_rx_write_index = 0;
        uint8 local_uart_tx_read_index = 0;

        #if (UART0_FEATURE == ENABLED)
            if (uart == COM0)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART0_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART0_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART0_Tx_Read_Index == UART0_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART0_Tx_Read_Index;
                        if (local_uart_tx_read_index <= UART0_Tx_Write_Index)
                        {
                            response = (UART0_TX_BUF_SIZE - UART0_Tx_Write_Index) + local_uart_tx_read_index - 1;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART0_Tx_Write_Index - 1;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART0_Rx_Write_Index;
                        if (UART0_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART0_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART0_RX_BUF_SIZE -  UART0_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART0_LastData.overrun_err)||       // Overrun error
                           (UART0_LastData.noise_err)||             // Noisy error
                           (UART0_LastData.frame_err)||         // Frame error
                           (UART0_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (UART1_FEATURE == ENABLED)
            if (uart == COM1)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART1_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART1_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART1_Tx_Read_Index == UART1_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART1_Tx_Read_Index;
                        if (local_uart_tx_read_index <= UART1_Tx_Write_Index)
                        {
                            response = (UART1_TX_BUF_SIZE - UART1_Tx_Write_Index) + local_uart_tx_read_index - 1;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART1_Tx_Write_Index - 1;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART1_Rx_Write_Index;
                        if (UART1_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART1_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART1_RX_BUF_SIZE -  UART1_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART1_LastData.overrun_err)||       // Overrun error
                           (UART1_LastData.noise_err)||             // Noisy error
                           (UART1_LastData.frame_err)||         // Frame error
                           (UART1_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    default:
                        break;
                }
            }
        #endif

        #if (UART2_FEATURE == ENABLED)
            if (uart == COM2)
            {
                switch(index)
                {
                    case UART_STATUS_BREAK_IN:
                        if(UART2_LastData.incomming_break == TRUE)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_BREAK_OUT:
                        response = UART2_Break;
                        break;

                    case UART_STATUS_TX_EMPTY:
                        if (UART2_Tx_Read_Index == UART2_Tx_Write_Index)
                        {
                            response = TRUE;
                        }
                        break;

                    case UART_STATUS_TX_AVAILABLE:
                        local_uart_tx_read_index = UART2_Tx_Read_Index;
                        if (local_uart_tx_read_index <= UART2_Tx_Write_Index)
                        {
                            response = (UART1_TX_BUF_SIZE - UART2_Tx_Write_Index) + local_uart_tx_read_index - 3;
                        }
                        else
                        {
                            response = local_uart_tx_read_index - UART2_Tx_Write_Index - 3;
                        }
                        break;

                    case UART_STATUS_RX_AVAILABLE:
                        local_uart_rx_write_index = UART2_Rx_Write_Index;
                        if (UART2_Rx_Read_Index <= local_uart_rx_write_index)
                        {
                            response = local_uart_rx_write_index - UART2_Rx_Read_Index;
                        }
                        else
                        {
                            response = (UART2_RX_BUF_SIZE -  UART2_Rx_Read_Index) + local_uart_rx_write_index;
                        }
                        break;

                    case UART_STATUS_ERROR:
                        if((UART2_LastData.overrun_err)||       // Overrun error
                           (UART2_LastData.noise_err)||             // Noisy error
                           (UART2_LastData.frame_err)||         // Frame error
                           (UART2_LastData.parity_err))         // Parity error
                        {
                            response = TRUE;
                        }
                        break;

                    default:
                        break;
                }
            }
        #endif

    #else
        uint8 response;
        response = FALSE;
    #endif

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles Uart Data Transmission and also the break signal
 *    @details  The break signal has highest priority then the Data Transmission.\n
 *              If the application requests for the Break signal transmission then handler inserts break signal after
 *              completion of the ongoing transmission\n
 *              and stops the further Data transmission until the Break signal is resumed.\n
 *              This method checks if there is data in transmit FIFO. If data is present then this method\n
 *              transmits the data on Uart.
 *
 *    @image    html    Uart__Handler.jpg
 */
void Uart__Handler(void)
{
    #if (UART0_FEATURE == ENABLED)
        #if (UART_POLLING_MODE == ENABLED)
            //COM0 Handling
            Uart__Interrupt0();
        #else
            if (UART0_Tx_Write_Index != UART0_Tx_Read_Index)   // Is there something to be sent?
            {
            	if(((UART0.UART_C2 & UART_C2_TE_MASK)== FALSE)||
           			((UART0.UART_C2 & UART_C2_TIE_MASK)== FALSE))
                {
                    Uart__Enable(COM0, UART_OPTION_TX_EN);
                }
            }
        #endif
    #endif

    #if (UART1_FEATURE == ENABLED)
        #if (UART_POLLING_MODE == ENABLED)
            //COM1 Handling
            Uart__Interrupt1();
        #else
            if (UART1_Tx_Write_Index != UART1_Tx_Read_Index)   // Is there something to be sent?
            {
            	if(((UART1.UART_C2 & UART_C2_TE_MASK)== FALSE)||
           			((UART1.UART_C2 & UART_C2_TIE_MASK)== FALSE))
                {
                    Uart__Enable(COM1, UART_OPTION_TX_EN);
                }
            }
        #endif
    #endif

    #if (UART2_FEATURE == ENABLED)
        #if (UART_POLLING_MODE == ENABLED)
            //COM2 Handling
            Uart__Interrupt2();
        #else
            if (UART2_Tx_Write_Index != UART2_Tx_Read_Index)   // Is there something to be sent?
            {
            	if(((UART2.UART_C2 & UART_C2_TE_MASK)== FALSE)||
           			((UART2.UART_C2 & UART_C2_TIE_MASK)== FALSE))
                {
                    Uart__Enable(COM2, UART_OPTION_TX_EN);
                }
            }
        #endif
    #endif

}

////---------------------------------------------------------------------------------------------------------------------
///**
// *    @brief    This method handles the Uart0 receive interrupt.
// *    @details  This needs to be configured in the InterruptVector_prm.h file as below-\n
// *              #define USART0_RX_INTERRUPTION      Uart__Uart1RxIsr\n
// *              This ISR is called whenever the data is received in the Uart_DR register.
// */
void Uart__Interrupt0(void)
{
#if (UART0_FEATURE == ENABLED)
	volatile UART_BASIC_DEF status;
	uint8 index;
	volatile uint8 data;

	status.UART_S1 = UART0.UART_S1;					//Read UART Status Register 1

	//Receive part----------------------------------------------
	if (((status.UART_S1)& ((UART_S1_RDRF_MASK) | (UART_S1_OR_MASK) | (UART_S1_NF_MASK)   |
			                (UART_S1_FE_MASK)   | (UART_S1_PF_MASK))) != FALSE)
	{
		data = (UART0.UART_DATA & (uint8) 0xFF);                          //Read data from UART_DATA register

		if ((status.UART_S1 & UART_S1_RDRF_MASK) != FALSE)
		{
			index = UART0_Rx_Write_Index;

			*((uint16 *) (&UART0_Rx_Buffer[index])) = 0;

			*((uint16 *) (&UART0_Rx_Buffer[index])) = (uint8) status.UART_S1;

			UART0_Rx_Buffer[index].data = data;                               //Copy UART Receive data in UART1_Rx_Buffer.

			if (((UART0.UART_S1 & UART_S1_FE_MASK) != FALSE) && (UART0_Rx_Buffer[index].data == 0))
			{
				UART0_Rx_Buffer[index].incomming_break = TRUE;                //Framing error detected
			}
			else
			{
				UART0_Rx_Buffer[index].incomming_break = FALSE;               //No Framing error detected
			}

			index++;

			if (index >= UART0_RX_BUF_SIZE)
			{
				index = 0;
			}

			UART0_Rx_Write_Index = index;
		}
	}

	//Transmitter Part -----------------------------------------------------------------------------------------------
	#if (UART_POLLING_MODE == DISABLED)
		if ((UART0.UART_C2 & UART_C2_TIE_MASK) != FALSE)
	#endif
	{
		if ((status.UART_S1 & UART_S1_TDRE_MASK) != FALSE)
		{
			if (UART0_Tx_Write_Index == UART0_Tx_Read_Index)
			{
				#if (UART_POLLING_MODE == DISABLED)                           //UART Running in Interrupt Mode
					UART0.UART_C2 &= (uint8) ~(UART_C2_TIE_MASK);
				#endif
			}
			else
			{
				#if (UART_POLLING_MODE == ENABLED)                            //UART Running in Polling Mode
					if((UART0.UART_C2 & UART_C2_TE_MASK) == FALSE)
					{
						Uart__Enable(COM0, UART_OPTION_TX_EN);
					}
				#endif

				UART0.UART_DATA = (UART0_Tx_Buffer[UART0_Tx_Read_Index]	& (uint16) 0x01FF);

				UART0_Tx_Read_Index++;
				if (UART0_Tx_Read_Index >= UART0_TX_BUF_SIZE)                 //Check Tx Buffer Size
				{
					UART0_Tx_Read_Index = 0;
				}
			}
		}

	}
#else
	UART0.UART_C2 &= (uint8)~((uint8)(UART_C2_TIE_MASK | UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK));
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles the Uart1 receive interrupt.
 *    @details  This needs to be configured in the InterruptVector_prm.h file as below-\n
 *              #define USART1_RX_INTERRUPTION      Uart__Uart1RxIsr\n
 *              This ISR is called whenever the data is received in the UART_DATA register.
 */
void Uart__Interrupt1(void)
{
#if (UART1_FEATURE == ENABLED)
	volatile UART_BASIC_DEF status;
	uint8 index;
	volatile uint8 data;

	status.UART_S1 = UART1.UART_S1;					//Read UART Status Register 1

	//Receive part----------------------------------------------
	if (((status.UART_S1)& ((UART_S1_RDRF_MASK) | (UART_S1_OR_MASK) | (UART_S1_NF_MASK) |
			                (UART_S1_FE_MASK) | (UART_S1_PF_MASK))) != FALSE)
	{
		data = (UART1.UART_DATA & (uint8) 0xFF);                        //Read data from UART_DATA register

		if ((status.UART_S1 & UART_S1_RDRF_MASK) != FALSE)
		{

			index = UART1_Rx_Write_Index;

			*((uint16 *) (&UART1_Rx_Buffer[index])) = 0;
			*((uint16 *) (&UART1_Rx_Buffer[index])) = (uint8) status.UART_S1;

			UART1_Rx_Buffer[index].data = data;                             //Copy UART Receive data in UART1_Rx_Buffer.

			if (((UART1.UART_S1 & UART_S1_FE_MASK) != FALSE) && (UART1_Rx_Buffer[index].data == 0))
			{
				UART1_Rx_Buffer[index].incomming_break = TRUE;               //Framing error
			}
			else
			{
				UART1_Rx_Buffer[index].incomming_break = FALSE;              //No Framing error detected
			}

			index++;

			if (index >= UART1_RX_BUF_SIZE)                                  //Check Rx Buffer Size
			{
				index = 0;
			}

			UART1_Rx_Write_Index = index;
		}
	}

	//Transmitter---------------------------------------------------------------------------------------------------
	#if (UART_POLLING_MODE == DISABLED)
		if ((UART1.UART_C2 & UART_C2_TIE_MASK) != FALSE)
	#endif
	{
		if ((status.UART_S1 & UART_S1_TDRE_MASK) != FALSE)
		{
			if (UART1_Tx_Write_Index == UART1_Tx_Read_Index)
			{
				#if (UART_POLLING_MODE == DISABLED)                           //UART Running in Interrupt Mode
					UART1.UART_C2 &= (uint8) ~(UART_C2_TIE_MASK);
				#endif
			}
			else
			{
				#if (UART_POLLING_MODE == ENABLED)                            //UART Running in Polling Mode
					if((UART1.UART_C2 & UART_C2_TE_MASK) == FALSE)
					{
						Uart__Enable(COM1, UART_OPTION_TX_EN);
					}
				#endif

				UART1.UART_DATA = (UART1_Tx_Buffer[UART1_Tx_Read_Index]	& (uint16) 0x01FF);   //Transmit UART data

				UART1_Tx_Read_Index++;
				if (UART1_Tx_Read_Index >= UART1_TX_BUF_SIZE)                 //Check Tx Buffer Size
				{
					UART1_Tx_Read_Index = 0;
				}
			}
		}

	}
#else
	UART1.UART_C2 &= (uint8)~((uint8)(UART_C2_TIE_MASK | UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK));
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method handles the Uart3 receive interrupt.
 *    @details  This needs to be configured in the InterruptVector_prm.h file as below-\n
 *              #define USART3_RX_INTERRUPTION      Uart__Uart3RxIsr\n
 *              This ISR is called whenever the data is received in the Uart_DR register.
 */
void Uart__Interrupt2(void)
{
#if (UART2_FEATURE == ENABLED)
	volatile UART_BASIC_DEF status;
	uint8 index;
	volatile uint8 data;

	status.UART_S1 = UART2.UART_S1;				//Read UART Status Register 1

	//Receive part----------------------------------------------
	if (((status.UART_S1)& ((UART_S1_RDRF_MASK) | (UART_S1_OR_MASK) | (UART_S1_NF_MASK) |
							(UART_S1_FE_MASK) |	(UART_S1_PF_MASK))) != FALSE)
	{
		data = (UART2.UART_DATA & (uint8) 0xFF);                          //Read data from UART_DATA register

		if ((status.UART_S1 & UART_S1_RDRF_MASK) != FALSE)
		{
			index = UART2_Rx_Write_Index;

			*((uint16 *) (&UART2_Rx_Buffer[index])) = 0;

			*((uint16 *) (&UART2_Rx_Buffer[index])) = (uint8) status.UART_S1;

			UART2_Rx_Buffer[index].data = data;                               //Copy UART Receive data in UART1_Rx_Buffer.

			if (((UART2.UART_S1 & UART_S1_FE_MASK) != FALSE) && (UART2_Rx_Buffer[index].data == 0))
			{
				UART2_Rx_Buffer[index].incomming_break = TRUE;                //Framing error detected
			}
			else
			{
				UART2_Rx_Buffer[index].incomming_break = FALSE;               //No Framing error detected
			}
			index++;

			if (index >= UART2_RX_BUF_SIZE)
			{
				index = 0;
			}

			UART2_Rx_Write_Index = index;
		}
	}

	//Transmitter---------------------------------------------------------------------------------------------------
	#if (UART_POLLING_MODE == DISABLED)
		if ((UART2.UART_C2 & UART_C2_TIE_MASK) != FALSE)
	#endif
	{
		if ((status.UART_S1 & UART_S1_TDRE_MASK) != FALSE)
		{
			if (UART2_Tx_Write_Index == UART2_Tx_Read_Index)
			{
				#if (UART_POLLING_MODE == DISABLED)                           //UART Running in Interrupt Mode
					UART2.UART_C2 &= (uint8) ~(UART_C2_TIE_MASK);
				#endif
			}
			else
			{
			#if (UART_POLLING_MODE == ENABLED)                                //UART Running in Polling Mode
				if((UART2.UART_C2 & UART_C2_TE_MASK) == FALSE)
				{
					Uart__Enable(COM1, UART_OPTION_TX_EN);
				}
			#endif

				UART2.UART_DATA = (UART2_Tx_Buffer[UART2_Tx_Read_Index]	& (uint16) 0x01FF);

				UART2_Tx_Read_Index++;
				if (UART2_Tx_Read_Index >= UART2_TX_BUF_SIZE)                 //Check Tx Buffer Size
				{
					UART2_Tx_Read_Index = 0;
				}
			}
		}

	}
#else
	UART2.UART_C2 &= (uint8) ~((uint8) (UART_C2_TIE_MASK | UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK));
#endif

}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

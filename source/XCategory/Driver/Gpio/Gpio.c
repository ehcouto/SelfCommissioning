/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This module provides a standard way to access the GPIO ports and port-bits for Kinetis KE02x micro-contoller
 *
 *  $Header: Gpio.c 1.4 2015/08/21 13:56:34EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2012-$Date: 2015/08/21 13:56:34EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Gpio.h"
#include "SRMicro.h"
#include "SRException.h"
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef GPIO_PROTECTION_FEATURE
    // By default the feature is ENABLED
    #define GPIO_PROTECTION_FEATURE   ENABLED
#endif


/*
 * The below tables are based on the PIN_TYPE_DEF which is defined in the uc_MKE02x.h file -
 *      typedef enum
 *      {
 *          INPUT_FLOAT,
 *          INPUT_PULLUP,
 *          INPUT_PULLDOWN,
 *          INPUT_ANALOG,
 *          SPECIAL_FUNCTION_OPEN_DRAIN,
 *          SPECIAL_FUNCTION_PUSH_PULL,
 *          OUTPUT_PUSHPULL,
 *          OUTPUT_OPEN_DRAIN
 *      } PIN_TYPE_DEF;
 */

volatile GPIO_DEF * const GPIO_PORT_LIST[] = {  (volatile GPIO_DEF *)GPIOA_BASE,
                                                (volatile GPIO_DEF *)GPIOB_BASE,
                                                (volatile GPIO_DEF *)GPIOC_BASE,
                                                (volatile GPIO_DEF *)GPIOD_BASE,
                                                (volatile GPIO_DEF *)GPIOE_BASE};

volatile PORTx_DEF * const PCI_PORT_LIST[] = {  (volatile PORTx_DEF *)PORTA_BASE,
                                                (volatile PORTx_DEF *)PORTB_BASE,
                                                (volatile PORTx_DEF *)PORTC_BASE,
                                                (volatile PORTx_DEF *)PORTD_BASE,
                                                (volatile PORTx_DEF *)PORTE_BASE};


#define ALL_OUTPUT							(uint32)0xFFFFFFFF
#define ALL_INPUT							(uint32)0x00000000

unsigned char const GPIO_SET_MASK[] = {0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80};
unsigned char const GPIO_CLR_MASK[] = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint32 UpdateMaskedRegister(uint32 current,uint32 mask, uint32 value);
static BOOL_TYPE GpioCheckPinConfig(GPIO_PORT_TYPE port,uint8 pin,GPIO_PIN_MODE_TYPE type);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief 	This method Initializes the GPIO module and its variables
 *    @details  Connects the GPIO module to the clock Bus and calls the Gpio__RefreshConfig() which configures the
 *              Gpio Ports to there default configuration as specified in the Gpio_prm.h file
 */
void Gpio__Initialize(void)
 {
	// Enable all of the port clocks. These have to be enabled to configure pin muxing options.

	SIM.SCGC5 |= (SIM_SCGC5_PORTA_MASK |			// Connect the GPIOA device to the Core bus
			SIM_SCGC5_PORTB_MASK |					// Connect the GPIOB device to the Core bus
			SIM_SCGC5_PORTC_MASK |					// Connect the GPIOC device to the Core bus
			SIM_SCGC5_PORTD_MASK |					// Connect the GPIOD device to the Core bus
			SIM_SCGC5_PORTE_MASK);					// Connect the GPIOE device to the Core bus

	Gpio__RefreshConfig();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief 	This method refreshes the configuration of the port.
 *    @details  Configures the Gpio Ports to there default configuration as specified in the Gpio_prm.h file
 *              Interrupt are disabled before assigning value to register and enabled after
 *              value is assigned
 */
void Gpio__RefreshConfig(void)
{
    #if (GPIO_PROTECTION_FEATURE == ENABLED)
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    #endif

    #if (GPIO_PROTECTION_FEATURE == ENABLED)
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL(); // It saves and disable interrupts
    #endif

//*********** Refresh PORTA Registers *********************
	#ifdef PORTA_PDDR_DEFAULT					//For Port Data Direction Register
		#ifdef PORTA_MASK
			GPIOA.PDDR = UpdateMaskedRegister(GPIOA.PDDR,PORTA_MASK, PORTA_PDDR_DEFAULT);
		#else
			GPIOA.PDDR.BYTE = PORTA_PDDR_DEFAULT;

		#endif
	#endif

	#ifdef PORTA_PULLUP_DEFAULT						//For Port Pull up Enable Register
		#ifdef PORTA_MASK
			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
			PCIPORTA.PCR[0] = UpdateMaskedRegister(PCIPORTA.PCR[0],PORTA_MASK, PORTA_PCR_DEFAULT);
		#else
			PORTPULLA.PUE.BYTE = PORTA_PULLUP_DEFAULT;

		#endif
	#endif

////*********** Refresh PORTB Registers *********************
	#ifdef PORTB_PDDR_DEFAULT						//For Port Data Direction Register
		#ifdef PORTB_MASK
			GPIOB.PDDR = UpdateMaskedRegister(GPIOB.PDDR,PORTB_MASK, PORTB_PDDR_DEFAULT);
		#else
			GPIOB.PDDR = PORTB_PDDR_DEFAULT;
		#endif
	#endif

//	#ifdef PORTB_PIDR_DEFAULT						//For Port Input Disable Register
//		#ifdef PORTB_MASK
//			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
//			GPIOB.PIDR.BYTE = UpdateMaskedRegister(GPIOB.PIDR.BYTE,PORTB_MASK, PORTB_PIDR_DEFAULT);
//		#else
//			GPIOB.PIDR.BYTE = PORTB_PIDR_DEFAULT;
//
//		#endif
//	#endif
//
//	#ifdef PORTB_PULLUP_DEFAULT						//For Port Pull up Enable Register
//		#ifdef PORTB_MASK
//			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
//			PORTPULLB.PUE.BYTE = UpdateMaskedRegister(PORTPULLB.PUE.BYTE,PORTB_MASK, PORTB_PULLUP_DEFAULT);
//		#else
//			PORTPULLB.PUE.BYTE = PORTB_PULLUP_DEFAULT;
//
//		#endif
//	#endif
//
//*********** Refresh PORTC Registers *********************
	#ifdef PORTC_PDDR_DEFAULT						//For Port Data Direction Register
		#ifdef PORTC_MASK
			GPIOC.PDDR = UpdateMaskedRegister(GPIOC.PDDR,PORTC_MASK, PORTC_PDDR_DEFAULT);
		#else
			GPIOC.PDDR.BYTE = PORTC_PDDR_DEFAULT;
		#endif
	#endif
//
//	#ifdef PORTC_PIDR_DEFAULT						//For Port Input Disable Register
//		#ifdef PORTC_MASK
//			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
//			GPIOC.PIDR.BYTE = UpdateMaskedRegister(GPIOC.PIDR.BYTE,PORTC_MASK, PORTC_PIDR_DEFAULT);
//		#else
//			GPIOC.PIDR.BYTE = PORTC_PIDR_DEFAULT;
//
//		#endif
//	#endif
//
//	#ifdef PORTC_PULLUP_DEFAULT						//For Port Pull up Enable Register
//		#ifdef PORTC_MASK
//			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
//			PORTPULLC.PUE.BYTE = UpdateMaskedRegister(PORTPULLC.PUE.BYTE,PORTC_MASK, PORTC_PULLUP_DEFAULT);
//		#else
//			PORTPULLC.PUE.BYTE = PORTC_PULLUP_DEFAULT;
//
//		#endif
//	#endif
//
//*********** Refresh PORTD Registers *********************
	#ifdef PORTD_PDDR_DEFAULT						//For Port Data Direction Register
		#ifdef PORTD_MASK
			GPIOD.PDDR = UpdateMaskedRegister(GPIOD.PDDR,PORTD_MASK, PORTD_PDDR_DEFAULT);
		#else
			GPIOD.PDDR.BYTE = PORTD_PDDR_DEFAULT;
		#endif
	#endif
//
//	#ifdef PORTD_PIDR_DEFAULT						//For Port Input Disable Register
//		#ifdef PORTD_MASK
//			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
//			GPIOD.PIDR.BYTE = UpdateMaskedRegister(GPIOD.PIDR.BYTE,PORTD_MASK, PORTD_PIDR_DEFAULT);
//		#else
//			GPIOD.PIDR.BYTE = PORTD_PIDR_DEFAULT;
//
//		#endif
//	#endif
//
//	#ifdef PORTD_PULLUP_DEFAULT						//For Port Pull up Enable Register
//		#ifdef PORTD_MASK
//			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
//			PORTPULLD.PUE.BYTE = UpdateMaskedRegister(PORTPULLD.PUE.BYTE,PORTD_MASK, PORTD_PULLUP_DEFAULT);
//		#else
//			PORTPULLD.PUE.BYTE = PORTD_PULLUP_DEFAULT;
//
//		#endif
//	#endif
//
//*********** Refresh PORTE Registers *********************
	#ifdef PORTE_PDDR_DEFAULT						//For Port Data Direction Register
		#ifdef PORTE_MASK
			GPIOE.PDDR = UpdateMaskedRegister(GPIOE.PDDR,PORTE_MASK, PORTE_PDDR_DEFAULT);
		#else
			GPIOE.PDDR.BYTE = PORTE_PDDR_DEFAULT;

		#endif
	#endif
//
//	#ifdef PORTE_PIDR_DEFAULT						//For Port Input Disable Register
//		#ifdef PORTE_MASK
//			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
//			GPIOE.PIDR.BYTE = UpdateMaskedRegister(GPIOE.PIDR.BYTE,PORTE_MASK, PORTE_PIDR_DEFAULT);
//		#else
//			GPIOE.PIDR.BYTE = PORTE_PIDR_DEFAULT;
//
//		#endif
//	#endif
//
//	#ifdef PORTE_PULLUP_DEFAULT						//For Port Pull up Enable Register
//		#ifdef PORTE_MASK
//			//static uint8 UpdateMaskedRegister(uint8 current,uint8 mask, uint8 value)
//			PORTPULLE.PUE.BYTE = UpdateMaskedRegister(PORTPULLE.PUE.BYTE,PORTE_MASK, PORTE_PULLUP_DEFAULT);
//		#else
//			PORTPULLE.PUE1.BYTE = PORTE_PULLUP_DEFAULT;
//
//		#endif
//	#endif
//

//*********** End of Refresh PORT Registers *********************
    #if (GPIO_PROTECTION_FEATURE == ENABLED)
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();  // Reestablish the interruption configuration
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	This method validates the port pin number.
 * @param 	port: Defines the specific port
 * @param 	pin	: Defines the specific pin
 * @return
 */
PASS_FAIL_TYPE Gpio__Validate(GPIO_PORT_TYPE port,uint8 pin)
 {

	if ((port < GPIO_PORT_NUM) && (pin < MAX_PIN))
	{
		return (PASS);
	}
	else
	{
		return (FAIL);
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief 	This method configures a port-bit to a specific state.
 *    @details  Interrupt are disabled before assigning value to register and enabled after
 *              value is assigned
 *    @param 	port: Defines the specific port
 *    @param 	pin : Defines the specific pin
 *    @param 	type: Defines the specific type the selected pin will configure as
 */
void Gpio__PinConfig(GPIO_PORT_TYPE port,uint8 pin,GPIO_PIN_MODE_TYPE type)
 {
	volatile GPIO_DEF * port_ptr;
	volatile PORTx_DEF * pci_ptr;

	port_ptr = (volatile GPIO_DEF *) GPIO_PORT_LIST[port];
	pci_ptr = (volatile PORTx_DEF *) PCI_PORT_LIST[port];

	#if (GPIO_PROTECTION_FEATURE == ENABLED)
		MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
	#endif

	#if (GPIO_PROTECTION_FEATURE == ENABLED)
		MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
	#endif

	if (Gpio__Validate(port, pin) == PASS)
	{
		switch (type)
		{
		case INPUT_PULLDOWN:
			port_ptr->PDDR &= ~(1 << pin);									//Disable Port Data Direction Register - pin configured as general purpose input
			pci_ptr->PCR[pin] = (PORT_PCR_MUX(1) | PORT_PCR_PE_MASK);	//Internal Pull down resistor is enabled on corresponding pin
			break;

		case INPUT_PULLUP:
			port_ptr->PDDR &= ~(1 << pin);									// Disable Port DDR - pin configured as general purpose input
			pci_ptr->PCR[pin] = (PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);//Internal pullup resistor is enabled on the corresponding pin
			break;

		case INPUT_ANALOG:
			port_ptr->PDDR &= ~(1 << pin);									//Disable Port DDR - pin configured as general purpose input
			pci_ptr->PCR[pin] = (PORT_PCR_MUX(0));						//Pin configure as Analog
			break;

		case OUTPUT_PUSHPULL:
			port_ptr->PDDR |= (1 << pin);									//Enable Port Data Direction Register - pin configured as general purpose output
			pci_ptr->PCR[pin] = (PORT_PCR_MUX(1));
			break;

		case SPECIAL_FUNCTION_OPEN_DRAIN:
		case OUTPUT_OPEN_DRAIN:
			port_ptr->PDDR |= (1 << pin);									//Enable Port Data Direction Register - pin configured as general purpose output
			pci_ptr->PCR[pin] = (PORT_PCR_MUX(1) | PORT_PCR_ODE_MASK);	//GPIO with Open Drain Enable
			break;

		case INPUT_FLOAT:
		default:
			port_ptr->PDDR &= ~(1 << pin);									//Disable Port Data Direction Register - pin configured as general purpose input
			pci_ptr->PCR[pin] = (PORT_PCR_MUX(1));						//Internal pullup or pulldown resistor is not enabled on the corresponding pin.
			break;

		}
	}

	#if (GPIO_PROTECTION_FEATURE == ENABLED)
		MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL(); 							// Reestablish the interruption configuration
	#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief 	This method writes on a specific Port-Bit
 *    @param 	port: Defines the specific port
 *    @param 	pin : Defines the specific pin
 *    @param    value: Defines the value to be applied on the port-bit
 */
void Gpio__PinWrite(GPIO_PORT_TYPE port,uint8 pin,BOOL_TYPE value)
 {
	volatile GPIO_DEF * port_ptr;

	port_ptr = GPIO_PORT_LIST[port];

	if (Gpio__Validate(port, pin) == PASS)
	{

		if (value)
		{
			port_ptr->PDOR |= (1 << pin);				//Write logic '1' on selected pin
		}
		else
		{
			port_ptr->PDOR &= ~(1 << pin);				//Write logic '0' on selected pin
		}
	}

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief 	This method reads the a specific Port-bit status
 *    @details 	Reads the PDIR register
 *    @param 	port: Defines the specific port
 *    @param 	pin : Defines the specific pin
 *    @return   result
 *      @retval	TRUE: The port-bit is in high level
 *      @retval FALSE: The port-bit is in low level
 */
BOOL_TYPE Gpio__PinRead(GPIO_PORT_TYPE port,uint8 pin)
 {
	BOOL_TYPE response;
	volatile GPIO_DEF * port_ptr;

	response = FALSE;
	port_ptr = GPIO_PORT_LIST[port];

	if (Gpio__Validate(port, pin) == PASS)
	{

		//This reads from GPIOx_PDIR and returns the input value of the associated/requested port pin.
		if (BIT_TEST(port_ptr->PDIR, pin))
		{
			response = TRUE;
		}
	}

	return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief 	This method configures a port to a specific state.
 *    @details 	By default configures the port to ALL_INPUT_FLOAT mode
 *    @param 	port: Defines the specific port
 *    @param 	type: Defines the specific type the selected port will configure as
 */
void Gpio__PortConfig(GPIO_PORT_TYPE port, GPIO_PIN_MODE_TYPE type)
 {
	uint8 i;
	volatile GPIO_DEF * port_ptr;
	volatile PORTx_DEF * pci_ptr;

	#if (GPIO_PROTECTION_FEATURE == ENABLED)
		MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
	#endif

	#if (GPIO_PROTECTION_FEATURE == ENABLED)
		MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
	#endif

	port_ptr = GPIO_PORT_LIST[port];
	pci_ptr = PCI_PORT_LIST[port];

	// It protects against attempts to write out of the range
	if (Gpio__Validate(port, 0) == PASS)
	{
		switch (type)
		{

		case INPUT_PULLDOWN:
			port_ptr->PDDR = ALL_INPUT;											//Disable Port DDR - All pins are configured as general purpose input
			for (i = 0; i < 32; i++)
			{
				pci_ptr->PCR[i] = (PORT_PCR_MUX(1) | PORT_PCR_PE_MASK);	//Internal Pull down resistor is enabled on corresponding pin
			}
			break;

		case INPUT_PULLUP:
			port_ptr->PDDR = ALL_INPUT;											//Disable Port DDR - All pins are configured as general purpose input
			for (i = 0; i < 32; i++)
			{
				pci_ptr->PCR[i] = (PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);//Internal pullup resistor is enabled on the corresponding pin
			}
			break;

		case INPUT_ANALOG:
			port_ptr->PDDR = ALL_INPUT;											//Disable Port DDR - All pins are configured as general purpose input
			for (i = 0; i < 32; i++)
			{
				pci_ptr->PCR[i] = (PORT_PCR_MUX(0));						//Pin configure as Analog
			}
			break;

		case OUTPUT_PUSHPULL:													// Configures the port in OUTPUT PUSHPULL mode
			port_ptr->PDDR = ALL_OUTPUT;										//Enable Port DDR - All pins are configured as general purpose output
			for (i = 0; i < 32; i++)
			{
				pci_ptr->PCR[i] = (PORT_PCR_MUX(1));
			}
			break;

		case SPECIAL_FUNCTION_OPEN_DRAIN:
		case OUTPUT_OPEN_DRAIN:
			port_ptr->PDDR = ALL_OUTPUT;										//Enable Port DDR - All pins are configured as general purpose output
			for (i = 0; i < 32; i++)
			{
				pci_ptr->PCR[i] = (PORT_PCR_MUX(1) | PORT_PCR_ODE_MASK);	//GPIO with Open Drain Enable
			}
			break;

		case INPUT_FLOAT:
		default:
			port_ptr->PDDR = ALL_INPUT;											//Disable Port DDR - All pins are configured as general purpose input
			for (i = 0; i < 32; i++)
			{
				pci_ptr->PCR[i] = (PORT_PCR_MUX(1));						//Internal pullup or pulldown resistor is not enabled on the corresponding pin.
			}
			break;
		}
	}

	#if (GPIO_PROTECTION_FEATURE == ENABLED)
		MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
	#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief 	This method writes data on a specific port using a mask to define which bits will be affected
 *    @param 	port: Defines the specific port
 *    @param	mask: mask of affected bits (1 = will be affected, 0 = won't be affected)
 *    @param    value: Defines the value to be applied on the port-bit
 */
void Gpio__PortWrite(GPIO_PORT_TYPE port,PORT_DATA_TYPE mask, PORT_DATA_TYPE value)
 {
	volatile GPIO_DEF * port_ptr;

	port_ptr = GPIO_PORT_LIST[port];

	#if (GPIO_PROTECTION_FEATURE == ENABLED)
		MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
	#endif

	#if (GPIO_PROTECTION_FEATURE == ENABLED)				// it generates a sub-mask to clear bits
		MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();				// Save interrupt state and disable that
	#endif

	PORT_DATA_TYPE aux;
	aux = mask & (~value);									// it generates a sub-mask to clear bits
	port_ptr->PDOR &= (~aux);								// It clears the bits supposed to be clear
	aux = mask & value;										// it generates a sub-mask to set bits
	port_ptr->PDOR |= aux;									// it sets the bits supposed to be set

	#if (GPIO_PROTECTION_FEATURE == ENABLED)
		MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();			// Restore the interrupt context saved before
	#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief	This method reads and entire port
 *    @details 	Reads the IDR register
 *    @param	port: Defines the specific port
 *    @return   PORT_DATA_TYPE : The read value from the port requested
 */
PORT_DATA_TYPE Gpio__PortRead(GPIO_PORT_TYPE port)
 {
	volatile GPIO_DEF * port_ptr;

	port_ptr = GPIO_PORT_LIST[port];

	if (Gpio__Validate(port, 0) == PASS)
	{
		//This reads from GPIOx_PDIR and returns the input value of the associated/requested port
		return (port_ptr->PDIR);
	}
	else
	{
		return ((PORT_DATA_TYPE) 0);
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    Check the configuration of a Gpio Port bit
 *    @details  This function is used to determine whether the particular Gpio pin was configured properly.
 *    @details  Port are configured to particular state as par the parameter are passed.
 *              Pin position is obtained by updating table SR_GPIO_PINS_DEF in SRGpio_prm.h
 *    @return   BOOL_TYPE
 *              @retval TRUE(1)  means configuration matches
 *              @retval FALSE(0) means configuration different them the expected.
 */
BOOL_TYPE Gpio__CheckConfig(GPIO_PORT_TYPE port,uint8 pin,GPIO_PIN_MODE_TYPE type, BOOL_TYPE state)
{
    BOOL_TYPE retval = FALSE;
    if (Gpio__Validate(port, pin) == PASS)
    {
        if (GpioCheckPinConfig(port, pin, type))
        {
            if ((type == OUTPUT_PUSHPULL) ||
                (type == OUTPUT_OPEN_DRAIN))
            {
                // Read the actual pin state.
                BOOL_TYPE aux = FALSE;
                volatile GPIO_DEF * port_ptr = GPIO_PORT_LIST[port];
                if (BIT_TEST(port_ptr->PDOR, pin))
                {
                    aux = TRUE;
                }

                // Check if the actual pin state matches the expected pin state.
                if (aux != state)
                {
                    retval = FALSE;
                    //generate an exception when the GPIO data is corrupted
                    SRException__Queue(SREXCEPTION_GPIO_DATA_CORRUPTION, port, pin);
                }
                else
                {
                    retval = TRUE;
                }
            }
            else
            {
                retval = TRUE;                      // If pin is configured other than output then it should return TRUE.
            }
        }
        else
        {
            //generate an exception if the configuration is mismatched
            SRException__Queue(SREXCEPTION_GPIO_CONFIGURATION_CORRUPTION, port, pin);
        }

    }
    return (retval);
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
/**
 *    @brief    Updates a register based on a mask
 *    @param    current
 *    @param    mask
 *    @param    value
 *    @return   uint8 updated value
 */
static uint32 UpdateMaskedRegister(uint32 current,uint32 mask, uint32 value)
{
    uint32 reg;
    uint32 aux;

    aux = mask & (~value);                      //It generates a sub-mask to clear bits
    reg = current & (~aux);
    aux = mask & value;                         //It generates a sub-mask to set bits
    reg = reg | aux;                            //It sets the bits supposed to be set
    return (reg);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    Check the configuration of a Gpio Port bit
 *    @details  This function is used to determine whether the particular Gpio pin was configured properly.
 *    @details  Port are configured to particular state as par the parameter are passed.
 *              Pin position is obtained by updating table SR_GPIO_PINS_DEF in SRGpio_prm.h
 *    @return   BOOL_TYPE
 *              @retval TRUE(1)  means configuration matches
 *              @retval FALSE(0) means configuration different them the expected.
 */
static BOOL_TYPE GpioCheckPinConfig(GPIO_PORT_TYPE port,uint8 pin,GPIO_PIN_MODE_TYPE type)
{
    BOOL_TYPE retval = FALSE;
    volatile GPIO_DEF * port_ptr = GPIO_PORT_LIST[port];

    switch (type)
    {
        case SPECIAL_FUNCTION_OPEN_DRAIN: // What is this type?
        case SPECIAL_FUNCTION_PUSH_PULL: // What is this type?
        case OUTPUT_PUSHPULL:
        case OUTPUT_OPEN_DRAIN:
            retval = (BOOL_TYPE)((port_ptr->PDDR & GPIO_SET_MASK[pin]) != 0);
            break;

        case INPUT_ANALOG:
        case INPUT_FLOAT:
        case INPUT_PULLUP:
        case INPUT_PULLDOWN:
            retval = (BOOL_TYPE)((port_ptr->PDDR & GPIO_SET_MASK[pin]) == 0);
            break;

        default:
            break;
    }
    return (retval);
}

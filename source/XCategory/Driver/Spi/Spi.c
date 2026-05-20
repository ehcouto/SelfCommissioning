/**
 *  @file
 *  @brief      Spi GDM module for MV31x and MKV30x implements the Spi Standard API (Application Programming Interface).
 *  @details    This module provides the synchronous serial communication services to the application,
 *              such as initialization of Spi module, transmission/reception of byte and multiple bytes
 *              via Spi and status of Spi communication.
 * <pre>
 * The Spi0 is available in the below port pins in MKV3x Controllers
 * ------------------------------------------------------         ----------------------------------------
 * |                    MKV3x_FAMILY                    |        |              MKV3x_FAMILY             |
 * |----------------------------------------------------|        |---------------------------------------|
 * |SPI com      | REMAP 0 | REMAP 1 | REMAP 2 |REMAP 3 |        |SPI com     |REMAP 0| REMAP 1 |   NA   |
 * |-------------|---------|---------|---------|--------|        |------------|-------|---------|--------|
 * |      | MOSI |PORTE 19 |PORTA 16 | PORTC 6 |PORTD 2 |        |     | MOSI |PORTE 1| PORTB 16|PORTD 7 |
 * |      |------|---------|---------|---------|--------|        |     |------|-------|---------|--------|
 * |      | MISO |PORTE 18 |PORTA 17 | PORTC 7 |PORTD 3 |        |     | MISO |PORTE 3| PORTB 17|PORTD 6 |
 * | SPI0 |------|---------|---------|---------|--------|        |SPI1 |------|-------|---------|--------|
 * |      | SCK  |PORTE 17 |PORTA 15 | PORTC 5 |PORTD 1 |        |     | SCK  |PORTE 2| PORTB 11|PORTD 5 |
 * |      |------|---------|---------|---------|--------|        |     |------|-------|---------|--------|
 * |      | PCS0*|PORTE 16 |PORTA 14 | PORTC 4 |PORTD 0 |        |     | PCS0*|PORTE 4| PORTB 10|PORTD 4 |
 * |      |------|---------|---------|---------|--------|        |     |------|-------|---------|--------|
 * |      | PCS1*|   N/A   |   N/A   | PORTC 3 |PORTD 4 |        |     | PCS1*|PORTE 0| PORTB 9 |  N/A   |
 * |      |------|---------|---------|---------|--------|        |     |------|-------|---------|--------|
 * |      | PCS2*|   N/A   |   N/A   | PORTC 2 |PORTD 5 |        |     | PCS2*|PORTE 5|   N/A   |  N/A   |
 * |      |------|---------|---------|---------|--------|         ---------------------------------------
 * |      | PCS3*|   N/A   |   N/A   | PORTC 1 |PORTD 6 |
 * |      |------|---------|---------|---------|--------|
 * |      | PCS4*|   N/A   |   N/A   | PORTC 0 |  N/A   |
 * -----------------------------------------------------
 * </pre>
 * * PCSx is used only if SPIy_USE_PCSx is enabled
 *
 *
 *  @section    Applicable_Documents
 *              Requirement ID - 50717 - https://rm.whirlpool.oncloudone.com/rm/web#action=com.ibm.rdm.web.pages.showArtifact&artifactURI=https%3A%2F%2Frm.whirlpool.oncloudone.com%2Frm%2Fresources%2F_f67yIS8REearGv06AJXz5w&vvc.configuration=https%3A%2F%2Frm.whirlpool.oncloudone.com%2Frm%2Fcm%2Fstream%2F_qZzlsCEkEea1esD_TUzLUg&componentURI=https%3A%2F%2Frm.whirlpool.oncloudone.com%2Frm%2Frm-projects%2F_qYFucSEkEea1esD_TUzLUg%2Fcomponents%2F_qZvUQCEkEea1esD_TUzLUg
 *
 *  $Header: Spi.c $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Spi.h"
#include "uc.h"
#include "Micro.h"
#include "Gpio.h"
#include <string.h>
#include "Dma.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//Compiler directive
//=====================================================================================================================
#ifndef SPI0_TX_RX_MODE
    #define SPI0_TX_RX_MODE         ENABLED
#endif

#if (SPI0_BUFFER_SIZE == 0)
    #error : " SPI0_BUFFER_SIZE macro value must be non-zero in the Spi_prm.h file"
#endif

#if (SPI0_REMAP > 3)
    #error : " SPI0_REMAP macro value must be either 0 or 1 in the Spi_prm.h file"
#endif

#ifndef SPI0_USE_PCS0
    #define SPI0_USE_PCS0           DISABLED
#endif
#ifndef SPI0_USE_PCS1
    #define SPI0_USE_PCS1           DISABLED
#endif
#ifndef SPI0_USE_PCS2
    #define SPI0_USE_PCS2           DISABLED
#endif
#ifndef SPI0_USE_PCS3
    #define SPI0_USE_PCS3           DISABLED
#endif
#ifndef SPI0_USE_PCS4
    #define SPI0_USE_PCS4           DISABLED
#endif

// Configure PCS flags, if enabled
#if(SPI0_USE_PCS0 == ENABLED)
    #define SPI0_PCS0_FLAG          0x01U
#else
    #define SPI0_PCS0_FLAG          0x00U
#endif
#if(SPI0_USE_PCS1 == ENABLED)
    #define SPI0_PCS1_FLAG          0x02U
#else
    #define SPI0_PCS1_FLAG          0x00U
#endif
#if(SPI0_USE_PCS2 == ENABLED)
    #define SPI0_PCS2_FLAG          0x04U
#else
    #define SPI0_PCS2_FLAG          0x00U
#endif
#if(SPI0_USE_PCS3 == ENABLED)
    #define SPI0_PCS3_FLAG          0x08U
#else
    #define SPI0_PCS3_FLAG          0x00U
#endif
#if(SPI0_USE_PCS4 == ENABLED)
    #define SPI0_PCS4_FLAG          0x10U
#else
    #define SPI0_PCS4_FLAG          0x00U
#endif
#define SPI0_PCS_MASK               (SPI0_PCS0_FLAG | SPI0_PCS1_FLAG | SPI0_PCS2_FLAG | SPI0_PCS3_FLAG | SPI0_PCS4_FLAG)


// Configure PCSIS flags, if enabled
#if(SPI0_INVERT_PCS == ENABLE)
    #define SPI0_PCSIS_MASK     SPI0_PCS_MASK
#else
    #define SPI0_PCSIS_MASK     0U
#endif


//=====================================================================================================================
// Spi and Dma Configuration
//=====================================================================================================================
#define DSPI_MASTER_BASEADDR                            SPI_0
#define DSPI_MASTER_DMA_RX_REQUEST_SOURCE               kDmaRequestMux0SPI0Rx
#define DSPI_MASTER_DMA_TX_REQUEST_SOURCE               kDmaRequestMux0SPI0Tx
#define DSPI_MASTER_PCS_FOR_INIT                        kDSPI_Pcs0
#define DSPI_MASTER_PCS_FOR_TRANSFER                    kDSPI_MasterPcs0
#define DSPI_MASTER_DMA_MUX_BASEADDR                    ((DMAMUX_Type *)(EXAMPLE_DSPI_MASTER_DMA_MUX_BASE))
#define TRANSFER_baud_rate                               6000000U /*! Transfer baud_rate - 6M */
#define DSPI_CTAR0                                      0U  //DSPI Clock and Transfer Attributes Register (CTAR) selection.
//DSPI Peripheral Chip Select (Pcs) Polarity configuration
#define DSPI_PCS_ACTIVE_HIGH                            SPI0_PCSIS_MASK
#define DSPI_SCK_TO_SIN0_CLOCK                          0U /*!< 0 system clocks between SCK edge and SIN sample.*/
#define DSPI_MASTER_PCS_CONTINUOS                       1U << 20      /*!< Indicates whether the PCS signal is continuous. */
#define DSPI_MASTER_ACTIVE_AFTER_TRANSFER               1U << 21 /*!< Indicates whether the PCS signal is active after the last frame transfer.*/
#define FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(x) ((x) == DSPI0 ? (1) : \
                                                        ((x) == DSPI1 ? (0) : (-1)))  /* @brief Has separate DMA RX and TX requests. */

#define FSL_FEATURE_DSPI_FIFO_SIZEn(x)                  ((x) == DSPI0 ? (4) : \
                                                        ((x) == DSPI1 ? (1) : (-1))) //specifies the size for the fifo buffer

#define DSPI_BUSY                                       1
#define DSPI_DUMMY_DATA                                 0

#if ((SPI0_FEATURE == ENABLED) || (SPI1_FEATURE == ENABLED))
    #define SPI_DMA_FEATURE                             ENABLED
#else
    #define SPI_DMA_FEATURE                             DISABLED
#endif
#define DSPI_MASTER                                     1

// Set CONT flag if PCS lines are used
#if(SPI0_PCS_MASK != 0)
    #define HANDLE_COMMAND                              (0x80000000U | SPI0_PCS_MASK)
#else
    #define HANDLE_COMMAND                              (0x00000000U)
#endif

// Set EOQ flag if END Callback is used
#if defined(SPI0_END_CALLBACK)
    #define HANDLE_LAST_COMMAND                         (0x08000000U)
#else
    #define HANDLE_LAST_COMMAND                         (0x00000000U)
#endif

//=====================================================================================================================
//Variable section
//=====================================================================================================================
/* Defines constant value arrays for the baud rate pre-scalar and scalar divider values.*/
static const uint32 BAUD_RATE_PRESCALAR[] = {2, 3, 5, 7};
static const uint32 BAUD_RATE_SCALAR[] = {2,4,6,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
/*! @brief DSPI clock polarity configuration for a given CTAR.*/
typedef enum
{
    DSPI_CLOCK_POLARITY_ACTIVE_HIGH = 0U, /*!< CPOL=0. Active-high DSPI clock (idles low).*/
    DSPI_CLOCK_POLARITY_ACTIVE_LOW = 1U   /*!< CPOL=1. Active-low DSPI clock (idles high).*/
} DSPI_CLOCK_POLARITY;

/*! @brief DSPI clock phase configuration for a given CTAR.*/
typedef enum
{
    DSPI_CLOCK_PHASE_FIRST_EDGE = 0U, /*!< CPHA=0. Data is captured on the leading edge of the SCK and changed on the
                                         following edge.*/
    DSPI_ClLOCK_PHASE_SECOND_EDGE = 1U /*!< CPHA=1. Data is changed on the leading edge of the SCK and captured on the
                                        following edge.*/
} DSPI_CLOCK_PHASE;

/*! @brief DSPI data shifter direction options for a given CTAR.*/
typedef enum
{
    DSPI_MSB_FIRST = 0U, /*!< Data transfers start with the most significant bit.*/
    DSPI_LSB_FIRST = 1U  /*!< Data transfers start with the least significant bit.*/
} DSPI_SHIFT_DIRECTION;

/*! @brief eDMA transfer configuration */
typedef enum
{
    DMA_TRANSFER_SIZE_ONE_BYTE = 0x0U,  /*!< Source/Destination data transfer size is 1 byte every time */
    DMA_TRANSFER_SIZE_TWO_BYTE = 0x1U,  /*!< Source/Destination data transfer size is 2 bytes every time */
    DMA_TRANSFER_SIZE_FOUR_BYTE = 0x2U,  /*!< Source/Destination data transfer size is 4 bytes every time */
    DMA_TRANSFER_SIZE_SIXTEEN_BYTE = 0x4U, /*!< Source/Destination data transfer size is 16 bytes every time */
    DMA_TRANSFER_SIZE_THIRTY_TWO_BYTE = 0x5U, /*!< Source/Destination data transfer size is 32 bytes every time */
} EDMA_TRANSFER_SIZE;

/*!
 * @brief eDMA TCD.
 *
 * This structure is the same as the TCD register, which is described in the chip reference manual
 * and is used to configure scatter/gather feature as a next hardware TCD.
 */
typedef struct
{
    volatile uint32 SADDR;     /*!< SADDR register, used to save source address */
    volatile uint16 SOFF;      /*!< SOFF register, save offset bytes every transfer */
    volatile uint16 ATTR;      /*!< ATTR register, source/destination transfer size and modulo */
    volatile uint32 NBYTES;    /*!< Nbytes register, minor loop length in bytes */
    volatile uint32 SLAST;     /*!< SLAST register */
    volatile uint32 DADDR;     /*!< DADDR register, used for destination address */
    volatile uint16 DOFF;      /*!< DOFF register, used for destination offset */
    volatile uint16 CITER;     /*!< CITER register, current minor loop numbers, for unfinished minor loop.*/
    volatile uint32 DLAST_SGA; /*!< DLASTSGA register, next stcd address used in scatter-gather mode */
    volatile uint16 CSR;       /*!< CSR register, for TCD control status */
    volatile uint16 BITER;     /*!< BITER register, begin minor loop count. */
} EDMA_TCD;
/*! @brief eDMA transfer handle structure */
typedef struct
{
    void *userdata;          /*!< Callback function parameter. */
    DMA_TYPE *base;          /*!< eDMA peripheral base address. */
    EDMA_TCD *tcd_pool;     /*!< Pointer to memory stored TCDs. */
    uint8 channel;         /*!< eDMA channel number. */
    volatile uint8 header;  /*!< The first TCD index. */
    volatile uint8 tail;    /*!< The last TCD index. */
    volatile uint8 tcd_used; /*!< The number of used TCD slots. */
    volatile uint8 tcd_size; /*!< The total number of TCD slots in the queue. */
    uint8 flags;           /*!< The status of the current channel. */
} EDMA_HANDLE;
/*!
 * @brief eDMA TCD.
 *
 * This structure is the same as the TCD register, which is described in the chip reference manual
 * and is used to configure scatter/gather feature as a next hardware TCD.
 */
/*! @brief DSPI master eDMA transfer handle structure used for the transactional API. */
typedef struct
{
    uint32 bits_per_frame;         /*!< The desired number of bits per frame. */
    volatile uint32 command;     /*!< The desired data command. */
    volatile uint32 last_command; /*!< The desired last data command. */

    uint8 fifoSize; /*!< FIFO data_size. */

    BOOL_TYPE is_pcs_active_after_transfer; /*!< Indicates whether the PCS signal keeps active after the last frame transfer.*/
    BOOL_TYPE is_there_extra_byte;         /*!< Indicates whether there is an extra byte.*/

    uint8 *volatile tx_data;                  /*!< Send buffer. */
    uint8 *volatile rx_data;                  /*!< Receive buffer. */
    volatile size_t remaining_send_byte_count;    /*!< A number of bytes remaining to send.*/
    volatile size_t remaining_receive_byte_count; /*!< A number of bytes remaining to receive.*/
    size_t total_byte_count;                     /*!< A number of transfer bytes*/

    uint32 rx_buff_If_null; /*!< Used if there is not rx_data for DMA purpose.*/
    uint32 tx_buff_if_null; /*!< Used if there is not tx_data for DMA purpose.*/

    volatile uint8 state; /*!< DSPI transfer state, see _dspi_transfer_state.*/

    void *userdata;                                /*!< Callback user data. */

    EDMA_HANDLE *edma_rx_reg_to_rx_data_handle;        /*!<EDMA_HANDLE_t handle point used for RxReg to rx_data buff*/
    EDMA_HANDLE *edma_tx_data_to_intermediary_handle; /*!<EDMA_HANDLE_t handle point used for tx_data to Intermediary*/
    EDMA_HANDLE *edma_intermediary_to_tx_reg_handle;  /*!<EDMA_HANDLE_t handle point used for Intermediary to TxReg*/

    EDMA_TCD dspi_software_tcd[2]; /*!<software_tcd , internal used*/
}SPI_MASTER_DMA_HANDLE;
/*! @brief DSPI master/slave transfer structure.*/
typedef struct DSPI_TRANSFER
{
    uint8 *tx_data;          /*!< Send buffer. */
    uint8 *rx_data;          /*!< Receive buffer. */
    volatile size_t data_size; /*!< Transfer bytes. */

    uint32 config_flags; /*!< Transfer transfer configuration flags; set from _dspi_transfer_config_flag_for_master if the
                        transfer is used for master or _dspi_transfer_config_flag_for_slave enumeration if the transfer
                        is used for slave.*/
} dspi_transfer;
/*!
 * @brief eDMA transfer configuration
 *
 * This structure configures the source/destination transfer attribute.
 * This figure shows the eDMA's transfer model:
 *  _________________________________________________
 *              | Transfer Size |                    |
 *   Minor Loop |_______________| Major loop Count 1 |
 *     Bytes    | Transfer Size |                    |
 *  ____________|_______________|____________________|--> Minor loop complete
 *               ____________________________________
 *              |               |                    |
 *              |_______________| Major Loop Count 2 |
 *              |               |                    |
 *              |_______________|____________________|--> Minor loop  Complete
 *
 *               ---------------------------------------------------------> Transfer complete
 */
/*! @brief DSPI master command data configuration used for the SPIx_PUSHR.*/
typedef struct
{
    BOOL_TYPE is_pcs_continuous;            /*!< Option to enable the continuous assertion of the chip select between transfers.*/
    BOOL_TYPE is_end_of_queue;               /*!< Signals that the current transfer is the last in the queue.*/
    BOOL_TYPE clear_transfer_count;         /*!< Clears the SPI Transfer Counter (SPI_TCNT) before transmission starts.*/
} DSPI_COMMAND_DATA_CONFIG;

/*! @brief DSPI master transfer data configuration used for the transfer*/
typedef struct
{
    uint32 src_addr;                      /*!< Source data address. */
    uint32 dest_addr;                     /*!< Destination data address. */
    EDMA_TRANSFER_SIZE source_transfer_size;  /*!< Source data transfer size. */
    EDMA_TRANSFER_SIZE dest_transfer_size; /*!< Destination data transfer size. */
    uint16 source_offset;                     /*!< Sign-extended offset applied to the current source address to
                                                form the next-state value as each source read is completed. */
    uint16 dest_offset;                    /*!< Sign-extended offset applied to the current destination address to
                                                form the next-state value as each destination write is completed. */
    uint32 minor_loop_bytes;               /*!< Bytes to transfer in a minor loop*/
    uint32 major_loop_counts;              /*!< Major loop iteration count. */
}EDMA_TRANSFER_CONFIG;

/*! @brief DSPI master ctar configuration structure.*/
typedef struct
{
    uint32 baud_rate;                /*!< Baud Rate for DSPI. */
    uint32 bits_per_frame;            /*!< Bits per frame, minimum 4, maximum 16.*/
    DSPI_CLOCK_POLARITY cpol;       /*!< Clock polarity. */
    DSPI_CLOCK_PHASE cpha;          /*!< Clock phase. */
    DSPI_SHIFT_DIRECTION direction; /*!< MSB or LSB data shift direction. */

    uint32 pcs_to_sck_delay_in_nano_sec;        /*!< PCS to SCK delay time in nanoseconds; setting to 0 sets the minimum
                                               delay. It also sets the boundary value if out of range.*/
    uint32 last_sck_to_pcs_delay_nano_sec;    /*!< The last SCK to PCS delay time in nanoseconds; setting to 0 sets the
                                               minimum delay. It also sets the boundary value if out of range.*/

    uint32 between_transfer_delay_in_nano_sec; /*!< After the SCK delay time in nanoseconds; setting to 0 sets the minimum
                                             delay. It also sets the boundary value if out of range.*/
}DSPI_MASTER_CTAR_CONFIG;

/*! @brief DSPI master configuration structure.*/
typedef struct
{
    DSPI_MASTER_CTAR_CONFIG ctar_config; /*!< Set the ctar_config to the desired CTAR. */
    BOOL_TYPE enable_continuos_sck;   /*!< CONT_SCKE, continuous SCK enable. Note that the continuous SCK is only
                                     supported for CPHA = 1.*/
    BOOL_TYPE enable_rx_fifo_over_write; /*!< ROOE, receive FIFO overflow overwrite enable. If ROOE = 0, the incoming
                                     data is ignored and the data from the transfer that generated the overflow
                                     is also ignored. If ROOE = 1, the incoming data is shifted to the
                                     shift register. */

    BOOL_TYPE enable_modified_timing_format;        /*!< Enables a modified transfer format to be used if true.*/
}DSPI_MASTER_CONFIG;

#if ((SPI0_BUFFER_SIZE > 255) || (SPI1_BUFFER_SIZE > 255))
    typedef uint16 SPI_BUFFER_TYPE;
#else
    typedef uint8 SPI_BUFFER_TYPE;
#endif

#if (SPI0_FEATURE == ENABLED)
    static uint8 Spi0_Tx_Buffer[SPI0_BUFFER_SIZE];
    static SPI_BUFFER_TYPE Spi0_Tx_Write;
    static SPI_BUFFER_TYPE Spi0_Tx_Read;
    static BOOL_TYPE Spi0_Busy;

    #if (SPI0_TX_RX_MODE == ENABLED)
        static uint8 Spi0_Rx_Buffer[SPI0_BUFFER_SIZE];
        static SPI_BUFFER_TYPE Spi0_Rx_Write;
        static SPI_BUFFER_TYPE Spi0_Rx_Read;
    #endif
#endif

static    uint32 masterRxChannel , masterTxChannel;
static    DSPI_MASTER_CONFIG masterConfig;

#if(SPI_DMA_FEATURE==ENABLED)

    static uint16 word_to_send;
    static uint8 dummy_data;
    static uint32 rxAddr;
    static uint32 txAddr;
    static uint32 tmpreg;

    static SPI_MASTER_DMA_HANDLE  Spi_Master_Dma_Handle;
    static dspi_transfer master_transfer;



    static EDMA_TRANSFER_CONFIG transfer_config_a;
    static EDMA_TRANSFER_CONFIG transfer_config_b;
    static EDMA_TRANSFER_CONFIG transfer_config_c;

#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
void DspiMasterTransferEdma(SPI_TYPE *base, SPI_MASTER_DMA_HANDLE *handle, dspi_transfer *transfer);
void DspiSetConfig(DMA_ENGINE_TYPE dma_engine,DMA_CHANNEL_TYPE channel,EDMA_TRANSFER_CONFIG *transfer_config, EDMA_TCD *next_tcd);
void EdmaTcdReset( EDMA_TCD *tcd);
void EdmaTcdSetTransferConfig( EDMA_TCD *tcd, const EDMA_TRANSFER_CONFIG *config);
void EdmaStartTransfer(EDMA_HANDLE *handle);
void DspiMasterInit(SPI_TYPE *base, const DSPI_MASTER_CONFIG *masterConfig, uint32 src_clock_hz);
uint32 DspiMasterSetBaudrate(SPI_TYPE *base,uint32  which_ctar,uint32 baud_rate_bps,uint32 src_clock_hz);
void   DspiDisable(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes a specific SPI module by reading the Spi Configuration parameters
 *              defined in the Spi_prm.h file.
 *    @details  Before using Spi module this method should be called to initialize specific Spi module.
 *              To make use of this API the following configuration macros must be defined in the Spi_prm.h file-
 *              "#define SPI_x_pre_scaler"\n
 *              "#define SPI_x_LSBFIRST"\n
 *              "#define SPI_x_POL"\n
 *              "#define SPI_x_CPHA_EDGE"
 *    @param    spi : specific SPI module
 *    @image    html    Spi__Initialize.jpg
 */
void Spi__Initialize(SPI_ENUM_DEF spi)
{

    Spi__Clear(spi);                                                        //Clear the buffers

    #if (SPI0_FEATURE == ENABLED)
        if (spi == SPI0)                                                    //Initialize SPI0
        {
            SIM.SCGC6 |= SIM_SCGC6_SPI0_MASK;                               //Enable the clock for Spi0 module

            if (SPI0_REMAP == 0)
            {
                PCIPORTE.PCR[17] = PORT_PCR_MUX(2);                         //Set PortE pin 17 of SPI0 com: SCK
                PCIPORTE.PCR[18] = PORT_PCR_MUX(2);                         //Set PortE pin 18 of SPI0 com: SOUT(MOSI)
                PCIPORTE.PCR[19] = PORT_PCR_MUX(2);                         //Set PortE pin 19 of SPI0 com: SIN (MISO)
                #if(SPI0_USE_PCS0 == ENABLED)
                    PCIPORTE.PCR[16] = PORT_PCR_MUX(2);                     //Set PortE pin 16 of SPI0 com: PCS
                #endif
            }
            else if (SPI0_REMAP == 1)
            {
                PCIPORTA.PCR[15] = PORT_PCR_MUX(2);                         //Set PortA pin 15 of SPI0 com: SCK
                PCIPORTA.PCR[16] = PORT_PCR_MUX(2);                         //Set PortA pin 16 of SPI0 com: SOUT(MOSI)
                PCIPORTA.PCR[17] = PORT_PCR_MUX(2);                         //Set PortA pin 17 of SPI0 com: SIN (MISO)
                #if(SPI0_USE_PCS0 == ENABLED)
                    PCIPORTA.PCR[14] = PORT_PCR_MUX(2);                     //Set PortA pin 14 of SPI0 com: PCS
                #endif
            }
            else if (SPI0_REMAP == 2)
            {
                PCIPORTC.PCR[5] = PORT_PCR_MUX(2);                          //Set PortC pin 5 of SPI0 com: SCK
                PCIPORTC.PCR[6] = PORT_PCR_MUX(2);                          //Set PortC pin 6 of SPI0 com: SOUT(MOSI)
                PCIPORTC.PCR[7] = PORT_PCR_MUX(2);                          //Set PortC pin 7 of SPI0 com: SIN (MISO)
                #if(SPI0_USE_PCS0 == ENABLED)
                    PCIPORTC.PCR[4] = PORT_PCR_MUX(2);                      //Set PortC pin 4 of SPI0 com: PCS
                #elif(SPI0_USE_PCS1 == ENABLED)
                    PCIPORTC.PCR[3] = PORT_PCR_MUX(2);                      //Set PortC pin 3 of SPI0 com: PCS
                #elif(SPI0_USE_PCS2 == ENABLED)
                    PCIPORTC.PCR[2] = PORT_PCR_MUX(2);                      //Set PortC pin 2 of SPI0 com: PCS
                #elif(SPI0_USE_PCS3 == ENABLED)
                    PCIPORTC.PCR[1] = PORT_PCR_MUX(2);                      //Set PortC pin 1 of SPI0 com: PCS
                #elif(SPI0_USE_PCS4 == ENABLED)
                    PCIPORTC.PCR[0] = PORT_PCR_MUX(2);                      //Set PortC pin 0 of SPI0 com: PCS
                #endif
            }
            else
            {
                PCIPORTD.PCR[1] = PORT_PCR_MUX(2);                          //Set PortD pin 1 of SPI0 com: SCK
                PCIPORTD.PCR[2] = PORT_PCR_MUX(2);                          //Set PortD pin 2 of SPI0 com: SOUT(MOSI)
                PCIPORTD.PCR[3] = PORT_PCR_MUX(2);                          //Set PortD pin 3 of SPI0 com: SIN (MISO)
                #if(SPI0_USE_PCS0 == ENABLED)
                    PCIPORTD.PCR[0] = PORT_PCR_MUX(2);                      //Set PortD pin 0 of SPI0 com: PCS
                #elif(SPI0_USE_PCS1 == ENABLED)
                    PCIPORTD.PCR[4] = PORT_PCR_MUX(2);                      //Set PortD pin 4 of SPI0 com: PCS
                #elif(SPI0_USE_PCS2 == ENABLED)
                    PCIPORTD.PCR[5] = PORT_PCR_MUX(2);                      //Set PortD pin 5 of SPI0 com: PCS
                #elif(SPI0_USE_PCS3 == ENABLED)
                    PCIPORTD.PCR[6] = PORT_PCR_MUX(2);                      //Set PortD pin 6 of SPI0 com: PCS
                #endif
            }
        }

        #if(SPI_DMA_FEATURE==ENABLED)
            masterRxChannel = 0U;
            masterTxChannel = 2U;
            /*Master config*/
            masterConfig.ctar_config.baud_rate = TRANSFER_baud_rate;
            masterConfig.ctar_config.bits_per_frame = 8U;
            masterConfig.ctar_config.cpol = DSPI_CLOCK_POLARITY_ACTIVE_HIGH;
            masterConfig.ctar_config.cpha = DSPI_CLOCK_PHASE_FIRST_EDGE;
            masterConfig.ctar_config.direction = DSPI_MSB_FIRST;
            masterConfig.ctar_config.pcs_to_sck_delay_in_nano_sec = 1000000000U / TRANSFER_baud_rate;
            masterConfig.ctar_config.last_sck_to_pcs_delay_nano_sec = 1000000000U / TRANSFER_baud_rate;
            masterConfig.ctar_config.between_transfer_delay_in_nano_sec = 1000000000U / TRANSFER_baud_rate;
            masterConfig.enable_continuos_sck = FALSE;
            masterConfig.enable_rx_fifo_over_write = FALSE;
            masterConfig.enable_modified_timing_format = FALSE;
            /* DMA MUX init */
            SIM.SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
            DMAMUX->CHCFG[masterRxChannel] = ((DMAMUX->CHCFG[masterRxChannel] & ~DMAMUX_CHCFG_SOURCE_MASK) | DMAMUX_CHCFG_SOURCE(DSPI_MASTER_DMA_RX_REQUEST_SOURCE));
            DMAMUX->CHCFG[masterRxChannel] |= DMAMUX_CHCFG_ENBL_MASK;
            DMAMUX->CHCFG[masterTxChannel] = ((DMAMUX->CHCFG[masterTxChannel] & ~DMAMUX_CHCFG_SOURCE_MASK)| DMAMUX_CHCFG_SOURCE(DSPI_MASTER_DMA_TX_REQUEST_SOURCE));
            DMAMUX->CHCFG[masterTxChannel] |= DMAMUX_CHCFG_ENBL_MASK;

            DspiMasterInit(SPI_0, &masterConfig, Micro__GetClock()->SYSCLK_Frequency);
            Spi0_Busy = FALSE;

        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It enables a specific Spi device.
 *    @details  This method should be called after disabling the Spi module.
 *    @param    spi : specific spi device
 */
void Spi__Enable(SPI_ENUM_DEF spi)
{
    #if (SPI0_FEATURE == ENABLED)
        if (spi == SPI0)
        {
            SPI_0->MCR &= ~SPI_MCR_HALT_MASK;
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It disables a specific Spi device.
 *    @details  The Spi must be enabled by calling Spi_Enable() after disable before performing the Spi communication.
 *    @param    spi : specific spi device
 */
void Spi__Disable(SPI_ENUM_DEF spi)
{
    #if (SPI0_FEATURE == ENABLED)
        if (spi == SPI0)
        {
            SPI_0->MCR |= SPI_MCR_HALT_MASK;
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method clears transmit and receive buffers of a specific Spi.
 *    @param    spi : specific spi device
 *    @image    html    Spi__Clear.jpg
 */
void Spi__Clear(SPI_ENUM_DEF spi)
{
	#if (SPI0_FEATURE == ENABLED)
		if (spi == SPI0)
		{
			Spi0_Tx_Write = 0;
			Spi0_Tx_Read = 0;
			Spi0_Busy=FALSE;

			#if (SPI0_TX_RX_MODE == ENABLED)
				Spi0_Rx_Write = 0;
				Spi0_Rx_Read = 0;
			#endif

			Spi0_Busy = FALSE;

		}
	#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes data from "source" (buffer address mentioned by the Application)
 *              to the "destination" (Transmitter FIFO) as per the size mentioned to the specific Spi and also
 *              returns the response for the write operation.
 *    @details  The data in the Transmitter FIFO is sent to other Spi node through the specified Spi COM port
 *              whenever the Transmitter is free. Before writing into the Transmitter FIFO check for the
 *              FIFO availability using Spi__GetStatus() API to avoid the over writing on the existing data.
 *    @param    spi  : specific Spi device
 *    @param    buf  : source buffer from where the data to be sent
 *    @param    size : defines the amount of data to be sent
 *      @return   uint8
 *      @retval   True  : desired request is successfully executed
 *                False : desired request is not successfully executed
 *    @image    html    Spi__Write.jpg
 */
BOOL_TYPE Spi__Write(SPI_ENUM_DEF spi ,uint8 * buf ,uint16 size )
{
    BOOL_TYPE response;
    response = FALSE;

    #if ((SPI0_FEATURE == ENABLED) && (SPI_DMA_FEATURE==ENABLED))
        if (spi == SPI0)
        {
            Spi0_Busy=TRUE;
            #ifdef SPI0_START_CALLBACK
                SPI0_START_CALLBACK();
            #endif

            /* Start master transfer */
            memcpy(Spi0_Tx_Buffer, buf, size);
            master_transfer.tx_data = Spi0_Tx_Buffer;
            master_transfer.rx_data = Spi0_Rx_Buffer;
            master_transfer.data_size = size;
            Spi0_Rx_Write=master_transfer.data_size;


            #ifdef SPI0_END_CALLBACK
                        SPI_0->RSER |= SPI_RSER_EOQF_RE_MASK;
                        Micro__NVICEnableIRQ(SPI0_IRQ_CHANNEL, SPI0_INTERRUPT_PREEMPTION_PRIORITY, SPI0_INTERRUPT_SUB_PRIORITY);
            #endif
            DspiMasterTransferEdma(SPI_0, &Spi_Master_Dma_Handle, &master_transfer);
            #ifndef SPI0_END_CALLBACK
                Spi0_Busy=FALSE;
            #endif
            response = TRUE;
        }
    #endif

    return(response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads data from "receive FIFO" and copies to the "address of the buffer" mentioned by the
 *              Application as per the size and also returns the response for the read operation.
 *    @details  Before reading the Receiver FIFO check for the
 *              data availability in the Receiver FIFO using Spi__GetStatus() API.
 *    @param    spi : specific spi device
 *    @param    buf : Indicates the buffer’s address where the read data is stored
 *    @param    size : defines the amount of data to be read
 *    @return   True(1)   : desired request is successfully executed
 *              False(0)  : desired request is not successfully executed
 *    @image    html    Spi__Read.jpg
 */
BOOL_TYPE Spi__Read(SPI_ENUM_DEF spi, uint8 * buf, uint16 size)
 {
	BOOL_TYPE response;
	response = FALSE;

	#if (SPI0_FEATURE == ENABLED)					//Read SPI0
		if (spi == SPI0)
		{
			if (size <= (Spi0_Rx_Write - Spi0_Rx_Read))
			{
				memcpy(buf, (Spi0_Rx_Buffer + Spi0_Rx_Read), size);

				Spi0_Rx_Read = Spi0_Rx_Read + size;

				response = TRUE;
			}

		}
	#endif

		return (response);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method gets the status of a specific SPI device.
 *    @details  This method is used to get status from specific SPI module.
 *    @param    spi : specific spi device
 *    @param    index : Index would be SPI_STATUS_STATE or SPI_STATUS_BUFFER_IN
 *    @return   For SPI_STATUS_STATE the return would be-
 *               SPI_STATE_BUSY – The Spi communication is going on
 *               SPI_STATE_IDLE – The Spi is ready to transfer the data
 *               SPI_STATE_ERROR – The Spi module encounter an error
 *               For SPI_STATUS_BUFFER_IN it returns the number of data bytes available to read
 */
uint16 Spi__GetStatus(SPI_ENUM_DEF spi, SPI_STATUS_DEF index)
{
	uint16 response;

    response = FALSE;
	#if (SPI0_FEATURE == ENABLED)
		if(spi == SPI0)
		{
			if (index == SPI_STATUS_STATE)
			{
				if (Spi0_Busy == FALSE)
				{
					response = SPI_STATE_IDLE;
				}
				else
				{
					response = SPI_STATE_BUSY;
				}
			}
			#if (SPI0_TX_RX_MODE == ENABLED)
				else
				{
					response = Spi0_Rx_Write - Spi0_Rx_Read;
				}
			#endif
		}

	#endif

    return (response);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Reconfigure a specific Spi for a customised setting
 *
 *  @details    This function changes the configureation of a specific SPI and must called after the Spi has being initialzie
 *              Every time that the Spi is initialized it starts with the default parameters
 *
 *  @param      spi the specific spi device
 *  @param      pol defines the polarity of SCK signal
 *  @param      edge defines the the edge of action for the SCK line
 *  @param      order defines if the MSB bit goes first or the LSB bit goes first
 *  @image    html    Spi__SetConfiguration.jpg
 */
void Spi__SetConfiguration(SPI_ENUM_DEF spi, SPI_POLARITY_TYPE pol,SPI_EDGE_TYPE edge, SPI_ORDER_TYPE order)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Spi0 Interrupt handler
 *
 *  @details
 *
 *
 */
void Spi__Interrupt0(void)
{
    #if (SPI0_FEATURE == ENABLED)
    if ((SPI_0->SR & SPI_SR_EOQF_MASK))     // Data register empty interrupt
    {
        SPI_0->SR |= SPI_SR_EOQF_MASK;
        #if (SPI_DMA_FEATURE==ENABLED)
            SPI_0->RSER &= ~SPI_SR_EOQF_MASK;
            #ifdef SPI0_END_CALLBACK
                SPI0_END_CALLBACK();
                Spi0_Busy = FALSE;
            #endif
        #endif
    }
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API is used for SPI DMA Master communication
 *
 *  @details    This function  must be called to complete the SPI DMA configuration either for receiver and transmitter
 *
 *  @param      base     defines  the base address of device
 *  @param      handle   defines DSPI master eDMA transfer handle structure variable  used for the transactional API
 *  @param      transfer defines DSPI master/slave transfer structure
*/
void DspiMasterTransferEdma(SPI_TYPE *base, SPI_MASTER_DMA_HANDLE *handle, dspi_transfer *transfer)
{
    #if(SPI_DMA_FEATURE==ENABLED)
        word_to_send = 0;
        dummy_data = 0;
        rxAddr =(uint32)&(base->POPR);
        txAddr =(uint32)&(base->PUSHR);
        EDMA_TCD *software_tcd = ( EDMA_TCD*)((uint32)(&handle->dspi_software_tcd[1]) & (~0x1FU));
        handle->tx_buff_if_null = ((uint32)DSPI_DUMMY_DATA << 8) | DSPI_DUMMY_DATA;
        handle->state = DSPI_BUSY;
        base->MCR |= SPI_MCR_HALT_MASK;   //DSPI stop transfer
        base->MCR = (base->MCR & (~(SPI_MCR_CLR_TXF_MASK | SPI_MCR_CLR_RXF_MASK))) | SPI_MCR_CLR_TXF(TRUE) |
                    SPI_MCR_CLR_RXF(TRUE); //flush fifo
        base->SR = SPI_SR_TCF_MASK | SPI_SR_EOQF_MASK | SPI_SR_TFUF_MASK | SPI_SR_TFFF_MASK | SPI_SR_RFOF_MASK |SPI_SR_RFDF_MASK | SPI_SR_TXRXS_MASK;/*!< The status flags are cleared by writing 1 (w1c).*/
        handle->command=(uint32)HANDLE_COMMAND;
        handle->last_command=(uint32)HANDLE_LAST_COMMAND;
        handle->bits_per_frame = ((base->CTAR[DSPI_CTAR0] & SPI_CTAR_FMSZ_MASK) >> SPI_CTAR_FMSZ_SHIFT) + 1;
        if ((base->MCR & SPI_MCR_DIS_RXF_MASK) || (base->MCR & SPI_MCR_DIS_TXF_MASK))
        {
            handle->fifoSize = 1;
        }
        else
        {
            handle->fifoSize = FSL_FEATURE_DSPI_FIFO_SIZEn(base);
        }
        handle->tx_data = transfer->tx_data;
        handle->rx_data = transfer->rx_data;
        handle->remaining_send_byte_count = transfer->data_size;
        handle->remaining_receive_byte_count = transfer->data_size;
        handle->total_byte_count = transfer->data_size;
        base->RSER &= ~(SPI_RSER_TFFF_RE_MASK | SPI_RSER_TFFF_DIRS_MASK | SPI_RSER_RFDF_RE_MASK | SPI_RSER_RFDF_DIRS_MASK);
        handle->is_there_extra_byte = FALSE;
    #endif

    #if (SPI0_FEATURE == ENABLED)
        if (handle->bits_per_frame > 8)
        {
            /* If it's the last word */
            if (handle->remaining_send_byte_count <= 2)
            {
                if (handle->tx_data)
                {
                    if (handle->is_there_extra_byte)
                    {
                        word_to_send = *(handle->tx_data) | ((uint32) dummy_data << 8);
                    }
                    else
                    {
                        word_to_send = *(handle->tx_data);
                        ++handle->tx_data; /* increment to next data byte */
                        word_to_send |= (unsigned) (*(handle->tx_data)) << 8U;
                    }
                }
                else
                {
                    word_to_send = ((uint32) dummy_data << 8) | dummy_data;
                }
                handle->last_command = (handle->last_command & 0xffff0000U) | word_to_send;
            }
            else /* For all words except the last word , frame > 8bits */
            {
                if (handle->tx_data)
                {
                    word_to_send = *(handle->tx_data);
                    ++handle->tx_data; /* increment to next data byte */
                    word_to_send |= (unsigned) (*(handle->tx_data)) << 8U;
                    ++handle->tx_data; /* increment to next data byte */
                }
                else
                {
                    word_to_send = ((uint32) dummy_data << 8) | dummy_data;
                }
                handle->command = (handle->command & 0xffff0000U) | word_to_send;
            }
        }
        else /* Optimized for bits/frame less than or equal to one byte. */
        {
            if (handle->tx_data)
            {
                word_to_send = *(handle->tx_data);
                ++handle->tx_data; /* increment to next data word*/
            }
            else
            {
                word_to_send = dummy_data;
            }

            if (handle->remaining_send_byte_count == 1)
            {
                handle->last_command = (handle->last_command & 0xffff0000U) | word_to_send;
            }
            else
            {
                handle->command = (handle->command & 0xffff0000U) | word_to_send;
            }
        }
    #endif

    #if(SPI_DMA_FEATURE==ENABLED)
        DspiDisable(DMA_ENGINE1, DMA_CH0);
        transfer_config_a.src_addr = (uint32) rxAddr;
        transfer_config_a.source_offset = 0;

        if (handle->rx_data)
        {
            transfer_config_a.dest_addr = (uint32) &(handle->rx_data[0]);
            transfer_config_a.dest_offset = 1;
        }
        else
        {
            transfer_config_a.dest_addr = (uint32) &(handle->rx_buff_If_null);
            transfer_config_a.dest_offset = 0;
        }

        transfer_config_a.dest_transfer_size = DMA_TRANSFER_SIZE_ONE_BYTE;

        if (handle->bits_per_frame <= 8)
        {
            transfer_config_a.source_transfer_size = DMA_TRANSFER_SIZE_ONE_BYTE;
            transfer_config_a.minor_loop_bytes = 1;
            transfer_config_a.major_loop_counts = handle->remaining_receive_byte_count;
        }
        else
        {
            transfer_config_a.source_transfer_size = DMA_TRANSFER_SIZE_TWO_BYTE;
            transfer_config_a.minor_loop_bytes = 2;
            transfer_config_a.major_loop_counts = handle->remaining_receive_byte_count / 2;
        }
         DspiSetConfig(DMA_ENGINE1,DMA_CH0,&transfer_config_a,(EDMA_TCD *)NULL);
         DspiDisable(DMA_ENGINE1, DMA_CH1);

        if (handle->remaining_send_byte_count > 0)
        {
            if (handle->tx_data)
            {
                transfer_config_b.src_addr = (uint32) (handle->tx_data);
                transfer_config_b.source_offset = 1;
            }
            else
            {
                transfer_config_b.src_addr = (uint32) (&handle->tx_buff_if_null);
                transfer_config_b.source_offset = 0;
            }

            transfer_config_b.dest_addr = (uint32) (&handle->command);
            transfer_config_b.dest_offset = 0;

            transfer_config_b.source_transfer_size = DMA_TRANSFER_SIZE_ONE_BYTE;

            if (handle->bits_per_frame <= 8)
            {
                transfer_config_b.dest_transfer_size = DMA_TRANSFER_SIZE_ONE_BYTE;
                transfer_config_b.minor_loop_bytes = 1;

                if (1 == FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(base))
                {
                    /*already prepared the first data in "intermediary" , so minus 1 */
                    transfer_config_b.major_loop_counts = handle->remaining_send_byte_count - 1;
                }
                else
                {
                    /*Only enable channel_B minorlink to channel_C , so need to add one count due to the last time is
                     majorlink , the majorlink would not trigger the channel_C*/
                    transfer_config_b.major_loop_counts = handle->remaining_send_byte_count + 1;
                }
            }
            else
            {
                transfer_config_b.dest_transfer_size = DMA_TRANSFER_SIZE_TWO_BYTE;
                transfer_config_b.minor_loop_bytes = 2;
                if (1 == FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(base))
                {
                    /*already prepared the first data in "intermediary" , so minus 1 */
                    transfer_config_b.major_loop_counts = handle->remaining_send_byte_count / 2 - 1;
                }
                else
                {
                    /*Only enable channel_B minorlink to channel_C , so need to add one count due to the last time is
                     * majorlink*/
                    transfer_config_b.major_loop_counts = handle->remaining_send_byte_count / 2 + 1;
                }
            }

            DspiSetConfig(DMA_ENGINE1, DMA_CH1, &transfer_config_b, NULL);
        }

        /***channel_C ***carry the "intermediary" to SPIx_PUSHR. used the edma Scatter Gather function on channel_C to
         handle the last data */
        DspiDisable(DMA_ENGINE1, DMA_CH2);

        if (((handle->remaining_send_byte_count > 0) && (1 != FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(base)))
         || ((((handle->remaining_send_byte_count > 1) && (handle->bits_per_frame <= 8))
         || ((handle->remaining_send_byte_count > 2) && (handle->bits_per_frame > 8)))
         && (1 == FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(base))))
        {
            if (handle->tx_data)
            {
                uint32 bufferIndex = 0;

                if (1 == FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(base))
                {
                    if (handle->bits_per_frame <= 8)
                    {
                        bufferIndex = handle->remaining_send_byte_count - 1;
                    }
                    else
                    {
                        bufferIndex = handle->remaining_send_byte_count - 2;
                    }
                }
                else
                {
                    bufferIndex = handle->remaining_send_byte_count;
                }

                if (handle->bits_per_frame <= 8)
                {
                    handle->last_command = (handle->last_command & 0xffff0000U) | handle->tx_data[bufferIndex - 1];
                }
                else
                {
                    if (handle->is_there_extra_byte)
                    {
                        handle->last_command = (handle->last_command & 0xffff0000U) | handle->tx_data[bufferIndex - 2]
                        | ((uint32) dummy_data << 8);
                    }
                    else
                    {
                        handle->last_command = (handle->last_command & 0xffff0000U)
                        | ((uint32) handle->tx_data[bufferIndex - 1] << 8) | handle->tx_data[bufferIndex - 2];
                    }
                }
            }
            else
            {
                if (handle->bits_per_frame <= 8)
                {
                    word_to_send = dummy_data;
                }
                else
                {
                    word_to_send = ((uint32) dummy_data << 8) | dummy_data;
                }
                handle->last_command = (handle->last_command & 0xffff0000U) | word_to_send;
            }
        }
        if ((1 == FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(base))
         || ((1 != FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(base)) && (handle->remaining_send_byte_count > 0)))
        {
            transfer_config_c.src_addr = (uint32) &(handle->last_command);
            transfer_config_c.dest_addr = (uint32) txAddr;
            transfer_config_c.source_transfer_size = DMA_TRANSFER_SIZE_FOUR_BYTE;
            transfer_config_c.dest_transfer_size = DMA_TRANSFER_SIZE_FOUR_BYTE;
            transfer_config_c.source_offset = 0;
            transfer_config_c.dest_offset = 0;
            transfer_config_c.minor_loop_bytes = 4;
            transfer_config_c.major_loop_counts = 1;

            EdmaTcdReset(software_tcd);
            EdmaTcdSetTransferConfig(software_tcd, &transfer_config_c);
        }
        if (((handle->remaining_send_byte_count > 1) && (handle->bits_per_frame <= 8))
         || ((handle->remaining_send_byte_count > 2) && (handle->bits_per_frame > 8)))
        {
            transfer_config_c.src_addr = (uint32) (&(handle->command));
            transfer_config_c.dest_addr = (uint32) txAddr;

            transfer_config_c.source_transfer_size = DMA_TRANSFER_SIZE_FOUR_BYTE;
            transfer_config_c.dest_transfer_size = DMA_TRANSFER_SIZE_FOUR_BYTE;
            transfer_config_c.source_offset = 0;
            transfer_config_c.dest_offset = 0;
            transfer_config_c.minor_loop_bytes = 4;

            if (handle->bits_per_frame <= 8)
            {
                transfer_config_c.major_loop_counts = handle->remaining_send_byte_count - 1;
            }
            else
            {
                transfer_config_c.major_loop_counts = handle->remaining_send_byte_count / 2 - 1;
            }

            DspiSetConfig(DMA_ENGINE1, DMA_CH2, &transfer_config_c, software_tcd);
            //Enable auto stop request
            DMA0->TCD[DMA_CH2].CSR = (DMA0->TCD[DMA_CH2].CSR & (~DMA_CSR_DREQ_MASK)) | DMA_CSR_DREQ(FALSE);

        }
        else
        {
            DspiSetConfig(DMA_ENGINE1, DMA_CH2, &transfer_config_c, NULL);
        }
        //EdmaStartTransfer(handle->edma_rx_reg_to_rx_data_handle);
        DMA0->SERQ = DMA_SERQ_SERQ(DMA_CH0);
        DMA0->SERQ = DMA_SERQ_SERQ(DMA_CH1);
        DMA0->SERQ = DMA_SERQ_SERQ(DMA_CH2);
    #endif

    #if (SPI0_FEATURE == ENABLED)
         /*if there is Tx DMA request , carry the 32bits data (handle->command) to PUSHR first , then link to channelB
         to prepare the next 32bits data (User_send_buffer to handle->command) */
         if (handle->remaining_send_byte_count > 1)
         {
           /* Enable minor link */
             DMA0->TCD[DMA_CH2].CITER_ELINKNO |= DMA_CITER_ELINKYES_ELINK_MASK;
             DMA0->TCD[DMA_CH2].BITER_ELINKNO  |= DMA_BITER_ELINKYES_ELINK_MASK;
             /* Set likned channel */
             tmpreg = DMA0->TCD[DMA_CH2].CITER_ELINKNO  & (~DMA_CITER_ELINKYES_LINKCH_MASK);
             tmpreg |= DMA_CITER_ELINKYES_LINKCH(DMA_CH1);
             DMA0->TCD[DMA_CH2].CITER_ELINKNO  = tmpreg;
             tmpreg = DMA0->TCD[DMA_CH2].BITER_ELINKNO & (~DMA_BITER_ELINKYES_LINKCH_MASK);
             tmpreg |= DMA_BITER_ELINKYES_LINKCH(DMA_CH1);
             DMA0->TCD[DMA_CH2].BITER_ELINKNO  = tmpreg;
         }
        base->RSER |= (SPI_RSER_RFDF_RE_MASK | SPI_RSER_RFDF_DIRS_MASK)|(SPI_RSER_TFFF_RE_MASK | SPI_RSER_TFFF_DIRS_MASK);
    #endif

    #if(SPI_DMA_FEATURE==ENABLED)
        base->MCR &= ~SPI_MCR_HALT_MASK;
    #endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API is used for SPI set TCD register configuration for  Master communication
 *
 *  @details    TThis API is used for SPI set TCD register source address,destination address,Number of bytes for  Master communication
 *
 *  @param      dma_engine          :this variable is used to to select which DMA ENGINE is using
 *  @param      channel             :this variable is used to to select which channel is  using
 *  @param      transfer_config     :DSPI master transfer data configuration used for the transfer*
 *  @param      next_tcd            :is used to configure scatter/gather feature as a next hardware TCD.
*/
void DspiSetConfig(DMA_ENGINE_TYPE dma_engine,DMA_CHANNEL_TYPE channel,EDMA_TRANSFER_CONFIG *transfer_config, EDMA_TCD *next_tcd)
{
    DMA0->TCD[channel].SADDR = transfer_config->src_addr;
     /* destination address */
    DMA0->TCD[channel].DADDR = transfer_config->dest_addr;
     /* Source data and destination data transfer size */
     DMA0->TCD[channel].ATTR = DMA_ATTR_SSIZE(transfer_config->source_transfer_size) | DMA_ATTR_DSIZE(transfer_config->dest_transfer_size);
     /* Source address signed offset */
     DMA0->TCD[channel].SOFF = transfer_config->source_offset;
     /* Destination address signed offset */
     DMA0->TCD[channel].DOFF =transfer_config->dest_offset;
     /* Minor byte transfer count */
     DMA0->TCD[channel].NBYTES_MLNO = transfer_config->minor_loop_bytes;
     /* Current major iteration count */
     DMA0->TCD[channel].CITER_ELINKNO = transfer_config->major_loop_counts;
     /* Starting major iteration count */
     DMA0->TCD[channel].BITER_ELINKNO = transfer_config->major_loop_counts;
     if (next_tcd != NULL)
     {
         DMA0->TCD[channel].DLAST_SGA = (uint32)next_tcd;
         /*
             Before call EdmaTcdSetTransferConfig or EDMA_SetTransferConfig,
             user must call EdmaTcdReset or EDMA_ResetChannel which will set
             DREQ, so must use "|" or "&" rather than "=".

             Clear the DREQ bit because scatter gather has been enabled, so the
             previous transfer is not the last transfer, and channel request should
             be enabled at the next transfer(the next TCD).
         */
         DMA0->TCD[channel].CSR = (DMA0->TCD[channel].CSR | DMA_CSR_ESG_MASK) & ~DMA_CSR_DREQ_MASK;
     }
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API is used for  TCD registers reset
 *  @param      tcd            :is used to configure scatter/gather feature as a next hardware TCD.
*/

void EdmaTcdReset( EDMA_TCD *tcd)
{

    /* Reset channel TCD */
    tcd->SADDR = 0U;
    tcd->SOFF = 0U;
    tcd->ATTR = 0U;
    tcd->NBYTES = 0U;
    tcd->SLAST = 0U;
    tcd->DADDR = 0U;
    tcd->DOFF = 0U;
    tcd->CITER = 0U;
    tcd->DLAST_SGA = 0U;
    /* Enable auto disable request feature */
    tcd->CSR = DMA_CSR_DREQ(TRUE);
    tcd->BITER = 0U;
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API is used for  configure TCD registers
 *  @details    This API is used for  configure TCD registers based on the  requirement like source address,destination address,and number of bytes etc
 *  @param      tcd            :is used to configure scatter/gather feature as a next hardware TCD.
 *  @param      config         :DSPI master transfer data configuration used for the transfer
 *
*/
void EdmaTcdSetTransferConfig( EDMA_TCD *tcd, const EDMA_TRANSFER_CONFIG *config)
{
    /* source address */
    tcd->SADDR = config->src_addr;
    /* destination address */
    tcd->DADDR = config->dest_addr;
    /* Source data and destination data transfer size */
    tcd->ATTR = DMA_ATTR_SSIZE(config->source_transfer_size) | DMA_ATTR_DSIZE(config->dest_transfer_size);
    /* Source address signed offset */
    tcd->SOFF = config->source_offset;
    /* Destination address signed offset */
    tcd->DOFF = config->dest_offset;
    /* Minor byte transfer count */
    tcd->NBYTES = config->minor_loop_bytes;
    /* Current major iteration count */
    tcd->CITER = config->major_loop_counts;
    /* Starting major iteration count */
    tcd->BITER = config->major_loop_counts;
    /* Enable scatter/gather processing */
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API is used  to start DMA transfer
 *  @details    This API is used  to start DMA transfer
 *  @param      handle            :it is a DMA transfer handle structure variable
*/

void EdmaStartTransfer(EDMA_HANDLE *handle)
{

    handle->base->SERQ = DMA_SERQ_SERQ(handle->channel);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API is used  to initialize DMA_SPI Master initialization
 *  @details    This API is used  to start DMA transfer
 *  @param      base            :This variable contains base address of SPI
 *  @param      masterConfig    :DSPI master configuration structure variable
 *  @param      src_clock_hz    :This variable contains clock frequency of source
*/

void DspiMasterInit(SPI_TYPE *base, const DSPI_MASTER_CONFIG  *masterConfig, uint32 src_clock_hz)
{

    uint32 temp;
    base->MCR &= ~SPI_MCR_MDIS_MASK;
    base->MCR |= SPI_MCR_HALT_MASK;
    base->MCR = (base->MCR & (~SPI_MCR_MSTR_MASK)) | SPI_MCR_MSTR(DSPI_MASTER);


    temp = base->MCR & (~(SPI_MCR_CONT_SCKE_MASK | SPI_MCR_MTFE_MASK | SPI_MCR_ROOE_MASK | SPI_MCR_SMPL_PT_MASK |
                          SPI_MCR_DIS_TXF_MASK | SPI_MCR_DIS_RXF_MASK));

    base->MCR = temp | SPI_MCR_CONT_SCKE(masterConfig->enable_continuos_sck) |
                SPI_MCR_MTFE(masterConfig->enable_modified_timing_format) |
                SPI_MCR_ROOE(masterConfig->enable_rx_fifo_over_write) | SPI_MCR_SMPL_PT(DSPI_SCK_TO_SIN0_CLOCK) |
                SPI_MCR_DIS_TXF(FALSE) | SPI_MCR_DIS_RXF(FALSE);

    base->MCR |= SPI_MCR_PCSIS(DSPI_PCS_ACTIVE_HIGH );
    if (0 == DspiMasterSetBaudrate(base, DSPI_CTAR0, masterConfig->ctar_config.baud_rate, src_clock_hz))
    {
    }

    temp = base->CTAR[DSPI_CTAR0] &
           ~(SPI_CTAR_FMSZ_MASK | SPI_CTAR_CPOL_MASK | SPI_CTAR_CPHA_MASK | SPI_CTAR_LSBFE_MASK);

    base->CTAR[DSPI_CTAR0] =
        temp | SPI_CTAR_FMSZ(masterConfig->ctar_config.bits_per_frame - 1) | SPI_CTAR_CPOL(masterConfig->ctar_config.cpol) |
        SPI_CTAR_CPHA(masterConfig->ctar_config.cpha) | SPI_CTAR_LSBFE(masterConfig->ctar_config.direction);

    base->MCR &= ~SPI_MCR_HALT_MASK;
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API is used  to set  DMA_SPI baud rate
 *  @details    This API is used  to set  DMA_SPI baud rate
 *  @param      base             :This variable contains base address of SPI
 *  @param      baud_rate_bps    :This variable is used to select the baud rate defined by the user
 *  @param      src_clock_hz     :This variable contains clock frequency of source
*/
uint32 DspiMasterSetBaudrate(SPI_TYPE *base,uint32  which_ctar,uint32 baud_rate_bps,uint32 src_clock_hz)
{
    uint32 temp;
    uint32 pre_scaler, best_pre_scaler;
    uint32 scaler, best_scaler;
    uint32 dbr, best_dbr;
    uint32 real_baud_rate, best_baud_rate;
    uint32 diff, min_diff;
    uint32 baud_rate = baud_rate_bps;

    /* find combination of pre_scaler and scaler resulting in baud_rate closest to the requested value */
    min_diff = 0xFFFFFFFFU;
    best_pre_scaler = 0;
    best_scaler = 0;
    best_dbr = 1;
    best_baud_rate = 0; /* required to avoid compilation warning */

    /* In all for loops, if min_diff = 0, the exit for loop*/
    for (pre_scaler = 0; (pre_scaler < 4) && min_diff; pre_scaler++)
    {
        for (scaler = 0; (scaler < 16) && min_diff; scaler++)
        {
            for (dbr = 1; (dbr < 3) && min_diff; dbr++)
            {
                real_baud_rate = ((src_clock_hz * dbr) / (BAUD_RATE_PRESCALAR[pre_scaler] * (BAUD_RATE_SCALAR[scaler])));

                /* calculate the baud rate difference based on the conditional statement that states that the calculated
                * baud rate must not exceed the desired baud rate.
                */
                if (baud_rate >= real_baud_rate)
                {
                    diff = baud_rate - real_baud_rate;
                    if (min_diff > diff)
                    {
                        /* a better match found */
                        min_diff = diff;
                        best_pre_scaler = pre_scaler;
                        best_scaler = scaler;
                        best_baud_rate = real_baud_rate;
                        best_dbr = dbr;
                    }
                }
            }
        }
    }

    /* write the best dbr, prescalar, and baud rate scalar to the CTAR */
    temp = base->CTAR[DSPI_CTAR0] & ~(SPI_CTAR_DBR_MASK | SPI_CTAR_PBR_MASK | SPI_CTAR_BR_MASK);

    base->CTAR[DSPI_CTAR0] = temp | ((best_dbr - 1) << SPI_CTAR_DBR_SHIFT) | (best_pre_scaler << SPI_CTAR_PBR_SHIFT) |
                            (best_scaler << SPI_CTAR_BR_SHIFT);

    /* return the actual calculated baud rate */
    return best_baud_rate;
}
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Disables the specified DMAy Channelx.
  * @param  channel: Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  */
void DspiDisable(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
    /* Reset channel TCD */
     DMA0->TCD[channel].SADDR = 0U;
     DMA0->TCD[channel].SOFF = 0U;
     DMA0->TCD[channel].ATTR = 0U;
     DMA0->TCD[channel].NBYTES_MLNO = 0U;
     DMA0->TCD[channel].SLAST = 0U;
     DMA0->TCD[channel].DADDR = 0U;
     DMA0->TCD[channel].DOFF = 0U;
     DMA0->TCD[channel].CITER_ELINKNO = 0U;
     DMA0->TCD[channel].DLAST_SGA = 0U;
     /* Enable auto disable request feature */
     DMA0->TCD[channel].CSR = DMA_CSR_DREQ(TRUE);
     DMA0->TCD[channel].BITER_ELINKNO = 0U;
}


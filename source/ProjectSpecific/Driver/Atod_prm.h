/**
 *  @file
 *  @brief      All the possible configurations are defined within this file
 *
 *<pre>
 * For ADCx Channel Assignment
 *
 * -------------------------------------------------------------------------      -------------------------------------------------------------------------
 * |                      For ADC0 CHANNELS                                 |     |                      For ADC1 CHANNELS                                 |
 * |------------------------------------------------------------------------|     |------------------------------------------------------------------------|
 * |ATOD Channel| Input Signal|  Port  |   Pin    |                         |     |TOD Channel | Input Signal|  Port  |   Pin    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH0  | ADC0_DP0    |  --    | ADC0_DP0 |                         |     |  ATOD_CH0  | ADC1_DP0    |   --   | ADC1_DP0 |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH1  | ADC0_DP1    |  --    | ADC0_DP1 |                         |     |  ATOD_CH1  | ADC1_DP1    |   --   | ADC1_DP1 |                         |
 * | -----------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH2  | ADC0_DP2    |  --    | ADC0_DP2 |                         |     |  ATOD_CH2  | RESERVED    |   --   |    --    |                         |
 * |------------|-------------|--------|----------|  For ATOD_CH4 to ATOCH7 |     |------------|-------------|--------|----------|  For ATOD_CH4 to ATOCH7 |
 * |  ATOD_CH3  | ADC0_DP3    |  --    | ADC0_DP3 |   if ADC0_MUXSEL = 1    |     |  ATOD_CH3  | ADC1_DP3    |   --   | ADC1_DP3 |   if ADC1_MUXSEL = 1    |
 * |------------|-------------|--------|----------|-------------------------|     |------------|-------------|--------|----------|-------------------------|
 * |  ATOD_CH4  | ADC0_SE4A   | PORTE  |   16     | ADC0_SE4B | PORTC |  2  |     |  ATOD_CH4  | ADC1_SE4a   |  PORTE |    0     | ADC1_SE4B | PORTC |  8  |
 * |------------|-------------|--------|----------|-----------|-------|-----|     |------------|-------------|--------|----------|-----------|-------|-----|
 * |  ATOD_CH5  | ADC0_SE5A   | PORTE  |   17     | ADC0_SE5B | PORTD |  1  |     |  ATOD_CH5  | ADC1_SE5A   |  PORTE |    1     | ADC1_SE5B | PORTC |  9  |
 * |------------|-------------|--------|----------|-----------|-------|-----|     |------------|-------------|--------|----------|-----------|-------|-----|
 * |  ATOD_CH6  | ADC0_SE6A   | PORTE  |   18     | ADC0_SE6B | PORTD |  5  |     |  ATOD_CH6  | ADC1_SE6A   |  PORTE |    2     | ADC1_SE6B | PORTC | 10  |
 * |------------|-------------|--------|----------|-----------|-------|-----|     |------------|-------------|--------|----------|-----------|-------|-----|
 * |  ATOD_CH7  | ADC0_SE7A   | PORTE  |   19     | ADC0_SE7B | PORTD |  6  |     |  ATOD_CH7  | ADC1_SE7A   |  PORTE |    3     | ADC1_SE7B | PORTC | 11  |
 * |------------|-------------|--------|----------|-------------------------|     |------------|-------------|--------|----------|-------------------------|
 * |  ATOD_CH8  | ADC0_SE8    | PORTB  |    0     |                         |     |  ATOD_CH8  | ADC1_SE8    |  PORTB |    0     |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH9  | ADC0_SE9    | PORTB  |    1     |                         |     |  ATOD_CH9  | ADC1_SE9    |  PORTB |    1     |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH10 | RESERVED    |  --    |    --    |                         |     |  ATOD_CH10 | RESERVED    |   --   |    --    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH11 | RESERVED    |  --    |    --    |                         |     |  ATOD_CH11 | RESERVED    |   --   |    --    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH12 | ADC0_SE12   | PORTB  |    2     |                         |     |  ATOD_CH12 | ADC1_SE12   |  PORTB |    2     |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH13 | ADC0_SE13   | PORTB  |    3     |                         |     |  ATOD_CH13 | ADC1_SE13   |  PORTB |    3     |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH14 | ADC0_SE14   | PORTC  |    0     |                         |     |  ATOD_CH14 | ADC1_SE14   |  PORTB |    10    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH15 | ADC0_SE15   | PORTC  |    1     |                         |     |  ATOD_CH15 | ADC1_SE15   |  PORTB |    11    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH16 |             | PORTC  |    2     |                         |     |  ATOD_CH16 |             |   --   |    --    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH17 | ADC0_SE17   | PORTE  |    24    |                         |     |  ATOD_CH17 | ADC0_SE17   |  PORTA |    17    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH18 | ADC0_SE18   | PORTE  |    25    |                         |     |  ATOD_CH18 | ADC1_SE18   |  PORTE | ADC1_SE18|                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH19 | ADC0_DM0    |  --    | ADC0_DM0 |                         |     |  ATOD_CH19 | ADC1_DM0    |   --   | ADC1_DM0 |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH20 | ADC0_DM1    |  --    | ADC0_DM1 |                         |     |  ATOD_CH20 | ADC1_DM1    |   --   | ADC1_DM1 |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH21 | ADC0_SE21   |        |          |                         |     |  ATOD_CH21 | RESERVED    |        |          |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH22 | ADC0_SE22   |        |ADC0_SE22 |                         |     |  ATOD_CH22 | RESERVED    |        |    --    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH23 | ADC0_SE23   |        |ADC0_SE23 |                         |     |  ATOD_CH23 | ADC0_SE23   |        |ADC0_SE23 |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH24 | RESERVED    |  --    |    --    |                         |     |  ATOD_CH24 | RESERVED    |   --   |    --    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH25 | RESERVED    |  --    |    --    |                         |     |  ATOD_CH25 | RESERVED    |   --   |    --    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH26 | Temperature |        |          |                         |     |  ATOD_CH26 |  Temperature|        |Temperatur|                         |
 * |            | Sensor      |        |          |                         |     |            |   Sensor    |        | Sensor   |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH27 | Bandgap     |        |          |                         |     |  ATOD_CH27 | andgap      |        |Bandgap   |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH28 | RESERVED    |  --    |    --    |                         |     |  ATOD_CH28 | RESERVED    |   --   |    --    |                         |
 * |------------|-------------|--------|----------|                         |     |------------|-------------|--------|----------|                         |
 * |  ATOD_CH29 |             |      VREFH        |                         |     |  ATOD_CH29 |             |       VREFH       |                         |
 * |            |             |Power supply (Vbat)|                         |     |            |             |Power supply (Vbat)|                         |
 * |------------|-------------|-------------------|                         |     |------------|-------------|-------------------|                         |
 * |  ATOD_CH30 |             |      VREFL        |                         |     |  ATOD_CH30 |             |       VREFL       |                         |
 * |            |             |Power supply (Vbat)|                         |     |            |             |Power supply (Vbat)|                         |
 * |------------|-------------|-------------------|                         |     |------------|-------------|-------------------|                         |
 * |  ATOD_CH31 |             | Module Disabled   |                         |     |  ATOD_CH31 |             |  Module Disabled  |                         |
 *  -------------------------------------------------------------------------      -------------------------------------------------------------------------- </pre>
 *
 * $Header: SRAtod_prm.h 1.2 2015/09/01 08:25:59EDT Natak Rahul (NATAKRB) Exp  $
 *
 * @copyright  *****  Copyright 2013.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL
 */
#ifndef ATOD_PRM_H
    #define ATOD_PRM_H

//=====================================================================================================================
/**
 *    @brief  - Defines the number of conversions the Atod__Handler() method will perform each time it’s called
 *    @details- When channels are configured to ATOD_CONTINUOUS mode they are handled by the method Atod__Handler().\n
 *              This method reads some channels each time it's called. The parameter ADC_MAX_READINGS_PER_HANDLER_CALL defines how many
 *              channels will be read each time the Atod__Handler method is called.
 *
 *    @param  - 1 to 20(Maximum ADC Channels)
 *
 *    @note   - Following are the Examples\n
 *              "#define ADC_MAX_READINGS_PER_HANDLER_CALL 1" // The Max reading is 1\n
 *              "#define ADC_MAX_READINGS_PER_HANDLER_CALL 3" // The Max reading is 3
 */
//=====================================================================================================================
#define ADC_MAX_READINGS_PER_HANDLER_CALL	2

//=====================================================================================================================
/**
 * @brief  -  Enable/Disables the Atod conversion in continuous or in one shot mode.
 *
 * @param  - ENABLED  - Enables the Atod conversion in continuous mode
 *           DISABLED – Disables the Atod conversion in continuous mode
 *
 * @note   - Following are the Examples\n
 *           "#define ATOD_CONTINUOUS_MODE_FEATURE ENABLED"  // supports for both continuous and one shot mode\n
 *           "#define ATOD_CONTINUOUS_MODE_FEATURE DISABLED" // supports only one shot mode
 */
//=====================================================================================================================
#define ATOD_CONTINUOUS_MODE_FEATURE        ENABLED

//=====================================================================================================================
/**
 * @brief  -  If the application is reading the ADC conversion in the fixed resolution provided that all the channels use the
 *              resolution then its recommenced to use the below macro in order to reduce the code.
 *
 *              Comment this macro to use the Dynamic Resolutuion configuration
 *
 * @param  -    4      for 4 bits resolution
 *              8      for 8 bits resolution
 *              10     for 10 bits resolution
 *              12     for 12 bits resolution
 *              14     for 14 bits resolution
 *              16     for 16 bits resolution
 *
 * @note   - Following are the Examples\n
 *           "#define ATOD_HARDCODED_RESOLUTION           12" //12-bit hard-coded resolution is enabled
 *           "#define ATOD_HARDCODED_RESOLUTION           4" //4-bit hard-coded resolution is enabled
 */
//=====================================================================================================================
#define ATOD_HARDCODED_RESOLUTION           12
#define CR1_SAMPLE_ENABLED                  0

//=====================================================================================================================
/**
 *    @brief    Defines where the ADC sampling rate  has to defined
 *    @param   - 0 and 1
 *    for example - Max total conversion time
 *    ATOD_SAMPLING_RATE = 0, ADICLK = BUS Clock = 8Mhz and MODE = 10 or 12bit resoltuion
 *    	23ADCK cycles + 5 Bus clock cycles (refer to manual page  no 344)
 *    	Conversion time = (23ADCK cyc/(BUS CLOCK/Input clockdivide)) + (5 Bus cyc/(BUS CLOCK))
 *    					= 23/(8/1) + 5/8 = 3.5usec
 *    ATOD_SAMPLING_RATE = 1, ADICLK = BUS Clock and 10 or 12bit resoltuion
 *    	43ADCK cycles + 5 Bus clock cycles (refer to manual page  no 344)
 *    	Conversion time = 43/(8/1) + 5/8 = 6usec
 *
 *    @note    - #define ATOD_SAMPLING_RATE	 0   // This is Min value which takes a short sample time
 *				/#define ATOD_SAMPLING_RATE	 1   // This is Max value which takes a long sample time
 */
//=====================================================================================================================
#define SRATOD_SAMPLING_RATE						0

//=====================================================================================================================
/**
 *    @brief    Defines where the ADC related variables has to defined
 *    @details  NEAR - Variables will defined in NEAR memory segment
 *              FAR  - Variables will defined in FAR memory segment
 */
//=====================================================================================================================
#define ATOD_MEM_SEGMENT                            NEAR

//=====================================================================================================================
/**
 *    @brief  - Defines Number of ADC conversions the SRAtod__Handler() method will perform each time its called.
 *    @details- When channels are configured to ATOD_CONTINUOUS mode they are handled by the method SRAtod__Handler().
 *              This method reads some channels each time it's called.
 *              The parameter ADC_MAX_READINGS_PER_HANDLER_CALL defines how many channels will be read each time
 *              the SRAtod__Handler() method is called.
 *
 *    @param   - 0/1/2/3...
 *               Value 0 disables the ATOD_CONTINUOUS mode
 *
 *    @note    -"#define ADC_VREF_TEST_CLASSB       ENABLED"
 *              "#define ADC_VREF_TEST_CLASSB       DISABLED" // The Max reading is 3
 */
//=====================================================================================================================
#define SRATOD_VREF_TEST_CLASSB                        ENABLED

//=====================================================================================================================
/**
 *    @brief  - This macro enables the Atod Engines
 *    @details-
 *
 *    @param   - ENABLED / DISABLED
 *               Value 0 disables the ATOD_CONTINUOUS mode
 *
 *               #define ATOD0_ENGINE 					 ENABLED
 *               #define ATOD1_ENGINE 					 DISABLED
 */
//=====================================================================================================================
#define ATOD0_ENGINE 					 ENABLED

#define ATOD1_ENGINE 					 DISABLED

//=====================================================================================================================
/**
 *    @brief  - This macro selects between ADCx_SEn channels a and b
 *    @details- ADCx_CFG2[MUXSEL] bit selects between ADCx_SEn channels a and b.
 *
 * <pre>
 * For ADC0 ChannAssignment
 * --------------------------------------------------------------      --------------------------------------------------------------
 * |               For ADC0 CHANNELS                              |    |                      For ADC1 CHANNELS                       |
 * |--------------------------------------------------------------|    |--------------------------------------------------------------|
 * |   ATOD   | Input     |ADC0_MUXSEL=0| Input     |ADC0_MUXSEL=1|    |   ATOD   | Input     |ADC1_MUXSEL=0| Input     |ADC1_MUXSEL=1|
   |  Channel | Signal    |-------------| Signal    |-------------|    |  Channel | Signal    |-------------| Signal    |-------------|
 * |          |           | Port  |Pin  |           | Port  |Pin  |    |          |           | Port  |Pin  |           | Port  |Pin  |
 * |----------|-----------|-------|-----|-----------|-------|-----|    |----------|-----------|-------|-----|-----------|-------|-----|
 * | ATOD_CH4 | ADC0_SE4A | PORTE | 16  | ADC0_SE4B | PORTC |  2  |    | ATOD_CH4 | ADC1_SE4A | PORTE |  0  | ADC1_SE4B | PORTC |  8  |
 * |----------|-----------|-------|-----|-----------|-------|-----|    |----------|-----------|-------|-----|-----------|-------|-----|
 * | ATOD_CH5 | ADC0_SE5A | PORTE | 17  | ADC0_SE5B | PORTD |  1  |    | ATOD_CH5 | ADC1_SE5A | PORTE |  1  | ADC1_SE5B | PORTC |  9  |
 * |----------|-----------|-------|-----|-----------|-------|-----|    |----------|-----------|-------|-----|-----------|-------|-----|
 * | ATOD_CH6 | ADC0_SE6A | PORTE | 18  | ADC0_SE6B | PORTD |  5  |    | ATOD_CH6 | ADC1_SE6A | PORTE |  2  | ADC1_SE6B | PORTC | 10  |
 * |----------|-----------|-------|-----|-----------|-------|-----|    |----------|-----------|-------|-----|-----------|-------|-----|
 * | ATOD_CH7 | ADC0_SE7A | PORTE | 19  | ADC0_SE7B | PORTD |  6  |    | ATOD_CH7 | ADC1_SE7A | PORTE |  3  | ADC1_SE7B | PORTC | 11  |
 *  ---------------------------------------------------------------     ---------------------------------------------------------------
 *
 *    @param   - 0,1
 *
 *               #define ADC0_MUXSEL 					 0				//ADC0a channels are selected.
 *               #define ADC0_MUXSEL 					 1				//ADC0b channels are selected.
 *
 *               #define ADC1_MUXSEL 					 0				//ADC1a channels are selected.
 *               #define ADC1_MUXSEL 					 1				//ADC1a channels are selected.
 */
//=====================================================================================================================
#define ADC0_MUXSEL					 ENABLED

//#define ADC1_MUXSEL 					 DISABLED

#endif

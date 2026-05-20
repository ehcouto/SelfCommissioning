/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *  @details    Detailed description of the file contents
 *
 *  Port-bit for Comparator channels
 *  The CMP0 and CMP1are available in the below port pins in MKV03x Controllers.
 *  For CMPx Channel Assignment
 *   -------------------------------------   ------------------------------------------       -------------------------------------  ------------------------------------------
 *  |        CMP0 CHANNELS INPUTS         | |             CMP0 CHANNEL OUTPUT          |     |      CMP1 CHANNELS INPUTS          | |             CMP1 CHANNEL OUTPUT          |
 *  |-------------------------------------| |------------------------------------------|     |------------------------------------| |------------------------------------------|
 *  | Channel|Input Signal|  Port|   Pin  | |CMP0 OUTPUT| REMAP0  | REMAP1  |  REMAP2  |     |Channel| Input Signal| Port |  Pin  | |CMP0 OUTPUT| REMAP0  | REMAP1  |  REMAP2  |
 *  |--------|------------|------|--------| |-----------|---------|---------|----------|     |-------|-------------|------|-------| |-----------|---------|---------|----------|
 *  |  IN0   |CMP0_IN0    |PORT C|   5    | |           |PORT |Pin|PORT |Pin|PORT |Pin |     |  IN0  | CMP1_IN0    |PORT C|   2   | |           |PORT |Pin|PORT |Pin|PORT |Pin |
 *  |--------|------------|------|--------| |-----------|-----|---|-----|---|-----|----|     |-------|-------------|------|-------| |-----------|-----|---|-----|---|-----|----|
 *  |  IN1   |CMP0_IN1    |PORT C|   6    | | CMP0_OUT  |PORTA| 1 |PORTB| 20|PORTC| 5  |     |  IN1  | CMP1_IN1    |PORT C|   3   | | CMP0_OUT  |PORTA| 2 |PORTB| 21|PORTC| 4  |
 *  | -------|------------|------|--------|  ------------------------------------------      |-------|-------------|------|-------|  ------------------------------------------
 *  |  IN2   |CMP0_IN2    |PORT C|   8    |                                                  |  IN2  | CMP1_IN2    |   -- |    -- |
 *  |--------|------------|------|--------|                                                  |-------|-------------|------|-------|
 *  |  IN3   |CMP0_IN3    |PORT C|   9    |                                                  |  IN3  | CMP1_IN3    |   -- |    -- |
 *  |--------|------------|------|--------|                                                  |-------|-------------|------|-------|
 *  |  IN4   |CMP0_IN4    |   -- |CMP0_IN4|                                                  |  IN4  |    --       |   -- |    -- |
 *  |--------|------------|------|--------|                                                  |-------|-------------|------|-------|
 *  |  IN5   |CMP0_IN5    |   -- |CMP0_IN5|                                                  |  IN5  | CMP1_IN5    |   -- |    -- |
 *  |--------|------------|------|--------|                                                  |-------|-------------|------|-------|
 *  |  IN6   |Bandgap     |   -- |   --   |                                                  |  IN6  | Bandgap     |   -- |    -- |
 *  |--------|------------|------|--------|                                                  |-------|-------------|------|-------|
 *  |  IN7   |DAC0 Ref    |   -- |   --   |                                                  |  IN7  | DAC1 Ref    |   -- |    -- |
 *   -------------------------------------                                                    ------------------------------------
 *
 *   Comparator Functional Modes
 *    ---------------------------------------------------------------------------------------------------------------
 *   |MODE | CR1[EN] |     R1[WE]     |      CR1 [SE]    | CR0[FILTER_CNT] |FPR[FILT PER0]|          OPERATION       |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   |     |         | CMP_WINDOWING  |CMP_SAMPLE_ENABLED|  CMP_FILTER_CNT |   CMPx_FPR   |    CMP_MODES_DEF         |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   |  1  |    0    |       X        |        X         |       XX        |      X       |    DISABLED MODE         |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   | 2A  |    1    |       0        |        0         |      0x00       |      X       |    CONTINOUS MODE        |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   | 2B  |    1    |       0        |        0         |        X        |    0x00      |    CONTINOUS MODE        |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   | 3A  |    1    |       0        |        1         |      0x01       |      X       | SAMPLED NONFILTERED MODE |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   | 3B  |    1    |       0        |        0         |      0x01       |    >0x00     | SAMPLED NONFILTERED MODE |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   | 4A  |    1    |       0        |        1         |     >0x01       |      X       | SAMPLED FILTERED MODE    |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   | 4B  |    1    |       0        |        0         |     >0x01       |    >0x00     | SAMPLED FILTERED MODE    |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   | 5A  |    1    |       1        |        0         |      0x00       |      X       |   WINDOWED MODE          |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   | 5B  |    1    |       1        |        0         |        X        |     0x00     |   WINDOWED MODE          |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   |  6  |    1    |       1        |        0         |        1        |  0x01 - 0xFF | WINDOWED/RESAMPLED MODE  |
 *   |-----|---------|----------------|------------------|-----------------|--------------|--------------------------|
 *   |  7  |    1    |       1        |        0         |       >1        |  0x01 - 0xFF | WINDOWED/FILTERED MODE   |
 *   ----------------------------------------------------------------------------------------------------------------
 *
 * <pre>
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef CMP_PRM_H_
#define CMP_PRM_H_
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 * @brief  - This macro Enables or Disables the COMPARATOR0 and COMPARATOR1.
 * @details- The COM port must be Enabled before using in the application.
 *
 * @param  - ENABLED(1) - Which Enables the COMPARATORx
 *           DISABLED(0) – Which Disables the COMPARATORx
 *
 * @note   - Following are the Examples\n
 *           "#define COMPARATOR0_FEATURE       ENABLED"  //This Enables the Comparator 1 Feature\n
 *           "#define COMPARATOR0_FEATURE       DISABLED" // This Disables the Comparator 1 Feature
 */
//=====================================================================================================================
#define COMPARATOR0_FEATURE       ENABLED
#define COMPARATOR1_FEATURE       ENABLED

//=====================================================================================================================
/**
 *  @file
 *  * @brief  - Defines the possible Configuration for Comparator Inputs
 *    @details- When channels are configured to CMP_MODES mode they are handled by the method Comparator__config().\n
 *              This method reads one input from each channels and set the output after comparing inputs as per selected mode .

 *    @param  - 0 to 7(Maximum Comparator Channels)
 *
 *    @note   -
 *
 * <pre>
 */
//=====================================================================================================================
/**
 * @brief  - This macro Defines Filter sample count
 *
 * @details- Represents the number of consecutive samples that must agree prior to the comparator
 *           output filter accepting a new output.
 *
 * @param  -  000     For Filter is disabled
 *            001     For one sample must agree
 *            010     For two consecutive sample must agree
 *            011     For three consecutive sample must agree
 *            100     For four consecutive sample must agree
 *            101     For five consecutive sample must agree
 *            110     For six consecutive sample must agree
 *            111     For seven consecutive sample must agree
 *
 * @note   - Following are the Examples\n
 *           #define  CMP0_FILTER_CNT		0     //Filter is disabled
 *           #define  CMP1_FILTER_CNT		5     //five consecutive sample must agree
 */
//=====================================================================================================================
#define COMPARATOR0_FILTER_CNT		0
#define COMPARATOR1_FILTER_CNT		0

//=====================================================================================================================
/**
 *    @brief  - This macro defines the mode in enabling of sampling mode.
 *    @details- When comparator are configured to operation than particular mode in that operation is selected by MACRO.
 *
 *    @param  - 0/1     for  CMP_SAMPLE_ENABLED
 *
 *    @note   - Following are the Examples\n
 *              #define  COMPARATOR0_SAMPLE_ENABLED		0     //Sampling mode is not selected.
 *              #define  COMPARATOR0_SAMPLE_ENABLED		1     //Sampling mode is selected.
 */
//=====================================================================================================================
#define COMPARATOR0_SAMPLE_ENABLED		0
#define COMPARATOR1_SAMPLE_ENABLED		0

//=====================================================================================================================
/**
 *    @brief  - This macro defines the mode in Comparator operation.
 *    @details- When comparator are configured to operation than particular mode in that operation is selected by MACRO.
 *
 *    @param  - 0/1     for  CMP_SAMPLE_ENABLED
 *
 *    @note   - Following are the Examples\n
 *              #define  CMP_SAMPLE_ENABLED		0     // Internal clock is selected [Sampled Non-Filtered,Sampled Filtered]
 *              #define  CMP_SAMPLE_ENABLED		1     // External clock is selected [Sampled Non-Filtered,Sampled Filtered]
 */
//=====================================================================================================================
#define CMP_WINDOWING_ENABLED		0

//=====================================================================================================================
/**
 *    @brief  - This macro defines the Comparator Filter sample period value.
 *    @details- Specifies the sampling period, in bus clock cycles, of the comparator output filter, when CR1[SE]=0.
 *              Setting FILT_PER to 0x0 disables the filter
 *    @note - 	This field has no effect when CR1[SE]=1. In that case, the external SAMPLE signal is used to determine
 *				the sampling period.
 *
 *    @param  - 0 - 255     for  CMP_Filter Period Register
 *
 *    @note   - Following are the Examples\n
 *              #define  COMPARATOR0_FPR		0     //Setting Filter sample period to 0x00 disables the filter
 *              #define  COMPARATOR1_FPR		1     //Setting Filter sample period from 1 to 255 enables the filter
 */
//=====================================================================================================================
#define COMPARATOR0_FPR		0
#define COMPARATOR1_FPR		0

//=====================================================================================================================
/**
 *    @brief  - This macro select the source of supply voltage refernce.
 *
 *    @param  - 0 - 1     for  CMP_Filter Period Register
 *
 *    @note   - Following are the Examples\n
 *              #define  CMP0_SUPPLY_REFERENCE_SELECT		0     //Vin1 is selected as resistor ladder network supply reference.
 *              #define  CMP1_SUPPLY_REFERENCE_SELECT		1     //V_in2 = 3.3 V is selected as resistor ladder network supply reference.
 */
//=====================================================================================================================
#define CMP0_SUPPLY_REFERENCE_SELECT		1
#define CMP1_SUPPLY_REFERENCE_SELECT		1



//=====================================================================================================================
/**
 * @brief  - This macros re-maps the CMP0 and CMP1 pins.
 * @details-
 *      ------------------------------------------      ------------------------------------------
 *     |             CMP0 CHANNEL OUTPUT          |    |             CMP1 CHANNEL OUTPUT          |
 *     |------------------------------------------|    |------------------------------------------|
 *     |CMP0 OUTPUT| REMAP0  | REMAP1  |  REMAP2  |    |CMP0 OUTPUT| REMAP0  | REMAP1  |  REMAP2  |
 *     |-----------|---------|---------|----------|    |-----------|---------|---------|----------|
 *     |           |PORT |Pin|PORT |Pin|PORT |Pin |    |           |PORT |Pin|PORT |Pin|PORT |Pin |
 *     |-----------|-----|---|-----|---|-----|----|    |-----------|-----|---|-----|---|-----|----|
 *     | CMP0_OUT  |PORTA| 1 |PORTB| 20|PORTC| 5  |    | CMP0_OUT  |PORTA| 2 |PORTB| 21|PORTC| 4  |
 *      ------------------------------------------      ------------------------------------------
 *
 *   @param  - 0/1/2     for CMP0
 *    		   0/1/2     for CMP1
 *
 * @note   - Following are the Examples\n
 *           "#define CMP0_REMAP                     1"   // COUT = PORTB - Pin 20
 *           "#define CMP1_REMAP                     2"   // COUT = PORTC - Pin 4
 *
 */
//=====================================================================================================================
#define COMPARATOR0_REMAP			-1
#define COMPARATOR1_REMAP			-1

//=====================================================================================================================


#endif // CMP_PRM_H_

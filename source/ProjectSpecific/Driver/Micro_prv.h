/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      CLASS B related configuration parameters.
 *
 *  $Header: Micro_prv.h 1.1 2015/08/21 14:04:22EDT Natak Rahul (NATAKRB) Exp  $
 *
 *  @copyright  Copyright 2011-$Date: 2015/08/21 14:04:22EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef MICRO_PRV_H_
    #define MICRO_PRV_H_
#include "C_Extensions.h"
#include "SRMC_defs.h"
#include "SRClockMonitor.h"
#include "SRException.h"
#include "SRFlow.h"
#include "SRMCPwm.h"
//=====================================================================================================================
//-------------------------------------- CLASS B Configuration Parameter-----------------------------------------------
//=====================================================================================================================

// Set the inverter output according to settings in SRMCPwm_prm.h file
#define MICRO_CALLBACK_BEFORE_WATCHDOG_INIT()       SRMCPwm__Initialize_Before_Boot()


//=====================================================================================================================
/**
 *    @brief  - Defines the Enabling or disabling of the Watchdog test at startup.
 *    @details- Independent watchdog is used for performing the Watchdog Boot test.
 *              Its mandatory to enable the Independent watchdog in the MICRO_prm.h file as below -
 *              #define MICRO_IWDG_FEATURE              ENABLED
 *
 *    @param  - ENABLED(1)    - Which Enables the Independent Watchdog test at startup
 *              DISABLED(0)   – Which Disables the Independent Watchdog test at startup
 *
 *    @note -   "#define SRMICRO_WATCHDOG_BOOT_TEST      ENABLED"
 *              "#define SRMICRO_WATCHDOG_BOOT_TEST      DISABLED"
 */
//=====================================================================================================================
#define SRMICRO_WATCHDOG_BOOT_TEST              DISABLED




//=====================================================================================================================
/**
 *    @brief -  Defines the Enabling or disabling of the Ram test at startup.
 *    @details- MARCH C algorithm is used in order to test the RAM at startup.
 *              Normally this configuration should be enabled.
 *
 *    @param -  ENABLED(1)    - Which Enables the MarchC Ram boot test
 *              DISABLED(0)   – Which Disables the MarchC Ram boot test
 *
 *    @note  -  "#define SRMICRO_RAM_BOOT_TEST      ENABLED"
 *              "#define SRMICRO_RAM_BOOT_TEST      DISABLED"
 */
//=====================================================================================================================
#define SRMICRO_RAM_BOOT_TEST                   DISABLED

//=====================================================================================================================
/**
 *    @brief  -  Defines the Enabling or disabling of the Ram periodic(runtime) test.
 *    @details-  Normally this configuration should be enabled.
 *
 *    @param  - ENABLED(1)    - Which Enables the Ram periodic test
 *              DISABLED(0)   – Which Disables the Ram periodic test
 *
 *    @note  -  "#define SRMICRO_RAM_PERIODIC_TEST      ENABLED"
 *              "#define SRMICRO_RAM_PERIODIC_TEST      DISABLED"
 */
//=====================================================================================================================
#define SRMICRO_RAM_PERIODIC_TEST               DISABLED

//=====================================================================================================================
/**
 *    @brief  - Defines the RAM memory range to test the RAM boot test and the RAM periodic test.
 *    @details- RAM memory size varies depending on the density of the controller.
 *
 *              For the STM32 controllers the RAM starts from the 0x20000000.
 *              SRMICRO_RAM_INIT value should not be less than the 0x20000000.
 *
 *              The maximum RAM memory size for the High density device is 64KB
 *              SRMICRO_RAM_END value should not be more than 0x2000FFFF.
 *
 *              Note : Please verify the RAM memory range from the linker(config.icf) file
 *
 *    @param  - SRMICRO_RAM_INIT - RAM start address from 0x20000000
 *              SRMICRO_RAM_END  - RAM end address should not be more than 0x2000FFFF
 *
 *    @note  -  //For High density device-
 *              "#define SRMICRO_RAM_INIT       0x20000000"
 *              "#define SRMICRO_RAM_END        0x20004FFF"
 */
//=====================================================================================================================
#define MICRO_RAM_INIT       0x20000000
#define MICRO_RAM_END        0x20001FFF

//=====================================================================================================================
/**
 *    @brief  - Defines how many bytes will be processed each interaction of the Continuous RAM Test.
 *    @details- If it is defined to a very low number such as 1 the test will take long time
 *              to perform the Full RAM test executed
 *              If it is defined to a very high number it will take a lot of processing time of the microcontroller.
 *
 *              The time taken for complete RAM test is calculated below-
 *              FULL RAM TEST (in ms) = (RAM Size/ SRMICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST) * Call rate(of SRMicro__RamTest())
 *
 *              The time taken for 1 byte test is 4.1usec
 *              The time taken for 10byte RAM test is 21.2usec
 *
 *    @param  - 1 to 255
 *
 *    @note  -  "#define SRMICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST      4"
 *              "#define SRMICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST      10"
 */
//=====================================================================================================================
#if(WINDY_STRIP_BOARD == 1)
#define MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST    1
#elif(WINDY_INTERNATION_BOARD == 1)
#define MICRO_NUM_BYTE_FOR_RAM_CONTINUOUS_TEST    4
#endif

//=====================================================================================================================
/**
 *    @brief  - Defines the Enabling or disabling of the flash boot test.
 *    @details- Boot time Flash Test which goes over all the flash calculating CRC and compares it against
 *              a CRC value stored in FLASH memory.
 *
 *    @param  - ENABLED(1)    - Which Enables the flash boot test
 *              DISABLED(0)   – Which Disables flash boot test
 *
 *    @note  -  "#define SRMICRO_FLASH_BOOT_TEST      ENABLED"
 *              "#define SRMICRO_FLASH_BOOT_TEST      DISABLED"
 */
//=====================================================================================================================
#define SRMICRO_FLASH_BOOT_TEST             DISABLED

//=====================================================================================================================
/**
 *    @brief  - Defines the Enabling or disabling of the flash periodic(runtime) test.
 *    @details- Run time Flash Test which calculates CRC couple bytes each time and keeps on continuing
 *              until it reaches the FLASH memory ends and compare the calculated CRC against
 *              a CRC value stored in FLASH memory.
 *
 *    @param  - ENABLED(1)    - Which Enables the flash periodic (runtime) test
 *              DISABLED(0)   – Which Disables flash periodic (runtime) test
 */
//=====================================================================================================================
#define SRMICRO_FLASH_PERIODIC_TEST         DISABLED

//=====================================================================================================================
/**
 *    @brief  - Defines how many bytes/words will be processed in each interaction of the continuous Flash Test.
 *    @details- If it is defined to a very low number such as 1 the test will take long time
 *              to perform the Full FLASH test.
 *              If it is defined to a very high number it will take a lot of processing time of the microcontroller.
 *
 *              For the Inbuilt CRC calculator the SRMICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST is in words.
 *              For the external CRC calculator the SRMICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST is in bytes.
 *
 *              The time taken for complete FLASH test is calculated as below-
 *              For the inbuilt CRC tool(IELF) -
 *              FULL FLASH TEST (in ms) =
 *                 (((FLASH Size in words)-4)/ SRMICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST) * Call rate(of SRMicro__FlashTest())
 *
 *              If the external CRC tool(IELF) is used-
 *              FULL FLASH TEST (in ms) =
 *                  (((FLASH Size in bytes)-4)/ SRMICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST) * Call rate(of SRMicro__FlashTest())
 *
 *              In the calculation FLASH Size-4 because the last 4- bytes of the flash memory is used to
 *              hold the calculated CRC value on which the Flash test is not performed.
 *
 *              The time taken for 1 word test is 5.9us were the system clock is 72MHz by the Inbuilt tool.
 *              The time taken for 10 word test is 56.9us were the system clock is 72MHz by the Inbuilt tool.
 *
 *    @param  - 1 to 255
 *
 *    @note  -  "#define SRMICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST      4"
 *              "#define SRMICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST      10"
 */
//=====================================================================================================================
#define SRMICRO_NUM_BYTE_FOR_FLASH_CONTINUOUS_TEST          4

//=====================================================================================================================
/**
 *    @brief  - Configures the CRC calculation engine either Inbuilt CRC engine (IELF tool by the IAR compiler)
 *              or external CRC tool.
 *    @details- Boot time Flash Test which goes over all the flash calculating CRC and compares it against
 *              a CRC value stored in FLASH memory.
 *
 *              Inbuilt CRC tool(IELF)-
 *                1. Used by enabling the INBUILT_FLASH_CRC_CALCULATOR macro.
 *                2. It uses the crc GDM(CRC16) for CRC calculation.
 *                3. CRC16_CCITT_FULL macro should be uncommented in the crc_prm.h file in order to
 *                   use the FLASH CRC check.
 *                4. It uses the FLASH memory range from the linker file(config.icf).
 *
 *              External CRC tool-
 *                1. Disable the INBUILT_FLASH_CRC_CALCULATOR macro.
 *                2. Define the Flash memory range and the location where the CRC value is stored as below-
 *                    #define SRMICRO_FLASH_INIT            0x08000130        // It defines where the Flash starts
 *                    #define SRMICRO_FLASH_END             0x0800FFFb        // It defines where the Flash ends
 *                    #define SRMICRO_CRC_FLASH             0x0800fffc      // It defines where the CRC will be stored
 *                3. Defines the engine used for Crc calculation as below
 *                    For Crc32
 *                        #include "Crc32.h"
 *                        #define CRC__PROCESS(accumulator, data)     Crc32__ProcessByte(accumulator, data)
 *                        #define CRC_DATA_TYPE                       uint32
 *                        #define SRMICRO_CRC_INITIAL                 CRC_INITIAL
 *
 *                    For Crc (Crc 16 bits)
 *                        #include "Crc.h"
 *                        #define CRC__PROCESS(accumulator, data)     FastCRCUpd(accumulator, data)
 *                        #define CRC_DATA_TYPE                       uint16
 *                        #define SRMICRO_CRC_INITIAL                 ((uint16)(0xFFF))
 *
 *    @param  - ENABLED(1)    - Which uses the inbuilt CRC engine IELF tool by the IAR compiler
 *              DISABLED(0)   – User has to configure its own CRC engine
 *
 *    @note  -  "#define INBUILT_FLASH_CRC_CALCULATOR      ENABLED"
 *              "#define INBUILT_FLASH_CRC_CALCULATOR      DISABLED"
 */
//=====================================================================================================================
#define INBUILT_FLASH_CRC_CALCULATOR        ENABLED

//=====================================================================================================================
/**
 *    @brief  - Defines the Flash memory range and the location of the stored CRC if the external crc engine is used.
 *    @details-
 *              Mandatory :
 *              1.The flash memory ranges are applicable only if the external CRC tool is used for calculation.
 *              2.The calculated CRC value should be stored at the end of the application code.
 *              3.CRC should not be calculated on the stored crc location.
 *
 *    @note  -  "#define SRMICRO_FLASH_INIT            0x08000130"
 *              "#define SRMICRO_FLASH_END             0x0801FFFB"
 *              "#define SRMICRO_CRC_FLASH             0x0801FFFC"
 */
//=====================================================================================================================
#if (INBUILT_FLASH_CRC_CALCULATOR == DISABLED)
    #define SRMICRO_FLASH_INIT            0x08000130            // It defines where the Flash starts
    #define SRMICRO_FLASH_END             0x0801FFFB            // It defines where the Flash ends
    #define SRMICRO_CRC_FLASH             0x0801FFFC            // It defines where the CRC will be stored
#endif

//=====================================================================================================================
/**
 *    @brief -  Defines the engine used for Crc calculation for the Flash boot and continuous tests
 *              if the external crc engine is used.
 *    @details- This file should have the include for the .h associated to the Crc engine and the macros related
 *              to Crc.
 *
 *    @note   -
 *              For Crc32
 *                  #include "Crc32.h"
 *                  #define CRC__PROCESS(accumulator, data)     Crc32__ProcessByte(accumulator, data)
 *                  #define CRC_DATA_TYPE                       uint32
 *                  #define SRMICRO_CRC_INITIAL                 CRC_INITIAL
 *
 *              For Crc (Crc 16 bits)
 *                  #include "Crc.h"
 *                  #define CRC__PROCESS(accumulator, data)     FastCRCUpd(accumulator, data)
 *                  #define CRC_DATA_TYPE                       uint16
 *                  #define SRMICRO_CRC_INITIAL                 ((uint16)(0xFFF))
 */
//=====================================================================================================================
#if (INBUILT_FLASH_CRC_CALCULATOR == DISABLED)
    #include "crc.h"                          //Include for the Crc engine header file;
    #define CRC__PROCESS(accumulator, data)     FastCRCUpd(accumulator, data)
    #define CRC_DATA_TYPE                       uint16                //Type of the data used for the engine;
    #define SRMICRO_CRC_INITIAL                 0x00                 //Initial value for the accumulator;
#endif

//=====================================================================================================================
/**
 *    @brief  - Defines the Enabling or disabling of the Stack Overflow and Underflow Test.
 *    @details- By enabling this it writes the test pattern on the top and bottom of the stack and
 *              periodically checks the pattern value. If the test pattern value are unchanged then there is no
 *              stack overflow and underflow or if the pattern value is over written then there is stack overflow.
 *
 *    @param  - ENABLED(1)    - Which Enables the Stack Overflow and Underflow Test
 *              DISABLED(0)   – Which Disables the Stack Overflow and Underflow Test
 *
 *    @note  -  "#define SRMICRO_STACK__UNDERFLOW_OVERFLOW_TEST      ENABLED"
 *              "#define SRMICRO_STACK__UNDERFLOW_OVERFLOW_TEST      DISABLED"
 */
//=====================================================================================================================
#define SRMICRO_STACK__UNDERFLOW_OVERFLOW_TEST      DISABLED

//=====================================================================================================================
/**
 *    @brief  - Defines if micro should halt if boot test fails.
 *    @details -If the SRMICRO_HALT_ON_FAIL is ENABLED then-
 *              User can perform the fail routine task in the SRMICRO__TASKONFAIL() which executes
 *              in the continuous loop.
 *
 *              If the SRMICRO_HALT_ON_FAIL is DISABLED then-
 *              System resets
 *
 *
 *    @param  - ENABLED(1)    - Halt Micro on fail and perform the task defined in the SRMICRO__TASKONFAIL()
 *              DISABLED(0)   – Reset Micro on fail
 */
//=====================================================================================================================
#define SRMICRO_HALT_ON_FAIL                        ENABLED

//=====================================================================================================================
/**
 *    @brief  - Defines action in Micro Halt if boot test fail.
 *    @details -This could be used only if the SRMICRO_HALT_ON_FAIL is ENABLED.
 *              The task defined in this are executed in continuous loop.
 *
 *    @param  - User defined application function prototype for handling the Boot fail.
 *
 *    @note  -  "#define SRMICRO__TASKONFAIL()     __disable_interrupt()"
 *              "#define SRMICRO__TASKONFAIL()     User_Application _BootFail_Task()"
 */
//=====================================================================================================================
#define SRMICRO__TASKONFAIL()                       //__disable_interrupt()

//=====================================================================================================================
/**
 *    @brief  - Call-back for FLASH continuous mode test error.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the FLASH continuous mode test error.
 *
 *    @note  -  "#define SRMICRO_EXCEPTION_CONTINUOUS_FLASH(calc,expected)   SRException__Queue(FLASH_ERROR,calc,expected)"
 */
//=====================================================================================================================
#define SRMICRO_EXCEPTION_CONTINUOUS_FLASH(calc,expected)   //SRException__Queue(FLASH_ERROR,calc,expected)

//=====================================================================================================================
/**
 *    @brief  - Call-back for RAM continuous mode test error.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the RAM continuous mode test error.
 *
 *    @note  -  "#define SRMICRO_EXCEPTION_CONTINUOUS_RAM(position)  SRException__Queue(RAM_ERROR,position,0)"
 */
//=====================================================================================================================
#define SRMICRO_EXCEPTION_CONTINUOUS_RAM(position)          //SRException__Queue(RAM_ERROR,(uint8)position,0)

//=====================================================================================================================
/**
 *    @brief  - Call-back for CPU Register Runtime Test Fail.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the CPU Register Runtime Test Fail.
 *
 *    @note  -  "#define SRMICRO_EXCEPTION_RUNTIME_CPU_REGISTER()  SRException__Queue(CPU_REGISTER_ERROR,0,0)"
 */
//=====================================================================================================================
#define SRMICRO_EXCEPTION_RUNTIME_CPU_REGISTER()            //SRException__Queue(CPU_REGISTER_ERROR,0,0)

//=====================================================================================================================
/**
 *    @brief  - Call-back for Stack overflow exception.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the Stack overflow exception.
 *
 *    @note  -  "#define SRMICRO_EXCEPTION_STACK_OVERFLOW()  SRException__Queue(STACK_OVERFLOW_ERROR,0,0)"
 *
 */
//=====================================================================================================================
#define SRMICRO_EXCEPTION_STACK_OVERFLOW()                  //SRException__Queue(STACK_OVERFLOW_ERROR,0,0)

//=====================================================================================================================
/**
 *    @brief  - Call-back for Stack underflow exception.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the Stack underflow exception.
 *
 *    @note  -  "#define SRMICRO_EXCEPTION_STACK_UNDERFLOW()  SRException__Queue(STACK_UNDERFLOW_ERROR,0,0)"
 *
 */
//=====================================================================================================================
#define SRMICRO_EXCEPTION_STACK_UNDERFLOW()             //SRException__Queue(STACK_UNDERFLOW_ERROR,0,0)

//=====================================================================================================================
/**
 *    @brief  - Call-back for Flow control when the Stack overflow and underflow is tested.
 *    @details- The macros should be defined connecting the macro with the SRTaskSequence modules.
 *
 *    @param  - User defined application function prototype to log the Flow control when the Stack overflow and
 *              underflow is tested during the run time.
 *
 *    @note  -  "#define SRMICRO_FLOW_LOG_STACK_UNDERFLOW_OVERFLOW()    SRFlow__MainLogEvent(STACK_OVERFLOW_TEST_EVENT)"
 */
//=====================================================================================================================
#define SRMICRO_FLOW_LOG_STACK_UNDERFLOW_OVERFLOW()     //SRFlow__MainLogEvent(STACK_OVERFLOW_TEST_EVENT)

//=====================================================================================================================
/**
 *    @brief  - Call-back for Flow control when the RAM is tested.
 *    @details- The macros should be defined connecting the macro with the SRFlow modules.
 *
 *    @param  - User defined application function prototype to log the Flow control when the RAM is tested
 *              during the run time.
 *
 *    @note  -  "#define SRMICRO_FLOW_LOG_RAM_TEST_BEGIN()    SRFlow__MainLogEvent(SRFLOW_SRRAMTEST_MAIN_BEGIN)"
 */
//=====================================================================================================================
#define SR_RAMTEST_FLOW_BEGIN()              SRFlow__MainLogEvent(SRFLOW_SRRAMTEST_MAIN_BEGIN);
#define SR_RAMTEST_FLOW_END()                SRFlow__MainLogEvent(SRFLOW_SRRAMTEST_MAIN_END);

//=====================================================================================================================
/**
 *    @brief  - Call-back for Flow control when the FLASH is tested;
 *    @details- The macros should be defined connecting the macro with the SRFlow modules
 *
 *    @param  - User defined application function prototype to log the Flow control when the FLASH is tested
 *              during the run time.
 *
 *    @note  -  "#define SRMICRO_FLOW_LOG_FLASH_TEST_BEGIN()    SRFlow__MainLogEvent(SRFLOW_SRFLASHTEST_MAIN_BEGIN)"
 */
//=====================================================================================================================
#define SR_FLASHTEST_FLOW_BEGIN()            SRFlow__MainLogEvent(SRFLOW_SRFLASHTEST_MAIN_BEGIN);
#define SR_FLASHTEST_FLOW_END()              SRFlow__MainLogEvent(SRFLOW_SRFLASHTEST_MAIN_END);

//=====================================================================================================================
/**
 *    @brief  - Call-back for SRFlow control when the SRMicro__Initialize() API is called.
 *    @details- The macros should be defined connecting the macro with the SRFlow modules.
 *
 *    @param  - User defined application function prototype to log the Flow control
 *              when the SRMicro__Initialize() API is called.
 *
 *    @note  -  "#define SRMICRO_FLOW_INITIALIZE_BEGIN()             SRFlow__InitLogEvent(SRFLOW_SRMICRO_INIT_BEGIN)"
 */
//=====================================================================================================================
#define SR_MICRO_INITIALIZE_FLOW_BEGIN()               SRFlow__InitLogEvent(SRFLOW_SRMICRO_INIT_BEGIN);
#define SR_MICRO_INITIALIZE__FLOW_END()                SRFlow__InitLogEvent(SRFLOW_SRMICRO_INIT_END);

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Defines the action on SR Test failure during the boot time
 *    @details - Micro is reset if this macro is commented
 *
 *    @param  - User defined application function prototype for handling the Boot fail.
 *
 *    @note  -  "#define SRMICRO__TASKONFAIL()     __disable_interrupt()"
 *              "#define SRMICRO__TASKONFAIL()     User_Application _BootFail_Task()"
 *              "//#define MICRO_SR_BOOTTEST_FAILURE_TASK()  //This will call the micro reset"
 */
//#define MICRO_SR_BOOTTEST_FAILURE_TASK()                        //__disable_interrupt()

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Call-back for FLASH continuous mode test error.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the FLASH continuous mode test error.
 *
 *    @note  -  "#define SRMICRO_EXCEPTION_CONTINUOUS_FLASH(calc,expected)   SRException__Queue(FLASH_ERROR,calc,expected)"
 */
#define MICRO_SR_EXCEPTION_CONTINUOUS_FLASH(calc,expected)      SRException__Queue(EXCEPTION_FLASH_ERROR,calc,expected)

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Call-back for RAM continuous mode test error.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the RAM continuous mode test error.
 *
 *    @note  -  "#define MICRO_SR_EXCEPTION_CONTINUOUS_RAM(position)  SRException__Queue(RAM_ERROR,position,0)"
 */
#define MICRO_SR_EXCEPTION_CONTINUOUS_RAM(position)             SRException__Queue(EXCEPTION_RAM_RUNTIME_CORRUPTION,(uint8)position,0)

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Call-back for RAM startup mode test error.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the RAM continuous mode test error.
 *
 *    @note  -  "#define MICRO_SR_EXCEPTION_STARTUP_RAM(position)  SRException__Queue(RAM_ERROR,position,0)"
 */
#define MICRO_SR_EXCEPTION_STARTUP_RAM()                        SRException__Queue(EXCEPTION_RAM_STARTUP_CORRUPTION,0,0)


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Call-back for CPU Register Runtime Test Fail.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the CPU Register Runtime Test Fail.
 *
 *    @note  -  "#define MICRO_SR_EXCEPTION_RUNTIME_CPU_REGISTER()  SRException__Queue(CPU_REGISTER_ERROR,0,0)"
 */
#define MICRO_SR_EXCEPTION_RUNTIME_CPU_REGISTER()                       SRException__Queue(EXCEPTION_CPU_REGISTER_ERROR,0,0)


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Call-back for CPU Register Boot Test Fail.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the CPU Register Boot Test Fail.
 *
 *    @note  -  "#define MICRO_SR_EXCEPTION_STARTUP_CPU_REGISTER()  SRException__Queue(EXCEPTION_CPU_REGISTER_ERROR,0,0)"
 */
#define MICRO_SR_EXCEPTION_STARTUP_CPU_REGISTER()                       SRException__Queue(EXCEPTION_CPU_REGISTER_ERROR,0,0)

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Call-back for Stack overflow exception.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the Stack overflow exception.
 *
 *    @note  -  "#define MICRO_SR_EXCEPTION_STACK()  SRException__Queue(EXCEPTION_STACK_OVERFLOW_ERROR,0,0)"
 *
 */
#define MICRO_SR_EXCEPTION_STACK_OVERFLOW()                              SRException__Queue(EXCEPTION_STACK_OVERFLOW_ERROR,0,0)

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Call-back for Stack underflow exception.
 *    @details- The macros should be defined connecting the exceptions with the error handling module.
 *
 *    @param  - User defined application function prototype for handling the Stack Underflow exception.
 *
 *    @note  -  "#define MICRO_SR_EXCEPTION_STACK()  SRException__Queue(EXCEPTION_STACK_UNDERFLOW_ERROR,0,0)"
 *
 */
#define MICRO_SR_EXCEPTION_STACK_UNDERFLOW()                              SRException__Queue(EXCEPTION_STACK_UNDERFLOW_ERROR,0,0)

//=====================================================================================================================
/**
 *  @brief  -  Call-back for local LPTIMER clock continuous mode test error;
 *  @details-  The macros should be defined connecting the exceptions with the error handling module
 *
 *  @example -
 *              #define SRMICRO_EXCEPTION_CONTINUOUS_LOCO()  SRClockMonitor__MonitorExtClk(70)
 *               the count 50 is for LPTIMER (LPO clock) = 1Khz.
 *               This count will be different for different frequency
 */
//=====================================================================================================================
//#include "SRClockMonitor.h"
#define SRMICRO_EXCEPTION_CONTINUOUS_LPTIMER_HANDLER()         SRClockMonitor__MonitorExtClk(200)
//=====================================================================================================================



#endif // MICRO_PRV_H_

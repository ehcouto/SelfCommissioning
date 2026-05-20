/**
 *  @defgroup   CLASS_B
 *  @file       SRMCAtod.h
 *  @brief      Header for motor control analog conversions
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SRMCATOD_H_
	#define SRMCATOD_H_
#include "SRMCAtod_prm.h"
#include "SRMCAtod_macros.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


typedef enum
{
    SR_AD_ENGINE_0,
    SR_AD_ENGINE_1
} SRMCATOD_CONVERTER_TYPE;

typedef enum
{
    SR_MCATOD_SELF_TEST_DISABLED,
    SR_MCATOD_SELF_TEST_VREFH0_SCALE_0,
    SR_MCATOD_SELF_TEST_VREFH0_SCALE_0_5,
    SR_MCATOD_SELF_TEST_VREFH0_SCALE_1,
    SR_MCATOD_AUTOMATED_SELF_TEST
} SRMCATOD_DIAG_CONFIG_TYPE;

typedef enum
{
    SR_MCATOD_ERROR_TEST_NOT_PERFORMED      = 0,
    SR_MCATOD_ERROR_TEST_NOT_INITIALIZED    = 1,
    SR_MCATOD_ERROR_REGISTER_BIT_STUCK_AT_0 = 2,
    SR_MCATOD_ERROR_REGISTER_BIT_STUCK_AT_1 = 3,
    SR_MCATOD_ERROR_CONVERTER_ERROR         = 4,
    SR_MCATOD_ERROR_PERIPHERAL_NOT_SAFE     = 5,
    SR_MCATOD_ERROR_PERIPHERAL_SAFE         = 6
} SRMCATOD_DIAG_ERROR_TYPE;

typedef enum
{
    SR_MCATOD_WAITING_TO_CHECK,
    SR_MCATOD_START_EVAL,
    SR_MCATOD_CHECK_STUCK_AT_1,
    SR_MCATOD_CHECK_STUCK_AT_0,
    SR_MCATOD_CHECK_CONVERTER_ERROR,
    SR_MCATOD_FEEDBACK_UPDATED
} SRMCATOD_DIAG_STATE_TYPE;

//--------------------------------Macros--------------------------------------
#define SRMCATOD__GETADC1()           SRMCATOD_I1_DATA_REG()
#define SRMCATOD__GETADC2()           SRMCATOD_I2_DATA_REG()
#define SRMCATOD__GETADC3()           SRMCATOD_I3_DATA_REG()
#define SRMCATOD__GETDCBUSVOLTAGE()   SRMCATOD_HVDC_BUS_DATA_REG()

//------------------------------ Additional MACROS for 3rd current reconstruction strategy  ----
#define SRMCATOD__GETADC2_REC1()      SRMCATOD_I2_DATA_REG_REC1()
#define SRMCATOD__GETADC3_REC1()      SRMCATOD_I3_DATA_REG_REC1()

#define SRMCATOD__GETADC1_REC2()      SRMCATOD_I1_DATA_REG_REC2()
#define SRMCATOD__GETADC3_REC2()      SRMCATOD_I3_DATA_REG_REC2()

#define SRMCATOD__GETADC1_REC3()      SRMCATOD_I1_DATA_REG_REC3()
#define SRMCATOD__GETADC2_REC3()      SRMCATOD_I2_DATA_REG_REC3()


#define SRMCATOD__SETADC2_REC1()      SRMCATOD_I2_ADC_CONF_REC1()
#define SRMCATOD__SETADC3_REC1()      SRMCATOD_I3_ADC_CONF_REC1()

#define SRMCATOD__SETADC1_REC2()      SRMCATOD_I1_ADC_CONF_REC2()
#define SRMCATOD__SETADC3_REC2()      SRMCATOD_I3_ADC_CONF_REC2()

#define SRMCATOD__SETADC1_REC3()      SRMCATOD_I1_ADC_CONF_REC3()
#define SRMCATOD__SETADC2_REC3()      SRMCATOD_I2_ADC_CONF_REC3()

#define SRMCATOD__SETADC1_DEFAULT()             SRMCATOD_I1_ADC_CONF_DEF()
#define SRMCATOD__SETADC2_DEFAULT()             SRMCATOD_I2_ADC_CONF_DEF()
#define SRMCATOD__SETADC3_DEFAULT()             SRMCATOD_I3_ADC_CONF_DEF()
#define SRMCATOD__SETDCBUSVOLTAGE_DEFAULT()     SRMCATOD_DCBUS_ADC_CONF_DEF()

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRMCAtod__Initialize(void);
INTERRUPT_DIRECTIVE void SRMCAtod__ISR(void);
INTERRUPT_DIRECTIVE void SRMCPDB__ISR(void);

void SRMCAtod__DiagnosticHandler(void);
void SRMCAtod__EnableDiagnostic(sint16 commanded_threshold);
SRMCATOD_DIAG_ERROR_TYPE SRMCAtod__GetDiagnosticFeedback(void);
#if (SRMCATOD_ENABLE_ADDITIONAL_READING == 1)
void SRMCAtod__ConvertManualSample(void);
sint16 SRMCAtod__GetLastManualSample(void);
void SRMCAtod__ConfigureManualReading(SRMCATOD_CONVERTER_TYPE engine, SRMCATOD_CHANNEL_DEF channel, BOOL_TYPE use_mux);
#endif
#endif // SRMCATOD_H_

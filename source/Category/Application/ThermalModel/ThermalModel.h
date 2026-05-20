/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: ThermalModel.h 1.5.3.2.2.1.1.1 2016/05/25 07:47:04EDT Eduardo Couto (HENRIE2) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2016/05/25 07:47:04EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef THERMALMODEL_H_
#define THERMALMODEL_H_
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
    NOT_DEFINED = 0,
    sBPM,
    DD,
} OTE_MOTOR_ENUM_TYPE;



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ThermalModel__Initialize(void);
void ThermalModel__Handler25ms(void);
void ThermalModel__SetStatorTemperature(float input_temperature);
float ThermalModel__GetStatorTemperature(void);
OTE_MOTOR_ENUM_TYPE ThermalModel__GetMotorType(void);
//sint32 ThermalModel__GetValue(THERMAL_MODEL_RETURN_TYPE thermal_input);
//float32 ThermalModel__GetValueF(THERMAL_MODEL_RETURN_TYPE thermal_input);


#endif // THERMALMODEL_H_



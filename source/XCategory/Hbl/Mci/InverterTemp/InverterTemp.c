/**
 *  @file       InverterTemp.c
 *
 *  @brief      Inverter (silicon) NTC temperature
 *
 *  @details    Reads the analog input connected to an external NTC attached to the inverter silicon.
 *              This module reads the analog data, filters it out and scale the analog temperature
 *              in to Celsius degree.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: InverterTemp.c 1.6 2016/01/13 20:08:12CET Marcelo Silva (SILVAMC) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2016/01/13 20:08:12CET $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "InverterTemp.h"
#include "InverterTemp_prv.h"
#include "Filters.h"
#include "MathCalc.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


#ifdef MCI_MEASURE_INVERTER_TEMP
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#ifndef INVERTER_TEMP_SETTINGFILE_SUPPORT
    #define INVERTER_TEMP_SETTINGFILE_SUPPORT DISABLED
#endif

//! History of the filter
static sint32 Temperature_Sum;

//! Output of the lookup table
static sint32 Temperature_Degrees;

//! Raw ADC filtered value
static sint32 Temperature_Raw;

static MATHCALC_LUT_EXT_TYPE Actual_Inverter_Temp_Ext_LUT;

#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
static INVERTER_TEMP_PARAMS_DISPL_TYPE* InverterTemp_Params_Displ;

static BOOL_TYPE Inverter_Temp_Params_Loaded;
#endif // INVERTER_TEMP_SETTINGFILE_SUPPORT
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module InverterTemp and its variables
 *
 */
void InverterTemp__Initialize(void)
{
    // Initialize the ADC engine
    ATOD_INITIALIZE();
    // Start the ADC engine
    ATOD_START_CONVERSION();

#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
    Inverter_Temp_Params_Loaded = FALSE;
    Temperature_Degrees = 0;
    Temperature_Sum = 0;
    Temperature_Raw = 0;
#else
    // LUT initialization
    Actual_Inverter_Temp_Ext_LUT = Inverter_Temp_LUT_Ext_Array[INVERTER_TEMP_DEFAULT_LUT].ext_lut;

    // Initialize the temperatures to the default values
    Temperature_Sum = Inverter_Temp_LUT_Ext_Array[INVERTER_TEMP_DEFAULT_LUT].default_temp_adc  << INVERTER_TEMP_LPF_SHIFT;
    Temperature_Raw = Inverter_Temp_LUT_Ext_Array[INVERTER_TEMP_DEFAULT_LUT].default_temp_adc;

    Temperature_Degrees = MathCalc__GetInterpolationFast(Temperature_Raw, &Actual_Inverter_Temp_Ext_LUT);
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/*
 *  @brief      Handle the temperature read from the analog channel. It shall only
 *              filter the analog data read.
 *              Rate: it can be defined according to the setting the LPF filter shift.
 */
void InverterTemp__Handler(void)
{
    sint16 temp_adc;

#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
    if (Inverter_Temp_Params_Loaded)
    {
#endif
        // Read the adc inverter temperature channel
        temp_adc = ATOD_GET_TEMPERATURE();

        // Start the adc convertion to the next read
        ATOD_START_CONVERSION();

        // Low pass filter the converted adc data
        FILTERS__FILTER_CALC(temp_adc, Temperature_Sum, INVERTER_TEMP_LPF_SHIFT);

        // Get filtered adc value
        Temperature_Raw = FILTERS__FILTER_GET_FILTERED_VAR(Temperature_Sum, INVERTER_TEMP_LPF_SHIFT);

        // LUT and interpolation
        Temperature_Degrees = MathCalc__GetInterpolationFast(Temperature_Raw, &Actual_Inverter_Temp_Ext_LUT);
#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
    }
#endif
}





//---------------------------------------------------------------------------------------------------------------------
/*
 *  @brief      Handle the temperature read from the analog channel. It shall only
 *              filter the analog data read.
 *              Rate: it can be defined according to the setting the LPF filter shift.
 */
void InverterTemp__PwmHandler(void)
{
    INVERTER_TEMP_START__MANUAL_CONVERSION();
}

#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == DISABLED)
//---------------------------------------------------------------------------------------------------------------------
/*
 *  @brief      Set the LUT for temperature conversion.
 *	
 *	@details	This function will set the LUT for temperature conversion according
 *				to the specified index. If the index is out of range, the set will be
 *				discarded. \n
 *				The temperature will also be initialized to the specified value.
 *	
 *	@param[in]	lut_index The index to the LUT to set, as defined in the prm
 */
void InverterTemp__SetLUT(INVERTERTEMP_LUT_INDEX_TYPE lut_index)
{
    if (lut_index < sizeof(Inverter_Temp_LUT_Ext_Array)/sizeof(INVERTER_TEMP_LUT))
    {
        Actual_Inverter_Temp_Ext_LUT = Inverter_Temp_LUT_Ext_Array[lut_index].ext_lut;
        Temperature_Sum = Inverter_Temp_LUT_Ext_Array[lut_index].default_temp_adc;
        Temperature_Raw = Inverter_Temp_LUT_Ext_Array[lut_index].default_temp_adc;
    }
}
#endif // INVERTER_TEMP_SETTINGFILE_SUPPORT
//---------------------------------------------------------------------------------------------------------------------
/*
 *  @brief      Get the inverter temperature.
 *
 *  @return     Inverter temperature [degC x65536 or <<16]
 */
sint32 InverterTemp__GetInverterTemp_s16(void)
{
    return(Temperature_Degrees << 16);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get the ADC read of the inverter temperature
 * @return      ADC result of the temperature sensor [ADC counts]
 */
sint16 InverterTemp__GetInverterRawTemp(void)
{
    return(Temperature_Raw);
}



//---------------------------------------------------------------------------------------------------------------------
/*
 *  @brief  Compare the given temperature to a given threshold. There is no need to
 *          calculate the analog temperature.
 *  @param  [input] temperature [degC x65536 or <<16]
 *  @return [output] TRUE: temperature is equal or higher than the threshold
 *  @return [output] FALSE: temperature is lower than the threshold
 */
BOOL_TYPE InverterTemp__AboveThreshold_s16(sint32 temp_shift_16)
{
    sint32 temp_thr;
    BOOL_TYPE ret_val;

    // Scale the given threshold
    temp_thr = (temp_shift_16>>16);

    // Check the threshold
    if(Temperature_Degrees >= temp_thr)
    {
        ret_val = TRUE;
    }
    else
    {
        ret_val = FALSE;
    }

    return(ret_val);
}

#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Write the address of Inverter Temp parameters.
 *  @details
 *
 * @return  TRUE - value accepted
 *          FALSE - value denied - default answer.
 */
BOOL_TYPE InverterTemp__SetParams(void*  invtemp_params_address)
{
    BOOL_TYPE ret_val = FALSE;

    if (invtemp_params_address != NULL)
    {
        InverterTemp_Params_Displ = (INVERTER_TEMP_PARAMS_DISPL_TYPE*) invtemp_params_address;

        Temperature_Raw = InverterTemp_Params_Displ->Default_Temp_Adc;
        Temperature_Sum = InverterTemp_Params_Displ->Default_Temp_Adc << INVERTER_TEMP_LPF_SHIFT;

        Actual_Inverter_Temp_Ext_LUT.ptr_LUT = InverterTemp_Params_Displ->Inverter_Temp_Lut;
        Actual_Inverter_Temp_Ext_LUT.sizeof_lut = &InverterTemp_Params_Displ->Sizeof_Inverter_Temp_Lut;
        Actual_Inverter_Temp_Ext_LUT.step_inv = &InverterTemp_Params_Displ->Step_Inv_Inverter_Temp;

        Temperature_Degrees = MathCalc__GetInterpolationFast(Temperature_Raw, &Actual_Inverter_Temp_Ext_LUT);

        Inverter_Temp_Params_Loaded = TRUE;
        ret_val = TRUE;
    }

    return ret_val;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get parameters loaded status.
 *  @details
 *
 * @return  TRUE - parameters are loaded
 *          FALSE - parameters not loaded
 */
BOOL_TYPE InverterTemp__GetParameterLoaded(void)
{
    return Inverter_Temp_Params_Loaded;
}

#endif // INVERTER_TEMP_SETTINGFILE_SUPPORT
#endif

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: Mci_prm/InverterTemp_prv.h 1.4 2016/01/25 07:24:10CET Marcelo Silva (SILVAMC) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2016/01/25 07:24:10CET $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef INVERTERTEMP_PRV_H_
    #define INVERTERTEMP_PRV_H_

//! Choose whether to use Motor Control ADC driver or the generic one
#define INVERTER_TEMP_USE_SRMCATOD              1


typedef struct
{
    MATHCALC_LUT_EXT_TYPE ext_lut;                //<! LUT
    sint16 default_temp_adc;                      //<! Default ADC counts
} INVERTER_TEMP_LUT;

//! Define the adc channel to be used
#define INVERTER_TEMP_ADC_CHANNEL               0 //ADC0_THIRD_SAMPLE

//! Define the inverter temperature ADC resolution
#define INVERTER_TEMP_ADC_RESOLUTION            ATOD_RESOLUTION_12BITS

//! Define the cut off and frequency of the low pass filter. Refer to the MathCalc module.
//! NOTE: the sampling frequency has to match the udpate rate of the filter!
#define INVERTER_TEMP_LPF_SHIFT                 3 //FILTER_SHIFT_FS_40HZ_FC_0.868HZ


//---------------------------------------------------------------------------------------------------------------------
// ST IPM Parameters
//---------------------------------------------------------------------------------------------------------------------

//! Look Up Table to convert degC to ADC. Linear interpolation will be used for intermediate points.
MATHCALC_LUT_TYPE Inverter_Temp_LUT_ST[] =
{
        // Input ADC      Output - temperature
        {70 ,            3  },
        {90 ,            6  },
        {110 ,           8  },
        {130 ,           11 },
        {150 ,           13 },
        {170 ,           16 },
        {190 ,           18 },
        {210 ,           20 },
        {230 ,           22 },
        {250 ,           24 },
        {270 ,           26 },
        {290 ,           28 },
        {310 ,           30 },
        {330 ,           32 },
        {350 ,           33 },
        {370 ,           35 },
        {390 ,           37 },
        {410 ,           38 },
        {430 ,           39 },
        {450 ,           41 },
        {470 ,           42 },
        {490 ,           44 },
        {510 ,           45 },
        {530 ,           46 },
        {550 ,           47 },
        {570 ,           48 },
        {590 ,           49 },
        {610 ,           50 },
        {630 ,           51 },
        {650 ,           52 },
        {670 ,           53 },
        {690 ,           54 },
        {710 ,           55 },
        {730 ,           56 },
        {750 ,           57 },
        {770 ,           58 },
        {790 ,           59 },
        {810 ,           60 },
        {830 ,           60 },
        {850 ,           61 },
        {870 ,           62 },
        {890 ,           63 },
        {910 ,           64 },
        {930 ,           64 },
        {950 ,           65 },
        {970 ,           66 },
        {990 ,           66 },
        {1010,           67 },
        {1030,           68 },
        {1050,           69 },
        {1070,           69 },
        {1090,           70 },
        {1110,           71 },
        {1130,           71 },
        {1150,           72 },
        {1170,           73 },
        {1190,           73 },
        {1210,           74 },
        {1230,           75 },
        {1250,           75 },
        {1270,           76 },
        {1290,           77 },
        {1310,           77 },
        {1330,           78 },
        {1350,           79 },
        {1370,           79 },
        {1390,           80 },
        {1410,           80 },
        {1430,           81 },
        {1450,           82 },
        {1470,           82 },
        {1490,           83 },
        {1510,           84 },
        {1530,           84 },
        {1550,           85 },
        {1570,           86 },
        {1590,           86 },
        {1610,           87 },
        {1630,           88 },
        {1650,           88 },
        {1670,           89 },
        {1690,           90 },
        {1710,           90 },
        {1730,           91 },
        {1750,           91 },
        {1770,           92 },
        {1790,           93 },
        {1810,           93 },
        {1830,           94 },
        {1850,           95 },
        {1870,           95 },
        {1890,           96 },
        {1910,           97 },
        {1930,           97 },
        {1950,           98 },
        {1970,           98 },
        {1990,           99 },
        {2010,           100},
        {2030,           100},
        {2050,           101},
        {2070,           102},
        {2090,           102},
        {2110,           103},
        {2130,           104},
        {2150,           104},
        {2170,           105},
        {2190,           105},
        {2210,           106},
        {2230,           107},
        {2250,           107},
        {2270,           108},
        {2290,           109},
        {2310,           109},
        {2330,           110},
        {2350,           111},
        {2370,           112},
        {2390,           112},
        {2410,           113},
        {2430,           114},
        {2450,           114},
        {2470,           115},
        {2490,           116},
        {2510,           117},
        {2530,           117},
        {2550,           118},
        {2570,           119},
        {2590,           120},
        {2610,           121},
        {2630,           122},
        {2650,           123},
        {2670,           124},
        {2690,           125},
        {2710,           126},
        {2730,           127},
        {2750,           128},
        {2770,           129},
        {2790,           130},
        {2810,           131},
        {2830,           132},
        {2850,           133},
        {2870,           135},
        {2890,           136},
        {2910,           138},
        {2930,           139},
        {2950,           140},
        {2970,           142},
        {2990,           144},
        {3010,           145},
        {3030,           147},
        {3050,           149},
        {3070,           151},
};

//! Inverse X step for the LUT
sint32 Inv_Temp_LUT_X_Inv_Step_ST = (sint32)(1.0f/(20)*32768);

//! Size of the LUT
sint32 SizeOf_Inv_Temp_LUT_ST = sizeof(Inverter_Temp_LUT_ST)/sizeof(MATHCALC_LUT_TYPE);

//! Initial value for the ADC temperature
#define DEFAULT_ADC_TEMP_ST                 570 // 25 DegC

//---------------------------------------------------------------------------------------------------------------------
// Mitsubishi IPM Parameters
//---------------------------------------------------------------------------------------------------------------------

//! Look Up Table to convert degC to ADC. Linear interpolation will be used for intermediate points.
MATHCALC_LUT_TYPE Inverter_Temp_LUT_Mitsubishi[] =
{
         // Input ADC     Output - temperature
        { 586,           0  },
        { 2327,          55 },
        { 4068,          110},
};

//! Inverse X step for the LUT
sint32 Inv_Temp_LUT_X_Inv_Step_Mitsubishi = (sint32)(1.0f/(1741)*32768);

//! Size of the LUT
sint32 SizeOf_Inv_Temp_LUT_Mitsubishi = sizeof(Inverter_Temp_LUT_Mitsubishi)/sizeof(MATHCALC_LUT_TYPE);

//! Initial value for the ADC temperature
#define DEFAULT_ADC_TEMP_MITSUBISHI         1377 // 25 DegC

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//! Associate the extended LUT info with the relevant index
INVERTER_TEMP_LUT Inverter_Temp_LUT_Ext_Array[] =
{
    {.ext_lut.ptr_LUT=Inverter_Temp_LUT_ST, .ext_lut.step_inv=&Inv_Temp_LUT_X_Inv_Step_ST, .ext_lut.sizeof_lut=&SizeOf_Inv_Temp_LUT_ST, .default_temp_adc=DEFAULT_ADC_TEMP_ST},
    {.ext_lut.ptr_LUT=Inverter_Temp_LUT_Mitsubishi, .ext_lut.step_inv=&Inv_Temp_LUT_X_Inv_Step_Mitsubishi, .ext_lut.sizeof_lut=&SizeOf_Inv_Temp_LUT_Mitsubishi, .default_temp_adc=DEFAULT_ADC_TEMP_MITSUBISHI},
};

//! Choose the initial (default) LUT
#define INVERTER_TEMP_DEFAULT_LUT               INVERTERTEMP_LUT_ST

//! Customized functions
#if (INVERTER_TEMP_USE_SRMCATOD == 1)
    #include "SRMCAtod.h"
    #define    ATOD_INITIALIZE()
    #define    ATOD_START_CONVERSION()
    #define    ATOD_GET_TEMPERATURE()   SRMCAtod__GetLastManualSample()
    #define    INVERTER_TEMP_START__MANUAL_CONVERSION()  SRMCAtod__ConvertManualSample()
#else
    #define    ATOD_INITIALIZE()        Atod__Initialize(INVERTER_TEMP_ADC_CHANNEL, INVERTER_TEMP_ADC_RESOLUTION)
    #define    ATOD_START_CONVERSION()  Atod__Start(INVERTER_TEMP_ADC_CHANNEL, ATOD_ONE_SHOT)
    #define    ATOD_GET_TEMPERATURE()   (sint16)Atod__Read(INVERTER_TEMP_ADC_CHANNEL)
    #define    INVERTER_TEMP_START__MANUAL_CONVERSION()
#endif


#endif // INVERTERTEMP_PRV_H_

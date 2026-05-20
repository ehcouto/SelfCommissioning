/**
 *  @file       MciParametersLoaders.c
 *
 *  @brief      Mci Parameters Loader module
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MciParametersLoader.h"
#include "MciParametersLoader_prv.h"
#include "Mci.h"
#include "SrMotorSafetyMgr.h"

#include "string.h"
#include "Crc16.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
// ----------------- CLASS A AUTOMATED DEFINITIONS ------------------------
#ifdef INTERNAL_DISPLS
const uint16 Displ_Idxs_Internal[] = INTERNAL_DISPLS;
const uint32 Displ_Sizes_Internal[] = INTERNAL_DISPL_SIZES;
#define NUM_INTERNAL_DISPL  sizeof(Displ_Sizes_Internal)/sizeof(uint32)
#else
#define NUM_INTERNAL_DISPL 0
#endif

#ifdef SF_RAM_DISPLS
const uint16 Displ_Idxs_Ram[] = SF_RAM_DISPLS;
#define NUM_SF_RAM_DISPL  sizeof(Displ_Idxs_Ram)/sizeof(uint16)
#else
#define NUM_SF_RAM_DISPL 0
#endif

#ifdef SF_FLASH_DISPLS
const uint16 Displ_Idxs_Flash[] = SF_FLASH_DISPLS;
#define NUM_SF_FLASH_DISPL  sizeof(Displ_Idxs_Flash)/sizeof(uint16)
#else
#define NUM_SF_FLASH_DISPL 0
#endif

// Total number
#define  MCI_CLASS_A_DISPLACEMENTS     NUM_INTERNAL_DISPL + NUM_SF_RAM_DISPL + NUM_SF_FLASH_DISPL

// understand if any internally loaded
#ifdef INTERNAL_DISPLS
    #define MCI_CLASS_A_PARAMETERS_LOADER_INTERNAL                  ENABLED
#else
    #define MCI_CLASS_A_PARAMETERS_LOADER_INTERNAL                  DISABLED
#endif

// understand if any loaded from Setting file (to be used directly from flash)
#ifdef SF_FLASH_DISPLS
    #define MCI_CLASS_A_PARAMETERS_LOADER_SF                  ENABLED
#else
    #define MCI_CLASS_A_PARAMETERS_LOADER_SF                  DISABLED
#endif

// understand if any loaded from Setting file (to be buffered in ram)
#ifdef SF_RAM_DISPLS
    #define MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM                  ENABLED
#else
    #define MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM                  DISABLED
#endif

#if (MCI_CLASS_A_PARAMETERS_LOADER_INTERNAL == ENABLED)
//Some Mci Class-A displacements are not from setting file
    #include INTERNAL_CLASS_A_PARAMS_INCLUDE_FILE
#endif

// ----------------- CLASS B AUTOMATED DEFINITIONS ------------------------
#ifdef INTERNAL_CLASSB_DISPLS
const uint16 Displ_Sr_Idxs_Internal[] = INTERNAL_CLASSB_DISPLS;
const uint32 Displ_Sr_Sizes_Internal[] = INTERNAL_CLASSB_DISPL_SIZES;
#define NUM_CLASSB_INTERNAL_DISPL  sizeof(Displ_Sr_Sizes_Internal)/sizeof(uint32)
#else
#define NUM_CLASSB_INTERNAL_DISPL 0
#endif

#ifdef SF_CLASSB_RAM_DISPLS
const uint16 Displ_Sr_Idxs_Ram[] = SF_CLASSB_RAM_DISPLS;
#define NUM_CLASSB_SF_RAM_DISPL  sizeof(Displ_Sr_Idxs_Ram)/sizeof(uint16)
#else
#define NUM_CLASSB_SF_RAM_DISPL 0
#endif

#ifdef SF_CLASSB_FLASH_DISPLS
const uint16 Displ_Sr_Idxs_Flash[] = SF_CLASSB_FLASH_DISPLS;
#define NUM_CLASSB_SF_FLASH_DISPL  sizeof(Displ_Sr_Idxs_Flash)/sizeof(uint16)
#else
#define NUM_CLASSB_SF_FLASH_DISPL 0
#endif

// Total number
#define  MCI_CLASS_B_DISPLACEMENTS     NUM_CLASSB_INTERNAL_DISPL + NUM_CLASSB_SF_RAM_DISPL + NUM_CLASSB_SF_FLASH_DISPL

// understand if any internally loaded
#ifdef INTERNAL_CLASSB_DISPLS
    #define MCI_CLASS_B_PARAMETERS_LOADER_INTERNAL                  ENABLED
#else
    #define MCI_CLASS_B_PARAMETERS_LOADER_INTERNAL                  DISABLED
#endif

// understand if any loaded from Setting file (to be used directly from flash)
#ifdef SF_CLASSB_FLASH_DISPLS
    #define MCI_CLASS_B_PARAMETERS_LOADER_SF                  ENABLED
#else
    #define MCI_CLASS_B_PARAMETERS_LOADER_SF                  DISABLED
#endif

// understand if any loaded from Setting file (to be buffered in ram)
#ifdef SF_CLASSB_RAM_DISPLS
    #define MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM                  ENABLED
#else
    #define MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM                  DISABLED
#endif


#if (MCI_CLASS_B_PARAMETERS_LOADER_INTERNAL == ENABLED)
//Some Mci Class-B displacements are not from setting file
    #include INTERNAL_CLASS_B_PARAMS_INCLUDE_FILE
#endif
//////////////////////////////////////////////////////////////////////////////////////////////

#if ((MCI_CLASS_A_PARAMETERS_LOADER_SF == ENABLED) || (MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM == ENABLED) || \
     (MCI_CLASS_B_PARAMETERS_LOADER_SF == ENABLED) || (MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM == ENABLED))
#include "Mode.h"
#include "SettingFile.h"             //Some Mci Class-A displacements are from setting file
#endif

#if (MCI_CLASS_B_PARAMETERS_LOADER_INTERNAL != ENABLED)
    #ifndef SETTING_FILE_H      // to avoid double inclusion
        #include "SettingFile.h"
    #endif
#endif

#ifndef MCI_PARAMETERS_LOADER_ADDITIONAL_CHECK
    #define MCI_PARAMETERS_LOADER_ADDITIONAL_CHECK  TRUE
#endif

#ifndef PLATFORM_USED
    #define NUCLEUS_BASED                   0
    #define PLATFORM_2_5_BASED              1

    #define PLATFORM_USED                  NUCLEUS_BASED
#endif

#if (PLATFORM_USED == PLATFORM_2_5_BASED)
#include "Params.h"
#endif

#if (MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM == ENABLED)
    // allocate memory in RAM
    uint8 Mci_ClassA_Prm_Ram[MCI_CLASS_A_PARAMETERS_SIZE_RAM];    // sum of parameters in all used displacements from RAM
#endif

#if ((MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM == ENABLED) || (MCI_CLASS_A_PARAMETERS_LOADER_SF == ENABLED))
#if (PLATFORM_USED == NUCLEUS_BASED)
    SETTINGFILE_LOADER_TYPE Mci_ClassA_Prm_Flash_Ptr;
#else
    PARAMS_LOADER_TYPE Mci_ClassA_Prm_Flash_Ptr;
#endif
#endif

#if (MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM == ENABLED)
    // allocate memory in RAM
    uint8 Mci_ClassB_Prm_Ram[MCI_CLASS_B_PARAMETERS_SIZE_RAM];    // sum of parameters in all used displacements from RAM
#endif

#if ((MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM == ENABLED) || (MCI_CLASS_B_PARAMETERS_LOADER_SF == ENABLED))

    #if (PLATFORM_USED == NUCLEUS_BASED)
        SETTINGFILE_LOADER_TYPE Mci_ClassB_Prm_Flash_Ptr;
    #else
        PARAMS_LOADER_TYPE Mci_ClassB_Prm_Flash_Ptr;
    #endif
#endif

addr_t Mcl_SF_Displs[MCI_CLASS_A_DISPLACEMENTS];
addr_t Mci_Sr_SF_Displs[MCI_CLASS_B_DISPLACEMENTS];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
void MciClassAPointerLoading(void);
void MciClassBPointerLoading(void);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MciParametersLoaders and its variables.
 *
 */
void MciParametersLoader__Initialize(void)
{

}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle Mci Parameters Loader  events of 25ms.
 *
 */
void MciParametersLoader__25msHandler(void)
{

    if((Mci__GetDigital(MOTOR0, MCI_DI_PARAMETERS_LOADED) == FALSE) && (Mci__GetAnalog(MOTOR0, MCI_AI_INTERNAL_MCI_STATE)>2))
    {

#if ((MCI_CLASS_A_PARAMETERS_LOADER_SF == ENABLED) || (MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM == ENABLED) || \
     (MCI_CLASS_B_PARAMETERS_LOADER_SF == ENABLED) || (MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM == ENABLED))
        if (Mode__IsProgrammingEngaged() == FALSE) //Mci is Initialized and parameters not loaded
#else
        if (MCI_PARAMETERS_LOADER_ADDITIONAL_CHECK)
#endif
        {
            // Load Class A Pointer data
            MciClassAPointerLoading();

            // Load Class B Pointer data
#ifndef MCI_DISABLE_CLASSB_PARAMETERS_LOADER
            MciClassBPointerLoading();
#endif
        }
    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get a class A displacement address
 *
 *  @param[in]  displacement Displacement to get, as defined in SettingFile_prm.h
 *  @return     Pointer to the address of the displacement; NULL in case the index is out of range
 */
addr_t* MciParametersLoader__GetClassADispl(uint8 displacement)
{
    addr_t* res = (addr_t*)NULL;
    uint8 i = 0;
#if (MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM == ENABLED)
    // Get the index for the given displacement
    for (i = 0; i < NUM_SF_RAM_DISPL; i++)
    {
        if (Displ_Idxs_Ram[i] == displacement)
        {
            res = Mcl_SF_Displs + i;
        }
    }
#endif
#if (MCI_CLASS_A_PARAMETERS_LOADER_SF == ENABLED)
    // Get the index for the given displacement
    for (i = 0; i < NUM_SF_FLASH_DISPL; i++)
    {
        if (Displ_Idxs_Flash[i] == displacement)
        {
            res = Mcl_SF_Displs + i;
        }
    }
#endif
#if (MCI_CLASS_A_PARAMETERS_LOADER_INTERNAL == ENABLED)
    // Get the index for the given displacement
    for (i = 0; i < NUM_INTERNAL_DISPL; i++)
    {
        if (Displ_Idxs_Internal[i] == displacement)
        {
            res = Mcl_SF_Displs + i;
        }
    }
#endif

    return res;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get a class B displacement address
 *
 *  @param[in]  displacement Displacement to get, as defined in SettingFile_prm.h
 *  @return     The address of the displacement; NULL in case the index is out of range
 */
addr_t MciParametersLoader__GetClassBDispl(uint8 displacement)
{
    addr_t res = (addr_t)NULL;
    uint8 i = 0;
#if (MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM == ENABLED)
    for (i = 0; i < MCI_CLASS_B_DISPLACEMENTS; i++)
    {
        if (Displ_Sr_Idxs_Ram[i] == displacement)
        {
            res = Mci_Sr_SF_Displs[i];
        }
    }
#endif
#if (MCI_CLASS_B_PARAMETERS_LOADER_SF == ENABLED)
    for (i = 0; i < MCI_CLASS_B_DISPLACEMENTS; i++)
    {
        if (Displ_Sr_Idxs_Flash[i] == displacement)
        {
            res = Mci_Sr_SF_Displs[i];
        }
    }
#endif
#if (MCI_CLASS_B_PARAMETERS_LOADER_INTERNAL == ENABLED)
    for (i = 0; i < MCI_CLASS_B_DISPLACEMENTS; i++)
    {
        if (Displ_Sr_Idxs_Internal[i] == displacement)
        {
            res = Mci_Sr_SF_Displs[i];
        }
    }
#endif

    return res;
}


void MciClassAPointerLoading(void)
{
    uint8 displ_address = 0;
    uint8 i = 0;
#if MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM == ENABLED
    uint8 *mci_class_a_prm_sf_ptr;           // Pointer to Ram buffer for SF copy
#endif
#if MCI_CLASS_A_PARAMETERS_LOADER_INTERNAL == ENABLED
    uint8 *mci_class_a_prm_internal_ptr;     // Pointer to Ram area for internal params (not from setting file)
    uint8 *mci_class_a_prm_internal_ptr_max; // Max Pointer value given the size of internal parameters
#endif
    // init the displacement address for the vector pointer to be passed to MCI
    displ_address = 0;

    // initialize pointer RAM buffer (copy from flash)
#if MCI_CLASS_A_PARAMETERS_LOADER_SF_RAM == ENABLED
    mci_class_a_prm_sf_ptr = Mci_ClassA_Prm_Ram;
#endif

    // initialize pointer RAM (internal params)
#if MCI_CLASS_A_PARAMETERS_LOADER_INTERNAL == ENABLED
    mci_class_a_prm_internal_ptr = (uint8*)Mcl_Params_SF;
    mci_class_a_prm_internal_ptr_max = mci_class_a_prm_internal_ptr + INTERNAL_TOT_SIZES;
#endif

    // Loading required displacements of MCI Class A pointer ...
#ifdef SF_RAM_DISPLS
    for (i = 0; i<NUM_SF_RAM_DISPL; i++)
    {

#if (PLATFORM_USED == NUCLEUS_BASED)
        // loading from setting file in Flash
        if(SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, Displ_Idxs_Ram[i], &Mci_ClassA_Prm_Flash_Ptr) == PASS)
        {
            // copy from Flash to RAM
            memcpy(mci_class_a_prm_sf_ptr, Mci_ClassA_Prm_Flash_Ptr.Data, Mci_ClassA_Prm_Flash_Ptr.Length); //Copy from Flash to RAM

            // store RAM pointer value to be passed to MCI set params
            Mcl_SF_Displs[displ_address++] = (addr_t)mci_class_a_prm_sf_ptr;

            //update RAM pointer
            mci_class_a_prm_sf_ptr += Mci_ClassA_Prm_Flash_Ptr.Length;
        }
        else
        {
            // NULL pointer in case of loading exception
            Mcl_SF_Displs[displ_address++] = (addr_t)NULL;
        }
#else
        // loading from setting file in Flash
        if(Params__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, Displ_Idxs_Ram[i], &Mci_ClassA_Prm_Flash_Ptr) == PASS)
        {
            Mcl_SF_Displs[displ_address++] = (addr_t)Mci_ClassA_Prm_Flash_Ptr.Data;
        }
        else
        {
            // NULL pointer in case of loading exception
            Mcl_SF_Displs[displ_address++] = (addr_t)NULL;
        }
#endif

    }
#endif
#ifdef SF_FLASH_DISPLS
    for (i = 0; i<NUM_SF_FLASH_DISPL; i++)
    {
        // loading from setting file in Flash
        if((SettingFile__BasicLoader(SF_PTR_MCU_CLASS_A_MCI, Displ_Idxs_Flash[i], &Mci_ClassA_Prm_Flash_Ptr) == PASS) && (!mci_sf_error))
        {
            Mcl_SF_Displs[displ_address++] = (addr_t)Mci_ClassA_Prm_Flash_Ptr.Data;
        }
        else
        {
            // NULL pointer in case of loading exception
            Mcl_SF_Displs[displ_address++] = (addr_t)NULL;
        }
    }
#endif
#ifdef INTERNAL_DISPLS
    for (i = 0; i<NUM_INTERNAL_DISPL; i++)
    {
        if (mci_class_a_prm_internal_ptr < mci_class_a_prm_internal_ptr_max)
        {
            Mcl_SF_Displs[displ_address++] = (addr_t)mci_class_a_prm_internal_ptr;
        }
        else
        {
            // NULL pointer in case of loading exception
            Mcl_SF_Displs[displ_address++] = (addr_t)NULL;
        }

        mci_class_a_prm_internal_ptr += Displ_Sizes_Internal[i];
    }
#endif

    // complete the process
    Mci__SetParams(&Mcl_SF_Displs);
#if (INVERTER_TEMP_SETTINGFILE_SUPPORT == ENABLED)
    InverterTemp__SetParams((void*)*MciParametersLoader__GetClassADispl(SF_DISPL_INVERTER_PARAMS));
#endif
#if (POWERMODULE_MANAGER_USED == ENABLED)
    PowerModuleManager__SetParams((void*)*MciParametersLoader__GetClassADispl(SF_DISPL_POWERMODULE_MGR_PARAMS));
#endif
}


void MciClassBPointerLoading(void)
{
    uint16 crc_calc;
    uint16 crc_sf;
    uint8 displ_address = 0;
    uint8 i = 0;
#if MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM == ENABLED
    uint8 *mci_class_b_prm_sf_ptr;           // Pointer to Ram buffer for SF copy
#endif
#if MCI_CLASS_B_PARAMETERS_LOADER_INTERNAL == ENABLED
    uint8 *mci_class_b_prm_internal_ptr;     // Pointer to Ram area for internal params (not from setting file)
#endif
    // init the displacement address for the vector pointer to be passed to MCI
    displ_address = 0;

    // initialize pointer RAM buffer (copy from flash)
#if MCI_CLASS_B_PARAMETERS_LOADER_SF_RAM == ENABLED
    mci_class_b_prm_sf_ptr = Mci_ClassB_Prm_Ram;
#endif

    // initialize pointer RAM (internal params)
#if MCI_CLASS_B_PARAMETERS_LOADER_INTERNAL == ENABLED
    mci_class_b_prm_internal_ptr = (uint8*)Motor_Sr_Params_SF;
#endif

    // Loading required displacements of MCI Class B pointer ...
#ifdef SF_CLASSB_RAM_DISPLS
    #if (PLATFORM_USED == NUCLEUS_BASED)
    for (i = 0; i<NUM_CLASSB_SF_RAM_DISPL; i++)
    {
        // loading from setting file in Flash
        if(SettingFile__BasicLoader(SF_PTR_MCU_CLASS_B_MCI, Displ_Sr_Idxs_Ram[i], &Mci_ClassB_Prm_Flash_Ptr) == PASS)
        {
            // copy from Flash to RAM
            memcpy(mci_class_b_prm_sf_ptr, Mci_ClassB_Prm_Flash_Ptr.Data, Mci_ClassB_Prm_Flash_Ptr.Length); //Copy from Flash to RAM

            crc_calc = Crc16__RangeProcess((uint8 *)(&mci_class_b_prm_sf_ptr[2]),0xFFFF,(SAFETY_MOTOR_PARAMETERS_NUM-2));

            crc_sf = (uint16)(((mci_class_b_prm_sf_ptr[1])<<8) + (uint8)(mci_class_b_prm_sf_ptr[0]));

            if(crc_calc == crc_sf) //calculated crc is equal to the one from setting file
            {
                // store RAM pointer value to be passed to MCI set params
                Mci_Sr_SF_Displs[displ_address++] = (addr_t)mci_class_b_prm_sf_ptr;

                //update RAM pointer
                mci_class_b_prm_sf_ptr += Mci_ClassB_Prm_Flash_Ptr.Length;
            }
            else
            {
                // NULL pointer in case of loading exception
                Mci_Sr_SF_Displs[displ_address++] = (addr_t)NULL;
            }
        }
        else
        {
            // NULL pointer in case of loading exception
            Mci_Sr_SF_Displs[displ_address++] = (addr_t)NULL;
        }
    }
    #else
    for (i = 0; i<NUM_CLASSB_SF_RAM_DISPL; i++)
    {
        // loading from setting file in Flash
        if(Params__BasicLoader(SF_PTR_MCU_CLASS_B_MCI, Displ_Sr_Idxs_Ram[i], &Mci_ClassB_Prm_Flash_Ptr) == PASS)
        {
            crc_calc = Crc16__RangeProcess(&Mci_ClassB_Prm_Flash_Ptr.Data[2],0xFFFF,(SAFETY_MOTOR_PARAMETERS_NUM-2));

            crc_sf = (uint16)(((Mci_ClassB_Prm_Flash_Ptr.Data[1])<<8) + (Mci_ClassB_Prm_Flash_Ptr.Data[0]));

            if(crc_calc == crc_sf) //calculated crc is equal to the one from setting file
            {
                Mci_Sr_SF_Displs[displ_address++] = (addr_t)Mci_ClassB_Prm_Flash_Ptr.Data;
            }
            else
            {
                // NULL pointer in case of loading exception
                Mci_Sr_SF_Displs[displ_address++] = (addr_t)NULL;
            }
        }
        else
        {
            // NULL pointer in case of loading exception
            Mci_Sr_SF_Displs[displ_address++] = (addr_t)NULL;
        }
    }
    #endif
#endif
#ifdef SF_CLASSB_FLASH_DISPLS
    for (i = 0; i<NUM_CLASSB_SF_FLASH_DISPL; i++)
    {
        // loading from setting file in Flash
        if((SettingFile__BasicLoader(SF_PTR_MCU_CLASS_B_MCI, Displ_Sr_Idxs_Flash[i], &Mci_ClassB_Prm_Flash_Ptr) == PASS) && (!mci_sf_error))
        {
            crc_calc = Crc16__RangeProcess(Mci_ClassB_Prm_Flash_Ptr.Data[2],0xFFFF,(SAFETY_MOTOR_PARAMETERS_NUM-2));

            crc_sf = (uint16)(((*Mci_ClassB_Prm_Flash_Ptr.Data[1])<<8) + (*Mci_ClassB_Prm_Flash_Ptr.Data[0]));

            if(crc_calc == crc_sf) //calculated crc is equal to the one from setting file
            {
                Mci_Sr_SF_Displs[displ_address++] = (addr_t)Mci_ClassB_Prm_Flash_Ptr.Data;
            }
            else
            {
                // NULL pointer in case of loading exception
                Mci_Sr_SF_Displs[displ_address++] = (addr_t)NULL;
            }
        }
        else
        {
            // NULL pointer in case of loading exception
            Mci_Sr_SF_Displs[displ_address++] = (addr_t)NULL;
        }
    }
#endif
#ifdef INTERNAL_CLASSB_DISPLS
    for (i = 0; i<NUM_CLASSB_INTERNAL_DISPL; i++)
    {
        Mci_Sr_SF_Displs[displ_address++] = (addr_t)mci_class_b_prm_internal_ptr;

        mci_class_b_prm_internal_ptr += Displ_Sr_Sizes_Internal[i];
    }
#endif

    // complete the process
    SRMotorSafetyMgr__SetParams((void *)*Mci_Sr_SF_Displs);
}




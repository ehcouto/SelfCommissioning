/**
 *  @file       MclFvt.c
 *  @brief      MacroBlock Functional Verification Tests
 *
 *  @details    It perfoms all required FVT algorithms. Like current and DC bus voltage sensors,
 *              over current circuit and surge relay
 *
    @startuml{MCL_FVT_Class_Diagram.png}

    package "MclFvt"  {
        Gpio :
        SurgeRelay :
        ClrkPark :
        MotorSafetyMgr :
        C_Extensions :
        Mci_API:
        MacroBlocks:
    }
    @enduml
 *
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2017.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MclFvt.h"
#include "MclConfig.h"
#include "ClrkPark.h"
#include "MotorSafetyMgr.h"


#if defined (SURGE_RELAY_USED) && defined (SURGE_RELAY_FVT)
    #include "SurgeRelay.h"
#endif

// Include Gpio only if OC polling is enabled.
#if (defined MCI_HW_OC_DEBOUNCE_TIME) && (defined MCI_HW_OC_PORT) && (defined MCI_HW_OC_PIN) && (defined MCI_HW_OC_LEVEL)
    #include "Gpio.h"
#endif



//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! Define states of the FVT
typedef enum
{
    MCL_FVT_TEST_NOT_DEFINED = 0,                   //!< Not defined, it shall not be used
    MCL_FVT_TEST_INJECT_DC_CURRENT,                 //!< Apply controlled dc current to check current and dc bus voltage sensors
#ifdef OC_FVT
    MCL_FVT_TEST_OVERCURRENT_CIRCUIT,               //!< Apply high voltage to trigger the over current sensor
#endif
#if defined (SURGE_RELAY_USED) && defined (SURGE_RELAY_FVT)
    MCL_FVT_TEST_SURGE_RELAY,                       //!< Apply current and open and close surge relay.
#endif
    MCL_FVT_TEST_WAITING,                           //!< Reenable surge relay and wait until it is in normal operation.
    MCL_FVT_TEST_FINISHED,                          //!< After finishing all steps, stays here until feedback is read
} MCL_FVT_TEST_SEQUENCE_TYPE;


//! Status of FVT
typedef enum
{
    MCLFVT_STATUS_ACTIVE = 0,                              //!< FVT is engaged and running; false when all routines is performed

} MCLFVT_STATUS_TYPE;




//! Data used to capture the FVT results during dc current injection
typedef struct
{
    float Averaged_Phase_Current_A;
    float Averaged_Phase_Current_B;
    float Averaged_Phase_Current_C;
    float Averaged_Phase_Voltage_A;
    float Averaged_Phase_Voltage_B;
    float Averaged_Phase_Voltage_C;
    float Averaged_DC_Bus_Voltage;

    sint32 Sample_Counter;
    sint32 Required_Nr_Samples;

} MCL_FVT_TEST_DATA_SENSORS_DC_CURR_AVERAGE_TYPE;


//! Data used to capture the FVT results during surge relay test
typedef struct
{
    float Averaged_DC_Bus_Voltage_Relay_Open;
    float Averaged_DC_Bus_Voltage_Relay_Closed;

    sint32 Sample_Counter;
    sint32 Required_Nr_Samples;

} MCL_FVT_TEST_DATA_SURGE_RELAY_AVERAGE_TYPE;


uint8 MclFvt_Step;                                  //!< Define steps; each state has it own meaning
MCL_FVT_TEST_SEQUENCE_TYPE MclFvt_State;            //!< [MCL_FVT_TEST_SEQUENCE_TYPE] define the state the FVT is performing

//! [MCL_FVT_TEST_DATA_SENSORS_DC_CURR_AVERAGE_TYPE] Data acquired inject DC current - sensor check algorithm
MCL_FVT_TEST_DATA_SENSORS_DC_CURR_AVERAGE_TYPE MclFvt_Data_Test_Dc_Current;

#if defined (SURGE_RELAY_USED) && defined (SURGE_RELAY_FVT)
    //! [MCL_FVT_TEST_DATA_SURGE_RELAY_AVERAGE_TYPE] data acquired during the surge relay tests
    MCL_FVT_TEST_DATA_SURGE_RELAY_AVERAGE_TYPE MclFvt_Data_Test_Surge_Relay;
#endif
#ifdef OC_FVT
uint8 MclFvt_OC_Detected;                           //!< [Boolea] Over current was detected while running FVT
#endif

MCLFVT_STATUS_TYPE MclFvt_Status;                   //!< Status of fvt tests
float32 MclFvt_Injection_Inst_Level;                 //!< [A] - define the level used in the manual injection
float32 MclFvt_Injection_Level;                      //!< [A] - define the peak level for manual injection according to the user request
float32 MclFvt_Injection_Level_Rate;                 //!< [A] - Rate change of the level in level/s
float32 MclFvt_Injection_Position;                   //!< [rad] - position to apply current
uint16 MclFvt_Generic_Timer;                        //!< [s /pwm rate] - generic timer used to calculate some delays
MCI_FVT_ERROR_TYPE MclFvt_Error_List;               //!< Saves all information during FVT execution

//! The externalized variables are explicit in order to access the variables
extern MCL_INPUT_PROC_JOINT_PARAMS_TYPE MclInput_Params;
extern MCL_INPUT_PROC_IO_F_TYPE Mcl_Input_Proc_IO;    //!< Input process variables
extern MCL_DQ_CTRL_IO_F_TYPE Mcl_DQ_Ctrl_IO;          //!< DQ Controller variables
extern MCL_PWM_IO_F_TYPE Mcl_Pwm_IO;                  //!< Mcl Pwm variables
extern MCLPWM_JOINT_PARAMS_TYPE MclPwm_Params;      //!< Mcl Pwm joint parameters


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static void DC_Current_Injection_Control(void);
static uint8 Polls_Hw_OC(void);


uint8 fvt_debug_averaging;



//---------------------------------------------------------------------------------------------------------------------
/**
 * Entry point to initialize all FVT variables before running tests.
 *
 */
void MclFvt__Entry(void)
{
    BIT_SET(MclFvt_Status, MCLFVT_STATUS_ACTIVE);

    MclFvt_Error_List = (MCI_FVT_ERROR_TYPE)0;

    Mcl_IO.All_Devices_Off = FALSE;
    Mcl_IO.Lowers_On = FALSE;

    MclFvt_State = MCL_FVT_TEST_INJECT_DC_CURRENT;

    MclFvt_Data_Test_Dc_Current.Averaged_DC_Bus_Voltage = 0.0f;
    MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_A = 0.0f;
    MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_B = 0.0f;
    MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_C = 0.0f;
    MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_A = 0.0f;
    MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_B = 0.0f;
    MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_C = 0.0f;
    MclFvt_Data_Test_Dc_Current.Sample_Counter = 0;
    MclFvt_Data_Test_Dc_Current.Required_Nr_Samples = 8000;

    MclFvt_Generic_Timer = 8000; // empirically set based on measurements

    MclFvt_Injection_Level      = MCLFVT_INJECTION_LEVEL;
    MclFvt_Injection_Level_Rate = MCLFVT_INJECTION_LEVEL_RATE;
    MclFvt_Injection_Position   = MCLFVT_INJECTION_POSITION;

    MclFvt_Injection_Inst_Level = 0.0f;

    MclFvt_Step = 0;

    fvt_debug_averaging = 0;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Exit point to reset all FVT variables before exiting FVT.
 *
 */
void MclFvt__Exit(void)
{
#if defined (SURGE_RELAY_USED) && defined (SURGE_RELAY_FVT)

    SurgeRelay__Action(SURGERELAY_NORMAL);

#endif

    Mcl_DQ_Ctrl_IO.Sin_Cos = &Mcl_Quantities.Sin_Cos_Position_Flux;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Pwm handler of FVT. All required tests are performed in this handler. The FVT state machine is
 * performed within this handler.

     @startuml{MclFvt_State_Machine.png}

        title MclFvt - Macroblock Functional Verification Tests - FVT state machine

        [*] --> MCL_FVT_TEST_INJECT_DC_CURRENT

        MCL_FVT_TEST_INJECT_DC_CURRENT : Apply controlled dc current to check current and dc bus voltage sensors
        MCL_FVT_TEST_INJECT_DC_CURRENT --> MCL_FVT_TEST_SURGE_RELAY   : [No error found] AND\n[Time is over]

        MCL_FVT_TEST_SURGE_RELAY : Apply current and open and close surge relay.
        MCL_FVT_TEST_SURGE_RELAY --> MCL_FVT_TEST_OVERCURRENT_CIRCUIT : [No error found] AND\n[Time is over]

        MCL_FVT_TEST_OVERCURRENT_CIRCUIT : Reenable surge relay and wait until it is in normal operation.
        MCL_FVT_TEST_OVERCURRENT_CIRCUIT --> MCL_FVT_TEST_WAITING   : [[No error found] AND [OC detected]] OR\n[Time is over]

        MCL_FVT_TEST_WAITING : Waiting for Class B to be finish its checks,\ne.g. perform plausibility check.
        MCL_FVT_TEST_WAITING --> MCL_FVT_TEST_FINISHED               : [No error found] AND\n[Time is over]

        MCL_FVT_TEST_FINISHED : After finishing all steps, stays here until feedback is read

    @enduml
 *
 */
void MclFvt__PwmHandler(void)
{
    uint8 oc_found;
    float temp_f;

    oc_found = Polls_Hw_OC();

    if(MclFvt_State == MCL_FVT_TEST_INJECT_DC_CURRENT)
    {
        DC_Current_Injection_Control();

        // Ramp up current - transient
        if(MclFvt_Step == 0)
        {
            if(MclFvt_Injection_Inst_Level >= MclFvt_Injection_Level)
            {
                if(MclFvt_Generic_Timer)
                {
                    MclFvt_Generic_Timer--;
                }
                else
                {
                    MclFvt_Step = 1;
                }
            }
        }

        // Steady state current
        else if(MclFvt_Step == 1)
        {
            fvt_debug_averaging = TRUE;

            // Accumulate the reading
            MclFvt_Data_Test_Dc_Current.Averaged_DC_Bus_Voltage  += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_BUS_VOLTAGE_S16));
            MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_A += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_CURR_PHASE_A_S16));
            MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_B += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_CURR_PHASE_B_S16));
            MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_C += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_CURR_PHASE_C_S16));
            MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_A += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_VOLTAGE_PHASE_A_S16));
            MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_B += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_VOLTAGE_PHASE_B_S16));
            MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_C += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_VOLTAGE_PHASE_C_S16));
            MclFvt_Data_Test_Dc_Current.Sample_Counter++;

            if(MclFvt_Data_Test_Dc_Current.Sample_Counter >= MclFvt_Data_Test_Dc_Current.Required_Nr_Samples)
            {
                temp_f = 1.0f / (float)MclFvt_Data_Test_Dc_Current.Sample_Counter; // Do only one conversion from int to float

                // Average all quantities
                MclFvt_Data_Test_Dc_Current.Averaged_DC_Bus_Voltage *= temp_f;

                MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_A *= temp_f;
                MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_B *= temp_f;
                MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_C *= temp_f;

                MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_A *= temp_f;
                MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_B *= temp_f;
                MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_C *= temp_f;

#if defined (SURGE_RELAY_USED) && defined (SURGE_RELAY_FVT)

                MclFvt_Step = 0;
                MclFvt_Generic_Timer = 20000;
                MclFvt_State = MCL_FVT_TEST_SURGE_RELAY;

#else
#ifdef OC_FVT
                MclFvt_Generic_Timer = 2000;
                MclFvt_Step = 0;

                MclFvt_State = MCL_FVT_TEST_OVERCURRENT_CIRCUIT;
#else

                MclFvt_State = MCL_FVT_TEST_WAITING;
#endif

#endif
                fvt_debug_averaging = FALSE;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
#if defined (SURGE_RELAY_USED) && defined (SURGE_RELAY_FVT)
    else if(MclFvt_State == MCL_FVT_TEST_SURGE_RELAY)
    {
        if(MclFvt_Step == 0)
        {
            SurgeRelay__Action(SURGERELAY_OPEN);

            // Delay some time to wait the voltage stabilizes at lower value
            if(MclFvt_Generic_Timer)
            {
                MclFvt_Generic_Timer--;
            }
            else
            {
                MclFvt_Data_Test_Surge_Relay.Averaged_DC_Bus_Voltage_Relay_Open = 0.0f;
                MclFvt_Data_Test_Surge_Relay.Averaged_DC_Bus_Voltage_Relay_Closed = 0.0f;
                MclFvt_Data_Test_Surge_Relay.Sample_Counter = 0;
                MclFvt_Data_Test_Surge_Relay.Required_Nr_Samples = 8000;

                MclFvt_Step = 1;
            }
        }
        else if(MclFvt_Step == 1)
        {
            fvt_debug_averaging = TRUE;

            MclFvt_Data_Test_Surge_Relay.Averaged_DC_Bus_Voltage_Relay_Open  += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_BUS_VOLTAGE_S16));
            MclFvt_Data_Test_Surge_Relay.Sample_Counter++;

            if(MclFvt_Data_Test_Surge_Relay.Sample_Counter >= MclFvt_Data_Test_Surge_Relay.Required_Nr_Samples)
            {
                temp_f = 1.0f / (float)MclFvt_Data_Test_Surge_Relay.Sample_Counter; // Do only one conversion from int to float

                // Average all quantities
                MclFvt_Data_Test_Surge_Relay.Averaged_DC_Bus_Voltage_Relay_Open *= temp_f;

                MclFvt_Generic_Timer = 12000;
                MclFvt_Data_Test_Surge_Relay.Sample_Counter = 0;
                SurgeRelay__Action(SURGERELAY_CLOSE);
                MclFvt_Step = 2;

                fvt_debug_averaging = FALSE;
            }
        }
        else if(MclFvt_Step == 2)
        {
            // Delay some time to wait the voltage stabilizes at higher valuevalue
            if(MclFvt_Generic_Timer)
            {
                MclFvt_Generic_Timer--;
            }
            else
            {
                MclFvt_Step = 3;
            }
        }
        else if(MclFvt_Step == 3)
        {
            fvt_debug_averaging = TRUE;

            // Average voltage at higher voltage
            MclFvt_Data_Test_Surge_Relay.Averaged_DC_Bus_Voltage_Relay_Closed  += ((1.0f/65536.0f)*Mci__GetAnalog(MOTOR0, MCI_AI_BUS_VOLTAGE_S16));
            MclFvt_Data_Test_Surge_Relay.Sample_Counter++;

            if(MclFvt_Data_Test_Surge_Relay.Sample_Counter >= MclFvt_Data_Test_Surge_Relay.Required_Nr_Samples)
            {
                temp_f = 1.0f / (float)MclFvt_Data_Test_Surge_Relay.Sample_Counter; // Do only one conversion from int to float

                // Average all quantities
                MclFvt_Data_Test_Surge_Relay.Averaged_DC_Bus_Voltage_Relay_Closed *= temp_f;

                MclFvt_Generic_Timer = 2000;
                MclFvt_Step = 0;
                SurgeRelay__Action(SURGERELAY_CLOSE);
#ifdef OC_FVT
                MclFvt_State = MCL_FVT_TEST_OVERCURRENT_CIRCUIT;
#else
                MclFvt_State = MCL_FVT_TEST_WAITING;
#endif

                fvt_debug_averaging = FALSE;
            }
        }

        DC_Current_Injection_Control();
    }
#endif

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
#ifdef OC_FVT
    else if(MclFvt_State == MCL_FVT_TEST_OVERCURRENT_CIRCUIT)
    {
        if(MclFvt_Step == 0)
        {
            // Just delay a little bit with relay closed
            if(MclFvt_Generic_Timer)
            {
                MclFvt_Generic_Timer--;
            }
            else
            {
                MclFvt_Generic_Timer = 8000;

                MclFvt_Step = 1;
            }
        }
        else if(MclFvt_Step == 1)
        {
            // apply high voltage until over current is detected.
            // Pwm Handler
            Mcl_Pwm_IO.Vs_Alpha_Beta->Alpha = MCLFVT_OC_VOLTAGE_LEVEL;
            Mcl_Pwm_IO.Vs_Alpha_Beta->Beta = 0.0f;

            MclPwm__RunningHandler(&Mcl_Pwm_IO, &MclPwm_Params);

            // Wait some time looking for hw over current event
            if(MclFvt_Generic_Timer)
            {
                MclFvt_Generic_Timer--;
            }

            // If hardware OC detected or timeouted
            if( (oc_found == TRUE)   ||
                (MclFvt_Generic_Timer == 0) )
            {
                MclFvt_Step = 2;

                // Disable output
                Mcl_Pwm_IO.Vs_Alpha_Beta->Alpha = 0.0f;
                Mcl_Pwm_IO.Vs_Alpha_Beta->Beta = 0.0f;

                // If hw worked within the time, the event is detected!
                if(oc_found == TRUE)
                {
                    MclFvt_OC_Detected = TRUE;
                }
                else
                {
                    // otherwise, the time is expired.
                    MclFvt_OC_Detected = FALSE;
                }

                if(!MclFvt_Generic_Timer)
                {
                    BIT_SET(MclFvt_Error_List, MCI_FVT_ERROR_OC_TIMEOUT);
                }

                MclFvt_State = MCL_FVT_TEST_WAITING;
                MclPwm__RunningHandler(&Mcl_Pwm_IO, &MclPwm_Params);
            }
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    else if( MclFvt_State == MCL_FVT_TEST_WAITING)
    {
#if defined (SURGE_RELAY_USED) && defined (SURGE_RELAY_FVT)

        SurgeRelay__Action(SURGERELAY_NORMAL);

        if(SurgeRelay__IsBusReady())
        {
            // Wait until the system is recovered. Prevent false failure reporting
            MclFvt_State = MCL_FVT_TEST_FINISHED;
            BIT_CLR(MclFvt_Error_List, MCI_FVT_ERROR_DATA_OLD);
        }

#else

        // Wait until the system is recovered. Prevent false failure reporting
        MclFvt_State = MCL_FVT_TEST_FINISHED;
        BIT_CLR(MclFvt_Error_List, MCI_FVT_ERROR_DATA_OLD);

#endif
    }

    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    else if(MclFvt_State == MCL_FVT_TEST_FINISHED)
    {
        // do nothing, just stay here until the status is read...
        BIT_CLR(MclFvt_Status, MCLFVT_STATUS_ACTIVE);
    }


    // If OC happened while not testing OC
    if(oc_found == TRUE)
    {

#ifdef OC_FVT
        if(MclFvt_State < MCL_FVT_TEST_OVERCURRENT_CIRCUIT)
        {
            Mcl_IO.All_Devices_Off = TRUE;

            MclFvt_State = MCL_FVT_TEST_WAITING;

            BIT_SET(MclFvt_Error_List, MCI_FVT_ERROR_WRONG_OC);
        }

#else
        Mcl_IO.All_Devices_Off = TRUE;

        MclFvt_State = MCL_FVT_TEST_WAITING;

        BIT_SET(MclFvt_Error_List, MCI_FVT_ERROR_WRONG_OC);

#endif

    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Get if the guard condition to exit FVT is met.
 * @return TRUE: condition is met; FALSE: condition not met yet. FVT may be still running.
 *
 */
uint8 MclFvt__GuardCondition(void)
{
    return(!BIT_TEST(MclFvt_Status, MCLFVT_STATUS_ACTIVE));
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Get status if FVT test sequence is finished.
 * @return TRUE: sequence finished; FALSE: test are still running
 *
 */
uint8 MclFvt__GetStatus(MCI_FVT_STATUS_TYPE this_status)
{
    uint8 ret_val;

    ret_val = FALSE;

    if(this_status == MCI_FVT_STATUS_FINISHED)
    {
        ret_val = (MclFvt_State == MCL_FVT_TEST_FINISHED);
    }
    else if(this_status == MCI_FVT_STATUS_STARTED)
    {
        ret_val = TRUE;
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the FVT tests. The results are updated only after finishing the tests
 * @param MCLFVT_TEST_RESULT_TYPE *result: pointer to the result. The caller contains the structure instance
 *
 */
void MclFvt__GetResult(MCI_FVT_TEST_RESULT_TYPE *fvt_result)
{
    if(MclFvt_State == MCL_FVT_TEST_FINISHED)
    {
        fvt_result->Sensors_Test_Avg_Curr_A = (sint16)(1024.0f * MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_A);
        fvt_result->Sensors_Test_Avg_Curr_B = (sint16)(1024.0f * MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_B);
        fvt_result->Sensors_Test_Avg_Curr_C = (sint16)(1024.0f * MclFvt_Data_Test_Dc_Current.Averaged_Phase_Current_C);

        fvt_result->Sensors_Test_Avg_DC_Bus_Volt = (sint16)(64.0f * MclFvt_Data_Test_Dc_Current.Averaged_DC_Bus_Voltage);

        fvt_result->Sensors_Test_Avg_Phase_Volt_A = (sint16)(64.0f * MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_A);
        fvt_result->Sensors_Test_Avg_Phase_Volt_B = (sint16)(64.0f * MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_B);
        fvt_result->Sensors_Test_Avg_Phase_Volt_C = (sint16)(64.0f * MclFvt_Data_Test_Dc_Current.Averaged_Phase_Voltage_C);

#if defined (SURGE_RELAY_USED) && defined (SURGE_RELAY_FVT)

        fvt_result->Surge_Test_Avg_DC_Bus_Volt_Open = (sint16)(64.0f * MclFvt_Data_Test_Surge_Relay.Averaged_DC_Bus_Voltage_Relay_Open);
        fvt_result->Surge_Test_Avg_DC_Bus_Volt_Close = (sint16)(64.0f * MclFvt_Data_Test_Surge_Relay.Averaged_DC_Bus_Voltage_Relay_Closed);

#else

        fvt_result->Surge_Test_Avg_DC_Bus_Volt_Open = 0;
        fvt_result->Surge_Test_Avg_DC_Bus_Volt_Close = 0;

#endif
#ifdef OC_FVT
        fvt_result->OC_Test_Found = MclFvt_OC_Detected;
#else
        fvt_result->OC_Test_Found = 0;
#endif

        BIT_CLR(MclFvt_Error_List, MCI_FVT_ERROR_RESULT_NO_FVT_TEST);

        fvt_result->Error_List = MclFvt_Error_List;

        BIT_SET(MclFvt_Error_List, MCI_FVT_ERROR_DATA_OLD);
    }
    else
    {
        fvt_result->Sensors_Test_Avg_Curr_A = 0;
        fvt_result->Sensors_Test_Avg_Curr_B = 0;
        fvt_result->Sensors_Test_Avg_Curr_C = 0;

        fvt_result->Sensors_Test_Avg_DC_Bus_Volt = 0;

        fvt_result->Sensors_Test_Avg_Phase_Volt_A = 0;
        fvt_result->Sensors_Test_Avg_Phase_Volt_B = 0;
        fvt_result->Sensors_Test_Avg_Phase_Volt_C = 0;

        fvt_result->Surge_Test_Avg_DC_Bus_Volt_Open = 0;
        fvt_result->Surge_Test_Avg_DC_Bus_Volt_Close = 0;

        fvt_result->OC_Test_Found = FALSE;

        BIT_SET(MclFvt_Error_List, MCI_FVT_ERROR_RESULT_NO_FVT_TEST);

        fvt_result->Error_List = MclFvt_Error_List;

        BIT_SET(MclFvt_Error_List, MCI_FVT_ERROR_DATA_OLD);
    }
}






//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Performs current control
 */
void DC_Current_Injection_Control(void)
{
    DQ_COOR_SYST_F_TYPE manual_dq;

    // Input Processing Handler
    MclInputProc__RunningHandler(&Mcl_Input_Proc_IO,&MclInput_Params);

    // Zero the forced voltage, this allows the controller to proper
    // generate the voltage references
    Mcl_DQ_Ctrl_IO.Force_Out_Volt->D = 0.0f;
    Mcl_DQ_Ctrl_IO.Force_Out_Volt->Q = 0.0f;

    // Handle the voltage target increase
    if(MclFvt_Injection_Inst_Level < MclFvt_Injection_Level)
    {
        MclFvt_Injection_Inst_Level += MclFvt_Injection_Level_Rate;
    }
    else if(MclFvt_Injection_Inst_Level > MclFvt_Injection_Level)
    {
        MclFvt_Injection_Inst_Level -= MclFvt_Injection_Level_Rate;

        if(MclFvt_Injection_Inst_Level <= 0)
        {
            MclFvt_Injection_Inst_Level = 0.0f;
        }
    }
    else
    {
        MclFvt_Injection_Inst_Level = MclFvt_Injection_Level;
    }


    MathCalc__SinCosF(MclFvt_Injection_Position, Mcl_DQ_Ctrl_IO.Sin_Cos);

    manual_dq.ZeroSequence = 0.0f;

    ClrkPark__DirectParkF(Mcl_Input_Proc_IO.Is_Alpha_Beta, &manual_dq, Mcl_DQ_Ctrl_IO.Sin_Cos);

    *Mcl_DQ_Ctrl_IO.D_Ref = 0.0f;
    *Mcl_DQ_Ctrl_IO.D_Est = manual_dq.D;

    *Mcl_DQ_Ctrl_IO.Q_Ref = MclFvt_Injection_Inst_Level;
    *Mcl_DQ_Ctrl_IO.Q_Est = manual_dq.Q;

    MclDqCtrl__OverrideRegGains(&Mcl_Params_Displ1->Manual_Injection);
    // Dq controllers Handler
    MclDqCtrl__RunningHandler(&Mcl_DQ_Ctrl_IO, &Mcl_Params_Displ1->Manual_Injection);

    // Pwm Handler
    MclPwm__RunningHandler(&Mcl_Pwm_IO, &MclPwm_Params);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Check if signal of Over Current (OC) hardware is triggered
 */
uint8 Polls_Hw_OC(void)
{
#if (defined MCI_HW_OC_DEBOUNCE_TIME) && (defined MCI_HW_OC_PORT) && (defined MCI_HW_OC_PIN) && (defined MCI_HW_OC_LEVEL)
    static uint8 hw_oc_debouncer = (uint8)(MCI_HW_OC_DEBOUNCE_TIME/0.005f);
    uint8 hw_pin_in;
#endif

    uint8 temp_u8;

    //////////////////////////////////////////////////////////////////////////////////////////
    // DC Bus (over) current monitor
    temp_u8 = MotorSafetyMgr__GetFault(SR_FAULT_OVER_CURRENT);

#if (defined MCI_HW_OC_DEBOUNCE_TIME) && (defined MCI_HW_OC_PORT) && (defined MCI_HW_OC_PIN) && (defined MCI_HW_OC_LEVEL)

    // Refresh input config
    Gpio__PinConfig(MCI_HW_OC_PORT, MCI_HW_OC_PIN, INPUT_FLOAT);

    // Read input oc pin
    hw_pin_in = Gpio__PinRead(MCI_HW_OC_PORT, MCI_HW_OC_PIN);

    // Debounce oc pin, if the failure is found
    if(hw_pin_in == MCI_HW_OC_LEVEL)
    {
        // count up the time
        if(hw_oc_debouncer < (uint8)(MCI_HW_OC_DEBOUNCE_TIME/0.005f))
        {
            hw_oc_debouncer++;
        }
        else
        {
            temp_u8 = TRUE;
        }
    }
    else
    {
        // count down the time
        if(hw_oc_debouncer)
        {
            hw_oc_debouncer--;
        }
    }

#endif

    return(temp_u8);
}

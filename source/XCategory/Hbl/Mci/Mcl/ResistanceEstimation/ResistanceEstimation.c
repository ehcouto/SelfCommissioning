/**
 *  @file       ResistanceDetection.c
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2016.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "ResistanceEstimation.h"
#include "MclConfig.h"
#include "MathCalc.h"
#include "Mci.h"
#include "SpeedRefGen.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef RPM_TO_RADS
    #define RPM_TO_RADS     0.10471975511965977461542144610932f
#endif


typedef enum
{
    RES_EST_MEASURE_RESISTANCE_REQ = 0,                     //!< 0 - Request to measure resistance on next start
    RES_EST_MEASURE_ERROR          = 1,                     //!< 1 - Error occurs during resistanec estimation procedure
} RES_EST_FLAGS_TYPE;

/*
typedef enum
{
    RES_DETECT_INIT = 0,                            //!< 0
    RES_DETECT_RESISTANCE_ESTIMATED,                //!< 1
    RES_DETECT_WAIT_FOR_IDLE,                       //!< 2
    RES_DETECT_WAIT_PARK_TIME,                      //!< 3
    RES_DETECT_1ST_PLATEAU_WAIT,                    //!< 4
    RES_DETECT_SECOND_RAMP,                         //!< 5
    RES_DETECT_2ND_PLATEAU_WAIT,                    //!< 6
    RES_DETECT_THIRD_RAMP,                          //!< 7
    RES_DETECT_3RD_PLATEAU_WAIT,                    //!< 8
    RES_DETECT_CALCULATE_RESISTANCE                 //!< 9
} RESISTANCE_ESTIMATION_TYPE;
*/

// NEW
typedef enum
{
    RES_DETECT_INIT = 0,                            //!< 0
    RES_DETECT_WAIT_PARK_TIME,                      //!< 1
    RES_DETECT_1ST_PLATEAU_WAIT,                    //!< 2
    RES_DETECT_SECOND_RAMP,                         //!< 3
    RES_DETECT_2ND_PLATEAU_WAIT,                    //!< 4
    RES_DETECT_THIRD_RAMP,                          //!< 5
    RES_DETECT_3RD_PLATEAU_WAIT,                    //!< 6
    RES_DETECT_RESISTANCE_ESTIMATED,                //!< 7
} RESISTANCE_ESTIMATION_TYPE;

typedef enum
{
        RE_RATE_PATTERN_INITIALIZING = 0,
        RE_RATE_PATTERN_WAITING_FIRST_START,
        RE_RATE_PATTERN_AFTER_RESET,
        RE_RATE_PATTERN_NORMAL,
        RE_RATE_PATTERN_AFTER_SPIN,
} RESISTANCE_ESTIMATION_FREQUENCY_TYPE;


static RESISTANCE_ESTIMATION_FREQUENCY_TYPE Res_Measure_Rate_State;
static RESISTANCE_ESTIMATION_TYPE Res_Est_State;
static uint16 Res_Est_Flags;
static float32 Estimated_Resistance;
static float32 Last_Estimated_Resistance;
static uint16 Speed_Oscillation_Debouncer;
static uint32 Timer_To_Reset_Resistance;
static float32 Voltage_Level;
static uint16 Res_Frequency_Timer;
static uint8 Res_Frequency_Counter;
static uint8 Res_Frequency_Counter_Spin;

static uint16 This_Timer;
static uint16 This_Timeout;
#if (FAST_RESISTANCE_CALC == ENABLED)
static float32 Current_1st_Averaged;
static float32 Voltage_1st_Averaged;
static float32 Current_2nd_Averaged;
static float32 Voltage_2nd_Averaged;
#endif
static uint8 Time_Prescaler;
static sint16 Sample_Counter;
static float32 Voltage_1st_Sum;
static float32 Current_1st_Sum;
static float32 Voltage_2nd_Sum;
static float32 Current_2nd_Sum;
static float32 Delta_Current_Nominal;


#define RE_SET_FLOW(re_flag)                BIT_SET(Res_Est_Flags, (re_flag))
#define RE_CLR_FLOW(re_flag)                BIT_CLR(Res_Est_Flags, (re_flag))
#define RE_TEST_FLOW(re_flag)               BIT_TEST(Res_Est_Flags, (re_flag))


//=====================================================================================================================
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
//=====================================================================================================================
static void CalculateResistance(void);
static void TimerDefinition(RES_EST_PARAM_TYPE *param);
static void OscillationCheck(RES_EST_IO_TYPE *io, RES_EST_PARAM_TYPE *param);
static uint8 OscillationCheckPassed(RES_EST_PARAM_TYPE *param);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


/**
 *
    @startuml{Resistance Estimation State Machine.png}

        title Resistance Estimation - State Machine

        [*] --> 0_RES_DETECT_INIT

        0_RES_DETECT_INIT : Waits until Resistance Estimation Start request is issued
        0_RES_DETECT_INIT : Do / Keep voltage level to 0
        0_RES_DETECT_INIT --> 1_RES_DETECT_WAIT_PARK_TIME : [Resistance Estimation Start request]


        1_RES_DETECT_WAIT_PARK_TIME : Pump Voltage to generate the first current level
        1_RES_DETECT_WAIT_PARK_TIME : Entry / Set the timer for first step and the timeout (RES_DETECT_WAIT_FOR_IDLE)
        1_RES_DETECT_WAIT_PARK_TIME : Do / Increase the voltage level till reaching the first current level
        1_RES_DETECT_WAIT_PARK_TIME : Exit /
        1_RES_DETECT_WAIT_PARK_TIME --> 2_RES_DETECT_1ST_PLATEAU_WAIT   : [1st step current level reached and time expired]
        1_RES_DETECT_WAIT_PARK_TIME --> MCI_FAILED : [timeout expired - current level not reached]


        2_RES_DETECT_1ST_PLATEAU_WAIT : Start measuring (1st current step)
        2_RES_DETECT_1ST_PLATEAU_WAIT : Entry / Reset the sample counter and voltage and current accumulators
        2_RES_DETECT_1ST_PLATEAU_WAIT : Do / Start accumulating voltage and current samples after oscillation check
        2_RES_DETECT_1ST_PLATEAU_WAIT : Exit /
        2_RES_DETECT_1ST_PLATEAU_WAIT --> 3_RES_DETECT_SECOND_RAMP : [minimum number of samples reached]
        2_RES_DETECT_1ST_PLATEAU_WAIT --> 7_RES_DETECT_RESISTANCE_ESTIMATED : [timeout expired - **__Resistance value not updated__**]

        3_RES_DETECT_SECOND_RAMP : increase\decrease voltage level to reach the current second step
        3_RES_DETECT_SECOND_RAMP : Entry / Calculate the current and voltage average on first step and set the timer for second step
        3_RES_DETECT_SECOND_RAMP : Do / Increase\decrease the voltage level till reaching the second current level
        3_RES_DETECT_SECOND_RAMP : Exit /
        3_RES_DETECT_SECOND_RAMP --> 4_RES_DETECT_2ND_PLATEAU_WAIT   : [time expired and checked 2nd step current level reached]
        3_RES_DETECT_SECOND_RAMP --> 7_RES_DETECT_RESISTANCE_ESTIMATED : [timeout expired - **__Resistance value not updated__**]


        4_RES_DETECT_2ND_PLATEAU_WAIT : Start measuring (2nd current step)
        4_RES_DETECT_2ND_PLATEAU_WAIT : Entry / Reset the sample counter and voltage and current accumulators
        4_RES_DETECT_2ND_PLATEAU_WAIT : Do / Start accumulating voltage and current samples after oscillation check
        4_RES_DETECT_2ND_PLATEAU_WAIT : Exit /
        4_RES_DETECT_2ND_PLATEAU_WAIT --> 5_RES_DETECT_THIRD_RAMP : [minimum number of samples reached]
        4_RES_DETECT_2ND_PLATEAU_WAIT --> 7_RES_DETECT_RESISTANCE_ESTIMATED : [timeout expired - **__Resistance value not updated__**]


        5_RES_DETECT_THIRD_RAMP : **__Update resistance value on entry__** and decrease voltage level to reach the final current value
        5_RES_DETECT_THIRD_RAMP : Entry / calculate the resistance value estimated by the procedure
        5_RES_DETECT_THIRD_RAMP : Do / Decrease the voltage level till reaching the final current level
        5_RES_DETECT_THIRD_RAMP : Exit /
        5_RES_DETECT_THIRD_RAMP --> 6_RES_DETECT_3RD_PLATEAU_WAIT   : [final current level reached]
        5_RES_DETECT_THIRD_RAMP --> 7_RES_DETECT_RESISTANCE_ESTIMATED : [timeout expired]


        6_RES_DETECT_3RD_PLATEAU_WAIT : wait the time for 3rd current step
        6_RES_DETECT_3RD_PLATEAU_WAIT : Entry / Set the timer value for the third step
        6_RES_DETECT_3RD_PLATEAU_WAIT : Do /
        6_RES_DETECT_3RD_PLATEAU_WAIT : Exit /
        6_RES_DETECT_3RD_PLATEAU_WAIT --> 7_RES_DETECT_RESISTANCE_ESTIMATED : [timer expired or timeout expired]


        MCI_FAILED : resistance estimation error (probably due to motor not connected)





    @enduml
 */







//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ResistanceDetection and its variables
 *
 */
void ResistanceEstimation__Initialize(RES_EST_PARAM_TYPE *param)
{
#if (defined (COMPILE_4_SIMULINK)) && (defined (SKIP_RESISTANCE_ESTIMATION))
    Res_Est_State = RES_DETECT_RESISTANCE_ESTIMATED;
    Estimated_Resistance = param->nominal_resistance;
    Last_Estimated_Resistance = Estimated_Resistance;
    RE_CLR_FLOW(RES_EST_MEASURE_RESISTANCE_REQ);
    Res_Measure_Rate_State = RE_RATE_PATTERN_NORMAL;
    Res_Frequency_Timer = param->time_estimate_resistance_normal;
#else
    Res_Est_State = RES_DETECT_INIT;
    Last_Estimated_Resistance = 0.0f;
    Estimated_Resistance = param->nominal_resistance;
    RE_SET_FLOW(RES_EST_MEASURE_RESISTANCE_REQ);
    Res_Measure_Rate_State = RE_RATE_PATTERN_INITIALIZING;
    Time_Prescaler = param->timer_prescaler;
    Sample_Counter = 0;
    Delta_Current_Nominal = MATHCALC__FABS(param->current_level_1st - param->current_level_2nd) * 0.5f * param->min_number_of_samples;  //50% tolerance
    RE_CLR_FLOW(RES_EST_MEASURE_ERROR);
#endif
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Enter brief description of function.
 *
 *  @param
 *  @return
 */
void ResistanceEstimation__Start(RES_EST_PARAM_TYPE *param)
{
    Voltage_Level = 0.0f;
    //Res_Est_State = RES_DETECT_WAIT_FOR_IDLE;
    Res_Est_State = RES_DETECT_WAIT_PARK_TIME;
    This_Timer = param->time_1st_step;
    This_Timeout = param->timeout_threshold;


    if(Res_Measure_Rate_State == RE_RATE_PATTERN_WAITING_FIRST_START)
    {
        Res_Measure_Rate_State = RE_RATE_PATTERN_AFTER_RESET;
        Res_Frequency_Counter++;
        Res_Frequency_Timer = param->time_estimate_resistance_after_reset;
    }
}





BOOL_TYPE ResistanceEstimation__Requested(void)
{
    BOOL_TYPE ret_val;

    if(RE_TEST_FLOW(RES_EST_MEASURE_RESISTANCE_REQ))
    {
        ret_val = TRUE;
    }
    else
    {
        ret_val = FALSE;
    }

    return ret_val;
}





BOOL_TYPE ResistanceEstimation__Calculate(RES_EST_IO_TYPE *io, RES_EST_PARAM_TYPE *param)
{
    BOOL_TYPE calculation_finished;
    BOOL_TYPE current_reached;


    calculation_finished = FALSE;
    current_reached = FALSE;

    if(Time_Prescaler)
    {
        Time_Prescaler--;
    }
    else
    {
        Time_Prescaler = param->timer_prescaler;


        if(This_Timer)
        {
            This_Timer--;
        }


        OscillationCheck(io, param);

        switch (Res_Est_State)
        {
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            case RES_DETECT_INIT:

                Voltage_Level = 0.0f;

                break;




            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            case RES_DETECT_WAIT_PARK_TIME:

                if((*io->Ctrl_Specific->Is_Abs) < (param->current_level_1st))
                {
                    Voltage_Level += (param->delta_voltage_to_1st_step);
                }
                else
                {
                    if(!This_Timer)
                    {
                        Sample_Counter = 0;
                        Voltage_1st_Sum = 0.0f;
                        Current_1st_Sum = 0.0f;

                        Res_Est_State = RES_DETECT_1ST_PLATEAU_WAIT;
                    }
                }

                break;




            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            case RES_DETECT_1ST_PLATEAU_WAIT:

                if((OscillationCheckPassed(param) == TRUE))
                {
                    Voltage_1st_Sum += *io->vs_voltage_abs;
                    Current_1st_Sum += *io->Ctrl_Specific->Is_Abs;
                    Sample_Counter++;
                }


                if(Sample_Counter >= param->min_number_of_samples)
                {
                    //! First plateau is finished. Go to seconds plateau
                    #if (FAST_RESISTANCE_CALC == ENABLED)
                    Voltage_1st_Averaged = Voltage_1st_Sum / Sample_Counter;
                    Current_1st_Averaged = Current_1st_Sum / Sample_Counter;
                    #endif
                    This_Timer = param->time_2nd_step;
                    Res_Est_State = RES_DETECT_SECOND_RAMP;

                }
                break;




            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            case RES_DETECT_SECOND_RAMP:
                if((param->current_level_2nd) >= (param->current_level_1st))
                {
                    if((*io->Ctrl_Specific->Is_Abs) < (param->current_level_2nd))
                    {
                        Voltage_Level += (param->delta_voltage_to_2nd_step);
                    }
                    else
                    {
                        current_reached = TRUE;
                    }
                }
                else // CURR_DC_LEVEL_1ST > CURR_DC_LEVEL_2ND
                {
                    if((*io->Ctrl_Specific->Is_Abs) > (param->current_level_2nd))
                    {
                        Voltage_Level -= (param->delta_voltage_to_2nd_step);
                    }
                    else
                    {
                        current_reached = TRUE;
                    }
                }

                if(current_reached && (!This_Timer))
                {
                    Sample_Counter = 0;
                    Voltage_2nd_Sum = 0.0f;
                    Current_2nd_Sum = 0.0f;

                    Res_Est_State = RES_DETECT_2ND_PLATEAU_WAIT;
                }

                break;




            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            case RES_DETECT_2ND_PLATEAU_WAIT:

                if((OscillationCheckPassed(param) == TRUE))
                {
                    Voltage_2nd_Sum += *io->vs_voltage_abs;
                    Current_2nd_Sum += *io->Ctrl_Specific->Is_Abs;
                    Sample_Counter++;

                }

                if(Sample_Counter >= param->min_number_of_samples)
                {
                    #if (FAST_RESISTANCE_CALC == ENABLED)
                    Voltage_2nd_Averaged = Voltage_2nd_Sum / Sample_Counter;
                    Current_2nd_Averaged = Current_2nd_Sum / Sample_Counter;
                    #endif
                    //! Second plateau is finished. Calculate resistance value

                    CalculateResistance();

                    // check if 0
                    if(!Estimated_Resistance)
                    {
                        Estimated_Resistance = param->nominal_resistance;
                    }

                    // Refresh the timer that indicates when the last resistance was measured
                    Timer_To_Reset_Resistance = param->timer_to_reset_resistance;
                    Last_Estimated_Resistance = Estimated_Resistance;

                    if(Res_Measure_Rate_State == RE_RATE_PATTERN_AFTER_SPIN)
                    {
                        Res_Frequency_Counter_Spin++;
                    }
                    Res_Est_State = RES_DETECT_THIRD_RAMP;
                }
                break;




            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            case RES_DETECT_THIRD_RAMP:
                if(*io->Ctrl_Specific->Is_Abs > (param->current_level_3rd))
                {
                    Voltage_Level -= (param->delta_voltage_to_3rd_step);
                }
                else
                {
                    This_Timer = param->time_3rd_step;
                    Res_Est_State = RES_DETECT_3RD_PLATEAU_WAIT;
                }
                break;




            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            case RES_DETECT_3RD_PLATEAU_WAIT:
                if(!This_Timer)
                {
                    Res_Est_State = RES_DETECT_RESISTANCE_ESTIMATED;

                }
                break;




            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            case RES_DETECT_RESISTANCE_ESTIMATED:

                // when finished stay here.
                RE_CLR_FLOW(RES_EST_MEASURE_RESISTANCE_REQ);

                calculation_finished = TRUE;
                break;


            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////
            default:
                // Left blank intentionally.
                break;
        }


        //Time out for resistance estimation.
        if(This_Timeout)
        {
            This_Timeout--;
        }
        else
        {
            Voltage_Level = 0.0f;

            //If time expires a limit, assumes the last resistance measured.
            //If it is not available, it assumes the nominal resistance value
            Estimated_Resistance = Last_Estimated_Resistance;
            if(!Estimated_Resistance)
            {
                Estimated_Resistance = param->nominal_resistance;
            }

            if(Res_Est_State == RES_DETECT_WAIT_PARK_TIME)
            {
                // resistance estimation error (probably due to motor not connected)
                io->Ctrl_Specific->flags.bit.resistance_est_error = 1;
            }
            Res_Est_State = RES_DETECT_RESISTANCE_ESTIMATED;
			RE_SET_FLOW(RES_EST_MEASURE_ERROR);
        }
    }

    *io->level_injection_out = Voltage_Level;

    return calculation_finished;
}





float32 ResistanceEstimation__GetResistance(void)
{
    return Estimated_Resistance;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Enter brief description of function.
 *
 *  @param
 *  @return
 */
void ResistanceEstimation__Handler25ms(RES_EST_PARAM_TYPE *param)
{
    if(Timer_To_Reset_Resistance)
    {
        Timer_To_Reset_Resistance--;
    }
    else
    {
#if RESET_RESISTANCE_MANAGEMENT == ENABLED
        // Force to nominal value when reset timer expires and resistance measurement is not successful
        // (if not defined, the last value is always used till a new good measurement)
        Last_Estimated_Resistance = 0;
#endif
    }

    TimerDefinition(param);
}



/**
 *  @brief      Tells if resistance has been measured recently based on a timer (RES_TIMER_40S_BASE_25MS).
 *
 *  @param      none
 *  @return     TRUE (yes) or FALSE (no)
 */
BOOL_TYPE ResistanceEstimation__IsResBeenUpdated(void)
{
    BOOL_TYPE response;

    if(Timer_To_Reset_Resistance)
    {
        response = TRUE;
    }
    else
    {
        response = FALSE;
    }

    return(response);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


void CalculateResistance(void)
{
#if(FAST_RESISTANCE_CALC == DISABLED)
    // check proper delta current
    float32 delta_current;
    delta_current = MATHCALC__FABS(Current_2nd_Sum - Current_1st_Sum);
    // update estimated value if delta current is correct
    if (delta_current >= Delta_Current_Nominal)
    {
        Estimated_Resistance = (Voltage_2nd_Sum - Voltage_1st_Sum)/delta_current;
        Estimated_Resistance = MATHCALC__FABS(Estimated_Resistance);
        RE_CLR_FLOW(RES_EST_MEASURE_ERROR);
    }
    else
    {
        Estimated_Resistance = Last_Estimated_Resistance;
        RE_SET_FLOW(RES_EST_MEASURE_ERROR);
    }

#else
        Estimated_Resistance = ((Voltage_1st_Averaged/Current_1st_Averaged) + (Voltage_2nd_Averaged/Current_2nd_Averaged));
        Estimated_Resistance = (Estimated_Resistance/2.0f);
        Estimated_Resistance = MATHCALC__FABS(Estimated_Resistance);
        Estimated_Resistance = (Estimated_Resistance - R_COMP_FACTOR);
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Enter brief description of function.
 *
 *  @param
 *  @return
 */
void OscillationCheck(RES_EST_IO_TYPE *io, RES_EST_PARAM_TYPE *param)
{
    float32 speed_thr_rads;
    speed_thr_rads = param->oscillation_speed_threshold;
    //The oscillation threshold is considered as 0.005% of the full speed range
    if((*io->Ctrl_Specific->Speed_Rotor_Observed_Mech_Abs) < speed_thr_rads)
    {
        Speed_Oscillation_Debouncer++;
    }
    else
    {
        Speed_Oscillation_Debouncer = 0;
    }
}




uint8 OscillationCheckPassed(RES_EST_PARAM_TYPE *param)
{
    uint8 passed;

    if(Speed_Oscillation_Debouncer > param->oscillation_check_timer)
    {
        passed = TRUE;
    }
    else
    {
        passed = FALSE;
    }

    return passed;
}


void TimerDefinition(RES_EST_PARAM_TYPE *param)
{
    float32 speed_ref;
    float32 speed_thr_rads;
    speed_thr_rads = param->speed_threshold_to_estimate_resistance * RPM_TO_RADS;

    speed_ref = SpeedRefGen__GetSpeedRef();
    if(speed_ref > speed_thr_rads)
    {
        Res_Measure_Rate_State = RE_RATE_PATTERN_AFTER_SPIN;
        Res_Frequency_Counter_Spin = 0;
    }

    if(Mci__GetError(MOTOR0,MCI_ERROR_ANY_ERROR_FOUND))
    {
        RE_SET_FLOW(RES_EST_MEASURE_RESISTANCE_REQ);
    }

    switch(Res_Measure_Rate_State)
    {

        case RE_RATE_PATTERN_INITIALIZING:
            Res_Frequency_Timer        = 0;
            Res_Frequency_Counter      = 0;
            Res_Frequency_Counter_Spin = 0;

            if (Mci__GetDigital(MOTOR0, MCI_DI_IDLE))
            {
                Res_Measure_Rate_State = RE_RATE_PATTERN_WAITING_FIRST_START;
            }
            break;

        case RE_RATE_PATTERN_WAITING_FIRST_START:
                //Just waiting for first measurement...
            break;

        case RE_RATE_PATTERN_AFTER_RESET:
            if(!Res_Frequency_Timer)
            {
                Res_Frequency_Timer = param->time_estimate_resistance_after_reset;
                RE_SET_FLOW(RES_EST_MEASURE_RESISTANCE_REQ);
                Res_Frequency_Counter++;

                if(Res_Frequency_Counter > param->number_estimation_after_reset)
                {
                    Res_Frequency_Counter = 0;
                    Res_Measure_Rate_State = RE_RATE_PATTERN_NORMAL;
                    Res_Frequency_Timer = param->time_estimate_resistance_normal;
                }
            }
            else
            {
                Res_Frequency_Timer--;
            }
            break;

        case RE_RATE_PATTERN_AFTER_SPIN:
            RE_SET_FLOW(RES_EST_MEASURE_RESISTANCE_REQ);

            if(Res_Frequency_Counter_Spin >= param->number_estimation_after_spin)
            {
                Res_Frequency_Counter_Spin = 0;
                Res_Measure_Rate_State = RE_RATE_PATTERN_NORMAL;
                Res_Frequency_Timer = param->time_estimate_resistance_normal;
            }
            break;

        case RE_RATE_PATTERN_NORMAL:
            if(!Res_Frequency_Timer)
            {
                Res_Frequency_Timer = param->time_estimate_resistance_normal;
                RE_SET_FLOW(RES_EST_MEASURE_RESISTANCE_REQ);
            }
            else
            {
                Res_Frequency_Timer--;
            }
            break;

        default:
            // Go back to the beginning..
            Res_Measure_Rate_State = RE_RATE_PATTERN_INITIALIZING;
            break;
    }
}

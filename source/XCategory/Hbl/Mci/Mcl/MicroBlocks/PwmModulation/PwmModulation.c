/**
 *  @file
 *  @brief       Space Vector Modulation Routines
 *  @details     This module calculates PWM duties to drive the inverter according
 *               to the voltage desired
 *  @author      alessio.beato/luigi.fagnano  (only temporary, since it is not integrated in MKS)
 *  $Header: FOC/PwmModulation.c 1.6 2016/02/02 17:26:49CET Luigi Fagnano (FAGNAL) Exp  $
 * @copyright Copyright 2012 - $Date: 2016/02/02 17:26:49CET $. Whirlpool Corporation. All rights reserved � CONFIDENTIAL
*/
/*
 *
 *---------------------------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "PwmModulation.h"
#include "McMathCalc_macros.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef SQRT3
    #define SQRT3   1.7320508075688772935274463415059f
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------





//---------------------------------------------------------------------------------------------------------------------
 /** @brief  Space vector modulation in rectangular coordinates
  *
  *
  *  @param[in]      v_alphabeta_in    = motor compensated voltage alpha-beta components
  *  @param[out]     duty_out          = PWM duties in abc components
  *  @param[in]      duty_cycle_limit  = duty cycle limitation
  *  @return         Sector number
  *
  */
 uint8 PwmModulation__SpaceVectorModulationRectangular(mc_sint32 v_dc, ALPHA_BETA_COOR_SYST_TYPE* v_alphabeta_in, ABC_COOR_SYST_TYPE* duty_out)
 {
     static ALPHA_BETA_COOR_SYST_TYPE vs_alpha_beta_comp;
     uint8 sector = 0;
     mc_sint32 x;
     mc_sint32 y;
     mc_sint32 t1_on;
     mc_sint32 t2_on;

     // Ripple compensation
     vs_alpha_beta_comp.Alpha = MC_DIV(MC_SCALE(v_alphabeta_in->Alpha,IQ15,IQ30),v_dc, IQ30, IQ15, IQ15);
     vs_alpha_beta_comp.Beta  = MC_DIV(MC_SCALE(v_alphabeta_in->Beta ,IQ15,IQ30),v_dc, IQ30, IQ15, IQ15);

     // X and Y calculation
     x = MC_DIV_SH(vs_alpha_beta_comp.Beta, 1);
     y = MC_MULT(vs_alpha_beta_comp.Alpha, MC_SQRT3_OVER_TWO, IQ15, IQ15, IQ15);

     // Sector identification
     if (x >= 0)
     {
         if ((x + y) >= 0)
         {
             // sector I - II
             sector = 1;
             t1_on = x + y;
             t2_on = x * 2;
         }
         else
         {
             // sector III - IV
             sector = 2;
             t1_on = x - y;
             t2_on = - y - x;
         }
     }
     else
     {
         if (x < y)
         {
             // sector V - VI
             sector = 3;
             t1_on = y - x;
             t2_on = -x * 2;
         }
         else
         {
             // sector III - IV
             sector = 2;
             t1_on = x - y;
             t2_on = - y - x;
         }
     }

     // Over-modulation
     if (t1_on > t2_on)
     {
         if (t1_on > OVERMODULATION_THR)
         {
              t2_on = MC_SCALE(t2_on, IQ15, IQ30) / (t1_on);
              t1_on = OVERMODULATION_THR;
         }
#if (PWM_DUTY_LIMITATION == ENABLED)
         if (t2_on > DUTY_LIMIT_MID)
         {
           t2_on = DUTY_LIMIT_MID;
         }
        #if (PWM_DUTY_BOOTSTRAP_LIMITATION == ENABLED)
         if (t1_on > DUTY_LIMIT_MID)
         {
           t1_on = DUTY_LIMIT_MID;
         }
        #endif
#endif
     }
     else
     {
         if (t2_on > OVERMODULATION_THR)
         {
              t1_on = MC_SCALE(t1_on, IQ15, IQ30) / (t2_on);
              t2_on = OVERMODULATION_THR;
         }
#if (PWM_DUTY_LIMITATION == ENABLED)
         if (t1_on > DUTY_LIMIT_MID)
         {
           t1_on = DUTY_LIMIT_MID;
         }
         #if (PWM_DUTY_BOOTSTRAP_LIMITATION == ENABLED)
         if (t2_on > DUTY_LIMIT_MID)
         {
           t2_on = DUTY_LIMIT_MID;
         }
        #endif
#endif
     }

     switch(sector)
     {
         case 1:
         {
             // Sectors I - II
             duty_out->A = t1_on;
             duty_out->B = t2_on;
             duty_out->C = 0;
             break;
         }
         case 2:
         {
             // Sectors III - IV
             duty_out->A = 0;
             duty_out->B = t1_on;
             duty_out->C = t2_on;
             break;
         }
         case 3:
         {
             // Sectors V - VI
             duty_out->A = t1_on;
             duty_out->B = 0;
             duty_out->C = t2_on;
             break;
         }
     }

     return sector;
 }


/**
 *  @brief      Space vector modulation (improved version, including not saturated ripple compensation and limitations).
 *  @details
 *
 * This function calculates the space vector modulation of the alpha/beta input
 * into the a,b,c output. The calculation is the following:
 *
 *
 * Ripple compensation function equations.
 *
 *  Implemented equations:
 *   \f[
 *      \begin{aligned}
 *         & V_{\alpha-comp}' = \frac{V_{\alpha}'}{V_{dc}'} = \frac{V_{\alpha}}{V_{dc}}\cdot\sqrt{3} \\
 *         & V_{\beta-comp}' = \frac{V_{\beta}'}{V_{dc}'} = \frac{V_{\beta}}{V_{dc}}\cdot\sqrt{3} \\
 *      \end{aligned}
 *    \f]
 *
 * where
 *
 * \f$ V_{dc}' \f$: DC bus voltage in Q17.15 notation
 *
 * \f$ V_{dc} \f$: DC bus voltage [physical value]
 *
 * \f$ V_{\alpha}' \f$: motor voltage reference on Alpha-axis in Q17.15 notation
 *
 * \f$ V_{\beta}' \f$: motor voltage reference on Beta-axis in Q17.15 notation
 *
 * \f$ V_{\alpha} \f$: motor voltage reference on Alpha-axis [physical value]
 *
 * \f$ V_{\beta} \f$: motor voltage reference on Beta-axis [physical value]
 *
 * \f$ V_{\alpha-comp}' \f$: motor voltage reference compensated on Alpha-axis in Q17.15 notation
 *
 * \f$ V_{\beta-comp}' \f$: motor voltage reference compensated on Beta-axis in Q17.15 notation
 *
 *
 * The algorithm is scaled into the fractional arithmetic
 * There are two different scaling ranges:
 *   - one for DC Bus voltage           ->    BASE_BUS_VOLTAGE
 *   - second for motor phase voltage   ->    BASE_VOLTAGE
 *
 * Relation between two scales mentioned above is the following:
 *
 * BASE_VOLTAGE = BASE_BUS_VOLTAGE / sqrt(3)
 *
 *
 *  Space Vector Modulation Algorithm
 *
 *  <b>1 - X, Y, Z calculation:</b>
 *
 *   Original requirement for X, Y, Z:
 *
 *   \f[
 *      \begin{aligned}
 *
 *          & X = \sqrt{3} \cdot \frac{V_{\beta}}{V_{dc}}\\
 *          & Y = \frac{\sqrt{3}}{2}\cdot\frac{(\sqrt{3} \cdot V_{\alpha} + V_{\beta})}{V_{dc}}\\
 *          & Z = \frac{\sqrt{3}}{2}\cdot\frac{(-\sqrt{3} \cdot V_{\alpha} + V_{\beta})}{V_{dc}}\\
 *      \end{aligned}
 *    \f]
 *
 *   The Sqrt(3) is not used in the calculation because the compensated voltages are used.
 *   So the modified equation used for X, Y, Z are:
 *
 *   \f[
 *      \begin{aligned}
 *          & X = V_{\beta-comp}\\
 *          & Y = \frac{\sqrt{3}}{2}\cdot V_{\alpha-comp} + \frac{1}{2}\cdot V_{\beta-comp}\\
 *          & Z = \frac{-\sqrt{3}}{2}\cdot V_{\alpha-comp} + \frac{1}{2}\cdot V_{\beta-comp}\\
 *      \end{aligned}
 *    \f]
 *  \n
 *
 *  <b>2 - SECTOR IDENTIFICATION:</b>
 *
 *   Sector 1: X >= 0 and Y > 0 and Z <= 0 \n
 *   Sector 2: X >= 0 and Y > 0 and Z > 0 \n
 *   Sector 3: X >= 0 and Y <= 0 and Z > 0 \n
 *   Sector 4: X < 0 and Z > 0 \n
 *   Sector 5: X < 0 and Y <= 0 and Z <= 0 \n
 *   Sector 6: X < 0 and Y > 0 and Z <= 0 \n
 *   Sector 7: X >= 0 and Y <=0 and Z <=0 => Sector 6 \n \n
 *
 *
 *  <b>3 - T1, T2 calculation:</b>
 *
 *   Sector 1: T1 = X, T2 = -Z \n
 *   Sector 2: T1 = Y, T2 = Z \n
 *   Sector 3: T1 = -Y, T2 = X \n
 *   Sector 4: T1 = Z, T2 = -X \n
 *   Sector 5: T1 = -Z, T2 = -Y \n
 *   Sector 6: T1 = -X, T2 = Y \n \n
 *
 *
 *
 *  <b>4 - T1, T2 Limitation:</b>
 *
 *   \f[
 *      \begin{aligned}
 *         &if \quad T1 + T2 >= 1  \quad then  \quad T1=\frac{T1}{(T1 + T2)}  \quad and \quad T2=\frac{T2}{(T1 + T2)}
 *      \end{aligned}
 *    \f]
 *
 *  <b>5 - Duty cycles T0_on, T1_on and T2_on calculation:</b>
 *
 *   Original equations are:
 *
 *   \f[
 *      \begin{aligned}
 *         &T0_{on} =\frac{1-T1-T2}{2}=\frac{1}{2} -\frac{T1+T2}{2}\\
 *         &T1_{on} =T0_{on}+T1 = \frac{1}{2} +\frac{T1-T2}{2}\\
 *         &T2_{on} =T1_{on}+T2 = \frac{1}{2} +\frac{T1+T2}{2}\\
 *      \end{aligned}
 *    \f]
 *
 *   As As mentioned in (1) the X, Y, Z equations are not multiplied by Sqrt(3) therefore
 *   the equations must be modified like this:
 *
 *   \f[
 *      \begin{aligned}
 *         &T0_{on} =\frac{1}{2} -\frac{\sqrt{3}}{2}\cdot(T1 + T2)\\
 *         &T1_{on} =\frac{1}{2} +\frac{\sqrt{3}}{2}\cdot(T1 - T2)\\
 *         &T2_{on} =\frac{1}{2} +\frac{\sqrt{3}}{2}\cdot(T1 + T2)\\
 *      \end{aligned}
 *    \f]
 *
 *   \n
 *
 *  <b>6 - Saturation:</b>
 *
 *   To avoid extremly small duty cycle (thanks to current measurement) it is necessary
 *   to assure the T1_on and T2_on values to be greater than the minimum value.
 *
 *   if T1_on < MIN_DUTY_CYCLE then T1_on = MIN_DUTY_CYCLE \n
 *   if T2_on < MIN_DUTY_CYCLE then T2_on = MIN_DUTY_CYCLE \n
 *
 *   \n
 *
 *  <b>7 - a, b, c assignment:</b>
 *
 *   Sector 1: PhaseA = T2_on, PhaseB = T1_on, PhaseC = T0_on \n
 *   Sector 2: PhaseA = T1_on, PhaseB = T2_on, PhaseC = T0_on \n
 *   Sector 3: PhaseA = T0_on, PhaseB = T2_on, PhaseC = T1_on \n
 *   Sector 4: PhaseA = T0_on, PhaseB = T1_on, PhaseC = T2_on \n
 *   Sector 5: PhaseA = T1_on, PhaseB = T0_on, PhaseC = T2_on \n
 *   Sector 6: PhaseA = T2_on, PhaseB = T0_on, PhaseC = T1_on \n
 *
 *   \n
 *
 *
 *  @param[in]      v_dc              = DC bus voltage in Q17.15 notation
 *  @param[in]      v_alphabeta_in    = motor compensated voltage alpha-beta components
 *  @param[out]     duty_out          = PWM duties in abc components
 *  @param[in]      duty_cycle_limit  = duty cycle limitation
 *  @return         sector_3rd_current = current to be reconstructed
 */
 uint8 PwmModulation__SpaceVectorModulation_v2(mc_sint32 v_dc, ALPHA_BETA_COOR_SYST_TYPE* v_alphabeta_in, ABC_COOR_SYST_TYPE* duty_out)
{
    uint8   sector = 0;
    ALPHA_BETA_COOR_SYST_TYPE     Vs_Alpha_Beta_Comp;

    mc_sint32 x;
    mc_sint32 y;
    mc_sint32 z;
    mc_sint32 t1;
    mc_sint32 t2;
    mc_sint32 tsum;
    mc_sint32 t0_on;
    mc_sint32 t1_on;
    mc_sint32 t2_on;

    /* Ripple compensation */
    Vs_Alpha_Beta_Comp.Alpha = MC_DIV(MC_SCALE(v_alphabeta_in->Alpha,IQ15,IQ30),v_dc, IQ30, IQ15, IQ15);
    Vs_Alpha_Beta_Comp.Beta  = MC_DIV(MC_SCALE(v_alphabeta_in->Beta ,IQ15,IQ30),v_dc, IQ30, IQ15, IQ15);


    /* X, Y, Z calculation */
    /*
        X =  beta
        Y = (alpha * sqrt(3) / 2 + beta / 2)
        Z = (-alpha * sqrt(3) / 2 + beta / 2)
    */
    x = Vs_Alpha_Beta_Comp.Beta;

    //y = IQMATH__IQSCALE((Vs_Alpha_Beta_Comp->Alpha * MC_SQRT3_OVER_TWO  + IQMATH__SHR((IQMATH__IQSCALE_UP(Vs_Alpha_Beta_Comp->Beta,IQ15)),1)),IQ15);
    y = Vs_Alpha_Beta_Comp.Alpha * MC_SQRT3_OVER_TWO;
    y = y + MC_DIV_SH(MC_SCALE(Vs_Alpha_Beta_Comp.Beta,IQ15,IQ30),1);
    y = MC_SCALE(y,IQ30,IQ15);

    //z = IQMATH__IQSCALE((-Vs_Alpha_Beta_Comp->Alpha * MC_SQRT3_OVER_TWO + IQMATH__SHR((IQMATH__IQSCALE_UP(Vs_Alpha_Beta_Comp->Beta, IQ15)),1)),IQ15);
    z = -Vs_Alpha_Beta_Comp.Alpha * MC_SQRT3_OVER_TWO;
    z = z + MC_DIV_SH(MC_SCALE(Vs_Alpha_Beta_Comp.Beta,IQ15,IQ30),1);
    z = MC_SCALE(z,IQ30,IQ15);

    // sector identification
    if (x>=0)
    {
        // sector 1,2,3,7
        if (y>0)
        {
            // sector 1,2
            if (z<=0)
            {
                // sector 1
                sector = 1;
                t1 = x;
                t2 = -z;
            }
            else
            {
                // sector 2
                sector = 2;
                t1 = y;
                t2 = z;
            }
        }
        else
        {
            // sector 3,7
            if (z>0)
            {
                // sector 3
                sector = 3;
                t1 = -y;
                t2 = x;
            }
            else
            {
                // sector 7 --> 6
                sector = 6;
                t1 = -x;
                t2 = y;
            }
        }
    }
    else
    {
        // sector 4,5,6
        if (z>0)
        {
           // sector 4
           sector = 4;
           t1 = z;
           t2 = -x;
        }
        else
        {
            if (y>0)
            {
                // sector 6
                sector = 6;
                t1 = -x;
                t2 = y;
            }
            else
            {
                // sector 5
                sector = 5;
                t1 = -z;
                t2 = -y;
            }
        }
    }


    // Over-modulation
    tsum = t1+t2;
    if (tsum>OVERMODULATION_THR)
    {
        t1 = MC_SCALE(t1,IQ15,IQ30)/(tsum);
        t2 = OVERMODULATION_THR - t1;
        // T0_on = 0.5 - 0.5*(T1+T2);
        t0_on = 0;
    }
    else
    {
        // T0_on = 0.5 - 0.5*(T1+T2);
        t0_on = MC_ONE_OVER_TWO - MC_DIV_SH(tsum,1);
    }

    /*
    T0_on = 0.5 - 0.5*(T1+T2);
    T1_on = T1 + T0_on;
    T2_on = T2 + T1_on;
    */
    t1_on = t1 + t0_on;
    t2_on = t2 + t1_on;


 /* ****** DUTY LIMITATIONS ***** */
#if (PWM_DUTY_LIMITATION == ENABLED)
    if (t1_on>DUTY_LIMIT_MID)
    {
      t1_on = DUTY_LIMIT_MID;
    }

    #if (PWM_DUTY_BOOTSTRAP_LIMITATION == ENABLED)
    if (t2_on>DUTY_LIMIT_MID)
    {
      t2_on = DUTY_LIMIT_MID;
    }

    #elif (PWM_DUTY_ANTI_GLITCH_LIMITATION == ENABLED)
    #if (PWM_DUTY_BOOTSTRAP_LIMITATION == DISABLED)
    // Higher anti-glitch limitation makes sense only if just t1_on is limited
    if (t2_on > DUTY_ANTI_GLITCH_UPPER)
    {
        t2_on = DUTY_100;
    }
    #endif

    if (t0_on < DUTY_ANTI_GLITCH_LOWER)
    {
        t0_on = 0;
    }
    #endif

#endif
    /* ****** ********* ***** */


    // duty calculation
    switch (sector)
    {
     case 1:
         duty_out->A = t2_on;
         duty_out->B = t1_on;
         duty_out->C = t0_on;
         break;

     case 2:
         duty_out->A = t1_on;
         duty_out->B = t2_on;
         duty_out->C = t0_on;
         break;

     case 3:
         duty_out->A = t0_on;
         duty_out->B = t2_on;
         duty_out->C = t1_on;
         break;

     case 4:
         duty_out->A = t0_on;
         duty_out->B = t1_on;
         duty_out->C = t2_on;
         break;

     case 5:
         duty_out->A = t1_on;
         duty_out->B = t0_on;
         duty_out->C = t2_on;
         break;

     case 6:
         duty_out->A = t2_on;
         duty_out->B = t0_on;
         duty_out->C = t1_on;
         break;

    }

    return (sector);

}


void PwmModulation__InverterLossComp_v2(ABC_COOR_SYST_TYPE *i_abc_in, ABC_COOR_SYST_TYPE *duty, MATHCALC_LUT_EXT_TYPE *inverter_loss_params)
{
    sint32 duty_cycle;
    // check for 0% or 100% duties
    if ((duty->A != 0) && (duty->A != DUTY_100))
    {
        duty_cycle = duty->A;
        duty_cycle += MathCalc__GetInterpolationFast(i_abc_in->A, inverter_loss_params);
        duty->A = MATHCALC__SATURATE_DIRECT(0, duty_cycle, DUTY_100);
    }

    if ((duty->B != 0) && (duty->B != DUTY_100))
    {
        duty_cycle = duty->B;
        duty_cycle += MathCalc__GetInterpolationFast(i_abc_in->B, inverter_loss_params);
        duty->B = MATHCALC__SATURATE_DIRECT(0, duty_cycle, DUTY_100);
    }

    if ((duty->C != 0) && (duty->C != DUTY_100))
    {
        duty_cycle = duty->C;
        duty_cycle += MathCalc__GetInterpolationFast(i_abc_in->C, inverter_loss_params);
        duty->C = MATHCALC__SATURATE_DIRECT(0, duty_cycle, DUTY_100);
    }

}



/**
 *  @brief      Deadtime compensation with linear current gradient (improved version v2).
 *  @details
 *
 *
 *  Implemented equations:
 *   \f[
 *      \begin{aligned}
 *         & V_{a-comp} = V_a+ sgn(I_a)\cdot deadtime\\
 *         & V_{b-comp} = V_b+ sgn(I_b)\cdot deadtime\\
 *         & V_{c-comp} = V_c+ sgn(I_c)\cdot deadtime\\
 *      \end{aligned}
 *    \f]
 *  A smoothing linear equation is used in order to avoid discontinuities when current change sign
 *  If the not-compensated duty is 100% or 0%, no correction is applied.
 *
 *  @param[in]      i_abc_in          = motor phase currents in abc components
 *  @param[in-out]  duty 	          = PWM duties
 */
void PwmModulation__DeadtimeCompensationGradient_v2(ABC_COOR_SYST_TYPE* i_abc_in, ABC_COOR_SYST_TYPE* duty)
{
    mc_sint32 duty_cycle;
    mc_sint32 current_sat;

    ////////////// phase A ///////////////////////////
    // check for 0% or 100% duties
    if ((duty->A != 0) && (duty->A != DUTY_100))
    {
        current_sat = MATHCALC__SATURATE_DIRECT(-DT_COMP_I_THR, i_abc_in->A, DT_COMP_I_THR);
        duty_cycle = duty->A;
        // calculation of the compensation factor
        duty_cycle += MC_SCALE((DT_COMP_M * current_sat),IQ30,IQ15);

        // assignment with 0% - 100% saturation
        duty->A = MATHCALC__SATURATE_DIRECT(0, duty_cycle, DUTY_100);
    }

    ////////////// phase B ///////////////////////////
    // check for 0% or 100% duties
    if ((duty->B != 0) && (duty->B != DUTY_100))
    {
        current_sat = MATHCALC__SATURATE_DIRECT(-DT_COMP_I_THR, i_abc_in->B, DT_COMP_I_THR);
        duty_cycle = duty->B;
        // calculation of the compensation factor
        duty_cycle += MC_SCALE((DT_COMP_M * current_sat),IQ30,IQ15);

        // assignment with 0% - 100% saturation
        duty->B = MATHCALC__SATURATE_DIRECT(0, duty_cycle, DUTY_100);
    }

    ////////////// phase C ///////////////////////////
    // check for 0% or 100% duties
    if ((duty->C != 0) && (duty->C != DUTY_100))
    {
        current_sat = MATHCALC__SATURATE_DIRECT(-DT_COMP_I_THR, i_abc_in->C, DT_COMP_I_THR);
        duty_cycle = duty->C;
        // calculation of the compensation factor
        duty_cycle += MC_SCALE((DT_COMP_M * current_sat),IQ30,IQ15);

        // assignment with 0% - 100% saturation
        duty->C = MATHCALC__SATURATE_DIRECT(0, duty_cycle, DUTY_100);
    }
}

/**
 *  @brief      Space vector modulation: Floating Point Implementation
 *  @details
 *
 * This function calculates a,b,c duty cycles that generates the desired reference voltage (alpha/beta input)
 * into the a,b,c output. The flow chart is the following:
 *
 * <b> 1 - Reference voltage normalization (DC Bus Ripple Compensation) </b>
 * <b> 2 - X, Y, Z quantities calculation </b>
 * <b> 3 - Sector Identification </b>
 * <b> 4 - Over-modulation </b>
 * <b> 5 - Duty Cycle Limitations (High-Performance approach) </b>
 * <b> 6 - a,b,c Duty Cycle assignment </b>
 *
 *
 *  @param[in]      v_dc               = DC bus voltage
 *  @param[in]      v_alphabeta_in     = motor reference voltage (alpha-beta components)
 *  @param[out]     duty_out           = PWM duty-cycles (a,b,c)
 *  @param[in]      enable_flat_bottom = boolean flag (TRUE --> implement flat bottom Space Vector Modulation forcing dlow = 0)
 *  @return         sector             = sector value
 */
uint8 PwmModulation__SpaceVectorModulationF(float32 v_dc, ALPHA_BETA_COOR_SYST_F_TYPE* v_alphabeta_in, ABC_COOR_SYST_F_TYPE* duty_out, BOOL_TYPE enable_flat_bottom)
{
    uint8   sector = 0;
    ALPHA_BETA_COOR_SYST_F_TYPE     Vs_Alpha_Beta_Comp;
    float32 x;
    float32 y;
    float32 z;
    float32 d2;
    float32 d1;
    float32 dsum;
    float32 d_low;
    float32 d_mid;
    float32 d_high;

    x = SQRT3 / v_dc; // just used as temporary variable to optimize division

    /* Ripple compensation */
    Vs_Alpha_Beta_Comp.Alpha = v_alphabeta_in->Alpha * x;
    Vs_Alpha_Beta_Comp.Beta = v_alphabeta_in->Beta * x;

    /* X, Y, Z calculation */
    x = Vs_Alpha_Beta_Comp.Beta;

    y = Vs_Alpha_Beta_Comp.Alpha * F_SQRT3_OVER_TWO;
    y = y + (Vs_Alpha_Beta_Comp.Beta / 2);

    z = -Vs_Alpha_Beta_Comp.Alpha * F_SQRT3_OVER_TWO;
    z = z + (Vs_Alpha_Beta_Comp.Beta / 2);

    // sector identification
    if (x>=0)
    {
        // sector 1,2,3,7
        if (y>0)
        {
            // sector 1,2
            if (z<=0)
            {
                // sector 1
                sector = 1;
                d1 = -z;
                d2 = x;
            }
            else
            {
                // sector 2
                sector = 2;
                d1 = z;
                d2 = y;
            }
        }
        else
        {
            // sector 3,7
            if (z>0)
            {
                // sector 3
                sector = 3;
                d1 = x;
                d2 = -y;
            }
            else
            {
                // sector 7 --> 6
                sector = 6;
                d1 = y;
                d2 = -x;

            }
        }
    }
    else
    {
        // sector 4,5,6
        if (z>0)
        {
           // sector 4
           sector = 4;
           d1 = -x;
           d2 = z;
        }
        else
        {
            if (y>0)
            {
                // sector 6
                sector = 6;
                d1 = y;
                d2 = -x;
            }
            else
            {
                // sector 5
                sector = 5;
                d1 = -y;
                d2 = -z;
            }
        }
    }



    // Duty cycles calculation:

    /*
    Ton Duration Formulas, Ts is Pwm Switching Period:
    T_low_on = T0/2 = (Ts-T1-T2)/2;
    T_mid_on = T0/2 + T2 = Tlow_on + T2;
    T_high_on = T0/2 + T1 + T2 = T_mid_on + T1;

    Duty Cycles - Times relations:
    d1 = T1/Ts
    d2 = T2/Ts

    d_low = T_low_on/Ts = (d_limit - d1 - d2)/2
    d_mid = T_mid_on/Ts = d_low + d2
    d_high = T_high_on/Ts = d_mid + d1
    */

    // Over-modulation -> it automatically limits the d_high to F_OVERMODULATION_THR
    dsum = d2 + d1;
    if (dsum > F_OVERMODULATION_THR)
    {
    	//ration between d2 and d1 shall be the same to maintain the same vector direction
    	// (d2_new/ (F_OVERMODULATION_THR - d2_new)) = (d2_old/d1_old)
    	
        d2 = (d2 * F_OVERMODULATION_THR) / dsum;
        d1 = F_OVERMODULATION_THR - d2;
        d_low = 0;
    }
    else
    {
        //d_low = (T0/2)/Ts = (F_OVERMODULATION_THR-D1-D2)/2; -> F_OVERMODULATION_THR is equal to Duty Cycle Limit
        d_low = (F_OVERMODULATION_THR - dsum) / 2.0f;

        if ((d_low < F_DUTY_ANTI_GLITCH_LOWER) || (enable_flat_bottom == TRUE))
        {
            d_low = 0;
        }
    }


    ///////////////// d_mid calculation ///////////////
    d_mid = d2 + d_low;

    ///////////////////////////////////////////////////

    ///////////////// d_high calculation ///////////////
    d_high = d1 + d_mid;

    ///////////////////////////////////////////////////

    ///////////////// limitations on d_mid and d_high ///////////////
    if (d_mid > F_DUTY_LIMIT_MID)
    {
      d_mid = F_DUTY_LIMIT_MID;
    }

    if (d_high > F_DUTY_ANTI_GLITCH_UPPER)
    {
        d_high = F_DUTY_100;
    }
    ///////////////////////////////////////////////////


    switch (sector)
    {
     case 1:
         duty_out->A = d_high;
         duty_out->B = d_mid;
         duty_out->C = d_low;
         break;

     case 2:
         duty_out->A = d_mid;
         duty_out->B = d_high;
         duty_out->C = d_low;
         break;

     case 3:
         duty_out->A = d_low;
         duty_out->B = d_high;
         duty_out->C = d_mid;
         break;

     case 4:
         duty_out->A = d_low;
         duty_out->B = d_mid;
         duty_out->C = d_high;
         break;

     case 5:
         duty_out->A = d_mid;
         duty_out->B = d_low;
         duty_out->C = d_high;
         break;

     case 6:
         duty_out->A = d_high;
         duty_out->B = d_low;
         duty_out->C = d_mid;
         break;

    }

    return (sector);

}

//---------------------------------------------------------------------------------------------------------------------
 /** @brief  Space vector modulation in rectangular coordinates - floating point
  *
  *
  *  @param[in]      v_alphabeta_in    = motor compensated voltage alpha-beta components
  *  @param[out]     duty_out          = PWM duties in abc components
  *  @param[in]      duty_cycle_limit  = duty cycle limitation
  *  @return         Sector number
  *
  */
 uint8 PwmModulation__SpaceVectorModulationRectangularF(float32 v_dc, ALPHA_BETA_COOR_SYST_F_TYPE* v_alphabeta_in, ABC_COOR_SYST_F_TYPE* duty_out)
 {
     static ALPHA_BETA_COOR_SYST_F_TYPE vs_alpha_beta_comp;
     uint8 sector = 0;
     float32 x;
     float32 y;
     float32 t1_on;
     float32 t2_on;

     x = SQRT3 / v_dc; // just used as temporary variable to optimize division

     // Ripple compensation
     vs_alpha_beta_comp.Alpha = v_alphabeta_in->Alpha * x;
     vs_alpha_beta_comp.Beta = v_alphabeta_in->Beta * x;

     // X and Y calculation
     x = vs_alpha_beta_comp.Beta / 2.0f;
     y = vs_alpha_beta_comp.Alpha * F_SQRT3_OVER_TWO;

     // Sector identification
     if (x >= 0)
     {
         if ((x + y) >= 0)
         {
             // sector I - II
             sector = 1;
             t1_on = x + y;
             t2_on = x * 2;
         }
         else
         {
             // sector III - IV
             sector = 2;
             t1_on = x - y;
             t2_on = - y - x;
         }
     }
     else
     {
         if (x < y)
         {
             // sector V - VI
             sector = 3;
             t1_on = y - x;
             t2_on = -x * 2;
         }
         else
         {
             // sector III - IV
             sector = 2;
             t1_on = x - y;
             t2_on = - y - x;
         }
     }

     // Over-modulation
     if (t1_on > t2_on)
     {
         if (t1_on > F_OVERMODULATION_THR)
         {
              t2_on = t2_on / t1_on;
              t1_on = F_OVERMODULATION_THR;
         }
#if (PWM_DUTY_LIMITATION == ENABLED)
         if (t2_on > F_DUTY_LIMIT_MID)
         {
           t2_on = F_DUTY_LIMIT_MID;
         }
        #if (PWM_DUTY_BOOTSTRAP_LIMITATION == ENABLED)
         if (t1_on > F_DUTY_LIMIT_MID)
         {
           t1_on = F_DUTY_LIMIT_MID;
         }
        #endif
#endif
     }
     else
     {
         if (t2_on > F_OVERMODULATION_THR)
         {
              t1_on = t1_on / t2_on;
              t2_on = F_OVERMODULATION_THR;
         }
#if (PWM_DUTY_LIMITATION == ENABLED)
         if (t1_on > F_DUTY_LIMIT_MID)
         {
           t1_on = F_DUTY_LIMIT_MID;
         }
         #if (PWM_DUTY_BOOTSTRAP_LIMITATION == ENABLED)
         if (t2_on > F_DUTY_LIMIT_MID)
         {
           t2_on = F_DUTY_LIMIT_MID;
         }
        #endif
#endif
     }

     switch(sector)
     {
         case 1:
         {
             // Sectors I - II
             duty_out->A = t1_on;
             duty_out->B = t2_on;
             duty_out->C = 0;
             break;
         }
         case 2:
         {
             // Sectors III - IV
             duty_out->A = 0;
             duty_out->B = t1_on;
             duty_out->C = t2_on;
             break;
         }
         case 3:
         {
             // Sectors V - VI
             duty_out->A = t1_on;
             duty_out->B = 0;
             duty_out->C = t2_on;
             break;
         }
     }

     return sector;
}


void PwmModulation__InverterLossCompF(ABC_COOR_SYST_F_TYPE *i_abc_in, ABC_COOR_SYST_F_TYPE *duty, MATHCALC_LUT_EXT_F_TYPE *inverter_loss_params)
{
    float32 duty_cycle;
    // check for 0% or 100% duties
    if ((duty->A != 0) && (duty->A != F_DUTY_100))
    {
        duty_cycle = duty->A;
        duty_cycle += MathCalc__GetInterpolationFastF(i_abc_in->A, inverter_loss_params);
        duty->A = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }

    if ((duty->B != 0) && (duty->B != F_DUTY_100))
    {
        duty_cycle = duty->B;
        duty_cycle += MathCalc__GetInterpolationFastF(i_abc_in->B, inverter_loss_params);
        duty->B = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }

    if ((duty->C != 0) && (duty->C != F_DUTY_100))
    {
        duty_cycle = duty->C;
        duty_cycle += MathCalc__GetInterpolationFastF(i_abc_in->C, inverter_loss_params);
        duty->C = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }
}


void PwmModulation__InverterLossOnStateCompF(ABC_COOR_SYST_F_TYPE *i_abc_in, ABC_COOR_SYST_F_TYPE *duty, MATHCALC_LUT_EXT_F_TYPE *inverter_loss_params, float32 v_dc)
{
    float32 duty_cycle;
    float32 k,k_2;
    float32 loss;

#ifdef F_VDC_CALIBRATION
    k = F_VDC_CALIBRATION/v_dc;
#else
    k = 1.0f;
#endif

    k_2 = F_OVERMODULATION_THR/2.0f*(1.0f-k);


    // check for 0% or 100% duties
    if ((duty->A != 0) && (duty->A != F_DUTY_100))
    {
        duty_cycle = duty->A;
        loss = MathCalc__GetInterpolationFastF(i_abc_in->A, inverter_loss_params);
        loss = k*loss + k_2;
        duty_cycle += loss;
        duty->A = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }

    if ((duty->B != 0) && (duty->B != F_DUTY_100))
    {
        duty_cycle = duty->B;
        loss = MathCalc__GetInterpolationFastF(i_abc_in->B, inverter_loss_params);
        loss = k*loss + k_2;
        duty_cycle += loss;
        duty->B = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }

    if ((duty->C != 0) && (duty->C != F_DUTY_100))
    {
        duty_cycle = duty->C;
        loss = MathCalc__GetInterpolationFastF(i_abc_in->C, inverter_loss_params);
        loss = k*loss + k_2;
        duty_cycle += loss;
        duty->C = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }
}


/**
 *  @brief      Deadtime compensation with linear current gradient - floating point
 *  @details
 *
 *
 *  Implemented equations:
 *   \f[
 *      \begin{aligned}
 *         & V_{a-comp} = V_a+ sgn(I_a)\cdot deadtime\\
 *         & V_{b-comp} = V_b+ sgn(I_b)\cdot deadtime\\
 *         & V_{c-comp} = V_c+ sgn(I_c)\cdot deadtime\\
 *      \end{aligned}
 *    \f]
 *  A smoothing linear equation is used in order to avoid discontinuities when current change sign
 *  If the not-compensated duty is 100% or 0%, no correction is applied.
 *
 *  @param[in]      i_abc_in          = motor phase currents in abc components
 *  @param[in-out]  duty              = PWM duties
 */
void PwmModulation__DeadtimeCompensationGradientF(ABC_COOR_SYST_F_TYPE* i_abc_in, ABC_COOR_SYST_F_TYPE* duty)
{
    float32 duty_cycle;
    float32 current_sat;

    ////////////// phase A ///////////////////////////
    // check for 0% or 100% duties
    if ((duty->A != 0) && (duty->A != F_DUTY_100))
    {
        current_sat = MATHCALC__SATURATE_DIRECT(-DT_COMP_I_THR_REAL, i_abc_in->A, DT_COMP_I_THR_REAL);
        duty_cycle = duty->A;
        // calculation of the compensation factor
        duty_cycle += (DT_COMP_M_FLOAT * current_sat);

        // assignment with 0% - 100% saturation
        duty->A = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }

    ////////////// phase B ///////////////////////////
    // check for 0% or 100% duties
    if ((duty->B != 0) && (duty->B != F_DUTY_100))
    {
        current_sat = MATHCALC__SATURATE_DIRECT(-DT_COMP_I_THR_REAL, i_abc_in->B, DT_COMP_I_THR_REAL);
        duty_cycle = duty->B;
        // calculation of the compensation factor
        duty_cycle += (DT_COMP_M_FLOAT * current_sat);

        // assignment with 0% - 100% saturation
        duty->B = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }

    ////////////// phase C ///////////////////////////
    // check for 0% or 100% duties
    if ((duty->C != 0) && (duty->C != F_DUTY_100))
    {
        current_sat = MATHCALC__SATURATE_DIRECT(-DT_COMP_I_THR_REAL, i_abc_in->C, DT_COMP_I_THR_REAL);
        duty_cycle = duty->C;
        // calculation of the compensation factor
        duty_cycle += (DT_COMP_M_FLOAT * current_sat);

        // assignment with 0% - 100% saturation
        duty->C = MATHCALC__SATURATE_DIRECT(0, duty_cycle, F_DUTY_100);
    }
}



uint8 PwmModulation__SpaceVectorModulationSixSteps(float32 v_dc, ALPHA_BETA_COOR_SYST_F_TYPE* v_alphabeta_in, ABC_COOR_SYST_F_TYPE* duty_out)
{
    uint8   sector = 0;
    ALPHA_BETA_COOR_SYST_F_TYPE     Vs_Alpha_Beta_Comp;
    float32 x;
    float32 y;
    float32 z;
    float32 t1;
    float32 t2;
    float32 tsum;
    float32 t0_on;
    float32 t1_on;
    float32 t2_on;
    float32 arg_tmp;
    float32 mindex;

    x = SQRT3 / v_dc; // just used as temporary variable to optimize division

    /* Ripple compensation */
    Vs_Alpha_Beta_Comp.Alpha = v_alphabeta_in->Alpha * x;
    Vs_Alpha_Beta_Comp.Beta = v_alphabeta_in->Beta * x;

    mindex = MC_SQRT_F(Vs_Alpha_Beta_Comp.Alpha* Vs_Alpha_Beta_Comp.Alpha + Vs_Alpha_Beta_Comp.Beta * Vs_Alpha_Beta_Comp.Beta);
    /* X, Y, Z calculation */
    x = Vs_Alpha_Beta_Comp.Beta;

    y = Vs_Alpha_Beta_Comp.Alpha * F_SQRT3_OVER_TWO;
    y = y + (Vs_Alpha_Beta_Comp.Beta / 2);

    z = -Vs_Alpha_Beta_Comp.Alpha * F_SQRT3_OVER_TWO;
    z = z + (Vs_Alpha_Beta_Comp.Beta / 2);

    // sector identification
    if (x>=0)
    {
        // sector 1,2,3,7
        if (y>0)
        {
            // sector 1,2
            if (z<=0)
            {
                // sector 1
                sector = 1;
                t1 = x;
                t2 = -z;
            }
            else
            {
                // sector 2
                sector = 2;
                t1 = y;
                t2 = z;
            }
        }
        else
        {
            // sector 3,7
            if (z>0)
            {
                // sector 3
                sector = 3;
                t1 = -y;
                t2 = x;
            }
            else
            {
                // sector 7 --> 6
                sector = 6;
                t1 = -x;
                t2 = y;
            }
        }
    }
    else
    {
        // sector 4,5,6
        if (z>0)
        {
           // sector 4
           sector = 4;
           t1 = z;
           t2 = -x;
        }
        else
        {
            if (y>0)
            {
                // sector 6
                sector = 6;
                t1 = -x;
                t2 = y;
            }
            else
            {
                // sector 5
                sector = 5;
                t1 = -z;
                t2 = -y;
            }
        }
    }


    // Over-modulation
    tsum = t1 + t2;
    if (tsum > F_OVERMODULATION_THR)
    {
        if (mindex < 1.1547f)
        {
           arg_tmp = asinf(1/mindex) + PI/3.0f;
           arg_tmp = mindex * MathCalc__SinF(arg_tmp);
           if (t1>t2)
           {
               t1 = arg_tmp;
               t2 = 1 - arg_tmp;
           }
           else
           {
               t2 = arg_tmp;
               t1 = 1 - arg_tmp;
           }
        }
        else
        {
            if (t1>t2)
            {
                t1 = 1.0f;
                t2 = 0;
            }
            else
            {
                t2 = 1.0f;
                t1 = 0;
            }
        }
        t0_on = 0;
    }
    else
    {
        // T0_on = 0.5 - 0.5*(T1+T2);
        t0_on = F_ONE_OVER_TWO - (tsum / 2);
    }

    /*
    T0_on = 0.5 - 0.5*(T1+T2);
    T1_on = T1 + T0_on;
    T2_on = T2 + T1_on;
    */
    t1_on = t1 + t0_on;
    t2_on = t2 + t1_on;

  /* ****** DUTY LIMITATIONS ***** */
#if (PWM_DUTY_LIMITATION == ENABLED)
    if (t1_on > F_DUTY_LIMIT_MID)
    {
      t1_on = F_DUTY_LIMIT_MID;
    }

    #if (PWM_DUTY_BOOTSTRAP_LIMITATION == ENABLED)
    if (t2_on > F_DUTY_LIMIT_MID)
    {
      t2_on = F_DUTY_LIMIT_MID;
    }

    #elif (PWM_DUTY_ANTI_GLITCH_LIMITATION == ENABLED)
    #if (PWM_DUTY_BOOTSTRAP_LIMITATION == DISABLED)
    // Higher anti-glitch limitation makes sense only if just t1_on is limited
    if (t2_on > F_DUTY_ANTI_GLITCH_UPPER)
    {
        t2_on = F_DUTY_100;
    }
    #endif

    if (t0_on < F_DUTY_ANTI_GLITCH_LOWER)
    {
        t0_on = 0;
    }
    #endif

#endif
    /* ****** ********* ***** */


    // duty calculation
    switch (sector)
    {
     case 1:
         duty_out->A = t2_on;
         duty_out->B = t1_on;
         duty_out->C = t0_on;
         break;

     case 2:
         duty_out->A = t1_on;
         duty_out->B = t2_on;
         duty_out->C = t0_on;
         break;

     case 3:
         duty_out->A = t0_on;
         duty_out->B = t2_on;
         duty_out->C = t1_on;
         break;

     case 4:
         duty_out->A = t0_on;
         duty_out->B = t1_on;
         duty_out->C = t2_on;
         break;

     case 5:
         duty_out->A = t1_on;
         duty_out->B = t0_on;
         duty_out->C = t2_on;
         break;

     case 6:
         duty_out->A = t2_on;
         duty_out->B = t0_on;
         duty_out->C = t1_on;
         break;

    }

    return (sector);

}

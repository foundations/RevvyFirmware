/*
 * BatteryCalculator.c
 *
 * Created: 03/05/2019 14:05:00
 *  Author: Dániel Buga
 */

#include "BatteryCalculator.h"
#include <math.h>
#include "utils/functions.h"

#define BATTERY_SMOOTHING_COEFF   ((float) 0.9f)
#define BATTERY_LOW_DETECT_PERCENTAGE   ((uint8_t) 10u)
#define BATTERY_LOW_RELEASE_PERCENTAGE  ((uint8_t) 15u)

static float Battery_ExponentialAverage(BatteryCalculator_Context_t* context, float input)
{
    context->filterPreviousValue = BATTERY_SMOOTHING_COEFF * context->filterPreviousValue + (1.0f - BATTERY_SMOOTHING_COEFF) * input;
    context->biasCorrection *= BATTERY_SMOOTHING_COEFF;

    return (context->filterPreviousValue / (1.0f - context->biasCorrection));
}

void BatteryCalculator_Run_OnInit(BatteryCalculator_Context_t* context)
{
    context->filterPreviousValue = 0.0f;
    context->biasCorrection = 1.0f;
}

void BatteryCalculator_Run_Update(BatteryCalculator_Context_t* context)
{
    float mv = BatteryCalculator_Read_Voltage(context);
    float filtered = Battery_ExponentialAverage(context, mv);

    if (filtered > context->detectionVoltage)
    {
        float p = map_constrained(filtered, context->minVoltage, context->maxVoltage, 0, 100);
        
        BatteryCalculator_Write_BatteryPresent(context, true);
        BatteryCalculator_Write_Percentage(context, lroundf(p));

        if (context->batteryLow)
        {
            if (p > BATTERY_LOW_RELEASE_PERCENTAGE)
            {
                context->batteryLow = false;
            }
        }
        else
        {
            if (p < BATTERY_LOW_DETECT_PERCENTAGE)
            {
                context->batteryLow = true;
            }
        }

        BatteryCalculator_Write_LowBatteryCondition(context, context->batteryLow);
    }
    else
    {
        BatteryCalculator_Write_BatteryPresent(context, false);
        BatteryCalculator_Write_Percentage(context, 0u);
        
        BatteryCalculator_Write_LowBatteryCondition(context, true);
    }
}

__attribute__((weak))
float BatteryCalculator_Read_Voltage(BatteryCalculator_Context_t* context)
{
    (void) context;
    return 0.0f;
}

__attribute__((weak))
void BatteryCalculator_Write_Percentage(BatteryCalculator_Context_t* context, uint8_t percent)
{
    (void) context;
    (void) percent;
    /* nothing to do */
}

__attribute__((weak))
void BatteryCalculator_Write_BatteryPresent(BatteryCalculator_Context_t* context, bool present)
{
    (void) context;
    (void) present;
    /* nothing to do */
}

__attribute__((weak))
void BatteryCalculator_Write_LowBatteryCondition(BatteryCalculator_Context_t* context, bool low)
{
    (void) context;
    (void) low;
    /* nothing to do */
}

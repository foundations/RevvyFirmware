/*
 * BatteryIndicator.c
 *
 * Created: 03/05/2019 14:35:28
 *  Author: Dániel Buga
 */
#include "BatteryIndicator.h"
#include "rrrc_indication.h"

#include <math.h>
#include "utils/functions.h"

#define BLINK_PERIOD    25
#define BLINK_LENGTH     1

#define CHARGING_COLOR             (rgb_t) LED_CYAN
#define BATTERY_NOT_DETECTED_COLOR (rgb_t) LED_OFF

static void write_color_based_on_percentage(BatteryIndicator_Context_t* context, uint8_t percentage)
{
    rgb_t color = { 0u };

    color.G = lroundf(map(percentage, 0, 100, 0, LED_BRIGHT));
    color.R = LED_BRIGHT - color.G;

    BatteryIndicator_Write_LedColor(context, color);
}

void BatteryIndicator_Run_OnInit(BatteryIndicator_Context_t* context)
{
    context->blinkTimer = 0u;
}

void BatteryIndicator_Run_Update(BatteryIndicator_Context_t* context)
{
    uint8_t percentage = BatteryIndicator_Read_Percentage(context);
    switch (BatteryIndicator_Read_Status(context))
    {
        case BatteryStatus_Present:
            context->blinkTimer = 0u;
            write_color_based_on_percentage(context, percentage);
            break;

        case BatteryStatus_Charging:
            if (context->blinkTimer < BLINK_LENGTH)
            {
                context->blinkTimer++;
                BatteryIndicator_Write_LedColor(context, CHARGING_COLOR);
            }
            else if (context->blinkTimer < BLINK_PERIOD)
            {
                context->blinkTimer++;
                write_color_based_on_percentage(context, percentage);
            }
            else
            {
                context->blinkTimer = 0u;
                write_color_based_on_percentage(context, percentage);
            }
            break;

        default:
        case BatteryStatus_NotPresent:
            context->blinkTimer = 0u;
            BatteryIndicator_Write_LedColor(context, BATTERY_NOT_DETECTED_COLOR);
            break;
    }
}

__attribute__((weak))
uint8_t BatteryIndicator_Read_Percentage(BatteryIndicator_Context_t* context)
{
    return 0u;
}

__attribute__((weak))
BatteryStatus_t BatteryIndicator_Read_Status(BatteryIndicator_Context_t* context)
{
    return BatteryStatus_NotPresent;
}

__attribute__((weak))
void BatteryIndicator_Write_LedColor(BatteryIndicator_Context_t* context, rgb_t color)
{
    /* nothing to do */
}

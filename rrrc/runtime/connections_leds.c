/*
 * connections_leds.c
 *
 * Created: 2019. 07. 27. 16:27:03
 *  Author: Dániel Buga
 */ 
 
#include "../rrrc_hal.h"
#include "../rrrc_worklogic.h"
#include "../rrrc_indication.h"

extern BatteryCalculator_Context_t mainBattery;
extern BatteryIndicator_Context_t mainBatteryIndicator;
extern BatteryIndicator_Context_t motorBatteryIndicator;

static bool statusLedsChanged;
static bool ringLedsChanged;
static rgb_t statusLeds[4] = { LED_OFF, LED_OFF, LED_OFF, LED_OFF };
static rgb_t ringLeds[RING_LEDS_AMOUNT] = { 0 };

static MasterStatus_t masterStatus;
static bool battery_low = true;

rgb_t LEDController_Read_StatusLED(uint32_t led_idx)
{
    if (led_idx >= ARRAY_SIZE(statusLeds))
    {
        return (rgb_t) LED_OFF;
    }
    else
    {
        if (battery_low && led_idx != 0u)
        {
            return (rgb_t) LED_OFF;
        }
        else
        {
            return statusLeds[led_idx];
        }
    }
}

rgb_t LEDController_Read_RingLED(uint32_t led_idx)
{
    if (led_idx >= ARRAY_SIZE(ringLeds) || battery_low)
    {
        return (rgb_t) LED_OFF;
    }
    else
    {
        return ringLeds[led_idx];
    }
}

#define MAIN_BATTERY_INDICATOR_LED  0
#define MOTOR_BATTERY_INDICATOR_LED 1
#define BLUETOOTH_INDICATOR_LED     2
#define STATUS_INDICATOR_LED        3

void BluetoothIndicator_Write_LedColor(rgb_t color)
{
    statusLeds[BLUETOOTH_INDICATOR_LED] = color;
    statusLedsChanged = true;
}

void BrainStatusIndicator_Write_LedColor(rgb_t color)
{
    statusLeds[STATUS_INDICATOR_LED] = color;
    statusLedsChanged = true;
}

void BatteryIndicator_Write_LedColor(BatteryIndicator_Context_t* context, rgb_t color)
{
    if (context == &mainBatteryIndicator)
    {
        if (!rgb_equals(color, statusLeds[MAIN_BATTERY_INDICATOR_LED]))
        {
            statusLeds[MAIN_BATTERY_INDICATOR_LED] = color;
            statusLedsChanged = true;
        }
    }
    else if (context == &motorBatteryIndicator)
    {
        if (!rgb_equals(color, statusLeds[MOTOR_BATTERY_INDICATOR_LED]))
        {
            statusLeds[MOTOR_BATTERY_INDICATOR_LED] = color;
            statusLedsChanged = true;
        }
    }
    else
    {
        ASSERT(0);
    }
}

bool LEDController_Read_StatusLEDs_Changed(void)
{
    bool changed = statusLedsChanged;
    statusLedsChanged = false;
    return changed;
}

bool LEDController_Read_RingLEDs_Changed(void)
{
    bool changed = ringLedsChanged;
    ringLedsChanged = false;
    return changed;
}

void RingLedDisplay_Write_LedColor(uint32_t led_idx, rgb_t color)
{
    ASSERT(led_idx < RING_LEDS_AMOUNT);
    if (!rgb_equals(ringLeds[led_idx], color))
    {
        ringLeds[led_idx] = color;
        ringLedsChanged = true;
    }
}

void MasterStatusObserver_Write_MasterStatus(MasterStatus_t status)
{
    if (masterStatus != status)
    {
        masterStatus = status;
        statusLedsChanged = true;

        /* TODO this should be moved to a separate component, probably */
        switch (status)
        {
            default:
            case MasterStatus_Unknown:
                portENTER_CRITICAL();
                statusLeds[STATUS_INDICATOR_LED] = (rgb_t) LED_RED;
                portEXIT_CRITICAL();
                break;

            case MasterStatus_Operational:
                portENTER_CRITICAL();
                statusLeds[STATUS_INDICATOR_LED] = (rgb_t) LED_ORANGE;
                portEXIT_CRITICAL();
                break;

            case MasterStatus_Controlled:
                portENTER_CRITICAL();
                statusLeds[STATUS_INDICATOR_LED] = (rgb_t) LED_GREEN;
                portEXIT_CRITICAL();
                break;
        }
    }
}

SystemState_t BrainStatusIndicator_Read_SystemState(void)
{
    SystemState_t systemState = SystemState_Error;
    switch (masterStatus)
    {
        default:
        case MasterStatus_Unknown:
            systemState = SystemState_Startup;
            break;

        case MasterStatus_Operational:
        case MasterStatus_Controlled:
            systemState = SystemState_Operational;
            break;
    }
    return systemState;
}

bool BrainStatusIndicator_Read_BluetoothControllerPresent(void)
{
    return masterStatus == MasterStatus_Controlled;
}

void BatteryCalculator_Write_LowBatteryCondition(BatteryCalculator_Context_t* context, bool low)
{
    if (context == &mainBattery)
    {
        battery_low = low;
    }
}

#include "../rrrc_hal.h"
#include "../rrrc_worklogic.h"
#include "../rrrc_indication.h"

extern BatteryIndicator_Context_t mainBatteryIndicator;
extern BatteryIndicator_Context_t motorBatteryIndicator;

#define MAIN_BATTERY_INDICATOR_LED  0
#define MOTOR_BATTERY_INDICATOR_LED 1
#define BLUETOOTH_INDICATOR_LED     2
#define STATUS_INDICATOR_LED        3

static rgb_t leds[16] = {
    LED_OFF, LED_OFF, LED_OFF, LED_OFF,
    LED_OFF, LED_OFF, LED_OFF, LED_OFF,
    LED_OFF, LED_OFF, LED_OFF, LED_OFF,
    LED_OFF, LED_OFF, LED_OFF, LED_OFF
};

static MasterStatus_t masterStatus;
static bool battery_low = true;

rgb_t LEDController_Read_Colors(uint32_t led_idx)
{
    ASSERT(led_idx < 16u);
    if (battery_low && led_idx != 0u)
    {
        return (rgb_t) LED_OFF;
    }
    else
    {
        return leds[led_idx];
    }
}

void BluetoothIndicator_Write_LedColor(rgb_t color)
{
    leds[BLUETOOTH_INDICATOR_LED] = color;
}

void BrainStatusIndicator_Write_LedColor(rgb_t color)
{
    leds[STATUS_INDICATOR_LED] = color;
}

void BatteryIndicator_Write_LedColor(BatteryIndicator_Context_t* context, rgb_t color)
{
    if (context == &mainBatteryIndicator)
    {
        leds[MAIN_BATTERY_INDICATOR_LED] = color;
    }
    else if (context == &motorBatteryIndicator)
    {
        leds[MOTOR_BATTERY_INDICATOR_LED] = color;
    }
    else
    {
        ASSERT(0);
    }
}

void RingLedDisplay_Write_LedColor(uint32_t led_idx, rgb_t color)
{
    ASSERT(led_idx < RING_LEDS_AMOUNT);
    leds[STATUS_LEDS_AMOUNT + led_idx] = color;
}

void MasterStatusObserver_Write_MasterStatus(MasterStatus_t status)
{
    if (masterStatus != status)
    {
        masterStatus = status;

        /* TODO this should be moved to a separate component, probably */
        switch (status)
        {
            default:
            case MasterStatus_Unknown:
                portENTER_CRITICAL();
                leds[STATUS_INDICATOR_LED] = (rgb_t) LED_RED;
                portEXIT_CRITICAL();
                break;

            case MasterStatus_Operational:
                portENTER_CRITICAL();
                leds[STATUS_INDICATOR_LED] = (rgb_t) LED_ORANGE;
                portEXIT_CRITICAL();
                break;

            case MasterStatus_Controlled:
                portENTER_CRITICAL();
                leds[STATUS_INDICATOR_LED] = (rgb_t) LED_GREEN;
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

void BatteryCalculator_Write_MainBatteryLow(bool low)
{
    battery_low = low;
}

uint8_t LEDController_Read_MaxBrightness(void)
{
    return battery_low ? 10u : 24u;
}

#include "LedDisplayController.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include "rrrc_indication.h"
#include "utils/functions.h"
#include <math.h>

#define BRIGHTNESS_LOW      ((uint8_t) 10u)
#define BRIGHTNESS_NORMAL   ((uint8_t) 24u)

#define MAIN_BATTERY_INDICATOR_LED  0
#define MOTOR_BATTERY_INDICATOR_LED 1
#define BLUETOOTH_INDICATOR_LED     2
#define STATUS_INDICATOR_LED        3

#define BLUETOOTH_BLINK_PERIOD    130
#define BLUETOOTH_BLINK_LENGTH     5

#define CHARGING_BLINK_PERIOD    125
#define CHARGING_BLINK_LENGTH     5

#define MOTOR_MISSING_BLINK_PERIOD    50
#define MOTOR_MISSING_BLINK_LENGTH    15

#define BLE_ON_COLOR                (rgb_t) LED_CYAN
#define BLE_OFF_COLOR               (rgb_t) LED_OFF
#define BLE_NOT_INITIALIZED_COLOR   (rgb_t) LED_OFF

#define MASTER_UNKNOWN_COLOR        (rgb_t) LED_RED
#define MASTER_OPERATIONAL_COLOR    (rgb_t) LED_ORANGE
#define MASTER_CONTROLLED_COLOR     (rgb_t) LED_GREEN

typedef enum {
    LedDisplayMode_LowBattery,
    LedDisplayMode_MotorBatteryMissing,
    LedDisplayMode_Normal,
} LedDisplayMode_t;

static uint32_t _motor_battery_feedback;
static LedDisplayMode_t _previous_display_mode;
static uint32_t _charging_blink_timer;
static uint32_t _blink_timer;

static bool _motors_active(void)
{
    for (uint32_t i = 0u; i < 6u; i++)
    {
        if (LedDisplayController_Read_MotorDriveValues(i) != 0)
        {
            return true;
        }
    }
    return false;
}

static bool _blink(uint32_t* timer, uint32_t on_time, uint32_t period)
{
    if (*timer < on_time)
    {
        (*timer)++;
        return true;
    }
    else if (*timer < period)
    {
        (*timer)++;
        return false;
    }
    else
    {
        *timer = 0u;
        return false;
    }
}

static void _clear_display(void)
{
    for (uint32_t i = 0u; i < 16u; i++)
    {
        LedDisplayController_Write_Leds(i, (rgb_t) LED_OFF);
    }
}

static LedDisplayMode_t _get_display_mode(void)
{
    if (LedDisplayController_Read_MainBatteryLow())
    {
        return LedDisplayMode_LowBattery;
    }
    else
    {
        if (_motors_active() && !LedDisplayController_Read_MotorBatteryPresent())
        {
            return LedDisplayMode_MotorBatteryMissing;
        }
        else if (_previous_display_mode == LedDisplayMode_MotorBatteryMissing)
        {
            /* force at least one period */
            if (_blink_timer != 0u)
            {
                return LedDisplayMode_MotorBatteryMissing;
            }
        }
    }

    return LedDisplayMode_Normal;
}

static rgb_t _display_main_battery(void)
{
    switch (LedDisplayController_Read_MainBatteryStatus())
    {
        case ChargerState_NotPluggedIn:
        case ChargerState_Charged:
            break;

        case ChargerState_Charging:
            if (_blink(&_charging_blink_timer, CHARGING_BLINK_LENGTH, CHARGING_BLINK_PERIOD))
            {
                return (rgb_t) LED_BLUE;
            }
            break;

        case ChargerState_Fault:
            if (_blink(&_charging_blink_timer, CHARGING_BLINK_LENGTH, CHARGING_BLINK_PERIOD))
            {
                return (rgb_t) LED_RED;
            }
            break;
    }

    rgb_t color;

    uint8_t percentage = LedDisplayController_Read_MainBatteryLevel();
    color.G = lroundf(map_constrained(percentage, 10, 90, 0, LED_BRIGHT));
    color.R = lroundf(map_constrained(percentage, 50, 90, LED_BRIGHT, 0));
    color.B = 0u;

    return color;
}

static rgb_t _display_motor_battery(void)
{
    if (!LedDisplayController_Read_MotorBatteryPresent())
    {
        return (rgb_t) LED_OFF;
    }

    rgb_t color;

    uint8_t percentage = LedDisplayController_Read_MotorBatteryLevel();
    color.G = lroundf(map_constrained(percentage, 10, 90, 0, LED_BRIGHT));
    color.R = lroundf(map_constrained(percentage, 50, 90, LED_BRIGHT, 0));
    color.B = 0u;

    return color;
}
/* End User Code Section: Declarations */

void LedDisplayController_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    _motor_battery_feedback = 0u;
    _previous_display_mode = LedDisplayMode_Normal;
    LedDisplayController_Write_MaxBrightness(BRIGHTNESS_NORMAL);
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void LedDisplayController_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    LedDisplayMode_t current_display_mode = _get_display_mode();

    if (current_display_mode != _previous_display_mode)
    {
        /* Handle leaving state */
        switch (_previous_display_mode)
        {
            default:
                break;

            case LedDisplayMode_LowBattery:
                LedDisplayController_Write_MaxBrightness(BRIGHTNESS_NORMAL);
                break;
        }

        /* Handle entering state */
        switch (current_display_mode)
        {
            default:
                break;

            case LedDisplayMode_LowBattery:
                _clear_display();
                LedDisplayController_Write_MaxBrightness(BRIGHTNESS_LOW);
                LedDisplayController_Write_Leds(MAIN_BATTERY_INDICATOR_LED, (rgb_t) LED_RED);
                break;

            case LedDisplayMode_MotorBatteryMissing:
                _clear_display();
                _blink_timer = 0u;
                break;

            case LedDisplayMode_Normal:
                _clear_display();
                _charging_blink_timer = 0u;
                _blink_timer = 0u;
                break;
        }
    }

    switch (current_display_mode)
    {
        case LedDisplayMode_Normal:
            LedDisplayController_Write_Leds(MAIN_BATTERY_INDICATOR_LED, _display_main_battery());
            LedDisplayController_Write_Leds(MOTOR_BATTERY_INDICATOR_LED, _display_motor_battery());

            switch (LedDisplayController_Read_BluetoothStatus())
            {
                case BluetoothStatus_Inactive:
                    LedDisplayController_Write_Leds(BLUETOOTH_INDICATOR_LED, BLE_NOT_INITIALIZED_COLOR);
                    _blink_timer = 0u;
                    break;

                case BluetoothStatus_NotConnected:
                    if (_blink(&_blink_timer, BLUETOOTH_BLINK_LENGTH, BLUETOOTH_BLINK_PERIOD))
                    {
                        LedDisplayController_Write_Leds(BLUETOOTH_INDICATOR_LED, BLE_ON_COLOR);
                    }
                    else
                    {
                        LedDisplayController_Write_Leds(BLUETOOTH_INDICATOR_LED, BLE_OFF_COLOR);
                    }
                    break;

                case BluetoothStatus_Connected:
                    LedDisplayController_Write_Leds(BLUETOOTH_INDICATOR_LED, BLE_ON_COLOR);
                    _blink_timer = 0u;
                    break;
            }

            switch (LedDisplayController_Read_MasterStatus())
            {
                case MasterStatus_Unknown:
                    LedDisplayController_Write_Leds(STATUS_INDICATOR_LED, MASTER_UNKNOWN_COLOR);
                    break;

                case MasterStatus_Operational:
                    LedDisplayController_Write_Leds(STATUS_INDICATOR_LED, MASTER_OPERATIONAL_COLOR);
                    break;

                case MasterStatus_Controlled:
                    LedDisplayController_Write_Leds(STATUS_INDICATOR_LED, MASTER_CONTROLLED_COLOR);
                    break;
            }

            /* apply ring led */
            for (uint32_t i = 0u; i < 12u; i++)
            {
                LedDisplayController_Write_Leds(i + 4u, LedDisplayController_Read_RingLeds(i));
            }
            break;

        case LedDisplayMode_MotorBatteryMissing:
            if (_blink(&_blink_timer, MOTOR_MISSING_BLINK_LENGTH, MOTOR_MISSING_BLINK_PERIOD))
            {
                LedDisplayController_Write_Leds(MOTOR_BATTERY_INDICATOR_LED, (rgb_t) LED_RED);
            }
            else
            {
                LedDisplayController_Write_Leds(MOTOR_BATTERY_INDICATOR_LED, (rgb_t) LED_OFF);
            }
            break;

        case LedDisplayMode_LowBattery:
            break;
    }

    _previous_display_mode = current_display_mode;
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void LedDisplayController_Write_Leds(uint32_t index, const rgb_t value)
{
    ASSERT(index < 16);
    /* Begin User Code Section: Leds Start */

    /* End User Code Section: Leds Start */
    /* Begin User Code Section: Leds End */

    /* End User Code Section: Leds End */
}

__attribute__((weak))
void LedDisplayController_Write_MaxBrightness(const uint8_t value)
{
    /* Begin User Code Section: MaxBrightness Start */

    /* End User Code Section: MaxBrightness Start */
    /* Begin User Code Section: MaxBrightness End */

    /* End User Code Section: MaxBrightness End */
}

__attribute__((weak))
BluetoothStatus_t LedDisplayController_Read_BluetoothStatus(void)
{
    /* Begin User Code Section: BluetoothStatus Start */

    /* End User Code Section: BluetoothStatus Start */
    /* Begin User Code Section: BluetoothStatus End */

    /* End User Code Section: BluetoothStatus End */
    return BluetoothStatus_Inactive;
}

__attribute__((weak))
uint8_t LedDisplayController_Read_MainBatteryLevel(void)
{
    /* Begin User Code Section: MainBatteryLevel Start */

    /* End User Code Section: MainBatteryLevel Start */
    /* Begin User Code Section: MainBatteryLevel End */

    /* End User Code Section: MainBatteryLevel End */
    return 0u;
}

__attribute__((weak))
bool LedDisplayController_Read_MainBatteryLow(void)
{
    /* Begin User Code Section: MainBatteryLow Start */

    /* End User Code Section: MainBatteryLow Start */
    /* Begin User Code Section: MainBatteryLow End */

    /* End User Code Section: MainBatteryLow End */
    return false;
}

__attribute__((weak))
ChargerState_t LedDisplayController_Read_MainBatteryStatus(void)
{
    /* Begin User Code Section: MainBatteryStatus Start */

    /* End User Code Section: MainBatteryStatus Start */
    /* Begin User Code Section: MainBatteryStatus End */

    /* End User Code Section: MainBatteryStatus End */
    return ChargerState_NotPluggedIn;
}

__attribute__((weak))
MasterStatus_t LedDisplayController_Read_MasterStatus(void)
{
    /* Begin User Code Section: MasterStatus Start */

    /* End User Code Section: MasterStatus Start */
    /* Begin User Code Section: MasterStatus End */

    /* End User Code Section: MasterStatus End */
    return MasterStatus_Unknown;
}

__attribute__((weak))
uint8_t LedDisplayController_Read_MotorBatteryLevel(void)
{
    /* Begin User Code Section: MotorBatteryLevel Start */

    /* End User Code Section: MotorBatteryLevel Start */
    /* Begin User Code Section: MotorBatteryLevel End */

    /* End User Code Section: MotorBatteryLevel End */
    return 0u;
}

__attribute__((weak))
bool LedDisplayController_Read_MotorBatteryPresent(void)
{
    /* Begin User Code Section: MotorBatteryPresent Start */

    /* End User Code Section: MotorBatteryPresent Start */
    /* Begin User Code Section: MotorBatteryPresent End */

    /* End User Code Section: MotorBatteryPresent End */
    return false;
}

__attribute__((weak))
int8_t LedDisplayController_Read_MotorDriveValues(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorDriveValues Start */

    /* End User Code Section: MotorDriveValues Start */
    /* Begin User Code Section: MotorDriveValues End */

    /* End User Code Section: MotorDriveValues End */
    return 0;
}

__attribute__((weak))
rgb_t LedDisplayController_Read_RingLeds(uint32_t index)
{
    ASSERT(index < 12);
    /* Begin User Code Section: RingLeds Start */

    /* End User Code Section: RingLeds Start */
    /* Begin User Code Section: RingLeds End */

    /* End User Code Section: RingLeds End */
    return (rgb_t){0, 0, 0};
}

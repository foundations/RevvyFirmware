#include "utils.h"
#include "BatteryCalculator.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include <math.h>
#include "utils/functions.h"

#define BATTERY_SMOOTHING_COEFF   ((float) 0.9f)
#define BATTERY_LOW_DETECT_PERCENTAGE   ((uint8_t) 10u)
#define BATTERY_LOW_RELEASE_PERCENTAGE  ((uint8_t) 15u)

static BatteryConfiguration_t mainBatteryParameters;
static BatteryConfiguration_t motorBatteryParameters;

static bool mainBatteryLow;

static float mainBatteryPrevious = 0.0f;
static float mainBatteryBiasCorrection = 1.0f;

static float motorBatteryPrevious = 0.0f;
static float motorBatteryBiasCorrection = 1.0f;

static float _exp_average(float input, float* previous, float* biasCorrection)
{
    *previous = BATTERY_SMOOTHING_COEFF * *previous + (1.0f - BATTERY_SMOOTHING_COEFF) * input;
    *biasCorrection *= BATTERY_SMOOTHING_COEFF;

    return (*previous / (1.0f - *biasCorrection));
}

static void _update_main_battery(void)
{
    float mv = BatteryCalculator_Read_MainBatteryVoltage();
    float filtered = _exp_average(mv, &mainBatteryPrevious, &mainBatteryBiasCorrection);

    if (filtered > mainBatteryParameters.detectionVoltage)
    {
        float p = map_constrained(filtered, mainBatteryParameters.minVoltage, mainBatteryParameters.maxVoltage, 0, 100);

        BatteryCalculator_Write_MainBatteryLevel(lroundf(p));

        if (mainBatteryLow)
        {
            if (p > BATTERY_LOW_RELEASE_PERCENTAGE)
            {
                mainBatteryLow = false;
                BatteryCalculator_Write_MainBatteryLow(mainBatteryLow);
            }
        }
        else
        {
            if (p < BATTERY_LOW_DETECT_PERCENTAGE)
            {
                mainBatteryLow = true;
                BatteryCalculator_Write_MainBatteryLow(mainBatteryLow);
            }
        }
    }
    else
    {
        mainBatteryLow = true;
        BatteryCalculator_Write_MainBatteryLevel(0u);
        BatteryCalculator_Write_MainBatteryLow(true);
    }
}

static void _update_motor_battery(void)
{
    float mv = BatteryCalculator_Read_MotorBatteryVoltage();
    float filtered = _exp_average(mv, &motorBatteryPrevious, &motorBatteryBiasCorrection);

    if (filtered > motorBatteryParameters.detectionVoltage)
    {
        float p = map_constrained(filtered, motorBatteryParameters.minVoltage, motorBatteryParameters.maxVoltage, 0, 100);

        BatteryCalculator_Write_MotorBatteryPresent(true);
        BatteryCalculator_Write_MotorBatteryLevel(lroundf(p));
    }
    else
    {
        BatteryCalculator_Write_MotorBatteryPresent(false);
        BatteryCalculator_Write_MotorBatteryLevel(0u);
    }
}
/* End User Code Section: Declarations */

void BatteryCalculator_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    mainBatteryPrevious = 0.0f;
    mainBatteryBiasCorrection = 1.0f;

    motorBatteryPrevious = 0.0f;
    motorBatteryBiasCorrection = 1.0f;

    BatteryCalculator_Read_MainBatteryParameters(&mainBatteryParameters);
    BatteryCalculator_Read_MotorBatteryParameters(&motorBatteryParameters);

    mainBatteryLow = true;
    BatteryCalculator_Write_MainBatteryLow(true);
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void BatteryCalculator_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    _update_main_battery();
    _update_motor_battery();
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void BatteryCalculator_Write_MainBatteryLevel(const uint8_t value)
{
    (void) value;
    /* Begin User Code Section: MainBatteryLevel Start */

    /* End User Code Section: MainBatteryLevel Start */
    /* Begin User Code Section: MainBatteryLevel End */

    /* End User Code Section: MainBatteryLevel End */
}

__attribute__((weak))
void BatteryCalculator_Write_MainBatteryLow(const bool value)
{
    (void) value;
    /* Begin User Code Section: MainBatteryLow Start */

    /* End User Code Section: MainBatteryLow Start */
    /* Begin User Code Section: MainBatteryLow End */

    /* End User Code Section: MainBatteryLow End */
}

__attribute__((weak))
void BatteryCalculator_Write_MotorBatteryLevel(const uint8_t value)
{
    (void) value;
    /* Begin User Code Section: MotorBatteryLevel Start */

    /* End User Code Section: MotorBatteryLevel Start */
    /* Begin User Code Section: MotorBatteryLevel End */

    /* End User Code Section: MotorBatteryLevel End */
}

__attribute__((weak))
void BatteryCalculator_Write_MotorBatteryPresent(const bool value)
{
    (void) value;
    /* Begin User Code Section: MotorBatteryPresent Start */

    /* End User Code Section: MotorBatteryPresent Start */
    /* Begin User Code Section: MotorBatteryPresent End */

    /* End User Code Section: MotorBatteryPresent End */
}

__attribute__((weak))
void BatteryCalculator_Read_MainBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: MainBatteryParameters Start */

    /* End User Code Section: MainBatteryParameters Start */
    *value = (BatteryConfiguration_t) { .detectionVoltage = 0.0f, .minVoltage = 0.0f, .maxVoltage = 0.0f };
    /* Begin User Code Section: MainBatteryParameters End */

    /* End User Code Section: MainBatteryParameters End */
}

__attribute__((weak))
Voltage_t BatteryCalculator_Read_MainBatteryVoltage(void)
{
    /* Begin User Code Section: MainBatteryVoltage Start */

    /* End User Code Section: MainBatteryVoltage Start */
    /* Begin User Code Section: MainBatteryVoltage End */

    /* End User Code Section: MainBatteryVoltage End */
    return 0.0f;
}

__attribute__((weak))
void BatteryCalculator_Read_MotorBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: MotorBatteryParameters Start */

    /* End User Code Section: MotorBatteryParameters Start */
    *value = (BatteryConfiguration_t) { .detectionVoltage = 0.0f, .minVoltage = 0.0f, .maxVoltage = 0.0f };
    /* Begin User Code Section: MotorBatteryParameters End */

    /* End User Code Section: MotorBatteryParameters End */
}

__attribute__((weak))
Voltage_t BatteryCalculator_Read_MotorBatteryVoltage(void)
{
    /* Begin User Code Section: MotorBatteryVoltage Start */

    /* End User Code Section: MotorBatteryVoltage Start */
    /* Begin User Code Section: MotorBatteryVoltage End */

    /* End User Code Section: MotorBatteryVoltage End */
    return 0.0f;
}

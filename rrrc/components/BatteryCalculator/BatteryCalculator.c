#include "BatteryCalculator.h"

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

void BatteryCalculator_Run_OnInit(void)
{
    mainBatteryPrevious = 0.0f;
    mainBatteryBiasCorrection = 1.0f;

    motorBatteryPrevious = 0.0f;
    motorBatteryBiasCorrection = 1.0f;

    BatteryCalculator_Read_MainBatteryParameters(&mainBatteryParameters);
    BatteryCalculator_Read_MotorBatteryParameters(&motorBatteryParameters);
    
    mainBatteryLow = true;
    BatteryCalculator_Write_MainBatteryLow(true);
}

void BatteryCalculator_Run_Update(void)
{
    _update_main_battery();
    _update_motor_battery();
}

__attribute__((weak))
void BatteryCalculator_Read_MainBatteryParameters(BatteryConfiguration_t* value)
{
    *value = (BatteryConfiguration_t) { .detectionVoltage = 0.0f, .minVoltage = 0.0f, .maxVoltage = 0.0f };
}

__attribute__((weak))
void BatteryCalculator_Read_MotorBatteryParameters(BatteryConfiguration_t* value)
{
    *value = (BatteryConfiguration_t) { .detectionVoltage = 0.0f, .minVoltage = 0.0f, .maxVoltage = 0.0f };
}

__attribute__((weak))
bool BatteryCalculator_Read_MainBatteryParametersChanged(void)
{
    return false;
}

__attribute__((weak))
bool BatteryCalculator_Read_MotorBatteryParametersChanged(void)
{
    return false;
}

__attribute__((weak))
Voltage_t BatteryCalculator_Read_MainBatteryVoltage(void)
{
    return 0.0f;
}

__attribute__((weak))
Voltage_t BatteryCalculator_Read_MotorBatteryVoltage(void)
{
    return 0.0f;
}

__attribute__((weak))
void BatteryCalculator_Write_MainBatteryLevel(uint8_t value)
{
    (void) value;
}

__attribute__((weak))
void BatteryCalculator_Write_MotorBatteryLevel(uint8_t value)
{
    (void) value;
}

__attribute__((weak))
void BatteryCalculator_Write_MotorBatteryPresent(bool value)
{
    (void) value;
}

__attribute__((weak))
void BatteryCalculator_Write_MainBatteryLow(bool value)
{
    (void) value;
}

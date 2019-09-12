#include "MotorCurrentFilter.h"
#include "utils.h"

static Current_t motorCurrents[6];

void MotorCurrentFilter_Run_OnInit(void)
{
    for (uint32_t i = 0u; i < ARRAY_SIZE(motorCurrents); i++)
    {
        motorCurrents[i] = 0.0f;
    }
}

void MotorCurrentFilter_Run_Update(void)
{
    for (uint32_t i = 0u; i < ARRAY_SIZE(motorCurrents); i++)
    {
        Current_t prev_current = motorCurrents[i];
        Current_t current = MotorCurrentFilter_Read_RawCurrent(i);

        current = current * 0.05f + prev_current * 0.95f;
        motorCurrents[i] = current;

        MotorCurrentFilter_Write_FilteredCurrent(i, current);
    }
}

__attribute__((weak))
Current_t MotorCurrentFilter_Read_RawCurrent(uint32_t index)
{
    (void) index;
    return 0.0f;
}

__attribute__((weak))
void MotorCurrentFilter_Write_FilteredCurrent(uint32_t index, Current_t value)
{
    (void) index;
    (void) value;
}

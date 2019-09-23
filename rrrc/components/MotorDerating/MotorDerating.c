#include "MotorDerating.h"
#include "utils.h"
#include "utils_assert.h"

void MotorDerating_Run_OnUpdate(void)
{
    MotorDeratingParameters_t params;
    MotorDerating_Read_Parameters(&params);

    for (uint32_t motor_idx = 0u; motor_idx < 6u; motor_idx++)
    {
        Temperature_t temp = MotorDerating_Read_MotorTemperature(motor_idx);
        int8_t control = MotorDerating_Read_ControlValue(motor_idx);

        float derated = map_constrained(temp, params.MaxSafeTemperature, params.MaxAllowedTemperature, control, 0);

        MotorDerating_Write_DeratedControlValue(motor_idx, (int8_t)lroundf(derated));
    }
}

__attribute__((weak))
void MotorDerating_Write_DeratedControlValue(uint32_t index, const int8_t value)
{
    ASSERT(index < 6);
}

__attribute__((weak))
int8_t MotorDerating_Read_ControlValue(uint32_t index)
{
    ASSERT(index < 6);
    return 0;
}

__attribute__((weak))
Temperature_t MotorDerating_Read_MotorTemperature(uint32_t index)
{
    ASSERT(index < 6);
    return (Temperature_t) 20.0f;
}

__attribute__((weak))
void MotorDerating_Read_Parameters(MotorDeratingParameters_t* value)
{
    ASSERT(value != NULL);
}

/*
 * MotorDerating.c
 *
 * Created: 2019. 07. 26. 15:03:18
 *  Author: Dániel Buga
 */ 

#include "MotorDerating.h"

#include "utils/functions.h"

#include <math.h>

#define MAX_SAFE_TEMP        (70.0f)
#define MAX_ALLOWED_TEMP    (130.0f)

void MotorDerating_Run_OnUpdate(uint8_t motor_idx)
{
    float temp = MotorDerating_Read_MotorTemperature(motor_idx);
    int8_t control = MotorDerating_Read_ControlValue(motor_idx);

    float derated = map_constrained(temp, MAX_SAFE_TEMP, MAX_ALLOWED_TEMP, control, 0);

    MotorDerating_Write_ControlValue(motor_idx, (int8_t)lroundf(derated));
}

__attribute__((weak))
float MotorDerating_Read_MotorTemperature(uint8_t motor_idx)
{
    (void) motor_idx;

    return 20.0f;
}

__attribute__((weak))
int8_t MotorDerating_Read_ControlValue(uint8_t motor_idx)
{
    (void) motor_idx;

    return 0;
}

__attribute__((weak))
void MotorDerating_Write_ControlValue(uint8_t motor_idx, int8_t control_value)
{
    (void) motor_idx;
    (void) control_value;
}

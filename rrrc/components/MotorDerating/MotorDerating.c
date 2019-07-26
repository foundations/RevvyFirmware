/*
 * MotorDerating.c
 *
 * Created: 2019. 07. 26. 15:03:18
 *  Author: Dániel Buga
 */ 

#include "MotorDerating.h"

__attribute__((weak))
uint8_t MotorDerating_Read_MotorTemperature(MotorDerating_t* motor)
{
    return 20u;
}

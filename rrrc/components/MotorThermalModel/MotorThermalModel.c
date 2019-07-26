/*
 * MotorThermalModel.c
 *
 * Created: 2019. 07. 26. 14:41:53
 *  Author: Dániel Buga
 */

#include "MotorThermalModel.h"

void MotorThermalModel_Run_OnInit(MotorThermalModel_t* model)
{

}

void MotorThermalModel_Run_OnUpdate(MotorThermalModel_t* model)
{

}

__attribute__((weak))
float MotorThermalModel_Read_MotorCurrent(MotorThermalModel_t* model)
{
    return 0.0f;
}

__attribute__((weak))
void MotorThermalModel_Write_Temperature(MotorThermalModel_t* model, uint8_t temp)
{

}

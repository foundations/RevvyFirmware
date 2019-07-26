/*
 * MotorThermalModel.c
 *
 * Created: 2019. 07. 26. 14:41:53
 *  Author: Dániel Buga
 */

#include "MotorThermalModel.h"

#define T_AMBIENT   (20.0f)

void MotorThermalModel_Run_OnInit(MotorThermalModel_t* model)
{
    model->temperature = T_AMBIENT;
}

void MotorThermalModel_Run_OnUpdate(MotorThermalModel_t* model)
{
    float current = MotorThermalModel_Read_MotorCurrent(model);



    MotorThermalModel_Write_Temperature(model, model->temperature);
}

__attribute__((weak))
float MotorThermalModel_Read_MotorCurrent(MotorThermalModel_t* model)
{
    return 0.0f;
}

__attribute__((weak))
void MotorThermalModel_Write_Temperature(MotorThermalModel_t* model, float temp)
{

}

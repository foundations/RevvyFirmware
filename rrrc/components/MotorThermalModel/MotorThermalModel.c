/*
 * MotorThermalModel.c
 *
 * Created: 2019. 07. 26. 14:41:53
 *  Author: Dániel Buga
 */

#include "MotorThermalModel.h"

#define T_AMBIENT           (20.0f)
#define MOTOR_RESISTANCE    (3.5f)
#define COEFF_OF_COOLING    (0.02f)
#define COEFF_OF_HEATING    (0.2f)

void MotorThermalModel_Run_OnInit(MotorThermalModel_t* model)
{
    model->temperature = T_AMBIENT;
}

void MotorThermalModel_Run_OnUpdate(MotorThermalModel_t* model)
{
    float current = MotorThermalModel_Read_MotorCurrent(model);
    float power = current * current * MOTOR_RESISTANCE;

    float cooling = COEFF_OF_COOLING * (model->temperature - T_AMBIENT);
    float heating = COEFF_OF_HEATING * power;

    model->temperature += heating - cooling;

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

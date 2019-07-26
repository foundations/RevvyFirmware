/*
 * MotorThermalModel.h
 *
 * Created: 2019. 07. 26. 14:42:05
 *  Author: Dániel Buga
 */ 

#ifndef COMPONENT_MOTOR_THERMAL_MODEL_H_
#define COMPONENT_MOTOR_THERMAL_MODEL_H_

#include <stdio.h>

typedef struct {
    uint8_t idx;
    float temperature;
} MotorThermalModel_t;

void MotorThermalModel_Run_OnInit(MotorThermalModel_t* model);
void MotorThermalModel_Run_OnUpdate(MotorThermalModel_t* model);

float MotorThermalModel_Read_MotorCurrent(MotorThermalModel_t* model);
void MotorThermalModel_Write_Temperature(MotorThermalModel_t* model, float temp);

#endif /* COMPONENT_MOTOR_THERMAL_MODEL_H_ */

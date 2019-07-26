/*
 * MotorDerating.h
 *
 * Created: 2019. 07. 26. 15:03:28
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_MOTOR_DERATING_H_
#define COMPONENT_MOTOR_DERATING_H_

#include <stdio.h>

void MotorDerating_Run_OnUpdate(uint8_t motor_idx);
float MotorDerating_Read_MotorTemperature(uint8_t motor_idx);
int8_t MotorDerating_Read_ControlValue(uint8_t motor_idx);
void MotorDerating_Write_ControlValue(uint8_t motor_idx, int8_t control_value);

#endif /* COMPONENT_MOTOR_DERATING_H_ */

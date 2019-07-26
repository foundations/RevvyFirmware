/*
 * MotorDerating.h
 *
 * Created: 2019. 07. 26. 15:03:28
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_MOTOR_DERATING_H_
#define COMPONENT_MOTOR_DERATING_H_

#include <stdio.h>

typedef struct {
    uint8_t motor_idx;
} MotorDerating_t;

uint8_t MotorDerating_Read_MotorTemperature(MotorDerating_t* motor);

#endif /* COMPONENT_MOTOR_DERATING_H_ */

/*
 * motor_servo.h
 *
 * Created: 2/27/2019 9:10:06 PM
 *  Author: User
 */ 


#ifndef MOTOR_SERVO_H_
#define MOTOR_SERVO_H_


#include <stddef.h>
#include "rrrc_motor_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t SERVO_Init(void* hw_port);

static motor_lib_entry_t motor_servo =
{
	.type_id = MOTOR_SERVO,
	.name = "MOTOR_SERVO",
	.MotorInit = SERVO_Init,
	.MotorDeInit = NULL,
};

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_SERVO_H_ */
/*
 * rrrc_motors.h
 *
 * Created: 1/13/2019 10:08:30 PM
 *  Author: User
 */ 


#ifndef RRRC_MOTORS_H_
#define RRRC_MOTORS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "motors/rrrc_motor_interface.h"

#define MOTOR_PORT_AMOUNT 6

uint8_t MotorPortGetPortsAmount();
uint8_t	MotorPortGetTypesAmount();
char* MotorPortGetTypeName(uint32_t idx);
uint8_t MotorPortGetTypeId(uint32_t idx);
uint32_t MotorPortGetTypes(uint8_t *data, uint32_t max_size);

uint32_t MotorPortSetType(uint32_t port, motor_type_t type);
uint32_t MotorPortGetType(uint32_t port);
uint32_t MotorPortSetState(uint32_t port, int8_t state); // free run
int8_t MotorPortGetState(uint32_t port);
uint32_t MotoPortSetSteps(uint32_t port, uint32_t steps); // run steps
uint32_t MotorPortGetCount(uint32_t port_idx, uint32_t* data);

int32_t MotorPortInit(uint32_t port);
int32_t MotorPortDeInit(uint32_t port);

#ifdef __cplusplus
}
#endif

#endif /* RRRC_MOTORS_H_ */
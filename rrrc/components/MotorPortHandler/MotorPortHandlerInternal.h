/*
 * MotorPortHandlerInternal.h
 *
 * Created: 09/05/2019 16:10:40
 *  Author: Dániel Buga
 */ 


#ifndef MOTOR_PORT_HANDLER_INTERNAL_H_
#define MOTOR_PORT_HANDLER_INTERNAL_H_

#include "MotorPortHandler.h"

#include <stdbool.h>

void MotorPort_SetGreenLed(struct _MotorPort_t* port, bool state);
void MotorPort_SetDriveValue(struct _MotorPort_t* port, int8_t value);

#endif /* MOTOR_PORT_HANDLER_INTERNAL_H_ */

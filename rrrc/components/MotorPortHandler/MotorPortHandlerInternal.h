/*
 * MotorPortHandlerInternal.h
 *
 * Created: 09/05/2019 16:10:40
 *  Author: Dániel Buga
 */ 


#ifndef MOTOR_PORT_HANDLER_INTERNAL_H_
#define MOTOR_PORT_HANDLER_INTERNAL_H_

#include "MotorPortHandler.h"

struct _MotorPort_t;

#include "MotorPortLibraries/MotorPortLibrary.h"

typedef struct _MotorPort_t
{
    const struct _MotorLibrary_t* library;
} MotorPort_t;

#endif /* MOTOR_PORT_HANDLER_INTERNAL_H_ */

/*
 * MotorPortHandlerInternal.c
 *
 * Created: 10/05/2019 13:44:51
 *  Author: Dániel Buga
 */ 
#include "MotorPortHandlerInternal.h"

void MotorPort_SetGreenLed(MotorPort_t* port, bool state)
{
    gpio_set_pin_level(port->led, state);
}

void MotorPort_SetDriveValue(struct _MotorPort_t* port, int8_t value)
{
    MotorPortHandler_Write_MotorDriveValue(port->port_idx, value);
}

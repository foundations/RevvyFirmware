/*
 * MotorPortHandlerInternal.c
 *
 * Created: 10/05/2019 13:44:51
 *  Author: D�niel Buga
 */ 
#include "MotorPortHandlerInternal.h"

#include <hal_gpio.h>

void MotorPort_SetGreenLed(MotorPort_t* port, bool state)
{
    gpio_set_pin_level(port->led, !state);
}

void MotorPort_SetDriveValue(struct _MotorPort_t* port, int8_t value)
{
    MotorPortHandler_Write_MotorDriveValue(port->driver_idx, port->driver_channel, value);
}

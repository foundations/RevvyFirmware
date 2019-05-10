/*
 * SensorPortHandlerInternal.c
 *
 * Created: 10/05/2019 18:01:33
 *  Author: Dániel Buga
 */ 
#include "SensorPortHandlerInternal.h"

void SensorPort_SetGreenLed(SensorPort_t* port, bool state)
{
    gpio_set_pin_level(port->led0, state);
}

void SensorPort_SetOrangeLed(SensorPort_t* port, bool state)
{
    gpio_set_pin_level(port->led1, state);
}

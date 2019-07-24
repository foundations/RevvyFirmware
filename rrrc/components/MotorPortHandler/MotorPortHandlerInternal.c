/*
 * MotorPortHandlerInternal.c
 *
 * Created: 10/05/2019 13:44:51
 *  Author: Dániel Buga
 */ 
#include "MotorPortHandlerInternal.h"

#include <hal_gpio.h>
#include <hal_ext_irq.h>

void MotorPort_SetGreenLed(MotorPort_t* port, bool state)
{
    gpio_set_pin_level(port->led, !state);
}

void MotorPort_SetDriveValue(struct _MotorPort_t* port, int8_t value)
{
    MotorPortHandler_Write_MotorDriveValue(port->driver_idx, port->driver_channel, value);
}

void MotorPort_DisableExti0(struct _MotorPort_t* motorPort)
{
    ext_irq_disable(motorPort->enc0);
}

void MotorPort_DisableExti1(struct _MotorPort_t* motorPort)
{
    ext_irq_disable(motorPort->enc1);
}

void MotorPort_EnableExti0(struct _MotorPort_t* motorPort)
{
    ext_irq_enable(motorPort->enc0);
}

void MotorPort_EnableExti1(struct _MotorPort_t* motorPort)
{
    ext_irq_enable(motorPort->enc1);
}
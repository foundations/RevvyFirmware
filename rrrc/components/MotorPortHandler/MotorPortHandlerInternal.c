#include "MotorPortHandlerInternal.h"

#include <hal_gpio.h>
#include <hal_ext_irq.h>

void MotorPort_SetGreenLed(MotorPort_t* port, bool state)
{
    gpio_set_pin_level(port->led, !state);
}

void MotorPort_SetDriveValue(struct _MotorPort_t* port, int8_t value)
{
    MotorPortHandler_Write_DriveStrength(port->port_idx, value);
}

void MotorPort_DisableExti0(struct _MotorPort_t* motorPort)
{
    int32_t res = ext_irq_disable(motorPort->enc0);
    ASSERT(res == ERR_NONE);
}

void MotorPort_DisableExti1(struct _MotorPort_t* motorPort)
{
    int32_t res = ext_irq_disable(motorPort->enc1);
    ASSERT(res == ERR_NONE);
}

void MotorPort_EnableExti0(struct _MotorPort_t* motorPort)
{
    int32_t res = ext_irq_enable(motorPort->enc0, &MotorPort_gpio0_ext_cb, motorPort);
    ASSERT(res == ERR_NONE);
}

void MotorPort_EnableExti1(struct _MotorPort_t* motorPort)
{
    int32_t res = ext_irq_enable(motorPort->enc1, &MotorPort_gpio1_ext_cb, motorPort);
    ASSERT(res == ERR_NONE);
}

void MotorPort_gpio0_ext_cb(void* port)
{
    MotorPort_t* motorPort = (MotorPort_t*) port;
    motorPort->library->Gpio0Callback(port, gpio_get_pin_level(motorPort->enc0), gpio_get_pin_level(motorPort->enc1));
}

void MotorPort_gpio1_ext_cb(void* port)
{
    MotorPort_t* motorPort = (MotorPort_t*) port;
    motorPort->library->Gpio1Callback(port, gpio_get_pin_level(motorPort->enc0), gpio_get_pin_level(motorPort->enc1));
}

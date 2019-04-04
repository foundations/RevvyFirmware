/*
 * rrrc_motor_base_function.h
 *
 * Created: 2/27/2019 9:09:20 PM
 *  Author: User
 */ 


#ifndef RRRC_MOTOR_BASE_FUNCTION_H_
#define RRRC_MOTOR_BASE_FUNCTION_H_

#include <stdint.h>
#include <stddef.h>
#include "rrrc_motor_interface.h"
#include "rrrc_hal.h"
#include "hal_gpio.h"
#include "hal_timer.h"

//*********************************************************************************************
static int32_t MotorPort_gpio0_get_state(const p_hw_motor_port_t motport)
{
    if (motport == NULL)
        return 0;
    return gpio_get_pin_level(motport->enc0_gpio);
}

//*********************************************************************************************
static int32_t MotorPort_gpio1_get_state(const p_hw_motor_port_t motport)
{
    if (motport == NULL)
        return 0;
    return gpio_get_pin_level(motport->enc1_gpio);
}

//*********************************************************************************************
static int32_t MotorPort_gpio0_set_as_extint(const p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return 0;
	gpio_set_pin_pull_mode(motport->enc0_gpio, GPIO_PULL_OFF);
	gpio_set_pin_direction(motport->enc0_gpio, GPIO_DIRECTION_IN);
	gpio_set_pin_function(motport->enc0_gpio, GPIO_PIN_FUNCTION_A);
	return 0;
}

//*********************************************************************************************
static int32_t MotorPort_gpio1_set_as_extint(const p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return 0;
	gpio_set_pin_pull_mode(motport->enc1_gpio, GPIO_PULL_OFF);
	gpio_set_pin_direction(motport->enc1_gpio, GPIO_DIRECTION_IN);
	gpio_set_pin_function(motport->enc1_gpio, GPIO_PIN_FUNCTION_A);
	return 0;
}

//*********************************************************************************************
static void MotorPort_led0_on(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led0_gpio, 1);
}

//*********************************************************************************************
static void MotorPort_led0_off(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led0_gpio, 0);
}

//*********************************************************************************************
static void MotorPort_led0_toggle(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_toggle_pin_level(motport->led0_gpio);
}

//*********************************************************************************************
static void MotorPort_led1_on(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led1_gpio, 1);
}

//*********************************************************************************************
static void MotorPort_led1_off(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led1_gpio, 0);
}

//*********************************************************************************************
static void MotorPort_led1_toggle(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_toggle_pin_level(motport->led1_gpio);
}

#endif /* RRRC_MOTOR_BASE_FUNCTION_H_ */
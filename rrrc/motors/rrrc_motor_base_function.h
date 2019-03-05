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

static void MotorPort_dir_stop(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;	
	gpio_set_pin_level(motport->dir0_gpio, false);
	gpio_set_pin_level(motport->dir1_gpio, false);
}

static void MotorPort_dir_forward(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	MotorPort_dir_stop(port);
	gpio_set_pin_level(motport->dir0_gpio, true);
}

static void MotorPort_dir_backward(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	MotorPort_dir_stop(port);
	gpio_set_pin_level(motport->dir1_gpio, true);
}

static void MotorPort_set_speed(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	//tcc_timer_set_duty_cycle(motport->PWM0, motport->pwm0_ch, data)
}

static void MotorPort_led0_on(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led0_gpio, 1);
}

static void MotorPort_led0_off(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led0_gpio, 0);
}

static void MotorPort_led0_toggle(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	gpio_toggle_pin_level(motport->led0_gpio);
}

static void MotorPort_led1_on(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led1_gpio, 1);
}

static void MotorPort_led1_off(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led1_gpio, 0);
}

static void MotorPort_led1_toggle(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;
	gpio_toggle_pin_level(motport->led1_gpio);
}


#endif /* RRRC_MOTOR_BASE_FUNCTION_H_ */
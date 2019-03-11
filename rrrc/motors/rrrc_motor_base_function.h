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

static void MotorPort_set_duty_cicle(p_hw_motor_port_t motport, uint32_t duty)
{
	if (motport == NULL)
		return;
	timer_stop(motport->pwm);
	timer_set_chan_compare_value(motport->pwm, motport->pwm_ch, duty);
	timer_start(motport->pwm);
}

static void MotorPort_dir_stop(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;	
	//MotorPort_set_duty_cicle(motport, 200);
	gpio_set_pin_level(motport->dir0_gpio, false);
	gpio_set_pin_level(motport->dir1_gpio, false);
}

static void MotorPort_dir_forward(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	MotorPort_dir_stop(motport);
	gpio_set_pin_level(motport->dir0_gpio, true);
}

static void MotorPort_dir_backward(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	MotorPort_dir_stop(motport);
	gpio_set_pin_level(motport->dir1_gpio, true);
}

static void MotorPort_led0_on(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led0_gpio, 1);
}

static void MotorPort_led0_off(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led0_gpio, 0);
}

static void MotorPort_led0_toggle(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_toggle_pin_level(motport->led0_gpio);
}

static void MotorPort_led1_on(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led1_gpio, 1);
}

static void MotorPort_led1_off(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_set_pin_level(motport->led1_gpio, 0);
}

static void MotorPort_led1_toggle(p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return;
	gpio_toggle_pin_level(motport->led1_gpio);
}

static int32_t MotorPort_gpio0_get_state(const p_hw_motor_port_t motport)
{
	if (motport == NULL)
		return 0;
	return gpio_get_pin_level(motport->enc0_gpio);
}

static int32_t MotorPort_gpio1_get_state(const p_hw_motor_port_t motport)
{
	if (motport == NULL)
	return 0;
	return gpio_get_pin_level(motport->enc1_gpio);
}

#endif /* RRRC_MOTOR_BASE_FUNCTION_H_ */
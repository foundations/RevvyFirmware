/*
 * rrrc_sensor_base_func.h
 *
 * Created: 2/26/2019 6:15:37 PM
 *  Author: User
 */ 


#ifndef RRRC_SENSOR_BASE_FUNC_H_
#define RRRC_SENSOR_BASE_FUNC_H_

#include <stdint.h>
#include <stddef.h>
#include "rrrc_sensor_interface.h"
#include "rrrc_hal.h"
#include "hal_gpio.h"
#include "hal_timer.h"


//*********************************************************************************************
static void SensorPort_timer_start(const void* port) 
{
	p_hw_sensor_port_t sensport = port;
	if (sensport == NULL)
		return;
	_tc_timer_start(sensport->TIMER);
	return;
}

//*********************************************************************************************
static void SensorPort_timer_stop(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return;
	_tc_timer_stop(sensport->TIMER);
	return;
}

//*********************************************************************************************
static uint32_t SensorPort_timer_get_counter(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return;
	uint32_t val = 0;
	timer_get_clock_cycles_in_tick(sensport->TIMER, &val);
	return val;
}

//*********************************************************************************************
static int32_t SensorPort_gpio0_set_as_gpio(const p_hw_sensor_port_t sensport, enum gpio_direction dir, enum gpio_pull_mode pull_mode)
{
	if (sensport == NULL)
		return 0;
	gpio_set_pin_function(sensport->gpio0_num, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_direction(sensport->gpio0_num, dir);
	gpio_set_pin_pull_mode(sensport->gpio0_num, pull_mode);
	return 0;
}

//*********************************************************************************************
static int32_t SensorPort_gpio0_set_as_extint(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return 0;
	gpio_set_pin_pull_mode(sensport->gpio0_num, GPIO_PULL_OFF);
	gpio_set_pin_direction(sensport->gpio0_num, GPIO_DIRECTION_IN);
	gpio_set_pin_function(sensport->gpio0_num, GPIO_PIN_FUNCTION_A);
	return 0;
}

//*********************************************************************************************
static int32_t SensorPort_gpio0_get_state(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return 0;
	return gpio_get_pin_level(sensport->gpio0_num);
}

//*********************************************************************************************
static int32_t SensorPort_gpio0_set_state(const p_hw_sensor_port_t sensport, enum gpio_direction data)
{
	if (sensport == NULL)
		return 0;
	gpio_set_pin_level(sensport->gpio0_num, data?1:0);
	return gpio_get_pin_level(sensport->gpio0_num);
}

//*********************************************************************************************
static int32_t SensorPort_gpio0_toggle_state(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return 0;
	gpio_toggle_pin_level(sensport->gpio0_num);
	return gpio_get_pin_level(sensport->gpio0_num);
}

//*********************************************************************************************
static int32_t SensorPort_gpio1_set_as_gpio(const p_hw_sensor_port_t sensport, enum gpio_direction dir, enum gpio_pull_mode pull_mode)
{
	if (sensport == NULL)
		return 0;
	gpio_set_pin_function(sensport->gpio1_num, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_direction(sensport->gpio1_num, dir);
	gpio_set_pin_pull_mode(sensport->gpio1_num, pull_mode);
	return 0;
}

//*********************************************************************************************
static int32_t SensorPort_gpio1_set_as_extint(const p_hw_sensor_port_t sensport, enum gpio_direction dir)
{
	if (sensport == NULL)
		return 0;
	gpio_set_pin_pull_mode(sensport->gpio1_num, GPIO_PULL_OFF);
	gpio_set_pin_direction(sensport->gpio1_num, GPIO_DIRECTION_IN);
	gpio_set_pin_function(sensport->gpio1_num, GPIO_PIN_FUNCTION_A);
	return 0;		
}

//*********************************************************************************************
static int32_t SensorPort_gpio1_get_state(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return 0;
	return gpio_get_pin_level(sensport->gpio1_num);
}

//*********************************************************************************************
static int32_t SensorPort_gpio1_set_state(const p_hw_sensor_port_t sensport, int32_t data)
{
	if (sensport == NULL)
		return 0;
	gpio_set_pin_level(sensport->gpio1_num, data?1:0);
	return gpio_get_pin_level(sensport->gpio1_num);
}

//*********************************************************************************************
static int32_t SensorPort_gpio1_toggle_state(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return 0;
	gpio_toggle_pin_level(sensport->gpio1_num);
	return gpio_get_pin_level(sensport->gpio1_num);
}

//*********************************************************************************************
static void SensorPort_led0_on(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return;
	gpio_set_pin_level(sensport->led0_gpio, 1);
}

//*********************************************************************************************
static void SensorPort_led0_off(const p_hw_sensor_port_t sensport)
{

	if (sensport == NULL)
		return;
	gpio_set_pin_level(sensport->led0_gpio, 0);
}

//*********************************************************************************************
static void SensorPort_led0_toggle(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return;
	gpio_toggle_pin_level(sensport->led0_gpio);
}

//*********************************************************************************************
static void SensorPort_led1_on(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return;
	gpio_set_pin_level(sensport->led1_gpio, 1);
}

//*********************************************************************************************
static void SensorPort_led1_off(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return;
	gpio_set_pin_level(sensport->led1_gpio, 0);
}

//*********************************************************************************************
static void SensorPort_led1_toggle(const p_hw_sensor_port_t sensport)
{
	if (sensport == NULL)
		return;
	gpio_toggle_pin_level(sensport->led1_gpio);
}

//*********************************************************************************************
static void SensorPort_set_vccio(const p_hw_sensor_port_t sensport, sensor_type_t val){
	if (sensport == NULL)
		return;
	gpio_set_pin_level(sensport->vccio_pin, val);
}

#endif /* RRRC_SENSOR_BASE_FUNC_H_ */
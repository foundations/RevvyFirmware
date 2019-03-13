
#include "rrrc_sensor_base_func.h"
#include "sensor_HC_SR05.h"
#include <hal_delay.h>

uint32_t GetTimerCounter(p_hw_sensor_port_t hwport);

#define MAX_SENSOR_VALUES 1

static void HC_SR05_thread_done(const struct timer_task *const timer_task);

int32_t HC_SR05_Init(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_sensor_port_t sensport = hw_port;
	if (!sensport)
		return ERR_INVALID_DATA;

	memset(sensport->lib_data, 0, SENSOR_PORT_LIBDATA);
	
	SensorPort_set_vccio(sensport, SENSOR_VCCIO_5V0);
	SensorPort_gpio0_set_as_extint(sensport);
	SensorPort_gpio1_set_as_gpio(sensport, GPIO_DIRECTION_OUT, GPIO_PULL_OFF);
	SensorPort_gpio1_set_state(sensport, 0);	

	p_hc_sr05_data_t sens_data = sensport->lib_data;
	RRRC_add_task(&(sens_data->sensor_task), &HC_SR05_thread_done, 20, sensport, true);

	return result;
}

uint32_t HC_SR05_get_value(void* hw_port, uint32_t* data, uint32_t max_size)
{
	uint32_t amount = 0;
	p_hw_sensor_port_t sensport = hw_port;
	if (hw_port && data && max_size && (max_size>=MAX_SENSOR_VALUES))
	{
		p_hc_sr05_data_t sens_data = sensport->lib_data;
		uint32_t ticks_in_us = get_cycles_for_1us();
		uint32_t distance_cm = sens_data->distanse_tick/ticks_in_us/58;
		data[0] = SwapEndian(distance_cm);
		amount = 1;
	}
	return amount;
}

void HC_SR05_Thread(void* hw_port)
{
	p_hw_sensor_port_t sensport = hw_port;
	if (sensport == NULL)
		return;
	p_hc_sr05_data_t sens_data = sensport->lib_data;

	uint32_t ticks_in_us = get_cycles_for_1us();
	uint32_t ticks_in_ms = get_cycles_for_1ms();
	uint32_t distance_us = sens_data->distanse_tick/ticks_in_us;
	uint32_t distance_ms = sens_data->distanse_tick/ticks_in_ms;
	uint32_t distance_cm = distance_us/58;

	if (sens_data->self_curr_count == sens_data->self_prev_count)
	{
 		SensorPort_gpio1_set_state(sensport, 1);
 		delay_us(15);
 		SensorPort_gpio1_set_state(sensport, 0);
	}
	sens_data->self_prev_count = sens_data->self_curr_count;

	return;
}

void HC_SR05_gpio0_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = hw_port;
	if (sensport == NULL)
		return;
	p_hc_sr05_data_t sens_data = sensport->lib_data;

	if (data)
	{
		sens_data->start_time = get_system_tick();
	}
	else
	{
		sens_data->finish_time = get_system_tick();
		sens_data->distanse_tick = sens_data->start_time - sens_data->finish_time;
		RRRC_remove_task(&(sens_data->sensor_task));
		RRRC_add_task(&(sens_data->sensor_task), &HC_SR05_thread_done, 20, sensport, true);
	}
	return;
}

static void HC_SR05_thread_done(const struct timer_task *const timer_task)
{
	p_hw_sensor_port_t sensport = timer_task->user_data;
	if (!sensport)
		return;

	p_hc_sr05_data_t sens_data = sensport->lib_data;
	uint32_t ticks_in_us = get_cycles_for_1us();
	uint32_t ticks_in_ms = get_cycles_for_1ms();
	uint32_t distance_us = sens_data->distanse_tick/ticks_in_us;
	uint32_t distance_ms = sens_data->distanse_tick/ticks_in_ms;
	uint32_t distance_cm = distance_us/58;
	if (sensport == NULL)	
		return;
	SensorPort_gpio1_set_state(sensport, 1);
	delay_us(15);
	SensorPort_gpio1_set_state(sensport, 0);
	sens_data->self_curr_count++;
	SensorPort_led1_toggle(sensport);
}

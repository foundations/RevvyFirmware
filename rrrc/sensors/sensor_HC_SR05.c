
#include "rrrc_sensor_base_func.h"
#include "sensor_HC_SR05.h"
#include <hal_delay.h>

uint32_t GetTimerCounter(p_hw_sensor_port_t hwport);

#define MAX_SENSOR_VALUES 1

static void HC_SR05_thread_done(const struct timer_task *const timer_task);
void HCSR05_xTask(void* hw_port);

//*********************************************************************************************
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

	char task_name[configMAX_TASK_NAME_LEN+1];
	snprintf(task_name, configMAX_TASK_NAME_LEN, "hcsr05_p%01d", sensport->index);

	if (xTaskCreate(HCSR05_xTask, task_name, 256 / sizeof(portSTACK_TYPE), sensport, tskIDLE_PRIORITY+1, &(sens_data->xHCSR05Task)) != pdPASS)
		return ERR_FAILURE;

	return result;
}

int32_t HC_SR05_DeInit(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_sensor_port_t sensport = hw_port;
	if (sensport == NULL)
		return ERR_INVALID_ARG;
	p_hc_sr05_data_t sens_data = sensport->lib_data;
	vTaskDelete(sens_data->xHCSR05Task);

	return result;
}

//*********************************************************************************************
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

//*********************************************************************************************
void HCSR05_xTask(void* hw_port)
{
	BaseType_t xResult;
	uint32_t ulNotifiedValue;

	p_hw_sensor_port_t sensport = hw_port;
	if (sensport == NULL)
		return;
	p_hc_sr05_data_t sens_data = sensport->lib_data;

    TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
	 	SensorPort_gpio1_set_state(sensport, 1);
	 	delay_us(15);
	 	SensorPort_gpio1_set_state(sensport, 0);
		sens_data->self_curr_count++;

		SensorPort_led1_on(sensport);
        (void) xTaskNotifyWait(pdFALSE, UINT32_MAX, &ulNotifiedValue, portMAX_DELAY);
    	SensorPort_led1_off(sensport);

        // sleep for at least 20ms but set a desired sample rate of 2 samples per sec
        vTaskDelay(rtos_ms_to_ticks(20));
        vTaskDelayUntil(&xLastWakeTime, rtos_ms_to_ticks(500));
	}
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
		if (sens_data->err_wait_counter == 10)
		{
            xTaskNotify(sens_data->xHCSR05Task, 0x01, eSetBits);
			sens_data->err_wait_counter = 0;
		}else
			sens_data->err_wait_counter++;
	}
    else
    {
        sens_data->self_prev_count = sens_data->self_curr_count;
        sens_data->err_wait_counter = 0u;
    }

	return;
}

//*********************************************************************************************
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
		if (sens_data->start_time>sens_data->finish_time)
			sens_data->distanse_tick = sens_data->finish_time - sens_data->start_time;
		else
			sens_data->distanse_tick = sens_data->finish_time - sens_data->start_time;
		
		BaseType_t xHigherPriorityTaskWoken = pdTRUE;
		xTaskNotifyFromISR(sens_data->xHCSR05Task, 0x01, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	return;
}

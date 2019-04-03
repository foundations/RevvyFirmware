
#include "rrrc_sensor_base_func.h"
#include "sensor_HC_SR05.h"
#include <hal_delay.h>
#include <math.h>

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

		uint32_t ticks_in_ms = high_res_timer_ticks_per_ms();
        portENTER_CRITICAL();
        uint32_t distance_tick = sens_data->filtered_distance_tick;
        portEXIT_CRITICAL();

		float distance_ms = (float)distance_tick / ticks_in_ms;
		uint32_t distance_cm = (uint32_t)lroundf(distance_ms * 17.0f);

		data[0] = SwapEndian(distance_cm);
		amount = 1;
	}
	return amount;
}

static inline void swap_uint32(uint32_t* a, uint32_t* b)
{
    uint32_t t = *a;
    *a = *b;
    *b = t;
}

static void update_filtered_distance(p_hc_sr05_data_t sens_data)
{
    uint32_t ordered[HCSR05_MEDIAN_FITLER_SIZE];

    memcpy(&ordered[0], &sens_data->distanceBuffer[0], sizeof(sens_data->distanceBuffer));
    ordered[HCSR05_MEDIAN_FITLER_SIZE - 1] = sens_data->distance_tick;

    sens_data->distanceBuffer[sens_data->distanceBufferWriteIdx] = sens_data->distance_tick;
    sens_data->distanceBufferWriteIdx = (sens_data->distanceBufferWriteIdx + 1) % ARRAY_SIZE(sens_data->distanceBuffer);

    for (uint32_t i = 0; i < HCSR05_MEDIAN_FITLER_SIZE - 1; i++)
    {
        for (uint32_t j = i; j < HCSR05_MEDIAN_FITLER_SIZE - 1; j++)
        {
            if (ordered[i] > ordered[j])
            {
                swap_uint32(&ordered[i], &ordered[j]);
            }
        }
    }

    sens_data->filtered_distance_tick = ordered[HCSR05_MEDIAN_FITLER_SIZE / 2];
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

        (void) ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        update_filtered_distance(sens_data);

        // set a desired sample rate of 2 samples per sec but sleep for at least 200ms
        vTaskDelay(rtos_ms_to_ticks(200));
        vTaskDelayUntil(&xLastWakeTime, rtos_ms_to_ticks(500));
	}
}
void HC_SR05_Thread(void* hw_port)
{
	p_hw_sensor_port_t sensport = hw_port;
	if (sensport == NULL)
		return;
	p_hc_sr05_data_t sens_data = sensport->lib_data;

	if (sens_data->self_curr_count == sens_data->self_prev_count)
	{
		if (sens_data->err_wait_counter == 10)
		{
            xTaskNotifyGive(sens_data->xHCSR05Task);
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
		sens_data->start_time = high_res_timer_get_count();
		SensorPort_led1_on(sensport);
	}
	else
	{
		uint32_t finish_time = high_res_timer_get_count();
    	SensorPort_led1_off(sensport);

		uint32_t dist = finish_time - sens_data->start_time;

        if (dist < 0x7FFFFFFF)
        {
            sens_data->distance_tick = dist;
        }
		
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(sens_data->xHCSR05Task, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	return;
}

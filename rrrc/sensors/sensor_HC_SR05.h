

#ifndef _SENSOR_HC_SR05_H_
#define _SENSOR_HC_SR05_H_

#include "rrrc_sensor_interface.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t HC_SR05_Init(void* hw_port);
int32_t HC_SR05_DeInit(void* hw_port);
uint32_t HC_SR05_get_value(void* hw_port, uint32_t* data, uint32_t max_size);
void HC_SR05_Thread(void* hw_port);
void HC_SR05_gpio0_callback(void* hw_port, uint32_t data);
void HC_SR05_gpio1_callback(void* hw_port, uint32_t data);

static sensor_lib_entry_t sensor_hc_sr_05 =
{
    .type_id = SENSOR_HC_SR05,
    .name = "HC_SR05",
    .SensorInit = HC_SR05_Init,
	.SensorDeInit = HC_SR05_DeInit,
    .sensor_thread = HC_SR05_Thread,
	.sensor_get_values = HC_SR05_get_value,
	.write_data = NULL,
	.read_data = NULL,

    //callback from sensor port
    .timer_callback = NULL,
    .adc_callback = NULL,
    .gpio0_callback = HC_SR05_gpio0_callback,
    .gpio1_callback = NULL,
};


typedef struct _hc_sr05_data_t
{
	TaskHandle_t    xHCSR05Task;
	uint32_t distanse_tick;
	uint32_t start_time;
	uint32_t finish_time;
	uint32_t self_prev_count;
	uint32_t self_curr_count;
}hc_sr05_data_t, *p_hc_sr05_data_t;

#ifdef __cplusplus
}
#endif

#endif

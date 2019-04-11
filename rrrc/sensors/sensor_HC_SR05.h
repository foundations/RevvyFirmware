

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
void HC_SR05_gpio0_callback(void* hw_port, uint32_t data);
void HC_SR05_gpio1_callback(void* hw_port, uint32_t data);

static sensor_lib_entry_t sensor_hc_sr_05 =
{
    .type_id = SENSOR_HC_SR05,
    .name = "HC_SR05",
    .SensorInit = HC_SR05_Init,
	.SensorDeInit = HC_SR05_DeInit,
	.sensor_get_values = HC_SR05_get_value,
	.write_data = NULL,
	.read_data = NULL,

    //callback from sensor port
    .timer_callback = NULL,
    .adc_callback = NULL,
    .gpio0_callback = HC_SR05_gpio0_callback,
};

#define HCSR05_MEDIAN_FITLER_SIZE   3

typedef struct _hc_sr05_data_t
{
    TaskHandle_t    xHCSR05Task;
    TaskHandle_t    xHCSR05CheckerTask;
	uint32_t distance_tick;
	uint32_t filtered_distance_tick;
	uint32_t start_time;
	uint32_t self_prev_count;
	uint32_t self_curr_count;
    uint32_t err_wait_counter;
    uint32_t distanceBuffer[HCSR05_MEDIAN_FITLER_SIZE - 1];
    uint32_t distanceBufferWriteIdx;
}hc_sr05_data_t, *p_hc_sr05_data_t;

#ifdef __cplusplus
}
#endif

#endif

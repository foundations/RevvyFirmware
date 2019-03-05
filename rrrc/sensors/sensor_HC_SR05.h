

#ifndef _SENSOR_HC_SR05_H_
#define _SENSOR_HC_SR05_H_

#include "rrrc_sensor_interface.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t HC_SR05_Init(void* hw_port);
uint32_t HC_SR05_get_value(void* hw_port, uint32_t* data, uint32_t max_size);
void HC_SR05_Thread(void* hw_port);
void HC_SR05_gpio0_callback(void* hw_port, uint32_t data);
void HC_SR05_gpio1_callback(void* hw_port, uint32_t data);

static sensor_lib_entry_t sensor_hc_sr_05 =
{
    .type_id = SENSOR_HC_SR05,
    .name = "HC_SR05",
    .SensorInit = HC_SR05_Init,
    .sensor_thread = HC_SR05_Thread,
	.sensor_get_values = HC_SR05_get_value,

    //callback from sensor port
    .timer_callback = NULL,
    .adc_callback = NULL,
    .gpio0_callback = HC_SR05_gpio0_callback,
    .gpio1_callback = HC_SR05_gpio1_callback,
};


typedef struct _hc_sr05_data_t
{
	uint32_t distanse;
}hc_sr05_data_t, *p_hc_sr05_data_t;

#ifdef __cplusplus
}
#endif

#endif

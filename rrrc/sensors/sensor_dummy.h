

#ifndef _SENSOR_DUMMY_H_
#define _SENSOR_DUMMY_H_

#include "rrrc_sensor_interface.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t SENSOR_DUMMY_Init(void* hw_port);
void SENSOR_DUMMY_Thread(void* hw_port);
void SENSOR_DUMMY_gpio0_callback(void* hw_port, uint32_t data);
void SENSOR_DUMMY_adc_callback(void* hw_port, uint32_t data);
void SENSOR_DUMMY_timer_callback(void* hw_port, uint32_t data);

static sensor_lib_entry_t sensor_dummy =
{
    .type_id = SENSOR_NOT_SET,
    .name = "NO_SET",
    .SensorInit = SENSOR_DUMMY_Init,
	.SensorDeInit = NULL,
    .sensor_thread = SENSOR_DUMMY_Thread,
	.write_data = NULL,
	.read_data = NULL,

    //callback from sensor port
    .timer_callback = SENSOR_DUMMY_timer_callback,
    .adc_callback = SENSOR_DUMMY_adc_callback,
    .gpio0_callback = SENSOR_DUMMY_gpio0_callback,
};

#ifdef __cplusplus
}
#endif

#endif //_SENSOR_DUMMY_H_



#ifndef _SENSOR_BUTTON_H_
#define _SENSOR_BUTTON_H_


#include "rrrc_sensor_interface.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t BUTTON_Init(void* hw_port);
uint32_t BUTTON_get_value(void* hw_port, uint32_t* data, uint32_t max_size);
void BUTTON_Thread(void* hw_port);
void BUTTON_gpio0_callback(void* hw_port, uint32_t data);

static sensor_lib_entry_t sensor_button =
{
    .type_id = SENSOR_SWITCH,
    .name = "BUTTON",
    .SensorInit = BUTTON_Init,
	.sensor_get_values = BUTTON_get_value,
    .sensor_thread = BUTTON_Thread,
	.write_data = NULL,
	.read_data = NULL,

    //callback from sensor port
    .timer_callback = NULL,
    .adc_callback = NULL,
    .gpio0_callback = BUTTON_gpio0_callback,
    .gpio1_callback = NULL,
};

typedef struct _button_data_t
{
	uint32_t state;
}button_data_t, *p_button_data_t;


#ifdef __cplusplus
}
#endif

#endif //_SENSOR_BUTTON_H_

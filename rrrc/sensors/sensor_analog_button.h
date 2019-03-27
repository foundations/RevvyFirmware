/*
 * sensor_analog_button.h
 *
 * Created: 3/6/2019 9:25:36 PM
 *  Author: User
 */ 


#ifndef SENSOR_ANALOG_BUTTON_H_
#define SENSOR_ANALOG_BUTTON_H_

#include "rrrc_sensor_interface.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t ABUTTON_Init(void* hw_port);
uint32_t ABUTTON_get_value(void* hw_port, uint32_t* data, uint32_t max_size);
void ABUTTON_Thread(void* hw_port);
void ABUTTON_adc_callback(void* hw_port, uint32_t data);

static sensor_lib_entry_t sensor_analog_button =
{
	.type_id = SENSOR_ANALOG_SWITCH,
	.name = "ABUTTON",
	.SensorInit = ABUTTON_Init,
	.SensorDeInit = NULL,
	.sensor_get_values = ABUTTON_get_value,
	.sensor_thread = ABUTTON_Thread,
	.write_data = NULL,
	.read_data = NULL,

	//callback from sensor port
	.timer_callback = NULL,
	.adc_callback = ABUTTON_adc_callback,
	.gpio0_callback = NULL,
	.gpio1_callback = NULL,
};

typedef struct _abutton_data_t
{
	uint32_t state;
}abutton_data_t, *p_abutton_data_t;


#ifdef __cplusplus
}
#endif



#endif /* SENSOR_ANALOG_BUTTON_H_ */
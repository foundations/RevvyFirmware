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

sensor_lib_entry_t sensor_analog_button;

typedef struct _abutton_data_t
{
	uint32_t state;
}abutton_data_t, *p_abutton_data_t;

#ifdef __cplusplus
}
#endif



#endif /* SENSOR_ANALOG_BUTTON_H_ */
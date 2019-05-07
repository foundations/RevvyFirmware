

#ifndef _SENSOR_BUTTON_H_
#define _SENSOR_BUTTON_H_


#include "rrrc_sensor_interface.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

sensor_lib_entry_t sensor_button;

typedef struct _button_data_t
{
	uint32_t state;
}button_data_t, *p_button_data_t;

#ifdef __cplusplus
}
#endif

#endif //_SENSOR_BUTTON_H_

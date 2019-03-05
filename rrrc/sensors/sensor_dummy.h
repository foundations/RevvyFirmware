

#ifndef _SENSOR_DUMMY_H_
#define _SENSOR_DUMMY_H_

#include "rrrc_sensor_interface.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t SENSOR_DUMMY_Init(void* hw_port);
void SENSOR_DUMMY_Thread(void* hw_port);

static sensor_lib_entry_t sensor_dummy =
{
    .type_id = SENSOR_NOT_SET,
    .name = "NO_SET",
    .SensorInit = SENSOR_DUMMY_Init,
    .sensor_thread = SENSOR_DUMMY_Thread,

    //callback from sensor port
    .timer_callback = NULL,
    .adc_callback = NULL,
    .gpio0_callback = NULL,
    .gpio1_callback = NULL,
};

#ifdef __cplusplus
}
#endif

#endif //_SENSOR_DUMMY_H_

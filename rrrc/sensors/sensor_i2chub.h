/*
 * sensor_i2chub.h
 *
 * Created: 10-Mar-19 02:54:42
 *  Author: vmyakovlev
 */ 


#ifndef SENSOR_I2CHUB_H_
#define SENSOR_I2CHUB_H_

#include "rrrc_sensor_interface.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t I2CHUB_Init(void* hw_port);
void I2CHUB_Thread(void* hw_port);
uint32_t I2CHUB_write_data(void* hw_port, void* data, uint32_t size);
uint32_t I2CHUB_read_data(void* hw_port, void* data, uint32_t size);

static sensor_lib_entry_t sensor_i2chub =
{
	.type_id = SENSOR_I2CHUB,
	.name = "I2CHUB",
	.SensorInit = I2CHUB_Init,
	.SensorDeInit = NULL,
	.sensor_get_values = NULL,
	.sensor_thread = I2CHUB_Thread,
	.write_data = I2CHUB_write_data,
	.read_data = I2CHUB_read_data,

	//callback from sensor port
	.timer_callback = NULL,
	.adc_callback = NULL,
	.gpio0_callback = NULL,
	.gpio1_callback = NULL,
};

#ifdef __cplusplus
}
#endif


#endif /* SENSOR_I2CHUB_H_ */
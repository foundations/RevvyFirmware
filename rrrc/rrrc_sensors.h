/*
 * sensors.h
 *
 * Created: 1/11/2019 12:16:27 PM
 *  Author: User
 */ 


#ifndef SENSORS_H_
#define SENSORS_H_



#ifdef __cplusplus
extern "C" {
#endif

#include "sensors/rrrc_sensor_interface.h"

#define SENSOR_PORT_AMOUNT 4


uint8_t SensorPortGetPortsAmount();
uint8_t	SensorPortGetTypesAmount();
char* SensorPortGetTypeName(uint32_t idx);
uint8_t SensorPortGetTypeId(uint32_t idx);
uint32_t SensorPortGetTypes(uint8_t *data, uint32_t max_size);

int32_t SensorPortSetType(uint32_t port, sensor_type_t type);
int32_t SensorPortGetType(uint32_t port);
uint32_t SensorPortGetValues(uint32_t port, uint32_t* data); 

int32_t SensorPortInit(uint32_t port);
int32_t SensorPortDeInit(uint32_t port);

void Sensor_init(uint32_t index, sensor_type_t type);
void Sensor_deinit(uint32_t index);

#ifdef __cplusplus
}
#endif



#endif /* SENSORS_H_ */

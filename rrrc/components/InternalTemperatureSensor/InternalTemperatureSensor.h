/*
 * InternalTemperatureSensor.h
 *
 * Created: 02/05/2019 18:29:59
 *  Author: D�niel Buga
 */ 

#ifndef INTERNAL_TEMPERATURE_SENSOR_H_
#define INTERNAL_TEMPERATURE_SENSOR_H_

#include <stdint.h>

void InternalTemperatureSensor_Run_Convert(float tp, float tc, float* temp);

#endif /* INTERNAL_TEMPERATURE_SENSOR_H_ */
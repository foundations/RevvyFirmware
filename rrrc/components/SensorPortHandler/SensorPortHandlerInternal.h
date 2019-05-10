/*
 * SensorPortHandlerInternal.h
 *
 * Created: 09/05/2019 16:10:40
 *  Author: Dániel Buga
 */ 

#ifndef SENSOR_PORT_HANDLER_INTERNAL_H_
#define SENSOR_PORT_HANDLER_INTERNAL_H_

#include "SensorPortHandler.h"

void SensorPort_SetGreenLed(struct _SensorPort_t* port, bool state);
void SensorPort_SetOrangeLed(struct _SensorPort_t* port, bool state);

#endif /* SENSOR_PORT_HANDLER_INTERNAL_H_ */

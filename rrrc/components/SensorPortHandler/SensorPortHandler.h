/*
 * SensorPortHandler.h
 *
 * Created: 09/05/2019 14:03:48
 *  Author: Dániel Buga
 */ 

#ifndef SENSOR_PORT_HANDLER_H_
#define SENSOR_PORT_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../MasterCommunication/CommunicationManager.h"

Comm_Status_t SensorPortHandler_GetSensorPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t SensorPortHandler_GetSensorPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

#endif /* SENSOR_PORT_HANDLER_H_ */

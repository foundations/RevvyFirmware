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
#include <hal_gpio.h>
#include "../MasterCommunication/CommunicationManager.h"

struct _SensorPort_t;

#include "SensorPortLibraries/SensorPortLibrary.h"

typedef struct _SensorPort_t
{
    const struct _SensorLibrary_t* library;
    void* libraryData;
    const struct _SensorLibrary_t* requestedLibrary;

    uint8_t led0;
    uint8_t led1;

    uint8_t gpio0;
    uint8_t gpio1;
    uint8_t vccio;

    uint8_t adc;
} SensorPort_t;

Comm_Status_t SensorPortHandler_GetPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t SensorPortHandler_GetPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t SensorPortHandler_SetPortType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t SensorPortHandler_SetPortType_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t SensorPortHandler_SetPortConfig_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t SensorPortHandler_SetPortConfig_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t SensorPortHandler_GetSensorData_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t SensorPortHandler_GetSensorData_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

void SensorPortHandler_Run_OnInit(SensorPort_t* ports, size_t portCount);
void SensorPortHandler_Run_Update(void);
void SensorPortHandler_Run_PortUpdate(uint8_t port_idx);
uint8_t SensorPortHandler_Read_AdcData(uint8_t port_idx);

void* SensorPortHandler_Call_Allocate(size_t size);
void SensorPortHandler_Call_Free(void** ptr);

#endif /* SENSOR_PORT_HANDLER_H_ */

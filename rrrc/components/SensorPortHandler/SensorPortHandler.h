/*
 * SensorPortHandler.h
 *
 * Created: 09/05/2019 14:03:48
 *  Author: D�niel Buga
 */ 

#ifndef SENSOR_PORT_HANDLER_H_
#define SENSOR_PORT_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>
#include <hal_gpio.h>
#include "../MasterCommunication/CommunicationManager.h"

struct _SensorPort_t;

#include "hal_i2c_m_async.h"
#include "SensorPortLibraries/SensorPortLibrary.h"

#ifndef COMPONENT_TYPES_SENSOR_PORT_HANDLER_H_
#define COMPONENT_TYPES_SENSOR_PORT_HANDLER_H_


typedef struct {
    uint8_t* bytes;
    size_t count;
} ByteArray_t;

#endif

typedef struct _SensorPort_t
{
    uint8_t port_idx;
    const struct _SensorLibrary_t* library;
    void* libraryData;
    const struct _SensorLibrary_t* requestedLibrary;

    uint8_t led0;
    uint8_t led1;

    uint8_t gpio0;
    uint8_t gpio1;
    uint8_t vccio;

    void* i2c_hw;
    struct i2c_m_async_desc i2c;
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
uint8_t SensorPortHandler_Read_AdcData(uint32_t port_idx);
void SensorPort_Call_UpdatePortStatus(uint8_t port_idx, ByteArray_t status);
uint16_t SensorPortHandler_Call_ReadCurrentTicks(void);

float SensorPort_Call_ConvertTicksToMs(uint32_t ticks);
void* SensorPortHandler_Call_Allocate(size_t size);
void SensorPortHandler_Call_Free(void** ptr);

#endif /* SENSOR_PORT_HANDLER_H_ */

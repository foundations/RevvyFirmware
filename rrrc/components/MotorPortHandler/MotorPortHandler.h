/*
 * MotorPortHandler.h
 *
 * Created: 09/05/2019 14:03:48
 *  Author: D�niel Buga
 */ 

#ifndef MOTOR_PORT_HANDLER_H_
#define MOTOR_PORT_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../MasterCommunication/CommunicationManager.h"

struct _MotorPort_t;

#include "MotorPortLibraries/MotorPortLibrary.h"

#ifndef COMPONENT_TYPES_MOTOR_PORT_HANDLER_H_
#define COMPONENT_TYPES_MOTOR_PORT_HANDLER_H_


typedef struct {
    uint8_t* bytes;
    size_t count;
} ByteArray_t;

typedef enum {
    DriveRequest_RequestType_Speed,
    DriveRequest_RequestType_Position,
    DriveRequest_RequestType_RelativePosition,
    DriveRequest_RequestType_Power
} DriveRequest_RequestType_t;

typedef union {
    float speed;
    int32_t position;
    int8_t power;
} DriveRequest_RequestValue_t;

typedef struct {
    float power_limit;
    float speed_limit;
    DriveRequest_RequestType_t request_type;
    DriveRequest_RequestValue_t request;
} DriveRequest_t;

#endif

typedef struct _MotorPort_t
{
    const struct _MotorLibrary_t* library;
    void* libraryData;
    const struct _MotorLibrary_t* requestedLibrary;

    uint8_t port_idx;

    uint8_t led;

    uint8_t enc0;
    uint8_t enc1;

    uint8_t driver_idx;
    uint8_t driver_channel;
} MotorPort_t;

Comm_Status_t MotorPortHandler_GetPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_GetPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetPortType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetPortType_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetPortConfig_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetPortConfig_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetControlValue_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_GetStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

void MotorPortHandler_Run_OnInit(MotorPort_t* ports, uint8_t portCount);
void MotorPortHandler_Run_Update(void);
void MotorPortHandler_Run_PortUpdate(uint8_t port_idx);

void* MotorPortHandler_Call_Allocate(size_t size);
void MotorPortHandler_Call_Free(void** ptr);

void MotorPortHandler_Write_DriveStrength(uint32_t index, const int8_t value);
bool MotorPortHandler_Read_DriverFault(uint8_t port_idx);
void MotorPortHandler_Call_UpdatePortStatus(uint8_t port_idx, ByteArray_t status);

void MotorPortHandler_Write_DriveRequest(uint32_t port_idx, const DriveRequest_t* command);
void MotorPortHandler_Read_AppliedDriveRequest(uint32_t port_idx, DriveRequest_t* dst);

#endif /* MOTOR_PORT_HANDLER_H_ */

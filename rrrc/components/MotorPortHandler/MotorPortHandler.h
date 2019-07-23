/*
 * MotorPortHandler.h
 *
 * Created: 09/05/2019 14:03:48
 *  Author: Dániel Buga
 */ 

#ifndef MOTOR_PORT_HANDLER_H_
#define MOTOR_PORT_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../MasterCommunication/CommunicationManager.h"

struct _MotorPort_t;

#include "MotorPortLibraries/MotorPortLibrary.h"

typedef enum 
{
    MotorPort_DriveRequest_Speed,
    MotorPort_DriveRequest_Position,
    MotorPort_DriveRequest_Position_Relative,
    MotorPort_DriveRequest_Power,
} MotorPort_DriveRequestType_t;

typedef struct
{
    MotorPort_DriveRequestType_t type;
    union
    {
        int8_t pwm;
        float speed;
        int32_t position;
    } v;
    float speed_limit;
    float power_limit;
} MotorPort_DriveRequest_t;

typedef struct _MotorPort_t
{
    const struct _MotorLibrary_t* library;
    void* libraryData;
    const struct _MotorLibrary_t* requestedLibrary;

    uint8_t port_idx;

    uint8_t led;

    uint8_t enc0;
    uint8_t enc1;
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

void MotorPortHandler_Write_MotorDriveValue(uint8_t port_idx, int8_t value);
bool MotorPortHandler_Read_DriverFault(uint8_t port_idx);
void MotorPort_Write_PortState(uint8_t port_idx, uint8_t* pData, uint8_t dataSize);

void MotorPortHandler_Write_DriveRequest(uint8_t port_idx, const MotorPort_DriveRequest_t* command);
void MotorPortHandler_Read_DriveRequest(uint8_t port_idx, MotorPort_DriveRequest_t* dst);

#endif /* MOTOR_PORT_HANDLER_H_ */

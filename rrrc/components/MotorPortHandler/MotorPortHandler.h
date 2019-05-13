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
#include <hal_gpio.h>
#include <hal_timer.h>
#include "../MasterCommunication/CommunicationManager.h"

struct _MotorPort_t;

#include "MotorPortLibraries/MotorPortLibrary.h"

typedef struct _MotorPort_t
{
    const struct _MotorLibrary_t* library;
    void* libraryData;
    const struct _MotorLibrary_t* requestedLibrary;

    uint8_t port_idx;

    uint8_t led0;
    uint8_t led1;

    uint8_t enc0;
    uint8_t enc1;
    struct timer_descriptor* enc0_timer;
    enum TIMER_CB_FUNC_TUPE enc0_timer_event;
    struct timer_descriptor* enc1_timer;
    enum TIMER_CB_FUNC_TUPE enc1_timer_event;
} MotorPort_t;

Comm_Status_t MotorPortHandler_GetPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_GetPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetPortType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetPortType_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetPortConfig_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetPortConfig_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_SetControlValue_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t MotorPortHandler_GetPosition_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

void MotorPortHandler_Run_OnInit(MotorPort_t* ports, uint8_t portCount);
void MotorPortHandler_Run_Update(void);
void MotorPortHandler_Run_PortUpdate(uint8_t port_idx);

void* MotorPortHandler_Call_Allocate(size_t size);
void MotorPortHandler_Call_Free(void** ptr);

void MotorPortHandler_Write_MotorDriveValue(uint8_t port_idx, int8_t value);
bool MotorPortHandler_Read_DriverFault(uint8_t port_idx);

#endif /* MOTOR_PORT_HANDLER_H_ */

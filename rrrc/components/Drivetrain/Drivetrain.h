/*
 * Drivetrain.h
 *
 * Created: 2019. 05. 30. 14:41:19
 *  Author: bugad
 */ 


#ifndef COMPONENTS_DRIVETRAIN_H_
#define COMPONENTS_DRIVETRAIN_H_

#include <stdint.h>
#include <stdbool.h>
#include "../MasterCommunication/CommunicationManager.h"

typedef enum {
    DriveTrain_Request_Position,
    DriveTrain_Request_Speed,
    DriveTrain_Request_Power
} DriveTrain_RequestType_t;

typedef struct {
    DriveTrain_RequestType_t type;
    union 
    {
        float speed;
        int32_t position;
        int8_t power;
    };
    float power_limit;
    float speed_limit;
} DriveTrain_DriveRequest_t;

Comm_Status_t DriveTrain_Set_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t DriveTrain_SetControlValue_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

void DriveTrain_Run_OnInit(void);

void DriveTrain_Write_MotorAssigned(uint8_t port_idx, bool isAssigned);
void DriveTrain_Write_DriveRequest(uint8_t port_idx, const DriveTrain_DriveRequest_t* command);

#endif /* COMPONENTS_DRIVETRAIN_H_ */

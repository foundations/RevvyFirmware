#ifndef COMPONENT_DRIVE_TRAIN_H_
#define COMPONENT_DRIVE_TRAIN_H_

#ifndef COMPONENT_TYPES_DRIVE_TRAIN_H_
#define COMPONENT_TYPES_DRIVE_TRAIN_H_

#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include "../MasterCommunication/CommunicationManager.h"

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

#endif /* COMPONENT_TYPES_DRIVE_TRAIN_H_ */

Comm_Status_t DriveTrain_Set_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t DriveTrain_SetControlValue_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t DriveTrain_TurnCommand_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

void DriveTrain_Run_OnInit(void);
void DriveTrain_Run_Update(void);
void DriveTrain_Write_DriveRequest(uint32_t index, const DriveRequest_t* value);
void DriveTrain_Write_MotorUsed(uint32_t index, const bool value);
float DriveTrain_Read_YawAngle(void);

#endif /* COMPONENT_DRIVE_TRAIN_H_ */

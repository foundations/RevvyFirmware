#ifndef COMPONENT_DRIVE_REQUEST_MULTIPLEXER_H_
#define COMPONENT_DRIVE_REQUEST_MULTIPLEXER_H_

#ifndef COMPONENT_TYPES_DRIVE_REQUEST_MULTIPLEXER_H_
#define COMPONENT_TYPES_DRIVE_REQUEST_MULTIPLEXER_H_

#include <float.h>
#include <stdbool.h>
#include <stdint.h>


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

#endif /* COMPONENT_TYPES_DRIVE_REQUEST_MULTIPLEXER_H_ */

void DriveRequestMultiplexer_Run_OnUpdate(void);
void DriveRequestMultiplexer_Write_AppliedDriveRequest(uint32_t index, const DriveRequest_t* value);
void DriveRequestMultiplexer_Read_DrivetrainDriveRequest(uint32_t index, DriveRequest_t* value);
bool DriveRequestMultiplexer_Read_IsDrivetrainMotor(uint32_t index);
void DriveRequestMultiplexer_Read_UserDriveRequest(uint32_t index, DriveRequest_t* value);

#endif /* COMPONENT_DRIVE_REQUEST_MULTIPLEXER_H_ */

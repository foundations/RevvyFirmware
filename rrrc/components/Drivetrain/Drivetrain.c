/*
 * Drivetrain.c
 *
 * Created: 2019. 05. 30. 15:32:48
 *  Author: D�niel Buga
 */ 

#include "Drivetrain.h"
#include "utils.h"
#include "utils/converter.h"

#define DRIVETRAIN_CONTROL_GO_POS ((uint8_t) 0u)
#define DRIVETRAIN_CONTROL_GO_SPD ((uint8_t) 1u)
#define DRIVETRAIN_CONTROL_STOP   ((uint8_t) 2u)

typedef enum {
    Motor_NotAssigned = 0u,
    Motor_Left,
    Motor_Right
} MotorAssingment_t;

static MotorAssingment_t motors[6];

static void assign_motor(uint8_t motor_idx, MotorAssingment_t assignment)
{
    if (assignment > Motor_Right)
    {
        assignment = Motor_NotAssigned;
    }
    DriveTrain_Write_MotorAssigned(motors[motor_idx], assignment != Motor_NotAssigned);
    motors[motor_idx] = assignment;
}

Comm_Status_t DriveTrain_Set_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != ARRAY_SIZE(motors))
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    for (size_t i = 0u; i < ARRAY_SIZE(motors); i++)
    {
        assign_motor(i, commandPayload[i]);
    }

    return Comm_Status_Ok;
}

Comm_Status_t DriveTrain_SetControlValue_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize < 1u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    else
    {
        DriveTrain_DriveRequest_t leftDriveRequest;
        DriveTrain_DriveRequest_t rightDriveRequest;

        switch (commandPayload[0])
        {
            case DRIVETRAIN_CONTROL_GO_POS:
                if (commandSize != 18u)
                {
                    return Comm_Status_Error_PayloadLengthError;
                }
                leftDriveRequest.type  = DriveTrain_Request_Position;
                rightDriveRequest.type = DriveTrain_Request_Position;
                
                leftDriveRequest.position  = get_int32(&commandPayload[1]);
                rightDriveRequest.position = get_int32(&commandPayload[5]);

                leftDriveRequest.speed_limit  = get_float(&commandPayload[9]);
                rightDriveRequest.speed_limit = get_float(&commandPayload[13]);

                leftDriveRequest.power_limit  = (float) commandPayload[17];
                rightDriveRequest.power_limit = (float) commandPayload[17];
                break;

            case DRIVETRAIN_CONTROL_GO_SPD:
                if (commandSize != 10u)
                {
                    return Comm_Status_Error_PayloadLengthError;
                }
                leftDriveRequest.type = DriveTrain_Request_Speed;
                rightDriveRequest.type = DriveTrain_Request_Speed;
                
                leftDriveRequest.speed  = get_float(&commandPayload[1]);
                rightDriveRequest.speed = get_float(&commandPayload[5]);

                leftDriveRequest.power_limit  = (float) commandPayload[9];
                rightDriveRequest.power_limit = (float) commandPayload[9];
                break;

            case DRIVETRAIN_CONTROL_STOP:
                if (commandSize != 1u)
                {
                    return Comm_Status_Error_PayloadLengthError;
                }
                leftDriveRequest.type = DriveTrain_Request_Power;
                rightDriveRequest.type = DriveTrain_Request_Power;
                
                leftDriveRequest.power = 0;
                rightDriveRequest.power = 0;
                break;
        }

        for (size_t i = 0u; i < ARRAY_SIZE(motors); i++)
        {
            switch (motors[i])
            {
                case Motor_Left:
                    DriveTrain_Write_DriveRequest(i, &leftDriveRequest);
                    break;

                case Motor_Right:
                    DriveTrain_Write_DriveRequest(i, &rightDriveRequest);
                    break;

                default:
                    break;
            }
        }
    }

    return Comm_Status_Ok;
}

void DriveTrain_Run_OnInit(void)
{
    for (size_t i = 0u; i < ARRAY_SIZE(motors); i++)
    {
        assign_motor(i, Motor_NotAssigned);
    }
}

__attribute__((weak))
void DriveTrain_Write_MotorAssigned(uint8_t port_idx, bool isAssigned)
{
    /* nothing to do */
}

__attribute__((weak))
void DriveTrain_Write_DriveRequest(uint8_t port_idx, const DriveTrain_DriveRequest_t* command)
{
    /* nothing to do */
}
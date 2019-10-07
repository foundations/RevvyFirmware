/*
 * Drivetrain.c
 *
 * Created: 2019. 05. 30. 15:32:48
 *  Author: Dániel Buga
 */ 

#include "DriveTrain.h"
#include "utils.h"
#include "utils/converter.h"
#include "controller/pid.h"

#include <math.h>

#define DRIVETRAIN_CONTROL_GO_POS ((uint8_t) 0u)
#define DRIVETRAIN_CONTROL_GO_SPD ((uint8_t) 1u)
#define DRIVETRAIN_CONTROL_STOP   ((uint8_t) 2u)

#define DRIVETRAIN_DIFFERENTIAL ((uint8_t) 0u)

typedef enum {
    Motor_NotAssigned = 0u,
    Motor_Left,
    Motor_Right
} MotorAssingment_t;

static MotorAssingment_t motors[6];
static uint8_t driveTrainType;

static bool turnCommandActive;
static bool turnStartAngleRead;

static int32_t turnAngle;
static float turnTargetAngle;
static float turnPowerLimit;
static PID_t yawAngleController;
static float errorFilterValue;
static const float errorFilterAlpha = 0.95f;

static void assign_motor(uint8_t motor_idx, MotorAssingment_t assignment)
{
    if (assignment > Motor_Right)
    {
        assignment = Motor_NotAssigned;
    }
    DriveTrain_Write_MotorAssigned(motor_idx, assignment != Motor_NotAssigned);
    motors[motor_idx] = assignment;
}

static void _apply(const DriveTrain_DriveRequest_t* left, const DriveTrain_DriveRequest_t* right)
{
    for (size_t i = 0u; i < ARRAY_SIZE(motors); i++)
    {
        switch (motors[i])
        {
            case Motor_Left:
                DriveTrain_Write_DriveRequest(i, left);
                break;

            case Motor_Right:
                DriveTrain_Write_DriveRequest(i, right);
                break;

            default:
                break;
        }
    }
}

static Comm_Status_t differentialDrivetrain(const uint8_t* commandPayload, uint8_t commandSize)
{
    DriveTrain_DriveRequest_t leftDriveRequest;
    DriveTrain_DriveRequest_t rightDriveRequest;

    leftDriveRequest.speed_limit  = 0.0f;
    rightDriveRequest.speed_limit = 0.0f;

    leftDriveRequest.power_limit  = 0.0f;
    rightDriveRequest.power_limit = 0.0f;

    switch (commandPayload[0])
    {
        case DRIVETRAIN_CONTROL_GO_POS:
            if (commandSize != 18u)
            {
                return Comm_Status_Error_PayloadLengthError;
            }
            leftDriveRequest.type  = DriveTrain_Request_Position;
            rightDriveRequest.type = DriveTrain_Request_Position;
                
            leftDriveRequest.v.position  = get_int32(&commandPayload[1]);
            rightDriveRequest.v.position = get_int32(&commandPayload[5]);

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

            leftDriveRequest.v.speed  = get_float(&commandPayload[1]);
            rightDriveRequest.v.speed = get_float(&commandPayload[5]);

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

            leftDriveRequest.v.power = 0;
            rightDriveRequest.v.power = 0;
            break;
    }

    turnCommandActive = false;
    _apply(&leftDriveRequest, &rightDriveRequest);

    return Comm_Status_Ok;
}

Comm_Status_t DriveTrain_Set_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize != ARRAY_SIZE(motors) + 1u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    driveTrainType = commandPayload[0];

    for (size_t i = 0u; i < ARRAY_SIZE(motors); i++)
    {
        assign_motor(i, commandPayload[1u + i]);
    }

    return Comm_Status_Ok;
}

Comm_Status_t DriveTrain_SetControlValue_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize < 1u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    else
    {
        Comm_Status_t status = Comm_Status_Error_CommandError;
        switch (driveTrainType)
        {
            case DRIVETRAIN_DIFFERENTIAL:
                status = differentialDrivetrain(commandPayload, commandSize);
                break;

            default:
                break;
        }
        return status;
    }
}

Comm_Status_t DriveTrain_TurnCommand_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;
    
    if (commandSize != 9u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    else
    {
        turnCommandActive = true;
        turnStartAngleRead = false;
        
        /* +ve number is CCW angle */
        turnAngle = get_int32(&commandPayload[0]);
        float wheelSpeed = get_float(&commandPayload[4]);
        turnPowerLimit = (float) commandPayload[8];
        
        yawAngleController.config.LowerLimit = -wheelSpeed;
        yawAngleController.config.UpperLimit = wheelSpeed;

        errorFilterValue = turnAngle;
    }
    
    return Comm_Status_Ok;
}

void DriveTrain_Run_OnInit(void)
{
    for (size_t i = 0u; i < ARRAY_SIZE(motors); i++)
    {
        assign_motor(i, Motor_NotAssigned);
    }

    pid_initialize(&yawAngleController);
    yawAngleController.config.P = 10.0f;
    yawAngleController.config.I = 0.0f;
    yawAngleController.config.D = 0.0f;
}

void DriveTrain_Run_Update(void)
{
    if (turnCommandActive)
    {
        /* +ve number is CCW angle */
        float currentAngle = DriveTrain_Read_YawAngle();
        if (!isnan(currentAngle))
        {
            if (!turnStartAngleRead)
            {
                turnStartAngleRead = true;
                turnTargetAngle = currentAngle + turnAngle;
            }

            errorFilterValue = errorFilterValue * errorFilterAlpha + (turnTargetAngle - currentAngle) * (1.0f - errorFilterAlpha);
            float u = pid_update(&yawAngleController, turnTargetAngle, currentAngle);

            DriveTrain_DriveRequest_t leftDriveRequest;
            DriveTrain_DriveRequest_t rightDriveRequest;

            leftDriveRequest.speed_limit  = 0.0f;
            rightDriveRequest.speed_limit  = 0.0f;

            leftDriveRequest.power_limit  = 0.0f;
            rightDriveRequest.power_limit  = 0.0f;

            if (fabsf(u) < 10.0f && fabsf(errorFilterValue) < 1.0f)
            {
                turnCommandActive = false;
                leftDriveRequest.type = DriveTrain_Request_Power;
                rightDriveRequest.type = DriveTrain_Request_Power;

                leftDriveRequest.v.power = 0;
                rightDriveRequest.v.power = 0;
            }
            else
            {
                leftDriveRequest.type = DriveTrain_Request_Speed;
                rightDriveRequest.type = DriveTrain_Request_Speed;

                /* +ve u means CCW turning */
                leftDriveRequest.v.speed = -u;
                rightDriveRequest.v.speed = u;

                leftDriveRequest.power_limit  = turnPowerLimit;
                rightDriveRequest.power_limit = turnPowerLimit;
            }

            _apply(&leftDriveRequest, &rightDriveRequest);
        }
    }
}

__attribute__((weak))
float DriveTrain_Read_YawAngle(void)
{
    return 0.0f;
}

__attribute__((weak))
void DriveTrain_Write_MotorAssigned(uint8_t port_idx, bool isAssigned)
{
    (void) port_idx;
    (void) isAssigned;
    /* nothing to do */
}

__attribute__((weak))
void DriveTrain_Write_DriveRequest(uint8_t port_idx, const DriveTrain_DriveRequest_t* command)
{
    (void) port_idx;
    (void) command;
    /* nothing to do */
}

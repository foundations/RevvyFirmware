#include "Differential.h"

#include "controller/pid.h"

#include "utils.h"
#include "utils/converter.h"
#include <math.h>

#define MAX_INPUT_DV    (10.0f)

typedef enum {
    Motor_NotAssigned = 0u,
    Motor_Left,
    Motor_Right
} MotorAssignment_t;

static MotorAssignment_t motors[6];

static bool turnCommandActive;
static bool turnStartAngleRead;

static int32_t turnAngle;
static float turnTargetAngle;
static float turnPowerLimit;
static PID_t yawAngleController;
static float errorFilterValue;
static const float errorFilterAlpha = 0.95f;

static uint8_t last_command;

static float last_left_speed;
static float last_right_speed;

static DriveTrainLibraryStatus_t command_go_to_pos(const ByteArray_t params);
static DriveTrainLibraryStatus_t command_go_speed(const ByteArray_t params);
static DriveTrainLibraryStatus_t command_stop(const ByteArray_t params);
static DriveTrainLibraryStatus_t command_turn(const ByteArray_t params);

static void turn(void);

typedef DriveTrainLibraryStatus_t (*differential_drivetrain_command)(const ByteArray_t params);

static void (*update_function)(void);

static const differential_drivetrain_command commands[] = {
    &command_go_to_pos,
    &command_go_speed,
    &command_stop,
    &command_turn
};

static float _ramp(float requested, float last, float increment)
{
    if (requested < last)
    {
        if (requested < last - increment)
        {
            return last - increment;
        }
        else
        {
            return requested;
        }
    }
    else
    {
        if (requested > last + increment)
        {
            return last + increment;
        }
        else
        {
            return requested;
        }
    }
}

static void _apply(const DriveRequest_t* left, const DriveRequest_t* right)
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

static void assign_motor(uint8_t motor_idx, MotorAssignment_t assignment)
{
    DriveTrain_Write_MotorUsed(motor_idx, assignment != Motor_NotAssigned);
    motors[motor_idx] = assignment;
}

static DriveTrainLibraryStatus_t command_go_to_pos(const ByteArray_t params)
{
    if (params.count != 17u)
    {
        return DriveTrainLibraryStatus_InputError_Length;
    }

    DriveRequest_t leftDriveRequest;
    DriveRequest_t rightDriveRequest;

    leftDriveRequest.request_type  = DriveRequest_RequestType_Position;
    rightDriveRequest.request_type = DriveRequest_RequestType_Position;

    leftDriveRequest.request.position  = get_int32(&params.bytes[0]);
    rightDriveRequest.request.position = get_int32(&params.bytes[4]);

    leftDriveRequest.speed_limit  = get_float(&params.bytes[8]);
    rightDriveRequest.speed_limit = get_float(&params.bytes[12]);

    leftDriveRequest.power_limit  = (float) params.bytes[16];
    rightDriveRequest.power_limit = (float) params.bytes[16];

    update_function = NULL;
    _apply(&leftDriveRequest, &rightDriveRequest);

    return DriveTrainLibraryStatus_Ok;
}

static DriveTrainLibraryStatus_t command_go_speed(const ByteArray_t params)
{
    if (params.count != 9u)
    {
        return DriveTrainLibraryStatus_InputError_Length;
    }

    DriveRequest_t leftDriveRequest;
    DriveRequest_t rightDriveRequest;

    leftDriveRequest.request_type  = DriveRequest_RequestType_Speed;
    rightDriveRequest.request_type = DriveRequest_RequestType_Speed;

    float requested_left_speed = get_int32(&params.bytes[0]);
    float requested_right_speed = get_int32(&params.bytes[4]);

    leftDriveRequest.speed_limit  = 0.0f;
    rightDriveRequest.speed_limit = 0.0f;

    leftDriveRequest.power_limit  = (float) params.bytes[8];
    rightDriveRequest.power_limit = (float) params.bytes[8];

    update_function = NULL;
    
    if (last_command != 1u)
    {
        last_left_speed = 0.0f;
        last_right_speed = 0.0f;
    }    
    
    leftDriveRequest.request.speed = _ramp(requested_left_speed, last_left_speed, MAX_INPUT_DV);
    rightDriveRequest.request.speed = _ramp(requested_right_speed, last_right_speed, MAX_INPUT_DV);
    
    _apply(&leftDriveRequest, &rightDriveRequest);

    return DriveTrainLibraryStatus_Ok;
}

static DriveTrainLibraryStatus_t command_stop(const ByteArray_t params)
{
    if (params.count != 0u)
    {
        return DriveTrainLibraryStatus_InputError_Length;
    }

    DriveRequest_t leftDriveRequest;
    DriveRequest_t rightDriveRequest;

    leftDriveRequest.request_type  = DriveRequest_RequestType_Power;
    rightDriveRequest.request_type = DriveRequest_RequestType_Power;

    leftDriveRequest.request.power  = 0.0f;
    rightDriveRequest.request.power = 0.0f;

    leftDriveRequest.speed_limit  = 0.0f;
    rightDriveRequest.speed_limit = 0.0f;

    leftDriveRequest.power_limit  = 0.0f;
    rightDriveRequest.power_limit = 0.0f;

    update_function = NULL;
    _apply(&leftDriveRequest, &rightDriveRequest);

    return DriveTrainLibraryStatus_Ok;
}

static DriveTrainLibraryStatus_t command_turn(const ByteArray_t params)
{
    if (params.count != 18u)
    {
        return DriveTrainLibraryStatus_InputError_Length;
    }

    turnCommandActive = true;
    turnStartAngleRead = false;

    /* +ve number is CCW angle */
    turnAngle = get_int32(&params.bytes[0]);
    float wheelSpeed = get_float(&params.bytes[4]);
    turnPowerLimit = (float) params.bytes[8];

    yawAngleController.config.LowerLimit = -wheelSpeed;
    yawAngleController.config.UpperLimit = wheelSpeed;

    errorFilterValue = turnAngle;

    update_function = &turn;

    return DriveTrainLibraryStatus_Ok;
}

static void turn(void)
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

        DriveRequest_t leftDriveRequest;
        DriveRequest_t rightDriveRequest;

        leftDriveRequest.speed_limit  = 0.0f;
        rightDriveRequest.speed_limit  = 0.0f;

        leftDriveRequest.power_limit  = 0.0f;
        rightDriveRequest.power_limit  = 0.0f;

        if (fabsf(u) < 10.0f && fabsf(errorFilterValue) < 1.0f)
        {
            turnCommandActive = false;
            leftDriveRequest.request_type = DriveRequest_RequestType_Power;
            rightDriveRequest.request_type = DriveRequest_RequestType_Power;

            leftDriveRequest.request.power = 0;
            rightDriveRequest.request.power = 0;
            
            /* turn has ended, stop requesting recall */
            update_function = NULL;
        }
        else
        {
            leftDriveRequest.request_type = DriveRequest_RequestType_Speed;
            rightDriveRequest.request_type = DriveRequest_RequestType_Speed;

            /* +ve u means CCW turning */
            leftDriveRequest.request.speed = -u;
            rightDriveRequest.request.speed = u;

            leftDriveRequest.power_limit  = turnPowerLimit;
            rightDriveRequest.power_limit = turnPowerLimit;
        }

        _apply(&leftDriveRequest, &rightDriveRequest);
    }
}

static DriveTrainLibraryStatus_t Differential_Init(const ByteArray_t config)
{
    if (!(1 <= config.count && config.count <= 6))
    {
        return DriveTrainLibraryStatus_InputError_Length;
    }

    last_command = 0xFFu;

    bool error = false;
    for (uint8_t i = 0u; i < 6u; i++)
    {
        if (i < config.count)
        {
            MotorAssignment_t assignment = config.bytes[i];

            if (assignment > Motor_Right)
            {
                error = true;
                break;
            }
            assign_motor(i, assignment);
        }
        else
        {
            assign_motor(i, Motor_NotAssigned);
        }
    }

    if (error)
    {
        for (uint8_t i = 0u; i < 6u; i++)
        {
            assign_motor(i, Motor_NotAssigned);
        }

        return DriveTrainLibraryStatus_InputError_Value;
    }
    else
    {
        return DriveTrainLibraryStatus_Ok;
    }
}

static DriveTrainLibraryStatus_t Differential_DeInit(void)
{
    for (uint8_t i = 0u; i < 6u; i++)
    {
        assign_motor(i, Motor_NotAssigned);
    }
    
    last_command = 0xFFu;

    return DriveTrainLibraryStatus_Ok;
}

static DriveTrainLibraryStatus_t Differential_Update(void)
{
    if (update_function)
    {
        update_function();
    }

    return DriveTrainLibraryStatus_Ok;
}

static DriveTrainLibraryStatus_t Differential_Command(const ByteArray_t config)
{
    if (config.count == 0u)
    {
        return DriveTrainLibraryStatus_InputError_Length;
    }

    uint8_t commandIdx = config.bytes[0];
    if (commandIdx >= ARRAY_SIZE(commands))
    {
        return DriveTrainLibraryStatus_InputError_Value;
    }

    const ByteArray_t commandParams = {
        .bytes = &config.bytes[1],
        .count = config.count - 1u
    };

    DriveTrainLibraryStatus_t result = commands[commandIdx](commandParams);
    
    last_command = commandIdx;
    
    return result;
}

const DriveTrainLibrary_t library_differential = {
    .name = "Differential",

    .Init = &Differential_Init,
    .DeInit = &Differential_DeInit,
    .Update = &Differential_Update,

    .Command = &Differential_Command
};

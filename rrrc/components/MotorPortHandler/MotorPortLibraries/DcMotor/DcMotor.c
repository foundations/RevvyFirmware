/*
 * DcMotor.c
 *
 * Created: 2019. 05. 24. 15:03:34
 *  Author: D�niel Buga
 */ 
 
#include "DcMotor.h"

#include "utils/converter.h"
#include "utils/functions.h"
#include "controller/pid.h"
#include "utils.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <math.h>

#define MOTOR_CONTROL_PWM      ((uint8_t) 0u)
#define MOTOR_CONTROL_SPEED    ((uint8_t) 1u)
#define MOTOR_CONTROL_POSITION ((uint8_t) 2u)

#define DRIVE_CONTSTRAINED_POWER    ((uint8_t) 0u)
#define DRIVE_CONTSTRAINED_SPEED    ((uint8_t) 1u)

typedef struct 
{
    bool configured;

    /* configuration */
    int32_t positionLimitMax;
    int32_t positionLimitMin;
    PID_t positionController;
    PID_t speedController;
    uint8_t configuration[48];

    /* last status */
    int32_t lastPosition;
    int32_t positionBuffer[1];
    uint8_t positionBufferIdx;
    uint8_t positionBufferCount;

    /* current status */
    int32_t position;
    float speed;
    int8_t pwm;
} MotorLibrary_Dc_Data_t;

MotorLibraryStatus_t DcMotor_Init(MotorPort_t* motorPort)
{
    MotorLibrary_Dc_Data_t* libdata = MotorPortHandler_Call_Allocate(sizeof(MotorLibrary_Dc_Data_t));

    libdata->configured = false;

    motorPort->libraryData = libdata;
    MotorPort_SetGreenLed(motorPort, true);
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t DcMotor_DeInit(MotorPort_t* motorPort)
{
    MotorPort_SetDriveValue(motorPort, 0);
    MotorPort_SetGreenLed(motorPort, false);
    MotorPortHandler_Call_Free(&motorPort->libraryData);
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t DcMotor_Update(MotorPort_t* motorPort)
{
    MotorLibrary_Dc_Data_t* libdata = (MotorLibrary_Dc_Data_t*) motorPort->libraryData;

    if (libdata->configured)
    {
        /* Calculate current speed */
        /* Record last N position difference values */
        int32_t posDiff = libdata->position - libdata->lastPosition;
        libdata->lastPosition = libdata->position;

        libdata->positionBuffer[libdata->positionBufferIdx] = posDiff;
        libdata->positionBufferIdx = (libdata->positionBufferIdx + 1u) % ARRAY_SIZE(libdata->positionBuffer);

        if (libdata->positionBufferCount < ARRAY_SIZE(libdata->positionBuffer))
        {
            libdata->positionBufferCount++;
        }
            
        /* Get the average */
        int32_t sumPosDiff = 0;
        for (size_t i = 0u; i < ARRAY_SIZE(libdata->positionBuffer); i++)
        {
            sumPosDiff += libdata->positionBuffer[i];
        }
        float avgPosDiff = sumPosDiff / (float) libdata->positionBufferCount;
 
        /* Calculate speed - 20ms cycle time */
        float dt = 0.02f;
        float speed = avgPosDiff / dt;

        libdata->speed = speed;

        MotorPort_DriveRequest_t driveRequest;
        MotorPortHandler_Read_DriveRequest(motorPort->port_idx, &driveRequest);

        /* set up limits */
        if (driveRequest.speed_limit == 0.0f)
        {
            libdata->positionController.config.LowerLimit = get_float(&libdata->configuration[20]);
            libdata->positionController.config.UpperLimit = get_float(&libdata->configuration[24]);
        }
        else
        {
            libdata->positionController.config.LowerLimit = -driveRequest.speed_limit;
            libdata->positionController.config.UpperLimit = driveRequest.speed_limit;
        }

        if (driveRequest.power_limit == 0.0f)
        {
            libdata->speedController.config.LowerLimit = get_float(&libdata->configuration[40]);
            libdata->speedController.config.UpperLimit = get_float(&libdata->configuration[44]);
        }
        else
        {
            libdata->speedController.config.LowerLimit = -driveRequest.power_limit;
            libdata->speedController.config.UpperLimit = driveRequest.power_limit;
        }

        /* control the motor */
        int8_t pwm = 0;
        if (driveRequest.type == MotorPort_DriveRequest_Power)
        {
            /* pwm value directly given */
            pwm = driveRequest.pwm;
        }
        else
        {
            float reqSpeed = 0.0f;
            if (driveRequest.type == MotorPort_DriveRequest_Speed)
            {
                /* requested speed is given */
                reqSpeed = driveRequest.speed;
            }
            else
            {
                /* calculate speed to reach requested position */
                int32_t reqPosition = driveRequest.position;
                if (libdata->positionLimitMin < libdata->positionLimitMax)
                {
                    reqPosition = constrain_int32(reqPosition, libdata->positionLimitMin, libdata->positionLimitMax);
                }
                reqSpeed = pid_update(&libdata->positionController, reqPosition, libdata->position);
            }

            /* calculate drive value to control speed */
            float u = pid_update(&libdata->speedController, reqSpeed, libdata->speed);
            pwm = (int8_t) lroundf(u);
        }
        
        libdata->pwm = pwm;
        MotorPort_SetDriveValue(motorPort, pwm);
    }

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t DcMotor_Gpio0Callback(MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state)
{
    MotorLibrary_Dc_Data_t* libdata = (MotorLibrary_Dc_Data_t*) motorPort->libraryData;
    
    int32_t delta = 0;
    if (pin0state)
    {
        /* rising edge */
        if (pin1state)
        {
            delta = -1;
        }
        else
        {
            delta = 1;
        }
    }
    else
    {
        /* falling edge */
        if (pin1state)
        {
            delta = 1;
        }
        else
        {
            delta = -1;
        }
    }
    libdata->position += delta;

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t DcMotor_Gpio1Callback(MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state)
{
    MotorLibrary_Dc_Data_t* libdata = (MotorLibrary_Dc_Data_t*) motorPort->libraryData;
    
    int32_t delta = 0;
    if (pin0state)
    {
        /* rising edge */
        if (pin1state)
        {
            delta = 1;
        }
        else
        {
            delta = -1;
        }
    }
    else
    {
        /* falling edge */
        if (pin1state)
        {
            delta = -1;
        }
        else
        {
            delta = 1;
        }
    }
    libdata->position += delta;

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t DcMotor_SetConfig(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    MotorLibrary_Dc_Data_t* libdata = (MotorLibrary_Dc_Data_t*) motorPort->libraryData;
    if (size != sizeof(libdata->configuration))
    {
        return MotorLibraryStatus_InputError;
    }

    portENTER_CRITICAL();
    libdata->configured = false;
    memcpy(&libdata->configuration[0], data, size);
    portEXIT_CRITICAL();

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t DcMotor_UpdateConfiguration(MotorPort_t* motorPort)
{
    MotorLibrary_Dc_Data_t* libdata = (MotorLibrary_Dc_Data_t*) motorPort->libraryData;

    pid_initialize(&libdata->positionController);
    libdata->positionLimitMin = get_int32(&libdata->configuration[0]);
    libdata->positionLimitMax = get_int32(&libdata->configuration[4]);
    libdata->positionController.config.P = get_float(&libdata->configuration[8]);
    libdata->positionController.config.I = get_float(&libdata->configuration[12]);
    libdata->positionController.config.D = get_float(&libdata->configuration[16]);
    libdata->positionController.config.LowerLimit = get_float(&libdata->configuration[20]);
    libdata->positionController.config.UpperLimit = get_float(&libdata->configuration[24]);
    
    pid_initialize(&libdata->speedController);
    libdata->speedController.config.P = get_float(&libdata->configuration[28]);
    libdata->speedController.config.I = get_float(&libdata->configuration[32]);
    libdata->speedController.config.D = get_float(&libdata->configuration[36]);
    libdata->speedController.config.LowerLimit = get_float(&libdata->configuration[40]);
    libdata->speedController.config.UpperLimit = get_float(&libdata->configuration[44]);

    libdata->lastPosition = 0;
    libdata->position = 0;
    libdata->positionBufferIdx = 0u;
    libdata->positionBufferCount = 0u;
    memset(libdata->positionBuffer, 0, sizeof(libdata->positionBuffer));

    libdata->speed = 0.0f;
    libdata->pwm = 0;

    static const MotorPort_DriveRequest_t defaultDriveRequest = {
        .type = MotorPort_DriveRequest_Power,
        .pwm = 0,
        .speed_limit = 0.0f,
        .power_limit = 0.0f
    };

    MotorPortHandler_Write_DriveRequest(motorPort->port_idx, &defaultDriveRequest);
    
    libdata->configured = true;

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t DcMotor_GetStatus(MotorPort_t* motorPort, uint8_t* data, uint8_t* dataSize)
{
    MotorLibrary_Dc_Data_t* libdata = (MotorLibrary_Dc_Data_t*) motorPort->libraryData;
    
    portENTER_CRITICAL();
    memcpy(data, &libdata->position, sizeof(int32_t));
    memcpy(&data[sizeof(int32_t)], &libdata->speed, sizeof(float));
    data[sizeof(float) + sizeof(int32_t)] = libdata->pwm;
    portEXIT_CRITICAL();

    *dataSize = sizeof(float) + sizeof(int32_t) + sizeof(int8_t);

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t DcMotor_SetControlReference(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    if (size == 0u)
    {
        return MotorLibraryStatus_InputError;
    }
    else
    {
        MotorPort_DriveRequest_t driveRequest = {
            .type = MotorPort_DriveRequest_Power,
            .pwm = 0,
            .speed_limit = 0.0f,
            .power_limit = 0.0f
        };
        switch (data[0])
        {
            case MOTOR_CONTROL_PWM:
                if (size != 2u)
                {
                    return MotorLibraryStatus_InputError;
                }
                
                driveRequest.type = MotorPort_DriveRequest_Power;
                driveRequest.pwm = data[1];
                break;

            case MOTOR_CONTROL_SPEED:
                if (size != 5u)
                {
                    if (size == 9u)
                    {
                        /* constrained drive command - power constraint */
                        driveRequest.power_limit = get_float(&data[5]);
                    }
                    else
                    {
                        return MotorLibraryStatus_InputError;
                    }
                }
                else
                {
                    /* reset controller limits */
                    driveRequest.power_limit = 0.0f;
                    driveRequest.speed_limit = 0.0f;
                }
                
                driveRequest.type = MotorPort_DriveRequest_Speed;
                driveRequest.speed = get_float(&data[1]);
                break;

            case MOTOR_CONTROL_POSITION:
                if (size != 5u)
                {
                    if (size == 10u)
                    {
                        /* constrained drive command */
                        switch (data[5])
                        {
                            case DRIVE_CONTSTRAINED_POWER:
                                driveRequest.power_limit = get_float(&data[6]);
                                break;

                            case DRIVE_CONTSTRAINED_SPEED:
                                driveRequest.speed_limit = get_float(&data[6]);
                                break;

                            default:
                                return MotorLibraryStatus_InputError;
                        }
                    }
                    else if (size == 13u)
                    {
                        driveRequest.speed_limit = get_float(&data[6]);
                        driveRequest.power_limit = get_float(&data[10]);
                    }
                    else
                    {
                        return MotorLibraryStatus_InputError;
                    }
                }
                else
                {
                    /* reset controller limits */
                    driveRequest.power_limit = 0.0f;
                    driveRequest.speed_limit = 0.0f;
                }
                
                driveRequest.type = MotorPort_DriveRequest_Position;
                driveRequest.position = get_int32(&data[1]);
                break;

            default:
                return MotorLibraryStatus_InputError;
        }

        MotorPortHandler_Write_DriveRequest(motorPort->port_idx, &driveRequest);
    }

    return MotorLibraryStatus_Ok;
}

const MotorLibrary_t motor_library_dc = 
{
    .name                = "DcMotor",
    .Init                = &DcMotor_Init,
    .DeInit              = &DcMotor_DeInit,
    .Update              = &DcMotor_Update,
    .Gpio0Callback       = &DcMotor_Gpio0Callback,
    .Gpio1Callback       = &DcMotor_Gpio1Callback,
    .SetConfig           = &DcMotor_SetConfig,
    .UpdateConfiguration = &DcMotor_UpdateConfiguration,
    .GetStatus           = &DcMotor_GetStatus,
    .SetControlReference = &DcMotor_SetControlReference,
};

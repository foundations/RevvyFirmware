/*
 * PositionControlled.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
 */ 
#include "PositionControlled.h"

#include "utils/converter.h"
#include "controller/pid.h"

#include <string.h>
#include <math.h>

typedef struct 
{
    int32_t position;
    int32_t targetPosition;
    PID_t controller;
    uint8_t configuration[20];
} MotorLibrary_PositionControlled_Data_t;

MotorLibraryStatus_t PositionControlled_Init(MotorPort_t* motorPort)
{
    MotorLibrary_PositionControlled_Data_t* libdata = MotorPortHandler_Call_Allocate(sizeof(MotorLibrary_PositionControlled_Data_t));

    libdata->position = 0;
    libdata->targetPosition = 0;
    pid_initialize(&libdata->controller);

    motorPort->libraryData = libdata;
    MotorPort_SetGreenLed(motorPort, true);
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_DeInit(MotorPort_t* motorPort)
{
    MotorPort_SetDriveValue(motorPort, 0);
    MotorPort_SetGreenLed(motorPort, false);
    MotorPortHandler_Call_Free(&motorPort->libraryData);
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_Update(MotorPort_t* motorPort)
{
    MotorLibrary_PositionControlled_Data_t* libdata = (MotorLibrary_PositionControlled_Data_t*) motorPort->libraryData;

    float u = pid_update(&libdata->controller, libdata->targetPosition, libdata->position);
    int8_t driveValue = (int8_t) lroundf(u);
    MotorPort_SetDriveValue(motorPort, driveValue);

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_Gpio0Callback(MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state)
{
    MotorLibrary_PositionControlled_Data_t* libdata = (MotorLibrary_PositionControlled_Data_t*) motorPort->libraryData;
    
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

MotorLibraryStatus_t PositionControlled_Gpio1Callback(MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state)
{
    MotorLibrary_PositionControlled_Data_t* libdata = (MotorLibrary_PositionControlled_Data_t*) motorPort->libraryData;
    
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

MotorLibraryStatus_t PositionControlled_SetConfig(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    MotorLibrary_PositionControlled_Data_t* libdata = (MotorLibrary_PositionControlled_Data_t*) motorPort->libraryData;
    if (size != sizeof(libdata->configuration))
    {
        return MotorLibraryStatus_InputError;
    }

    memcpy(&libdata->configuration[0], data, size);

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_UpdateConfiguration(MotorPort_t* motorPort)
{
    MotorLibrary_PositionControlled_Data_t* libdata = (MotorLibrary_PositionControlled_Data_t*) motorPort->libraryData;

    pid_initialize(&libdata->controller);
    libdata->controller.config.P = get_float(&libdata->configuration[0]);
    libdata->controller.config.I = get_float(&libdata->configuration[4]);
    libdata->controller.config.D = get_float(&libdata->configuration[8]);
    libdata->controller.config.LowerLimit = get_float(&libdata->configuration[12]);
    libdata->controller.config.UpperLimit = get_float(&libdata->configuration[16]);
    
    libdata->position = 0;
    libdata->targetPosition = 0;

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_GetPosition(MotorPort_t* motorPort, int32_t* data)
{
    MotorLibrary_PositionControlled_Data_t* libdata = (MotorLibrary_PositionControlled_Data_t*) motorPort->libraryData;
    *data = libdata->position;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_SetControlReference(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    if (size != 4u)
    {
        return MotorLibraryStatus_InputError;
    }
    MotorLibrary_PositionControlled_Data_t* libdata = (MotorLibrary_PositionControlled_Data_t*) motorPort->libraryData;
    libdata->targetPosition = get_int32(data);

    return MotorLibraryStatus_Ok;
}

const MotorLibrary_t motor_library_position_controlled = 
{
    .name                = "PositionControlled",
    .Init                = &PositionControlled_Init,
    .DeInit              = &PositionControlled_DeInit,
    .Update              = &PositionControlled_Update,
    .Gpio0Callback       = &PositionControlled_Gpio0Callback,
    .Gpio1Callback       = &PositionControlled_Gpio1Callback,
    .SetConfig           = &PositionControlled_SetConfig,
    .UpdateConfiguration = &PositionControlled_UpdateConfiguration,
    .GetPosition         = &PositionControlled_GetPosition,
    .SetControlReference = &PositionControlled_SetControlReference,
};

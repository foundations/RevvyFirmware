/*
 * SpeedControlled.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
 */ 
#include "SpeedControlled.h"

#include "utils/converter.h"
#include "controller/pid.h"

#include <string.h>
#include <math.h>

typedef struct 
{
    int32_t position;
    int32_t lastPosition;
    PID_t controller;
    uint8_t configuration[20];

    float targetSpeed;

    float alpha;
    float y0;
} MotorLibrary_SpeedControlled_Data_t;

MotorLibraryStatus_t SpeedControlled_Init(MotorPort_t* motorPort)
{
    MotorLibrary_SpeedControlled_Data_t* libdata = MotorPortHandler_Call_Allocate(sizeof(MotorLibrary_SpeedControlled_Data_t));
        
    libdata->position = 0;
    libdata->lastPosition = 0;
    libdata->targetSpeed = 0.0f;
    pid_initialize(&libdata->controller);

    motorPort->libraryData = libdata;
    MotorPort_SetGreenLed(motorPort, true);
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_DeInit(MotorPort_t* motorPort)
{
    MotorPort_SetDriveValue(motorPort, 0);
    MotorPort_SetGreenLed(motorPort, false);
    MotorPortHandler_Call_Free(&motorPort->libraryData);
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_Update(MotorPort_t* motorPort)
{
    MotorLibrary_SpeedControlled_Data_t* libdata = (MotorLibrary_SpeedControlled_Data_t*) motorPort->libraryData;

    int32_t position = libdata->position;
    int32_t measuredSpeed = position - libdata->lastPosition; // todo more sensible (less arbitrary) unit of speed

    libdata->y0 = libdata->alpha * measuredSpeed + (1 - libdata->alpha) * libdata->y0;
    libdata->lastPosition = position;

    float u = pid_update(&libdata->controller, libdata->targetSpeed, libdata->y0);
    int8_t driveValue = (int8_t) lroundf(u);
    MotorPort_SetDriveValue(motorPort, driveValue);

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_Gpio0Callback(MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state)
{
    MotorLibrary_SpeedControlled_Data_t* libdata = (MotorLibrary_SpeedControlled_Data_t*) motorPort->libraryData;
    
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

MotorLibraryStatus_t SpeedControlled_Gpio1Callback(MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state)
{
    MotorLibrary_SpeedControlled_Data_t* libdata = (MotorLibrary_SpeedControlled_Data_t*) motorPort->libraryData;
    
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

MotorLibraryStatus_t SpeedControlled_SetConfig(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    MotorLibrary_SpeedControlled_Data_t* libdata = (MotorLibrary_SpeedControlled_Data_t*) motorPort->libraryData;
    if (size != sizeof(libdata->configuration))
    {
        return MotorLibraryStatus_InputError;
    }

    memcpy(&libdata->configuration[0], data, size);

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_UpdateConfiguration(MotorPort_t* motorPort)
{
    MotorLibrary_SpeedControlled_Data_t* libdata = (MotorLibrary_SpeedControlled_Data_t*) motorPort->libraryData;

    pid_initialize(&libdata->controller);
    libdata->controller.config.P = get_float(&libdata->configuration[0]);
    libdata->controller.config.I = get_float(&libdata->configuration[4]);
    libdata->controller.config.D = get_float(&libdata->configuration[8]);
    libdata->controller.config.LowerLimit = get_float(&libdata->configuration[12]);
    libdata->controller.config.UpperLimit = get_float(&libdata->configuration[16]);
    
    libdata->position = 0;
    libdata->lastPosition = 0;
    libdata->targetSpeed = 0.0f;

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_GetConfig(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    *size = 0u;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_GetPosition(MotorPort_t* motorPort, int32_t* data)
{
    MotorLibrary_SpeedControlled_Data_t* libdata = (MotorLibrary_SpeedControlled_Data_t*) motorPort->libraryData;
    *data = libdata->position;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_SetControlReference(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    if (size != 4u)
    {
        return MotorLibraryStatus_InputError;
    }
    MotorLibrary_SpeedControlled_Data_t* libdata = (MotorLibrary_SpeedControlled_Data_t*) motorPort->libraryData;
    libdata->targetSpeed = get_float(data);

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_GetControlReference(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    *size = 0u;
    return MotorLibraryStatus_Ok;
}

const MotorLibrary_t motor_library_speed_controlled = 
{
    .name = "SpeedControlled",
    .Init                = &SpeedControlled_Init,
    .DeInit              = &SpeedControlled_DeInit,
    .Update              = &SpeedControlled_Update,
    .Gpio0Callback       = &SpeedControlled_Gpio0Callback,
    .Gpio1Callback       = &SpeedControlled_Gpio1Callback,
    .SetConfig           = &SpeedControlled_SetConfig,
    .UpdateConfiguration = &SpeedControlled_UpdateConfiguration,
    .GetConfig           = &SpeedControlled_GetConfig,
    .GetPosition         = &SpeedControlled_GetPosition,
    .SetControlReference = &SpeedControlled_SetControlReference,
    .GetControlReference = &SpeedControlled_GetControlReference
};

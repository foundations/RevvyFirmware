/*
 * OpenLoop.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
 */ 
#include "OpenLoop.h"

MotorLibraryStatus_t OpenLoop_Init(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_DeInit(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_Update(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_Gpio0Callback(MotorPort_t* motorPort, uint32_t state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_Gpio1Callback(MotorPort_t* motorPort, uint32_t state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_SetConfig(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_GetConfig(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    *size = 0u;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_GetPosition(MotorPort_t* motorPort, int32_t* data)
{
    *data = 0;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_SetControlReference(MotorPort_t* motorPort, uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_GetControlReference(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    *size = 0u;
    return MotorLibraryStatus_Ok;
}

const MotorLibrary_t motor_library_open_loop = 
{
    .name = "OpenLoop",
    .Init                = &OpenLoop_Init,
    .DeInit              = &OpenLoop_DeInit,
    .Update              = &OpenLoop_Update,
    .Gpio0Callback       = &OpenLoop_Gpio0Callback,
    .Gpio1Callback       = &OpenLoop_Gpio1Callback,
    .SetConfig           = &OpenLoop_SetConfig,
    .GetConfig           = &OpenLoop_GetConfig,
    .GetPosition         = &OpenLoop_GetPosition,
    .SetControlReference = &OpenLoop_SetControlReference,
    .GetControlReference = &OpenLoop_GetControlReference
};

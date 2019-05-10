/*
 * Dummy.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: D�niel Buga
 */ 
#include "Dummy.h"

MotorLibraryStatus_t Dummy_Init(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_DeInit(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_Update(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_Gpio0Callback(MotorPort_t* motorPort, uint32_t state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_Gpio1Callback(MotorPort_t* motorPort, uint32_t state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_SetConfig(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_GetConfig(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    *size = 0u;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_GetPosition(MotorPort_t* motorPort, int32_t* data)
{
    *data = 0;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_SetControlReference(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_GetControlReference(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    *size = 0u;
    return MotorLibraryStatus_Ok;
}

const MotorLibrary_t motor_library_dummy = 
{
    .name = "NotConfigured",
    .Init                = &Dummy_Init,
    .DeInit              = &Dummy_DeInit,
    .Update              = &Dummy_Update,
    .Gpio0Callback       = &Dummy_Gpio0Callback,
    .Gpio1Callback       = &Dummy_Gpio1Callback,
    .SetConfig           = &Dummy_SetConfig,
    .GetConfig           = &Dummy_GetConfig,
    .GetPosition         = &Dummy_GetPosition,
    .SetControlReference = &Dummy_SetControlReference,
    .GetControlReference = &Dummy_GetControlReference
};

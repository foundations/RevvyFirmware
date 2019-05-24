/*
 * Dummy.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
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

MotorLibraryStatus_t Dummy_Gpio0Callback(MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_Gpio1Callback(MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_SetConfig(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_UpdateConfiguration(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_GetStatus(MotorPort_t* motorPort, uint8_t* data, uint8_t* dataSize)
{
    *dataSize = 0u;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t Dummy_SetControlReference(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

const MotorLibrary_t motor_library_dummy = 
{
    .name                = "NotConfigured",
    .Init                = &Dummy_Init,
    .DeInit              = &Dummy_DeInit,
    .Update              = &Dummy_Update,
    .Gpio0Callback       = &Dummy_Gpio0Callback,
    .Gpio1Callback       = &Dummy_Gpio1Callback,
    .SetConfig           = &Dummy_SetConfig,
    .UpdateConfiguration = &Dummy_UpdateConfiguration,
    .GetStatus           = &Dummy_GetStatus,
    .SetControlReference = &Dummy_SetControlReference,
};

/*
 * PositionControlled.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
 */ 
#include "PositionControlled.h"

MotorLibraryStatus_t PositionControlled_Init(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_DeInit(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_Update(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_Gpio0Callback(MotorPort_t* motorPort, uint32_t state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_Gpio1Callback(MotorPort_t* motorPort, uint32_t state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_SetConfig(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_GetConfig(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_GetPosition(MotorPort_t* motorPort, uint8_t* data, uint32_t max_size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_SetControlReference(MotorPort_t* motorPort, uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t PositionControlled_GetControlReference(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    return MotorLibraryStatus_Ok;
}

const MotorLibrary_t motor_library_position_controlled = 
{
    .name = "PositionControlled",
    .Init                = &PositionControlled_Init,
    .DeInit              = &PositionControlled_DeInit,
    .Update              = &PositionControlled_Update,
    .Gpio0Callback       = &PositionControlled_Gpio0Callback,
    .Gpio1Callback       = &PositionControlled_Gpio1Callback,
    .SetConfig           = &PositionControlled_SetConfig,
    .GetConfig           = &PositionControlled_GetConfig,
    .GetPosition         = &PositionControlled_GetPosition,
    .SetControlReference = &PositionControlled_SetControlReference,
    .GetControlReference = &PositionControlled_GetControlReference
};

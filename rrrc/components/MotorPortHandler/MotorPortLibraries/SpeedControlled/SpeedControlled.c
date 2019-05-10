/*
 * SpeedControlled.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
 */ 
#include "SpeedControlled.h"

typedef struct 
{

} MotorLibrary_SpeedControlled_Data_t;

MotorLibraryStatus_t SpeedControlled_Init(MotorPort_t* motorPort)
{
    motorPort->libraryData = MotorPortHandler_Call_Allocate(sizeof(MotorLibrary_SpeedControlled_Data_t));
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
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_Gpio0Callback(MotorPort_t* motorPort, uint32_t state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_Gpio1Callback(MotorPort_t* motorPort, uint32_t state)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_SetConfig(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_UpdateConfiguration(MotorPort_t* motorPort)
{
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_GetConfig(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    *size = 0u;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_GetPosition(MotorPort_t* motorPort, int32_t* data)
{
    *data = 0;
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t SpeedControlled_SetControlReference(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
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

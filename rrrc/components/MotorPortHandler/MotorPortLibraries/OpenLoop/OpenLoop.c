/*
 * OpenLoop.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
 */ 
#include "OpenLoop.h"

#include "utils/functions.h"

typedef struct 
{
    int8_t positiveLimit;
    int8_t negativeLimit;
    int8_t controlValue;
} MotorLibrary_OpenLoop_Data_t;

MotorLibraryStatus_t OpenLoop_Init(MotorPort_t* motorPort)
{
    MotorLibrary_OpenLoop_Data_t* libdata = MotorPortHandler_Call_Allocate(sizeof(MotorLibrary_OpenLoop_Data_t));
    
    libdata->controlValue = 0;
    libdata->positiveLimit = 100;
    libdata->negativeLimit = -100;
    
    motorPort->libraryData = libdata;
    MotorPort_SetGreenLed(motorPort, true);
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_DeInit(MotorPort_t* motorPort)
{
    MotorPort_SetDriveValue(motorPort, 0);
    MotorPort_SetGreenLed(motorPort, false);
    MotorPortHandler_Call_Free(&motorPort->libraryData);
    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_Update(MotorPort_t* motorPort)
{
    MotorLibrary_OpenLoop_Data_t* libdata = (MotorLibrary_OpenLoop_Data_t*) motorPort->libraryData;
    MotorPort_SetDriveValue(motorPort, libdata->controlValue);
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
    if (size != 2u)
    {
        return MotorLibraryStatus_InputError;
    }
    
    MotorLibrary_OpenLoop_Data_t* libdata = (MotorLibrary_OpenLoop_Data_t*) motorPort->libraryData;
    libdata->positiveLimit = (int8_t) data[0];
    libdata->negativeLimit = (int8_t) data[1];

    libdata->controlValue = constrain_int8(libdata->controlValue, libdata->negativeLimit, libdata->positiveLimit);

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_UpdateConfiguration(MotorPort_t* motorPort)
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

MotorLibraryStatus_t OpenLoop_SetControlReference(MotorPort_t* motorPort, const uint8_t* data, uint8_t size)
{
    if (size != 1u)
    {
        return MotorLibraryStatus_InputError;
    }
    
    MotorLibrary_OpenLoop_Data_t* libdata = (MotorLibrary_OpenLoop_Data_t*) motorPort->libraryData;
    libdata->controlValue = constrain_int8((int8_t) data[0], libdata->negativeLimit, libdata->positiveLimit);

    return MotorLibraryStatus_Ok;
}

MotorLibraryStatus_t OpenLoop_GetControlReference(MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    *size = 0u;
    return MotorLibraryStatus_Ok;
}

const MotorLibrary_t motor_library_open_loop = 
{
    .name                = "OpenLoop",
    .Init                = &OpenLoop_Init,
    .DeInit              = &OpenLoop_DeInit,
    .Update              = &OpenLoop_Update,
    .Gpio0Callback       = &OpenLoop_Gpio0Callback,
    .Gpio1Callback       = &OpenLoop_Gpio1Callback,
    .SetConfig           = &OpenLoop_SetConfig,
    .UpdateConfiguration = &OpenLoop_UpdateConfiguration,
    .GetConfig           = &OpenLoop_GetConfig,
    .GetPosition         = &OpenLoop_GetPosition,
    .SetControlReference = &OpenLoop_SetControlReference,
    .GetControlReference = &OpenLoop_GetControlReference
};

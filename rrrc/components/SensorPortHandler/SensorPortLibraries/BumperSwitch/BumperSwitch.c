/*
 * BumperSwitch.c
 *
 * Created: 10/05/2019 10:49:56
 *  Author: Dániel Buga
 */ 
#include "BumperSwitch.h"

#include "FreeRTOS.h"
#include "task.h"

typedef struct 
{
    uint8_t threshold;
    uint8_t analogValue;
    bool was_pressed;
} SensorLibrary_BumperSwitch_Data_t;
 
SensorLibraryStatus_t BumperSwitch_Init(SensorPort_t* sensorPort)
{
    SensorLibrary_BumperSwitch_Data_t* libdata = SensorPortHandler_Call_Allocate(sizeof(SensorLibrary_BumperSwitch_Data_t));
    libdata->threshold = 75u; /* sensor ADC input has a range of 0..5V, bumper switch is 3V, set threshold to half of that */
    libdata->was_pressed = false;
    
    sensorPort->libraryData = libdata;
    SensorPort_SetGreenLed(sensorPort, true);
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_DeInit(SensorPort_t* sensorPort)
{
    SensorPort_SetOrangeLed(sensorPort, false);
    SensorPort_SetGreenLed(sensorPort, false);
    SensorPortHandler_Call_Free(&sensorPort->libraryData);
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_Update(SensorPort_t* sensorPort)
{
    SensorLibrary_BumperSwitch_Data_t* libdata = (SensorLibrary_BumperSwitch_Data_t*) sensorPort->libraryData;

    if (libdata->analogValue > libdata->threshold)
    {
        SensorPort_SetOrangeLed(sensorPort, true);
    }
    else
    {
        SensorPort_SetOrangeLed(sensorPort, false);
    }

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_SetConfig(SensorPort_t* sensorPort, const uint8_t* data, uint8_t size)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_PrepareGetValue(SensorPort_t* sensorPort, const uint8_t* parameter, uint8_t parameterLength)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_GetValue(SensorPort_t* sensorPort, uint8_t* value, uint8_t maxSize, uint8_t* valueSize)
{
    SensorLibrary_BumperSwitch_Data_t* libdata = (SensorLibrary_BumperSwitch_Data_t*) sensorPort->libraryData;

    portENTER_CRITICAL();
    value[0] = (libdata->analogValue > libdata->threshold) ? 1u : 0u;
    value[1] = libdata->was_pressed ? 1u : 0u;
    libdata->was_pressed = false;
    portEXIT_CRITICAL();

    *valueSize = 2u;

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_UpdateConfiguration(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_InterruptCallback(SensorPort_t* sensorPort, bool status)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_UpdateAnalogData(SensorPort_t* sensorPort, uint8_t rawValue)
{
    SensorLibrary_BumperSwitch_Data_t* libdata = (SensorLibrary_BumperSwitch_Data_t*) sensorPort->libraryData;
    portENTER_CRITICAL();
    libdata->analogValue = rawValue;
    if (rawValue > libdata->threshold)
    {
        libdata->was_pressed = true;
    }
    portEXIT_CRITICAL();
    return SensorLibraryStatus_Ok;
}

const SensorLibrary_t sensor_library_bumper_switch = 
{
    .name                = "BumperSwitch",
    .Init                = &BumperSwitch_Init,
    .DeInit              = &BumperSwitch_DeInit,
    .Update              = &BumperSwitch_Update,
    .SetConfig           = &BumperSwitch_SetConfig,
    .PrepareGetValue     = &BumperSwitch_PrepareGetValue,
    .GetValue            = &BumperSwitch_GetValue,
    .UpdateConfiguration = &BumperSwitch_UpdateConfiguration,
    .UpdateAnalogData    = &BumperSwitch_UpdateAnalogData,
    .InterruptHandler    = &BumperSwitch_InterruptCallback
};

/*
 * Dummy.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
 */ 
#include "Dummy.h"

SensorLibraryStatus_t DummySensor_Init(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_DeInit(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_SetConfig(SensorPort_t* sensorPort, const uint8_t* data, uint8_t size)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_GetConfig(SensorPort_t* sensorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    return SensorLibraryStatus_Ok;
}

const SensorLibrary_t sensor_library_dummy = 
{
    .name = "Dummy",
    .Init      = &DummySensor_Init,
    .DeInit    = &DummySensor_DeInit,
    .SetConfig = &DummySensor_SetConfig,
    .GetConfig = &DummySensor_GetConfig,
};

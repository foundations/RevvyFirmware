/*
 * Dummy.c
 *
 * Created: 09/05/2019 16:18:37
 *  Author: Dániel Buga
 */ 
#include "Dummy.h"

SensorLibraryStatus_t DummySensor_Init(SensorPort_t* sensorPort)
{
    (void) sensorPort;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_DeInit(SensorPort_t* sensorPort)
{
    (void) sensorPort;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_Update(SensorPort_t* sensorPort)
{
    (void) sensorPort;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_SetConfig(SensorPort_t* sensorPort, const uint8_t* data, uint8_t size)
{
    (void) sensorPort;
    (void) data;
    (void) size;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_PrepareGetValue(SensorPort_t* sensorPort, const uint8_t* parameter, uint8_t parameterLength)
{
    (void) sensorPort;
    (void) parameter;
    (void) parameterLength;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_GetValue(SensorPort_t* sensorPort, uint8_t* value, uint8_t maxSize, uint8_t* valueSize)
{
    (void) sensorPort;
    (void) value;
    (void) maxSize;
    *valueSize = 0u;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_UpdateConfiguration(SensorPort_t* sensorPort)
{
    (void) sensorPort;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_UpdateAnalogData(SensorPort_t* sensorPort, uint8_t rawValue)
{
    (void) sensorPort;
    (void) rawValue;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DummySensor_InterruptCallback(SensorPort_t* sensorPort, bool status)
{
    (void) sensorPort;
    (void) status;
    return SensorLibraryStatus_Ok;
}

const SensorLibrary_t sensor_library_dummy = 
{
    .name                = "NotConfigured",
    .Init                = &DummySensor_Init,
    .DeInit              = &DummySensor_DeInit,
    .Update              = &DummySensor_Update,
    .SetConfig           = &DummySensor_SetConfig,
    .PrepareGetValue     = &DummySensor_PrepareGetValue,
    .GetValue            = &DummySensor_GetValue,
    .UpdateConfiguration = &DummySensor_UpdateConfiguration,
    .UpdateAnalogData    = &DummySensor_UpdateAnalogData,
    .InterruptHandler    = &DummySensor_InterruptCallback
};

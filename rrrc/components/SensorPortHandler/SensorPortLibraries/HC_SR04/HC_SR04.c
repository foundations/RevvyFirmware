/*
 * HC_SR04.c
 *
 * Created: 10/05/2019 10:49:56
 *  Author: Dániel Buga
 */ 
#include "HC_SR04.h"
 
SensorLibraryStatus_t HC_SR04_Init(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_DeInit(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_Update(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_SetConfig(SensorPort_t* sensorPort, const uint8_t* data, uint8_t size)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_GetConfig(SensorPort_t* sensorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    return SensorLibraryStatus_Ok;
}

const SensorLibrary_t sensor_library_hc_sr04 = 
{
    .name      = "HC_SR04",
    .Init      = &HC_SR04_Init,
    .DeInit    = &HC_SR04_DeInit,
    .Update    = &HC_SR04_Update,
    .SetConfig = &HC_SR04_SetConfig,
    .GetConfig = &HC_SR04_GetConfig,
};

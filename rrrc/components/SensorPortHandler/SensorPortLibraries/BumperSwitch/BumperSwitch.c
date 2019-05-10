/*
 * BumperSwitch.c
 *
 * Created: 10/05/2019 10:49:56
 *  Author: Dániel Buga
 */ 
#include "BumperSwitch.h"
 
SensorLibraryStatus_t BumperSwitch_Init(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_DeInit(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_Update(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_SetConfig(SensorPort_t* sensorPort, const uint8_t* data, uint8_t size)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t BumperSwitch_GetConfig(SensorPort_t* sensorPort, uint8_t* data, uint8_t* size, uint8_t max_size)
{
    return SensorLibraryStatus_Ok;
}

const SensorLibrary_t sensor_library_bumper_switch = 
{
    .name      = "BumperSwitch",
    .Init      = &BumperSwitch_Init,
    .DeInit    = &BumperSwitch_DeInit,
    .Update    = &BumperSwitch_Update,
    .SetConfig = &BumperSwitch_SetConfig,
    .GetConfig = &BumperSwitch_GetConfig,
};

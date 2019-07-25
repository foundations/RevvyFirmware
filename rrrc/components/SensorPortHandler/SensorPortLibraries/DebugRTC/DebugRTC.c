/*
 * DebugRTC.c
 *
 * Created: 2019. 07. 25. 14:54:32
 *  Author: Dániel Buga
 */ 
 
#include "DebugRTC.h"
#include "../SensorPort_I2C.h"
#include <stdbool.h>
#define I2C_ADDRESS ((uint8_t) 0x68u) //0110 1000

static uint8_t data;
static uint8_t prev_data;
static bool reading;

static void rxcomplete(struct i2c_m_async_desc *const i2c)
{
    SensorPort_t *port = CONTAINER_OF(i2c, SensorPort_t, i2c);
    SensorPort_SetOrangeLed(port, ((data % 2) == 0));
    reading = false;
}

static void txcomplete(struct i2c_m_async_desc *const i2c)
{

}

static void error(struct i2c_m_async_desc *const i2c, int32_t error)
{
    reading = false;
}

const i2c_callbacks_t callbacks = 
{
    .rx_complete = rxcomplete,
    .error = error,
    .tx_complete = txcomplete
};

SensorLibraryStatus_t DebugRTC_Init(SensorPort_t* sensorPort)
{
    SensorPort_SetVccIo(sensorPort, Sensor_VccIo_5V);
    SensorPort_I2C_Enable(sensorPort, 100, &callbacks);

    reading = false;
    data = 0;
    prev_data = 0;

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DebugRTC_DeInit(SensorPort_t* sensorPort)
{
    SensorPort_SetGreenLed(sensorPort, false);
    SensorPort_I2C_Disable(sensorPort);
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DebugRTC_Update(SensorPort_t* sensorPort)
{
    if (!reading)
    {
        reading = true;
        if (SensorPort_I2C_StartRegRead(sensorPort, I2C_ADDRESS, 0x00, &data, 1) == SensorPort_I2C_Error)
        {
            SensorPort_SetGreenLed(sensorPort, false);
            reading = false;
        }
        else
        {
            SensorPort_SetGreenLed(sensorPort, true);
        }
    }
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DebugRTC_SetConfig(SensorPort_t* sensorPort, const uint8_t* data, uint8_t size)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DebugRTC_PrepareGetValue(SensorPort_t* sensorPort, const uint8_t* parameter, uint8_t parameterLength)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DebugRTC_GetValue(SensorPort_t* sensorPort, uint8_t* value, uint8_t maxSize, uint8_t* valueSize)
{
    *valueSize = 0u;
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DebugRTC_UpdateConfiguration(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DebugRTC_UpdateAnalogData(SensorPort_t* sensorPort, uint8_t rawValue)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t DebugRTC_InterruptCallback(SensorPort_t* sensorPort, bool status)
{
    return SensorLibraryStatus_Ok;
}

const SensorLibrary_t sensor_library_debug_rtc = 
{
    .name                = "DebugRTC",
    .Init                = &DebugRTC_Init,
    .DeInit              = &DebugRTC_DeInit,
    .Update              = &DebugRTC_Update,
    .SetConfig           = &DebugRTC_SetConfig,
    .PrepareGetValue     = &DebugRTC_PrepareGetValue,
    .GetValue            = &DebugRTC_GetValue,
    .UpdateConfiguration = &DebugRTC_UpdateConfiguration,
    .UpdateAnalogData    = &DebugRTC_UpdateAnalogData,
    .InterruptHandler    = &DebugRTC_InterruptCallback
};

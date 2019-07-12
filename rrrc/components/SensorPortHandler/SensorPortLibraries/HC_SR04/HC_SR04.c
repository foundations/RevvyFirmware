/*
 * HC_SR04.c
 *
 * Created: 10/05/2019 10:49:56
 *  Author: Dániel Buga
 */ 
#include "HC_SR04.h"
#include "rrrc_hal.h"
#include <hal_delay.h>
#include <string.h>
#include <math.h>

#define HCSR05_MEDIAN_FITLER_SIZE ((uint8_t) 5u)

typedef struct 
{
    bool isMeasuring;
    bool finished;
    uint32_t start_time;
    uint32_t distance_tick;
    uint32_t filtered_distance_tick;
    uint32_t distanceBuffer[HCSR05_MEDIAN_FITLER_SIZE - 1];
    uint32_t distanceBufferWriteIdx;
} SensorLibrary_HC_SR04_Data_t;

static uint32_t _get_cm(uint32_t distance_tick)
{
    uint32_t ticks_in_ms = high_res_timer_ticks_per_ms();

    float distance_ms = (float)distance_tick / ticks_in_ms;
    return (uint32_t)lroundf(distance_ms * 17.0f);
}

static inline void swap_uint32(uint32_t* a, uint32_t* b)
{
    uint32_t t = *a;
    *a = *b;
    *b = t;
}

static void update_filtered_distance(SensorLibrary_HC_SR04_Data_t* sens_data)
{
    uint32_t ordered[HCSR05_MEDIAN_FITLER_SIZE];

    uint32_t distance = sens_data->distance_tick;
    if (distance < 1000)
    {
        memcpy(&ordered[0], &sens_data->distanceBuffer[0], sizeof(sens_data->distanceBuffer));
        ordered[HCSR05_MEDIAN_FITLER_SIZE - 1] = distance;

        sens_data->distanceBuffer[sens_data->distanceBufferWriteIdx] = distance;
        sens_data->distanceBufferWriteIdx = (sens_data->distanceBufferWriteIdx + 1) % ARRAY_SIZE(sens_data->distanceBuffer);

        for (uint32_t i = 0; i < HCSR05_MEDIAN_FITLER_SIZE - 1; i++)
        {
            for (uint32_t j = i; j < HCSR05_MEDIAN_FITLER_SIZE - 1; j++)
            {
                if (ordered[i] > ordered[j])
                {
                    swap_uint32(&ordered[i], &ordered[j]);
                }
            }
        }

        sens_data->filtered_distance_tick = ordered[HCSR05_MEDIAN_FITLER_SIZE / 2];
    }
}
 
SensorLibraryStatus_t HC_SR04_Init(SensorPort_t* sensorPort)
{
    SensorLibrary_HC_SR04_Data_t* libdata = SensorPortHandler_Call_Allocate(sizeof(SensorLibrary_HC_SR04_Data_t));

    libdata->finished = false;
    libdata->isMeasuring = false;

    sensorPort->libraryData = libdata;
    SensorPort_SetGreenLed(sensorPort, true);
    
    SensorPort_SetVccIo(sensorPort, Sensor_VccIo_5V);
    SensorPort_ConfigureGpio0_Output(sensorPort);
    SensorPort_ConfigureGpio1_Interrupt(sensorPort);

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_DeInit(SensorPort_t* sensorPort)
{
    SensorPort_SetVccIo(sensorPort, Sensor_VccIo_3V3);
    SensorPort_ConfigureGpio0_Input(sensorPort);
    SensorPort_ConfigureGpio1_Input(sensorPort);

    SensorPort_SetOrangeLed(sensorPort, false);
    SensorPort_SetGreenLed(sensorPort, false);
    SensorPortHandler_Call_Free(&sensorPort->libraryData);
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_Update(SensorPort_t* sensorPort)
{
    SensorLibrary_HC_SR04_Data_t* libdata = (SensorLibrary_HC_SR04_Data_t*) sensorPort->libraryData;

    if (!libdata->isMeasuring)
    {
        SensorPort_SetGpio0_Output(sensorPort, true);
        delay_us(30);
        SensorPort_SetGpio0_Output(sensorPort, false);

        libdata->finished = false;
        libdata->isMeasuring = true;
    }
    else
    {
        if (libdata->finished)
        {
            libdata->isMeasuring = false;
            update_filtered_distance(libdata);
            
            uint16_t cm = (uint16_t) _get_cm(libdata->filtered_distance_tick);
            SensorPort_Write_PortState(sensorPort->port_idx, cm, sizeof(cm));
        }
    }
    
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_SetConfig(SensorPort_t* sensorPort, const uint8_t* data, uint8_t size)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_PrepareGetValue(SensorPort_t* sensorPort, const uint8_t* parameter, uint8_t parameterLength)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_GetValue(SensorPort_t* sensorPort, uint8_t* value, uint8_t maxSize, uint8_t* valueSize)
{
    SensorLibrary_HC_SR04_Data_t* libdata = (SensorLibrary_HC_SR04_Data_t*) sensorPort->libraryData;
    
    uint32_t cm = _get_cm(libdata->filtered_distance_tick);

    memcpy(value, &cm, sizeof(uint32_t));
    *valueSize = 4u;

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_UpdateConfiguration(SensorPort_t* sensorPort)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_UpdateAnalogData(SensorPort_t* sensorPort, uint8_t rawValue)
{
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_InterruptCallback(SensorPort_t* sensorPort, bool status)
{
    SensorLibrary_HC_SR04_Data_t* libdata = (SensorLibrary_HC_SR04_Data_t*) sensorPort->libraryData;
    if (status)
    {
        libdata->start_time = high_res_timer_get_count();
    }
    else
    {
        uint32_t finish_time = high_res_timer_get_count();
        uint32_t dist = finish_time - libdata->start_time;

        if (dist < 0x7FFFu) /* FIXME: this is a hack to prevent start > finish cases (unknown root cause) */
        {
            libdata->distance_tick = dist;
        }
        libdata->finished = true;
    }
    SensorPort_SetOrangeLed(sensorPort, status);
    return SensorLibraryStatus_Ok;
}

const SensorLibrary_t sensor_library_hc_sr04 = 
{
    .name                = "HC_SR04",
    .Init                = &HC_SR04_Init,
    .DeInit              = &HC_SR04_DeInit,
    .Update              = &HC_SR04_Update,
    .SetConfig           = &HC_SR04_SetConfig,
    .PrepareGetValue     = &HC_SR04_PrepareGetValue,
    .GetValue            = &HC_SR04_GetValue,
    .UpdateConfiguration = &HC_SR04_UpdateConfiguration,
    .UpdateAnalogData    = &HC_SR04_UpdateAnalogData,
    .InterruptHandler    = &HC_SR04_InterruptCallback,
};

/*
 * HC_SR04.c
 *
 * Created: 10/05/2019 10:49:56
 *  Author: Dániel Buga
 */ 
#include "HC_SR04.h"
#include <hal_delay.h>
#include <string.h>
#include <math.h>

#define HCSR05_MEDIAN_FITLER_SIZE ((uint8_t) 5u)

typedef struct 
{
    bool isMeasuring;
    bool finished;
    uint16_t start_time;
    uint32_t distance_tick;
    uint32_t filtered_distance_tick;
    uint32_t distanceBuffer[HCSR05_MEDIAN_FITLER_SIZE - 1];
    uint32_t distanceBufferWriteIdx;

    uint32_t timeout;
} SensorLibrary_HC_SR04_Data_t;

static bool ultrasonic_used[] = {false, false, false, false};
static uint8_t ultrasonic_active = 0u;

static uint32_t _get_cm(uint32_t distance_tick)
{
    float distance_ms = SensorPort_Call_GetMsFromTicks(distance_tick);
    uint32_t cm = (uint32_t)lroundf(distance_ms * 17.0f);

    return cm;
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

    ultrasonic_used[sensorPort->port_idx] = true;

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_DeInit(SensorPort_t* sensorPort)
{
    ultrasonic_used[sensorPort->port_idx] = false;

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
        /* make sure an active sensor is selected */
        for (uint8_t i = 0u; i < sizeof(ultrasonic_used); i++)
        {
            if (!ultrasonic_used[ultrasonic_active])
            {
                /* current sensor is inactive, check the next */
                ultrasonic_active = (ultrasonic_active + 1u) % ARRAY_SIZE(ultrasonic_used);
            }
            else
            {
                /* an active sensor is selected, stop searching */
                break;
            }
        }
        
        /* if the current sensor is active, start measurement */
        if (ultrasonic_active == sensorPort->port_idx)
        {
            if (SensorPort_Read_Gpio1(sensorPort) == false)
            {
                SensorPort_SetGpio0_Output(sensorPort, true);
                delay_us(30);
                SensorPort_SetGpio0_Output(sensorPort, false);

                libdata->timeout = 0u;

                libdata->finished = false;
                libdata->isMeasuring = true;
            }
        }
    }
    else
    {
        /* this sensor is currently measuring - check if it has finished */
        if (libdata->finished)
        {
            update_filtered_distance(libdata);
            
            uint32_t cm = _get_cm(libdata->filtered_distance_tick);
            SensorPort_Write_PortState(sensorPort->port_idx, (uint8_t*) &cm, sizeof(cm));
            
            /* mark next sensor as active */
            ultrasonic_active = (ultrasonic_active + 1u) % ARRAY_SIZE(ultrasonic_used);
            libdata->isMeasuring = false;
        }
        else
        {
            if (libdata->timeout == 99u)
            {
                libdata->finished = false;
                libdata->isMeasuring = false;
                SensorPort_SetOrangeLed(sensorPort, false);
            }
            else
            {
                ++libdata->timeout;
            }
        }
    }
    
    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_SetConfig(SensorPort_t* sensorPort, const uint8_t* data, uint8_t size)
{
    (void) sensorPort;
    (void) data;
    (void) size;

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_PrepareGetValue(SensorPort_t* sensorPort, const uint8_t* parameter, uint8_t parameterLength)
{
    (void) sensorPort;
    (void) parameter;
    (void) parameterLength;

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_GetValue(SensorPort_t* sensorPort, uint8_t* value, uint8_t maxSize, uint8_t* valueSize)
{
    SensorLibrary_HC_SR04_Data_t* libdata = (SensorLibrary_HC_SR04_Data_t*) sensorPort->libraryData;
    
    ASSERT(maxSize >= sizeof(uint32_t));

    uint32_t cm = _get_cm(libdata->filtered_distance_tick);

    memcpy(value, &cm, sizeof(uint32_t));
    *valueSize = sizeof(uint32_t);

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_UpdateConfiguration(SensorPort_t* sensorPort)
{
    (void) sensorPort;

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_UpdateAnalogData(SensorPort_t* sensorPort, uint8_t rawValue)
{
    (void) sensorPort;
    (void) rawValue;

    return SensorLibraryStatus_Ok;
}

SensorLibraryStatus_t HC_SR04_InterruptCallback(SensorPort_t* sensorPort, bool status)
{
    SensorLibrary_HC_SR04_Data_t* libdata = (SensorLibrary_HC_SR04_Data_t*) sensorPort->libraryData;
    if (libdata->isMeasuring)
    {
        if (status)
        {
            libdata->start_time = SensorPort_Read_CurrentTicks();
        }
        else
        {
            uint16_t finish_time = SensorPort_Read_CurrentTicks();
            uint16_t dist = finish_time - libdata->start_time;

            if (SensorPort_Call_GetMsFromTicks(dist) < 40.0f) /**< arbitrary limit that is shorter than sensor timeout */
            {
                libdata->distance_tick = dist;
            }

            libdata->finished = true;
        }
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

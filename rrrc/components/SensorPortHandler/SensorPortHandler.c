/*
 * SensorPortHandler.c
 *
 * Created: 09/05/2019 14:03:32
 *  Author: Dániel Buga
 */ 

#include "SensorPortHandlerInternal.h"
#include "utils.h"
#include <string.h>

static const SensorLibrary_t* libraries[] = {};

static size_t sensorPortCount = 0u;
static SensorPort_t* sensorPorts = NULL;

static void _init_port(SensorPort_t* port)
{
    /* init led pins */
    gpio_set_pin_pull_mode(port->led0, GPIO_PULL_UP);
    gpio_set_pin_function(port->led0, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(port->led0, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(port->led0, false);

    gpio_set_pin_pull_mode(port->led1, GPIO_PULL_UP);
    gpio_set_pin_function(port->led1, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(port->led1, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(port->led1, false);

    /* init gpios */
}

Comm_Status_t SensorPortHandler_GetSensorPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    response[0] = sensorPortCount;
    *responseCount = 1u;

    return Comm_Status_Ok;
}

Comm_Status_t SensorPortHandler_GetSensorPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    uint8_t len = 0u;
    for (uint32_t i = 0u; i < ARRAY_SIZE(libraries); i++)
    {
        const SensorLibrary_t* lib = libraries[i];
        size_t name_length = strlen(lib->name);
        if (len + name_length + 2u > responseBufferSize)
        {
            *responseCount = 0u;
            return Comm_Status_Error_InternalError;
        }
        response[len] = i;
        response[len + 1] = name_length;
        memcpy(&response[len + 2], lib->name, name_length);
        len = len + 2 + name_length;
    }
    *responseCount = len;

    return Comm_Status_Ok;
}

void SensorPortHandler_Run_OnInit(SensorPort_t* ports, size_t portCount)
{
    sensorPorts = ports;
    sensorPortCount = portCount;

    for (size_t i = 0u; i < portCount; i++)
    {
        _init_port(&sensorPorts[i]);
    }
}

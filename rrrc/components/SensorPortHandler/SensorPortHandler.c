/*
 * SensorPortHandler.c
 *
 * Created: 09/05/2019 14:03:32
 *  Author: D�niel Buga
 */ 

#include "SensorPortHandlerInternal.h"
#include "utils.h"
#include <string.h>

#include "SensorPortLibraries/Dummy/Dummy.h"

static const SensorLibrary_t* libraries[] = 
{
    &sensor_library_dummy
};

static size_t sensorPortCount = 0u;
static SensorPort_t* sensorPorts = NULL;
static SensorPort_t* configuredPort = NULL;

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

    /* set dummy library */
    port->library = &sensor_library_dummy;
}

Comm_Status_t SensorPortHandler_GetPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    response[0] = sensorPortCount;
    *responseCount = 1u;

    return Comm_Status_Ok;
}

Comm_Status_t SensorPortHandler_GetPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
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

Comm_Status_t SensorPortHandler_SetPortType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint8_t port_idx = commandPayload[0];
    uint8_t type_idx = commandPayload[1];
    if (port_idx >= sensorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    if (type_idx >= ARRAY_SIZE(libraries))
    {
        return Comm_Status_Error_CommandError;
    }

    SensorPort_t* port = &sensorPorts[port_idx];
    port->requestedLibrary = libraries[type_idx];

    configuredPort = port;

    return Comm_Status_Pending;
}

Comm_Status_t SensorPortHandler_SetPortType_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (configuredPort == NULL)
    {
        return Comm_Status_Error_InvalidOperation;
    }

    if (configuredPort->requestedLibrary == configuredPort->library)
    {
        configuredPort = NULL;
        return Comm_Status_Ok;
    }
    else
    {
        return Comm_Status_Pending;
    }
}

Comm_Status_t SensorPortHandler_SetPortConfig_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    return Comm_Status_Error_InternalError;
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

void SensorPortHandler_Run_Update(uint8_t port_idx)
{
    if (port_idx < sensorPortCount)
    {
        SensorPort_t* port = &sensorPorts[port_idx];

        const SensorLibrary_t* requestedLibrary = port->requestedLibrary;
        if (requestedLibrary != port->library)
        {
            port->library->DeInit(port);
            port->library = requestedLibrary;
            port->library->Init(port);
        }

        port->library->Update(port);
    }
}

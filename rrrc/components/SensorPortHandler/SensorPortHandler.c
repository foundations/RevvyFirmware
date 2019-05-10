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
#include "SensorPortLibraries/BumperSwitch/BumperSwitch.h"
#include "SensorPortLibraries/HC_SR04/HC_SR04.h"

static const SensorLibrary_t* libraries[] = 
{
    &sensor_library_dummy,
    &sensor_library_bumper_switch,
    &sensor_library_hc_sr04,
};

static size_t sensorPortCount = 0u;
static SensorPort_t* sensorPorts = NULL;
static SensorPort_t* configuredPort = NULL;
static SensorPort_t* activePort = NULL;

static void SensorPort_ext0_callback(void* user_data)
{
    SensorPort_t* port = (SensorPort_t*) user_data;

    port->library->InterruptHandler(port, SensorPort_Read_Gpio0(port));
}

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

    /* init vccio pin */
    gpio_set_pin_pull_mode(port->vccio, GPIO_PULL_OFF);
    gpio_set_pin_function(port->vccio, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(port->vccio, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(port->vccio, false);

    ext_irq_register(port->gpio0, &SensorPort_ext0_callback, port);

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
        return Comm_Status_Ok;
    }
    else
    {
        return Comm_Status_Pending;
    }
}

Comm_Status_t SensorPortHandler_SetPortConfig_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize < 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    uint8_t port_idx = commandPayload[0];
    if (port_idx >= sensorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    SensorPort_t* port = &sensorPorts[port_idx];
    SensorLibraryStatus_t result = port->library->SetConfig(port, &commandPayload[1], commandSize - 1u);

    if (result == SensorLibraryStatus_Ok)
    {
        configuredPort = port;
        return Comm_Status_Pending;
    }
    else
    {
        return Comm_Status_Error_InternalError;
    }
}

Comm_Status_t SensorPortHandler_SetPortConfig_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (configuredPort == NULL)
    {
        return Comm_Status_Ok;
    }
    else
    {
        return Comm_Status_Pending;
    }
}

Comm_Status_t SensorPortHandler_GetSensorData_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize < 1u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    uint8_t port_idx = commandPayload[0];
    if (port_idx >= sensorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    SensorPort_t* port = &sensorPorts[port_idx];
    SensorLibraryStatus_t result = port->library->PrepareGetValue(port, &commandPayload[1], commandSize - 1u);

    /* Prepare does not return with data. If it returns ok, data shall be read using opeartion GetResult */
    if (result == SensorLibraryStatus_Ok)
    {
        activePort = port;
        return Comm_Status_Pending;
    }
    else
    {
        return Comm_Status_Error_InternalError;
    }
}

Comm_Status_t SensorPortHandler_GetSensorData_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (activePort == NULL)
    {
        return Comm_Status_Error_InvalidOperation;
    }

    SensorLibraryStatus_t result = activePort->library->GetValue(activePort, response, responseBufferSize, responseCount);

    if (result == SensorLibraryStatus_Ok)
    {
        activePort = NULL;
        return Comm_Status_Ok;
    }
    else if (result == SensorLibraryStatus_Pending)
    {
        return Comm_Status_Pending;
    }
    else
    {
        activePort = NULL;
        return Comm_Status_Error_InternalError;
    }
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

void SensorPortHandler_Run_Update(void)
{
    if (configuredPort != NULL)
    {
        const SensorLibrary_t* requestedLibrary = configuredPort->requestedLibrary;
        if (requestedLibrary != configuredPort->library)
        {
            configuredPort->library->DeInit(configuredPort);
            configuredPort->library = requestedLibrary;
            configuredPort->library->Init(configuredPort);
        }

        configuredPort->library->UpdateConfiguration(configuredPort);

        configuredPort = NULL;
    }
}

void SensorPortHandler_Run_PortUpdate(uint8_t port_idx)
{
    ASSERT(port_idx < sensorPortCount);

    SensorPort_t* port = &sensorPorts[port_idx];
    port->library->UpdateAnalogData(port, SensorPortHandler_Read_AdcData(port->adc));
    port->library->Update(port);
}

__attribute__((weak))
uint8_t SensorPortHandler_Read_AdcData(uint8_t channel_idx)
{
    return 0u;
}

__attribute__((weak))
void* SensorPortHandler_Call_Allocate(size_t size)
{
    return NULL;
}

__attribute__((weak))
void SensorPortHandler_Call_Free(void** ptr)
{
    *ptr = NULL;
}
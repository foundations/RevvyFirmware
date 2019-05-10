/*
 * MotorPortHandler.c
 *
 * Created: 09/05/2019 14:03:32
 *  Author: Dániel Buga
 */ 

#include "MotorPortHandlerInternal.h"
#include "utils.h"
#include <string.h>

#include "MotorPortLibraries/Dummy/Dummy.h"
#include "MotorPortLibraries/SpeedControlled/SpeedControlled.h"
#include "MotorPortLibraries/PositionControlled/PositionControlled.h"
#include "MotorPortLibraries/OpenLoop/OpenLoop.h"

static const MotorLibrary_t* libraries[] = 
{
    &motor_library_dummy,
    &motor_library_open_loop,
    &motor_library_speed_controlled,
    &motor_library_position_controlled,
};

static size_t motorPortCount = 0u;
static MotorPort_t* motorPorts = NULL;
static MotorPort_t* configuredPort = NULL;

static void _init_port(MotorPort_t* port)
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
    port->library = &motor_library_dummy;
}

Comm_Status_t MotorPortHandler_GetPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    response[0] = motorPortCount;
    *responseCount = 1u;

    return Comm_Status_Ok;
}

Comm_Status_t MotorPortHandler_GetPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    uint8_t len = 0u;
    for (uint32_t i = 0u; i < ARRAY_SIZE(libraries); i++)
    {
        const MotorLibrary_t* lib = libraries[i];
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

Comm_Status_t MotorPortHandler_SetPortType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint8_t port_idx = commandPayload[0];
    uint8_t type_idx = commandPayload[1];
    if (port_idx >= motorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    if (type_idx >= ARRAY_SIZE(libraries))
    {
        return Comm_Status_Error_CommandError;
    }

    MotorPort_t* port = &motorPorts[port_idx];
    port->requestedLibrary = libraries[type_idx];

    configuredPort = port;

    return Comm_Status_Pending;
}

Comm_Status_t MotorPortHandler_SetPortType_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
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

Comm_Status_t MotorPortHandler_SetPortConfig_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize < 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    uint8_t port_idx = commandPayload[0];
    if (port_idx >= motorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    MotorPort_t* port = &motorPorts[port_idx];
    MotorLibraryStatus_t result = port->library->SetConfig(port, &commandPayload[1], commandSize - 1u);

    if (result == MotorLibraryStatus_Ok)
    {
        configuredPort = port;
        return Comm_Status_Pending;
    }
    else
    {
        return Comm_Status_Error_InternalError;
    }
}

Comm_Status_t MotorPortHandler_SetPortConfig_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
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

void MotorPortHandler_Run_OnInit(MotorPort_t* ports, uint8_t portCount)
{
    motorPorts = ports;
    motorPortCount = portCount;

    for (size_t i = 0u; i < portCount; i++)
    {
        _init_port(&motorPorts[i]);
    }
}

void MotorPortHandler_Run_Update(void)
{
    if (configuredPort != NULL)
    {
        const MotorLibrary_t* requestedLibrary = configuredPort->requestedLibrary;
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

void MotorPortHandler_Run_PortUpdate(uint8_t port_idx)
{
    ASSERT(port_idx < motorPortCount);

    MotorPort_t* port = &motorPorts[port_idx];
    port->library->Update(port);
}

__attribute__((weak))
void MotorPortHandler_Write_MotorDriveValue(uint8_t port_idx, int8_t value)
{
    /* nothing to do */
}

__attribute__((weak))
bool MotorPortHandler_Read_DriverFault(uint8_t port_idx)
{
    return false;
}

__attribute__((weak))
void* MotorPortHandler_Call_Allocate(size_t size)
{
    return NULL;
}

__attribute__((weak))
void MotorPortHandler_Call_Free(void** ptr)
{
    *ptr = NULL;
}

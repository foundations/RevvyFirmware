/*
 * MotorPortHandler.c
 *
 * Created: 09/05/2019 14:03:32
 *  Author: D�niel Buga
 */ 

#include "MotorPortHandlerInternal.h"
#include "utils.h"
#include "atmel_start_pins.h"
#include <string.h>

#include "MotorPortLibraries/Dummy/Dummy.h"
#include "MotorPortLibraries/DcMotor/DcMotor.h"

#include <hal_gpio.h>
#include <hal_ext_irq.h>

static const MotorLibrary_t* libraries[] = 
{
    &motor_library_dummy,
    &motor_library_dc
};

static size_t motorPortCount = 0u;
static MotorPort_t* motorPorts = NULL;
static MotorPort_t* configuredPort = NULL;

static void MotorPort_gpio0_ext_cb(void* port);
static void MotorPort_gpio1_ext_cb(void* port);

static void _init_port(MotorPort_t* port)
{
    /* init led pins */
    gpio_set_pin_pull_mode(port->led, GPIO_PULL_UP);
    gpio_set_pin_function(port->led, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(port->led, GPIO_DIRECTION_OUT);
    MotorPort_SetGreenLed(port, false);

    /* encoders */
    gpio_set_pin_direction(port->enc0, GPIO_DIRECTION_IN);
    gpio_set_pin_function(port->enc0, GPIO_PIN_FUNCTION_A);
    gpio_set_pin_pull_mode(port->enc0, GPIO_PULL_OFF);

    gpio_set_pin_direction(port->enc1, GPIO_DIRECTION_IN);
    gpio_set_pin_function(port->enc1, GPIO_PIN_FUNCTION_A);
    gpio_set_pin_pull_mode(port->enc1, GPIO_PULL_OFF);
    
    __disable_irq();
    ext_irq_register(port->enc0, &MotorPort_gpio0_ext_cb, port);
    ext_irq_register(port->enc1, &MotorPort_gpio1_ext_cb, port);
    
    MotorPort_DisableExti0(port);
    MotorPort_DisableExti1(port);
    /* set dummy library */
    port->library = &motor_library_dummy;
    __enable_irq();
}

static void MotorPort_gpio0_ext_cb(void* port)
{
    (void) data;
    MotorPort_t* motorPort = (MotorPort_t*) port;
    motorPort->library->Gpio0Callback(port, gpio_get_pin_level(motorPort->enc0), gpio_get_pin_level(motorPort->enc1));
}

static void MotorPort_gpio1_ext_cb(void* port)
{
    (void) data;
    MotorPort_t* motorPort = (MotorPort_t*) port;
    motorPort->library->Gpio1Callback(port, gpio_get_pin_level(motorPort->enc0), gpio_get_pin_level(motorPort->enc1));
}

Comm_Status_t MotorPortHandler_GetPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    ASSERT(responseBufferSize >= 1u);

    response[0] = motorPortCount;
    *responseCount = 1u;

    return Comm_Status_Ok;
}

Comm_Status_t MotorPortHandler_GetPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

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
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize != 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint8_t port_idx = commandPayload[0];
    uint8_t type_idx = commandPayload[1];
    if (port_idx == 0u || port_idx > motorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    if (type_idx >= ARRAY_SIZE(libraries))
    {
        return Comm_Status_Error_CommandError;
    }

    MotorPort_t* port = &motorPorts[port_idx - 1u];
    port->requestedLibrary = libraries[type_idx];

    configuredPort = port;

    return Comm_Status_Pending;
}

Comm_Status_t MotorPortHandler_SetPortType_GetResult(uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (configuredPort == NULL)
    {
        return Comm_Status_Ok;
    }
    else
    {
        return Comm_Status_Pending;
    }
}

Comm_Status_t MotorPortHandler_SetPortConfig_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize < 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    uint8_t port_idx = commandPayload[0];
    if (port_idx == 0u || port_idx > motorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    MotorPort_t* port = &motorPorts[port_idx - 1u];
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
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (configuredPort == NULL)
    {
        return Comm_Status_Ok;
    }
    else
    {
        return Comm_Status_Pending;
    }
}

Comm_Status_t MotorPortHandler_SetControlValue_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize < 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    uint8_t port_idx = commandPayload[0];
    if (port_idx == 0u || port_idx > motorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    MotorPort_t* port = &motorPorts[port_idx - 1u];
    MotorLibraryStatus_t result = port->library->SetControlReference(port, &commandPayload[1], commandSize - 1u);

    if (result == MotorLibraryStatus_Ok)
    {
        return Comm_Status_Ok;
    }
    else
    {
        return Comm_Status_Error_InternalError;
    }
}

Comm_Status_t MotorPortHandler_GetStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != 1u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    ASSERT (responseBufferSize >= 4u);
    uint8_t port_idx = commandPayload[0];
    if (port_idx == 0u || port_idx > motorPortCount)
    {
        return Comm_Status_Error_CommandError;
    }
    
    MotorPort_t* port = &motorPorts[port_idx - 1u];
    MotorLibraryStatus_t result = port->library->GetStatus(port, response, responseCount);

    if (result == MotorLibraryStatus_Ok)
    {
        return Comm_Status_Ok;
    }
    else
    {
        *responseCount = 0u;
        return Comm_Status_Error_InternalError;
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
            /* configuredPort set by SetPortType */
            configuredPort->library->DeInit(configuredPort);
            configuredPort->library = requestedLibrary;
            configuredPort->library->Init(configuredPort);
        }
        else
        {
            /* configuredPort set by SetPortConfig */
            configuredPort->library->UpdateConfiguration(configuredPort);
        }
        
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
void MotorPortHandler_Write_MotorDriveValue(uint8_t driver_idx, uint8_t channel_idx, int8_t value)
{
    (void) port_idx;
    (void) value;
    /* nothing to do */
}

__attribute__((weak))
bool MotorPortHandler_Read_DriverFault(uint8_t port_idx)
{
    (void) port_idx;
    return false;
}

__attribute__((weak))
void* MotorPortHandler_Call_Allocate(size_t size)
{
    (void) size;
    return NULL;
}

__attribute__((weak))
void MotorPortHandler_Call_Free(void** ptr)
{
    ASSERT(ptr);
    *ptr = NULL;
}

__attribute__((weak))
void MotorPortHandler_Write_DriveRequest(uint8_t port_idx, const MotorPort_DriveRequest_t* command)
{
    (void) port_idx;
    ASSERT(command);
    /* nothing to do */
}

__attribute__((weak))
void MotorPortHandler_Read_DriveRequest(uint8_t port_idx, MotorPort_DriveRequest_t* dst)
{
    (void) port_idx;
    ASSERT(dst);
    *dst = (MotorPort_DriveRequest_t) {
        .type = MotorPort_DriveRequest_Power,
        .v.pwm = 0,
        .speed_limit = 0.0f,
        .power_limit = 0.0f
    };
}

__attribute__((weak))
void MotorPort_Write_PortState(uint8_t port_idx, uint8_t* pData, uint8_t dataSize)
{
    (void) port_idx;
    (void) dataSize;
    ASSERT(pData);
}

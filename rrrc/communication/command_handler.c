/*
 * command_handler.c
 *
 * Created: 2019. 04. 11. 10:20:36
 *  Author: Dániel Buga
 */ 

#include "command_handler.h"

#include "../rrrc_hal.h"
#include "../rrrc_i2c_protocol.h"

#include "../rrrc_sensors.h"
#include "../rrrc_motors.h"
#include "../rrrc_indication.h"
#include "../converter.h"

#include <string.h>

/* Misc */
static RRRC_I2C_Status_t Command_Dummy(const request_t* request, response_t* response)
{
    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Echo(const request_t* request, response_t* response)
{
    if (request->bufferSize <= response->bufferSize)
    {
        memcpy(response->buffer, request->buffer, request->bufferSize);
        return RRRC_I2C_STATUS_OK;
    }
    else
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
}

static RRRC_I2C_Status_t Command_SysMon_GetStatus(const request_t* request, response_t* response)
{
    response->bufferCount = SysMonGetValues(response->buffer);

    return RRRC_I2C_STATUS_OK;
}

/* Sensor */
static RRRC_I2C_Status_t Command_Sensor_GetPortAmount(const request_t* request, response_t* response)
{
    response->buffer[0] = SensorPortGetPortsAmount();
    response->bufferCount = 1u;

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Sensor_GetTypes(const request_t* request, response_t* response)
{
    response->bufferCount = SensorPortGetTypes(response->buffer, response->bufferSize);

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Sensor_GetType(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    if (portIdx >= SENSOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
    response->buffer[0] = SensorPortGetType(portIdx);
    response->bufferCount = 1u;

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Sensor_SetType(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    uint8_t type    = request->buffer[1];
    if (portIdx >= SENSOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }

    int32_t status = SensorPortSetType(portIdx, type);
    if (status != ERR_NONE)
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
    else
    {
        return RRRC_I2C_STATUS_OK;
    }
}

static RRRC_I2C_Status_t Command_Sensor_GetValue(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    if (portIdx >= SENSOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
    uint32_t valueCount = SensorPortGetValues(portIdx, response->buffer);
    response->bufferCount = valueCount * sizeof(uint32_t);

    return RRRC_I2C_STATUS_OK;
}

/* Motor */
static RRRC_I2C_Status_t Command_Motor_GetPortAmount(const request_t* request, response_t* response)
{
    response->buffer[0] = MotorPortGetPortsAmount();
    response->bufferCount = 1u;

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Motor_GetTypes(const request_t* request, response_t* response)
{
    response->bufferCount = MotorPortGetTypes(response->buffer, response->bufferSize);

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Motor_GetType(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    if (portIdx >= MOTOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
    response->buffer[0] = MotorPortGetType(portIdx);
    response->bufferCount = 1u;

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Motor_SetType(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    uint8_t type    = request->buffer[1];
    if (portIdx >= MOTOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }

    int32_t status = MotorPortSetType(portIdx, type);
    if (status != ERR_NONE)
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
    else
    {
        return RRRC_I2C_STATUS_OK;
    }
}

static RRRC_I2C_Status_t Command_Motor_GetState(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    if (portIdx >= MOTOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
    response->buffer[0] = MotorPortGetState(portIdx);
    response->bufferCount = 1u;

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Motor_GetPosition(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    if (portIdx >= MOTOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
    response->bufferCount = MotorPortGetPosition(portIdx, response->buffer);

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Motor_SetControl(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    if (portIdx >= MOTOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
    int32_t state = get_int32(&request->buffer[1]);
    int32_t status = MotorPortSetControl(portIdx, state);
    if (status != ERR_NONE)
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
    else
    {
        return RRRC_I2C_STATUS_OK;
    }
}

static RRRC_I2C_Status_t Command_Motor_SetConfig(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    if (portIdx >= MOTOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
    int32_t status = MotorPortSetConfig(portIdx, &request->buffer[1], request->bufferSize - 1u);
    if (status != ERR_NONE)
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
    else
    {
        return RRRC_I2C_STATUS_OK;
    }
}

/* Indication */
static RRRC_I2C_Status_t Command_Indication_GetRingLedAmount(const request_t* request, response_t* response)
{
    response->buffer[0] = IndicationGetRingLedsAmount();
    response->bufferCount = 1u;
    
    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Indication_GetStatusLedAmount(const request_t* request, response_t* response)
{
    response->buffer[0] = IndicationGetStatusLedsAmount();
    response->bufferCount = 1u;

    return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Indication_SetRingScenario(const request_t* request, response_t* response)
{
    uint8_t scenarioIdx = request->buffer[0];
    int32_t status = IndicationSetRingType(scenarioIdx);
    if (status != ERR_NONE)
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
    else
    {
        return RRRC_I2C_STATUS_OK;
    }
}

static RRRC_I2C_Status_t Command_Indication_SetRingUserFrame(const request_t* request, response_t* response)
{
    uint32_t frame_idx = request->buffer[0];
    led_ring_frame_t* led_frame = &request->buffer[1];
    uint32_t status = IndicationUpdateUserFrame(frame_idx, led_frame);

    if (status != ERR_NONE)
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
    else
    {
        return RRRC_I2C_STATUS_OK;
    }
}

static RRRC_I2C_Status_t Command_Indication_SetStatusLed(const request_t* request, response_t* response)
{
    uint32_t led_idx = request->buffer[0];
    p_led_val_t led_rgb = &request->buffer[1];
    uint32_t status = IndicationSetStatusLed(led_idx, led_rgb);
    
    if (status != ERR_NONE)
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
    else
    {
        return RRRC_I2C_STATUS_OK;
    }
}

static const CommandHandler_t commandHandlers[RRRC_I2C_COMMAND_COUNT] = 
{
    /* Misc commands */
    [RRRC_I2C_CMD_DUMMY]           = { .handler = &Command_Dummy,            .minPayloadLength = 0u, .maxPayloadLength = 255u },
    [RRRC_I2C_CMD_ECHO]            = { .handler = &Command_Echo,             .minPayloadLength = 0u, .maxPayloadLength = 255u },
    [RRRC_I2C_CMD_SYSMON_GET_STAT] = { .handler = &Command_SysMon_GetStatus, .minPayloadLength = 0u, .maxPayloadLength = 0u },

    /* Sensors */
    [RRRC_I2C_CMD_SENSOR_GET_PORT_AMOUNT] = { .handler = &Command_Sensor_GetPortAmount, .minPayloadLength = 0u, .maxPayloadLength = 0u },
    [RRRC_I2C_CMD_SENSOR_GET_TYPES]       = { .handler = &Command_Sensor_GetTypes,      .minPayloadLength = 0u, .maxPayloadLength = 0u },
    [RRRC_I2C_CMD_SENSOR_SET_TYPE]        = { .handler = &Command_Sensor_SetType,       .minPayloadLength = 2u, .maxPayloadLength = 2u },
    [RRRC_I2C_CMD_SENSOR_GET_TYPE]        = { .handler = &Command_Sensor_GetType,       .minPayloadLength = 1u, .maxPayloadLength = 1u },
    [RRRC_I2C_CMD_SENSOR_GET_VALUE]       = { .handler = &Command_Sensor_GetValue,      .minPayloadLength = 1u, .maxPayloadLength = 1u },

    /* Motors */
    [RRRC_I2C_CMD_MOTOR_GET_PORT_AMOUNT] = { .handler = &Command_Motor_GetPortAmount, .minPayloadLength = 0u, .maxPayloadLength = 0u },
    [RRRC_I2C_CMD_MOTOR_GET_TYPES]       = { .handler = &Command_Motor_GetTypes,      .minPayloadLength = 0u, .maxPayloadLength = 0u },
    [RRRC_I2C_CMD_MOTOR_GET_TYPE]        = { .handler = &Command_Motor_GetType,       .minPayloadLength = 1u, .maxPayloadLength = 1u },
    [RRRC_I2C_CMD_MOTOR_SET_TYPE]        = { .handler = &Command_Motor_SetType,       .minPayloadLength = 2u, .maxPayloadLength = 2u },
    [RRRC_I2C_CMD_MOTOR_SET_CONTROL]     = { .handler = &Command_Motor_SetControl,    .minPayloadLength = 5u, .maxPayloadLength = 5u },
    [RRRC_I2C_CMD_MOTOR_GET_STATE]       = { .handler = &Command_Motor_GetState,      .minPayloadLength = 1u, .maxPayloadLength = 1u },
    [RRRC_I2C_CMD_MOTOR_GET_POSITION]    = { .handler = &Command_Motor_GetPosition,   .minPayloadLength = 1u, .maxPayloadLength = 1u },
    [RRRC_I2C_CMD_MOTOR_SET_CONFIG]      = { .handler = &Command_Motor_SetConfig,     .minPayloadLength = 1u, .maxPayloadLength = 255u },

    /* Indication */
    [RRRC_I2C_CMD_INDICATION_GET_RING_LEDS_AMOUNT]   = { .handler = &Command_Indication_GetRingLedAmount,   .minPayloadLength = 0u, .maxPayloadLength = 0u },
    [RRRC_I2C_CMD_INDICATION_GET_STATUS_LEDS_AMOUNT] = { .handler = &Command_Indication_GetStatusLedAmount, .minPayloadLength = 0u, .maxPayloadLength = 0u },
    [RRRC_I2C_CMD_INDICATION_SET_RING_SCENARIO]      = { .handler = &Command_Indication_SetRingScenario,    .minPayloadLength = 1u, .maxPayloadLength = 1u },
    [RRRC_I2C_CMD_INDICATION_SET_RING_USER_FRAME]    = { .handler = &Command_Indication_SetRingUserFrame,   .minPayloadLength = 1u + sizeof(led_ring_frame_t), .maxPayloadLength = 1u + sizeof(led_ring_frame_t) },
    [RRRC_I2C_CMD_INDICATION_SET_STATUS_LED]         = { .handler = &Command_Indication_SetStatusLed,       .minPayloadLength = 1u + sizeof(led_val_t),        .maxPayloadLength = 1u + sizeof(led_val_t) }
};

static bool isCommandChecksumValid(const commandBuffer_t* commandBuffer)
{
    /* TODO: implement */
    return true;
}

static bool isValidCommand(uint8_t command)
{
    return command < RRRC_I2C_COMMAND_COUNT && commandHandlers[command].handler != NULL;
}

static bool isPayloadLengthCorrect(const commandBuffer_t* commandBuffer, const CommandHandler_t* handler)
{
    return handler->minPayloadLength <= commandBuffer->header.payloadLength && commandBuffer->header.payloadLength <= handler->maxPayloadLength;
}

size_t CommandHandler_Handle(const commandBuffer_t* commandBuffer, responseBuffer_t* responseBuffer)
{
    request_t request   = {
        .buffer      = commandBuffer->payload,
        .bufferSize  = commandBuffer->header.payloadLength
    };
    
    response_t response = {
        .buffer      = responseBuffer->payload,
        .bufferSize  = ARRAY_SIZE(responseBuffer->payload),
        .bufferCount = 0u
    };

    RRRC_I2C_Status_t responseStatus = RRRC_I2C_STATUS_ERROR_OTHER;
    if (isCommandChecksumValid(commandBuffer))
    {
        if (isValidCommand(commandBuffer->header.command))
        {
            const CommandHandler_t* handler = &commandHandlers[commandBuffer->header.command];
            if (isPayloadLengthCorrect(commandBuffer, handler))
            {
                responseStatus = handler->handler(&request, &response);
            }
            else
            {
                responseStatus = RRRC_I2C_STATUS_ERROR_PAYLOAD_LENGTH;
            }
        }
        else
        {
            /* respond with unknown command */
            responseStatus = RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND;
        }
    }
    else
    {
        responseStatus = RRRC_I2C_STATUS_ERROR_CHECKSUM;
    }
    
    responseBuffer->header.status        = responseStatus;
    responseBuffer->header.payloadLength = response.bufferCount;
    responseBuffer->header.crc           = 0u; /* CRC  */

    return response.bufferCount + sizeof(responseBuffer->header);
}

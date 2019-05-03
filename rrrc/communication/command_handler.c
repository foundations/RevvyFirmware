/*
 * command_handler.c
 *
 * Created: 2019. 04. 11. 10:20:36
 *  Author: D�niel Buga
 */ 

#include "command_handler.h"

#include "../rrrc_hal.h"
#include "../rrrc_i2c_protocol.h"

#include "../rrrc_sensors.h"
#include "../rrrc_motors.h"
#include "../rrrc_sysmon.h"
#include "../rrrc_indication.h"
#include "../utils/converter.h"

#include "../components/RingLedDisplay/RingLedDisplay.h"

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
    response->bufferCount = SysMonGetValues((uint32_t*) response->buffer);

    return RRRC_I2C_STATUS_OK;
}

/* Sensor */
static RRRC_I2C_Status_t Command_Sensor_GetPortAmount(const request_t* request, response_t* response)
{
    response->buffer[0] = SensorPortGetPortsAmount();
    response->bufferCount = 1u;

    return RRRC_I2C_CMD_SENSOR_GET_PORT_AMOUNT;//return RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Sensor_GetTypes(const request_t* request, response_t* response)
{
    response->bufferCount = SensorPortGetTypes(response->buffer, response->bufferSize);

    return RRRC_I2C_CMD_SENSOR_GET_TYPES;//RRRC_I2C_STATUS_OK;
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

    return RRRC_I2C_CMD_SENSOR_GET_TYPE;//RRRC_I2C_STATUS_OK;
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
    uint32_t valueCount = SensorPortGetValues(portIdx, (uint32_t*) response->buffer);
    response->bufferCount = valueCount * sizeof(uint32_t);

    return RRRC_I2C_CMD_SENSOR_GET_VALUE;//RRRC_I2C_STATUS_OK;
}

/* Motor */
static RRRC_I2C_Status_t Command_Motor_GetPortAmount(const request_t* request, response_t* response)
{
    response->buffer[0] = MotorPortGetPortsAmount();
    response->bufferCount = 1u;

    return RRRC_I2C_CMD_MOTOR_GET_PORT_AMOUNT;//RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Motor_GetTypes(const request_t* request, response_t* response)
{
    response->bufferCount = MotorPortGetTypes(response->buffer, response->bufferSize);

    return RRRC_I2C_CMD_MOTOR_GET_TYPES;//RRRC_I2C_STATUS_OK;
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

    return RRRC_I2C_CMD_MOTOR_GET_TYPE;//RRRC_I2C_STATUS_OK;
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

    return RRRC_I2C_CMD_MOTOR_GET_STATE;//RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Motor_GetPosition(const request_t* request, response_t* response)
{
    uint8_t portIdx = request->buffer[0];
    if (portIdx >= MOTOR_PORT_AMOUNT)
    {
        return RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT;
    }
    response->bufferCount = MotorPortGetPosition(portIdx, (uint32_t*) response->buffer);

    return RRRC_I2C_CMD_MOTOR_GET_POSITION;//RRRC_I2C_STATUS_OK;
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
    
    return RRRC_I2C_CMD_INDICATION_GET_RING_LEDS_AMOUNT;//RRRC_I2C_STATUS_OK;
}

static RRRC_I2C_Status_t Command_Indication_GetStatusLedAmount(const request_t* request, response_t* response)
{
    response->buffer[0] = IndicationGetStatusLedsAmount();
    response->bufferCount = 1u;

    return RRRC_I2C_CMD_INDICATION_GET_STATUS_LEDS_AMOUNT; //RRRC_I2C_STATUS_OK;
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
    led_ring_frame_t* led_frame = (led_ring_frame_t*) &request->buffer[1];
    bool success = RingLedDisplay_Run_AddUserFrame(led_frame, RING_LEDS_AMOUNT);

    if (success)
    {
        return RRRC_I2C_STATUS_OK;
    }
    else
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
}

static RRRC_I2C_Status_t Command_Indication_SetStatusLed(const request_t* request, response_t* response)
{
    uint32_t led_idx = request->buffer[0];
    //p_led_val_t led_rgb = (p_led_val_t) &request->buffer[1];
    int32_t status = ERR_NONE;//IndicationSetStatusLed(led_idx, led_rgb);
    
    if (status != ERR_NONE)
    {
        return RRRC_I2C_STATUS_ERROR_OTHER;
    }
    else
    {
        return RRRC_I2C_STATUS_OK;
    }
}

typedef struct 
{
    bool hasData;
    uint8_t delay;
    uint8_t count;
    uint8_t data[253];
} LongEcho_Ctx_t;

static LongEcho_Ctx_t longEchoData;

LongCommand_Start_Status_t LongCommand_Echo_Start(const request_t* request, void* privateData)
{
    LongEcho_Ctx_t* data = (LongEcho_Ctx_t*) privateData;

    if (request->bufferSize > sizeof(data->data))
    {
        return LongCommand_Start_InputError;
    }
    else
    {
        /* store data and delay */
        data->delay = request->buffer[0];
        memcpy(data->data, &request->buffer[1], request->bufferSize - 1u);
        data->count = request->bufferSize - 1u;
        data->hasData = true;

        return LongCommand_Start_Successful;
    }
}

LongCommand_GetResult_Status_t LongCommand_Echo_GetResult(response_t* response, void* privateData)
{
    LongEcho_Ctx_t* data = (LongEcho_Ctx_t*) privateData;

    if (!data->hasData)
    {
        return LongCommand_GetResult_Error_NotStarted;
    }
    else if (data->delay > 0u)
    {
        data->delay--;
        return LongCommand_GetResult_Pending;
    }
    else
    {
        memcpy(response->buffer, data->data, data->count);
        response->bufferCount = data->count;

        // data->hasData = false; // behaviour depends on interpretation: allow multiple reading? yes -> don't set to false

        return LongCommand_GetResult_Done;
    }
}

void LongCommand_Echo_Cancel(void* privateData)
{
    LongEcho_Ctx_t* data = (LongEcho_Ctx_t*) privateData;
    
    data->hasData = false;
}

/* Long commands */
static const LongCommandHandler_t longCommandHandlers[RRRC_I2C_LONG_COMMAND_COUNT] = 
{
    [RRRC_I2C_LONG_CMD_ECHO] = { .privateData = &longEchoData, .start = &LongCommand_Echo_Start, .getResult = &LongCommand_Echo_GetResult, .cancel = &LongCommand_Echo_Cancel }
};

static RRRC_I2C_Status_t LongCommand_Start(const request_t* request, response_t* response)
{
    uint8_t commandIdx = request->buffer[0];
    if (commandIdx >= RRRC_I2C_LONG_COMMAND_COUNT)
    {
        return RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND;
    }

    const LongCommandHandler_t* command = &longCommandHandlers[commandIdx];
    if (command->start == NULL)
    {
        return RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND;
    }

    const uint8_t offset = 1u;
    request_t subrequest = {
        .buffer = &request->buffer[offset],
        .bufferSize = request->bufferSize - offset
    };
    LongCommand_Start_Status_t status = command->start(&subrequest, command->privateData);
    switch (status)
    {
        case LongCommand_Start_Successful:
            return RRRC_I2C_CMD_LONG_COMMAND_START;
    
        default:
            return RRRC_I2C_STATUS_ERROR_OTHER;
    }
}

static RRRC_I2C_Status_t LongCommand_GetResult(const request_t* request, response_t* response)
{
    uint8_t commandIdx = request->buffer[0];
    if (commandIdx >= RRRC_I2C_LONG_COMMAND_COUNT)
    {
        return RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND;
    }

    const LongCommandHandler_t* command = &longCommandHandlers[commandIdx];
    if (command->getResult == NULL)
    {
        return RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND;
    }

    LongCommand_GetResult_Status_t status = command->getResult(response, command->privateData);
    switch (status)
    {
        case LongCommand_GetResult_Pending:
            response->bufferCount = 0u;
            return RRRC_I2C_STATUS_PENDING;

        case LongCommand_GetResult_Done:
            return RRRC_I2C_CMD_LONG_COMMAND_GET_RESULT;
        
        default:
            response->bufferCount = 0u;
            return RRRC_I2C_STATUS_ERROR_OTHER;
    }
}

static RRRC_I2C_Status_t LongCommand_Cancel(const request_t* request, response_t* response)
{
    uint8_t commandIdx = request->buffer[0];
    if (commandIdx >= RRRC_I2C_LONG_COMMAND_COUNT)
    {
        return RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND;
    }

    const LongCommandHandler_t* command = &longCommandHandlers[commandIdx];
    if (command->cancel == NULL)
    {
        return RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND;
    }

    command->cancel(command->privateData);

    response->bufferCount = 0u;

    return RRRC_I2C_STATUS_OK;
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
    [RRRC_I2C_CMD_INDICATION_SET_STATUS_LED]         = { .handler = &Command_Indication_SetStatusLed,       .minPayloadLength = 1u/* + sizeof(led_val_t)*/,        .maxPayloadLength = 1u/* + sizeof(led_val_t)*/ },

    /* Long commands */
    [RRRC_I2C_CMD_LONG_COMMAND_START]      = { .handler = &LongCommand_Start,     .minPayloadLength = 1u, .maxPayloadLength = 255u },
    [RRRC_I2C_CMD_LONG_COMMAND_GET_RESULT] = { .handler = &LongCommand_GetResult, .minPayloadLength = 1u, .maxPayloadLength =   1u },
    [RRRC_I2C_CMD_LONG_COMMAND_CANCEL]     = { .handler = &LongCommand_Cancel,    .minPayloadLength = 1u, .maxPayloadLength =   1u },
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

/*
 * CommunicationManager.c
 *
 * Created: 2019. 05. 06. 18:02:54
 *  Author: Dániel Buga
 */ 

#include "CommunicationManager.h"
#include "utils_assert.h"

#include "utils/crc.h"
#include <stdbool.h>

static const Comm_CommandHandler_t* comm_commandTable = NULL;
static size_t comm_commandTableSize = 0u;

static bool _checkChecksum(const Comm_Command_t* command)
{
    /* calculate CRC, skipping the CRC field */
    uint16_t headerCrc = CRC16_Calculate(0xFFFFu, (const uint8_t*) command, sizeof(Comm_Operation_t) + sizeof(uint8_t) + sizeof(uint8_t));
    uint16_t messageCrc = CRC16_Calculate(headerCrc, (const uint8_t*) command->payload, command->payloadLength);

    return command->checksum == messageCrc;
}

void Comm_Init(const Comm_CommandHandler_t* commandTable, size_t commandTableSize)
{
    ASSERT(commandTable);

    if (comm_commandTable != NULL)
    {
        for (size_t i = 0u; i < comm_commandTableSize; i++)
        {
            comm_commandTable[i].Cancel();
        }
    }

    comm_commandTable     = commandTable;
    comm_commandTableSize = commandTableSize;
}

static Comm_Status_t _handleOperation_Cancel(const Comm_Command_t* command)
{
    if (comm_commandTable[command->command].Cancel != NULL)
    {
        comm_commandTable[command->command].Cancel();
    }

    return Comm_Status_Ok;
}

static Comm_Status_t _handleOperation_GetResult(const Comm_Command_t* command, uint8_t* responseBuffer, uint8_t payloadBufferSize, uint8_t* responseLength)
{
    if (comm_commandTable[command->command].GetResult == NULL)
    {
        return Comm_Status_Error_InternalError;
    }
    else
    {
        return comm_commandTable[command->command].GetResult(responseBuffer, payloadBufferSize, responseLength);
    }
}

static Comm_Status_t _handleOperation_Start(const Comm_Command_t* command, uint8_t* responseBuffer, uint8_t payloadBufferSize, uint8_t* responseLength)
{
    Comm_Status_t resultStatus = comm_commandTable[command->command].Start((const uint8_t*) command->payload, command->payloadLength, responseBuffer, payloadBufferSize, responseLength);
    if (resultStatus == Comm_Status_Pending)
    {
        return _handleOperation_GetResult(command, responseBuffer, payloadBufferSize, responseLength);
    }
    else
    {
        return resultStatus;
    }
}

/**
 * Handle a request and prepare a response
 *
 * @return the response length in bytes
 */
size_t Comm_Handle(const Comm_Command_t* command, Comm_Response_t* response, size_t responseBufferSize)
{
    const size_t responseHeaderSize = sizeof(Comm_Status_t) + sizeof(uint8_t) + sizeof(uint16_t);
    
    ASSERT(responseBufferSize > responseHeaderSize);
    ASSERT((responseBufferSize - responseHeaderSize) < 256u); /* protocol limitation */
    uint8_t payloadBufferSize = responseBufferSize - responseHeaderSize;
    uint8_t payloadSize = 0u;
    Comm_Status_t resultStatus;

    if (!_checkChecksum(command))
    {
        /* integrity error */
        resultStatus = Comm_Status_Error_IntegrityError;
    }
    else if (command->command >= comm_commandTableSize || comm_commandTable[command->command].Start == NULL)
    {
        /* unimplemented command */
        resultStatus = Comm_Status_Error_UnknownCommand;
    }
    else
    {
        switch (command->operation)
        {
            case Comm_Operation_Cancel:
                resultStatus = _handleOperation_Cancel(command);
                break;

            case Comm_Operation_Start:
                resultStatus = _handleOperation_Start(command, (uint8_t*) response->payload, payloadBufferSize, &payloadSize);
                break;

            case Comm_Operation_Restart:
                (void) _handleOperation_Cancel(command);
                resultStatus = _handleOperation_Start(command, (uint8_t*) response->payload, payloadBufferSize, &payloadSize);
                break;

            case Comm_Operation_GetResult:
                resultStatus = _handleOperation_GetResult(command, (uint8_t*) response->payload, payloadBufferSize, &payloadSize);
                break;

            default:
                resultStatus = Comm_Status_Error_UnknownOperation;
                break;
        }
    }

    /* detect overwrite */
    if (payloadSize > payloadBufferSize)
    {
        resultStatus = Comm_Status_Error_InternalError;
    }

    /* only certain responses may contain a payload */
    if (resultStatus != Comm_Status_Ok && resultStatus != Comm_Status_Error_CommandError)
    {
        payloadSize = 0u;
    }

    /* fill header */
    response->status = resultStatus;
    response->payloadLength = payloadSize;

    return payloadSize + responseHeaderSize;
}

void Comm_Protect(Comm_Response_t* response)
{
    /* calculate CRC, skipping the CRC field */
    uint16_t headerCrc = CRC16_Calculate(0xFFFFu, (const uint8_t*) response, sizeof(Comm_Status_t) + sizeof(uint8_t));
    uint16_t messageCrc = CRC16_Calculate(headerCrc, (const uint8_t*) response->payload, response->payloadLength);

    response->checksum = messageCrc;
}

/*
 * MasterCommunication.c
 *
 * Created: 2019. 05. 06. 20:16:07
 *  Author: D�niel Buga
 */ 

#include "MasterCommunication.h"
#include "utils/crc.h"

static uint8_t responseBuffer[256];

static Comm_ResponseHeader_t defaultResponse = 
{
    .status = Comm_Status_Busy,
    .payloadLength = 0u
};

static Comm_ResponseHeader_t longRxErrorResponse = 
{
    .status = Comm_Status_Error_PayloadLengthError,
    .payloadLength = 0u
};

void MasterCommunication_Run_GetDefaultResponse(uint8_t** defaultResponseBuffer, size_t* defaultResponseLength)
{
    *defaultResponseBuffer = (uint8_t*) &defaultResponse;
    *defaultResponseLength = sizeof(defaultResponse);
}

void MasterCommunication_Run_GetLongRxErrorResponse(uint8_t** longRxErrorResponseBuffer, size_t* longRxErrorResponseLength)
{
    *longRxErrorResponseBuffer = (uint8_t*) &longRxErrorResponse;
    *longRxErrorResponseLength = sizeof(longRxErrorResponse);
}

void MasterCommunication_Run_OnInit(const Comm_CommandHandler_t* commandTable, size_t commandTableSize)
{
    defaultResponse.checksum     = CRC16_Calculate(0xFFFFu, (uint8_t*) &defaultResponse, sizeof(Comm_ResponseHeader_t) - sizeof(uint16_t));
    longRxErrorResponse.checksum = CRC16_Calculate(0xFFFFu, (uint8_t*) &longRxErrorResponse, sizeof(Comm_ResponseHeader_t) - sizeof(uint16_t));

    Comm_Init(commandTable, commandTableSize);
}

void MasterCommunication_Run_HandleCommand(const uint8_t* buffer, size_t bufferSize)
{
    const Comm_Command_t* command = (const Comm_Command_t*) buffer;
    const size_t headerSize = sizeof(Comm_Operation_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t);

    size_t responseCount;
    Comm_Response_t* response = (Comm_Response_t*) responseBuffer;

    if (command->payloadLength + headerSize != bufferSize)
    {
        response->status = Comm_Status_Error_PayloadLengthError;
        responseCount = sizeof(Comm_Status_t) + sizeof(uint8_t) + sizeof(uint16_t);
    }
    else
    {
        responseCount = Comm_Handle(command, response, sizeof(responseBuffer));
    }

    Comm_Protect(response);
    MasterCommunication_Call_SendResponse(&responseBuffer[0], responseCount);
}

__attribute__((weak))
void MasterCommunication_Call_SendResponse(const uint8_t* responseBuffer, size_t responseSize)
{
    /* nothing to do */
}

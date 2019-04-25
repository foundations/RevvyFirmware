/*
 * command_handler.h
 *
 * Created: 2019. 04. 11. 10:10:41
 *  Author: Dániel Buga
 */ 


#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

#include <stdio.h>

#include "rrrc_i2c_protocol.h"

typedef struct 
{
    const uint8_t* buffer;
    uint8_t bufferSize;
} request_t;

typedef struct 
{
    uint8_t* buffer;
    const uint8_t bufferSize;
    uint8_t bufferCount;
} response_t;

typedef RRRC_I2C_Status_t (*CommandHandlerFn_t)(const request_t* request, response_t* response);
typedef struct 
{
    CommandHandlerFn_t handler;
    uint8_t minPayloadLength;
    uint8_t maxPayloadLength;
} CommandHandler_t;

size_t CommandHandler_Handle(const commandBuffer_t* commandBuffer, responseBuffer_t* responseBuffer);

#endif /* COMMAND_HANDLER_H_ */
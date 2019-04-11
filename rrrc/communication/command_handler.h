/*
 * command_handler.h
 *
 * Created: 2019. 04. 11. 10:10:41
 *  Author: Dániel Buga
 */ 


#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

#include <stdio.h>

typedef int32_t (*CommandHandlerFunc_t)(const uint8_t* pData, size_t size);
typedef size_t (*CommandResponseFunc_t)(uint8_t command, uint8_t* pData, size_t bufSize);
typedef struct 
{
	CommandHandlerFunc_t commandHandler;
	CommandResponseFunc_t responseHandler;
} CommandHandler_t;

size_t CommandHandler_Handle(const uint8_t* command, size_t commandSize, uint8_t* pData, size_t bufSize);

#endif /* COMMAND_HANDLER_H_ */
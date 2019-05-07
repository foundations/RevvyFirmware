/*
 * MasterCommunication.h
 *
 * Created: 2019. 05. 06. 20:15:46
 *  Author: D�niel Buga
 */ 


#ifndef MASTER_COMMUNICATION_H_
#define MASTER_COMMUNICATION_H_

#include "CommunicationManager.h"

void MasterCommunication_Run_OnInit(const Comm_CommandHandler_t* commandTable, size_t commandTableSize);
void MasterCommunication_Run_HandleCommand(const uint8_t* buffer, size_t bufferSize);
void MasterCommunication_Call_SendResponse(const uint8_t* responseBuffer, size_t responseSize);

#endif /* MASTER_COMMUNICATION_H_ */

/*
 * MasterCommunication.h
 *
 * Created: 2019. 05. 06. 20:15:46
 *  Author: Dániel Buga
 */ 


#ifndef MASTER_COMMUNICATION_H_
#define MASTER_COMMUNICATION_H_

#include "../communication/CommunicationManager.h"

void MasterCommunication_Run_OnInit(const Comm_CommandHandler_t* commandTable, size_t commandTableSize);

#endif /* MASTER_COMMUNICATION_H_ */

/*
 * MasterStatusObserver.h
 *
 * Created: 2019. 05. 07. 21:43:05
 *  Author: Dániel Buga
 */ 


#ifndef MASTER_STATUS_OBSERVER_H_
#define MASTER_STATUS_OBSERVER_H_

#include "../MasterCommunication/CommunicationManager.h"

typedef enum 
{
    MasterStatus_Unknown,
    MasterStatus_Operational,
    MasterStatus_Controlled
} MasterStatus_t;

void MasterStatusObserver_Run_OnInit(void);
Comm_Status_t MasterStatusObserver_SetMasterStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
void MasterStatusObserver_Write_MasterStatus(MasterStatus_t status);

#endif /* MASTER_STATUS_OBSERVER_H_ */

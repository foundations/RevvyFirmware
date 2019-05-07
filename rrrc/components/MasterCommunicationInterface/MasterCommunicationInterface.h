/*
 * MasterCommunicationInterface.h
 *
 * Created: 07/05/2019 10:30:22
 *  Author: D�niel Buga
 */ 

#ifndef MASTER_COMMUNICATION_INTERFACE_H_
#define MASTER_COMMUNICATION_INTERFACE_H_

#include <stdio.h>

typedef struct 
{
    uint8_t* defaultResponseBuffer;
    size_t defaultResponseLength;

    uint8_t* longRxErrorResponseBuffer;
    size_t longRxErrorResponseLength;

    uint32_t rxTimeout;
} MasterCommunicationInterface_Config_t;

void MasterCommunicationInterface_Run_OnInit(const MasterCommunicationInterface_Config_t* config);
void MasterCommunicationInterface_Run_SetResponse(const uint8_t* buffer, size_t bufferSize);

void MasterCommunicationInterface_Call_OnMessageReceived(const uint8_t* buffer, size_t bufferSize);
void MasterCommunicationInterface_Call_RxTimeout(void);

#endif /* MASTER_COMMUNICATION_INTERFACE_H_ */

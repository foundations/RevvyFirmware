/*
 * McuStatusCollectorWrapper.h
 *
 * Created: 2019. 07. 12. 11:20:57
 *  Author: Dániel Buga
 */ 

#ifndef COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_
#define COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_

#include "../MasterCommunication/CommunicationManager.h"

Comm_Status_t McuStatusCollectorWrapper_Reset_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t McuStatusCollectorWrapper_ControlSlot_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t McuStatusCollectorWrapper_ReadStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

void McuStatusCollectorWrapper_Run_ResetSlots(void);
void McuStatusCollectorWrapper_Run_EnableSlot(uint8_t slot);
void McuStatusCollectorWrapper_Run_DisableSlot(uint8_t slot);
void McuStatusCollectorWrapper_Run_ReadData(uint8_t* pData, uint8_t bufferSize, uint8_t* dataSize);

#endif /* COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_ */

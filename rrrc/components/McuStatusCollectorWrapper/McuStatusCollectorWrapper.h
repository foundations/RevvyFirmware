/*
 * McuStatusCollectorWrapper.h
 *
 * Created: 2019. 07. 12. 11:20:57
 *  Author: D�niel Buga
 */ 

#ifndef COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_
#define COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_

#ifndef COMPONENT_TYPES_MCU_STATUS_COLLECTOR_WRAPPER_H_
#define COMPONENT_TYPES_MCU_STATUS_COLLECTOR_WRAPPER_H_

#include <stdint.h>
#include "../MasterCommunication/CommunicationManager.h"

typedef struct {
    uint8_t* bytes;
    size_t count;
} ByteArray_t;

#endif

Comm_Status_t McuStatusCollectorWrapper_Reset_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t McuStatusCollectorWrapper_ControlSlot_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t McuStatusCollectorWrapper_ReadStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

void McuStatusCollectorWrapper_Call_ResetSlots(void);
void McuStatusCollectorWrapper_Call_EnableSlot(uint8_t slot);
void McuStatusCollectorWrapper_Call_DisableSlot(uint8_t slot);
uint8_t McuStatusCollectorWrapper_Call_Read(ByteArray_t destination);

#endif /* COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_ */

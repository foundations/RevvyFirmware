/*
 * McuStatusCollectorWrapper.h
 *
 * Created: 2019. 07. 12. 11:20:57
 *  Author: Dániel Buga
 */ 

#ifndef COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_
#define COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_

Comm_Status_t McuStatusCollector_EnableSlot_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t McuStatusCollector_ReadStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

#endif /* COMPONENTS_MCU_STATUS_COLLECTOR_WRAPPER_H_ */

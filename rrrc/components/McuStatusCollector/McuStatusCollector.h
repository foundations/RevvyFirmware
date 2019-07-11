/*
 * McuStatusCollector.h
 *
 * Created: 2019. 07. 11. 17:31:14
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_MCU_STATUS_COLLECTOR_H_
#define COMPONENT_MCU_STATUS_COLLECTOR_H_

#include <stdint.h>

void McuStatusCollector_Run_ResetSlots(void);
void McuStatusCollector_Run_EnableSlot(uint8_t slot);
void McuStatusCollector_Run_DisableSlot(uint8_t slot);
void McuStatusCollector_Run_ReadData(uint8_t* pData, uint8_t bufferSize, uint8_t* dataSize);

void McuStatusCollector_Call_ClearSlotData(uint8_t slot);
void McuStatusCollector_Read_SlotData(uint8_t slot, uint8_t* pData, uint8_t bufferSize, uint8_t* slotDataSize);

#endif /* COMPONENT_MCU_STATUS_COLLECTOR_H_ */

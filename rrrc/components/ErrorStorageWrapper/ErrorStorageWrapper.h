/*
 * ErrorStorageWrapper.h
 *
 * Created: 2019. 07. 19. 10:31:11
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENTS_ERROR_STORAGE_WRAPPER_H_
#define COMPONENTS_ERROR_STORAGE_WRAPPER_H_

#include "../MasterCommunication/CommunicationManager.h"
#include "../ErrorStorage/ErrorStorage.h"

Comm_Status_t ErrorStorageWrapper_ReadCount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t ErrorStorageWrapper_ReadErrors_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t ErrorStorageWrapper_ClearMemory_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t ErrorStorageWrapper_StoreTestError_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

bool ErrorStorageWrapper_Call_Read(uint32_t index, ErrorInfo_t* pDst);
void ErrorStorageWrapper_Call_ClearMemory(void);
uint32_t ErrorStorageWrapper_Read_NumberOfStoredErrors(void);

#endif /* COMPONENTS_ERROR_STORAGE_WRAPPER_H_ */

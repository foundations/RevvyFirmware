/*
 * ErrorStorage.h
 *
 * Created: 2019. 07. 18. 8:59:14
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENTS_ERROR_STORAGE_H_
#define COMPONENTS_ERROR_STORAGE_H_

/*
 * This component is responsible for storing detected errors into the internal flash.
 *
 * Error storage must be resiliant against power loss and shall include a reasonable amount of wear leveling.
 * Error storage is not intended to implement a complete filesystem.
 */
 
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    uint32_t base_address;
    uint16_t allocated;
    uint16_t deleted;
} BlockInfo_t;

typedef struct {
    uint8_t error_id;
    uint8_t data[30];
} ErrorInfo_t;

void ErrorStorage_Run_OnInit(BlockInfo_t* blocks, size_t num_blocks);
void ErrorStorage_Run_Store(const ErrorInfo_t* data);
void ErrorStorage_Run_Clear(void);

void ErrorStorage_Write_NumberOfStoredErrors(uint32_t number);
bool ErrorStorage_Run_Read(uint32_t index, ErrorInfo_t* pDst);

#endif /* COMPONENTS_ERROR_STORAGE_H_ */

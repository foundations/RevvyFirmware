/*
 * ErrorStorageWrapper.c
 *
 * Created: 2019. 07. 19. 10:31:01
 *  Author: Dániel Buga
 */ 

#include "ErrorStorageWrapper.h"
#include "rrrc_worklogic.h"
#include "utils/converter.h"
#include <string.h>

Comm_Status_t ErrorStorageWrapper_ReadCount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint32_t count = ErrorStorageWrapper_Read_NumberOfStoredErrors();

    memcpy(response, &count, sizeof(uint32_t));
    *responseCount = sizeof(uint32_t);

    return Comm_Status_Ok;
}

Comm_Status_t ErrorStorageWrapper_ReadErrors_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != 4u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint32_t count = ErrorStorageWrapper_Read_NumberOfStoredErrors();
    uint32_t start = get_uint32(commandPayload);

    for (uint32_t i = start; i < count; i++)
    {
        if (*responseCount + sizeof(ErrorInfo_t) <= responseBufferSize)
        {
            ErrorInfo_t error;
            if (ErrorStorageWrapper_Call_Read(i, &error))
            {
                memcpy(&response[*responseCount], &error, sizeof(ErrorInfo_t));
                *responseCount += sizeof(ErrorInfo_t);
            }
        }
        else
        {
            break;
        }
    }

    return Comm_Status_Ok;
}

Comm_Status_t ErrorStorageWrapper_ClearMemory_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    ErrorStorageWrapper_Call_ClearMemory();

    return Comm_Status_Ok;
}

Comm_Status_t ErrorStorageWrapper_StoreTestError_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    ErrorInfo_t error;
    error.error_id = ERROR_ID_TEST_ERROR;
    for (uint32_t i = 0u; i < sizeof(error.data); i++)
    {
        error.data[i] = (uint8_t) (i % 256u);
    }

    ErrorStorage_Run_Store(&error);
    return Comm_Status_Ok;
}

__attribute__((weak))
bool ErrorStorageWrapper_Call_Read(uint32_t index, ErrorInfo_t* pDst)
{
    return false;
}

__attribute__((weak))
void ErrorStorageWrapper_Call_ClearMemory(void)
{
    
}

__attribute__((weak))
uint32_t ErrorStorageWrapper_Read_NumberOfStoredErrors(void)
{
    return 0u;
}

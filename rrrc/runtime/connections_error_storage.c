/*
 * connections_error_storage.c
 *
 * Created: 2019. 07. 27. 16:24:31
 *  Author: Dániel Buga
 */ 

#include "../rrrc_worklogic.h"

static uint32_t number_of_errors = 0u;

bool ErrorStorageWrapper_Call_Read(uint32_t index, ErrorInfo_t* pDst)
{
    __disable_irq();
    bool found = ErrorStorage_Run_Read(index, pDst);
    __enable_irq();

    return found;
}

void ErrorStorageWrapper_Call_ClearMemory(void)
{
    __disable_irq();
    ErrorStorage_Run_Clear();
    __enable_irq();
}

void ErrorStorage_Write_NumberOfStoredErrors(uint32_t number)
{
    number_of_errors = number;
}

uint32_t ErrorStorageWrapper_Read_NumberOfStoredErrors(void)
{
    return number_of_errors;
}

void IMU_Call_LogError(void)
{
    ErrorInfo_t errorInfo = {0};

    errorInfo.error_id = ERROR_ID_IMU_ERROR;

    /* TODO add debug data here */

    ErrorStorage_Run_Store(&errorInfo);
}

/*
 * McuStatusCollectorWrapper.c
 *
 * Created: 2019. 07. 15. 10:09:32
 *  Author: Dániel Buga
 */ 

#include "McuStatusCollectorWrapper.h"

Comm_Status_t McuStatusCollectorWrapper_Reset_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    
    McuStatusCollectorWrapper_Run_ResetSlots();

    return Comm_Status_Ok;
}

/**
 * Enable or disable a given slot
 */
Comm_Status_t McuStatusCollectorWrapper_ControlSlot_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseBufferSize;
    (void) responseCount;

    if (commandSize != 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint8_t slot = commandPayload[0];
    uint8_t state = commandPayload[1];
    if (slot > 31u || state > 1u)
    {
        return Comm_Status_Error_CommandError;
    }

    if (state)
    {
        McuStatusCollectorWrapper_Run_EnableSlot(slot);
    }
    else
    {
        McuStatusCollectorWrapper_Run_DisableSlot(slot);
    }

    return Comm_Status_Ok;
}

Comm_Status_t McuStatusCollectorWrapper_ReadStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    
    McuStatusCollectorWrapper_Run_ReadData(response, responseBufferSize, responseCount);

    return Comm_Status_Ok;
}

__attribute__((weak))
void McuStatusCollectorWrapper_Run_ResetSlots(void)
{

}

__attribute__((weak))
void McuStatusCollectorWrapper_Run_EnableSlot(uint8_t slot)
{
    (void) slot;
}

__attribute__((weak))
void McuStatusCollectorWrapper_Run_DisableSlot(uint8_t slot)
{
    (void) slot;
}

__attribute__((weak))
void McuStatusCollectorWrapper_Run_ReadData(uint8_t* pData, uint8_t bufferSize, uint8_t* dataSize)
{
    (void) bufferSize;
    ASSERT(pData);
    ASSERT(dataSize);

    *dataSize = 0u;
}

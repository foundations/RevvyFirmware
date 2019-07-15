/*
 * VersionProvider.c
 *
 * Created: 09/05/2019 12:52:29
 *  Author: Dániel Buga
 */ 
#include "VersionProvider.h"
#include "fw_version.h"
#include "rrrc_hal.h"

#include <string.h>

static const char* hw_version_strings[] = 
{
    "1.0.0",
    "1.0.1",
    "2.0.0"
};

#define FIRMWARE_VERSION_STRING "0.2." FW_VERSION

Comm_Status_t VersionProvider_GetHardwareVersion_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }
    
    uint32_t hw = FLASH_HEADER->hw_version;

    if (hw < ARRAY_SIZE(hw_version_strings))
    {
        uint8_t len = strlen(hw_version_strings[hw]);
        ASSERT(responseBufferSize >= len);
        memcpy(response, hw_version_strings[hw], len);
        *responseCount = len;
    
        return Comm_Status_Ok;
    }
    else
    {
        return Comm_Status_Error_InternalError;
    }
}

Comm_Status_t VersionProvider_GetFirmwareVersion_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) commandPayload;
    if (commandSize != 0u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    ASSERT(responseBufferSize >= strlen(FIRMWARE_VERSION_STRING));

    memcpy(response, FIRMWARE_VERSION_STRING, strlen(FIRMWARE_VERSION_STRING));
    *responseCount = strlen(FIRMWARE_VERSION_STRING);

    return Comm_Status_Ok;
}

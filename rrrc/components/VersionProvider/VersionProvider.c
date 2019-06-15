/*
 * VersionProvider.c
 *
 * Created: 09/05/2019 12:52:29
 *  Author: D�niel Buga
 */ 
#include "VersionProvider.h"
#include "fw_version.h"
#include "rrrc_hal.h"

#include <string.h>

static const char* hw_version_strings[] = 
{
    "1.0",
    "1.1"
};

#define FIRMWARE_VERSION_STRING "0.1-r" FW_VERSION

Comm_Status_t VersionProvider_GetHardwareVersion_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    uint32_t hw = FLASH_HEADER->hw_version;

    if (hw < ARRAY_SIZE(hw_version_strings))
    {
        uint8_t len = strlen(hw_version_strings[hw]);
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
    memcpy(response, FIRMWARE_VERSION_STRING, strlen(FIRMWARE_VERSION_STRING));
    *responseCount = strlen(FIRMWARE_VERSION_STRING);

    return Comm_Status_Ok;
}

/*
 * VersionProvider.c
 *
 * Created: 09/05/2019 12:52:29
 *  Author: Dániel Buga
 */ 
#include "VersionProvider.h"
#include "fw_version.h"

#include <string.h>

#define HARDWARE_VERSION "1.1" /* TODO read from NVM */
#define FIRMWARE_VERSION_STRING "0.1-r" FW_VERSION

Comm_Status_t VersionProvider_GetHardwareVersion_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    memcpy(response, HARDWARE_VERSION, strlen(HARDWARE_VERSION));
    *responseCount = strlen(HARDWARE_VERSION);

    return Comm_Status_Ok;
}

Comm_Status_t VersionProvider_GetFirmwareVersion_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    memcpy(response, FIRMWARE_VERSION_STRING, strlen(FIRMWARE_VERSION_STRING));
    *responseCount = strlen(FIRMWARE_VERSION_STRING);

    return Comm_Status_Ok;
}

/*
 * FirmwareVersionProvider.c
 *
 * Created: 09/05/2019 12:52:29
 *  Author: Dániel Buga
 */ 
#include "FirmwareVersionProvider.h"

#include <string.h>

#define FIRMWARE_VERSION "1.0.0"
#define FIRMWARE_VERSION_STRING FIRMWARE_VERSION " (" __DATE__ " " __TIME__ ")"

Comm_Status_t FirmwareVersionProvider_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    memcpy(response, FIRMWARE_VERSION_STRING, strlen(FIRMWARE_VERSION_STRING));
    *responseCount = strlen(FIRMWARE_VERSION_STRING);

    return Comm_Status_Ok;
}

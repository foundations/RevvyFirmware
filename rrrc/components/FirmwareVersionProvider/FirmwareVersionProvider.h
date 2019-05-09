/*
 * FirmwareVersionProvider.h
 *
 * Created: 09/05/2019 12:52:45
 *  Author: Dániel Buga
 */
#ifndef FIRMWARE_VERSION_PROVIDER_H_
#define FIRMWARE_VERSION_PROVIDER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../MasterCommunication/CommunicationManager.h"

Comm_Status_t FirmwareVersionProvider_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

#endif /* FIRMWARE_VERSION_PROVIDER_H_ */

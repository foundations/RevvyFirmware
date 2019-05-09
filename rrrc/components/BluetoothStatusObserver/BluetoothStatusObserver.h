/*
 * BluetoothStatusObserver.h
 *
 * Created: 09/05/2019 11:47:12
 *  Author: Dániel Buga
 */ 

#ifndef BLUETOOTH_STATUS_OBSERVER_H_
#define BLUETOOTH_STATUS_OBSERVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../MasterCommunication/CommunicationManager.h"

void BluetoothStatusObserver_Run_OnInit(void);
Comm_Status_t BluetoothStatusObserver_SetBluetoothStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
void BluetoothStatusObserver_Write_IsConnected(bool status);

#endif /* BLUETOOTH_STATUS_OBSERVER_H_ */

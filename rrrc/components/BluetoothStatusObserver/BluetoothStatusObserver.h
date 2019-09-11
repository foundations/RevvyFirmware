#ifndef COMPONENT_BLUETOOTH_STATUS_OBSERVER_H_
#define COMPONENT_BLUETOOTH_STATUS_OBSERVER_H_

#ifndef COMPONENT_TYPES_BLUETOOTH_STATUS_OBSERVER_H_
#define COMPONENT_TYPES_BLUETOOTH_STATUS_OBSERVER_H_

#include "../MasterCommunication/CommunicationManager.h"

typedef enum {
    BluetoothStatus_Inactive,
    BluetoothStatus_NotConnected,
    BluetoothStatus_Connected
} BluetoothStatus_t;

#endif /* COMPONENT_TYPES_BLUETOOTH_STATUS_OBSERVER_H_ */

void BluetoothStatusObserver_Run_OnInit(void);
Comm_Status_t BluetoothStatusObserver_SetBluetoothStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
void BluetoothStatusObserver_Write_ConnectionStatus(BluetoothStatus_t value);

#endif /* COMPONENT_BLUETOOTH_STATUS_OBSERVER_H_ */

#ifndef COMPONENT_BLUETOOTH_INDICATOR_H_
#define COMPONENT_BLUETOOTH_INDICATOR_H_

#ifndef COMPONENT_TYPES_BLUETOOTH_INDICATOR_H_
#define COMPONENT_TYPES_BLUETOOTH_INDICATOR_H_

#include "utils/color.h"


typedef enum {
    BluetoothStatus_Inactive,
    BluetoothStatus_NotConnected,
    BluetoothStatus_Connected
} BluetoothStatus_t;

#endif /* COMPONENT_TYPES_BLUETOOTH_INDICATOR_H_ */

void BluetoothIndicator_Run_OnInit(void);
void BluetoothIndicator_Run_Update(void);
void BluetoothIndicator_Write_LedColor(const rgb_t value);
BluetoothStatus_t BluetoothIndicator_Read_ConnectionStatus(void);

#endif /* COMPONENT_BLUETOOTH_INDICATOR_H_ */

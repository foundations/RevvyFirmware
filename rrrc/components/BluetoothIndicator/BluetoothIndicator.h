#ifndef COMPONENT_BLUETOOTH_INDICATOR_H_
#define COMPONENT_BLUETOOTH_INDICATOR_H_

#include "utils/color.h"

#ifndef COMPONENT_TYPES_BLUETOOTH_INDICATOR_H_
#define COMPONENT_TYPES_BLUETOOTH_INDICATOR_H_

typedef enum {
    BluetoothStatus_Inactive,
    BluetoothStatus_NotConnected,
    BluetoothStatus_Connected
} BluetoothStatus_t;

#endif /* COMPONENT_TYPES_BLUETOOTH_INDICATOR_H_ */

void BluetoothIndicator_Run_OnInit(void);
void BluetoothIndicator_Run_Update(void);
BluetoothStatus_t BluetoothIndicator_Read_ConnectionStatus(void);
void BluetoothIndicator_Write_LedColor(rgb_t color);

#endif /* COMPONENT_BLUETOOTH_INDICATOR_H_ */

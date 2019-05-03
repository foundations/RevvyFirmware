/*
 * BluetoothIndicator.h
 *
 * Created: 03/05/2019 13:35:46
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_BLUETOOTH_INDICATOR_H_
#define COMPONENT_BLUETOOTH_INDICATOR_H_

#include "utils/color.h"
#include <stdbool.h>

void BluetoothIndicator_Run_OnInit(void);
void BluetoothIndicator_Run_Update(void);

bool BluetoothIndicator_Read_IsConnected(void);
void BluetoothIndicator_Write_LedColor(rgb_t color);

#endif /* BLUETOOTHINDICATOR_H_ */

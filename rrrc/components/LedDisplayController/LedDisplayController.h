#ifndef COMPONENT_LED_DISPLAY_CONTROLLER_H_
#define COMPONENT_LED_DISPLAY_CONTROLLER_H_

#ifndef COMPONENT_TYPES_LED_DISPLAY_CONTROLLER_H_
#define COMPONENT_TYPES_LED_DISPLAY_CONTROLLER_H_

#include "utils/color.h"
#include <stdbool.h>
#include <stdint.h>


typedef enum {
    ChargerState_NotPluggedIn,
    ChargerState_Charging,
    ChargerState_Charged,
    ChargerState_Fault
} ChargerState_t;

typedef enum {
    MasterStatus_Unknown,
    MasterStatus_Operational,
    MasterStatus_Controlled
} MasterStatus_t;

typedef enum {
    BluetoothStatus_Inactive,
    BluetoothStatus_NotConnected,
    BluetoothStatus_Connected
} BluetoothStatus_t;

#endif /* COMPONENT_TYPES_LED_DISPLAY_CONTROLLER_H_ */

void LedDisplayController_Run_OnInit(void);
void LedDisplayController_Run_Update(void);
void LedDisplayController_Write_Leds(uint32_t index, const rgb_t value);
void LedDisplayController_Write_MaxBrightness(const uint8_t value);
BluetoothStatus_t LedDisplayController_Read_BluetoothStatus(void);
uint8_t LedDisplayController_Read_MainBatteryLevel(void);
bool LedDisplayController_Read_MainBatteryLow(void);
ChargerState_t LedDisplayController_Read_MainBatteryStatus(void);
MasterStatus_t LedDisplayController_Read_MasterStatus(void);
uint8_t LedDisplayController_Read_MotorBatteryLevel(void);
bool LedDisplayController_Read_MotorBatteryPresent(void);
int8_t LedDisplayController_Read_MotorDriveValues(uint32_t index);
rgb_t LedDisplayController_Read_RingLeds(uint32_t index);

#endif /* COMPONENT_LED_DISPLAY_CONTROLLER_H_ */

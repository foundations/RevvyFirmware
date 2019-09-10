#ifndef COMPONENT_BATTERY_STATUS_PROVIDER_H_
#define COMPONENT_BATTERY_STATUS_PROVIDER_H_

#include "../MasterCommunication/CommunicationManager.h"

#ifndef COMPONENT_TYPES_BATTERY_STATUS_PROVIDER_H_
#define COMPONENT_TYPES_BATTERY_STATUS_PROVIDER_H_

#include <stdint.h>

typedef enum {
    ChargerState_NotPluggedIn,
    ChargerState_Charging,
    ChargerState_Charged,
    ChargerState_Fault
} ChargerState_t;

#endif /* COMPONENT_TYPES_BATTERY_STATUS_PROVIDER_H_ */

Comm_Status_t BatteryStatusProvider_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
uint8_t BatteryStatusProvider_Read_MainBatteryLevel(void);
uint8_t BatteryStatusProvider_Read_MotorBatteryLevel(void);
ChargerState_t BatteryStatusProvider_Read_IsMainBatteryCharging(void);

#endif /* COMPONENT_BATTERY_STATUS_PROVIDER_H_ */

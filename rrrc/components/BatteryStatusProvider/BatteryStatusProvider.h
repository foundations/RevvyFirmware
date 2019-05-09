/*
 * BatteryStatusProvider.h
 *
 * Created: 09/05/2019 13:34:12
 *  Author: Dániel Buga
 */ 


#ifndef BATTERY_STATUS_PROVIDER_H_
#define BATTERY_STATUS_PROVIDER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../MasterCommunication/CommunicationManager.h"

typedef enum {
    BatteryStatusProvider_ChargerStatus_NotConnected,
    BatteryStatusProvider_ChargerStatus_NotCharging,
    BatteryStatusProvider_ChargerStatus_Charging,
    BatteryStatusProvider_ChargerStatus_Error
} BatteryStatusProvider_ChargerStatus_t;

Comm_Status_t BatteryStatusProvider_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
BatteryStatusProvider_ChargerStatus_t BatteryStatusProvider_Read_IsMainBatteryCharging(void);
uint8_t BatteryStatusProvider_Read_MainBatteryLevel(void);
uint8_t BatteryStatusProvider_Read_MotorBatteryLevel(void);

#endif /* BATTERY_STATUS_PROVIDER_H_ */
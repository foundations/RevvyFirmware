/*
 * BatteryCharger.h
 *
 * Created: 01/05/2019 08:47:07
 *  Author: Dániel Buga
 */ 

#ifndef COMPONENT_BATTERY_CHARGER_H_
#define COMPONENT_BATTERY_CHARGER_H_

typedef enum 
{
    ChargerState_NotPluggedIn,
    ChargerState_Charging,
    ChargerState_Charged,
    ChargerState_Fault
} ChargerState_t;

void BatteryCharger_Run_OnInit( void );
void BatteryCharger_Run_EnableFastCharge( void );
void BatteryCharger_Run_DisableFastCharge( void );
ChargerState_t BatteryCharger_Run_GetChargerState( void );
void BatteryCharger_Run_Update( void );

#endif /* COMPONENT_BATTERY_CHARGER_H_ */

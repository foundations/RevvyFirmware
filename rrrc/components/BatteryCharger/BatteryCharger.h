#ifndef COMPONENT_BATTERY_CHARGER_H_
#define COMPONENT_BATTERY_CHARGER_H_

#ifndef COMPONENT_TYPES_BATTERY_CHARGER_H_
#define COMPONENT_TYPES_BATTERY_CHARGER_H_



typedef enum {
    ChargerState_NotPluggedIn,
    ChargerState_Charging,
    ChargerState_Charged,
    ChargerState_Fault
} ChargerState_t;

#endif /* COMPONENT_TYPES_BATTERY_CHARGER_H_ */

void BatteryCharger_Run_OnInit(void);
void BatteryCharger_Run_Update(void);
void BatteryCharger_Write_ChargerState(const ChargerState_t value);

#endif /* COMPONENT_BATTERY_CHARGER_H_ */

/*
 * BatteryIndicator.h
 *
 * Created: 03/05/2019 14:32:04
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_BATTERY_INDICATOR_H_
#define COMPONENT_BATTERY_INDICATOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "utils/color.h"

typedef enum 
{
    BatteryStatus_NotPresent,
    BatteryStatus_Present,
    BatteryStatus_Charging,
    BatteryStatus_Charging_Fault,
} BatteryStatus_t;

typedef struct
{
    uint8_t blinkTimer;
} BatteryIndicator_Context_t;

void BatteryIndicator_Run_OnInit(BatteryIndicator_Context_t* context);
void BatteryIndicator_Run_Update(BatteryIndicator_Context_t* context);
uint8_t BatteryIndicator_Read_Percentage(BatteryIndicator_Context_t* context);
BatteryStatus_t BatteryIndicator_Read_Status(BatteryIndicator_Context_t* context);
void BatteryIndicator_Write_LedColor(BatteryIndicator_Context_t* context, rgb_t color);

#endif /* COMPONENT_BATTERY_INDICATOR_H_ */

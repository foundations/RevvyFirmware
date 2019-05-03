/*
 * BatteryCalculator.h
 *
 * Created: 03/05/2019 14:03:36
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_BATTERY_CALCULATOR_H_
#define COMPONENT_BATTERY_CALCULATOR_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    float detectionVoltage;
    float minVoltage;
    float maxVoltage;
    float filterPreviousValue;
    float biasCorrection;
} BatteryCalculator_Context_t;

void BatteryCalculator_Run_OnInit(BatteryCalculator_Context_t* context);
void BatteryCalculator_Run_Update(BatteryCalculator_Context_t* context);
float BatteryCalculator_Read_Voltage(BatteryCalculator_Context_t* context);
void BatteryCalculator_Write_Percentage(BatteryCalculator_Context_t* context, uint8_t percent);
void BatteryCalculator_Write_BatteryPresent(BatteryCalculator_Context_t* context, bool present);

#endif /* COMPONENT_BATTERY_CALCULATOR_H_ */
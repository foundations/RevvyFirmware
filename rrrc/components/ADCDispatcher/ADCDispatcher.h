#ifndef COMPONENT_ADC_DISPATCHER_H_
#define COMPONENT_ADC_DISPATCHER_H_

#ifndef COMPONENT_TYPES_ADC_DISPATCHER_H_
#define COMPONENT_TYPES_ADC_DISPATCHER_H_

#include <float.h>
#include <stdint.h>

typedef float Voltage_t;
typedef float Current_t;

#endif /* COMPONENT_TYPES_ADC_DISPATCHER_H_ */

void ADCDispatcher_Run_OnInit(void);
void ADCDispatcher_Run_Update(void);
uint16_t ADCDispatcher_Read_ADC0_RawChannelData(uint32_t index);
Voltage_t ADCDispatcher_Read_ADC0_ChannelVoltage(uint32_t index);
uint16_t ADCDispatcher_Read_ADC1_RawChannelData(uint32_t index);
Voltage_t ADCDispatcher_Read_ADC1_ChannelVoltage(uint32_t index);
void ADCDispatcher_Write_Sensor_ADC(uint32_t index, uint8_t value);
void ADCDispatcher_Write_MotorCurrent(uint32_t index, Current_t value);
void ADCDispatcher_Write_MainBatteryVoltage(Voltage_t value);
void ADCDispatcher_Write_MotorBatteryVoltage(Voltage_t value);

#endif /* COMPONENT_ADC_DISPATCHER_H_ */

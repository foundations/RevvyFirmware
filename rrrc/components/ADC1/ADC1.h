#ifndef COMPONENT_ADC1_H_
#define COMPONENT_ADC1_H_

#ifndef COMPONENT_TYPES_ADC1_H_
#define COMPONENT_TYPES_ADC1_H_

#include <float.h>
#include <stdint.h>

typedef float Voltage_t;

#endif /* COMPONENT_TYPES_ADC1_H_ */

void ADC1_Run_OnInit(void);
void ADC1_Run_Update(void);
void ADC1_Write_RawChannelData(uint32_t index, uint16_t value);
void ADC1_Write_ChannelVoltage(uint32_t index, Voltage_t value);

#endif /* COMPONENT_ADC1_H_ */

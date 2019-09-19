#ifndef COMPONENT_ADC0_H_
#define COMPONENT_ADC0_H_

#ifndef COMPONENT_TYPES_ADC0_H_
#define COMPONENT_TYPES_ADC0_H_

#include <float.h>
#include <stdint.h>

typedef float Voltage_t;

#endif /* COMPONENT_TYPES_ADC0_H_ */

void ADC0_Run_OnInit(void);
void ADC0_Run_Update(void);
void ADC0_Write_ChannelVoltage(uint32_t index, const Voltage_t value);
void ADC0_Write_RawChannelData(uint32_t index, const uint16_t value);

#endif /* COMPONENT_ADC0_H_ */

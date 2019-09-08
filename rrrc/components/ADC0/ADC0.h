#ifndef COMPONENT_ADC0_H_
#define COMPONENT_ADC0_H_

#include <stdint.h>

void ADC0_Run_OnInit(void);
void ADC0_Run_Update(void);

void ADC0_Write_ChannelData_Raw(uint32_t channel_idx, uint16_t adc_data);
void ADC0_Write_ChannelVoltage(uint32_t channel_idx, float voltage);

#endif /* COMPONENT_ADC0_H_ */

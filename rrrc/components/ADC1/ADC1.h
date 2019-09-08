#ifndef COMPONENT_ADC1_H_
#define COMPONENT_ADC1_H_

#include <stdint.h>

void ADC1_Run_OnInit(void);
void ADC1_Run_Update(void);

void ADC1_Write_ChannelData_Raw(uint32_t channel_idx, uint16_t adc_data);
void ADC1_Write_ChannelVoltage(uint32_t channel_idx, float voltage);

#endif /* COMPONENT_ADC1_H_ */

/*
 * adc.h
 *
 * Created: 01/05/2019 10:03:35
 *  Author: Dániel Buga
 */ 

#ifndef COMPONENT_ADC_H_
#define COMPONENT_ADC_H_

#include <stdint.h>

void ADC_Run_OnInit(void);
void ADC_Run_Update(void);

void ADC_Write_ChannelData_Raw(uint32_t adc_idx, uint32_t channel_idx, uint16_t adc_data);
void ADC_Write_ChannelVoltage(uint32_t adc_idx, uint32_t channel_idx, float voltage);

#endif /* ADC_H_ */
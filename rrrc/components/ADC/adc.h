/*
 * adc.h
 *
 * Created: 01/05/2019 10:03:35
 *  Author: Dániel Buga
 */ 

#ifndef COMPONENT_ADC_H_
#define COMPONENT_ADC_H_

#include <stdint.h>

#define ADC_BUFFER_ADC1_04  2
#define ADC_BUFFER_ADC1_10  0
#define ADC_BUFFER_ADC1_11  1
#define ADC_BUFFER_ADC1_28  3
#define ADC_BUFFER_ADC1_29  4

void ADC_Run_OnInit(void);
void ADC_Run_Update(void);

void ADC_Write_RawSamples_ADC0(uint16_t smaples[4]);
void ADC_Write_RawSamples_ADC1(uint16_t smaples[5]);

void ADC_Write_ChannelData(uint32_t adc_idx, uint32_t channel_idx, uint16_t adc_data);

void ADC_Write_Samples_ADC0(float smaples[4]);
void ADC_Write_Samples_ADC1(float smaples[5]);

#endif /* ADC_H_ */
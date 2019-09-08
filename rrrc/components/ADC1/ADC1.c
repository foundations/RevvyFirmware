#include "ADC1.h"

// TODO
#include "atmel_start_pins.h"
#include "utils.h"
#include <hal_adc_async.h>
#include <peripheral_clk_config.h>

#include <stdbool.h>

#define ADC_MAX 4095
static inline float adc_to_mv(float x)
{
    return ((3300.0f / ADC_MAX) * x);
}

static const adc_pos_input_t adc_channels[] = 
{
    S1_ADC_CH,
    S2_ADC_CH,
    S3_ADC_CH,
    M0_ISEN_CH,
    M2_ISEN_CH,
    M5_ISEN_CH,
    ADC_CH_BAT_VOLTAGE,
    ADC_CH_MOT_VOLTAGE
};

typedef struct 
{
    struct adc_async_descriptor hwDescriptor;
    uint32_t currentChannel;
    bool conversionRunning;
} adc_context_t;

static adc_context_t adc = 
{
    .currentChannel = 0u,
    .conversionRunning = false
};

//*********************************************************************************************
static void ADC_1_init(void)
{
    hri_mclk_set_APBDMASK_ADC1_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, ADC1_GCLK_ID, CONF_GCLK_ADC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    adc_async_init(&adc.hwDescriptor, ADC1);
}

static int32_t adc_convert_channel(uint32_t channel_idx)
{
    int32_t result = ERR_INVALID_ARG;

    if (channel_idx < ARRAY_SIZE(adc_channels))
    {
        adc.conversionRunning = true;
        adc.currentChannel = channel_idx;
    
        adc_async_set_inputs(&adc.hwDescriptor, adc_channels[channel_idx], ADC_CHN_INT_GND, 0);
        adc_async_start_conversion(&adc.hwDescriptor);
    
        result = ERR_NONE;
    }

    return result;
}

static void conversion_complete(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
    (void) descr;
    (void) channel;

    uint32_t channel_idx = adc.currentChannel;

    /* we can assume that adc_idx and channel_idx are valid */
    ADC1_Write_ChannelData_Raw(adc_channels[channel_idx], adc_data);
    ADC1_Write_ChannelVoltage(adc_channels[channel_idx], adc_to_mv(adc_data));

    if (channel_idx < ARRAY_SIZE(adc_channels) - 1u)
    {
        adc_convert_channel(channel_idx + 1u);
    }
    else
    {
        adc.conversionRunning = false;
    }
}

void ADC1_Run_OnInit(void)
{
    adc.conversionRunning = false;

    ADC_1_init();
    adc_async_register_callback(&adc.hwDescriptor, 0, ADC_ASYNC_CONVERT_CB, &conversion_complete);
    adc_async_enable_channel(&adc.hwDescriptor, 0);
}

void ADC1_Run_Update(void)
{
    if (!adc.conversionRunning)
    {
        /* start new conversion */
        adc_convert_channel(0u);
    }
}

__attribute__((weak))
void ADC1_Write_ChannelData_Raw(uint32_t channel_idx, uint16_t adc_data)
{
    (void) channel_idx;
    (void) adc_data;
}

__attribute__((weak))
void ADC1_Write_ChannelVoltage(uint32_t channel_idx, float voltage)
{
    (void) channel_idx;
    (void) voltage;
}

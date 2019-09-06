/*
 * ADC.c
 *
 * Created: 01/05/2019 10:03:42
 *  Author: Dániel Buga
 */ 

#include "ADC.h"

// TODO
#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <hpl_adc_base.h>

#include <stdbool.h>
#include <string.h>

#define ADC_MAX 4095
#define adc_to_mv(x)     ((3300.0f / ADC_MAX) * x)

typedef struct 
{
    const adc_pos_input_t input;
} adc_channel_config_t;

/*
 ADC channel assingments:
  - ADC0:
   - 10 M4
   - 11 M3
   - 14 S0
   - 15 M1

  - ADC1:
   - 4  M5
   - 5  S2
   - 6  M0
   - 7  S3
   - 10 M2
   - 11 S1
   - 12 V_MOT
   - 13 V_BAT
*/

static adc_channel_config_t adc0_channels[] = 
{
    { .input = S0_ADC_CH },
    { .input = M1_ISEN_CH },
    { .input = M3_ISEN_CH },
    { .input = M4_ISEN_CH }
};

static adc_channel_config_t adc1_channels[] = 
{
    { .input = S1_ADC_CH },
    { .input = S2_ADC_CH },
    { .input = S3_ADC_CH },
    { .input = M0_ISEN_CH },
    { .input = M2_ISEN_CH },
    { .input = M5_ISEN_CH },
    { .input = ADC_CH_BAT_VOLTAGE },
    { .input = ADC_CH_MOT_VOLTAGE },
};

typedef struct 
{
    uint8_t idx;
    struct adc_async_descriptor hwDescriptor;
    adc_channel_config_t * const channels;
    const size_t channelCount;
    uint32_t currentChannel;
    bool conversionRunning;
} adc_context_t;

static adc_context_t adc[] = 
{
    {
        .idx = 0u,
        .channels = &adc0_channels[0],
        .channelCount = ARRAY_SIZE(adc0_channels),
        .currentChannel = 0u,
        .conversionRunning = false
    },

    {
        .idx = 1u,
        .channels = &adc1_channels[0],
        .channelCount = ARRAY_SIZE(adc1_channels),
        .currentChannel = 0u,
        .conversionRunning = false
    }
};

//*********************************************************************************************
static void ADC_0_init(void)
{
    hri_mclk_set_APBDMASK_ADC0_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, ADC0_GCLK_ID, CONF_GCLK_ADC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    adc_async_init(&adc[0].hwDescriptor, ADC0);
}

//*********************************************************************************************
static void ADC_1_init(void)
{
    hri_mclk_set_APBDMASK_ADC1_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, ADC1_GCLK_ID, CONF_GCLK_ADC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    adc_async_init(&adc[1].hwDescriptor, ADC1);
}

static int32_t adc_convert_channel(adc_context_t* ctx, uint32_t channel_idx)
{
    int32_t result = ERR_INVALID_ARG;

    if (channel_idx < ctx->channelCount)
    {
        ctx->conversionRunning = true;
        ctx->currentChannel = channel_idx;
    
        adc_async_set_inputs(&ctx->hwDescriptor, ctx->channels[channel_idx].input, ADC_CHN_INT_GND, 0);
        adc_async_start_conversion(&ctx->hwDescriptor);
    
        result = ERR_NONE;
    }

    return result;
}

static void conversion_complete(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
    (void) channel;

    adc_context_t* ctx = CONTAINER_OF(descr, adc_context_t, hwDescriptor);
    uint32_t channel_idx = ctx->currentChannel;
    uint32_t input = ctx->channels[channel_idx].input;
    uint8_t adc_idx = ctx->idx;

    /* we can assume that adc_idx and channel_idx are valid */
    ADC_Write_ChannelData_Raw(adc_idx, input, adc_data);
    ADC_Write_ChannelVoltage(adc_idx, input, adc_to_mv(adc_data));

    if (channel_idx < ctx->channelCount - 1u)
    {
        adc_convert_channel(ctx, channel_idx + 1u);
    }
    else
    {
        ctx->conversionRunning = false;
    }
}

void ADC_Run_OnInit(void)
{
    adc[0].conversionRunning = false;
    adc[1].conversionRunning = false;

    ADC_0_init();
    adc_async_register_callback(&adc[0].hwDescriptor, 0, ADC_ASYNC_CONVERT_CB, conversion_complete);
    adc_async_enable_channel(&adc[0].hwDescriptor, 0);

    ADC_1_init();
    adc_async_register_callback(&adc[1].hwDescriptor, 0, ADC_ASYNC_CONVERT_CB, conversion_complete);
    adc_async_enable_channel(&adc[1].hwDescriptor, 0);
}

void ADC_Run_Update(void)
{
    if (!adc[0].conversionRunning)
    {
        /* start new conversion */
        adc_convert_channel(&adc[0], 0u);
    }
    
    if (!adc[1].conversionRunning)
    {
        /* start new conversion */
        adc_convert_channel(&adc[1], 0u);
    }
}

__attribute__((weak))
void ADC_Write_ChannelData_Raw(uint32_t adc_idx, uint32_t channel_idx, uint16_t adc_data)
{
    (void) adc_idx;
    (void) channel_idx;
    (void) adc_data;
}

__attribute__((weak))
void ADC_Write_ChannelVoltage(uint32_t adc_idx, uint32_t channel_idx, float voltage)
{
    (void) adc_idx;
    (void) channel_idx;
    (void) voltage;
}

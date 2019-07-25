/*
 * adc.c
 *
 * Created: 01/05/2019 10:03:42
 *  Author: Dániel Buga
 */ 

#include "adc.h"

// TODO
#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <hpl_adc_base.h>

#include <stdbool.h>
#include <string.h>

#define ADC_MAX 4095
#define adc_to_mv(x)     ((3300.0f / ADC_MAX) * x)

/* The channel amount for ADC */
#define ADC_0_CH_AMOUNT 1
#define ADC_1_CH_AMOUNT 1

/* The maximal channel number of enabled channels */
#define ADC_0_CH_MAX 15
#define ADC_1_CH_MAX 0x1D

static struct adc_async_descriptor         ADC_0;
static struct adc_async_descriptor         ADC_1;
static struct adc_async_channel_descriptor ADC_0_ch[ADC_0_CH_AMOUNT];
static struct adc_async_channel_descriptor ADC_1_ch[ADC_1_CH_AMOUNT];

//static uint8_t ADC_0_buffer[ADC_0_BUFFER_SIZE];
//static uint8_t ADC_1_buffer[ADC_1_BUFFER_SIZE];
static uint8_t ADC_0_map[ADC_0_CH_MAX + 1];
static uint8_t ADC_1_map[ADC_1_CH_MAX + 1];

//*********************************************************************************************
static void ADC_0_init(void)
{
    hri_mclk_set_APBDMASK_ADC0_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, ADC0_GCLK_ID, CONF_GCLK_ADC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    adc_async_init(&ADC_0, ADC0, ADC_0_map, ADC_0_CH_MAX, ADC_0_CH_AMOUNT, &ADC_0_ch[0]);
//    adc_async_register_channel_buffer(&ADC_0, 0, ADC_0_buffer, ADC_0_BUFFER_SIZE);
}

//*********************************************************************************************
static void ADC_1_init(void)
{
    hri_mclk_set_APBDMASK_ADC1_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, ADC1_GCLK_ID, CONF_GCLK_ADC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    adc_async_init(&ADC_1, ADC1, ADC_1_map, ADC_1_CH_MAX, ADC_1_CH_AMOUNT, &ADC_1_ch[0]);
//    adc_async_register_channel_buffer(&ADC_1, 0, ADC_1_buffer, ADC_1_BUFFER_SIZE);
}

typedef struct 
{
    const adc_pos_input_t input;
} adc_channel_config_t;

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
    struct adc_async_descriptor* const hwDescriptor;
    adc_channel_config_t * const channels;
    const size_t channelCount;
    uint32_t currentChannel;
    bool conversionRunning;
} adc_context_t;

static adc_context_t adc[] = 
{
    {
        .hwDescriptor = &ADC_0,
        .channels = &adc0_channels[0],
        .channelCount = ARRAY_SIZE(adc0_channels),
        .currentChannel = 0u,
        .conversionRunning = false
    },

    {
        .hwDescriptor = &ADC_1,
        .channels = &adc1_channels[0],
        .channelCount = ARRAY_SIZE(adc1_channels),
        .currentChannel = 0u,
        .conversionRunning = false
    }
};

static int32_t adc_convert_channel(uint32_t adc_idx, uint32_t channel_idx)
{
    int32_t result = ERR_INVALID_ARG;
    
    if (adc_idx < ARRAY_SIZE(adc))
    {
        adc_context_t* ctx = &adc[adc_idx];

        if (channel_idx < ctx->channelCount)
        {
            ctx->conversionRunning = true;
            ctx->currentChannel = channel_idx;
    
            adc_async_set_inputs(ctx->hwDescriptor, ctx->channels[channel_idx].input, ADC_CHN_INT_GND, 0);
            adc_async_start_conversion(ctx->hwDescriptor);
    
            result = ERR_NONE;
        }
    }

    return result;
}

static void conversion_complete(uint32_t adc_idx, uint32_t channel_idx, uint16_t adc_data)
{
    /* we can assume that adc_idx and channel_idx are valid */
    ADC_Write_ChannelData_Raw(adc_idx, adc[adc_idx].channels[channel_idx].input, adc_data);
    ADC_Write_ChannelVoltage(adc_idx, adc[adc_idx].channels[channel_idx].input, adc_to_mv(adc_data));

    if (channel_idx < adc[adc_idx].channelCount - 1u)
    {
        adc_convert_channel(adc_idx, channel_idx + 1u);
    }
    else
    {
        adc[adc_idx].conversionRunning = false;
    }
}

static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
    (void) descr;
    (void) channel;

    conversion_complete(0, adc[0].currentChannel, adc_data);
}

static void convert_cb_ADC_1(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
    (void) descr;
    (void) channel;

    conversion_complete(1, adc[1].currentChannel, adc_data);
}

int32_t adc_convertion_start(uint32_t adc_idx)
{
    return adc_convert_channel(adc_idx, 0u);
}

void ADC_Run_OnInit(void)
{
    adc[0].conversionRunning = false;
    adc[1].conversionRunning = false;

    ADC_0_init();
    adc_async_register_callback(&ADC_0, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0);
    adc_async_enable_channel(&ADC_0, 0);

    ADC_1_init();
    adc_async_register_callback(&ADC_1, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_1);
    adc_async_enable_channel(&ADC_1, 0);
}

void ADC_Run_Update(void)
{
    if (!adc[0].conversionRunning)
    {
        /* start new conversion */
        adc_convertion_start(0);
    }
    
    if (!adc[1].conversionRunning)
    {
        /* start new conversion */
        adc_convertion_start(1);
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

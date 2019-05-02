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

struct adc_async_descriptor         ADC_0;
struct adc_async_descriptor         ADC_1;
struct adc_async_channel_descriptor ADC_0_ch[ADC_0_CH_AMOUNT];
struct adc_async_channel_descriptor ADC_1_ch[ADC_1_CH_AMOUNT];

//static uint8_t ADC_0_buffer[ADC_0_BUFFER_SIZE];
//static uint8_t ADC_1_buffer[ADC_1_BUFFER_SIZE];
static uint8_t ADC_0_map[ADC_0_CH_MAX + 1];
static uint8_t ADC_1_map[ADC_1_CH_MAX + 1];

//*********************************************************************************************
static void ADC_0_init(void)
{
    hri_mclk_set_APBDMASK_ADC0_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, ADC0_GCLK_ID, CONF_GCLK_ADC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    adc_async_init(&ADC_0, ADC0, ADC_0_map, ADC_0_CH_MAX, ADC_0_CH_AMOUNT, &ADC_0_ch[0], (void *)NULL);
//    adc_async_register_channel_buffer(&ADC_0, 0, ADC_0_buffer, ADC_0_BUFFER_SIZE);
}

//*********************************************************************************************
static void ADC_1_init(void)
{
    hri_mclk_set_APBDMASK_ADC1_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, ADC1_GCLK_ID, CONF_GCLK_ADC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    adc_async_init(&ADC_1, ADC1, ADC_1_map, ADC_1_CH_MAX, ADC_1_CH_AMOUNT, &ADC_1_ch[0], (void *)NULL);
//    adc_async_register_channel_buffer(&ADC_1, 0, ADC_1_buffer, ADC_1_BUFFER_SIZE);
}

typedef struct 
{
    const adc_pos_input_t input;
} adc_channel_config_t;

static adc_channel_config_t adc0_channels[] = 
{
    { .input = ADC_CH_SEN_0 },
    { .input = ADC_CH_SEN_1 },
    { .input = ADC_CH_SEN_2 },
    { .input = ADC_CH_SEN_3 }
};

static adc_channel_config_t adc1_channels[] = 
{
    { .input = ADC_CH_MOT_CURRENT },
    { .input = ADC_CH_BAT_VOLTAGE },
    { .input = ADC_CH_MOT_VOLTAGE },
    { .input = ADC_CH_TEMP_SENSOR_P },
    { .input = ADC_CH_TEMP_SENSOR_C },
};

static uint16_t adc0_buffer[ARRAY_SIZE(adc0_channels)];
static uint16_t adc1_buffer[ARRAY_SIZE(adc1_channels)];

typedef struct 
{
    struct adc_async_descriptor* const hwDescriptor;
    adc_channel_config_t * const channels;
    uint16_t* const samples;
    const size_t channelCount;
    uint32_t currentChannel;
    bool conversionRunning;
} adc_context_t;

static adc_context_t adc[] = 
{
    {
        .hwDescriptor = &ADC_0,
        .channels = &adc0_channels[0],
        .samples = &adc0_buffer[0],
        .channelCount = ARRAY_SIZE(adc0_channels),
        .currentChannel = 0u,
        .conversionRunning = false
    },

    {
        .hwDescriptor = &ADC_1,
        .channels = &adc1_channels[0],
        .samples = &adc1_buffer[0],
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
    adc[adc_idx].samples[channel_idx] = adc_data;

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
    conversion_complete(0, adc[0].currentChannel, adc_data);
}

static void convert_cb_ADC_1(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
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
    memset(adc0_buffer, 0u, sizeof(adc0_buffer));

    ADC_1_init();
    adc_async_register_callback(&ADC_1, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_1);
    adc_async_enable_channel(&ADC_1, 0);
    memset(adc1_buffer, 0u, sizeof(adc1_buffer));
}

void ADC_Run_Update(void)
{
    if (!adc[0].conversionRunning)
    {
        /* update outputs */
        ADC_Write_RawSamples_ADC0(adc0_buffer);

        float samples[ARRAY_SIZE(adc0_buffer)];
        for (uint32_t i = 0u; i < ARRAY_SIZE(adc0_buffer); i++)
        {
            samples[i] = adc_to_mv(adc0_buffer[i]);
        }
        ADC_Write_Samples_ADC0(samples);

        /* start new conversion */
        adc_convertion_start(0);
    }
    
    if (!adc[1].conversionRunning)
    {
        /* update outputs */
        ADC_Write_RawSamples_ADC1(adc1_buffer);

        float samples[ARRAY_SIZE(adc1_buffer)];
        for (uint32_t i = 0u; i < ARRAY_SIZE(adc1_buffer); i++)
        {
            samples[i] = adc_to_mv(adc1_buffer[i]);
        }
        ADC_Write_Samples_ADC1(samples);

        /* start new conversion */
        adc_convertion_start(1);
    }
}

__WEAK void ADC_Write_RawSamples_ADC0(uint16_t smaples[4]) {}
__WEAK void ADC_Write_RawSamples_ADC1(uint16_t smaples[5]) {}

__WEAK void ADC_Write_Samples_ADC0(float smaples[4]) {}
__WEAK void ADC_Write_Samples_ADC1(float smaples[5]) {}

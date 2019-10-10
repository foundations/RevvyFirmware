#include "utils.h"
#include "ADC1.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include "atmel_start_pins.h"
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

        adc_async_set_inputs(&adc.hwDescriptor, adc_channels[channel_idx], ADC_CHN_INT_GND);
        adc_async_start_conversion(&adc.hwDescriptor);

        result = ERR_NONE;
    }

    return result;
}

static void conversion_complete(const struct adc_async_descriptor *const descr, uint16_t adc_data)
{
    (void) descr;

    uint32_t channel_idx = adc.currentChannel;

    /* we can assume that adc_idx and channel_idx are valid */
    ADC1_Write_RawChannelData(channel_idx, adc_data);
    ADC1_Write_ChannelVoltage(channel_idx, adc_to_mv(adc_data));

    if (channel_idx < ARRAY_SIZE(adc_channels) - 1u)
    {
        adc_convert_channel(channel_idx + 1u);
    }
    else
    {
        adc.conversionRunning = false;
    }
}
/* End User Code Section: Declarations */

void ADC1_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    adc.conversionRunning = false;

    ADC_1_init();
    adc_async_register_callback(&adc.hwDescriptor, ADC_ASYNC_CONVERT_CB, &conversion_complete);
    adc_async_enable(&adc.hwDescriptor);
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void ADC1_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    if (!adc.conversionRunning)
    {
        /* start new conversion */
        adc_convert_channel(0u);
    }
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void ADC1_Write_ChannelVoltage(uint32_t index, const Voltage_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 8);
    /* Begin User Code Section: ChannelVoltage Start */

    /* End User Code Section: ChannelVoltage Start */
    /* Begin User Code Section: ChannelVoltage End */

    /* End User Code Section: ChannelVoltage End */
}

__attribute__((weak))
void ADC1_Write_RawChannelData(uint32_t index, const uint16_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 8);
    /* Begin User Code Section: RawChannelData Start */

    /* End User Code Section: RawChannelData Start */
    /* Begin User Code Section: RawChannelData End */

    /* End User Code Section: RawChannelData End */
}

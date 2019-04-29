/*
 * worklogic.c
 *
 * Created: 1/4/2019 6:56:55 PM
 *  Author: User
 */ 

 /*
//=======================================================================================================
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    //DBG_printf(DBG_LEVEL_DEBUG, "HAL_I2C_SlaveTxCpltCallback. %d bytes trasmitted\n", hi2c->XferCount);
    //buff_i2c_to_tx_bytes = 0;
}

//=======================================================================================================
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    //DBG_printf(DBG_LEVEL_DEBUG, "HAL_I2C_SlaveRxCpltCallback, %d bytes received \n", hi2c->XferCount);
    //uint8_t ret = CommandHandler ( (ptransaction) hi2c->pBuffPtr, hi2c->XferCount);
    //buff_i2c_to_tx_bytes = MakeResponse(ret, (ptransaction) &buff_i2c_tx);

}*/

#include "rrrc_hal.h"
#include "rrrc_sensors.h"
#include "rrrc_motors.h"
//#include "rrrc_i2c_protocol.h"



//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************

trans_buffer_t rx_buffer;
trans_buffer_t tx_buffer;
static bool tx_first;
static bool tx_stopped;/* < hacky solution to allow partially reading the tx buffer */
//*****************************************************************************************************
void rrrc_i2c_transmit(size_t size)
{
    tx_buffer.index = 0;
    tx_buffer.size = size;
    tx_first = true;
    tx_stopped = false;
}

void rrrc_i2c_s_async_tx(struct _i2c_s_async_device *const device)
{
    if ( tx_buffer.index < tx_buffer.size )
    {
        if (tx_stopped)
        {
            tx_stopped = false;
            _i2c_s_async_write_byte(device, tx_buffer.buff[tx_buffer.index]);
        }
        else if (tx_first)
        {
            tx_first = false;
            _i2c_s_async_write_byte(device, tx_buffer.buff[tx_buffer.index]);
        }
        else
        {
            _i2c_s_async_write_byte(device, tx_buffer.buff[++tx_buffer.index]);
        }
    }
    else
    {
        _i2c_s_async_write_byte(device, 0xFF);
    }
}

//*****************************************************************************************************
void rrrc_i2c_s_async_byte_received(struct _i2c_s_async_device *const device, const uint8_t data)
{
    struct i2c_s_async_descriptor *descr = CONTAINER_OF(device, struct i2c_s_async_descriptor, device);

    if (rx_buffer.size < MAX_TRANSACTION_SIZE)
    {
        rx_buffer.buff[rx_buffer.size] = data;
        rx_buffer.size++;
    }
}

//*****************************************************************************************************
void rrrc_i2c_s_async_stop(struct _i2c_s_async_device *const device, const uint8_t dir)
{
    if (!dir)
    {
        // master write
        if (rx_buffer.size > 0)
        {
            CommunicationTask_NotifyRxCompleteFromISR();
        }
    }
    else
    {
        // master read
        hri_sercomi2cs_clear_STATUS_RXNACK_bit(device->hw);
        tx_stopped = true;
    }
}

//*****************************************************************************************************
void rrrc_i2c_s_async_addr_match(struct _i2c_s_async_device *const device, const uint8_t dir)
{
    struct i2c_s_async_descriptor *descr = CONTAINER_OF(device, struct i2c_s_async_descriptor, device);

    if (!dir)
    {
        // master write
    }
    else
    { 
        // master read
        hri_sercomi2cs_clear_INTFLAG_DRDY_bit(device->hw);
        tx_stopped = false;
    }
}

//*****************************************************************************************************
void rrrc_i2c_s_async_error(struct _i2c_s_async_device *const device)
{
    struct i2c_s_async_descriptor *descr = CONTAINER_OF(device, struct i2c_s_async_descriptor, device);

    //TODO need restart bus
    return;
}

//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************

//static volatile uint8_t adc_data = 0;
static volatile int adc0_ch = 0;
static volatile int adc1_ch = 0;

typedef struct _adc_channel_callback
{
    uint32_t chan;
    channel_adc_data_cb_t  channel_cb_s;
    void* user_data;
} adc_channel_callback;

#define ADC_INTERNAL_GND_ch 0x18
adc_channel_callback adc0_channel_callback[] = {{0, NULL,NULL},{13, NULL,NULL},{12, NULL,NULL},{1, NULL,NULL}};
adc_channel_callback adc1_channel_callback[] = {{4, NULL,NULL},{10, NULL,NULL},{11, NULL,NULL},{0x1D, NULL,NULL}};

static struct adc_async_descriptor* get_adc_descriptor(uint32_t adc_idx)
{
    struct adc_async_descriptor* adc = NULL;
    switch (adc_idx)
    {
        case 0:
            adc = &ADC_0;
            break;

        case 1:
            adc = &ADC_1;
            break;

        default:
           break;
    }
    return adc;
}

static adc_channel_callback* get_adc_channel(uint32_t adc_idx, uint32_t chan_idx)
{
    adc_channel_callback* callback = NULL;
    switch (adc_idx)
    {
        case 0:
            if (chan_idx < ARRAY_SIZE(adc0_channel_callback))
            {
                callback = &adc0_channel_callback[chan_idx];
            }
            break;

        case 1:
            if (chan_idx < ARRAY_SIZE(adc1_channel_callback))
            {
                callback = &adc1_channel_callback[chan_idx];
            }
            break;

        default:
            break;
    }

    return callback;
}

//*********************************************************************************************
int32_t RRRC_channel_adc_register_cb(uint32_t adc_idx, uint32_t chan_idx, channel_adc_data_cb_t func, void* user_data)
{
    if (func == NULL)
    {
        return ERR_INVALID_ARG;
    }

    adc_channel_callback* callback = get_adc_channel(adc_idx, chan_idx);

    if (callback != NULL)
    {
        callback->channel_cb_s = func;
        callback->user_data = user_data;
        return ERR_NONE;
    }

    return ERR_FAILURE;
}

//*********************************************************************************************
int32_t RRRC_channel_adc_unregister_cb(uint32_t adc_idx, uint32_t chan_idx)
{
    adc_channel_callback* callback = get_adc_channel(adc_idx, chan_idx);

    if (callback != NULL)
    {
        callback->channel_cb_s = NULL;
        callback->user_data = NULL;
        return ERR_NONE;
    }

    return ERR_FAILURE;
}

//*********************************************************************************************
static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
    if (adc0_channel_callback[adc0_ch].channel_cb_s) 
        adc0_channel_callback[adc0_ch].channel_cb_s(adc_data, adc0_channel_callback[adc0_ch].user_data);
        
    if (adc0_ch < ARRAY_SIZE(adc0_channel_callback) - 1u)
    {
        adc0_ch++;
    }
    else
    {
        adc0_ch = 0;
    }

    adc_async_set_inputs(descr, adc0_channel_callback[adc0_ch].chan, ADC_INTERNAL_GND_ch, 0);
}

//*********************************************************************************************
static void convert_cb_ADC_1(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
    if (adc1_channel_callback[adc1_ch].channel_cb_s) 
        adc1_channel_callback[adc1_ch].channel_cb_s(adc_data, adc1_channel_callback[adc1_ch].user_data);

    if (adc1_ch < ARRAY_SIZE(adc1_channel_callback) - 1u)
    {
        adc1_ch++;
    }
    else
    {
        adc1_ch = 0;
    }

    adc_async_set_inputs(descr, adc1_channel_callback[adc1_ch].chan, ADC_INTERNAL_GND_ch, 0);
}

int32_t adc_convertion_start(uint32_t adc_idx)
{
    int32_t result = ERR_NONE;
    switch (adc_idx)
    {
        case 0:
            adc0_ch = 0;
            break;

        case 1:
            adc1_ch = 0;
            break;

        default:
            result = ERR_INVALID_ARG;
            break;
    }
    struct adc_async_descriptor* adc = get_adc_descriptor(adc_idx);
    adc_channel_callback* callback = get_adc_channel(adc_idx, 0);

    if (adc == NULL || callback == NULL)
    {
        result = ERR_INVALID_ARG;
    }
    else
    {
        adc_async_enable_channel(adc, 0);
        adc_async_register_callback(adc, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_1);
        adc_async_set_inputs(adc, callback->chan, ADC_INTERNAL_GND_ch, 0);
        adc_async_start_conversion(adc);
    }
    
    return result;
}

int32_t adc_convertion_stop(uint32_t adc_idx)
{
    int32_t result = ERR_INVALID_ARG;

    struct adc_async_descriptor* adc = get_adc_descriptor(adc_idx);
    if (adc != NULL)
    {
        result = adc_async_stop_conversion(adc);
    }

    return result;
}

static struct timer_descriptor* high_res_timer;
void high_res_timer_init(struct timer_descriptor* timer)
{
    high_res_timer = timer;

    timer_start(timer);
}

uint32_t high_res_timer_get_count(void)
{
    hri_tc_set_CTRLB_CMD_bf(high_res_timer->device.hw, TC_CTRLBSET_CMD_READSYNC_Val);
    while (hri_tc_read_CTRLB_CMD_bf(high_res_timer->device.hw) != 0);

    return hri_tccount16_get_COUNT_COUNT_bf(high_res_timer->device.hw, 0xFFFF);
}

uint32_t high_res_timer_ticks_per_ms(void)
{
    return 24;
}

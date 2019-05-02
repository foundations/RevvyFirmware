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

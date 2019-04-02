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

//*****************************************************************************************************
 static inline void rrrc_i2c_send_stop(void *const hw)
{
#define CMD_STOP 0x03
	hri_sercomi2cm_set_CTRLB_ACKACT_bit(hw);
	hri_sercomi2cm_set_CTRLB_CMD_bf(hw, CMD_STOP);
}

//*****************************************************************************************************
void rrrc_i2c_s_async_tx(struct _i2c_s_async_device *const device)
{
    if ( tx_buffer.index < tx_buffer.size )
    {
        
		if (tx_buffer.index+1 == tx_buffer.size)
			rrrc_i2c_send_stop(device);
		_i2c_s_async_write_byte(device, tx_buffer.buff[tx_buffer.index]);
		tx_buffer.index++;
    }else
    {
		rrrc_i2c_send_stop(device);
		_i2c_s_async_write_byte(device, 0xFF);
		
        tx_buffer.index = 0;
        tx_buffer.size = 0;
    }
}

extern TaskHandle_t    xCommunicationTask;
//*****************************************************************************************************
void rrrc_i2c_s_async_byte_received(struct _i2c_s_async_device *const device, const uint8_t data)
{
	struct i2c_s_async_descriptor *descr = CONTAINER_OF(device, struct i2c_s_async_descriptor, device);

    if (rx_buffer.size < MAX_TRANSACTION_SIZE)
    {
        rx_buffer.buff[rx_buffer.size] = data;
		rx_buffer.size++;
    }else
    {
        rrrc_i2c_send_stop(device);
		BaseType_t xHigherPriorityTaskWoken = pdTRUE;
		xTaskNotifyFromISR(xCommunicationTask, 0x01, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

//         uint8_t cmd = CommandHandler(rx_buffer.buff, rx_buffer.size);
//         tx_buffer.size = MakeResponse(cmd, tx_buffer.buff);
//         rx_buffer.size = 0;
    }
}

//*****************************************************************************************************
void rrrc_i2c_s_async_stop(struct _i2c_s_async_device *const device, const uint8_t dir)
{
    if (dir)
    { //tx
        tx_buffer.index = 0;
        tx_buffer.size = 0;
    }else
    { //rx
//         enum RRRC_I2C_CMD cmd = CommandHandler(rx_buffer.buff, rx_buffer.size);
//         tx_buffer.size = MakeResponse(cmd, tx_buffer.buff);
//         rx_buffer.size = 0;
		BaseType_t xHigherPriorityTaskWoken = pdTRUE;
		xTaskNotifyFromISR(xCommunicationTask, 0x01, eSetBits, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

//*****************************************************************************************************
void rrrc_i2c_s_async_addr_match(struct _i2c_s_async_device *const device, const uint8_t dir)
{
    struct i2c_s_async_descriptor *descr = CONTAINER_OF(device, struct i2c_s_async_descriptor, device);

    if (!dir)
    { //tx
        if (tx_buffer.size == 0)
            rrrc_i2c_send_stop(device);
    }else
    { // rx
        if (rx_buffer.size >= MAX_TRANSACTION_SIZE)
            rrrc_i2c_send_stop(device);
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

struct _adc_channel_callback
{
	uint32_t chan;
	channel_adc_data_cb_t  channel_cb_s;
	void* user_data;
};

#define ADC_INTERNAL_GND_ch 0x18
struct _adc_channel_callback adc0_channel_callback[SENSOR_PORT_AMOUNT] = {{0, NULL,NULL},{13, NULL,NULL},{12, NULL,NULL},{1, NULL,NULL}};
struct _adc_channel_callback adc1_channel_callback[3] = {{4, NULL,NULL},{10, NULL,NULL},{11, NULL,NULL}/*,{0x1D, NULL,NULL}*/};

//*********************************************************************************************
int32_t RRRC_channel_adc_register_cb(uint32_t adc_idx, uint32_t chan_idx, channel_adc_data_cb_t func, void* user_data)
{
	if (adc_idx>=2 || chan_idx>=SENSOR_PORT_AMOUNT || func==NULL)
		return ERR_INVALID_ARG;
	if (adc_idx == 0 && chan_idx<ARRAY_SIZE(adc0_channel_callback))
	{
		adc0_channel_callback[chan_idx].channel_cb_s = func;
		adc0_channel_callback[chan_idx].user_data = user_data;
		return ERR_NONE;
	}else if (adc_idx == 1 && chan_idx<ARRAY_SIZE(adc1_channel_callback))
	{
		adc1_channel_callback[chan_idx].channel_cb_s = func;
		adc1_channel_callback[chan_idx].user_data = user_data;
		return ERR_NONE;
	}
	return ERR_FAILURE;
}

//*********************************************************************************************
int32_t RRRC_channel_adc_unregister_cb(uint32_t adc_idx, uint32_t chan_idx)
{
	if (adc_idx>=2 || chan_idx>=SENSOR_PORT_AMOUNT)
		return ERR_INVALID_ARG;
	if (adc_idx == 0 && chan_idx<ARRAY_SIZE(adc0_channel_callback))
	{
		adc0_channel_callback[chan_idx].channel_cb_s = NULL;
		adc0_channel_callback[chan_idx].user_data = NULL;
		return ERR_NONE;
	}else if (adc_idx == 1 && chan_idx<ARRAY_SIZE(adc1_channel_callback))
	{
		adc1_channel_callback[chan_idx].channel_cb_s = NULL;
		adc1_channel_callback[chan_idx].user_data = NULL;
		return ERR_NONE;
	}
	return ERR_FAILURE;
}

//*********************************************************************************************
static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
	if (adc0_channel_callback[adc0_ch].channel_cb_s) 
		adc0_channel_callback[adc0_ch].channel_cb_s(adc_data, adc0_channel_callback[adc0_ch].user_data);

	adc0_ch++;
	if (adc0_ch>=ARRAY_SIZE(adc0_channel_callback))
		adc0_ch = 0;

	adc_async_set_inputs(descr, adc0_channel_callback[adc0_ch].chan, ADC_INTERNAL_GND_ch, 0); 
	
	return;
}

//*********************************************************************************************
static void convert_cb_ADC_1(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
	if (adc1_channel_callback[adc1_ch].channel_cb_s) 
		adc1_channel_callback[adc1_ch].channel_cb_s(adc_data, adc1_channel_callback[adc1_ch].user_data);

	adc1_ch++;
	if (adc1_ch>=ARRAY_SIZE(adc1_channel_callback))
		adc1_ch = 0;

	adc_async_set_inputs(descr, adc1_channel_callback[adc1_ch].chan, ADC_INTERNAL_GND_ch, 0); 
	return;
}

//*********************************************************************************************
int32_t RRRC_add_task(struct timer_task *const task, timer_task_cb_t func, uint32_t interval, void* user_data, bool oneshot)
{
	int32_t result = ERR_NONE;
	if (!task)
		return ERR_INVALID_ARG;
	task->interval = interval;
	task->cb       = func;
	if (oneshot)
		task->mode     = TIMER_TASK_ONE_SHOT;
	else
		task->mode     = TIMER_TASK_REPEAT;
	task->user_data = user_data;
	timer_stop(&TIMER_RTC);
	timer_add_task(&TIMER_RTC, task);
	timer_start(&TIMER_RTC);
	return result;
}

int32_t adc_convertion_start(uint32_t adc_idx)
{
	int32_t result = ERR_NONE;
	if (0 == adc_idx)
    {
	    adc_async_enable_channel(&ADC_0, 0);
	    adc_async_register_callback(&ADC_0, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0);
	    adc_async_set_inputs(&ADC_0, adc0_ch, 0, 0);
	    adc_async_start_conversion(&ADC_0);
    }else if (1 == adc_idx)
    {
	    adc_async_enable_channel(&ADC_1, 0);
	    adc_async_register_callback(&ADC_1, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_1);
	    adc_async_set_inputs(&ADC_1, adc1_ch, 0, 0);
	    adc_async_start_conversion(&ADC_1);
    }else
		result = ERR_INVALID_ARG;

	return result;
}

int32_t adc_convertion_stop(uint32_t adc_idx)
{
	int32_t result = ERR_NONE;
	if (0 == adc_idx)
		result = adc_async_stop_conversion(&ADC_0);
	if (1 == adc_idx)
		result = adc_async_stop_conversion(&ADC_1);
	else
		result = ERR_INVALID_ARG;
	return result;
}

//*********************************************************************************************
int32_t RRRC_remove_task(struct timer_task const* task)
{
	int32_t result = ERR_NONE;
	if (!task)
	return ERR_INVALID_ARG;
	timer_stop(&TIMER_RTC);
	timer_remove_task(&TIMER_RTC, task);
	timer_start(&TIMER_RTC);
	return result;
}


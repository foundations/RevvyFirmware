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

#include "driver_init.h"
#include "pwm_lite.h"

#include "rrrc_hal.h"
#include "rrrc_sensors.h"
#include "rrrc_motors.h"
#include "rrrc_i2c_protocol.h"

//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************

typedef struct _trans_buffer_t
{
    uint8_t buff[MAX_TRANSACTION_SIZE];
    uint32_t size;
    uint32_t index;
}trans_buffer_t, *p_trans_buffer_t;

trans_buffer_t rx_buffer;
trans_buffer_t tx_buffer;

//*****************************************************************************************************
 static inline void i2c_send_stop(void *const hw)
{
#define CMD_STOP 0x03
	hri_sercomi2cm_set_CTRLB_ACKACT_bit(hw);
	hri_sercomi2cm_set_CTRLB_CMD_bf(hw, CMD_STOP);
}

//*****************************************************************************************************
void i2c_s_async_tx(struct _i2c_s_async_device *const device)
{
    if ( tx_buffer.index < tx_buffer.size )
    {
        
		if (tx_buffer.index+1 == tx_buffer.size)
			i2c_send_stop(device);
		_i2c_s_async_write_byte(device, tx_buffer.buff[tx_buffer.index]);
		tx_buffer.index++;
    }else
    {
		i2c_send_stop(device);
		_i2c_s_async_write_byte(device, 0xFF);
		
        tx_buffer.index = 0;
        tx_buffer.size = 0;
    }
}

//*****************************************************************************************************
void i2c_s_async_byte_received(struct _i2c_s_async_device *const device, const uint8_t data)
{
	struct i2c_s_async_descriptor *descr = CONTAINER_OF(device, struct i2c_s_async_descriptor, device);

    if (rx_buffer.size < MAX_TRANSACTION_SIZE)
    {
        rx_buffer.buff[rx_buffer.size] = data;
		rx_buffer.size++;
    }else
    {
        i2c_send_stop(device);
        uint8_t cmd = CommandHandler(rx_buffer.buff, rx_buffer.size);
        tx_buffer.size = MakeResponse(cmd, tx_buffer.buff);
        rx_buffer.size = 0;
    }
}

//*****************************************************************************************************
void i2c_s_async_stop(struct _i2c_s_async_device *const device, const uint8_t dir)
{
    if (dir)
    { //tx
        tx_buffer.index = 0;
        tx_buffer.size = 0;
    }else
    { //rx
        uint8_t cmd = CommandHandler(rx_buffer.buff, rx_buffer.size);
        tx_buffer.size = MakeResponse(cmd, tx_buffer.buff);
        rx_buffer.size = 0;
    }
}

//*****************************************************************************************************
void i2c_s_async_addr_match(struct _i2c_s_async_device *const device, const uint8_t dir)
{
    struct i2c_s_async_descriptor *descr = CONTAINER_OF(device, struct i2c_s_async_descriptor, device);

    if (!dir)
    { //tx
        if (tx_buffer.size == 0)
            i2c_send_stop(device);
    }else
    { // rx
        if (rx_buffer.size >= MAX_TRANSACTION_SIZE)
            i2c_send_stop(device);
    }
}

//*****************************************************************************************************
void i2c_s_async_error(struct _i2c_s_async_device *const device)
{
	struct i2c_s_async_descriptor *descr = CONTAINER_OF(device, struct i2c_s_async_descriptor, device);

    //TODO need restart bus
}

//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************

static volatile int val = 0;

/**
 * Example of using TIMER_0.
 */
static void TIMER_0_task1_cb(const struct timer_task *const timer_task)
{
    val +=1;
	return;
}

static void TIMER_0_task2_cb(const struct timer_task *const timer_task)
{
	val +=1;
	return;
}



static void button_on_PB31_pressed(void)
{
	//printf("hello");

	val +=1;
	return;
}


static volatile uint8_t adc_data = 0;
static volatile int adc0_ch = 0;
static volatile int adc1_ch = 10;




struct _adc_channel_callback
{
	channel_adc_data_cb_t  channel_cb_s;
	void* user_data;
}adc_channel_callback[SENSOR_PORT_AMOUNT] = {{NULL,NULL},{NULL,NULL},{NULL,NULL},{NULL,NULL}};
//channel_adc_data_cb_t channel_cb_s[4]  = {NULL,NULL,NULL,NULL};


void RRRC_channel_adc_register_cb(uint32_t chan_idx, channel_adc_data_cb_t func, void* user_data)
{
	if (chan_idx>=SENSOR_PORT_AMOUNT || func==NULL)
		return;
	adc_channel_callback[chan_idx].channel_cb_s = func;
	adc_channel_callback[chan_idx].user_data = user_data;
}

void RRRC_channel_adc_unregister_cb(uint32_t chan_idx)
{
	if (chan_idx>=SENSOR_PORT_AMOUNT)
		return;
	adc_channel_callback[chan_idx].channel_cb_s = NULL;
	adc_channel_callback[chan_idx].user_data = NULL;
}

static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel)
{
	adc_async_read_channel(descr, channel, &adc_data, sizeof(adc_data));
	if (adc0_ch==0)
		if (adc_channel_callback[0].channel_cb_s) adc_channel_callback[0].channel_cb_s(adc_data, adc_channel_callback[0].user_data);
	if (adc0_ch==1)
		if (adc_channel_callback[1].channel_cb_s) adc_channel_callback[1].channel_cb_s(adc_data, adc_channel_callback[1].user_data);
	if (adc0_ch==12)
		if (adc_channel_callback[2].channel_cb_s) adc_channel_callback[2].channel_cb_s(adc_data, adc_channel_callback[2].user_data);
	if (adc0_ch==13)
		if (adc_channel_callback[3].channel_cb_s) adc_channel_callback[3].channel_cb_s(adc_data, adc_channel_callback[3].user_data);

	adc0_ch++;
	if (adc0_ch==2)
		adc0_ch = 12;
 	if (adc0_ch==14)
 		adc0_ch = 0;
	adc_async_set_inputs(descr, adc0_ch, 0, channel);

	
	return;
}

static void convert_cb_ADC_1(const struct adc_async_descriptor *const descr, const uint8_t channel)
{
	adc_async_read_channel(descr, channel, &adc_data, sizeof(adc_data));
	adc1_ch++;
	if (adc1_ch==2)
		adc1_ch = 0;
	adc_async_set_inputs(descr, adc1_ch, 0, channel);
	return;
}

struct timer_task* RRRC_add_task(timer_cb_t func, uint32_t interval, void* user_data)
{
	struct timer_task* task = malloc(sizeof(struct timer_task));
	if (!task)
		return NULL;
	task->interval = interval;
	task->cb       = func;
	task->mode     = TIMER_TASK_REPEAT;
	task->user_data = user_data;
	timer_stop(&TIMER_RTC);
	timer_add_task(&TIMER_RTC, task);
	timer_start(&TIMER_RTC);
	return task;
}

void RRRC_remove_task(struct timer_task* task)
{
	if (!task)
		return;
	timer_stop(&TIMER_RTC);
	timer_remove_task(&TIMER_RTC, task);
	free(task);
	timer_start(&TIMER_RTC);
}


int RRRC_Init(void)
{
    adc_async_enable_channel(&ADC_0, 0);
    adc_async_register_callback(&ADC_0, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0);
	adc_async_set_inputs(&ADC_0, adc0_ch, 0, 0);
    adc_async_start_conversion(&ADC_0);

	adc_async_enable_channel(&ADC_1, 0);
    adc_async_register_callback(&ADC_1, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_1);
	adc_async_set_inputs(&ADC_1, adc1_ch, 0, 0);
    adc_async_start_conversion(&ADC_1);


	i2c_s_async_enable(&I2C_0);

	//i2c_s_async_get_io_descriptor(&I2C_0, &slave_i2c_io);
	//i2c_s_async_enable(&I2C_0);

// 	RRRC_add_task(&TIMER_0_task1_cb, 100, NULL);
// 	RRRC_add_task(&TIMER_0_task2_cb, 200, NULL);

	//timer_add_task(&TIMER_RTC, &TIMER_RTC_task1);
	//timer_add_task(&TIMER_RTC, &TIMER_RTC_task2);
	//timer_start(&TIMER_RTC);


 	

	//ext_irq_register(PIN_PB31, button_on_PB31_pressed);




	return 0;
}

uint32_t example_selection       = 1;
void tcc_examples()
{
	//TCC_DEFAULT_CONFIGURATION;
	switch (example_selection) {
		case 1:
		pwm_1khz_example();
		example_selection = 255;
		break;
		case 2:
		circular_buffer_example();
		example_selection = 255;
		break;
		case 3:
		dead_time_insertion_example();
		example_selection = 255;
		break;
		case 4:
		tcc_swap_feature_example();
		example_selection = 255;
		break;
		case 5:
		tcc_invert_feature_example();
		example_selection = 255;
		break;
		case 6:
		tcc_pattern_generation_example();
		example_selection = 255;
		break;
		case 7:
		tcc_ramp2_feature_example();
		example_selection = 255;
		break;
		case 8:
		tcc_ramp2a_feature_example();
		example_selection = 255;
		break;
		case 9:
		tcc_pwm_dual_slope_mode_example();
		example_selection = 255;
		break;
		default:
		break;
	}
}

// int l_pwm_enable = 0;
// int l_pwm_disable = 0;
// 
// int l_i2c_read = 1;
// int l_i2c_write = 0;
// uint8_t i2c_addr = 0xD0;
// uint8_t buffer[16];
// uint16_t io_size = 1;
// int ret =0;



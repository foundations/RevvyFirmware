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
#include "rrrc_i2c_protocol.h"

extern hw_motor_port_t motor_ports[MOTOR_PORT_AMOUNT];
extern hw_sensor_port_t sensor_ports[SENSOR_PORT_AMOUNT];

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
        uint8_t cmd = CommandHandler(rx_buffer.buff, rx_buffer.size);
        tx_buffer.size = MakeResponse(cmd, tx_buffer.buff);
        rx_buffer.size = 0;
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
        enum RRRC_I2C_CMD cmd = CommandHandler(rx_buffer.buff, rx_buffer.size);
        tx_buffer.size = MakeResponse(cmd, tx_buffer.buff);
        rx_buffer.size = 0;
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
static volatile int adc1_ch = 10;

struct _adc_channel_callback
{
	channel_adc_data_cb_t  channel_cb_s;
	void* user_data;
}adc_channel_callback[SENSOR_PORT_AMOUNT] = {{NULL,NULL},{NULL,NULL},{NULL,NULL},{NULL,NULL}};
//channel_adc_data_cb_t channel_cb_s[4]  = {NULL,NULL,NULL,NULL};

//*********************************************************************************************
void RRRC_channel_adc_register_cb(uint32_t chan_idx, channel_adc_data_cb_t func, void* user_data)
{
	if (chan_idx>=SENSOR_PORT_AMOUNT || func==NULL)
		return;
	adc_channel_callback[chan_idx].channel_cb_s = func;
	adc_channel_callback[chan_idx].user_data = user_data;
}

//*********************************************************************************************
void RRRC_channel_adc_unregister_cb(uint32_t chan_idx)
{
	if (chan_idx>=SENSOR_PORT_AMOUNT)
		return;
	adc_channel_callback[chan_idx].channel_cb_s = NULL;
	adc_channel_callback[chan_idx].user_data = NULL;
}

//*********************************************************************************************
static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
	//adc_async_read_channel(descr, channel, &adc_data, sizeof(adc_data));
	if (adc0_ch==0)
		if (adc_channel_callback[0].channel_cb_s) adc_channel_callback[0].channel_cb_s(adc_data, adc_channel_callback[0].user_data);
	if (adc0_ch==1)
		if (adc_channel_callback[1].channel_cb_s) adc_channel_callback[3].channel_cb_s(adc_data, adc_channel_callback[3].user_data);
	if (adc0_ch==13)
		if (adc_channel_callback[2].channel_cb_s) adc_channel_callback[1].channel_cb_s(adc_data, adc_channel_callback[1].user_data);
	if (adc0_ch==12)
		if (adc_channel_callback[3].channel_cb_s) adc_channel_callback[2].channel_cb_s(adc_data, adc_channel_callback[2].user_data);

	adc0_ch++;
	if (adc0_ch==2)
		adc0_ch = 12;
 	if (adc0_ch==14)
 		adc0_ch = 0;
	adc_async_set_inputs(descr, adc0_ch, 0, channel);

	
	return;
}

//*********************************************************************************************
static void convert_cb_ADC_1(const struct adc_async_descriptor *const descr, const uint8_t channel, uint16_t adc_data)
{
	//adc_async_read_channel(descr, channel, &adc_data, sizeof(adc_data));
	adc1_ch++;
	if (adc1_ch==2)
		adc1_ch = 0;
	adc_async_set_inputs(descr, adc1_ch, 0, channel);
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

//*********************************************************************************************
static void SensorsPinsInit()
{
	//led pins
	for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
	{
		gpio_set_pin_pull_mode(sensor_ports[idx].led0_gpio, GPIO_PULL_UP);
		gpio_set_pin_function(sensor_ports[idx].led0_gpio, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(sensor_ports[idx].led0_gpio, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(sensor_ports[idx].led0_gpio, false);

		gpio_set_pin_pull_mode(sensor_ports[idx].led1_gpio, GPIO_PULL_UP);
		gpio_set_pin_function(sensor_ports[idx].led1_gpio, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(sensor_ports[idx].led1_gpio, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(sensor_ports[idx].led1_gpio, false);
	}

	//gpio0 - in & extint
	for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
	{
		gpio_set_pin_pull_mode(sensor_ports[idx].gpio0_num, GPIO_PULL_OFF);
		gpio_set_pin_direction(sensor_ports[idx].gpio0_num, GPIO_DIRECTION_IN);
		gpio_set_pin_function(sensor_ports[idx].gpio0_num, GPIO_PIN_FUNCTION_A);		
	}

	//gpio1 - out
	for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
	{
		gpio_set_pin_pull_mode(sensor_ports[idx].gpio1_num, GPIO_PULL_OFF);
		gpio_set_pin_direction(sensor_ports[idx].gpio1_num, GPIO_DIRECTION_OUT);
		gpio_set_pin_function(sensor_ports[idx].gpio1_num, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_level(sensor_ports[idx].gpio1_num, false);
	}

	//adc pins
	for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
	{
		gpio_set_pin_direction(sensor_ports[idx].adc_gpio, GPIO_DIRECTION_OFF);
		gpio_set_pin_function(sensor_ports[idx].adc_gpio, GPIO_PIN_FUNCTION_B);
	}

	// i2c pins - TODO now this pins init in driver_init()
// 	for  (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++)
// 	{
// 		gpio_set_pin_pull_mode(sensor_ports[idx].i2c_gpio0, GPIO_PULL_OFF);
// 		gpio_set_pin_direction(sensor_ports[idx].i2c_gpio0, GPIO_DIRECTION_OFF);
// 		gpio_set_pin_function(sensor_ports[idx].i2c_gpio0, GPIO_PIN_FUNCTION_C);
// 		gpio_set_pin_pull_mode(sensor_ports[idx].i2c_gpio1, GPIO_PULL_OFF);
// 		gpio_set_pin_direction(sensor_ports[idx].i2c_gpio1, GPIO_DIRECTION_OFF);
// 		gpio_set_pin_function(sensor_ports[idx].i2c_gpio1, GPIO_PIN_FUNCTION_C);
// 	}

	for (int idx=0; idx<ARRAY_SIZE(sensor_ports); idx++) 
	{
		gpio_set_pin_pull_mode(sensor_ports[idx].vccio_pin, GPIO_PULL_UP);
		gpio_set_pin_function(sensor_ports[idx].vccio_pin, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(sensor_ports[idx].vccio_pin, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(sensor_ports[idx].vccio_pin, false);//VCCIO_3V3
	}
}

//*********************************************************************************************
static void MotorsPinsInit()
{

	//led pins
	for (int idx=0; idx<ARRAY_SIZE(motor_ports); idx++)
	{
		gpio_set_pin_pull_mode(motor_ports[idx].led0_gpio, GPIO_PULL_UP);
		gpio_set_pin_function(motor_ports[idx].led0_gpio, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(motor_ports[idx].led0_gpio, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(motor_ports[idx].led0_gpio, false);

		gpio_set_pin_pull_mode(motor_ports[idx].led1_gpio, GPIO_PULL_UP);
		gpio_set_pin_function(motor_ports[idx].led1_gpio, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(motor_ports[idx].led1_gpio, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(motor_ports[idx].led1_gpio, false);
	}

	//dir pins
	for (int idx=0; idx<ARRAY_SIZE(motor_ports); idx++)
	{
		gpio_set_pin_pull_mode(motor_ports[idx].dir0_gpio, GPIO_PULL_UP);
		gpio_set_pin_function(motor_ports[idx].dir0_gpio, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(motor_ports[idx].dir0_gpio, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(motor_ports[idx].dir0_gpio, false);

		gpio_set_pin_pull_mode(motor_ports[idx].dir1_gpio, GPIO_PULL_UP);
		gpio_set_pin_function(motor_ports[idx].dir1_gpio, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(motor_ports[idx].dir1_gpio, GPIO_DIRECTION_OUT);
		gpio_set_pin_level(motor_ports[idx].dir1_gpio, false);
	}

	//enc pins //TODO !!!!TESTING
	for (int idx=0; idx<ARRAY_SIZE(motor_ports); idx++)
	{
//		gpio_set_pin_function(motor_ports[idx].enc0_gpio, GPIO_PIN_FUNCTION_OFF);
// 		gpio_set_pin_direction(motor_ports[idx].enc0_gpio, GPIO_DIRECTION_OUT);
// 		gpio_set_pin_pull_mode(motor_ports[idx].enc0_gpio, GPIO_PULL_OFF);
// 		gpio_set_pin_level(motor_ports[idx].enc0_gpio, false);
// 		
// 		gpio_set_pin_function(motor_ports[idx].enc1_gpio, GPIO_PIN_FUNCTION_OFF);
// 		gpio_set_pin_direction(motor_ports[idx].enc1_gpio, GPIO_DIRECTION_OUT);
// 		gpio_set_pin_pull_mode(motor_ports[idx].enc1_gpio, GPIO_PULL_OFF);
// 		gpio_set_pin_level(motor_ports[idx].enc1_gpio, false);
//gpio_set_pin_pull_mode(motor_ports[idx].enc0_gpio, GPIO_PULL_DOWN);
		gpio_set_pin_direction(motor_ports[idx].enc0_gpio, GPIO_DIRECTION_IN);
		gpio_set_pin_function(motor_ports[idx].enc0_gpio, GPIO_PIN_FUNCTION_E);
//gpio_set_pin_pull_mode(motor_ports[idx].enc1_gpio, GPIO_PULL_DOWN);
		gpio_set_pin_direction(motor_ports[idx].enc1_gpio, GPIO_DIRECTION_IN);
		gpio_set_pin_function(motor_ports[idx].enc1_gpio, GPIO_PIN_FUNCTION_E);
	}

	//PWM pins 
	for (int idx=0; idx<ARRAY_SIZE(motor_ports); idx++)
	{
		gpio_set_pin_direction(motor_ports[idx].pwm_pin, GPIO_DIRECTION_OFF);
		gpio_set_pin_function(motor_ports[idx].pwm_pin, GPIO_PIN_FUNCTION_F);
	}

	uint8_t moto_stbypins[] = {M12STBY, M34STBY, M56STBY};
	for (int idx=0; idx<ARRAY_SIZE(moto_stbypins); idx++) // chips always ON
	{
		gpio_set_pin_pull_mode(moto_stbypins[idx], GPIO_PULL_UP);
		gpio_set_pin_function(moto_stbypins[idx], GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(moto_stbypins[idx], GPIO_DIRECTION_OUT);
		gpio_set_pin_level(moto_stbypins[idx], true);
	}

	gpio_set_pin_pull_mode(MOTPWEN, GPIO_PULL_UP);
	gpio_set_pin_function(MOTPWEN, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_direction(MOTPWEN, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(MOTPWEN, true);	

}

//*********************************************************************************************
void SystemMonitorPinsInit(void)
{
	gpio_set_pin_direction(BAT_TS, GPIO_DIRECTION_IN);
	gpio_set_pin_function(BAT_TS, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_direction(BAT_CHG, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(BAT_CHG, GPIO_PULL_UP);
	gpio_set_pin_function(BAT_CHG, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_direction(BAT_PG, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(BAT_PG, GPIO_PULL_UP);
	gpio_set_pin_function(BAT_PG, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_direction(BAT_ISET2, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(BAT_ISET2, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_level(BAT_ISET2, false); //100mA

	//register ADC callback for voltage monitoring
}

//*********************************************************************************************
int RRRC_Init(void)
{
	MotorsPinsInit();
	SensorsPinsInit();
	SystemMonitorPinsInit();

    adc_async_enable_channel(&ADC_0, 0);
    adc_async_register_callback(&ADC_0, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0);
	adc_async_set_inputs(&ADC_0, adc0_ch, 0, 0);
    adc_async_start_conversion(&ADC_0);

// 	adc_async_enable_channel(&ADC_1, 0);
//  adc_async_register_callback(&ADC_1, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_1);
// 	adc_async_set_inputs(&ADC_1, adc1_ch, 0, 0);
//  adc_async_start_conversion(&ADC_1);

 	i2c_s_async_enable(&I2C_0);

 	timer_start(&TIMER_TC0);
	timer_start(&TIMER_TC1);
	//timer_start(&TIMER_TC2);
	timer_start(&TIMER_TC3);
	timer_start(&TIMER_TC4);
	timer_start(&TIMER_TC5);
	timer_start(&TIMER_TC6);
	//timer_start(&TIMER_TC7);

	return 0;
}



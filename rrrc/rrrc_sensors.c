/*
 * rrrc_sensors.c
 *
 * Created: 1/11/2019 12:16:38 PM
 *  Author: User
 */ 


#include "rrrc_hal.h"
#include "rrrc_sensors.h"

#include "sensors/sensor_dummy.h"
#include "sensors/sensor_button.h"
#include "sensors/sensor_HC_SR05.h"




p_sensor_lib_entry_t sensor_libs[] =
{
	&sensor_dummy,
	&sensor_hc_sr_05,
	&sensor_button,
};

static hw_sensor_port_t sensor_ports[] = 
{
	{
		.index = 0,
		.sensor_lib = NULL,
		.lib_data = {0},

		.I2C = &I2C_1,

		.ADC = &ADC_0,
		.adc_chan_idx = 0,
		.gpio0_num = PA19, //as input and ext_int
		.gpio1_num = PA06, //as output

		.led0_gpio = -1,
		.led1_gpio = -1,

		.TIMER = &TIMER_TC0,
	},
	{
		.index = 1,
		.sensor_lib = NULL,
		.lib_data = {0},

		.I2C = &I2C_2,

		.ADC = &ADC_0,
		.adc_chan_idx = 1,

		.gpio0_num = PA22,
		.gpio1_num = -1,

		.led0_gpio = -1,
		.led1_gpio = -1,

		.TIMER = &TIMER_TC1,
	},
	{
		.index = 2,
		.sensor_lib = NULL,
		.lib_data = {0},

		.I2C = &I2C_3,

		.ADC = &ADC_1,
		.adc_chan_idx = 10,

		.gpio0_num = PA20,
		.gpio1_num = -1,

		.led0_gpio = -1,
		.led1_gpio = -1,

		.TIMER = &TIMER_TC2,
	},
	{
		.index = 3,
		.sensor_lib = NULL,
		.lib_data = {0},

		.sensor_thread = NULL,
		.I2C = &I2C_4,

		.ADC = &ADC_1,
		.adc_chan_idx = 11,
		
		.gpio0_num = PA21,
		.gpio1_num = -1,

		.led0_gpio = -1,
		.led1_gpio = -1,

		.TIMER = &TIMER_TC3,
	},
};

uint8_t SensorPortGetPortsAmount()
{
	return ARRAY_SIZE(sensor_ports);//SENSOR_PORT_AMOUNT;
}

uint8_t	SensorPortGetTypesAmount()
{
	return ARRAY_SIZE(sensor_libs);
}

char* SensorPortGetTypeName(uint32_t idx)
{
	if (idx<ARRAY_SIZE(sensor_libs))
	{
		p_sensor_lib_entry_t sensor = sensor_libs[idx];
		return sensor->name;
	}
	return NULL;
}

uint8_t SensorPortGetTypeId(uint32_t idx)
{
	if (idx<ARRAY_SIZE(sensor_libs))
	{
		p_sensor_lib_entry_t sensor = sensor_libs[idx];
		return sensor->type_id;
	}	
	return 0xFF;
}

uint32_t SensorPortGetTypes(uint8_t *data, uint32_t max_size)
{
	int sensor_types = ARRAY_SIZE(sensor_libs);
	uint32_t size = 0;
	
	for (int idx=0; idx<sensor_types; idx++)
	{
		uint32_t len = strlen(sensor_libs[idx]->name);
		ASSERT( (size+len+2)<max_size );
		*(data + size) = sensor_libs[idx]->type_id; size++;
		*(data + size) = len; size++;
		strncpy( (data + size), sensor_libs[idx]->name, len);
		size += len;
	}
	return size;
}

int32_t SensorPortSetType(uint32_t sens_port, sensor_type_t sens_type)
{
	uint32_t result = ERR_INVALID_DATA;

	if (sens_port<ARRAY_SIZE(sensor_ports) && sens_type<ARRAY_SIZE(sensor_libs))
	{
		sensor_ports[sens_port].sensor_lib = sensor_libs[sens_type];
		result = sensor_ports[sens_port].sensor_lib->SensorInit(&sensor_ports[sens_port]);
		result = ERR_NONE;
	}

	return result;
}

int32_t SensorPortGetType(uint32_t sens_port)
{
	uint32_t result = 0;
	if (sens_port<ARRAY_SIZE(sensor_ports) && sensor_ports[sens_port].sensor_lib)
	{
		result = sensor_ports[sens_port].sensor_lib->type_id;
	}
	return result;
}

int32_t SensorPortGetValues(uint32_t port, uint32_t* data)
{
	uint32_t result = 0;
	if (port < ARRAY_SIZE(sensor_ports) && sensor_ports[port].sensor_lib && sensor_ports[port].sensor_lib->sensor_get_values)
	{
		result = sensor_ports[port].sensor_lib->sensor_get_values(&sensor_ports[port], data, 12);
	}

	return result;
}

//*********************************************************************************************
static void SensorPort_thread_tick_cb(const struct timer_task *const timer_task)
{
	p_hw_sensor_port_t sensport = timer_task->user_data;
	if (sensport == NULL)
		return;
	
	if (sensport->sensor_lib && sensport->sensor_lib->sensor_thread)
		sensport->sensor_lib->sensor_thread(sensport);
}

//*********************************************************************************************
static void SensorPort_adc_cb(const uint8_t adc_data, void* port)
{
	p_hw_sensor_port_t sensport = port;
	if (sensport == NULL)
		return;
	
	if (sensport->sensor_lib && sensport->sensor_lib->adc_callback)
		sensport->sensor_lib->adc_callback(sensport, adc_data);

	return;
}

//*********************************************************************************************
static void SensorPort_gpio0_ext_cb(const void* port)
{
	p_hw_sensor_port_t sensport = port;
	if (sensport == NULL)
		return;

	uint32_t val = gpio_get_pin_level(sensport->gpio0_num);
	if (sensport->sensor_lib && sensport->sensor_lib->gpio0_callback)
		sensport->sensor_lib->gpio0_callback(sensport, val);

	return;
}

//*********************************************************************************************
static void SensorPort_gpio1_ext_cb(const void* port)
{
	p_hw_sensor_port_t sensport = port;
	if (sensport == NULL)
		return;

	uint32_t val = gpio_get_pin_level(sensport->gpio1_num);
	if (sensport->sensor_lib && sensport->sensor_lib->gpio1_callback)
		sensport->sensor_lib->gpio1_callback(sensport, val);

	return;
}

//*********************************************************************************************
int32_t SensorPortInit(uint32_t port)
{
	uint32_t result = ERR_NONE;
	if (port>=SENSOR_PORT_AMOUNT)
		return -1;

	sensor_ports[port].sensor_thread = RRRC_add_task(&SensorPort_thread_tick_cb, 1000/*ms*/, &sensor_ports[port]);
	
	//*****************************
	//I2C_init(sensor_ports[port].I2C)
	//I2C already init in Sync mode
	//But in future will be Async mode
	//*****************************
	
	RRRC_channel_adc_register_cb(sensor_ports[port].index, SensorPort_adc_cb, &sensor_ports[port]);

	if (sensor_ports[port].gpio0_num >= 0)
		ext_irq_register(sensor_ports[port].gpio0_num, SensorPort_gpio0_ext_cb, &sensor_ports[port]);
	if (sensor_ports[port].gpio1_num >= 0)
		ext_irq_register(sensor_ports[port].gpio1_num, SensorPort_gpio1_ext_cb, &sensor_ports[port]);

		//timer_

	//timer_stop(&TIMER_RTC);	timer_add_task(&TIMER_RTC, task);	timer_start(&TIMER_RTC);

	result = SensorPortSetType(port, SENSOR_NOT_SET);

	return result;
}
int32_t SensorPortDeInit(uint32_t port)
{
	uint32_t result = ERR_NONE;
	if (sensor_ports[port].sensor_thread)
		RRRC_remove_task(sensor_ports[port].sensor_thread);
	sensor_ports[port].sensor_thread = NULL;

	RRRC_channel_adc_unregister_cb(sensor_ports[port].index);

	if (sensor_ports[port].gpio0_num >= 0)
		ext_irq_disable(sensor_ports[port].gpio0_num);
	if (sensor_ports[port].gpio1_num >= 0)
		ext_irq_disable(sensor_ports[port].gpio1_num);

	return result;
}
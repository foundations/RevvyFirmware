/*
 * rrrc_sensors.c
 *
 * Created: 1/11/2019 12:16:38 PM
 *  Author: User
 */ 


#include "rrrc_hal.h"
#include "rrrc_sensors.h"

#include "sensors/rrrc_sensor_base_func.h"
#include "sensors/sensor_dummy.h"
#include "sensors/sensor_button.h"
#include "sensors/sensor_analog_button.h"
#include "sensors/sensor_HC_SR05.h"
#include "sensors/sensor_i2chub.h"

p_sensor_lib_entry_t sensor_libs[] =
{
	&sensor_dummy,
	&sensor_hc_sr_05,
	&sensor_button,
	&sensor_analog_button,
	&sensor_i2chub
};

hw_sensor_port_t sensor_ports[] = 
{
	{
		.index = 0,
		.sensor_lib = NULL,
		.lib_data = {0},

		.I2C = &I2C_4,
		.i2c_sda = S0I2Csda,
		.i2c_scl = S0I2Cscl,

		.ADC = &ADC_0,
		.adc_chan_idx = 0,
		.adc_gpio = S0ADC,

		.gpio0_num = S0GPIO0, //as input and ext_int
		.gpio1_num = S0GPIO1, //as output

		.led0_gpio = S0LED0,
		.led1_gpio = S0LED1,
		
		.vccio_pin = S0IOVCC,
	},
	{
		.index = 1,
		.sensor_lib = NULL,
		.lib_data = {0},

		.I2C = &I2C_1,
		.i2c_sda = S1I2Csda,
		.i2c_scl = S1I2Cscl,

		.ADC = &ADC_0,
		.adc_chan_idx = 1,
		.adc_gpio = S1ADC,

		.gpio0_num = S1GPIO0, //as input and ext_int
		.gpio1_num = S1GPIO1, //as output

		.led0_gpio = S1LED0,
		.led1_gpio = S1LED1,

		.vccio_pin = S1IOVCC,
	},
	{
		.index = 2,
		.sensor_lib = NULL,
		.lib_data = {0},

		.I2C = &I2C_2,
		.i2c_sda = S2I2Csda,
		.i2c_scl = S2I2Cscl,

		.ADC = &ADC_0,
		.adc_chan_idx = 10,
		.adc_gpio = S2ADC,

		.gpio0_num = S2GPIO0, //as input and ext_int
		.gpio1_num = S2GPIO1, //as output

		.led0_gpio = S2LED0,
		.led1_gpio = S2LED1,
		
		.vccio_pin = S2IOVCC,

	},
	{
		.index = 3,
		.sensor_lib = NULL,
		.lib_data = {0},
			
		.I2C = &I2C_3,
		.i2c_sda = S3I2Csda,
		.i2c_scl = S3I2Cscl,

		.ADC = &ADC_0,
		.adc_chan_idx = 11,
		.adc_gpio = S3ADC,
		
		.gpio0_num = S3GPIO0, //as input and ext_int
		.gpio1_num = S3GPIO1, //as output

		.led0_gpio = S3LED0,
		.led1_gpio = S3LED1,

		.vccio_pin = S3IOVCC,
	},
};

//*********************************************************************************************
uint8_t SensorPortGetPortsAmount()
{
	return ARRAY_SIZE(sensor_ports);//SENSOR_PORT_AMOUNT;
}

//*********************************************************************************************
uint8_t	SensorPortGetTypesAmount()
{
	return ARRAY_SIZE(sensor_libs);
}

//*********************************************************************************************
char* SensorPortGetTypeName(uint32_t idx)
{
	if (idx<ARRAY_SIZE(sensor_libs))
	{
		p_sensor_lib_entry_t sensor = sensor_libs[idx];
		return sensor->name;
	}
	return NULL;
}

//*********************************************************************************************
uint8_t SensorPortGetTypeId(uint32_t idx)
{
	if (idx<ARRAY_SIZE(sensor_libs))
	{
		p_sensor_lib_entry_t sensor = sensor_libs[idx];
		return sensor->type_id;
	}
	return 0xFF;
}

//*********************************************************************************************
uint32_t SensorPortGetTypes(uint8_t *data, uint32_t max_size)
{
	int sensor_types = ARRAY_SIZE(sensor_libs);
	uint32_t size = 0u;
	
	for (uint32_t idx = 0u; idx < sensor_types; idx++)
	{
		uint32_t len = strlen(sensor_libs[idx]->name);
		if ( (size + len + 2u) >= max_size )
		{
            /* return with as much data as we can */
            break;
		}
        data[size++] = sensor_libs[idx]->type_id;
        data[size++] = len;
        strncpy( &data[size], sensor_libs[idx]->name, len);
		size += len;
	}
	return size;
}

//*********************************************************************************************
int32_t SensorPortSetType(uint32_t port_idx, sensor_type_t sens_type)
{
	uint32_t result = ERR_INVALID_DATA;

	if (port_idx<ARRAY_SIZE(sensor_ports) && sens_type<ARRAY_SIZE(sensor_libs))
	{
		if (sensor_ports[port_idx].sensor_lib != NULL)
		{
			if(sensor_ports[port_idx].sensor_lib->SensorDeInit)
				sensor_ports[port_idx].sensor_lib->SensorDeInit(&sensor_ports[port_idx]);
		}
		
		sensor_ports[port_idx].sensor_lib = sensor_libs[sens_type];
		result = sensor_ports[port_idx].sensor_lib->SensorInit(&sensor_ports[port_idx]);
		if ((result == ERR_NONE) && (sens_type != SENSOR_NOT_SET))
			SensorPort_led0_on(&sensor_ports[port_idx]);
		else
			SensorPort_led0_off(&sensor_ports[port_idx]);
	}

	return result;
}

//*********************************************************************************************
int32_t SensorPortGetType(uint32_t port_idx)
{
	uint32_t result = 0;
	if (port_idx<ARRAY_SIZE(sensor_ports) && sensor_ports[port_idx].sensor_lib)
	{
		result = sensor_ports[port_idx].sensor_lib->type_id;
	}
	return result;
}

uint32_t SensorPortGetValues(uint32_t port_idx, uint32_t* data)
{
	uint32_t amount = 0;
	if (port_idx < ARRAY_SIZE(sensor_ports) && sensor_ports[port_idx].sensor_lib && sensor_ports[port_idx].sensor_lib->sensor_get_values)
	{
		amount = sensor_ports[port_idx].sensor_lib->sensor_get_values(&sensor_ports[port_idx], data, 12);
	}

	return amount;
}

//*********************************************************************************************
static void SensorPort_adc_cb(const uint16_t adc_data, void* port)
{
	p_hw_sensor_port_t sensport = port;
	if (sensport == NULL)
		return;
	
	if (sensport->sensor_lib && sensport->sensor_lib->adc_callback)
		sensport->sensor_lib->adc_callback(sensport, adc_data);

	return;
}

void SensorPort_Adc_Update(uint32_t port_idx, uint16_t adc_data)
{
    if (port_idx < ARRAY_SIZE(sensor_ports))
	{
		SensorPort_adc_cb(adc_data, &sensor_ports[port_idx]);
	}
}

//*********************************************************************************************
static void SensorPort_gpio0_ext_cb(const void* port)
{
	p_hw_sensor_port_t sensport = port;
	if (sensport == NULL)
		return;

	uint32_t val = SensorPort_gpio0_get_state(sensport);
	//uint32_t val = gpio_get_pin_level(sensport->gpio0_num);
	if (sensport->sensor_lib && sensport->sensor_lib->gpio0_callback)
		sensport->sensor_lib->gpio0_callback(sensport, val);

	return;
}

//*********************************************************************************************
uint32_t SensorPortWriteUserData(uint32_t port, uint32_t* data, uint32_t size)
{
	p_hw_sensor_port_t sensport = port;
	if (sensport == NULL)
		return;

	if (sensport->sensor_lib && sensport->sensor_lib->write_data)
		sensport->sensor_lib->write_data(sensport, data, size);

	return;
}

//*********************************************************************************************
uint32_t SensorPortReadUserData(uint32_t port, uint32_t* data, uint32_t size)
{
	p_hw_sensor_port_t sensport = port;
	if (sensport == NULL)
		return;

	if (sensport->sensor_lib && sensport->sensor_lib->read_data)
		sensport->sensor_lib->read_data(sensport, data, size);

	return;
}

//*********************************************************************************************
int32_t SensorPortInit(uint32_t port_idx)
{
	uint32_t result = ERR_NONE;
	if (port_idx>=SENSOR_PORT_AMOUNT)
		return ERR_INVALID_DATA;

	result = SensorPortSetType(port_idx, SENSOR_NOT_SET);

	//RRRC_channel_adc_register_cb(0, sensor_ports[port_idx].index, SensorPort_adc_cb, &sensor_ports[port_idx]);

	if (sensor_ports[port_idx].gpio0_num >= 0)
		ext_irq_register(sensor_ports[port_idx].gpio0_num, SensorPort_gpio0_ext_cb, &sensor_ports[port_idx]);

	//*****************************
	//I2C_init(sensor_ports[port].I2C)
	//I2C already init in Sync mode
	//But in future will be Async mode
	//*****************************

	return result;
}

//*********************************************************************************************
int32_t SensorPortDeInit(uint32_t port_idx)
{
	uint32_t result = ERR_NONE;

	SensorPortSetType(port_idx, SENSOR_NOT_SET);

	//RRRC_channel_adc_unregister_cb(0, sensor_ports[port_idx].index);

	if (sensor_ports[port_idx].gpio0_num >= 0)
		ext_irq_disable(sensor_ports[port_idx].gpio0_num);

	return result;
}


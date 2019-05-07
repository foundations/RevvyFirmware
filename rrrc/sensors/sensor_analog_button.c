/*
 * sensor_analog_button.c
 *
 * Created: 3/6/2019 9:25:54 PM
 *  Author: User
 */ 

#include "rrrc_sensor_base_func.h"
#include "sensor_analog_button.h"
#include "utils/functions.h"
#include <string.h>

#define MAX_SENSOR_VALUES 1

//*********************************************************************************************
int32_t ABUTTON_Init(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_sensor_port_t sensport = (p_hw_sensor_port_t) hw_port;
	if (sensport)
    {
        memset(sensport->lib_data, 0, SENSOR_PORT_LIBDATA);
    }
	return result;
}

//*********************************************************************************************
uint32_t ABUTTON_get_value(void* hw_port, uint32_t* data, uint32_t max_size)
{
	uint32_t amount = 0;
	p_hw_sensor_port_t sensport = (p_hw_sensor_port_t) hw_port;
	if (sensport && data && (max_size >= MAX_SENSOR_VALUES))
	{
		p_abutton_data_t sens_data = (p_abutton_data_t) sensport->lib_data;
		data[0] = SwapEndian(sens_data->state);
		amount = 1;
	}
	return amount;
}

//*********************************************************************************************
void ABUTTON_adc_callback(void* hw_port, uint32_t data)
{
    p_hw_sensor_port_t sensport = (p_hw_sensor_port_t) hw_port;
    if (sensport)
    {
        p_abutton_data_t sens_data = (p_abutton_data_t) sensport->lib_data;
        if (data > 127u)
        {
            sens_data->state = 1;
            SensorPort_led1_on(sensport);
        }
        else
        {
            sens_data->state = 0;
            SensorPort_led1_off(sensport);
        }
    }

    return;
}

 //*********************************************************************************************
int32_t ABUTTON_DeInit(void* hw_port)
{
	p_hw_sensor_port_t sensport = (p_hw_sensor_port_t) hw_port;
	if (sensport)
	{
		SensorPort_led1_off(sensport);
	}

	return ERR_NONE;
}

sensor_lib_entry_t sensor_analog_button =
{
	.type_id = SENSOR_ANALOG_SWITCH,
	.name = "ABUTTON",
	.SensorInit = ABUTTON_Init,
	.SensorDeInit = ABUTTON_DeInit,
	.sensor_get_values = ABUTTON_get_value,
	.write_data = NULL,
	.read_data = NULL,

	//callback from sensor port
	.timer_callback = NULL,
	.adc_callback = ABUTTON_adc_callback,
	.gpio0_callback = NULL,
};

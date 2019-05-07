
#include "rrrc_sensor_base_func.h"
#include "sensor_button.h"
#include "utils/functions.h"

#include <string.h>

#define MAX_SENSOR_VALUES 1

//*********************************************************************************************
int32_t BUTTON_Init(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_sensor_port_t sensport = (p_hw_sensor_port_t) hw_port;
    if (sensport)
    {
        memset(sensport->lib_data, 0, SENSOR_PORT_LIBDATA);
    }

	p_button_data_t sens_data = sensport->lib_data;
	sens_data->state = SensorPort_gpio0_get_state(sensport) ? 0 : 1;
	if (sens_data->state)
	{
    	SensorPort_led1_on(sensport);
	}
	else
	{
    	SensorPort_led1_off(sensport);
	}
	return result;
}

//*********************************************************************************************
int32_t BUTTON_DeInit(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_sensor_port_t sensport = (p_hw_sensor_port_t) hw_port;
	if (sensport)
    {
        SensorPort_led1_off(sensport);
    }

    return result;
}

//*********************************************************************************************
uint32_t BUTTON_get_value(void* hw_port, uint32_t* data, uint32_t max_size)
{
	uint32_t amount = 0;
	p_hw_sensor_port_t sensport = (p_hw_sensor_port_t) hw_port;
	if (sensport && data && (max_size >= MAX_SENSOR_VALUES))
	{
		p_button_data_t sens_data = sensport->lib_data;
		data[0] = SwapEndian(sens_data->state);
		amount = 1;
	}
	return amount;
}

//*********************************************************************************************
void BUTTON_gpio0_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = (p_hw_sensor_port_t) hw_port;
	if (sensport)
	{
        p_button_data_t sens_data = (p_button_data_t) sensport->lib_data;
        sens_data->state = data ? 0 : 1;
        if (sens_data->state)
        {
            SensorPort_led1_on(sensport);
        }
        else
        {
            SensorPort_led1_off(sensport);
        }
	}
}

sensor_lib_entry_t sensor_button =
{
    .type_id = SENSOR_SWITCH,
    .name = "BUTTON",
    .SensorInit = BUTTON_Init,
	.SensorDeInit = BUTTON_DeInit,
	.sensor_get_values = BUTTON_get_value,
	.write_data = NULL,
	.read_data = NULL,

    //callback from sensor port
    .timer_callback = NULL,
    .adc_callback = NULL,
    .gpio0_callback = BUTTON_gpio0_callback,
};

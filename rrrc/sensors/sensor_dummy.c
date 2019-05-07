
#include "rrrc_sensor_base_func.h"
#include "sensor_dummy.h"

#define MAX_SENSOR_VALUES 0

//*********************************************************************************************
int32_t SENSOR_DUMMY_Init(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_sensor_port_t sensport = hw_port;

	return result;
}

//*********************************************************************************************
void SENSOR_DUMMY_gpio0_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = hw_port;
}

//*********************************************************************************************
void SENSOR_DUMMY_adc_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = hw_port;
}

//*********************************************************************************************
void SENSOR_DUMMY_timer_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = hw_port;
}

sensor_lib_entry_t sensor_dummy =
{
    .type_id = SENSOR_NOT_SET,
    .name = "NO_SET",
    .SensorInit = SENSOR_DUMMY_Init,
	.SensorDeInit = NULL,
	.write_data = NULL,
	.read_data = NULL,

    //callback from sensor port
    .timer_callback = SENSOR_DUMMY_timer_callback,
    .adc_callback = SENSOR_DUMMY_adc_callback,
    .gpio0_callback = SENSOR_DUMMY_gpio0_callback,
};

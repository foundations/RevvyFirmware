
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
void SENSOR_DUMMY_Thread(void* hw_port)
{
	p_hw_sensor_port_t sensport = hw_port;

	return;
}

//*********************************************************************************************
void SENSOR_DUMMY_gpio0_callback(void* hw_port, uint32_t data)
{

	p_hw_sensor_port_t sensport = hw_port;

	return;
}

void SENSOR_DUMMY_gpio1_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = hw_port;

	return;
}

//*********************************************************************************************
void SENSOR_DUMMY_adc_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = hw_port;

	return;
}

//*********************************************************************************************
void SENSOR_DUMMY_timer_callback(void* hw_port, uint32_t data)
{
	p_hw_sensor_port_t sensport = hw_port;

	return;
}
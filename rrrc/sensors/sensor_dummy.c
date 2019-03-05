

#include "sensor_dummy.h"

#define MAX_SENSOR_VALUES 0


int32_t SENSOR_DUMMY_Init(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_sensor_port_t sensport = hw_port;

	return result;
}


void SENSOR_DUMMY_Thread(void* hw_port)
{
	return;
}

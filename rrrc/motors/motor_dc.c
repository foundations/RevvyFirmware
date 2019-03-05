/*
 * motor_dc.c
 *
 * Created: 2/27/2019 9:10:32 PM
 *  Author: User
 */ 

 #include "motor_dc.h"

 #define MAX_MOTOR_VALUES 1
 int32_t DC_Init(void* hw_port)
 {
	int32_t result = ERR_NONE;
	p_hw_motor_port_t motsport = hw_port;
	if (motsport)
		memset(motsport->lib_data, 0, MOTOR_PORT_LIBDATA);
	return result;
 }

 uint32_t DC_get_counter(void* hw_port, uint32_t* data, uint32_t max_size)
 {
		uint32_t amount = 0;
		p_hw_motor_port_t motport = hw_port;
		if (motport && data && max_size && (max_size>=MAX_MOTOR_VALUES))
		{
			p_dc_data_t mot_data = motport->lib_data;
			data[0] = mot_data->counter_forward + mot_data->counter_backward;
			amount = 1;
// 			data[0] = mot_data->counter_forward;
// 			data[1] = mot_data->counter_backward;
// 			amount = 2;
		}
		return amount;
 }

int8_t DC_get_state(void* hw_port)
{
uint32_t state = 0;
p_hw_motor_port_t motport = hw_port;
if (motport)
{
	p_dc_data_t mot_data = motport->lib_data;
	state = mot_data->state;
}
return state;
}

uint32_t DC_set_state(void* hw_port, int8_t state)
{
	uint32_t result = 0;
	p_hw_motor_port_t motport = hw_port;
	if (motport)
	{
		p_dc_data_t mot_data = motport->lib_data;
		//TODO
		//if (set new state == OK)
		//{
		//	mot_data->state = state;
		//	result = OK;
		//}else
		//	result = ERROR;
		mot_data->state = state;
		
	}
	return result;
}

 void DC_Thread(void* hw_port)
 {
  	p_hw_motor_port_t motport = hw_port;
  	if (motport)
  	{

  	}
 }
 void DC_enc0_callback(void* hw_port, uint32_t data)
 {
 	p_hw_motor_port_t motport = hw_port;
 	if (motport)
 	{

 	}
 }

 void DC_enc1_callback(void* hw_port, uint32_t data)
 {
 	p_hw_motor_port_t motport = hw_port;
 	if (motport)
 	{
	 	p_dc_data_t mot_data = motport->lib_data;
	 	
 	}
 }
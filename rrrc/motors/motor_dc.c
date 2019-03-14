/*
 * motor_dc.c
 *
 * Created: 2/27/2019 9:10:32 PM
 *  Author: User
 */ 
#include "rrrc_motor_base_function.h"
#include "motor_dc.h"

#define MAX_MOTOR_VALUES 1

//*********************************************************************************************
int32_t DC_Init(void* hw_port)
{
	int32_t result = ERR_NONE;
	p_hw_motor_port_t motport = hw_port;
	if (motport)
		memset(motport->lib_data, 0, MOTOR_PORT_LIBDATA);
	p_dc_data_t mot_data = motport->lib_data;
	
	timer_get_clock_cycles_in_tick(motport->pwm, &(mot_data->timer_period));
	return result;
}

//*********************************************************************************************
uint32_t DC_get_counter(void* hw_port, uint32_t* data, uint32_t max_size)
{
	uint32_t amount = 0;
	p_hw_motor_port_t motport = hw_port;
	if (motport && data && max_size && (max_size>=MAX_MOTOR_VALUES))
	{
		p_dc_data_t mot_data = motport->lib_data;
// 			data[0] = mot_data->counter_forward + mot_data->counter_backward;
// 			amount = 1;
		data[0] = SwapEndian(mot_data->counter_forward);
		data[1] = SwapEndian(mot_data->counter_backward);
		amount = 2;
	}
	return amount;
}

//*********************************************************************************************
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

//*********************************************************************************************
uint32_t DC_set_state(void* hw_port, int8_t state)
{
	uint32_t result = 0;
	p_hw_motor_port_t motport = hw_port;
	if (motport)
	{
		p_dc_data_t mot_data = motport->lib_data;
		if (mot_data->state != state)
		{
			uint8_t speed = abs(state);
			uint32_t duty = mot_data->timer_period/100*speed;
			MotorPort_dir_stop(hw_port);
			if (state)
			{
				MotorPort_set_duty_cicle(hw_port, duty); //TODO  What if speed 0??
				if (state > 0)
				MotorPort_dir_forward(hw_port);
				else
				MotorPort_dir_backward(hw_port);
			}
			//mot_data->speed = speed;
			mot_data->state = state;
		}
		
	}
	return result;
}

//*********************************************************************************************
void DC_Thread(void* hw_port)
{
	p_hw_motor_port_t motport = hw_port;
	if (motport)
	{

	}
}

//*********************************************************************************************
void DC_enc0_callback(void* hw_port, uint32_t data)
{
	p_hw_motor_port_t motport = hw_port;
	if (motport)
	{
		p_dc_data_t mot_data = motport->lib_data;
		mot_data->counter_forward = ++;
	}
}

//*********************************************************************************************
void DC_enc1_callback(void* hw_port, uint32_t data)
{
	p_hw_motor_port_t motport = hw_port;
	if (motport)
	{
		p_dc_data_t mot_data = motport->lib_data;
		mot_data->counter_backward = ++;
	}
}

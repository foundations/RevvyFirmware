/*
 * motor_dc.h
 *
 * Created: 2/27/2019 9:09:47 PM
 *  Author: User
 */ 


#ifndef MOTOR_DC_H_
#define MOTOR_DC_H_

#include <stddef.h>
#include "rrrc_motor_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t DC_Init(void* hw_port);
uint32_t DC_get_counter(void* hw_port, uint32_t* data, uint32_t max_size);
int8_t DC_get_state(void* hw_port);
uint32_t DC_set_state(void* hw_port, int8_t state);
void DC_Thread(void* hw_port);
void DC_enc0_callback(void* hw_port, uint32_t data);
void DC_enc1_callback(void* hw_port, uint32_t data);

static motor_lib_entry_t motor_dc =
{
	.type_id = MOTOR_DC,
	.name = "MOTOR_DC",
	.MotorInit = DC_Init,
	.MotorDeInit = NULL,
	.motor_get_counter = DC_get_counter,
	.motor_get_state = DC_get_state,
	.motor_set_state = DC_set_state,
	.motor_thread = DC_Thread,

	//callback from motor port
	.enc0_callback = DC_enc0_callback,
	.enc1_callback = DC_enc1_callback,
};



typedef struct _dc_data_t
{
	int8_t state;
	//uint32_t speed;
	uint32_t steps; //run up to steps and stop, if 0 free run
	uint32_t counter_forward;
	uint32_t counter_backward;
	uint32_t timer_period;
}dc_data_t, *p_dc_data_t;

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_DC_H_ */
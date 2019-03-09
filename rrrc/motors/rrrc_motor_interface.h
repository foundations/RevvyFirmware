/*
 * rrrc_motor_interface.h
 *
 * Created: 2/27/2019 8:34:21 PM
 *  Author: User
 */ 


#ifndef RRRC_MOTOR_INTERFACE_H_
#define RRRC_MOTOR_INTERFACE_H_

#include <stdint.h>
#include "err_codes.h"

#ifdef __cplusplus
extern "C" {
#endif


#define MOTOR_PORT_LIBDATA 128

typedef enum _motor_type_t
{
	MOTOR_NOT_SET,
	MOTOR_DC,
	MOTOR_SERVO
}motor_type_t;

typedef struct _motor_lib_entry_t
{
	motor_type_t type_id;
	char* name;
	void* port;

	int32_t (*MotorInit)(void* hw_port);
	uint32_t (*motor_get_counter)(void* hw_port, uint32_t* data, uint32_t max_size);
	int8_t (*motor_get_state)(void* hw_port);
	uint32_t (*motor_set_state)(void* hw_port, int8_t state);
	uint32_t (*motor_set_steps)(void* hw_port, uint32_t steps);
	void (*motor_thread)(void* hw_port);

	//callback from encoder
	void (*enc0_callback)(void* hw_port, uint32_t state);
	void (*enc1_callback)(void* hw_port, uint32_t state);
}motor_lib_entry_t, *p_motor_lib_entry_t;



typedef uint8_t gpio_num;

typedef struct _hw_motor_port_t
{
	uint32_t index;
	p_motor_lib_entry_t motor_lib;

	uint8_t lib_data[MOTOR_PORT_LIBDATA];

	struct timer_task* motor_thread;

	struct timer_descriptor *ENCTIMER;
	gpio_num enc0_gpio;
	gpio_num enc1_gpio;

	gpio_num dir0_gpio;
	gpio_num dir1_gpio;

	gpio_num led0_gpio;
	gpio_num led1_gpio;
	
	struct timer_descriptor *PWM0;
	uint32_t pwm0_ch;
	gpio_num pwm_pin;

	struct timer_descriptor *PWM1;
	uint32_t pwm1_ch;

	

}hw_motor_port_t, *p_hw_motor_port_t;

#ifdef __cplusplus
}
#endif



#endif /* RRRC_MOTOR_INTERFACE_H_ */
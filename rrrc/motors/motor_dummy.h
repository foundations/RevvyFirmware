/*
 * motor_dummy.h
 *
 * Created: 2/27/2019 9:09:35 PM
 *  Author: User
 */ 


#ifndef MOTOR_DUMMY_H_
#define MOTOR_DUMMY_H_

#include <stddef.h>
#include "rrrc_motor_interface.h"

#ifdef __cplusplus
extern "C" {
	#endif

	int32_t MOTOR_DUMMY_Init(void* hw_port);
	void MOTOR_DUMMY_Thread(void* hw_port);
	void MOTOR_DUMMY_enc0_callback(void* hw_port, uint32_t data);
	void MOTOR_DUMMY_enc1_callback(void* hw_port, uint32_t data);

	static motor_lib_entry_t motor_dummy =
	{
		.type_id = MOTOR_NOT_SET,
		.name = "MOTOR_NO_SET",
		.MotorInit = MOTOR_DUMMY_Init,
		.motor_thread = MOTOR_DUMMY_Thread,

		//callback from motor port
		.enc0_callback = MOTOR_DUMMY_enc0_callback,
		.enc1_callback = MOTOR_DUMMY_enc1_callback,
	};



	#ifdef __cplusplus
}
#endif



#endif /* MOTOR_DUMMY_H_ */
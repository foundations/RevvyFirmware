/*
 * rrrc_motors.c
 *
 * Created: 1/13/2019 10:08:41 PM
 *  Author: User
 */ 
#include "rrrc_hal.h"
#include "rrrc_motors.h"

#include "motors/rrrc_motor_base_function.h"
#include "motors/motor_dummy.h"
#include "motors/motor_dc.h"
#include "motors/motor_servo.h"

p_motor_lib_entry_t motor_libs[] =
{
	&motor_dummy,
	&motor_dc,
	&motor_servo,
};

hw_motor_port_t motor_ports[] =
{
	{
		.index = 0,
		.motor_lib = NULL,
		.lib_data = {0},
		.xMotorPortTask = NULL,
		.enc_timer = &TIMER_TC0,
		.enc0_gpio = M1ENC0,
		.enc1_gpio = M1ENC1,
		.dir0_gpio = M1DIR1,
		.dir1_gpio = M1DIR2,
		.led0_gpio = M0LED0,
		.led1_gpio = M0LED1,
		.pwm = &TIMER_TCC0,
		.pwm_ch = 1,
		.pwm_pin = M0PWM0,
	},
	{
		.index = 1,
		.motor_lib = NULL,
		.lib_data = {0},
		.xMotorPortTask = NULL,
		.enc_timer = &TIMER_TC6,
		.enc0_gpio = M2ENC0,
		.enc1_gpio = M2ENC1,
		.dir0_gpio = M2DIR1,
		.dir1_gpio = M2DIR2,
		.led0_gpio = M1LED0,
		.led1_gpio = M1LED1,
		.pwm = &TIMER_TCC0,
		.pwm_ch = 0,
		.pwm_pin = M1PWM0,
	},
	{
		.index = 2,
		.motor_lib = NULL,
		.lib_data = {0},
		.xMotorPortTask = NULL,
		.enc_timer = &TIMER_TC3,
		.enc0_gpio = M3ENC0,
		.enc1_gpio = M3ENC1,
		.dir0_gpio = M3DIR1,
		.dir1_gpio = M3DIR2,
		.led0_gpio = M2LED0,
		.led1_gpio = M2LED1,
		.pwm = &TIMER_TCC0,
		.pwm_ch = 5,
		.pwm_pin = M3PWM0,
	},
	{
		.index = 3,
		.motor_lib = NULL,
		.lib_data = {0},
		.xMotorPortTask = NULL,
		.enc_timer = &TIMER_TC5,
		.enc0_gpio = M4ENC0,
		.enc1_gpio = M4ENC1,
		.dir0_gpio = M4DIR1,
		.dir1_gpio = M4DIR2,
		.led0_gpio = M3LED0,
		.led1_gpio = M3LED1,
		.pwm = &TIMER_TCC0,
		.pwm_ch = 2,
		.pwm_pin = M2PWM0,
	},
	{
		.index = 4,
		.motor_lib = NULL,
		.lib_data = {0},
		.xMotorPortTask = NULL,
		.enc_timer = &TIMER_TC1,
		.enc0_gpio = M5ENC0,
		.enc1_gpio = M5ENC1,
		.dir0_gpio = M5DIR1,
		.dir1_gpio = M5DIR2,
		.led0_gpio = M4LED0,
		.led1_gpio = M4LED1,
		.pwm = &TIMER_TCC0,
		.pwm_ch = 3,
		.pwm_pin = M4PWM0,
	},
	{
		.index = 5,
		.motor_lib = NULL,
		.lib_data = {0},
		.xMotorPortTask = NULL,
		.enc_timer = &TIMER_TC4,
		.enc0_gpio = M6ENC0,
		.enc1_gpio = M6ENC1,
		.dir0_gpio = M6DIR1,
		.dir1_gpio = M6DIR2,
		.led0_gpio = M5LED0,
		.led1_gpio = M5LED1,
		.pwm = &TIMER_TCC0,
		.pwm_ch = 4,
		.pwm_pin = M5PWM0,
	},
};

//*********************************************************************************************
uint8_t MotorPortGetPortsAmount()
{
	return ARRAY_SIZE(motor_ports); //MOTOR_PORT_AMOUNT;
}

//*********************************************************************************************
uint8_t	MotorPortGetTypesAmount()
{
	return ARRAY_SIZE(motor_libs);
}

//*********************************************************************************************
char* MotorPortGetTypeName(uint32_t idx)
{
	if (idx<ARRAY_SIZE(motor_libs))
	{
		p_motor_lib_entry_t motor = motor_libs[idx];
		return motor->name;
	}
	return NULL;
}

//*********************************************************************************************
uint8_t MotorPortGetTypeId(uint32_t idx)
{
	if (idx<ARRAY_SIZE(motor_libs))
	{
		p_motor_lib_entry_t motor = motor_libs[idx];
		return motor->type_id;
	}
	return 0xFF;
}

//*********************************************************************************************
uint32_t MotorPortGetTypes(uint8_t *data, uint32_t max_size)
{
	int motor_types = ARRAY_SIZE(motor_libs);
	uint32_t size = 0;
	
	for (int idx=0; idx<motor_types; idx++)
	{
		uint32_t len = strlen(motor_libs[idx]->name);
		ASSERT( (size+len+2)<max_size );
		*(data + size) = motor_libs[idx]->type_id; size++;
		*(data + size) = len; size++;
		strncpy( (data + size), motor_libs[idx]->name, len);
		size += len;
	}
	return size;
}

//*********************************************************************************************
uint32_t MotorPortSetType(uint32_t port_idx, motor_type_t type)
{
	uint32_t result = ERR_INVALID_DATA;

	if (port_idx<ARRAY_SIZE(motor_ports) && type<ARRAY_SIZE(motor_libs))
	{
		motor_ports[port_idx].motor_lib = motor_libs[type];
		if (motor_ports[port_idx].motor_lib->MotorDeInit)
			motor_ports[port_idx].motor_lib->MotorDeInit(&motor_ports[port_idx]);
		result = motor_ports[port_idx].motor_lib->MotorInit(&motor_ports[port_idx]);
		if ((result == ERR_NONE) && (type != MOTOR_NOT_SET))
		{
			MotorPort_led0_on(&motor_ports[port_idx]);
		}
		else
		{
			MotorPort_led0_off(&motor_ports[port_idx]);
		}
	}
	return result;
}

//*********************************************************************************************
uint32_t MotorPortGetType(uint32_t port_idx)
{
	uint32_t result = 0;
	if (port_idx<ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib)
	{
		result = motor_ports[port_idx].motor_lib->type_id;
	}
	return result;
}

//*********************************************************************************************
uint32_t MotorPortSetState(uint32_t port_idx, int8_t state)
{
	uint32_t result = 0;
	if (port_idx<ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_set_state)
	{
		result = motor_ports[port_idx].motor_lib->motor_set_state(&motor_ports[port_idx], state);
	}
	return result;
}

//*********************************************************************************************
int8_t MotorPortGetState(uint32_t port_idx)
{
	int8_t result = 0;
	if (port_idx<ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_get_state)
	{
		result = motor_ports[port_idx].motor_lib->motor_get_state(&motor_ports[port_idx]);
	}
	return result;
}

//*********************************************************************************************
uint32_t MotoPortSetSteps(uint32_t port_idx, uint32_t steps)
{
	uint32_t result = 0;
	if (port_idx<ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_set_steps)
	{
		result = motor_ports[port_idx].motor_lib->motor_set_steps(&motor_ports[port_idx], steps);
	}
	return result;
}

//*********************************************************************************************
uint32_t MotorPortGetCount(uint32_t port_idx, uint32_t* data)
{
	uint32_t result = 0;
	if (port_idx < ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_get_counter)
	{
		result = motor_ports[port_idx].motor_lib->motor_get_counter(&motor_ports[port_idx], data, 12);
	}
	return result;
}

//*********************************************************************************************
static void MotorPort_xTask(const void* user_data)
{
	p_hw_motor_port_t motport = user_data;
	if (motport == NULL)
		return;
	for(;;)
	{
		if (motport->motor_lib && motport->motor_lib->motor_thread)
			motport->motor_lib->motor_thread(motport);
		os_sleep(200);
	}
}

//*********************************************************************************************
static void MotorPort_enc0_cb(uint32_t data, void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;

	if (motport->motor_lib && motport->motor_lib->enc0_callback)
		motport->motor_lib->enc0_callback(motport, data);

	return;
}

//*********************************************************************************************
static void MotorPort_enc1_cb(uint32_t data, void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;

	if (motport->motor_lib && motport->motor_lib->enc1_callback)
		motport->motor_lib->enc1_callback(motport, data);

	return;
}

//*********************************************************************************************
int32_t MotorPortInit(uint32_t port_idx)
{
	uint32_t result = ERR_NONE;
	if (port_idx>=ARRAY_SIZE(motor_ports))
		return ERR_INVALID_DATA;
		
	result = MotorPortSetType(port_idx, MOTOR_NOT_SET);
	if (result)
		return result;

	timer_register_cb(motor_ports[port_idx].enc_timer,TIMER_MC0, MotorPort_enc0_cb, &motor_ports[port_idx]);
	timer_register_cb(motor_ports[port_idx].enc_timer,TIMER_MC1, MotorPort_enc1_cb, &motor_ports[port_idx]);


	char task_name[configMAX_TASK_NAME_LEN+1];
	snprintf(task_name, configMAX_TASK_NAME_LEN, "motorport%01d", port_idx);
	if (xTaskCreate(MotorPort_xTask, task_name, 256 / sizeof(portSTACK_TYPE), &motor_ports[port_idx], tskIDLE_PRIORITY+1, &motor_ports[port_idx].xMotorPortTask) != pdPASS) 
		return ERR_FAILURE;

	return result;
}

//*********************************************************************************************
int32_t MotorPortDeInit(uint32_t port_idx)
{
	uint32_t result = ERR_NONE;
	if (port_idx>=ARRAY_SIZE(motor_ports))
		return ERR_INVALID_DATA;

	MotorPortSetType(port_idx, MOTOR_NOT_SET);

	vTaskDelete(motor_ports[port_idx].xMotorPortTask);

	timer_unregister_cb(motor_ports[port_idx].enc_timer,TIMER_MC0);
	timer_unregister_cb(motor_ports[port_idx].enc_timer,TIMER_MC1);

	return result;
}
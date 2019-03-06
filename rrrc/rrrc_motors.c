/*
 * rrrc_motors.c
 *
 * Created: 1/13/2019 10:08:41 PM
 *  Author: User
 */ 
#include "driver_init.h"
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
		.motor_thread = NULL,
		.enc0_gpio = M1ENC0,
		.enc1_gpio = M1ENC1,
		.dir0_gpio = M1DIR1,
		.dir1_gpio = M1DIR2,
		.led0_gpio = M0LED0,
		.led1_gpio = M0LED1,
		.PWM0 = &TIMER_TCC0,
		.pwm0_ch = 0,
		.PWM1 = &TIMER_TCC1,
		.pwm1_ch = 0,
	},
	{
		.index = 1,
		.motor_lib = NULL,
		.lib_data = {0},
		.motor_thread = NULL,
		.enc0_gpio = M2ENC0,
		.enc1_gpio = M2ENC1,
		.dir0_gpio = M2DIR1,
		.dir1_gpio = M2DIR2,
		.led0_gpio = M1LED0,
		.led1_gpio = M1LED1,
		.PWM0 = &TIMER_TCC0,
		.pwm0_ch = 1,
		.PWM1 = &TIMER_TCC1,
		.pwm1_ch = 1,
	},
	{
		.index = 2,
		.motor_lib = NULL,
		.lib_data = {0},
		.motor_thread = NULL,
		.enc0_gpio = M3ENC0,
		.enc1_gpio = M3ENC1,
		.dir0_gpio = M3DIR1,
		.dir1_gpio = M3DIR2,
		.led0_gpio = M2LED0,
		.led1_gpio = M2LED1,
		.PWM0 = &TIMER_TCC0,
		.pwm0_ch = 2,
		.PWM1 = &TIMER_TCC1,
		.pwm1_ch = 2,
	},
	{
		.index = 3,
		.motor_lib = NULL,
		.lib_data = {0},
		.motor_thread = NULL,
		.enc0_gpio = M4ENC0,
		.enc1_gpio = M4ENC1,
		.dir0_gpio = M4DIR1,
		.dir1_gpio = M4DIR2,
		.led0_gpio = M3LED0,
		.led1_gpio = M3LED1,
		.PWM0 = &TIMER_TCC0,
		.pwm0_ch = 3,
		.PWM1 = &TIMER_TCC1,
		.pwm1_ch = 3,
	},
	{
		.index = 4,
		.motor_lib = NULL,
		.lib_data = {0},
		.motor_thread = NULL,
		.enc0_gpio = M5ENC0,
		.enc1_gpio = M5ENC1,
		.dir0_gpio = M5DIR1,
		.dir1_gpio = M5DIR2,
		.led0_gpio = M4LED0,
		.led1_gpio = M4LED1,
		.PWM0 = &TIMER_TCC0,
		.pwm0_ch = 4,
		.PWM1 = &TIMER_TCC2,
		.pwm1_ch = 0,
	},
	{
		.index = 5,
		.motor_lib = NULL,
		.lib_data = {0},
		.motor_thread = NULL,
		.enc0_gpio = M6ENC0,
		.enc1_gpio = M6ENC1,
		.dir0_gpio = M6DIR1,
		.dir1_gpio = M6DIR2,
		.led0_gpio = M5LED0,
		.led1_gpio = M5LED1,
		.PWM0 = &TIMER_TCC0,
		.pwm0_ch = 5,
		.PWM1 = &TIMER_TCC2,
		.pwm1_ch = 1,
	},
};

uint8_t MotorPortGetPortsAmount()
{
	return ARRAY_SIZE(motor_ports); //MOTOR_PORT_AMOUNT;
}

uint8_t	MotorPortGetTypesAmount()
{
	return ARRAY_SIZE(motor_libs);
}

char* MotorPortGetTypeName(uint32_t idx)
{
	if (idx<ARRAY_SIZE(motor_libs))
	{
		p_motor_lib_entry_t motor = motor_libs[idx];
		return motor->name;
	}
	return NULL;
}

uint8_t MotorPortGetTypeId(uint32_t idx)
{
	if (idx<ARRAY_SIZE(motor_libs))
	{
		p_motor_lib_entry_t motor = motor_libs[idx];
		return motor->type_id;
	}
	return 0xFF;
}

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

uint32_t MotorPortSetType(uint32_t port_idx, motor_type_t type)
{
	uint32_t result = ERR_INVALID_DATA;

	if (port_idx<ARRAY_SIZE(motor_ports) && type<ARRAY_SIZE(motor_libs))
	{
		motor_ports[port_idx].motor_lib = motor_libs[type];
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

uint32_t MotorPortGetType(uint32_t port_idx)
{
	uint32_t result = 0;
	if (port_idx<ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib)
	{
		result = motor_ports[port_idx].motor_lib->type_id;
	}
	return result;
}

uint32_t MotorPortSetState(uint32_t port_idx, int8_t state)
{
	uint32_t result = 0;
	if (port_idx<ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_set_state)
	{
		result = motor_ports[port_idx].motor_lib->motor_set_state(&motor_ports[port_idx], state);
	}
	return result;
}

int8_t MotorPortGetState(uint32_t port_idx)
{
	int8_t result = 0;
	if (port_idx<ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_get_state)
	{
		result = motor_ports[port_idx].motor_lib->motor_get_state(&motor_ports[port_idx]);
	}
	return result;
}

uint32_t MotoPortSetSteps(uint32_t port_idx, uint32_t steps)
{
	uint32_t result = 0;
	if (port_idx<ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_set_steps)
	{
		result = motor_ports[port_idx].motor_lib->motor_set_steps(&motor_ports[port_idx], steps);
	}
	return result;
}

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
static void MotorPort_thread_tick_cb(const struct timer_task *const timer_task)
{
	p_hw_motor_port_t motport = timer_task->user_data;
	if (motport == NULL)
		return;
	
	if (motport->motor_lib && motport->motor_lib->motor_thread)
	motport->motor_lib->motor_thread(motport);
}

//*********************************************************************************************
static void MotorPort_enc0_cb(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;

	uint32_t val = gpio_get_pin_level(motport->enc0_gpio);
	if (motport->motor_lib && motport->motor_lib->enc0_callback)
		motport->motor_lib->enc0_callback(motport, val);

	return;
}

//*********************************************************************************************
static void MotorPort_enc1_cb(const void* port)
{
	p_hw_motor_port_t motport = port;
	if (motport == NULL)
		return;

	uint32_t val = gpio_get_pin_level(motport->enc1_gpio);
	if (motport->motor_lib && motport->motor_lib->enc1_callback)
		motport->motor_lib->enc1_callback(motport, val);

	return;
}


static void MotorPinsInit(p_hw_motor_port_t port)
{
	//encoder pins
	//dir pins
	//leds pins
}

//*********************************************************************************************
int32_t MotorPortInit(uint32_t port)
{
	uint32_t result = ERR_NONE;
	if (port>=ARRAY_SIZE(motor_ports))
		return ERR_INVALID_DATA;

	motor_ports[port].motor_thread = RRRC_add_task(&MotorPort_thread_tick_cb, 1000/*ms*/, &motor_ports[port]);

	if (motor_ports[port].enc0_gpio >= 0)
		ext_irq_register(motor_ports[port].enc0_gpio, MotorPort_enc0_cb, &motor_ports[port]);
	if (motor_ports[port].enc1_gpio >= 0)
		ext_irq_register(motor_ports[port].enc1_gpio, MotorPort_enc1_cb, &motor_ports[port]);

	result = MotorPortSetType(port, MOTOR_NOT_SET);

	return result;
}

int32_t MotorPortDeInit(uint32_t port)
{
	uint32_t result = ERR_NONE;
	if (motor_ports[port].motor_thread)
		RRRC_remove_task(motor_ports[port].motor_thread);
	motor_ports[port].motor_thread = NULL;


	if (motor_ports[port].enc0_gpio >= 0)
		ext_irq_disable(motor_ports[port].enc0_gpio);
	if (motor_ports[port].enc1_gpio >= 0)
		ext_irq_disable(motor_ports[port].enc1_gpio);

	return result;
}
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
#include "motors/motor_openloop.h"
#include "motors/motor_position_controlled.h"
#include "motors/motor_speed_controlled.h"
#include "rrrc_tb6612fng.h"

tb6612fng_t motor_driver_configs[] =
{
    {
        .dir0_gpio = M1DIR1,
        .dir1_gpio = M1DIR2,
        .pwm = &TIMER_TCC0,
        .pwm_ch = 1
    },
    {
        .dir0_gpio = M2DIR1,
        .dir1_gpio = M2DIR2,
        .pwm = &TIMER_TCC0,
        .pwm_ch = 0
    },
    {
        .dir0_gpio = M3DIR1,
        .dir1_gpio = M3DIR2,
        .pwm = &TIMER_TCC0,
        .pwm_ch = 5
    },
    {
        .dir0_gpio = M4DIR1,
        .dir1_gpio = M4DIR2,
        .pwm = &TIMER_TCC0,
        .pwm_ch = 2
    },
    {
        .dir0_gpio = M5DIR1,
        .dir1_gpio = M5DIR2,
        .pwm = &TIMER_TCC0,
        .pwm_ch = 3
    },
    {
        .dir0_gpio = M6DIR1,
        .dir1_gpio = M6DIR2,
        .pwm = &TIMER_TCC0,
        .pwm_ch = 4
    },
};

p_motor_lib_entry_t motor_libs[] =
{
    &motor_dummy,
    &motor_openloop,
    &motor_position_controlled,
    &motor_speed_controlled
};

hw_motor_port_t motor_ports[] =
{
    {
        .index = 0,
        .motor_lib = NULL,
        .lib_data = {0},
        .enc_timer = &TIMER_TC0,
        .enc0_gpio = M1ENC0,
        .enc1_gpio = M1ENC1,
        .led0_gpio = M0LED0,
        .led1_gpio = M0LED1,
        .pwm_pin = M0PWM0,
        .motor_driver_lib = &motor_driver_tb6612fng,
        .motorDriverConfig = &motor_driver_configs[0]
    },
    {
        .index = 1,
        .motor_lib = NULL,
        .lib_data = {0},
        .enc_timer = &TIMER_TC6,
        .enc0_gpio = M2ENC0,
        .enc1_gpio = M2ENC1,
        .led0_gpio = M1LED0,
        .led1_gpio = M1LED1,
        .pwm_pin = M1PWM0,
        .motor_driver_lib = &motor_driver_tb6612fng,
        .motorDriverConfig = &motor_driver_configs[1]
    },
    {
        .index = 2,
        .motor_lib = NULL,
        .lib_data = {0},
        .enc_timer = &TIMER_TC3,
        .enc0_gpio = M3ENC0,
        .enc1_gpio = M3ENC1,
        .led0_gpio = M2LED0,
        .led1_gpio = M2LED1,
        .pwm_pin = M2PWM0,
        .motor_driver_lib = &motor_driver_tb6612fng,
        .motorDriverConfig = &motor_driver_configs[2]
    },
    {
        .index = 3,
        .motor_lib = NULL,
        .lib_data = {0},
        .enc_timer = &TIMER_TC5,
        .enc0_gpio = M4ENC0,
        .enc1_gpio = M4ENC1,
        .led0_gpio = M3LED0,
        .led1_gpio = M3LED1,
        .pwm_pin = M3PWM0,
        .motor_driver_lib = &motor_driver_tb6612fng,
        .motorDriverConfig = &motor_driver_configs[3]
    },
    {
        .index = 4,
        .motor_lib = NULL,
        .lib_data = {0},
        .enc_timer = &TIMER_TC1,
        .enc0_gpio = M5ENC0,
        .enc1_gpio = M5ENC1,
        .led0_gpio = M4LED0,
        .led1_gpio = M4LED1,
        .pwm_pin = M4PWM0,
        .motor_driver_lib = &motor_driver_tb6612fng,
        .motorDriverConfig = &motor_driver_configs[4]
    },
    {
        .index = 5,
        .motor_lib = NULL,
        .lib_data = {0},
        .enc_timer = &TIMER_TC4,
        .enc0_gpio = M6ENC0,
        .enc1_gpio = M6ENC1,
        .led0_gpio = M5LED0,
        .led1_gpio = M5LED1,
        .pwm_pin = M5PWM0,
        .motor_driver_lib = &motor_driver_tb6612fng,
        .motorDriverConfig = &motor_driver_configs[5]
    },
};

//*********************************************************************************************
uint8_t MotorPortGetPortsAmount()
{
    return ARRAY_SIZE(motor_ports); //MOTOR_PORT_AMOUNT;
}

//*********************************************************************************************
uint8_t    MotorPortGetTypesAmount()
{
    return ARRAY_SIZE(motor_libs);
}

//*********************************************************************************************
char* MotorPortGetTypeName(uint32_t idx)
{
    if (idx < ARRAY_SIZE(motor_libs))
    {
        p_motor_lib_entry_t motor = motor_libs[idx];
        return motor->name;
    }
    return NULL;
}

//*********************************************************************************************
uint8_t MotorPortGetTypeId(uint32_t idx)
{
    if (idx < ARRAY_SIZE(motor_libs))
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
    uint32_t size = 0u;
    
    for (uint32_t idx = 0u; idx < motor_types; idx++)
    {
        uint32_t len = strlen(motor_libs[idx]->name);
        if ( (size + len + 2u) >= max_size )
        {
            return 0u;
        }
        data[size++] = motor_libs[idx]->type_id;
        data[size++] = len;
        strncpy( &data[size], motor_libs[idx]->name, len);
        size += len;
    }
    return size;
}

//*********************************************************************************************
uint32_t MotorPortSetType(uint32_t port_idx, motor_type_t type)
{
    uint32_t result = ERR_INVALID_DATA;

    if (port_idx < ARRAY_SIZE(motor_ports) && type < ARRAY_SIZE(motor_libs))
    {
        if (motor_ports[port_idx].motor_lib)
        {
            MotorPort_led0_off(&motor_ports[port_idx]);
            if (motor_ports[port_idx].motor_lib->MotorDeInit)
            {
                /*if (motor_ports[port_idx].enc0_gpio >= 0)
                    ext_irq_disable(motor_ports[port_idx].enc0_gpio);
                if (motor_ports[port_idx].enc1_gpio >= 0)
                    ext_irq_disable(motor_ports[port_idx].enc1_gpio);*/
                    
                timer_stop(motor_ports[port_idx].enc_timer);
                motor_ports[port_idx].motor_lib->MotorDeInit(&motor_ports[port_idx]);
            }
        }

        motor_ports[port_idx].motor_lib = motor_libs[type];

        result = motor_ports[port_idx].motor_lib->MotorInit(&motor_ports[port_idx]);
        if ((result == ERR_NONE) && (type != MOTOR_NOT_SET))
        {
            MotorPort_led0_on(&motor_ports[port_idx]);

            /*if (motor_ports[port_idx].enc0_gpio >= 0 && motor_ports[port_idx].motor_lib->gpio0_callback)
                ext_irq_enable(motor_ports[port_idx].enc0_gpio);
            if (motor_ports[port_idx].enc1_gpio >= 0 && motor_ports[port_idx].motor_lib->gpio1_callback)
                ext_irq_enable(motor_ports[port_idx].enc1_gpio);*/
            timer_start(motor_ports[port_idx].enc_timer);
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
    if (port_idx < ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib)
    {
        result = motor_ports[port_idx].motor_lib->type_id;
    }
    return result;
}

//*********************************************************************************************
uint32_t MotorPortSetControl(uint32_t port_idx, int32_t value)
{
    uint32_t result = 0;
    if (port_idx < ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_set_control)
    {
        result = motor_ports[port_idx].motor_lib->motor_set_control(&motor_ports[port_idx], value);
    }
    return result;
}

//*********************************************************************************************
int32_t MotorPortSetConfig(uint32_t port_idx, uint8_t *data, uint32_t size)
{
    int32_t result = ERR_INVALID_ARG;
    if (port_idx < ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_set_config)
    {
        result = motor_ports[port_idx].motor_lib->motor_set_config(&motor_ports[port_idx], data, size);
    }
    return result;
}

//*********************************************************************************************
int8_t MotorPortGetState(uint32_t port_idx)
{
    int8_t result = 0;
    /*if (port_idx < ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_get_state)
    {
        result = motor_ports[port_idx].motor_lib->motor_get_state(&motor_ports[port_idx]);
    }*/
    return result;
}

//*********************************************************************************************
uint32_t MotoPortSetSteps(uint32_t port_idx, uint32_t steps)
{
    uint32_t result = 0;
    /*if (port_idx < ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_set_steps)
    {
        result = motor_ports[port_idx].motor_lib->motor_set_steps(&motor_ports[port_idx], steps);
    }*/
    return result;
}

//*********************************************************************************************
uint32_t MotorPortGetPosition(uint32_t port_idx, uint32_t* data)
{
    uint32_t result = 0;
    if (port_idx < ARRAY_SIZE(motor_ports) && motor_ports[port_idx].motor_lib && motor_ports[port_idx].motor_lib->motor_get_position)
    {
        result = motor_ports[port_idx].motor_lib->motor_get_position(&motor_ports[port_idx], data, 12);
    }
    return result;
}

//*********************************************************************************************
static void MotorPort_gpio0_ext_cb(uint32_t data, void* port)
{
    p_hw_motor_port_t motport = port;
    if (motport == NULL)
        return;

    if (motport->motor_lib && motport->motor_lib->gpio0_callback)
    {
        uint32_t val = MotorPort_gpio0_get_state(motport);
        motport->motor_lib->gpio0_callback(motport, val);
    }

    return;
}

//*********************************************************************************************
static void MotorPort_gpio1_ext_cb(uint32_t data, void* port)
{
    p_hw_motor_port_t motport = port;
    if (motport == NULL)
        return;

    if (motport->motor_lib && motport->motor_lib->gpio1_callback)
    {
        uint32_t val = MotorPort_gpio1_get_state(motport);
        motport->motor_lib->gpio1_callback(motport, val);
    }

    return;
}

//*********************************************************************************************
int32_t MotorPortInit(uint32_t port_idx)
{
    if (port_idx >= ARRAY_SIZE(motor_ports))
    {
        return ERR_INVALID_DATA;
    }

    if (motor_ports[port_idx].enc0_gpio >= 0)
    {
        gpio_set_pin_direction(motor_ports[port_idx].enc0_gpio, GPIO_DIRECTION_IN);
        gpio_set_pin_function(motor_ports[port_idx].enc0_gpio, GPIO_PIN_FUNCTION_E);
        gpio_set_pin_pull_mode(motor_ports[port_idx].enc0_gpio, GPIO_PULL_UP);
        //int32_t result = ext_irq_register(motor_ports[port_idx].enc0_gpio, MotorPort_gpio0_ext_cb, &motor_ports[port_idx]);
        int32_t result = timer_register_cb(motor_ports[port_idx].enc_timer,TIMER_MC0, MotorPort_gpio0_ext_cb, &motor_ports[port_idx]);
        ASSERT(result == ERR_NONE);
    }

    if (motor_ports[port_idx].enc1_gpio >= 0)
    {
        gpio_set_pin_direction(motor_ports[port_idx].enc1_gpio, GPIO_DIRECTION_IN);
        gpio_set_pin_function(motor_ports[port_idx].enc1_gpio, GPIO_PIN_FUNCTION_E);
        gpio_set_pin_pull_mode(motor_ports[port_idx].enc1_gpio, GPIO_PULL_UP);
        //int32_t result = ext_irq_register(motor_ports[port_idx].enc1_gpio, MotorPort_gpio1_ext_cb, &motor_ports[port_idx]);
        int32_t result = timer_register_cb(motor_ports[port_idx].enc_timer,TIMER_MC1, MotorPort_gpio1_ext_cb, &motor_ports[port_idx]);
        ASSERT(result == ERR_NONE);
    }
    
    return MotorPortSetType(port_idx, MOTOR_NOT_SET);
}

//*********************************************************************************************
int32_t MotorPortDeInit(uint32_t port_idx)
{
    uint32_t result = ERR_NONE;
    if (port_idx >= ARRAY_SIZE(motor_ports))
        return ERR_INVALID_DATA;

    MotorPortSetType(port_idx, MOTOR_NOT_SET);

    return result;
}

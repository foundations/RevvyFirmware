/*
 * motor_openloop.c
 *
 * Created: 4/3/2019 3:22:02 PM
 *  Author: Daniel Buga
 */ 

#include "motor_openloop.h"

static int32_t saturate_int32(int32_t value, int32_t min, int32_t max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    else
    {
        return value;
    }
}

typedef struct {} *p_motor_openloop_data_t;

static int32_t MOTOR_OPENLOOP_Init(void* hw_port)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_openloop_data_t data = (p_motor_openloop_data_t) motport->lib_data;

    ASSERT(motport && motport->motor_driver_lib);
    motport->motor_driver_lib->init(motport);

    return ERR_NONE;
}

static void MOTOR_OPENLOOP_DeInit(void* hw_port)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_openloop_data_t data = (p_motor_openloop_data_t) motport->lib_data;

    motport->motor_driver_lib->deinit(motport);
}

static void MOTOR_OPENLOOP_Update(void* hw_port)
{

}

static int32_t MOTOR_OPENLOOP_set_config(void* hw_port, const uint8_t* pData, uint32_t size)
{
    return ERR_NONE;
}

static uint32_t MOTOR_OPENLOOP_get_config(void* hw_port, uint8_t* data, uint32_t max_size)
{
    return 0;
}

static uint32_t MOTOR_OPENLOOP_get_position(void* hw_port, uint8_t* data, uint32_t max_size)
{
    return 0;
}

static uint32_t MOTOR_OPENLOOP_set_control(void* hw_port, int32_t value)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_openloop_data_t data = (p_motor_openloop_data_t) motport->lib_data;

    /* Note: even though the motor drivers internally limit the speed, 
     * saturation is done because of the int32->int8 conversion */

    // TODO: configurable limits?
    motport->motor_driver_lib->set_speed(motport, saturate_int32(value, -100, 100));

    return 0;
}

static uint32_t MOTOR_OPENLOOP_get_control(void* hw_port, int32_t* data, uint32_t max_size)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;

    if (max_size < 4)
    {
        return 0;
    }

    *data = motport->motor_driver_lib->get_speed(motport);

    return sizeof(int32_t);
}

motor_lib_entry_t motor_openloop =
{
    .type_id = MOTOR_OPENLOOP,
    .name    = "MOTOR_OPEN_LOOP",

    .MotorInit   = &MOTOR_OPENLOOP_Init,
    .MotorDeInit = &MOTOR_OPENLOOP_DeInit,
    .update      = &MOTOR_OPENLOOP_Update,

    .motor_set_config = &MOTOR_OPENLOOP_set_config,
    .motor_get_config = &MOTOR_OPENLOOP_get_config,

    .motor_get_position = &MOTOR_OPENLOOP_get_position,

    .motor_set_control = &MOTOR_OPENLOOP_set_control,
    .motor_get_control = &MOTOR_OPENLOOP_get_control,

    .gpio0_callback = NULL,
    .gpio1_callback = NULL
};

/*
 * motor_position_controlled.c
 *
 * Created: 4/4/2019 10:39:58 AM
 *  Author: Daniel Buga
 */ 

#include "motor_position_controlled.h"
#include "controller/pid.h"

typedef struct {
    PID_t controller;
} *p_motor_pos_ctrl_data_t;

static int32_t MOTOR_POSITION_CONTROLLED_Init(void* hw_port)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    ASSERT(motport && motport->motor_driver_lib);
    motport->motor_driver_lib->init(motport);

    pid_initialize(&data->controller);

    return ERR_NONE;
}

static void MOTOR_POSITION_CONTROLLED_DeInit(void* hw_port)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    motport->motor_driver_lib->deinit(motport);
}

static int32_t MOTOR_POSITION_CONTROLLED_set_config(void* hw_port, const uint8_t* pData, uint32_t size)
{
    return ERR_NONE;
}

static uint32_t MOTOR_POSITION_CONTROLLED_get_config(void* hw_port, uint8_t* data, uint32_t max_size)
{
    return 0;
}

static uint32_t MOTOR_POSITION_CONTROLLED_get_position(void* hw_port, uint8_t* data, uint32_t max_size)
{
    return 0;
}

static uint32_t MOTOR_POSITION_CONTROLLED_set_control(void* hw_port, int32_t value)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    motport->motor_driver_lib->set_speed(motport, value);

    return 0;
}

static uint32_t MOTOR_POSITION_CONTROLLED_get_control(void* hw_port, int32_t* data, uint32_t max_size)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;

    if (max_size < 4)
    {
        return 0;
    }

    *data = motport->motor_driver_lib->get_speed(motport);

    return sizeof(int32_t);
}

static void MOTOR_POSITION_CONTROLLED_gpio0_callback(void* hw_port, uint32_t state) 
{

}

static void MOTOR_POSITION_CONTROLLED_gpio1_callback(void* hw_port, uint32_t state) 
{

}

motor_lib_entry_t motor_position_controlled =
{
    .type_id = MOTOR_POSITION_CONTROLLED,
    .name    = "MOTOR_POSITION_CONTROLLED",

    .MotorInit   = &MOTOR_POSITION_CONTROLLED_Init,
    .MotorDeInit = &MOTOR_POSITION_CONTROLLED_DeInit,

    .motor_set_config = &MOTOR_POSITION_CONTROLLED_set_config,
    .motor_get_config = &MOTOR_POSITION_CONTROLLED_get_config,

    .motor_get_position = &MOTOR_POSITION_CONTROLLED_get_position,

    .motor_set_control = &MOTOR_POSITION_CONTROLLED_set_control,
    .motor_get_control = &MOTOR_POSITION_CONTROLLED_get_control,

    .gpio0_callback = &MOTOR_POSITION_CONTROLLED_gpio0_callback,
    .gpio1_callback = &MOTOR_POSITION_CONTROLLED_gpio1_callback
};

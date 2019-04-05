/*
 * motor_speed_controlled.c
 *
 * Created: 4/4/2019 10:39:18 AM
 *  Author: Daniel Buga
 */ 

#include "motor_speed_controlled.h"
#include "controller/pid.h"
#include "converter.h"
#include "rrrc_motor_base_function.h"

#include "jscope/jscope.h"

#include <math.h>

typedef struct {
    PID_t controller;
    TaskHandle_t task;

    int32_t refPosition;

    int32_t position;
} *p_motor_pos_ctrl_data_t;

static void MOTOR_POSITION_CONTROLLED_Task(void* userData)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) userData;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        int32_t pos;
        CRITICAL_SECTION_ENTER();
        pos = data->position;
        CRITICAL_SECTION_LEAVE();
        float u = pid_update(&data->controller, data->refPosition, pos);
        motport->motor_driver_lib->set_speed(motport, lroundf(u));
        
        float fpos = (float)pos;
        float fref = (float)data->refPosition;

        jscope_update(3 * (motport->index) + 0, &fpos);
        jscope_update(3 * (motport->index) + 1, &fref);

        vTaskDelay(rtos_ms_to_ticks(20));
	}
}

static int32_t MOTOR_POSITION_CONTROLLED_Init(void* hw_port)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    ASSERT(motport && motport->motor_driver_lib);
    motport->motor_driver_lib->init(motport);

    pid_initialize(&data->controller);

    if (xTaskCreate(MOTOR_POSITION_CONTROLLED_Task, "SPD_CTRL", 1024 / sizeof(portSTACK_TYPE), motport, tskIDLE_PRIORITY + 1, &data->task) != pdPASS)
        return ERR_FAILURE;

    return ERR_NONE;
}

static void MOTOR_POSITION_CONTROLLED_DeInit(void* hw_port)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

	vTaskDelete(data->task);

    motport->motor_driver_lib->deinit(motport);
}

static int32_t MOTOR_POSITION_CONTROLLED_set_config(void* hw_port, const uint8_t* pData, uint32_t size)
{
    if (size != 5 * 4)
    {
        return ERR_INVALID_DATA;
    }

    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    CRITICAL_SECTION_ENTER();
    data->controller.config.P = get_float(&pData[0]);
    data->controller.config.I = get_float(&pData[4]);
    data->controller.config.D = get_float(&pData[8]);
    data->controller.config.LowerLimit = get_float(&pData[12]);
    data->controller.config.UpperLimit = get_float(&pData[16]);
    CRITICAL_SECTION_LEAVE();

    return ERR_NONE;
}

static uint32_t MOTOR_POSITION_CONTROLLED_get_config(void* hw_port, uint8_t* data, uint32_t max_size)
{
    return 0;
}

static uint32_t MOTOR_POSITION_CONTROLLED_get_position(void* hw_port, uint8_t* dst, uint32_t max_size)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;
    
    uint32_t src = SwapEndian((uint32_t) (data->position & 0xFFFFFFFFu));
    memcpy(dst, &src, 4);
    
    return 4;
}

static uint32_t MOTOR_POSITION_CONTROLLED_set_control(void* hw_port, int32_t value)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    data->refPosition = value;

    return 0;
}

static uint32_t MOTOR_POSITION_CONTROLLED_get_control(void* hw_port, int32_t* dst, uint32_t max_size)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    if (max_size < 4)
    {
        return 0;
    }

    *dst = data->refPosition;

    return sizeof(int32_t);
}

static void MOTOR_POSITION_CONTROLLED_gpio0_callback(void* hw_port, uint32_t state) 
{
    int32_t delta = 0;
    if (state)
    {
        // rising edge
        if (MotorPort_gpio1_get_state(hw_port))
        {
            delta = -1;
        }
        else
        {
            delta = 1;
        }
    }
    else
    {
        // falling edge
        if (MotorPort_gpio1_get_state(hw_port))
        {
            delta = 1;
        }
        else
        {
            delta = -1;
        }
    }

    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    data->position += delta;
}

static void MOTOR_POSITION_CONTROLLED_gpio1_callback(void* hw_port, uint32_t state) 
{
    int32_t delta = 0;
    if (state)
    {
        // rising edge
        if (MotorPort_gpio0_get_state(hw_port))
        {
            delta = 1;
        }
        else
        {
            delta = -1;
        }
    }
    else
    {
        // falling edge
        if (MotorPort_gpio0_get_state(hw_port))
        {
            delta = -1;
        }
        else
        {
            delta = 1;
        }
    }

    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_pos_ctrl_data_t data = (p_motor_pos_ctrl_data_t) motport->lib_data;

    data->position += delta;
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

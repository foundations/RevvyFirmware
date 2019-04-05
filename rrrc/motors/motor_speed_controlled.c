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

#include <math.h>

typedef struct {
    PID_t controller;
    TaskHandle_t task;

    float refSpeed;

    int64_t position;
    int64_t lastPosition;
} *p_motor_speed_ctrl_data_t;

static void MOTOR_SPEED_CONTROLLED_Task(void* userData)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) userData;
    p_motor_speed_ctrl_data_t data = (p_motor_speed_ctrl_data_t) motport->lib_data;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        int64_t position;
        CRITICAL_SECTION_ENTER();
        position = data->position;
        CRITICAL_SECTION_LEAVE();

        int32_t measuredSpeed = position - data->lastPosition; // todo more sensible (less arbitrary) unit of speed
        data->lastPosition = position;
        
        jscope_update(0, measuredSpeed);
        jscope_update(1, data->refSpeed);

        int8_t speed = (int8_t) lroundf(pid_update(&data->controller, data->refSpeed, measuredSpeed));
        motport->motor_driver_lib->set_speed(motport, speed);

        vTaskDelay(rtos_ms_to_ticks(20));
	}
}

static int32_t MOTOR_SPEED_CONTROLLED_Init(void* hw_port)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_speed_ctrl_data_t data = (p_motor_speed_ctrl_data_t) motport->lib_data;

    ASSERT(motport && motport->motor_driver_lib);
    motport->motor_driver_lib->init(motport);

    pid_initialize(&data->controller);

    if (xTaskCreate(MOTOR_SPEED_CONTROLLED_Task, "SPD_CTRL", 1024 / sizeof(portSTACK_TYPE), motport, tskIDLE_PRIORITY + 1, &data->task) != pdPASS)
        return ERR_FAILURE;

    return ERR_NONE;
}

static void MOTOR_SPEED_CONTROLLED_DeInit(void* hw_port)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_speed_ctrl_data_t data = (p_motor_speed_ctrl_data_t) motport->lib_data;

	vTaskDelete(data->task);

    motport->motor_driver_lib->deinit(motport);
}

static int32_t MOTOR_SPEED_CONTROLLED_set_config(void* hw_port, const uint8_t* pData, uint32_t size)
{
    if (size != 5 * 4)
    {
        return ERR_INVALID_DATA;
    }

    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_speed_ctrl_data_t data = (p_motor_speed_ctrl_data_t) motport->lib_data;

    CRITICAL_SECTION_ENTER();
    data->controller.config.P = get_float(&pData[0]);
    data->controller.config.I = get_float(&pData[4]);
    data->controller.config.D = get_float(&pData[8]);
    data->controller.config.LowerLimit = get_float(&pData[12]);
    data->controller.config.UpperLimit = get_float(&pData[16]);
    CRITICAL_SECTION_LEAVE();

    return ERR_NONE;
}

static uint32_t MOTOR_SPEED_CONTROLLED_get_config(void* hw_port, uint8_t* data, uint32_t max_size)
{
    return 0;
}

static uint32_t MOTOR_SPEED_CONTROLLED_get_position(void* hw_port, uint8_t* dst, uint32_t max_size)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_speed_ctrl_data_t data = (p_motor_speed_ctrl_data_t) motport->lib_data;
    
    uint32_t src = SwapEndian((uint32_t) (data->position & 0xFFFFFFFFu));
    memcpy(dst, &src, 4);
    
    return 4;
}

static uint32_t MOTOR_SPEED_CONTROLLED_set_control(void* hw_port, int32_t value)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;
    p_motor_speed_ctrl_data_t data = (p_motor_speed_ctrl_data_t) motport->lib_data;

    data->refSpeed = value;

    return 0;
}

static uint32_t MOTOR_SPEED_CONTROLLED_get_control(void* hw_port, int32_t* data, uint32_t max_size)
{
    p_hw_motor_port_t motport = (p_hw_motor_port_t) hw_port;

    if (max_size < 4)
    {
        return 0;
    }

    *data = motport->motor_driver_lib->get_speed(motport);

    return sizeof(int32_t);
}

static void MOTOR_SPEED_CONTROLLED_gpio0_callback(void* hw_port, uint32_t state) 
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
    p_motor_speed_ctrl_data_t data = (p_motor_speed_ctrl_data_t) motport->lib_data;

    data->position += delta;
}

static void MOTOR_SPEED_CONTROLLED_gpio1_callback(void* hw_port, uint32_t state) 
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
    p_motor_speed_ctrl_data_t data = (p_motor_speed_ctrl_data_t) motport->lib_data;

    data->position += delta;
}

motor_lib_entry_t motor_speed_controlled =
{
    .type_id = MOTOR_SPEED_CONTROLLED,
    .name    = "MOTOR_SPEED_CONTROLLED",

    .MotorInit   = &MOTOR_SPEED_CONTROLLED_Init,
    .MotorDeInit = &MOTOR_SPEED_CONTROLLED_DeInit,

    .motor_set_config = &MOTOR_SPEED_CONTROLLED_set_config,
    .motor_get_config = &MOTOR_SPEED_CONTROLLED_get_config,

    .motor_get_position = &MOTOR_SPEED_CONTROLLED_get_position,

    .motor_set_control = &MOTOR_SPEED_CONTROLLED_set_control,
    .motor_get_control = &MOTOR_SPEED_CONTROLLED_get_control,

    .gpio0_callback = &MOTOR_SPEED_CONTROLLED_gpio0_callback,
    .gpio1_callback = &MOTOR_SPEED_CONTROLLED_gpio1_callback
};
/*
 * rrrc_tb6612fng.c
 *
 * Created: 4/3/2019 2:41:54 PM
 *  Author: Daniel Buga
 */ 

#include "rrrc_tb6612fng.h"

#define MOTOR_SPEED_RESOLUTION ((uint8_t) 100u)

const motor_driver_lib_entry_t motor_driver_tb6612fng = 
{
    .init      = &tb6612fng_init,
    .set_speed = &tb6612fng_set_speed,
    .get_speed = &tb6612fng_get_speed,
    .deinit    = &tb6612fng_deinit
};

//*********************************************************************************************
static void tb6612fng_drv_set_dir(p_tb6612fng_t drv, int8_t speed)
{
    if (speed == 0)
    {
        gpio_set_pin_level(drv->dir0_gpio, false);
        gpio_set_pin_level(drv->dir1_gpio, false);
    }
    else if (speed < 0)
    {
        gpio_set_pin_level(drv->dir0_gpio, false);
        gpio_set_pin_level(drv->dir1_gpio, true);
    }
    else
    {
        gpio_set_pin_level(drv->dir0_gpio, true);
        gpio_set_pin_level(drv->dir1_gpio, false);
    }
}

static void tb6612fng_drv_set_speed(p_tb6612fng_t drv, int8_t speed)
{
    tb6612fng_drv_set_dir(drv, speed);

    drv->speed = speed;
    uint8_t duty = (speed < 0) ? -speed : speed;

    if (duty > MOTOR_SPEED_RESOLUTION)
    {
        duty = MOTOR_SPEED_RESOLUTION;
    }

    timer_stop(drv->pwm);
    timer_set_chan_compare_value(drv->pwm, drv->pwm_ch, duty);
    timer_start(drv->pwm);
}

void tb6612fng_one_time_init(hw_motor_port_t* hw_port)
{
    p_tb6612fng_t drv = (p_tb6612fng_t) hw_port->motorDriverConfig;

    // DIR
    gpio_set_pin_pull_mode(drv->dir0_gpio, GPIO_PULL_UP);
    gpio_set_pin_function(drv->dir0_gpio, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(drv->dir0_gpio, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(drv->dir0_gpio, false);

    gpio_set_pin_pull_mode(drv->dir1_gpio, GPIO_PULL_UP);
    gpio_set_pin_function(drv->dir1_gpio, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(drv->dir1_gpio, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(drv->dir1_gpio, false);

    // PWM
    gpio_set_pin_direction(drv->pwm_pin, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(drv->pwm_pin, GPIO_PIN_FUNCTION_F);

    // ENC
}

void tb6612fng_init(hw_motor_port_t* hw_port)
{
    p_tb6612fng_t drv = (p_tb6612fng_t) hw_port->motorDriverConfig;

    // start pwm timer
    timer_set_clock_cycles_per_tick(drv->pwm, MOTOR_SPEED_RESOLUTION);
    tb6612fng_drv_set_speed(drv, 0);
}

void tb6612fng_set_speed(hw_motor_port_t* hw_port, int8_t speed)
{
    p_tb6612fng_t drv = (p_tb6612fng_t) hw_port->motorDriverConfig;

    tb6612fng_drv_set_speed(drv, speed);
}

int8_t tb6612fng_get_speed(hw_motor_port_t* hw_port)
{
    p_tb6612fng_t drv = (p_tb6612fng_t) hw_port->motorDriverConfig;

    return drv->speed;
}

void tb6612fng_deinit(hw_motor_port_t* hw_port)
{
    p_tb6612fng_t drv = (p_tb6612fng_t) hw_port->motorDriverConfig;

    // stop pwm timer
    timer_stop(drv->pwm);
}

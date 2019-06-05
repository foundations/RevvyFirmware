/*
 * MotorDriver_TB661FNG.c
 *
 * Created: 10/05/2019 14:42:47
 *  Author: Dániel Buga
 */ 

#include "MotorDriver_TB661FNG.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_tc_base.h>
#include <hpl_tcc.h>
#include "atmel_start_pins.h"

static struct timer_descriptor TIMER_TCC0;
#define MOTOR_SPEED_RESOLUTION ((uint8_t) 100u)

static void tb6612fng_drv_init(TB6612FNG_Channel_t* drv);
static void tb6612fng_drv_set_dir(TB6612FNG_Channel_t* drv, int8_t speed);
static void tb6612fng_drv_set_speed(TB6612FNG_Channel_t* drv, int8_t speed);

static void tb6612fng_drv_init(TB6612FNG_Channel_t* drv)
{
    drv->control_timer = &TIMER_TCC0;

    gpio_set_pin_pull_mode(drv->dir0, GPIO_PULL_UP);
    gpio_set_pin_function(drv->dir0, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(drv->dir0, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(drv->dir0, false);

    gpio_set_pin_pull_mode(drv->dir1, GPIO_PULL_UP);
    gpio_set_pin_function(drv->dir1, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(drv->dir1, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(drv->dir1, false);

    gpio_set_pin_direction(drv->pwm_pin, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(drv->pwm_pin, GPIO_PIN_FUNCTION_F);

    tb6612fng_drv_set_dir(drv, 0);
}

static void tb6612fng_drv_set_dir(TB6612FNG_Channel_t* drv, int8_t speed)
{
    if (speed == 0)
    {
        gpio_set_pin_level(drv->dir0, false);
        gpio_set_pin_level(drv->dir1, false);
    }
    else if (speed < 0)
    {
        gpio_set_pin_level(drv->dir0, true);
        gpio_set_pin_level(drv->dir1, false);
    }
    else
    {
        gpio_set_pin_level(drv->dir0, false);
        gpio_set_pin_level(drv->dir1, true);
    }
}

static void tb6612fng_drv_set_speed(TB6612FNG_Channel_t* drv, int8_t speed)
{
    tb6612fng_drv_set_dir(drv, speed);

    uint8_t duty = (speed < 0) ? -speed : speed;

    if (duty > MOTOR_SPEED_RESOLUTION)
    {
        duty = MOTOR_SPEED_RESOLUTION;
    }

    timer_stop(drv->control_timer);
    timer_set_chan_compare_value(drv->control_timer, drv->pwm_channel, duty);
    timer_start(drv->control_timer);
}

void MotorDriver_TB661FNG_Run_OnInit(void)
{
    hri_mclk_set_APBBMASK_TCC0_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TCC0_GCLK_ID, CONF_GCLK_TCC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    
    timer_init(&TIMER_TCC0, TCC0, _tcc_get_timer());
    timer_set_clock_cycles_per_tick(&TIMER_TCC0, MOTOR_SPEED_RESOLUTION);

    gpio_set_pin_pull_mode(MOTPWEN, GPIO_PULL_UP);
    gpio_set_pin_function(MOTPWEN, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(MOTPWEN, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(MOTPWEN, true);
}

void MotorDriver_TB661FNG_Run_OnDriverInit(TB6612FNG_t* driver)
{
    tb6612fng_drv_init(&driver->channel_a);
    tb6612fng_drv_init(&driver->channel_b);

    gpio_set_pin_pull_mode(driver->standby, GPIO_PULL_UP);
    gpio_set_pin_function(driver->standby, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(driver->standby, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(driver->standby, true);
}

void MotorDriver_TB661FNG_Run_Update(TB6612FNG_t* driver)
{
    int8_t speed_a = MotorDriver_TB661FNG_Read_DriveValue_ChannelA(driver);
    tb6612fng_drv_set_speed(&driver->channel_a, speed_a);

    int8_t speed_b = MotorDriver_TB661FNG_Read_DriveValue_ChannelB(driver);
    tb6612fng_drv_set_speed(&driver->channel_b, speed_b);
}

__attribute__((weak))
int8_t MotorDriver_TB661FNG_Read_DriveValue_ChannelA(TB6612FNG_t* driver)
{
    return 0;
}

__attribute__((weak))
int8_t MotorDriver_TB661FNG_Read_DriveValue_ChannelB(TB6612FNG_t* driver)
{
    return 0;
}

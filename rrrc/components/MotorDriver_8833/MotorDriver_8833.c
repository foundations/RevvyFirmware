/*
 * MotorDriver_8833.c
 *
 * Created: 2019. 07. 22. 22:44:38
 *  Author: D�niel Buga
 */

#include "MotorDriver_8833.h"

#include <peripheral_clk_config.h>
#include <hal_init.h>
#include <tc_lite.h>
#include <hal_gpio.h>
#include "atmel_start_pins.h"
#include "utils/functions.h"

#define MOTOR_SPEED_RESOLUTION (100)

static Tc* const timers[6] = {
    TC0,
    TC1,
    TC2,
    TC3,
    TC4,
    TC5
};

void MotorDriver_8833_Run_OnGlobalInit(void)
{
    hri_mclk_set_APBAMASK_TC0_bit(MCLK);
    hri_mclk_set_APBAMASK_TC1_bit(MCLK);
    hri_mclk_set_APBBMASK_TC2_bit(MCLK);
    hri_mclk_set_APBBMASK_TC3_bit(MCLK);
    hri_mclk_set_APBCMASK_TC4_bit(MCLK);
    hri_mclk_set_APBCMASK_TC5_bit(MCLK);
    
    hri_gclk_write_PCHCTRL_reg(GCLK, TC0_GCLK_ID, CONF_GCLK_TC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, TC1_GCLK_ID, CONF_GCLK_TC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, TC2_GCLK_ID, CONF_GCLK_TC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, TC3_GCLK_ID, CONF_GCLK_TC3_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, TC4_GCLK_ID, CONF_GCLK_TC4_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, TC5_GCLK_ID, CONF_GCLK_TC5_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    
    TIMER_0_init();
    TIMER_1_init();
    TIMER_2_init();
    TIMER_3_init();
    TIMER_4_init();
    TIMER_5_init();
}

static void drv8833_set_speed(MotorDriver_8833_Channel_t* channel, const int8_t speed)
{
    if (speed != channel->prev_speed)
    {
        channel->prev_speed = speed;

        /*
         * - if speed is negative, pwm_1 holds the absolute value, otherwise pwm_0
         * - subtraction from max is because we want to drive motors using slow decay
         */
        const uint8_t pwm_0 = MOTOR_SPEED_RESOLUTION - constrain_int8(speed, 0, MOTOR_SPEED_RESOLUTION);
        const uint8_t pwm_1 = MOTOR_SPEED_RESOLUTION + constrain_int8(speed, -MOTOR_SPEED_RESOLUTION, 0); // constrain first to avoid -128 overflowing

        hri_tccount8_write_CC_reg(timers[channel->timer], channel->ch1, pwm_0);
        hri_tccount8_write_CC_reg(timers[channel->timer], channel->ch2, pwm_1);

        /* restart counting to avoid unwanted long ON pulses */
        hri_tc_set_CTRLB_CMD_bf(timers[channel->timer], TC_CTRLBSET_CMD_RETRIGGER_Val);
    }
}

/**
 * Set timer period, enable and start counting
 */
static void enable_timer(const uint8_t timer)
{
    hri_tccount8_write_PER_reg(timers[timer], MOTOR_SPEED_RESOLUTION - 1u);

    hri_tc_set_CTRLA_ENABLE_bit(timers[timer]);
    hri_tc_set_CTRLB_CMD_bf(timers[timer], TC_CTRLBSET_CMD_RETRIGGER_Val);
}

/**
 * Configure the given pin as a TC waveform output pin (function E)
 */
static void configure_wo_pin(const uint8_t pin)
{
    gpio_set_pin_direction(pin, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(pin, GPIO_PIN_FUNCTION_E);
}

void MotorDriver_8833_Run_OnInit(MotorDriver_8833_t* driver)
{
    configure_wo_pin(driver->pwm_a.pin1);
    configure_wo_pin(driver->pwm_a.pin2);
    configure_wo_pin(driver->pwm_b.pin1);
    configure_wo_pin(driver->pwm_b.pin2);

    gpio_set_pin_direction(driver->fault, GPIO_DIRECTION_IN);
    gpio_set_pin_function(driver->fault, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_pull_mode(driver->fault, GPIO_PULL_OFF);

    gpio_set_pin_direction(driver->n_sleep, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(driver->n_sleep, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_pull_mode(driver->n_sleep, GPIO_PULL_OFF);

    gpio_set_pin_level(driver->n_sleep, false);

    enable_timer(driver->pwm_a.timer);
    enable_timer(driver->pwm_b.timer);
    
    /* set a random speed to force updating the counters */
    driver->pwm_a.prev_speed = 1;
    driver->pwm_b.prev_speed = 1;
    
    drv8833_set_speed(&driver->pwm_a, 0);
    drv8833_set_speed(&driver->pwm_b, 0);
}

void MotorDriver_8833_Run_OnUpdate(MotorDriver_8833_t* driver)
{
    if (!driver->has_fault)
    {
        driver->has_fault = gpio_get_pin_level(driver->fault) == false;

        if (driver->has_fault)
        {
            MotorDriver_8833_Call_OnFault(driver);
        }
    }
    
    if (driver->has_fault)
    {
        /* false = sleep mode active */
        gpio_set_pin_level(driver->n_sleep, false);

        drv8833_set_speed(&driver->pwm_a, 0);
        drv8833_set_speed(&driver->pwm_b, 0);
    }
    else
    {
        const int8_t speed_a = MotorDriver_8833_Read_DriveRequest_ChannelA(driver);
        const int8_t speed_b = MotorDriver_8833_Read_DriveRequest_ChannelB(driver);

        /* set sleep mode if not driven (false = sleep mode active) */
        gpio_set_pin_level(driver->n_sleep, speed_a != 0 || speed_b != 0);

        drv8833_set_speed(&driver->pwm_a, speed_a);
        drv8833_set_speed(&driver->pwm_b, speed_b);
    }
}

void MotorDriver_8833_Run_FaultCleared(MotorDriver_8833_t* driver)
{
    driver->has_fault = false;
}

__attribute__((weak))
void MotorDriver_8833_Call_OnFault(MotorDriver_8833_t* driver)
{
    MotorDriver_8833_Run_FaultCleared(driver);
}

__attribute__((weak))
int8_t MotorDriver_8833_Read_DriveRequest_ChannelA(MotorDriver_8833_t* driver)
{
    (void) driver;

    return 0;
}

__attribute__((weak))
int8_t MotorDriver_8833_Read_DriveRequest_ChannelB(MotorDriver_8833_t* driver)
{
    (void) driver;

    return 0;
}

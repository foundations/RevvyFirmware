/*
 * MotorDriver_8833.c
 *
 * Created: 2019. 07. 22. 22:44:38
 *  Author: Dániel Buga
 */

#include "MotorDriver_8833.h"

#include <peripheral_clk_config.h>
#include <hal_init.h>
#include <tc_lite.h>
#include <hal_gpio.h>
#include "atmel_start_pins.h"

#define MOTOR_SPEED_RESOLUTION (100)

static Tc* timers[6];

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

    hri_tccount8_write_PER_reg(TC0, MOTOR_SPEED_RESOLUTION - 1u);
    hri_tccount8_write_PER_reg(TC1, MOTOR_SPEED_RESOLUTION - 1u);
    hri_tccount8_write_PER_reg(TC2, MOTOR_SPEED_RESOLUTION - 1u);
    hri_tccount8_write_PER_reg(TC3, MOTOR_SPEED_RESOLUTION - 1u);
    hri_tccount8_write_PER_reg(TC4, MOTOR_SPEED_RESOLUTION - 1u);
    hri_tccount8_write_PER_reg(TC5, MOTOR_SPEED_RESOLUTION - 1u);
    
    timers[0] = TC0;
    timers[1] = TC1;
    timers[2] = TC2;
    timers[3] = TC3;
    timers[4] = TC4;
    timers[5] = TC5;
}

static void drv8833_set_speed(MotorDriver_8833_t* driver, MotorDriver_8833_Channel_t channel, int8_t speed)
{    
    uint32_t ch0;
    uint32_t ch1;
    
    uint8_t timer0;
    uint8_t timer1;

    bool reverse = speed < 0;
    speed = reverse ? -speed : speed;
    if (speed > MOTOR_SPEED_RESOLUTION)
    {
        speed = MOTOR_SPEED_RESOLUTION;
    }
    
    uint8_t pwm_0 = 0u;
    uint8_t pwm_1 = 0u;

    if (reverse)
    {
        pwm_1 = speed;
    }
    else
    {
        pwm_0 = speed;
    }

    if (channel == MotorDriver_8833_Channel_A)
    {
        ch0 = driver->pwm_a1_ch;
        ch1 = driver->pwm_a2_ch;
        
        timer0 = driver->pwm_a1_timer;
        timer1 = driver->pwm_a2_timer;

        driver->speed_a = speed;
    }
    else
    {
        ch0 = driver->pwm_b1_ch;
        ch1 = driver->pwm_b2_ch;
        
        timer0 = driver->pwm_b1_timer;
        timer1 = driver->pwm_b2_timer;
        
        driver->speed_b = speed;
    }
    
    /* subtraction is because we want to drive motors using slow decay */
    hri_tccount8_write_CC_reg(timers[timer0], ch0, MOTOR_SPEED_RESOLUTION - pwm_0);
    hri_tccount8_write_CC_reg(timers[timer1], ch1, MOTOR_SPEED_RESOLUTION - pwm_1);
}

void MotorDriver_8833_Run_OnInit(MotorDriver_8833_t* driver)
{
    gpio_set_pin_direction(driver->pwm_a1, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(driver->pwm_a1, GPIO_PIN_FUNCTION_E);
    
    gpio_set_pin_direction(driver->pwm_a2, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(driver->pwm_a2, GPIO_PIN_FUNCTION_E);
    
    gpio_set_pin_direction(driver->pwm_b1, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(driver->pwm_b1, GPIO_PIN_FUNCTION_E);
    
    gpio_set_pin_direction(driver->pwm_b2, GPIO_DIRECTION_OFF);
    gpio_set_pin_function(driver->pwm_b2, GPIO_PIN_FUNCTION_E);

    gpio_set_pin_function(driver->fault, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(driver->fault, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(driver->fault, GPIO_PULL_OFF);

    gpio_set_pin_function(driver->n_sleep, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(driver->n_sleep, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(driver->n_sleep, GPIO_PULL_OFF);
    gpio_set_pin_level(driver->n_sleep, false);
    
    hri_tc_set_CTRLA_ENABLE_bit(timers[driver->pwm_a1_timer]);
    hri_tc_set_CTRLB_CMD_bf(timers[driver->pwm_a1_timer], TC_CTRLBSET_CMD_RETRIGGER_Val);
    hri_tc_set_CTRLA_ENABLE_bit(timers[driver->pwm_a2_timer]);
    hri_tc_set_CTRLB_CMD_bf(timers[driver->pwm_a2_timer], TC_CTRLBSET_CMD_RETRIGGER_Val);
    
    hri_tc_set_CTRLA_ENABLE_bit(timers[driver->pwm_b1_timer]);
    hri_tc_set_CTRLB_CMD_bf(timers[driver->pwm_b1_timer], TC_CTRLBSET_CMD_RETRIGGER_Val);
    hri_tc_set_CTRLA_ENABLE_bit(timers[driver->pwm_b2_timer]);
    hri_tc_set_CTRLB_CMD_bf(timers[driver->pwm_b2_timer], TC_CTRLBSET_CMD_RETRIGGER_Val);
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
        gpio_set_pin_level(driver->n_sleep, false);

        drv8833_set_speed(driver, MotorDriver_8833_Channel_A, 0);
        drv8833_set_speed(driver, MotorDriver_8833_Channel_B, 0);
    }
    else
    {
        int8_t speed_a = MotorDriver_8833_Read_DriveRequest(driver, MotorDriver_8833_Channel_A);
        int8_t speed_b = MotorDriver_8833_Read_DriveRequest(driver, MotorDriver_8833_Channel_B);
    
        gpio_set_pin_level(driver->n_sleep, !(speed_a == 0 && speed_b == 0));

        drv8833_set_speed(driver, MotorDriver_8833_Channel_A, speed_a);
        drv8833_set_speed(driver, MotorDriver_8833_Channel_B, speed_b);
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
int8_t MotorDriver_8833_Read_DriveRequest(MotorDriver_8833_t* driver, MotorDriver_8833_Channel_t channel)
{
    (void) driver;
    (void) channel;

    return 0;
}

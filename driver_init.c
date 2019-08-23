/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

#include "rrrc_hal.h"

struct _timer_device tc6_device;

struct i2c_m_async_desc I2C_0;
struct i2c_m_async_desc I2C_1;
struct i2c_m_async_desc I2C_2;
struct i2c_m_async_desc I2C_3;

//*********************************************************************************************
void I2C_0_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_SLOW, CONF_GCLK_SERCOM3_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBBMASK_SERCOM3_bit(MCLK);

    gpio_set_pin_pull_mode(I2C0_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C0_SDApin, I2C0_SDApin_function);
    gpio_set_pin_pull_mode(I2C0_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C0_SCLpin, I2C0_SCLpin_function);
}

//*********************************************************************************************
void I2C_1_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_SLOW, CONF_GCLK_SERCOM1_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBAMASK_SERCOM1_bit(MCLK);

    gpio_set_pin_pull_mode(I2C1_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_pull_mode(I2C1_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C1_SCLpin, I2C1_SCLpin_function);
    gpio_set_pin_function(I2C1_SDApin, I2C1_SDApin_function);
}

//*********************************************************************************************
void I2C_2_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM0_GCLK_ID_SLOW, CONF_GCLK_SERCOM0_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBAMASK_SERCOM0_bit(MCLK);

    gpio_set_pin_pull_mode(I2C2_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C2_SDApin, I2C0_SDApin_function);
    gpio_set_pin_pull_mode(I2C2_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C2_SCLpin, I2C0_SCLpin_function);
}

//*********************************************************************************************
void I2C_3_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM6_GCLK_ID_CORE, CONF_GCLK_SERCOM6_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM6_GCLK_ID_SLOW, CONF_GCLK_SERCOM6_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBDMASK_SERCOM6_bit(MCLK);

    gpio_set_pin_pull_mode(I2C3_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C3_SDApin, I2C3_SDApin_function);
    gpio_set_pin_pull_mode(I2C3_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C3_SCLpin, I2C3_SCLpin_function);
}

//*********************************************************************************************
void EXTERNAL_IRQ_0_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID, CONF_GCLK_EIC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBAMASK_EIC_bit(MCLK);

    ext_irq_init();
}

//*********************************************************************************************
void delay_driver_init(void)
{
    delay_init(SysTick);
}

//*********************************************************************************************
static void TIMER_TC6_init(void)
{
    hri_mclk_set_APBDMASK_TC6_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC6_GCLK_ID, CONF_GCLK_TC6_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    _tc_timer_init(&tc6_device, TC6);
}

static void IT_init(void)
{
    /* Set everything to 3, interrupts that access FreeRTOS API must not be at priority 0 */
    for (uint8_t i = 0; i < 138; i++)
    {
        NVIC_SetPriority(i, 3);
    }

    /* RPi I2C has highest possible prio
    Datasheet: The integer number specified in the source refers to the respective bit position in the INTFLAG register of respective peripheral.

    Sercom 2 interrupt mapping:
    Source | Line | Bit (Slave) | Bit (Master)
        0  | 54   | PREC        | MB
        1  | 55   | AMATCH      | SB
        2  | 56   | DRDY        | -
        3  | 57   | -           | -
        4  | 57   | -           | -
        5  | 57   | -           | -
        7  | 57   | Error       | ERROR
    * only STOP (PREC) accesses FreeRTOS API, set others to 0 */
    NVIC_SetPriority(SERCOM2_0_IRQn, 1);
    NVIC_SetPriority(SERCOM2_1_IRQn, 0);
    NVIC_SetPriority(SERCOM2_2_IRQn, 0);
    NVIC_SetPriority(SERCOM2_3_IRQn, 0);
}

//*********************************************************************************************
void system_init(void)
{
    init_mcu();

    hri_mclk_set_APBAMASK_SUPC_bit(MCLK);
    hri_supc_write_VREF_SEL_bf(SUPC, SUPC_VREF_SEL_2V5_Val);

    /* ondemand mode so the proper temp channel will always be selected */
    hri_supc_set_VREF_ONDEMAND_bit(SUPC);
    hri_supc_set_VREF_TSEN_bit(SUPC);
    
    I2C_0_init();
    I2C_1_init();
    I2C_2_init();
    I2C_3_init();

    IT_init();

    EXTERNAL_IRQ_0_init();

    delay_driver_init();

    TIMER_TC6_init();

    high_res_timer_init(&tc6_device);
}

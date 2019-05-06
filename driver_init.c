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

#include <hpl_rtc_base.h>



struct timer_descriptor TIMER_RTC;
struct timer_descriptor TIMER_TC0;
struct timer_descriptor TIMER_TC1;
struct timer_descriptor TIMER_TC2;
struct timer_descriptor TIMER_TC3;
struct timer_descriptor TIMER_TC4;
struct timer_descriptor TIMER_TC5;
struct timer_descriptor TIMER_TC6;
struct timer_descriptor TIMER_TC7;

struct timer_descriptor TIMER_TCC0;
struct timer_descriptor TIMER_TCC1;
struct timer_descriptor TIMER_TCC2;
struct timer_descriptor TIMER_TCC3;
struct timer_descriptor TIMER_TCC4;

uint8_t                       SERCOM2_i2c_s_buffer[SERCOM2_I2CS_BUFFER_SIZE];
// struct i2c_m_async_desc I2C_1;
// struct i2c_m_async_desc I2C_2;
// struct i2c_m_async_desc I2C_3;
// struct i2c_m_async_desc I2C_4;

struct i2c_m_sync_desc        I2C_1;
struct i2c_m_sync_desc        I2C_2;
struct i2c_m_sync_desc        I2C_3;
struct i2c_m_sync_desc        I2C_4;

//struct spi_m_sync_descriptor SPI_0;

struct wdt_descriptor WDT_0;

//*********************************************************************************************
void EXTERNAL_IRQ_0_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, EIC_GCLK_ID, CONF_GCLK_EIC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBAMASK_EIC_bit(MCLK);

    ext_irq_init();
}

//*********************************************************************************************
static void TIMER_RTC_init(void)
{
    hri_mclk_set_APBAMASK_RTC_bit(MCLK);
    timer_init(&TIMER_RTC, RTC, _rtc_get_timer());
}

//*********************************************************************************************
void I2C_1_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_SLOW, CONF_GCLK_SERCOM1_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBAMASK_SERCOM1_bit(MCLK);

    i2c_m_sync_init(&I2C_1, I2C1_SERCOM);

    gpio_set_pin_pull_mode(I2C1_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_pull_mode(I2C1_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C1_SCLpin, I2C1_SCLpin_function);
    gpio_set_pin_function(I2C1_SDApin, I2C1_SDApin_function);
}

//*********************************************************************************************
void I2C_2_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM6_GCLK_ID_CORE, CONF_GCLK_SERCOM6_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM6_GCLK_ID_SLOW, CONF_GCLK_SERCOM6_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBDMASK_SERCOM6_bit(MCLK);

    i2c_m_sync_init(&I2C_2, I2C2_SERCOM);

    gpio_set_pin_pull_mode(I2C2_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C2_SDApin, I2C0_SDApin_function);
    gpio_set_pin_pull_mode(I2C2_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C2_SCLpin, I2C0_SCLpin_function);
}

//*********************************************************************************************
void I2C_3_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM3_GCLK_ID_SLOW, CONF_GCLK_SERCOM3_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBBMASK_SERCOM3_bit(MCLK);

    i2c_m_sync_init(&I2C_3, I2C3_SERCOM);

    gpio_set_pin_pull_mode(I2C3_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C3_SDApin, I2C3_SDApin_function);
    gpio_set_pin_pull_mode(I2C3_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C3_SCLpin, I2C3_SCLpin_function);
}

//*********************************************************************************************
void I2C_4_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM0_GCLK_ID_SLOW, CONF_GCLK_SERCOM0_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBAMASK_SERCOM0_bit(MCLK);

    i2c_m_sync_init(&I2C_4, I2C4_SERCOM);

    gpio_set_pin_pull_mode(I2C4_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C4_SDApin, I2C4_SDApin_function);
    gpio_set_pin_pull_mode(I2C4_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C4_SCLpin, I2C4_SCLpin_function);
}

//*********************************************************************************************
void delay_driver_init(void)
{
    delay_init(SysTick);
}

//*********************************************************************************************
static void TIMER_TC0_init(void)
{
    hri_mclk_set_APBAMASK_TC0_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC0_GCLK_ID, CONF_GCLK_TC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    timer_init(&TIMER_TC0, TC0, _tc_get_timer());
}

//*********************************************************************************************
static void TIMER_TC1_init(void)
{
    hri_mclk_set_APBAMASK_TC1_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC1_GCLK_ID, CONF_GCLK_TC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    timer_init(&TIMER_TC1, TC1, _tc_get_timer());
}

//*********************************************************************************************
static void TIMER_TC2_init(void)
{
    hri_mclk_set_APBBMASK_TC2_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC2_GCLK_ID, CONF_GCLK_TC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    timer_init(&TIMER_TC2, TC2, _tc_get_timer());
}

//*********************************************************************************************
static void TIMER_TC3_init(void)
{
    hri_mclk_set_APBBMASK_TC3_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC3_GCLK_ID, CONF_GCLK_TC3_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    timer_init(&TIMER_TC3, TC3, _tc_get_timer());
}

//*********************************************************************************************
static void TIMER_TC4_init(void)
{
    hri_mclk_set_APBCMASK_TC4_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC4_GCLK_ID, CONF_GCLK_TC4_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    timer_init(&TIMER_TC4, TC4, _tc_get_timer());
}

//*********************************************************************************************
static void TIMER_TC5_init(void)
{
    hri_mclk_set_APBCMASK_TC5_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC5_GCLK_ID, CONF_GCLK_TC5_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    timer_init(&TIMER_TC5, TC5, _tc_get_timer());
}

//*********************************************************************************************
static void TIMER_TC6_init(void)
{
    hri_mclk_set_APBDMASK_TC6_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC6_GCLK_ID, CONF_GCLK_TC6_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    timer_init(&TIMER_TC6, TC6, _tc_get_timer());    
}

//*********************************************************************************************
static void TIMER_TC7_init(void)
{
    hri_mclk_set_APBDMASK_TC7_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC7_GCLK_ID, CONF_GCLK_TC7_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    timer_init(&TIMER_TC7, TC7, _tc_get_timer());
}

//*********************************************************************************************
static void TIMER_TCC0_init(void)
{
    hri_mclk_set_APBBMASK_TCC0_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TCC0_GCLK_ID, CONF_GCLK_TCC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    
    timer_init(&TIMER_TCC0, TCC0, _tcc_get_timer());
}

//*********************************************************************************************
static void TIMER_TCC1_init(void)
{
    hri_mclk_set_APBBMASK_TCC1_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TCC1_GCLK_ID, CONF_GCLK_TCC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    
    timer_init(&TIMER_TCC1, TCC1, _tcc_get_timer());
}

//*********************************************************************************************
static void TIMER_TCC2_init(void)
{
    hri_mclk_set_APBCMASK_TCC2_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TCC2_GCLK_ID, CONF_GCLK_TCC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    
    timer_init(&TIMER_TCC2, TCC2, _tcc_get_timer());
}

//*********************************************************************************************
static void TIMER_TCC3_init(void)
{
    hri_mclk_set_APBCMASK_TCC3_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TCC3_GCLK_ID, CONF_GCLK_TCC3_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    
    timer_init(&TIMER_TCC3, TCC3, _tcc_get_timer());
}

//*********************************************************************************************
static void TIMER_TCC4_init(void)
{
    hri_mclk_set_APBDMASK_TCC4_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TCC4_GCLK_ID, CONF_GCLK_TCC4_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    
    timer_init(&TIMER_TCC4, TCC4, _tcc_get_timer());
}

//*********************************************************************************************
void WDT_0_init(void)
{
    hri_mclk_set_APBAMASK_WDT_bit(MCLK);
    wdt_init(&WDT_0, WDT);
}

static void IT_init(void)
{
    // Set everything to 1, interrupts must not be at priority 0
    for (uint8_t i = 0; i < 138; i++)
    {
        NVIC_SetPriority(i, 3);
    }

    NVIC_SetPriority(SERCOM2_0_IRQn, 1);
    NVIC_SetPriority(SERCOM2_1_IRQn, 1);
    NVIC_SetPriority(SERCOM2_2_IRQn, 1);
    NVIC_SetPriority(SERCOM2_3_IRQn, 1);
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

    IT_init();

    EXTERNAL_IRQ_0_init();

    TIMER_RTC_init();
    
    I2C_1_init();

    I2C_2_init();

    I2C_3_init();
    
    I2C_4_init();

    delay_driver_init();

    TIMER_TC0_init();
    TIMER_TC1_init();
    TIMER_TC2_init();
    TIMER_TC3_init();
    TIMER_TC4_init();
    TIMER_TC5_init();
     TIMER_TC6_init();
     TIMER_TC7_init();
     TIMER_TCC0_init();
//     TIMER_TCC1_init();
//     TIMER_TCC2_init();

    //WDT_0_init();

    high_res_timer_init(&TIMER_TC2);
}

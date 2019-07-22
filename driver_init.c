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

#include "rrrc_hal.h"

struct timer_descriptor TIMER_RTC;
struct timer_descriptor TIMER_TC0;
struct timer_descriptor TIMER_TC1;
struct timer_descriptor TIMER_TC2;
struct timer_descriptor TIMER_TC3;
struct timer_descriptor TIMER_TC4;
struct timer_descriptor TIMER_TC5;
struct timer_descriptor TIMER_TC6;
struct timer_descriptor TIMER_TC7;

struct timer_descriptor TIMER_TCC1;
struct timer_descriptor TIMER_TCC2;
struct timer_descriptor TIMER_TCC3;
struct timer_descriptor TIMER_TCC4;

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

    IT_init();

    EXTERNAL_IRQ_0_init();

    TIMER_RTC_init();

    delay_driver_init();

    TIMER_TC0_init();
    TIMER_TC1_init();
    TIMER_TC2_init();
    TIMER_TC3_init();
    TIMER_TC4_init();
    TIMER_TC5_init();
    TIMER_TC6_init();
    TIMER_TC7_init();

    high_res_timer_init(&TIMER_TC2);
}

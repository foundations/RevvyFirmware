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

struct timer_descriptor TIMER_TC6;

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

    timer_init(&TIMER_TC6, TC6, _tc_get_timer());    
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

    delay_driver_init();

    TIMER_TC6_init();

    high_res_timer_init(&TIMER_TC6);
}

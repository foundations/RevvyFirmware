/*
 * HighResolutionTimer.c
 *
 * Created: 2019. 08. 23. 19:37:27
 *  Author: Dániel Buga
 */ 

#include "HighResolutionTimer.h"

#include "driver_init.h"
#include <peripheral_clk_config.h>

void HighResolutionTimer_Run_OnInit(void)
{
    hri_mclk_set_APBDMASK_TC6_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC6_GCLK_ID, CONF_GCLK_TC6_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    struct _timer_device tc6_device;

    _tc_timer_init(&tc6_device, TC6);
    _tc_timer_start(&tc6_device);
}

uint16_t HighResolutionTimer_Run_GetTickCount(void)
{
    hri_tc_set_CTRLB_CMD_bf(TC6, TC_CTRLBSET_CMD_READSYNC_Val);
    while (hri_tc_read_CTRLB_CMD_bf(TC6) != 0u);

    return hri_tccount16_get_COUNT_COUNT_bf(TC6, 0xFFFFu);
}

uint16_t HighResolutionTimer_Constant_TicksInMs(void)
{
    return 24u;
}

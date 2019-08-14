/*
 * worklogic.c
 *
 * Created: 1/4/2019 6:56:55 PM
 *  Author: User
 */

#include "rrrc_hal.h"

//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************

static struct timer_descriptor* high_res_timer;
void high_res_timer_init(struct timer_descriptor* timer)
{
    high_res_timer = timer;

    timer_start(timer);
}

uint16_t high_res_timer_get_count(void)
{
    hri_tc_set_CTRLB_CMD_bf(high_res_timer->device.hw, TC_CTRLBSET_CMD_READSYNC_Val);
    while (hri_tc_read_CTRLB_CMD_bf(high_res_timer->device.hw) != 0);

    return hri_tccount16_get_COUNT_COUNT_bf(high_res_timer->device.hw, 0xFFFF);
}

uint16_t high_res_timer_ticks_per_ms(void)
{
    return 24u;
}

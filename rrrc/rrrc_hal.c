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

static struct _timer_device* high_res_timer;
void high_res_timer_init(struct _timer_device* timer)
{
    high_res_timer = timer;

    _tc_timer_start(timer);
}

uint16_t high_res_timer_get_count(void)
{
    hri_tc_set_CTRLB_CMD_bf(high_res_timer->hw, TC_CTRLBSET_CMD_READSYNC_Val);
    while (hri_tc_read_CTRLB_CMD_bf(high_res_timer->hw) != 0);

    return hri_tccount16_get_COUNT_COUNT_bf(high_res_timer->hw, 0xFFFF);
}

uint16_t high_res_timer_ticks_per_ms(void)
{
    return 24u;
}

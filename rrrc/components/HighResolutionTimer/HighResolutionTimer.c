#include "utils_assert.h"
#include "HighResolutionTimer.h"
#include "utils.h"

/* Begin User Code Section: Declarations */
#include "driver_init.h"
#include <peripheral_clk_config.h>
/* End User Code Section: Declarations */

void HighResolutionTimer_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    hri_mclk_set_APBDMASK_TC6_bit(MCLK);
    hri_gclk_write_PCHCTRL_reg(GCLK, TC6_GCLK_ID, CONF_GCLK_TC6_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

    struct _timer_device tc6_device;

    _tc_timer_init(&tc6_device, TC6);
    _tc_timer_start(&tc6_device);
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

uint16_t HighResolutionTimer_Run_GetTickCount(void)
{
    /* Begin User Code Section: GetTickCount Start */
    hri_tc_set_CTRLB_CMD_bf(TC6, TC_CTRLBSET_CMD_READSYNC_Val);
    while (hri_tc_read_CTRLB_CMD_bf(TC6) != 0u);

    return hri_tccount16_get_COUNT_COUNT_bf(TC6, 0xFFFFu);
    /* End User Code Section: GetTickCount Start */
    /* Begin User Code Section: GetTickCount End */

    /* End User Code Section: GetTickCount End */
}

float HighResolutionTimer_Run_ToMs(uint32_t ticks)
{
    /* Begin User Code Section: ToMs Start */
    return ticks / 24.0f;
    /* End User Code Section: ToMs Start */
    /* Begin User Code Section: ToMs End */

    /* End User Code Section: ToMs End */
}

uint16_t HighResolutionTimer_Constant_TicksInMs(void)
{
    /* Begin User Code Section: TicksInMs Start */

    /* End User Code Section: TicksInMs Start */
    /* Begin User Code Section: TicksInMs End */

    /* End User Code Section: TicksInMs End */
    return 24u;
}

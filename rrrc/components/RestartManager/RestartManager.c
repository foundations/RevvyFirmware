/*
 * RestartManager.c
 *
 * Created: 2019. 07. 15. 15:13:16
 *  Author: Dániel Buga
 */ 

#include "RestartManager.h"

#include "rrrc_hal.h"

static const void * s_rtc_module = (const void *) RTC;

void RestartManager_Run_Reset(void)
{
    NVIC_SystemReset();
}

void RestartManager_Run_RebootToBootloader(void)
{
    hri_rtcmode0_set_CTRLB_GP0EN_bit(s_rtc_module);
    hri_rtcmode0_set_CTRLB_GP2EN_bit(s_rtc_module);

    hri_rtc_write_GP_reg(s_rtc_module, 0u, (hri_rtc_gp_reg_t) 0xFFFFFFFFu);
    hri_rtc_write_GP_reg(s_rtc_module, 1u, (hri_rtc_gp_reg_t) 0xFFFFFFFFu);
    hri_rtc_write_GP_reg(s_rtc_module, 2u, (hri_rtc_gp_reg_t) 0xFFFFFFFFu);
    hri_rtc_write_GP_reg(s_rtc_module, 3u, (hri_rtc_gp_reg_t) 0xFFFFFFFFu);
    
    NVIC_SystemReset();
}

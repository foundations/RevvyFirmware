
/**
 * \file
 *
 * \brief TC related functionality implementation.
 *
 * Copyright (c) 2017 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include "tc_lite.h"

/* setup function for NPWM timers */
#define TIMER_INIT_FN(idx)                                               \
int8_t TIMER_## idx ##_init(void)                                        \
{                                                                        \
    if (!hri_tc_is_syncing(TC## idx, TC_SYNCBUSY_SWRST)) {               \
        if (hri_tc_get_CTRLA_reg(TC## idx, TC_CTRLA_ENABLE)) {           \
            hri_tc_clear_CTRLA_ENABLE_bit(TC## idx);                     \
            hri_tc_wait_for_sync(TC## idx, TC_SYNCBUSY_ENABLE);          \
        }                                                                \
        hri_tc_write_CTRLA_reg(TC## idx, TC_CTRLA_SWRST);                \
    }                                                                    \
    hri_tc_wait_for_sync(TC## idx, TC_SYNCBUSY_SWRST);                   \
                                                                         \
    hri_tc_write_CTRLB_reg(TC## idx,                                     \
                           0 << TC_CTRLBSET_CMD_Pos                      \
                               | 0 << TC_CTRLBSET_ONESHOT_Pos            \
                               | 0 << TC_CTRLBCLR_LUPD_Pos               \
                               | 0 << TC_CTRLBSET_DIR_Pos);              \
                                                                         \
    hri_tc_write_WAVE_reg(TC## idx, 2);                                  \
    hri_tc_write_CTRLA_ENABLE_bit(TC## idx, 1 << TC_CTRLA_ENABLE_Pos);   \
                                                                         \
    return 0;                                                            \
}

TIMER_INIT_FN(0)
TIMER_INIT_FN(1)
TIMER_INIT_FN(2)
TIMER_INIT_FN(3)
TIMER_INIT_FN(4)
TIMER_INIT_FN(5)

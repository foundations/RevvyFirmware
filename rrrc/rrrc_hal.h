/*
 * worklogic.h
 *
 * Created: 1/4/2019 8:22:31 PM
 *  Author: User
 */ 


#ifndef RRRC_HAL_H_
#define RRRC_HAL_H_

#include "driver_init.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_rtos.h"

#ifdef __cplusplus
extern "C" {
#endif

void high_res_timer_init(struct timer_descriptor* timer);
uint32_t high_res_timer_get_count(void);
uint32_t high_res_timer_ticks_per_ms(void);

static inline uint32_t rtos_get_ticks_in_ms()
{
    return portTICK_PERIOD_MS;
}

static inline uint32_t rtos_ms_to_ticks(uint32_t ms)
{
    return ms * rtos_get_ticks_in_ms();
}

#ifdef __cplusplus
}
#endif

#endif /* RRRC_HAL_H_ */
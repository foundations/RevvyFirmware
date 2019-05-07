/*
 * worklogic.h
 *
 * Created: 1/4/2019 8:22:31 PM
 *  Author: User
 */ 


#ifndef WORKLOGIC_H_
#define WORKLOGIC_H_

#include "driver_init.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_rtos.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_TRANSACTION_SIZE 3
#define MAX_TRANSACTION_SIZE 127

typedef struct _trans_buffer_t
{
    uint8_t buff[MAX_TRANSACTION_SIZE];
    uint32_t size;
    uint32_t index;
}trans_buffer_t, *p_trans_buffer_t;

int32_t RRRC_add_task(struct timer_task *const task, timer_task_cb_t func, uint32_t interval, void* user_data, bool oneshot);
int32_t RRRC_remove_task(struct timer_task const* task);

void high_res_timer_init(struct timer_descriptor* timer);
uint32_t high_res_timer_get_count(void);
uint32_t high_res_timer_ticks_per_ms(void);

static inline uint32_t rtos_get_ticks_in_ms()
{
	//uint32_t ticks = xTaskGetTickCount();
	return portTICK_PERIOD_MS;
}

static inline uint32_t rtos_ms_to_ticks(uint32_t ms)
{
	return ms * rtos_get_ticks_in_ms();
}

#ifdef __cplusplus
}
#endif

#endif /* WORKLOGIC_H_ */
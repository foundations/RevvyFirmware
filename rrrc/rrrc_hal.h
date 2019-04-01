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

typedef struct _mutex_t
{
	int mtx;
}mutex_t, *p_mutex_t;
static void Lock(mutex_t* mtx)
{
	CRITICAL_SECTION_ENTER();
	while(mtx->mtx);	mtx->mtx = 1;
	CRITICAL_SECTION_LEAVE();
}

static bool TryLock(mutex_t* mtx)
{
	CRITICAL_SECTION_ENTER();
	if (mtx->mtx)	
		return false;
	mtx->mtx = 1;
	return true;
	CRITICAL_SECTION_LEAVE();
}
static void UnLock(mutex_t* mtx)
{
	CRITICAL_SECTION_ENTER();
	mtx->mtx = 0;
	CRITICAL_SECTION_LEAVE();
}

#define MIN_TRANSACTION_SIZE 3
#define MAX_TRANSACTION_SIZE 127

typedef struct _trans_buffer_t
{
    uint8_t buff[MAX_TRANSACTION_SIZE];
    uint32_t size;
    uint32_t index;
}trans_buffer_t, *p_trans_buffer_t;

typedef void (*channel_adc_data_cb_t)(const uint8_t adc_data, void* user_data);

int32_t adc_convertion_start(uint32_t adc_idx);
int32_t adc_convertion_stop(uint32_t adc_idx);
int32_t RRRC_channel_adc_register_cb(uint32_t adc_idx, uint32_t chan_idx, channel_adc_data_cb_t func, void* user_data);
int32_t RRRC_channel_adc_unregister_cb(uint32_t adc_idx, uint32_t chan_idx);
int32_t RRRC_add_task(struct timer_task *const task, timer_task_cb_t func, uint32_t interval, void* user_data, bool oneshot);
int32_t RRRC_remove_task(struct timer_task const* task);

static uint32_t SwapEndian(uint32_t dig)
{
    uint8_t *c = (uint8_t *) &dig;
    return * (uint32_t *) (uint8_t[]) {c[3], c[2], c[1], c[0] };
}

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
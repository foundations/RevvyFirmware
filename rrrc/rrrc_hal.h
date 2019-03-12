/*
 * worklogic.h
 *
 * Created: 1/4/2019 8:22:31 PM
 *  Author: User
 */ 


#ifndef WORKLOGIC_H_
#define WORKLOGIC_H_

#include "driver_init.h"

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


typedef void (*channel_adc_data_cb_t)(const uint8_t adc_data, void* user_data);

void RRRC_channel_adc_register_cb(uint32_t chan_idx, channel_adc_data_cb_t func, void* user_data);
void RRRC_channel_adc_unregister_cb(uint32_t chan_idx);
int32_t RRRC_add_task(struct timer_task *const task, timer_task_cb_t func, uint32_t interval, void* user_data, bool oneshot);
int32_t RRRC_remove_task(struct timer_task const* task);

int RRRC_Init(void);


static uint32_t SwapEndian(uint32_t dig)
{
    uint8_t *c = (uint8_t *) &dig;
    return * (uint32_t *) (uint8_t[]) {c[3], c[2], c[1], c[0] };
}

#ifdef __cplusplus
}
#endif

#endif /* WORKLOGIC_H_ */
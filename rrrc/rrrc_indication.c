/*
 * rrrc_indication.c
 *
 * Created: 10-Mar-19 01:48:33
 *  Author: vmyakovlev
 */ 


led_val_t status_leds[STATUS_LEDS_AMOUNT];
led_val_t ring_leds[RING_LEDS_AMOUNT];

static struct timer_task* indication_thread;

static void indication_thread_tick_cb(const struct timer_task *const timer_task)
{

	return;
}

uint32_t IndicationInit()
{
	uint32_t result = ERR_NONE;
	
	indication_thread = RRRC_add_task(&indication_thread_tick_cb, 33/*ms as fps*/, NULL, false);
	
	return result;
}

uint32_t IndicationDeInit()
{
	uint32_t result = ERR_NONE;
	if (indication_thread)
		RRRC_remove_task(indication_thread)
	indication_thread = NULL;
	return result;	
}
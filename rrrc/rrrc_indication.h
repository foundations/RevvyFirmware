/*
 * rrrc_indication.h
 *
 * Created: 10-Mar-19 01:42:29
 *  Author: vmyakovlev
 */ 


#ifndef RRRC_INDICATION_H_
#define RRRC_INDICATION_H_


#define STATUS_LEDS_AMOUNT 4
#define RING_LEDS_AMOUNT 12

typedef struct _led_val_t
{
	uint8_t R;
	uint8_t G;
	uint8_t B;	
}led_val_t, *p_led_val_t;


uint32_t IndicationGetStatusLedsAmount();
uint32_t IndicationGetRingLedsAmount();



uint32_t IndicationInit();
uint32_t IndicationDeInit();

#endif /* RRRC_INDICATION_H_ */
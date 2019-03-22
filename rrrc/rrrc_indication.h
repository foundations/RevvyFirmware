/*
 * rrrc_indication.h
 *
 * Created: 10-Mar-19 01:42:29
 *  Author: vmyakovlev
 */ 


#ifndef RRRC_INDICATION_H_
#define RRRC_INDICATION_H_

#include <stdint.h>

#define STATUS_LEDS_AMOUNT 4
#define RING_LEDS_AMOUNT 12

#define LEDS_USER_MAX_FRAMES 100

enum INDICATON_RING_TYPE
{
	RING_LED_OFF,
	RING_LED_USER,
	RING_LED_PREDEF_1,
	RING_LED_PREDEF_2,
	RING_LED_PREDEF_3,
	RING_LED_PREDEF_4,

	RING_LED_PREDEF_MAX //Must be last
};

// 
// /* some RGB color definitions                                                 */
// #define Black           0x0000      /*   0,   0,   0 */
// #define Navy            0x000F      /*   0,   0, 128 */
// #define DarkGreen       0x03E0      /*   0, 128,   0 */
// #define DarkCyan        0x03EF      /*   0, 128, 128 */
// #define Maroon          0x7800      /* 128,   0,   0 */
// #define Purple          0x780F      /* 128,   0, 128 */
// #define Olive           0x7BE0      /* 128, 128,   0 */
// #define LightGrey       0xC618      /* 192, 192, 192 */
// #define DarkGrey        0x7BEF      /* 128, 128, 128 */
// #define Blue            0x001F      /*   0,   0, 255 */
// #define Green           0x07E0      /*   0, 255,   0 */
// #define Cyan            0x07FF      /*   0, 255, 255 */
// #define Red             0xF800      /* 255,   0,   0 */
// #define Magenta         0xF81F      /* 255,   0, 255 */
// #define Yellow          0xFFE0      /* 255, 255,   0 */
// #define White           0xFFFF      /* 255, 255, 255 */
// #define Orange          0xFD20      /* 255, 165,   0 */
// #define GreenYellow     0xAFE5      /* 173, 255,  47 */
// #define Pink                        0xF81F

#define LED_RED		0x00,0x30,0x00
#define LED_GREEN	0x30,0x00,0x00
#define LED_BLUE	0x00,0x00,0x30
#define LED_YELLOW	0x30,0x30,0x00
#define LED_CYAN	0x30,0x00,0x30
#define LED_WHITE	0x30,0x30,0x30
#define LED_OFF		0x00,0x00,0x00

typedef struct _led_val_t
{
	uint8_t R;
	uint8_t G;
	uint8_t B;	
}led_val_t, *p_led_val_t, led_status_t[STATUS_LEDS_AMOUNT], led_ring_frame_t[RING_LEDS_AMOUNT];



uint32_t IndicationGetStatusLedsAmount();
uint32_t IndicationGetRingLedsAmount();



uint32_t IndicationInit();
uint32_t IndicationDeInit();

#endif /* RRRC_INDICATION_H_ */
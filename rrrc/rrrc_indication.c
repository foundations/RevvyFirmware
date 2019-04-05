/*
 * rrrc_indication.c
 *
 * Created: 10-Mar-19 01:48:33
 *  Author: vmyakovlev
 */ 

#include "rrrc_hal.h"
#include "rrrc_indication.h"
#include "rrrc_indication_predefined.h"

#include <math.h>

led_status_t status_leds = 
{
	{LED_GREEN},
	{LED_GREEN},
	{LED_BLUE},
	{LED_WHITE}
};

typedef struct {
    uint32_t frame_max;
    led_ring_frame_t* frames;
    uint32_t period;
    uint32_t periodCounter;
    uint32_t frame_curr;
} frame_data_t, *p_frame_data_t;

typedef struct
{
    hsv_t color;
    float time;
    uint32_t periodCounter;
} colorwheel_data_t, *p_colorwheel_data_t;

static led_ring_frame_t* Led_ring_curr_buff = NULL;
static bool led_ring_busy;

struct _indication_handler_t;

typedef void (*ledRingInitializer)(struct _indication_handler_t* data);
typedef void (*ledRingWriter)(uint8_t* frame_leds, uint32_t* frame_idx, struct _indication_handler_t* data);

typedef struct _indication_handler_t
{
    ledRingInitializer init;
    ledRingWriter handler;
    void* userData;
} indication_handler_t;

static void initUserFrameWriter(indication_handler_t* data);
static void initFrameWriter(indication_handler_t* data);
static void initColorWheel(indication_handler_t* data);

static void ledRingOffWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data);
static void ledRingFrameWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data);
static void colorWheelWriter1(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data);

#define LED_RESET_SIZE 50
uint8_t frame_leds[LED_RESET_SIZE + (sizeof(led_val_t) * (STATUS_LEDS_AMOUNT + RING_LEDS_AMOUNT) * 8)];

enum INDICATON_RING_TYPE led_ring_mode = RING_LED_OFF;

static led_ring_frame_t led_ring_userframes[LEDS_USER_MAX_FRAMES] = {0x00};
static frame_data_t user_frame = { .frames = led_ring_userframes };

static frame_data_t predefined_frame_1 = { .period = 10, .frames = &ring_leds_round_red        , .frame_max = ARRAY_SIZE(ring_leds_round_red) };
static frame_data_t predefined_frame_2 = { .period = 10, .frames = &ring_leds_round_green      , .frame_max = ARRAY_SIZE(ring_leds_round_green) };
static frame_data_t predefined_frame_3 = { .period = 10, .frames = &ring_leds_round_blue       , .frame_max = ARRAY_SIZE(ring_leds_round_blue) };
static frame_data_t predefined_frame_4 = { .period = 10, .frames = &ring_leds_running_fire_blue, .frame_max = ARRAY_SIZE(ring_leds_running_fire_blue) };

static led_val_t hsv_to_rgb(hsv_t hsv_col)
{
    /* calculate base color from hue */
    led_val_t rgb_col;

    float h = hsv_col.h % 360;
    float s = hsv_col.s / 100.0f;
    float v = hsv_col.v / 100.0f;

    float hh = h / 60;
    int8_t i = (int8_t) hh;
    
    float ff = hh - i;
    float p = v * (1.0 - s);
    float q = v * (1.0 - (s * ff));
    float t = v * (1.0 - (s * (1.0 - ff)));
    
    float r, g, b;

    switch (i)
    {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
        default:
            r = v;
            g = p;
            b = q;
            break;
    }

    return (led_val_t) 
    {
        .R = (uint8_t) (r*255), 
        .G = (uint8_t) (g*255), 
        .B = (uint8_t) (b*255), 
    };
}

static colorwheel_data_t colorWheelData1 = { .color = { LED_HSV_CYAN } };

static indication_handler_t indication_handlers[RING_LED_PREDEF_MAX] = 
{
    [RING_LED_OFF]      = { .init = NULL,                 .handler = &ledRingOffWriter,   .userData = NULL },
    [RING_LED_USER]     = { .init = &initUserFrameWriter, .handler = &ledRingFrameWriter, .userData = &user_frame },
    [RING_LED_PREDEF_1] = { .init = &initFrameWriter,     .handler = &ledRingFrameWriter, .userData = &predefined_frame_1 },
    [RING_LED_PREDEF_2] = { .init = &initFrameWriter,     .handler = &ledRingFrameWriter, .userData = &predefined_frame_2 },
    [RING_LED_PREDEF_3] = { .init = &initFrameWriter,     .handler = &ledRingFrameWriter, .userData = &predefined_frame_3 },
    [RING_LED_PREDEF_4] = { .init = &initFrameWriter,     .handler = &ledRingFrameWriter, .userData = &predefined_frame_4 },
    
    [RING_LED_COLORWHEEL_1] = { .init = &initColorWheel, .handler = &colorWheelWriter1, .userData = &colorWheelData1 }
};

static TaskHandle_t      xIndicationTask;
//*********************************************************************************************
static void tx_complete_cb_SPI_0(struct _dma_resource *resource)
{
	led_ring_busy = false;
	return;
}

//*********************************************************************************************
static inline uint8_t getLedBitPattern(uint8_t bitValue)
{
#define LED_VAL_ZERO 0xC0
#define LED_VAL_ONE  0xFC
	
	if (bitValue)
	{
		return LED_VAL_ONE;
	}
	else
	{
		return LED_VAL_ZERO;
	}
}

static void add_frame_byte(uint8_t* frame_leds, uint32_t* frame_idx, uint8_t byte)
{
    for (int32_t bit=7; bit >= 0; bit--)
    {
        frame_leds[(*frame_idx)++] = getLedBitPattern((byte >> bit) & 0x01u);
    }
}

static void initUserFrameWriter(indication_handler_t* data)
{
    p_frame_data_t userData = (p_frame_data_t) data->userData;
    memset(&userData->frames[0], 0, ARRAY_SIZE(led_ring_userframes));
    userData->frame_max = 1;
    userData->frame_curr = 0;
}

static void initFrameWriter(indication_handler_t* data)
{
    p_frame_data_t userData = (p_frame_data_t) data->userData;
    userData->frame_curr = 0;
    userData->periodCounter = 0;
}

static void initColorWheel(indication_handler_t* data)
{
    p_colorwheel_data_t userData = (p_colorwheel_data_t) data->userData;

    userData->time = 0;
}

static void ledRingOffWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data)
{
    for(uint32_t idx=0; idx<RING_LEDS_AMOUNT; idx++)
    {
        add_frame_byte(frame_leds, frame_idx, 0);
        add_frame_byte(frame_leds, frame_idx, 0);
        add_frame_byte(frame_leds, frame_idx, 0);
    }
}

static void ledRingFrameWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* frameData)
{
    p_frame_data_t userData = (p_frame_data_t) frameData->userData;
    for(uint32_t idx=0; idx<RING_LEDS_AMOUNT; idx++)
    {
        add_frame_byte(frame_leds, frame_idx, userData->frames[userData->frame_curr][idx].G);
        add_frame_byte(frame_leds, frame_idx, userData->frames[userData->frame_curr][idx].R);
        add_frame_byte(frame_leds, frame_idx, userData->frames[userData->frame_curr][idx].B);
    }

    userData->periodCounter++;
    if (userData->periodCounter >= userData->period)
    {
        userData->periodCounter = 0u;
        userData->frame_curr++;
        if (userData->frame_curr >= userData->frame_max)
        {
            userData->frame_curr = 0;
        }
    }
}

static void colorWheelWriter1(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data)
{
    p_colorwheel_data_t userData = (p_colorwheel_data_t) data->userData;
    float t = userData->time;
    const float f = 1;
    for (uint8_t i = 0; i < 12; i++)
    {
        float phase = i * M_PI / 12;
        float brightness = 0.3;// (sinf(M_PI * f * t + phase) + 1.01f);

        hsv_t hsv = userData->color;
        hsv.s = ((200 + ((hsv.s + 8 * i) % 100)) / 3);
        hsv.v *= brightness;
        led_val_t rgb = hsv_to_rgb(hsv);
        
        add_frame_byte(frame_leds, frame_idx, rgb.G);
        add_frame_byte(frame_leds, frame_idx, rgb.R);
        add_frame_byte(frame_leds, frame_idx, rgb.B);
    }
    userData->periodCounter++;
    if (userData->periodCounter >= 4)
    {
        userData->periodCounter = 0u;
        userData->color.s = (userData->color.s + 8) % 100;
    }

    float dt = 0.02f;
    userData->time = t + dt;
}

//*********************************************************************************************
static void MakeLedBuffer()
{
#define LED_VAL_RES 0x00

	uint32_t frame_idx = 0;
	for (int32_t idx=0; idx<LED_RESET_SIZE; idx++)
	frame_leds[frame_idx++] = LED_VAL_RES;
	
	for (uint32_t idx=0; idx<STATUS_LEDS_AMOUNT; idx++)
	{
    	add_frame_byte(frame_leds, &frame_idx, status_leds[idx].G);
    	add_frame_byte(frame_leds, &frame_idx, status_leds[idx].R);
    	add_frame_byte(frame_leds, &frame_idx, status_leds[idx].B);
	}

    indication_handlers[led_ring_mode].handler(frame_leds, &frame_idx, &indication_handlers[led_ring_mode]);
}

static void Indication_xTask(const void* user_data)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	led_ring_busy = false;
	for(;;)
	{
		if (!led_ring_busy)
		{
			MakeLedBuffer();
			struct io_descriptor *io;
			spi_m_dma_get_io_descriptor(&SPI_0, &io);

			spi_m_dma_register_callback(&SPI_0, SPI_M_DMA_CB_TX_DONE, tx_complete_cb_SPI_0);
			spi_m_dma_enable(&SPI_0);
			led_ring_busy = true;
			io_write(io, frame_leds, ARRAY_SIZE(frame_leds));
		}
		vTaskDelayUntil(&xLastWakeTime, rtos_ms_to_ticks(20u));
	}
	return;
}

//*********************************************************************************************
int32_t IndicationUpdateUserFrame(uint32_t frame_idx, led_ring_frame_t* frame)
{
	if (frame_idx < LEDS_USER_MAX_FRAMES && frame && led_ring_mode == RING_LED_USER)
	{
		memcpy(&led_ring_userframes[frame_idx], frame, sizeof(led_ring_frame_t));
		if (frame_idx >= user_frame.frame_max)
		{
            user_frame.frame_max = frame_idx + 1;
        }
		return ERR_NONE;
	}
    else
	{
        return ERR_INVALID_ARG;
    }
}

//*********************************************************************************************
int32_t IndicationSetStatusLed(uint32_t stled_idx, p_led_val_t led_val)
{
	if (stled_idx<STATUS_LEDS_AMOUNT && led_val)
	{
		status_leds[stled_idx] = *led_val;
	}else
		return ERR_INVALID_ARG;
		
	return ERR_NONE;
}

//*********************************************************************************************
int32_t IndicationSetRingType(enum INDICATON_RING_TYPE type)
{
	int32_t status = ERR_NONE;

    if (type < RING_LED_PREDEF_MAX)
    {
        led_ring_mode = type;
        if (indication_handlers[led_ring_mode].init)
        {
            indication_handlers[led_ring_mode].init(&indication_handlers[led_ring_mode]);
        }
    }
    else
    {
        led_ring_mode = RING_LED_OFF;
        status = ERR_INVALID_DATA;
    }

	return status;
}

//*********************************************************************************************
uint32_t IndicationGetRingLedsAmount()
{
	return RING_LEDS_AMOUNT;
}

//*********************************************************************************************
uint32_t IndicationGetStatusLedsAmount()
{
	return STATUS_LEDS_AMOUNT;
}

//*********************************************************************************************
int32_t IndicationInit(){
	uint32_t result = ERR_NONE;
	IndicationSetRingType(RING_LED_OFF);

	char task_name[configMAX_TASK_NAME_LEN+1] = "Indication";
	if (xTaskCreate(Indication_xTask, task_name, 1024 / sizeof(portSTACK_TYPE), NULL, 5, &xIndicationTask) != pdPASS) 
		return ERR_FAILURE;

	return result;}
//*********************************************************************************************
int32_t IndicationDeInit()
{
	uint32_t result = ERR_NONE;
	
	vTaskDelete(xIndicationTask);

	return result;
}

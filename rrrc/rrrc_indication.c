/*
 * rrrc_indication.c
 *
 * Created: 10-Mar-19 01:48:33
 *  Author: vmyakovlev
 */ 

#include "rrrc_hal.h"
#include "rrrc_indication.h"
#include "rrrc_indication_predefined.h"

led_status_t status_leds = 
{
	{LED_OFF},
	{LED_OFF},
	{LED_OFF},
	{LED_RED}
};

typedef struct {
    uint32_t frame_max;
    led_ring_frame_t* frames;
} frame_data_t, *p_frame_data_t;

static led_ring_frame_t* Led_ring_curr_buff = NULL;
static bool led_ring_busy;

struct _indication_handler_t;

typedef void (*ledRingInitializer)(struct _indication_handler_t* data);
typedef void (*ledRingWriter)(uint8_t* frame_leds, uint32_t* frame_idx, struct _indication_handler_t* data);

typedef struct _indication_handler_t
{
    ledRingInitializer init;
    ledRingWriter handler;
    frame_data_t* userData;
    uint32_t period;
    uint32_t periodCounter;
    uint32_t frame_curr;
} indication_handler_t;

static void ledRingOffWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data);
static void ledRingFrameWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data);

static void initUserFrameWriter(indication_handler_t* data);
static void initFrameWriter(indication_handler_t* data);

#define LED_RESET_SIZE 50
uint8_t frame_leds[LED_RESET_SIZE + (sizeof(led_val_t) * (STATUS_LEDS_AMOUNT + RING_LEDS_AMOUNT) * 8)];

enum INDICATON_RING_TYPE led_ring_mode = RING_LED_OFF;

static led_ring_frame_t led_ring_userframes[LEDS_USER_MAX_FRAMES] = {0x00};
static frame_data_t user_frame = { .frames = led_ring_userframes };

static frame_data_t predefined_frame_1 = { .frames = &ring_leds_round_red         , .frame_max = ARRAY_SIZE(ring_leds_round_red) };
static frame_data_t predefined_frame_2 = { .frames = &ring_leds_round_green       , .frame_max = ARRAY_SIZE(ring_leds_round_green) };
static frame_data_t predefined_frame_3 = { .frames = &ring_leds_round_blue        , .frame_max = ARRAY_SIZE(ring_leds_round_blue) };
static frame_data_t predefined_frame_4 = { .frames = &ring_leds_running_fire_blue , .frame_max = ARRAY_SIZE(ring_leds_running_fire_blue) };

static indication_handler_t indication_handlers[RING_LED_PREDEF_MAX] = 
{
    [RING_LED_OFF]      = { .period = 0,  .init = NULL,                 .handler = &ledRingOffWriter,   .userData = NULL },
    [RING_LED_USER]     = { .period = 10, .init = &initUserFrameWriter, .handler = &ledRingFrameWriter, .userData = &user_frame },
    [RING_LED_PREDEF_1] = { .period = 10, .init = &initFrameWriter,     .handler = &ledRingFrameWriter, .userData = &predefined_frame_1 },
    [RING_LED_PREDEF_2] = { .period = 10, .init = &initFrameWriter,     .handler = &ledRingFrameWriter, .userData = &predefined_frame_2 },
    [RING_LED_PREDEF_3] = { .period = 10, .init = &initFrameWriter,     .handler = &ledRingFrameWriter, .userData = &predefined_frame_3 },
    [RING_LED_PREDEF_4] = { .period = 10, .init = &initFrameWriter,     .handler = &ledRingFrameWriter, .userData = &predefined_frame_4 },
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
        frame_leds[(*frame_idx)++] = getLedBitPattern(byte >> bit);
    }
}

static void initUserFrameWriter(indication_handler_t* data)
{
    memset(&data->userData->frames[0], 0, ARRAY_SIZE(led_ring_userframes));
    data->userData->frame_max = 1;
    data->frame_curr = 0;
}

static void initFrameWriter(indication_handler_t* data)
{
    data->frame_curr = 0;
    data->periodCounter = 0;
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
    for(uint32_t idx=0; idx<RING_LEDS_AMOUNT; idx++)
    {
        add_frame_byte(frame_leds, frame_idx, frameData->userData->frames[frameData->frame_curr][idx].G);
        add_frame_byte(frame_leds, frame_idx, frameData->userData->frames[frameData->frame_curr][idx].R);
        add_frame_byte(frame_leds, frame_idx, frameData->userData->frames[frameData->frame_curr][idx].B);
    }

    frameData->periodCounter++;
    if (frameData->periodCounter >= frameData->period)
    {
        frameData->periodCounter = 0u;
        frameData->frame_curr++;
        if (frameData->frame_curr >= frameData->userData->frame_max)
        {
            frameData->frame_curr = 0;
        }
    }
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
	IndicationSetRingType(RING_LED_PREDEF_4);

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
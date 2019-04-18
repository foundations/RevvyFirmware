/*
 * rrrc_indication.c
 *
 * Created: 10-Mar-19 01:48:33
 *  Author: vmyakovlev
 */ 

#include "rrrc_hal.h"
#include "rrrc_indication.h"
#include "rrrc_indication_predefined.h"

#include <string.h>
#include <math.h>

#define LED_VAL_ZERO    ((uint8_t) 0x3Fu)
#define LED_VAL_ONE     ((uint8_t) 0x03u)
#define LED_VAL_RES     ((uint8_t) 0xFFu)
#define LED_RESET_SIZE  ((size_t)    50u)

/* local type declarations */

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
    uint32_t time;
} colorwheel_data_t, *p_colorwheel_data_t;

struct _indication_handler_t;

typedef void (*ledRingInitializer)(struct _indication_handler_t* data);
typedef void (*ledRingWriter)(uint8_t* frame_leds, uint32_t* frame_idx, struct _indication_handler_t* data);

typedef struct _indication_handler_t
{
    ledRingInitializer init;
    ledRingWriter handler;
    void* userData;
} indication_handler_t;

/* local function declarations */

static void initUserFrameWriter(indication_handler_t* data);
static void initFrameWriter(indication_handler_t* data);
static void initColorWheel(indication_handler_t* data);

static void ledRingOffWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data);
static void ledRingFrameWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data);
static void colorWheelWriter1(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data);

/* private variable declarations */

static led_status_t status_leds = 
{
    {   // main battery
        .base_color = LED_GREEN,
        .blink_color = LED_OFF,
        .blink_period = 0,
        .blink_length = 0,
        .blink_counter = 0
    },
    {   // motor battery
        .base_color = LED_GREEN,
        .blink_color = LED_OFF,
        .blink_period = 0,
        .blink_length = 0,
        .blink_counter = 0
    },
    {   // bluetooth status
        .base_color = LED_OFF,
        .blink_color = LED_CYAN,
        .blink_period = 200,
        .blink_length = 3,
        .blink_counter = 0
    },
    {   // raspberry status
        .base_color = LED_RED,
        .blink_color = LED_OFF,
        .blink_period = 0,
        .blink_length = 0,
        .blink_counter = 0
    }
};

static bool led_ring_busy;

uint8_t frame_leds[LED_RESET_SIZE + (sizeof(led_val_t) * (STATUS_LEDS_AMOUNT + RING_LEDS_AMOUNT) * 8)];

enum INDICATON_RING_TYPE led_ring_mode = RING_LED_OFF;

static led_ring_frame_t led_ring_userframes[LEDS_USER_MAX_FRAMES] = {0x00};
static frame_data_t user_frame = { .frames = led_ring_userframes };

static frame_data_t predefined_frame_1 = { .period = 10, .frames = ring_leds_round_red        , .frame_max = ARRAY_SIZE(ring_leds_round_red) };
static frame_data_t predefined_frame_2 = { .period = 10, .frames = ring_leds_round_green      , .frame_max = ARRAY_SIZE(ring_leds_round_green) };
static frame_data_t predefined_frame_3 = { .period = 10, .frames = ring_leds_round_blue       , .frame_max = ARRAY_SIZE(ring_leds_round_blue) };
static frame_data_t predefined_frame_4 = { .period = 10, .frames = ring_leds_running_fire_blue, .frame_max = ARRAY_SIZE(ring_leds_running_fire_blue) };

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

static TaskHandle_t xIndicationTask;

//*********************************************************************************************
static void tx_complete_cb_SPI_0(struct _dma_resource *resource)
{
    led_ring_busy = false;
}

//*********************************************************************************************
static inline uint8_t getLedBitPattern(uint8_t bitValue)
{    
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

static void add_frame_color(uint8_t* frame_leds, uint32_t* frame_idx, rgb_t color)
{
    add_frame_byte(frame_leds, frame_idx, color.G);
    add_frame_byte(frame_leds, frame_idx, color.R);
    add_frame_byte(frame_leds, frame_idx, color.B);
}

static void initUserFrameWriter(indication_handler_t* data)
{
    p_frame_data_t userData = (p_frame_data_t) data->userData;
    memset(&userData->frames[0], 0, ARRAY_SIZE(led_ring_userframes));
    userData->frame_max = 0;
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

    userData->time = 0u;
}

static void ledRingOffWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data)
{
    for (uint32_t idx = 0u; idx < RING_LEDS_AMOUNT; idx++)
    {
        add_frame_color(frame_leds, frame_idx, (rgb_t) {0, 0, 0});
    }
}

static void ledRingFrameWriter(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* frameData)
{
    p_frame_data_t userData = (p_frame_data_t) frameData->userData;
    if (userData->frame_max != 0u)
    {
        for (uint32_t idx = 0u; idx<RING_LEDS_AMOUNT; idx++)
        {
            add_frame_color(frame_leds, frame_idx, userData->frames[userData->frame_curr][idx]);
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
}

static void colorWheelWriter1(uint8_t* frame_leds, uint32_t* frame_idx, indication_handler_t* data)
{
    p_colorwheel_data_t userData = (p_colorwheel_data_t) data->userData;
    
    hsv_t startColor = userData->color;
    startColor.h = (startColor.h + userData->time) % 360;

    for (uint8_t i = 0; i < 12u; i++)
    {
        hsv_t hsv = startColor;
        hsv.h = hsv.h + i * 30u;
        led_val_t rgb = hsv_to_rgb(hsv);
        
        add_frame_color(frame_leds, frame_idx, rgb);
    }
    userData->time += 6u;
}

//*********************************************************************************************
static void handle_status_led(uint8_t* frame_leds, uint32_t* frame_idx, led_t* led)
{
    if (led->blink_length == 0)
    {
        add_frame_color(frame_leds, frame_idx, led->base_color);
    }
    else
    {
        if (led->blink_counter < led->blink_length)
        {
            add_frame_color(frame_leds, frame_idx, led->blink_color);
        }
        else
        {
            add_frame_color(frame_leds, frame_idx, led->base_color);
        }

        if (led->blink_counter == led->blink_period - 1u)
        {
            led->blink_counter = 0u;
        }
        else
        {
            led->blink_counter++;
        }
    }
}

static bool MakeLedBuffer()
{
    uint32_t frame_idx = 0u;
    for (uint32_t idx = 0u; idx < LED_RESET_SIZE; idx++)
    {
        frame_leds[frame_idx++] = LED_VAL_RES;
    }

    for (uint32_t idx = 0u; idx < STATUS_LEDS_AMOUNT; idx++)
    {
        handle_status_led(frame_leds, &frame_idx, &status_leds[idx]);
    }

    indication_handlers[led_ring_mode].handler(frame_leds, &frame_idx, &indication_handlers[led_ring_mode]);

    return true;
}

static void Indication_xTask(const void* user_data)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    led_ring_busy = false;

    struct io_descriptor *io;
    spi_m_dma_get_io_descriptor(&SPI_0, &io);
    spi_m_dma_register_callback(&SPI_0, SPI_M_DMA_CB_TX_DONE, &tx_complete_cb_SPI_0);
    spi_m_dma_enable(&SPI_0);

    for(;;)
    {
        if (!led_ring_busy)
        {
            CRITICAL_SECTION_ENTER();
            MakeLedBuffer();
            CRITICAL_SECTION_LEAVE();

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
        if (frame_idx > user_frame.frame_max)
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
    int32_t status = ERR_NONE;
    if (stled_idx < STATUS_LEDS_AMOUNT && led_val)
    {
        status_leds[stled_idx].base_color = *led_val;
    }
    else
    {
        status = ERR_INVALID_ARG;
    }

    return status;
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
    int32_t result = ERR_NONE;
    IndicationSetRingType(RING_LED_OFF);
    
    if (xTaskCreate(&Indication_xTask, "Indication", 1024 / sizeof(portSTACK_TYPE), NULL, tskIDLE_PRIORITY, &xIndicationTask) != pdPASS)
    {
        result = ERR_FAILURE;
    }

    return result;}
//*********************************************************************************************
int32_t IndicationDeInit()
{
    int32_t result = ERR_NONE;
    
    vTaskDelete(xIndicationTask);

    return result;
}

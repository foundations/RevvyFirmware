/*
 * RingLedDisplay.c
 *
 * Created: 03/05/2019 15:31:44
 *  Author: Dániel Buga
 */ 
#include "RingLedDisplay.h"
#include "rrrc_indication.h"
#include "utils.h"

#include <string.h>

typedef void (*ledRingFn)(void* data);
typedef void (*ledRingFn)(void* data);

typedef struct
{
    const char* name;
    ledRingFn init;
    ledRingFn handler;
    ledRingFn DeInit;
    void* userData;
} indication_handler_t;

typedef struct
{
    uint32_t phase;
} colorwheel_data_t, *p_colorwheel_data_t;

static colorwheel_data_t colorWheelData1;

typedef struct {
    uint32_t frame_max;
    led_ring_frame_t* frames;
    uint32_t period;
    uint32_t periodCounter;
    uint32_t frame_curr;
} frame_data_t, *p_frame_data_t;

/* local function declarations */

static void initUserFrameWriter(void* data);
static void initColorWheel(void* data);

static void ledRingOffWriter(void* data);
static void ledRingFrameWriter(void* data);
static void colorWheelWriter1(void* data);

static led_ring_frame_t led_ring_userframes[LEDS_USER_MAX_FRAMES] = { 0 };
static frame_data_t user_frame = { .frames = led_ring_userframes };
static RingLedScenario_t currentScenario;
static RingLedScenario_t requestedScenario;
static indication_handler_t scenarioHandlers[] = 
{
    [RingLedScenario_Off] = { .name="RingLedOff", .init = NULL, .handler = &ledRingOffWriter, .DeInit = NULL, .userData = NULL },
    
    [RingLedScenario_UserAnimation] = { .name="UserAnimation", .init = &initUserFrameWriter, .handler = &ledRingFrameWriter, .DeInit = NULL, .userData = &user_frame },
    [RingLedScenario_ColorWheel]    = { .name="ColorWheel", .init = &initColorWheel,      .handler = &colorWheelWriter1, .DeInit = NULL, .userData = &colorWheelData1 },
};

Comm_Status_t RingLedDisplay_GetScenarioTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    uint8_t len = 0u;
    for (uint32_t i = 0u; i < ARRAY_SIZE(scenarioHandlers); i++)
    {
        const indication_handler_t* lib = &scenarioHandlers[i];
        size_t name_length = strlen(lib->name);
        if (len + name_length + 2u > responseBufferSize)
        {
            *responseCount = 0u;
            return Comm_Status_Error_InternalError;
        }
        response[len] = i;
        response[len + 1] = name_length;
        memcpy(&response[len + 2], lib->name, name_length);
        len = len + 2 + name_length;
    }
    *responseCount = len;

    return Comm_Status_Ok;
}

Comm_Status_t RingLedDisplay_SetScenarioType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != 1u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    uint8_t idx = commandPayload[0];
    if (idx >= ARRAY_SIZE(scenarioHandlers))
    {
        return Comm_Status_Error_CommandError;
    }

    requestedScenario = (RingLedScenario_t) idx;

    return Comm_Status_Ok;
}

static void ledRingOffWriter(void* data)
{
    for (uint32_t idx = 0u; idx < RING_LEDS_AMOUNT; idx++)
    {
        RingLedDisplay_Write_LedColor(idx, (rgb_t) LED_OFF);
    }
}

static void initUserFrameWriter(void* data)
{
    p_frame_data_t userData = (p_frame_data_t) data;
    memset(&userData->frames[0], 0, sizeof(led_ring_userframes));
    userData->frame_max = 0;
    userData->frame_curr = 0;
}

static void ledRingFrameWriter(void* frameData)
{
    p_frame_data_t userData = (p_frame_data_t) frameData;

    /* leave output unchanged if there is nothing to display - avoids flashing */
    if (userData->frame_max != 0u)
    {
        for (uint32_t idx = 0u; idx < RING_LEDS_AMOUNT; idx++)
        {
            RingLedDisplay_Write_LedColor(idx, userData->frames[userData->frame_curr][idx]);
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

static void initColorWheel(void* data)
{
    p_colorwheel_data_t userData = (p_colorwheel_data_t) data;

    userData->phase = 0u;
}

static void colorWheelWriter1(void* data)
{
    p_colorwheel_data_t userData = (p_colorwheel_data_t) data;
    
    for (uint32_t i = 0u; i < RING_LEDS_AMOUNT; i++)
    {
        hsv_t hsv = {
            .h = userData->phase + i * 360u / RING_LEDS_AMOUNT,
            .s = 100,
            .v = 100
        };
        rgb_t rgb = hsv_to_rgb(hsv);
        
        RingLedDisplay_Write_LedColor(i, rgb);
    }
    userData->phase = (userData->phase + 6u) % 360;
}

void RingLedDisplay_Run_OnInit(void)
{
    currentScenario = RingLedScenario_Off;
}

void RingLedDisplay_Run_Update(void)
{
    RingLedScenario_t newScenario = requestedScenario;
    if (newScenario != currentScenario)
    {
        RingLedDisplay_Run_SelectScenario(newScenario);
    }

    if (currentScenario >= ARRAY_SIZE(scenarioHandlers))
    {
        currentScenario = RingLedScenario_Off;
    }
    
    scenarioHandlers[currentScenario].handler(scenarioHandlers[currentScenario].userData);
}

bool RingLedDisplay_Run_AddUserFrame(rgb_t* leds, size_t ledCount)
{
    if (user_frame.frame_max + 1u < LEDS_USER_MAX_FRAMES)
    {
        uint32_t idx = user_frame.frame_max + 1u;
        for (uint32_t i = 0u; i < ledCount && i < RING_LEDS_AMOUNT; i++)
        {
            user_frame.frames[idx][i] = leds[i];
        }
        for (uint32_t i = ledCount; i < RING_LEDS_AMOUNT; i++)
        {
            user_frame.frames[idx][i] = (rgb_t) LED_OFF;
        }
        user_frame.frame_max = idx;

        return true;
    }
    else
    {
        return false;
    }
}

void RingLedDisplay_Run_SelectScenario(RingLedScenario_t scenario)
{
    if (scenarioHandlers[currentScenario].DeInit)
    {
        scenarioHandlers[currentScenario].DeInit(scenarioHandlers[currentScenario].userData);
    }
    currentScenario = scenario;
    if (scenarioHandlers[currentScenario].init)
    {
        scenarioHandlers[currentScenario].init(scenarioHandlers[currentScenario].userData);
    }
}

__attribute__((weak))
void RingLedDisplay_Write_LedColor(uint32_t led_idx, rgb_t color)
{
    /* nothing to do */
}

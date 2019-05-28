/*
 * RingLedDisplay.c
 *
 * Created: 03/05/2019 15:31:44
 *  Author: Dániel Buga
 */ 
#include "RingLedDisplay.h"
#include "rrrc_indication.h"
#include "utils.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>

typedef void (*ledRingFn)(void* data);

typedef struct
{
    const char* name;
    ledRingFn init;
    ledRingFn handler;
    ledRingFn DeInit;
    void* userData;
} indication_handler_t;

/* local function declarations */

static void ledRingOffWriter(void* data);
static void ledRingFrameWriter(void* data);
static void colorWheelWriter1(void* data);

static rgb565_t user_frame[RING_LEDS_AMOUNT];
static RingLedScenario_t currentScenario;
static RingLedScenario_t requestedScenario;
static indication_handler_t scenarioHandlers[] = 
{
    [RingLedScenario_Off] = { .name = "RingLedOff", .init = NULL, .handler = &ledRingOffWriter, .DeInit = NULL, .userData = NULL },
    
    [RingLedScenario_UserFrame]  = { .name = "UserFrame",  .init = NULL, .handler = &ledRingFrameWriter, .DeInit = NULL, .userData = &user_frame[0] },
    [RingLedScenario_ColorWheel] = { .name = "ColorWheel", .init = NULL, .handler = &colorWheelWriter1,  .DeInit = NULL, .userData = NULL },
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

Comm_Status_t RingLedDisplay_GetRingLedAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    *response = RING_LEDS_AMOUNT;
    *responseCount = 1u;

    return Comm_Status_Ok;
}

Comm_Status_t RingLedDisplay_SetUserFrame_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    if (commandSize != RING_LEDS_AMOUNT * 2u)
    {
        return Comm_Status_Error_PayloadLengthError;
    }

    RingLedDisplay_Run_SetUserFrame((const rgb565_t*) commandPayload, RING_LEDS_AMOUNT);

    return Comm_Status_Ok;
}

static void ledRingOffWriter(void* data)
{
    for (uint32_t idx = 0u; idx < RING_LEDS_AMOUNT; idx++)
    {
        RingLedDisplay_Write_LedColor(idx, (rgb565_t) LED_OFF);
    }
}

static void ledRingFrameWriter(void* frameData)
{
    rgb565_t* userData = (rgb565_t*) frameData;
    
    for (uint32_t idx = 0u; idx < RING_LEDS_AMOUNT; idx++)
    {
        RingLedDisplay_Write_LedColor(idx, userData[idx]);
    }
}

static void colorWheelWriter1(void* data)
{
    uint32_t phase = (xTaskGetTickCount() * 6) / 20;
    
    for (uint32_t i = 0u; i < RING_LEDS_AMOUNT; i++)
    {
        hsv_t hsv = {
            .h = phase + i * 360u / RING_LEDS_AMOUNT,
            .s = 100,
            .v = 100
        };
        rgb565_t rgb = rgb_to_rgb565(hsv_to_rgb(hsv));
        
        RingLedDisplay_Write_LedColor(i, rgb);
    }
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

bool RingLedDisplay_Run_SetUserFrame(const rgb565_t* leds, size_t ledCount)
{
    for (uint32_t i = 0u; i < ledCount && i < RING_LEDS_AMOUNT; i++)
    {
        user_frame[i] = leds[i];
    }
    for (uint32_t i = ledCount; i < RING_LEDS_AMOUNT; i++)
    {
        user_frame[i] = (rgb565_t) LED_OFF;
    }

    return true;
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
void RingLedDisplay_Write_LedColor(uint32_t led_idx, rgb565_t color)
{
    /* nothing to do */
}

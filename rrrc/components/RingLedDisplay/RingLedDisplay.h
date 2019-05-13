/*
 * RingLedDisplay.h
 *
 * Created: 03/05/2019 15:30:46
 *  Author: D�niel Buga
 */

#ifndef COMPONENT_RING_LED_DISPLAY_H_
#define COMPONENT_RING_LED_DISPLAY_H_

#include <stdio.h>
#include <stdbool.h>
#include "utils/color.h"
#include "rrrc_indication.h"
#include "../MasterCommunication/CommunicationManager.h"

typedef rgb_t led_ring_frame_t[RING_LEDS_AMOUNT];

typedef enum 
{
    RingLedScenario_Off = 0,
    RingLedScenario_UserAnimation = 1,
    RingLedScenario_ColorWheel = 2,
} RingLedScenario_t;

void RingLedDisplay_Run_OnInit(void);
void RingLedDisplay_Run_Update(void);

bool RingLedDisplay_Run_AddUserFrame(rgb_t* leds, size_t ledCount);
void RingLedDisplay_Run_SelectScenario(RingLedScenario_t scenario);

void RingLedDisplay_Write_LedColor(uint32_t led_idx, rgb_t color);

Comm_Status_t RingLedDisplay_GetScenarioTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t RingLedDisplay_SetScenarioType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

#endif /* RINGLEDDISPLAY_H_ */
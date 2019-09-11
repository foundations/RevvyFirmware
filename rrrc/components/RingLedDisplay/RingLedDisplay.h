#ifndef COMPONENT_RING_LED_DISPLAY_H_
#define COMPONENT_RING_LED_DISPLAY_H_

#ifndef COMPONENT_TYPES_RING_LED_DISPLAY_H_
#define COMPONENT_TYPES_RING_LED_DISPLAY_H_

#include <stdbool.h>
#include "utils/color.h"
#include "rrrc_indication.h"
#include "../MasterCommunication/CommunicationManager.h"

typedef enum {
    RingLedScenario_Off,
    RingLedScenario_UserFrame,
    RingLedScenario_ColorWheel,
    RingLedScenario_RainbowFade,
    RingLedScenario_BusyIndicator,
    RingLedScenario_BreathingGreen
} RingLedScenario_t;

#endif /* COMPONENT_TYPES_RING_LED_DISPLAY_H_ */

void RingLedDisplay_Run_OnInit(void);
void RingLedDisplay_Run_Update(void);

bool RingLedDisplay_Run_SetUserFrame(const uint8_t* bytes, size_t ledCount);
void RingLedDisplay_Run_SelectScenario(RingLedScenario_t scenario);

void RingLedDisplay_Write_LedColor(uint32_t led_idx, rgb_t color);
bool RingLedDisplay_Read_MasterReady(void);

Comm_Status_t RingLedDisplay_GetScenarioTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t RingLedDisplay_SetScenarioType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t RingLedDisplay_GetRingLedAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t RingLedDisplay_SetUserFrame_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

#endif /* COMPONENT_RING_LED_DISPLAY_H_ */

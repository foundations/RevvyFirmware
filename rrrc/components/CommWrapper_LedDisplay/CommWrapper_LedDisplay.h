#ifndef COMPONENT_COMM_WRAPPER__LED_DISPLAY_H_
#define COMPONENT_COMM_WRAPPER__LED_DISPLAY_H_

#ifndef COMPONENT_TYPES_COMM_WRAPPER__LED_DISPLAY_H_
#define COMPONENT_TYPES_COMM_WRAPPER__LED_DISPLAY_H_

#include "utils/color.h"
#include <stdint.h>
#include <stdio.h>
#include "../MasterCommunication/CommunicationManager.h"


typedef enum {
    RingLedScenario_Off,
    RingLedScenario_UserFrame,
    RingLedScenario_ColorWheel,
    RingLedScenario_RainbowFade,
    RingLedScenario_BusyIndicator,
    RingLedScenario_BreathingGreen
} RingLedScenario_t;

typedef struct {
    uint8_t* bytes;
    size_t count;
} ByteArray_t;

#endif /* COMPONENT_TYPES_COMM_WRAPPER__LED_DISPLAY_H_ */

Comm_Status_t CommWrapper_LedDisplay_GetScenarioTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t CommWrapper_LedDisplay_SetScenarioType_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t CommWrapper_LedDisplay_GetRingLedAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);
Comm_Status_t CommWrapper_LedDisplay_SetUserFrame_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount);

uint8_t CommWrapper_LedDisplay_Call_ReadScenarioName(RingLedScenario_t scenario, ByteArray_t destination);
void CommWrapper_LedDisplay_Write_Scenario(const RingLedScenario_t value);
void CommWrapper_LedDisplay_Write_UserFrame(uint32_t index, const rgb_t value);

#endif /* COMPONENT_COMM_WRAPPER__LED_DISPLAY_H_ */

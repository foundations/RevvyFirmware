/*
 * CommunicationObserver.c
 *
 * Created: 2019. 05. 07. 21:29:10
 *  Author: Dániel Buga
 */ 

#include "CommunicationObserver.h"
#include <stdint.h>
#include <stdbool.h>

#define ERROR_COUNTER_MAX              ((uint8_t) 4u)
#define ERROR_COUNTER_INCREMENT        ((uint8_t) 2u)

static uint8_t errorCounter;
static bool communicationWasActive;

void CommunicationObserver_Run_OnInit(void)
{
    errorCounter = ERROR_COUNTER_MAX;
    communicationWasActive = false;
}

void CommunicationObserver_Run_OnMessageMissed(void)
{
    if (errorCounter > 0)
    {
        --errorCounter;
    }

    if (errorCounter == 0u && communicationWasActive)
    {
        CommunicationObserver_Call_ErrorThresholdReached();
    }
}

void CommunicationObserver_Run_OnMessageReceived(void)
{
    communicationWasActive = true;
    if (errorCounter > ERROR_COUNTER_MAX - ERROR_COUNTER_INCREMENT)
    {
        errorCounter = ERROR_COUNTER_MAX;
    }
    else
    {
        errorCounter += ERROR_COUNTER_INCREMENT;
    }
}

__attribute__((weak))
void CommunicationObserver_Call_ErrorThresholdReached(void)
{
    /* nothing to do */
}

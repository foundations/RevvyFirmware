/*
 * BrainStatusIndicator.h
 *
 * Created: 03/05/2019 13:52:54
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_BRAIN_STATUS_INDICATOR_H_
#define COMPONENT_BRAIN_STATUS_INDICATOR_H_

#include "utils/color.h"
#include <stdbool.h>

typedef enum
{
    SystemState_Startup,
    SystemState_Operational,
    SystemState_Error,
} SystemState_t;

void BrainStatusIndicator_Run_OnInit(void);
void BrainStatusIndicator_Run_Update(void);

SystemState_t BrainStatusIndicator_Read_SystemState(void);
bool BrainStatusIndicator_Read_BluetoothControllerPresent(void);
void BrainStatusIndicator_Write_LedColor(rgb_t color);

#endif /* COMPONENT_BRAIN_STATUS_INDICATOR_H_ */

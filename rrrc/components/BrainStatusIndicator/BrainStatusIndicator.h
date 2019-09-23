#ifndef COMPONENT_BRAIN_STATUS_INDICATOR_H_
#define COMPONENT_BRAIN_STATUS_INDICATOR_H_

#ifndef COMPONENT_TYPES_BRAIN_STATUS_INDICATOR_H_
#define COMPONENT_TYPES_BRAIN_STATUS_INDICATOR_H_

#include "utils/color.h"
#include <stdbool.h>


typedef enum {
    SystemState_Startup,
    SystemState_Operational,
    SystemState_Error
} SystemState_t;

#endif /* COMPONENT_TYPES_BRAIN_STATUS_INDICATOR_H_ */

void BrainStatusIndicator_Run_Update(void);
void BrainStatusIndicator_Write_LedColor(const rgb_t value);
bool BrainStatusIndicator_Read_BluetoothControllerPresent(void);
SystemState_t BrainStatusIndicator_Read_SystemState(void);

#endif /* COMPONENT_BRAIN_STATUS_INDICATOR_H_ */

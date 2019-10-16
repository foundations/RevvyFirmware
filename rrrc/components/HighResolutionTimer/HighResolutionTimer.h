#ifndef COMPONENT_HIGH_RESOLUTION_TIMER_H_
#define COMPONENT_HIGH_RESOLUTION_TIMER_H_

#ifndef COMPONENT_TYPES_HIGH_RESOLUTION_TIMER_H_
#define COMPONENT_TYPES_HIGH_RESOLUTION_TIMER_H_

#include <float.h>
#include <stdint.h>


#endif /* COMPONENT_TYPES_HIGH_RESOLUTION_TIMER_H_ */

void HighResolutionTimer_Run_OnInit(void);
uint16_t HighResolutionTimer_Run_GetTickCount(void);
float HighResolutionTimer_Run_ToMs(uint32_t ticks);
uint16_t HighResolutionTimer_Constant_TicksInMs(void);

#endif /* COMPONENT_HIGH_RESOLUTION_TIMER_H_ */

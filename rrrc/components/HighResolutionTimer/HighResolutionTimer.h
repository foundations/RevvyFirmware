/*
 * HighResolutionTimer.h
 *
 * Created: 2019. 08. 23. 19:33:21
 *  Author: Dániel Buga
 */

#ifndef COMPONENT_HIGH_RESOLUTION_TIMER_H_
#define COMPONENT_HIGH_RESOLUTION_TIMER_H_

#include <stdint.h>

void HighResolutionTimer_Run_OnInit(void);
uint16_t HighResolutionTimer_Run_GetTickCount(void);

void HighResolutionTimer_Write_Resolution(uint16_t ticks_per_ms);

#endif /* COMPONENT_HIGH_RESOLUTION_TIMER_H_ */

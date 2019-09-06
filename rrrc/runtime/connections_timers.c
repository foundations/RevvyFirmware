/*
 * connections_timers.c
 *
 * Created: 2019. 08. 23. 19:45:29
 *  Author: Dániel Buga
 */ 
 
#include "../rrrc_worklogic.h"

static uint16_t hr_resolution = 0u;

void HighResolutionTimer_Write_Resolution(uint16_t ticks_per_ms)
{
    hr_resolution = ticks_per_ms;
}

uint16_t SensorPort_Read_CurrentTicks(void)
{
    return HighResolutionTimer_Run_GetTickCount();
}

float SensorPort_Call_GetMsFromTicks(uint16_t ticks)
{
    return ticks / hr_resolution;
}

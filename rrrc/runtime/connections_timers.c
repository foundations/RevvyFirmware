/*
 * connections_timers.c
 *
 * Created: 2019. 08. 23. 19:45:29
 *  Author: Dániel Buga
 */ 
 
#include "../rrrc_worklogic.h"

uint16_t SensorPort_Read_CurrentTicks(void)
{
    return HighResolutionTimer_Run_GetTickCount();
}

float SensorPort_Call_GetMsFromTicks(uint16_t ticks)
{
    return (float) ticks / HighResolutionTimer_Constant_TicksInMs();
}

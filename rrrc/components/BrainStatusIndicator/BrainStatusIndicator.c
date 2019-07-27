/*
 * BrainStatusIndicator.c
 *
 * Created: 03/05/2019 13:56:15
 *  Author: Dániel Buga
 */ 
#include "BrainStatusIndicator.h"
#include "rrrc_indication.h"
#include <stdint.h>

void BrainStatusIndicator_Run_OnInit(void)
{

}

void BrainStatusIndicator_Run_Update(void)
{
    switch (BrainStatusIndicator_Read_SystemState())
    {
        case SystemState_Startup:
            BrainStatusIndicator_Write_LedColor((rgb_t) LED_RED);
            break;

        case SystemState_Operational:
            if (BrainStatusIndicator_Read_BluetoothControllerPresent())
            {
                BrainStatusIndicator_Write_LedColor((rgb_t) LED_GREEN);
            }
            else
            {
                BrainStatusIndicator_Write_LedColor((rgb_t) LED_ORANGE);
            }
            break;

        case SystemState_Error:
        default:
            BrainStatusIndicator_Write_LedColor((rgb_t) LED_RED);
            break;
    }
}

__attribute__((weak))
SystemState_t BrainStatusIndicator_Read_SystemState(void)
{
    return SystemState_Operational;
}

__attribute__((weak))
bool BrainStatusIndicator_Read_BluetoothControllerPresent(void)
{
    return false;
}

__attribute__((weak))
void BrainStatusIndicator_Write_LedColor(rgb_t color)
{
    (void) color;
    /* nothing to do */
}
/*
 * BatteryCharger.c
 *
 * Created: 01/05/2019 08:49:16
 *  Author: Dániel Buga
 */ 

#include "BatteryCharger.h"

#include "rrrc_hal.h"

#include <stdbool.h>

#define ERROR_INCREMENT (20u)
#define MAX_ERROR_COUNT (220u)
#define ERROR_THRESHOLD (100u)

static ChargerState_t chargerState;
static ChargerState_t previousChargerState;
static uint8_t errorScore;

static ChargerState_t get_charger_state( void )
{
    bool isCharging = gpio_get_pin_level(CHARGER_STAT) == 0u;
    bool isCharged = gpio_get_pin_level(CHARGER_STBY) == 0u;
    
    if (isCharging)
    {
        return ChargerState_Charging;
    }
    else
    {
        if (isCharged)
        {
            return ChargerState_Charged;
        }
        else
        {
            return ChargerState_NotPluggedIn;
        }
    }
}

void BatteryCharger_Run_OnInit( void )
{
    gpio_set_pin_direction(CHARGER_STBY, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(CHARGER_STBY, GPIO_PULL_UP);
    gpio_set_pin_function(CHARGER_STBY, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_direction(CHARGER_STAT, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(CHARGER_STAT, GPIO_PULL_UP);
    gpio_set_pin_function(CHARGER_STAT, GPIO_PIN_FUNCTION_OFF);

    errorScore = 0u;
    
    chargerState = get_charger_state();
    previousChargerState = get_charger_state();
}

ChargerState_t BatteryCharger_Run_GetChargerState( void )
{
    return chargerState;
}

void BatteryCharger_Run_Update( void )
{
    ChargerState_t state = get_charger_state();

    if (state != previousChargerState)
    {
        if (errorScore < MAX_ERROR_COUNT)
        {
            errorScore += ERROR_INCREMENT;
        }
    }
    else if (errorScore > 0u)
    {
        --errorScore;
    }
    previousChargerState = state;

    if (errorScore > ERROR_THRESHOLD)
    {
        chargerState = ChargerState_Fault;
    }
    else
    {
        chargerState = state;
    }
}

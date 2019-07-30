/*
 * BatteryCharger.c
 *
 * Created: 01/05/2019 08:49:16
 *  Author: Dániel Buga
 */ 

#include "BatteryCharger.h"

#include "rrrc_hal.h"

#include <stdbool.h>

typedef enum 
{
    ISET2_100mA,
    ISET2_500mA,
    ISET2_Programmed
} ISET2_Mode_t;

static ChargerState_t chargerState;

static void set_iset2_mode( ISET2_Mode_t mode )
{
    /* according to datasheet, LOW = programmed current, HIGH = 500mA, floating = 100mA 
       Programmed current is set to ~540 mA using a 1k resistor */
    switch (mode)
    {
        default:
        case ISET2_100mA:
            gpio_set_pin_direction(SM_BAT_ISET2, GPIO_DIRECTION_IN);
            gpio_set_pin_function(SM_BAT_ISET2, GPIO_PIN_FUNCTION_OFF);
            break;

        case ISET2_500mA:
            gpio_set_pin_direction(SM_BAT_ISET2, GPIO_DIRECTION_OUT);
            gpio_set_pin_function(SM_BAT_ISET2, GPIO_PIN_FUNCTION_OFF);
            gpio_set_pin_level(SM_BAT_ISET2, true);
            break;

        case ISET2_Programmed:
            gpio_set_pin_direction(SM_BAT_ISET2, GPIO_DIRECTION_OUT);
            gpio_set_pin_function(SM_BAT_ISET2, GPIO_PIN_FUNCTION_OFF);
            gpio_set_pin_level(SM_BAT_ISET2, false);
            break;
    }
}

static ChargerState_t get_charger_state( void )
{
    bool isCharging = gpio_get_pin_level(SM_BAT_CHG) == 0u;
    bool powerGood = gpio_get_pin_level(SM_BAT_PG) == 0u;

    if (!powerGood)
    {
        if (isCharging)
        {
            /* Should not occur so let's just believe the charging signal */
            return ChargerState_Charging;
        }
        else
        {
            return ChargerState_NotPluggedIn;
        }
    }
    else
    {
        if (isCharging)
        {
            return ChargerState_Charging;
        }
        else
        {
            return ChargerState_Charged;
        }
    }
}

void BatteryCharger_Run_OnInit( void )
{
    gpio_set_pin_direction(SM_BAT_CHG, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SM_BAT_CHG, GPIO_PULL_UP);
    gpio_set_pin_function(SM_BAT_CHG, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_direction(SM_BAT_EN, GPIO_DIRECTION_IN);
    gpio_set_pin_function(SM_BAT_EN, GPIO_PIN_FUNCTION_OFF);

    gpio_set_pin_direction(SM_BAT_PG, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(SM_BAT_PG, GPIO_PULL_UP);
    gpio_set_pin_function(SM_BAT_PG, GPIO_PIN_FUNCTION_OFF);

    set_iset2_mode(ISET2_100mA);
    
    chargerState = get_charger_state();
}

void BatteryCharger_Run_EnableFastCharge( void )
{
    set_iset2_mode(ISET2_Programmed);
}

void BatteryCharger_Run_DisableFastCharge( void )
{
    set_iset2_mode(ISET2_100mA);
}

ChargerState_t BatteryCharger_Run_GetChargerState( void )
{
    return chargerState;
}

void BatteryCharger_Run_Update( void )
{
    chargerState = get_charger_state();
}

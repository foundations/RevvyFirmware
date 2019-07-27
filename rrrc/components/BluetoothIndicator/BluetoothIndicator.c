/*
 * BluetoothIndicator.c
 *
 * Created: 03/05/2019 13:37:22
 *  Author: Dániel Buga
 */ 

#include "BluetoothIndicator.h"

#include "rrrc_indication.h"
#include <stdint.h>

#define BLINK_PERIOD    26
#define BLINK_LENGTH     1

#define BLE_ON_COLOR                (rgb_t) LED_CYAN
#define BLE_OFF_COLOR               (rgb_t) LED_OFF
#define BLE_NOT_INITIALIZED_COLOR   (rgb_t) LED_OFF

static bool isConnected;
static uint32_t blinkTimer;

void BluetoothIndicator_Run_OnInit(void)
{
    isConnected = false;
    blinkTimer = 0u;
}

void BluetoothIndicator_Run_Update(void)
{
    if (BluetoothIndicator_Read_IsActive())
    {
        bool wasConnected = isConnected;
        isConnected = BluetoothIndicator_Read_IsConnected();
        if (isConnected == wasConnected)
        {
            if (isConnected)
            {
                BluetoothIndicator_Write_LedColor(BLE_ON_COLOR);
            }
            else
            {
                if (blinkTimer < BLINK_LENGTH)
                {
                    blinkTimer++;
                    BluetoothIndicator_Write_LedColor(BLE_ON_COLOR);
                }
                else if (blinkTimer < BLINK_PERIOD)
                {
                    blinkTimer++;
                    BluetoothIndicator_Write_LedColor(BLE_OFF_COLOR);
                }
                else
                {
                    blinkTimer = 0u;
                }
            }
        }
        else
        {
            blinkTimer = 0u;
        }
    }
    else
    {
        isConnected = false;
        blinkTimer = 0u;
        BluetoothIndicator_Write_LedColor(BLE_NOT_INITIALIZED_COLOR);
    }
}

__attribute__((weak))
bool BluetoothIndicator_Read_IsConnected(void)
{
    return false;
}

__attribute__((weak))
void BluetoothIndicator_Write_LedColor(rgb_t color)
{
    (void) color;
    /* nothing to do */
}

__attribute__((weak))
bool BluetoothIndicator_Read_IsActive(void)
{
    return false;
}

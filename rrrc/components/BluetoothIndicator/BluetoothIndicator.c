#include "BluetoothIndicator.h"
#include "utils.h"

/* Begin User Code Section: Declarations */
#include "rrrc_indication.h"
#include <stdbool.h>

#define BLINK_PERIOD    26
#define BLINK_LENGTH     1

#define BLE_ON_COLOR                (rgb_t) LED_CYAN
#define BLE_OFF_COLOR               (rgb_t) LED_OFF
#define BLE_NOT_INITIALIZED_COLOR   (rgb_t) LED_OFF

static bool isConnected;
static uint32_t blinkTimer;
/* End User Code Section: Declarations */

void BluetoothIndicator_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    isConnected = false;
    blinkTimer = 0u;
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void BluetoothIndicator_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    if (BluetoothIndicator_Read_ConnectionStatus() != BluetoothStatus_Inactive)
    {
        bool wasConnected = isConnected;
        isConnected = BluetoothIndicator_Read_ConnectionStatus() == BluetoothStatus_Connected;
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
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void BluetoothIndicator_Write_LedColor(const rgb_t value)
{
    /* Begin User Code Section: LedColor Start */

    /* End User Code Section: LedColor Start */
    /* Begin User Code Section: LedColor End */

    /* End User Code Section: LedColor End */
}

__attribute__((weak))
BluetoothStatus_t BluetoothIndicator_Read_ConnectionStatus(void)
{
    /* Begin User Code Section: ConnectionStatus Start */

    /* End User Code Section: ConnectionStatus Start */
    /* Begin User Code Section: ConnectionStatus End */

    /* End User Code Section: ConnectionStatus End */
    return BluetoothStatus_Inactive;
}

/*
 * BluetoothStatusObserver.c
 *
 * Created: 09/05/2019 11:54:17
 *  Author: Dániel Buga
 */ 
#include "BluetoothStatusObserver.h"

void BluetoothStatusObserver_Run_OnInit(void)
{
    BluetoothStatusObserver_Write_IsConnected(BluetoothStatus_Inactive);
}

Comm_Status_t BluetoothStatusObserver_SetBluetoothStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    Comm_Status_t status = Comm_Status_Ok;
    if  (commandSize != 1u)
    {
        status = Comm_Status_Error_PayloadLengthError;
    }
    else
    {
        switch (commandPayload[0])
        {
            case 0u:
                BluetoothStatusObserver_Write_IsConnected(BluetoothStatus_NotConnected);
                break;

            case 1u:
                BluetoothStatusObserver_Write_IsConnected(BluetoothStatus_Connected);
                break;

            default:
                status = Comm_Status_Error_CommandError;
                break;
        }
    }

    return status;
}

__attribute__((weak))
void BluetoothStatusObserver_Write_IsConnected(BluetoothStatus_t status)
{
    /* nothing to do */
}

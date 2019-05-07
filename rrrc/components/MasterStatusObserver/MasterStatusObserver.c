/*
 * MasterStatusObserver.c
 *
 * Created: 2019. 05. 07. 21:57:32
 *  Author: Dániel Buga
 */ 

#include "MasterStatusObserver.h"

void MasterStatusObserver_Run_OnInit(void)
{
    MasterStatusObserver_Write_MasterStatus(MasterStatus_Unknown);
}

Comm_Status_t MasterStatusObserver_SetMasterStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
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
                MasterStatusObserver_Write_MasterStatus(MasterStatus_Unknown);
                break;

            case 1u:
                MasterStatusObserver_Write_MasterStatus(MasterStatus_Operational);
                break;

            case 2u:
                MasterStatusObserver_Write_MasterStatus(MasterStatus_Controlled);
                break;

            default:
                status = Comm_Status_Error_CommandError;
                break;
        }
    }

    return status;
}

__attribute__((weak))
void MasterStatusObserver_Write_MasterStatus(MasterStatus_t status)
{
    /* nothing to do */
}

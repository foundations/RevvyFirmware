#include "utils_assert.h"
#include "MasterStatusObserver.h"
#include "utils.h"

/* Begin User Code Section: Declarations */
Comm_Status_t MasterStatusObserver_SetMasterStatus_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    (void) response;
    (void) responseCount;
    (void) responseBufferSize;

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
/* End User Code Section: Declarations */

void MasterStatusObserver_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    MasterStatusObserver_Write_MasterStatus(MasterStatus_Unknown);
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

__attribute__((weak))
void MasterStatusObserver_Write_MasterStatus(const MasterStatus_t value)
{
    (void) value;
    /* Begin User Code Section: MasterStatus Start */

    /* End User Code Section: MasterStatus Start */
    /* Begin User Code Section: MasterStatus End */

    /* End User Code Section: MasterStatus End */
}

#include "utils.h"
#include "utils_assert.h"
#include "DriveRequestMultiplexer.h"

/* Begin User Code Section: Declarations */

/* End User Code Section: Declarations */

void DriveRequestMultiplexer_Run_OnUpdate(void)
{
    /* Begin User Code Section: OnUpdate Start */
    DriveRequest_t request;
    for (size_t i = 0u; i < 6u; i++)
    {
        if (DriveRequestMultiplexer_Read_IsDrivetrainMotor(i))
        {
            DriveRequestMultiplexer_Read_DrivetrainDriveRequest(i, &request);
        }
        else
        {
            DriveRequestMultiplexer_Read_UserDriveRequest(i, &request);
        }
        DriveRequestMultiplexer_Write_AppliedDriveRequest(i, &request);
    }
    /* End User Code Section: OnUpdate Start */
    /* Begin User Code Section: OnUpdate End */

    /* End User Code Section: OnUpdate End */
}

__attribute__((weak))
void DriveRequestMultiplexer_Write_AppliedDriveRequest(uint32_t index, const DriveRequest_t* value)
{
    (void) index;
    ASSERT(value != NULL);
    ASSERT(index < 6);
    /* Begin User Code Section: AppliedDriveRequest Start */

    /* End User Code Section: AppliedDriveRequest Start */
    /* Begin User Code Section: AppliedDriveRequest End */

    /* End User Code Section: AppliedDriveRequest End */
}

__attribute__((weak))
void DriveRequestMultiplexer_Read_DrivetrainDriveRequest(uint32_t index, DriveRequest_t* value)
{
    ASSERT(value != NULL);
    ASSERT(index < 6);
    /* Begin User Code Section: DrivetrainDriveRequest Start */

    /* End User Code Section: DrivetrainDriveRequest Start */
    *value = (DriveRequest_t) { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = (DriveRequest_RequestValue_t) { .power = 0 } };
    /* Begin User Code Section: DrivetrainDriveRequest End */

    /* End User Code Section: DrivetrainDriveRequest End */
}

__attribute__((weak))
bool DriveRequestMultiplexer_Read_IsDrivetrainMotor(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: IsDrivetrainMotor Start */

    /* End User Code Section: IsDrivetrainMotor Start */
    /* Begin User Code Section: IsDrivetrainMotor End */

    /* End User Code Section: IsDrivetrainMotor End */
    return false;
}

__attribute__((weak))
void DriveRequestMultiplexer_Read_UserDriveRequest(uint32_t index, DriveRequest_t* value)
{
    ASSERT(value != NULL);
    ASSERT(index < 6);
    /* Begin User Code Section: UserDriveRequest Start */

    /* End User Code Section: UserDriveRequest Start */
    *value = (DriveRequest_t) { .power_limit = 0.0f, .speed_limit = 0.0f, .request_type = DriveRequest_RequestType_Power, .request = (DriveRequest_RequestValue_t) { .power = 0 } };
    /* Begin User Code Section: UserDriveRequest End */

    /* End User Code Section: UserDriveRequest End */
}

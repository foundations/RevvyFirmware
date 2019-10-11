#include "utils.h"
#include "YawAngleTracker.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include <math.h>
/* End User Code Section: Declarations */

void YawAngleTracker_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */

    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void YawAngleTracker_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    Orientation3D_t orientation;
    YawAngleTracker_Read_Orientation(&orientation);

    YawAngleTracker_Write_YawAngle(orientation.yaw * 180.0f / (float) M_PI);
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void YawAngleTracker_Write_YawAngle(const float value)
{
    (void) value;
    /* Begin User Code Section: YawAngle Start */

    /* End User Code Section: YawAngle Start */
    /* Begin User Code Section: YawAngle End */

    /* End User Code Section: YawAngle End */
}

__attribute__((weak))
void YawAngleTracker_Read_Orientation(Orientation3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: Orientation Start */

    /* End User Code Section: Orientation Start */
    *value = (Orientation3D_t) { .pitch = 0.0f, .roll = 0.0f, .yaw = 0.0f };
    /* Begin User Code Section: Orientation End */

    /* End User Code Section: Orientation End */
}

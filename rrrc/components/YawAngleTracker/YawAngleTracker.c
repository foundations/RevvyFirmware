#include "YawAngleTracker.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
static float pinned_angle;
/* End User Code Section: Declarations */

void YawAngleTracker_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    pinned_angle = 0.0f;
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void YawAngleTracker_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    Quaternion_t orientation;
    YawAngleTracker_Read_Orientation(&orientation);

    Orientation3D_t angles = YawAngleTracker_Call_ToEulerAngles(orientation);

    YawAngleTracker_Write_YawAngle(angles.yaw);
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
Orientation3D_t YawAngleTracker_Call_ToEulerAngles(Quaternion_t orientation)
{
    /* Begin User Code Section: ToEulerAngles Start */

    /* End User Code Section: ToEulerAngles Start */
    /* Begin User Code Section: ToEulerAngles End */

    /* End User Code Section: ToEulerAngles End */
    return (Orientation3D_t) { .pitch = 0.0f, .roll = 0.0f, .yaw = 0.0f };
}

__attribute__((weak))
void YawAngleTracker_Write_YawAngle(const float value)
{
    /* Begin User Code Section: YawAngle Start */

    /* End User Code Section: YawAngle Start */
    /* Begin User Code Section: YawAngle End */

    /* End User Code Section: YawAngle End */
}

__attribute__((weak))
void YawAngleTracker_Read_Orientation(Quaternion_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: Orientation Start */

    /* End User Code Section: Orientation Start */
    *value = (Quaternion_t) { .q0 = 0.0f, .q1 = 0.0f, .q2 = 0.0f, .q3 = 0.0f };
    /* Begin User Code Section: Orientation End */

    /* End User Code Section: Orientation End */
}

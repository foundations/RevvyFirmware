#include "IMUMovementDetector.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include <math.h>

#define IDLE_SENSITIVITY        ((float) 2.0f)
#define IDLE_NUM_SAMPLES        ((uint32_t) 200u)

static Vector3D_t currentMidValue;
static uint32_t samplesInCurrentBand;

static bool is_close_to(const Vector3D_t vector, const Vector3D_t reference, float threshold)
{
    return fabsf(vector.x - reference.x) <= threshold
            && fabsf(vector.y - reference.y) <= threshold
            && fabsf(vector.z - reference.z) <= threshold;
}

/* End User Code Section: Declarations */

void IMUMovementDetector_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    samplesInCurrentBand = 0u;
    currentMidValue = (Vector3D_t) {0.0f, 0.0f, 0.0f};
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void IMUMovementDetector_Run_OnUpdate(void)
{
    /* Begin User Code Section: OnUpdate Start */
    Vector3D_t angularSpeed;
    if (IMUMovementDetector_Read_AngularSpeeds(&angularSpeed) != QueueStatus_Empty)
    {
        if (is_close_to(angularSpeed, currentMidValue, IDLE_SENSITIVITY))
        {
            if (samplesInCurrentBand < IDLE_NUM_SAMPLES)
            {
                samplesInCurrentBand++;
                if (samplesInCurrentBand == IDLE_NUM_SAMPLES)
                {
                    IMUMovementDetector_Write_IsMoving(false);
                }
            }
        }
        else
        {
            samplesInCurrentBand = 0u;
            currentMidValue = angularSpeed;
            IMUMovementDetector_Write_IsMoving(true);
        }
    }
    /* End User Code Section: OnUpdate Start */
    /* Begin User Code Section: OnUpdate End */

    /* End User Code Section: OnUpdate End */
}

__attribute__((weak))
void IMUMovementDetector_Write_IsMoving(const bool value)
{
    (void) value;
    /* Begin User Code Section: IsMoving Start */

    /* End User Code Section: IsMoving Start */
    /* Begin User Code Section: IsMoving End */

    /* End User Code Section: IsMoving End */
}

__attribute__((weak))
QueueStatus_t IMUMovementDetector_Read_Acceleration(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: Acceleration Start */

    /* End User Code Section: Acceleration Start */
    /* Begin User Code Section: Acceleration End */

    /* End User Code Section: Acceleration End */
    return QueueStatus_Empty;
}

__attribute__((weak))
QueueStatus_t IMUMovementDetector_Read_AngularSpeeds(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: AngularSpeeds Start */

    /* End User Code Section: AngularSpeeds Start */
    /* Begin User Code Section: AngularSpeeds End */

    /* End User Code Section: AngularSpeeds End */
    return QueueStatus_Empty;
}

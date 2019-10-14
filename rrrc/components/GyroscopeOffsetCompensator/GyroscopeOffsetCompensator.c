#include "GyroscopeOffsetCompensator.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

static bool offset_calibrated;
static Vector3D_t averageAngularSpeed;
static Vector3D_t sumAngularSpeed;
static uint32_t averageAngularSpeedSamples;

#define AVERAGE_NUM_SAMPLES     ((uint32_t) 1000u)

static void restart_averaging(void)
{
    /* start a new calibration immediately */
    averageAngularSpeedSamples = 0u;

    /* only reset sum, average will be used for compensation */
    sumAngularSpeed = (Vector3D_t) {0.0f, 0.0f, 0.0f};
}
/* End User Code Section: Declarations */

void GyroscopeOffsetCompensator_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    offset_calibrated = false;

    restart_averaging();

    averageAngularSpeed = (Vector3D_t) {0.0f, 0.0f, 0.0f};
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void GyroscopeOffsetCompensator_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    Vector3D_t angularSpeed;
    if (GyroscopeOffsetCompensator_Read_AngularSpeeds(&angularSpeed) != QueueStatus_Empty)
    {
        if (GyroscopeOffsetCompensator_Read_IsMoving())
        {
            restart_averaging();
        }
        else
        {
            if (averageAngularSpeedSamples < AVERAGE_NUM_SAMPLES)
            {
                sumAngularSpeed.x += angularSpeed.x;
                sumAngularSpeed.y += angularSpeed.y;
                sumAngularSpeed.z += angularSpeed.z;

                ++averageAngularSpeedSamples;

                if (averageAngularSpeedSamples == AVERAGE_NUM_SAMPLES)
                {
                    averageAngularSpeed.x = sumAngularSpeed.x / AVERAGE_NUM_SAMPLES;
                    averageAngularSpeed.y = sumAngularSpeed.y / AVERAGE_NUM_SAMPLES;
                    averageAngularSpeed.z = sumAngularSpeed.z / AVERAGE_NUM_SAMPLES;

                    offset_calibrated = true;

                    restart_averaging();
                }
            }
        }

        if (offset_calibrated)
        {
            const Vector3D_t output =
            {
                .x = angularSpeed.x - averageAngularSpeed.x,
                .y = angularSpeed.y - averageAngularSpeed.y,
                .z = angularSpeed.z - averageAngularSpeed.z
            };

            GyroscopeOffsetCompensator_Write_CompensatedAngularSpeeds(&output);
        }
    }
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void GyroscopeOffsetCompensator_Write_CompensatedAngularSpeeds(const Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: CompensatedAngularSpeeds Start */

    /* End User Code Section: CompensatedAngularSpeeds Start */
    /* Begin User Code Section: CompensatedAngularSpeeds End */

    /* End User Code Section: CompensatedAngularSpeeds End */
}

__attribute__((weak))
QueueStatus_t GyroscopeOffsetCompensator_Read_AngularSpeeds(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: AngularSpeeds Start */

    /* End User Code Section: AngularSpeeds Start */
    /* Begin User Code Section: AngularSpeeds End */

    /* End User Code Section: AngularSpeeds End */
    return QueueStatus_Empty;
}

__attribute__((weak))
bool GyroscopeOffsetCompensator_Read_IsMoving(void)
{
    /* Begin User Code Section: IsMoving Start */

    /* End User Code Section: IsMoving Start */
    /* Begin User Code Section: IsMoving End */

    /* End User Code Section: IsMoving End */
    return false;
}

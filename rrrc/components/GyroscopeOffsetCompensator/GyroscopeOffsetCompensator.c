#include "GyroscopeOffsetCompensator.h"

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

static bool offset_calibrated;
static Vector3D_t averageAngularSpeed;
static Vector3D_t sumAngularSpeed;
static Vector3D_t currentMidValue;
static uint32_t averageAngularSpeedSamples;
static uint32_t samplesInCurrentBand;

#define IDLE_SENSITIVITY        ((float) 2.0f)
#define AVERAGE_NUM_SAMPLES     ((uint32_t) 1000u)
#define IDLE_NUM_SAMPLES        ((uint32_t) 200u)

/* runnables */
void GyroscopeOffsetCompensator_Run_OnInit(void)
{
    offset_calibrated = false;

    averageAngularSpeedSamples = 0u;

    sumAngularSpeed.x = 0.0f;
    sumAngularSpeed.y = 0.0f;
    sumAngularSpeed.z = 0.0f;

    averageAngularSpeed.x = 0.0f;
    averageAngularSpeed.y = 0.0f;
    averageAngularSpeed.z = 0.0f;

    currentMidValue.x = 0.0f;
    currentMidValue.y = 0.0f;
    currentMidValue.z = 0.0f;

    samplesInCurrentBand = 0u;
}

void GyroscopeOffsetCompensator_Run_Update(void)
{
    Vector3D_t angularSpeed;
    GyroscopeOffsetCompensator_Read_AngularSpeeds(&angularSpeed);
    // if () TODO make this queued
    {
        if (offset_calibrated)
        {
            Vector3D_t output;
            output.x = angularSpeed.x - averageAngularSpeed.x;
            output.y = angularSpeed.y - averageAngularSpeed.y;
            output.z = angularSpeed.z - averageAngularSpeed.z;

            GyroscopeOffsetCompensator_Write_CompensatedAngularSpeeds(&output);
        }

        if (fabsf(currentMidValue.x - angularSpeed.x) > IDLE_SENSITIVITY
            || fabsf(currentMidValue.y - angularSpeed.y) > IDLE_SENSITIVITY
            || fabsf(currentMidValue.z - angularSpeed.z) > IDLE_SENSITIVITY)
        {
            samplesInCurrentBand = 0u;
            currentMidValue = angularSpeed;
            averageAngularSpeedSamples = 0u;
        }
        else
        {
            if (samplesInCurrentBand == IDLE_NUM_SAMPLES)
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
                    }
                }
            }
            else
            {
                samplesInCurrentBand++;
                
                if (samplesInCurrentBand == IDLE_NUM_SAMPLES)
                {
                    sumAngularSpeed.x = 0.0f;
                    sumAngularSpeed.y = 0.0f;
                    sumAngularSpeed.z = 0.0f;
                    
                    averageAngularSpeedSamples = 0u;
                }
            }
        }
    }
}

__attribute__((weak))
void GyroscopeOffsetCompensator_Read_AngularSpeeds(Vector3D_t* value)
{
    *value = (Vector3D_t) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
}

__attribute__((weak))
void GyroscopeOffsetCompensator_Write_CompensatedAngularSpeeds(const Vector3D_t* value)
{
    (void) value;
}

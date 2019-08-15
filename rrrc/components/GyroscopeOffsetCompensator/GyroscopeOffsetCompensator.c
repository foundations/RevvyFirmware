/*
 * GyroscopeOffsetCompensator.c
 *
 * Created: 2019. 08. 15. 14:40:49
 *  Author: Dániel Buga
 */ 

#include "GyroscopeOffsetCompensator.h"

#include <math.h>

static bool offset_calibrated;
static AngularSpeedVector_t averageAngularSpeed;
static AngularSpeedVector_t sumAngularSpeed;
static AngularSpeedVector_t currentMidValue;
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
    AngularSpeedVector_t angularSpeed;
    if (GyroscopeOffsetCompensator_Read_AngularSpeeds(&angularSpeed))
    {
        if (offset_calibrated)
        {
            AngularSpeedVector_t output;
            output.x = angularSpeed.x - averageAngularSpeed.x;
            output.y = angularSpeed.y - averageAngularSpeed.y;
            output.z = angularSpeed.z - averageAngularSpeed.z;

            GyroscopeOffsetCompensator_Write_AngularSpeeds(&output);
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

/* ports */
__attribute__((weak))
bool GyroscopeOffsetCompensator_Read_AngularSpeeds(AngularSpeedVector_t* angularSpeed)
{
    (void) angularSpeed;
    
    return false;
}

__attribute__((weak))
void GyroscopeOffsetCompensator_Write_AngularSpeeds(const AngularSpeedVector_t* angularSpeed)
{
    (void) angularSpeed;
}

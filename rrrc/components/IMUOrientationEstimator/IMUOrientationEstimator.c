#include "IMUOrientationEstimator.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
static Quaternion_t orientation;
static float beta = 0.1f; // < TODO: tune if necessary

/**
 * Fast inverse square-root
 * See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
 */
static float invSqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

static Quaternion_t madgwick_imu(const float sampleFreq, const Vector3D_t acceleration, const Vector3D_t angularSpeed, const Quaternion_t previous)
{
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    float q0 = previous.q0;
    float q1 = previous.q1;
    float q2 = previous.q2;
    float q3 = previous.q3;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-q1 * angularSpeed.x - q2 * angularSpeed.y - q3 * angularSpeed.z);
    qDot2 = 0.5f * (q0 * angularSpeed.x + q2 * angularSpeed.z - q3 * angularSpeed.y);
    qDot3 = 0.5f * (q0 * angularSpeed.y - q1 * angularSpeed.z + q3 * angularSpeed.x);
    qDot4 = 0.5f * (q0 * angularSpeed.z + q1 * angularSpeed.y - q2 * angularSpeed.x);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if (!((acceleration.x == 0.0f) && (acceleration.y == 0.0f) && (acceleration.z == 0.0f))) {

        // Normalise accelerometer measurement
        recipNorm = invSqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y + acceleration.z * acceleration.z);
        float x = acceleration.x * recipNorm;
        float y = acceleration.y * recipNorm;
        float z = acceleration.z * recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * q0;
        _2q1 = 2.0f * q1;
        _2q2 = 2.0f * q2;
        _2q3 = 2.0f * q3;
        _4q0 = 4.0f * q0;
        _4q1 = 4.0f * q1;
        _4q2 = 4.0f * q2;
        _8q1 = 8.0f * q1;
        _8q2 = 8.0f * q2;
        q0q0 = q0 * q0;
        q1q1 = q1 * q1;
        q2q2 = q2 * q2;
        q3q3 = q3 * q3;

        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * x + _4q0 * q1q1 - _2q1 * y;
        s1 = _4q1 * q3q3 - _2q3 * x + 4.0f * q0q0 * q1 - _2q0 * y - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * z;
        s2 = 4.0f * q0q0 * q2 + _2q0 * x + _4q2 * q3q3 - _2q3 * y - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * z;
        s3 = 4.0f * q1q1 * q3 - _2q1 * x + 4.0f * q2q2 * q3 - _2q2 * y;

        // normalise step magnitude
        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    q0 += qDot1 * (1.0f / sampleFreq);
    q1 += qDot2 * (1.0f / sampleFreq);
    q2 += qDot3 * (1.0f / sampleFreq);
    q3 += qDot4 * (1.0f / sampleFreq);

    // Normalise quaternion
    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
    
    return (Quaternion_t) {q0, q1, q2, q3};
}

/* End User Code Section: Declarations */

void IMUOrientationEstimator_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    orientation = (Quaternion_t) {1.0f, 0.0f, 0.0f, 0.0f};
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void IMUOrientationEstimator_Run_OnUpdate(void)
{
    /* Begin User Code Section: OnUpdate Start */
    Vector3D_t acceleration;
    const float sampleFreq = 1.0f / IMUOrientationEstimator_Read_SampleTime();
    while (IMUOrientationEstimator_Read_Acceleration(&acceleration) == QueueStatus_Ok)
    {
        Vector3D_t angularSpeed;
        if (IMUOrientationEstimator_Read_AngularSpeeds(&angularSpeed))
        {
            orientation = madgwick_imu(sampleFreq, acceleration, angularSpeed, orientation);

            IMUOrientationEstimator_Write_Orientation(&orientation);
        }
    }
    /* End User Code Section: OnUpdate Start */
    /* Begin User Code Section: OnUpdate End */

    /* End User Code Section: OnUpdate End */
}

__attribute__((weak))
void IMUOrientationEstimator_Write_Orientation(const Quaternion_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: Orientation Start */

    /* End User Code Section: Orientation Start */
    /* Begin User Code Section: Orientation End */

    /* End User Code Section: Orientation End */
}

__attribute__((weak))
void IMUOrientationEstimator_Write_OrientationAngles(const Orientation3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: OrientationAngles Start */

    /* End User Code Section: OrientationAngles Start */
    /* Begin User Code Section: OrientationAngles End */

    /* End User Code Section: OrientationAngles End */
}

__attribute__((weak))
QueueStatus_t IMUOrientationEstimator_Read_Acceleration(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: Acceleration Start */

    /* End User Code Section: Acceleration Start */
    /* Begin User Code Section: Acceleration End */

    /* End User Code Section: Acceleration End */
    return QueueStatus_Empty;
}

__attribute__((weak))
QueueStatus_t IMUOrientationEstimator_Read_AngularSpeeds(Vector3D_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: AngularSpeeds Start */

    /* End User Code Section: AngularSpeeds Start */
    /* Begin User Code Section: AngularSpeeds End */

    /* End User Code Section: AngularSpeeds End */
    return QueueStatus_Empty;
}

__attribute__((weak))
float IMUOrientationEstimator_Read_SampleTime(void)
{
    /* Begin User Code Section: SampleTime Start */

    /* End User Code Section: SampleTime Start */
    /* Begin User Code Section: SampleTime End */

    /* End User Code Section: SampleTime End */
    return 0.0f;
}

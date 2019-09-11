/*
 * connections_imu.c
 *
 * Created: 2019. 08. 14. 16:52:56
 *  Author: Dániel Buga
 */

#include "../rrrc_worklogic.h"

#include <math.h>

static Vector3D_t raw_acceleration;
static bool has_new_acceleration;
static Vector3D_t calibrated_rotation;
static bool has_new_calibrated_rotation;

static float current_yaw_angle;
static float current_relative_yaw_angle;

void IMU_Write_AccelerometerSample(const Vector3D_t* sample)
{
    raw_acceleration = *sample;
    has_new_acceleration = true;
}

void IMU_Write_RawGyroscopeSample(const IMU_RawSample_t* sample)
{
    UpdateMcuStatus_Gyroscope(sample);
}

void GyroscopeOffsetCompensator_Write_CompensatedAngularSpeeds(const Vector3D_t* angularSpeed)
{
    calibrated_rotation = *angularSpeed;
    has_new_calibrated_rotation = true;
}

bool YawAngleTracker_Read_AngularSpeedZ(float* angularSpeed)
{
    if (has_new_calibrated_rotation)
    {
        *angularSpeed = calibrated_rotation.z;
        has_new_calibrated_rotation = false;
        return true;
    }

    return false;
}

void YawAngleTracker_Write_YawAngle(float angle, float relativeAngle)
{
    current_yaw_angle = angle;
    current_relative_yaw_angle = relativeAngle;

    UpdateMcuStatus_YawAngle((int32_t) lroundf(angle), (int32_t) lroundf(relativeAngle));
}

float DriveTrain_Read_YawAngle(void)
{
    return current_yaw_angle;
}

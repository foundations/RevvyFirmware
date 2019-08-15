/*
 * connections_imu.c
 *
 * Created: 2019. 08. 14. 16:52:56
 *  Author: Dániel Buga
 */

#include "../rrrc_worklogic.h"

static IMU_AxlSample_t raw_acceleration;
static bool has_new_acceleration;
static IMU_GyroSample_t raw_rotation;
static bool has_new_rotation;

static float current_yaw_angle;
static float current_relative_yaw_angle;

void IMU_Write_AccelerometerSample(const IMU_AxlSample_t* sample)
{
    raw_acceleration = *sample;
    has_new_acceleration = true;
}

void IMU_Write_GyroscopeSample(const IMU_GyroSample_t* sample)
{
    raw_rotation = *sample;
    has_new_rotation = true;
}

bool YawAngleTracker_Read_AngularSpeedZ(float* angularSpeed)
{
    if (has_new_rotation)
    {
        *angularSpeed = raw_rotation.z;
        has_new_rotation = false;
        return true;
    }

    return false;
}

float YawAngleTracker_Read_SampleTime(void)
{
    return 1.0f / 416;
}

void YawAngleTracker_Write_YawAngle(float angle)
{
    current_yaw_angle = angle;
}

void YawAngleTracker_Write_RelativeYawAngle(float angle)
{
    current_relative_yaw_angle = angle;
}

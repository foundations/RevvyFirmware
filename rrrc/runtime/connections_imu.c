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

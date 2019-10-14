/*
 * connections_imu.c
 *
 * Created: 2019. 08. 14. 16:52:56
 *  Author: D�niel Buga
 */

#include "../rrrc_worklogic.h"
static float current_yaw_angle;

#include <math.h>

void IMU_Write_RawGyroscopeSample(const IMU_RawSample_t* sample)
{
    UpdateMcuStatus_Gyroscope(sample);
}

void YawAngleTracker_Write_YawAngle(const float angle)
{
    current_yaw_angle = angle;
    UpdateMcuStatus_YawAngle((int32_t) lroundf(angle), 0);
}

float DriveTrain_Read_YawAngle(void)
{
    return current_yaw_angle;
}

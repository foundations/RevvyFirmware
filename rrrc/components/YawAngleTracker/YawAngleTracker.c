/*
 * YawAngleTracker.c
 *
 * Created: 2019. 08. 15. 10:24:22
 *  Author: Dániel Buga
 */ 

#include "YawAngleTracker.h"

static float _pinned_angle;
static float _current_angle;

/* runnables */
void YawAngleTracker_Run_OnInit(void)
{
    _pinned_angle = 0.0f;
    _current_angle = 0.0f;
}

void YawAngleTracker_Run_PinCurrentAngle(void)
{
    _pinned_angle = _current_angle;
}

void YawAngleTracker_Run_Update(void)
{
    float angleSpeed;
    if (YawAngleTracker_Read_AngularSpeedZ(&angleSpeed))
    {
        float ts = YawAngleTracker_Read_SampleTime();
        do
        {
            _current_angle += angleSpeed * ts;
        }
        while (YawAngleTracker_Read_AngularSpeedZ(&angleSpeed));
        
        YawAngleTracker_Write_YawAngle(_current_angle ,_current_angle - _pinned_angle);
    }
}

/* ports */
__attribute__((weak))
bool YawAngleTracker_Read_AngularSpeedZ(float* angularSpeed)
{
    *angularSpeed = 0.0f;
    return false;
}

__attribute__((weak))
float YawAngleTracker_Read_SampleTime(void)
{
    return 0.0f;
}

__attribute__((weak))
void YawAngleTracker_Write_YawAngle(float angle, float relativeAngle)
{
    (void) angle;
    (void) relativeAngle;
}

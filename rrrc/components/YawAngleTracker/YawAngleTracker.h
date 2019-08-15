/*
 * YawAngleTracker.h
 *
 * Created: 2019. 08. 15. 10:12:57
 *  Author: Dániel Buga
 */ 

#ifndef COMPONENT_YAW_ANGLE_TRACKER_H_
#define COMPONENT_YAW_ANGLE_TRACKER_H_

#include <stdint.h>
#include <stdbool.h>

/* runnables */
void YawAngleTracker_Run_OnInit(void);
void YawAngleTracker_Run_PinCurrentAngle(void);
void YawAngleTracker_Run_Update(void);

/* ports */
bool YawAngleTracker_Read_AngularSpeedZ(float* angularSpeed);
float YawAngleTracker_Read_SampleTime(void);
void YawAngleTracker_Write_YawAngle(float angle);
void YawAngleTracker_Write_RelativeYawAngle(float angle);

#endif /* YAWANGLETRACKER_H_ */

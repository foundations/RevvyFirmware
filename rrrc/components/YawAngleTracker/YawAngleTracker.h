#ifndef COMPONENT_YAW_ANGLE_TRACKER_H_
#define COMPONENT_YAW_ANGLE_TRACKER_H_

#ifndef COMPONENT_TYPES_YAW_ANGLE_TRACKER_H_
#define COMPONENT_TYPES_YAW_ANGLE_TRACKER_H_

#include <float.h>


typedef struct {
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion_t;

typedef struct {
    float pitch;
    float roll;
    float yaw;
} Orientation3D_t;

#endif /* COMPONENT_TYPES_YAW_ANGLE_TRACKER_H_ */

void YawAngleTracker_Run_OnInit(void);
void YawAngleTracker_Run_Update(void);
Orientation3D_t YawAngleTracker_Call_ToEulerAngles(Quaternion_t orientation);
void YawAngleTracker_Write_YawAngle(const float value);
void YawAngleTracker_Read_Orientation(Quaternion_t* value);

#endif /* COMPONENT_YAW_ANGLE_TRACKER_H_ */

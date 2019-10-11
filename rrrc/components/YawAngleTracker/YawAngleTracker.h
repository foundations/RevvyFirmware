#ifndef COMPONENT_YAW_ANGLE_TRACKER_H_
#define COMPONENT_YAW_ANGLE_TRACKER_H_

#ifndef COMPONENT_TYPES_YAW_ANGLE_TRACKER_H_
#define COMPONENT_TYPES_YAW_ANGLE_TRACKER_H_

#include <float.h>


typedef struct {
    float pitch;
    float roll;
    float yaw;
} Orientation3D_t;

#endif /* COMPONENT_TYPES_YAW_ANGLE_TRACKER_H_ */

void YawAngleTracker_Run_OnInit(void);
void YawAngleTracker_Run_Update(void);
void YawAngleTracker_Write_YawAngle(const float value);
void YawAngleTracker_Read_Orientation(Orientation3D_t* value);

#endif /* COMPONENT_YAW_ANGLE_TRACKER_H_ */

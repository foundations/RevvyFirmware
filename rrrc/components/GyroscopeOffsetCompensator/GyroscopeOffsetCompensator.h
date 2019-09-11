#ifndef COMPONENT_GYROSCOPE_OFFSET_COMPENSATOR_H_
#define COMPONENT_GYROSCOPE_OFFSET_COMPENSATOR_H_

#ifndef COMPONENT_TYPES_GYROSCOPE_OFFSET_COMPENSATOR_H_
#define COMPONENT_TYPES_GYROSCOPE_OFFSET_COMPENSATOR_H_

#include <float.h>

typedef struct {
    float x;
    float y;
    float z;
} Vector3D_t;

#endif /* COMPONENT_TYPES_GYROSCOPE_OFFSET_COMPENSATOR_H_ */

void GyroscopeOffsetCompensator_Run_OnInit(void);
void GyroscopeOffsetCompensator_Run_Update(void);
void GyroscopeOffsetCompensator_Read_AngularSpeeds(Vector3D_t* value);
void GyroscopeOffsetCompensator_Write_CompensatedAngularSpeeds(const Vector3D_t* value);

#endif /* COMPONENT_GYROSCOPE_OFFSET_COMPENSATOR_H_ */

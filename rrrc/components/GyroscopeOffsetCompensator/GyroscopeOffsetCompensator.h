/*
 * GyroscopeOffsetCompensator.h
 *
 * Created: 2019. 08. 15. 14:41:10
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_GYROSCOPE_OFFSET_COMPENSATOR_H_
#define COMPONENT_GYROSCOPE_OFFSET_COMPENSATOR_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
    float x;
    float y;
    float z;
} AngularSpeedVector_t;

/* runnables */
void GyroscopeOffsetCompensator_Run_OnInit(void);
void GyroscopeOffsetCompensator_Run_Update(void);

/* ports */
bool GyroscopeOffsetCompensator_Read_AngularSpeeds(AngularSpeedVector_t* angularSpeed);
void GyroscopeOffsetCompensator_Write_AngularSpeeds(const AngularSpeedVector_t* angularSpeed);

#endif /* COMPONENT_GYROSCOPE_OFFSET_COMPENSATOR_H_ */

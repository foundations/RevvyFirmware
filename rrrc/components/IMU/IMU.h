/*
 * IMU.h
 *
 * Created: 2019. 07. 25. 16:31:00
 *  Author: Dániel Buga
 */

#ifndef COMPONENT_IMU_H_
#define COMPONENT_IMU_H_

#include <stdint.h>

typedef struct {
    float x;
    float y;
    float z;
} IMU_AxlSample_t;

typedef struct {
    float x;
    float y;
    float z;
} IMU_GyroSample_t;

void IMU_Run_OnInit(void);
void IMU_Run_OnUpdate(void);

void IMU_Write_AccelerometerSample(const IMU_AxlSample_t* sample);
void IMU_Write_GyroscopeSample(const IMU_GyroSample_t* sample);

#endif /* COMPONENT_IMU_H_ */

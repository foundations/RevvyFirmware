/*
 * IMU.h
 *
 * Created: 2019. 07. 25. 16:31:00
 *  Author: Dániel Buga
 */

#ifndef COMPONENT_IMU_H_
#define COMPONENT_IMU_H_

#include <stdint.h>

typedef struct 
{
    int16_t x;
    int16_t y;
    int16_t z;
} IMU_RawSample_t;

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
void IMU_Write_RawAccelerometerSample(const IMU_RawSample_t* sample);
void IMU_Write_RawGyroscopeSample(const IMU_RawSample_t* sample);

void IMU_Call_LogError(void);

#endif /* COMPONENT_IMU_H_ */

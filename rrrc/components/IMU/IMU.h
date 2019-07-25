/*
 * IMU.h
 *
 * Created: 2019. 07. 25. 16:31:00
 *  Author: Dániel Buga
 */

#ifndef COMPONENT_IMU_H_
#define COMPONENT_IMU_H_

typedef struct {} IMU_Sample_t;

void IMU_Run_OnInit(void);
void IMU_Run_OnUpdate(void);

void IMU_Write_Sample(const IMU_Sample_t* sample);

#endif /* COMPONENT_IMU_H_ */

/*
 * MotorDriver_8833.h
 *
 * Created: 2019. 07. 22. 22:37:10
 *  Author: D�niel Buga
 */ 


#ifndef COMPONENT_MOTOR_DRIVER_8833_H_
#define COMPONENT_MOTOR_DRIVER_8833_H_

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    uint8_t timer;
    uint32_t ch1;
    uint32_t ch2;
    uint8_t pin1;
    uint8_t pin2;

    int8_t prev_speed;
} MotorDriver_8833_Channel_t;

typedef struct {
    uint8_t idx;

    uint8_t fault;
    uint8_t n_sleep;

    MotorDriver_8833_Channel_t pwm_a;
    MotorDriver_8833_Channel_t pwm_b;

    bool has_fault;
} MotorDriver_8833_t;

void MotorDriver_8833_Run_OnInit(void);

void MotorDriver_8833_Run_OnDriverInit(MotorDriver_8833_t* driver);
void MotorDriver_8833_Run_OnUpdate(MotorDriver_8833_t* driver);

void MotorDriver_8833_Call_OnFault(MotorDriver_8833_t* driver);
void MotorDriver_8833_Run_FaultCleared(MotorDriver_8833_t* driver);

int8_t MotorDriver_8833_Read_DriveRequest_ChannelA(MotorDriver_8833_t* driver);
int8_t MotorDriver_8833_Read_DriveRequest_ChannelB(MotorDriver_8833_t* driver);

#endif /* COMPONENT_MOTOR_DRIVER_8833_H_ */

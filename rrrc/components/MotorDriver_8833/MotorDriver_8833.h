/*
 * MotorDriver_8833.h
 *
 * Created: 2019. 07. 22. 22:37:10
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_MOTOR_DRIVER_8833_H_
#define COMPONENT_MOTOR_DRIVER_8833_H_

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    MotorDriver_8833_Channel_A,
    MotorDriver_8833_Channel_B
} MotorDriver_8833_Channel_t;

typedef struct {
    uint8_t pwm_a1;
    uint8_t pwm_a2;
    uint8_t pwm_b1;
    uint8_t pwm_b2;

    uint8_t fault;
    uint8_t n_sleep;

    uint8_t pwm_a1_timer;
    uint8_t pwm_a2_timer;
    uint8_t pwm_b1_timer;
    uint8_t pwm_b2_timer;
    
    uint32_t pwm_a1_ch;
    uint32_t pwm_a2_ch;
    uint32_t pwm_b1_ch;
    uint32_t pwm_b2_ch;
    
    int8_t speed_a;
    int8_t speed_b;

    bool has_fault;
} MotorDriver_8833_t;

void MotorDriver_8833_Run_GlobalInit(void);

void MotorDriver_8833_Run_OnInit(MotorDriver_8833_t* driver);
void MotorDriver_8833_Run_OnUpdate(MotorDriver_8833_t* driver);

void MotorDriver_8833_Call_OnFault(MotorDriver_8833_t* driver);
void MotorDriver_8833_Run_FaultCleared(MotorDriver_8833_t* driver);

int8_t MotorDriver_8833_Read_DriveRequest(MotorDriver_8833_t* driver, MotorDriver_8833_Channel_t channel);

#endif /* COMPONENT_MOTOR_DRIVER_8833_H_ */

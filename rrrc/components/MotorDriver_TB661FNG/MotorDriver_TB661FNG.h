/*
 * MotorDriver_TB661FNG.h
 *
 * Created: 10/05/2019 14:42:53
 *  Author: Dániel Buga
 */ 

#ifndef MOTOR_DRIVER_TB661FNG_H_
#define MOTOR_DRIVER_TB661FNG_H_

#include <hal_gpio.h>
#include <hal_timer.h>

typedef struct 
{
    uint8_t dir0;
    uint8_t dir1;
    uint8_t pwm_pin;
    struct timer_descriptor* control_timer;
    uint32_t pwm_channel;
} TB6612FNG_Channel_t;

typedef struct 
{
    TB6612FNG_Channel_t channel_a;
    TB6612FNG_Channel_t channel_b;
    uint8_t standby;
} TB6612FNG_t;

void MotorDriver_TB661FNG_Run_OnInit(void);
void MotorDriver_TB661FNG_Run_OnDriverInit(TB6612FNG_t* driver);
void MotorDriver_TB661FNG_Run_Update(TB6612FNG_t* driver);
int8_t MotorDriver_TB661FNG_Read_DriveValue_ChannelA(TB6612FNG_t* driver);
int8_t MotorDriver_TB661FNG_Read_DriveValue_ChannelB(TB6612FNG_t* driver);

#endif /* MOTOR_DRIVER_TB661FNG_H_ */

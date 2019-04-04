/*
 * pid.h
 *
 * Created: 4/4/2019 1:42:00 PM
 *  Author: Daniel Buga
 */ 

#ifndef PID_H_
#define PID_H_

typedef struct
{
    struct {
        float P;
        float I;
        float D;
        float LowerLimit;
        float UpperLimit;
    } config;
    
    struct {
        float previousFeedback;
        float previousError;
        float integratorFeedback;
    } state;
} PID_t;

void pid_initialize(PID_t* controller);
float pid_update(PID_t* controller, float refSignal, float feedback);

#endif /* PID_H_ */
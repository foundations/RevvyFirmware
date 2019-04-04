/*
 * pid.c
 *
 * Created: 4/4/2019 1:41:50 PM
 *  Author: Daniel Buga
 */ 

#include "pid.h"

static float saturate_float(float value, float min, float max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    else
    {
        return value;
    }
}

void pid_initialize(PID_t* controller)
{
    controller->config.P = 0.0f;
    controller->config.I = 0.0f;
    controller->config.D = 0.0f;
    controller->config.LowerLimit = 0.0f;
    controller->config.UpperLimit = 0.0f;

    controller->state.previousError = 0.0f;
    controller->state.previousFeedback = 0.0f;
    controller->state.integratorFeedback = 0.0f;
}

float pid_update(PID_t* controller, float refSignal, float feedback)
{
    float error = refSignal - feedback;
    float dError = controller->state.previousError - error;

    float p = error * controller->config.P;
    float d = dError * controller->config.D;
    float u = saturate_float(p + d + controller->state.integratorFeedback, controller->config.LowerLimit, controller->config.UpperLimit);

    controller->state.previousError = error;
    controller->state.previousFeedback = feedback;
    controller->state.integratorFeedback = (1 - controller->config.I) * controller->state.integratorFeedback + controller->config.I * u;

    return u;
}

/*
 * functions.c
 *
 * Created: 03/05/2019 14:21:39
 *  Author: Dániel Buga
 */ 

#include "functions.h"

float constrain_f32(float in, float min, float max)
{
    if (in < min)
    {
        return min;
    }
    else if (in > max)
    {
        return max;
    }
    else
    {
        return in;
    }
}

float map(float in, float min_in, float max_in, float min_out, float max_out)
{
    float in_fs = max_in - min_in;
    float out_fs = max_out - min_out;

    float input = constrain_f32(in, min_in, max_in);
    return (input - min_in) * (out_fs / in_fs) + min_out;
}

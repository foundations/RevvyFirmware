/*
 * functions.c
 *
 * Created: 03/05/2019 14:21:39
 *  Author: D�niel Buga
 */ 

#include "functions.h"

float constrain_f32(float in, float min, float max)
{
    if (max < min)
    {
        float m = max;
        max = min;
        min = m;
    }
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

int32_t constrain_int32(int32_t value, int32_t min, int32_t max)
{
    if (max < min)
    {
        int32_t m = max;
        max = min;
        min = m;
    }
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

int8_t constrain_int8(int8_t value, int8_t min, int8_t max)
{
    if (max < min)
    {
        int8_t m = max;
        max = min;
        min = m;
    }
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

float map_constrained(float in, float min_in, float max_in, float min_out, float max_out)
{
    float in_fs = max_in - min_in;
    float out_fs = max_out - min_out;

    float input = constrain_f32(in, min_in, max_in);
    return (input - min_in) * (out_fs / in_fs) + min_out;
}

float map(float in, float min_in, float max_in, float min_out, float max_out)
{
    float in_fs = max_in - min_in;
    float out_fs = max_out - min_out;

    return (in - min_in) * (out_fs / in_fs) + min_out;
}

uint32_t SwapEndian(uint32_t dig)
{
    uint8_t *c = (uint8_t *) &dig;
    return * (uint32_t *) (uint8_t[]) {c[3], c[2], c[1], c[0] };
}

/*
 * converter.c
 *
 * Created: 2019. 04. 26. 13:46:33
 *  Author: Dániel Buga
 */ 

#include "converter.h"

int32_t get_int32(const uint8_t* buffer)
{
    return *(int32_t*) buffer;
}

float get_float(const uint8_t* buffer)
{
    return *(float*) buffer;
}

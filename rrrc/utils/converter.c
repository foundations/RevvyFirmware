/*
 * converter.c
 *
 * Created: 2019. 04. 26. 13:46:33
 *  Author: Dániel Buga
 */ 

#include "converter.h"

int32_t get_int32(const uint8_t* buffer)
{
    uint32_t bytes = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3] << 0;

    return (int32_t) (bytes);
}

float get_float(const uint8_t* buffer)
{
    uint32_t bytes = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3] << 0;

    return *(float*) &bytes;
}

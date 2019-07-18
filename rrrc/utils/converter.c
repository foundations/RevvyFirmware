/*
 * converter.c
 *
 * Created: 2019. 04. 26. 13:46:33
 *  Author: D�niel Buga
 */ 

#include "converter.h"

int32_t get_int32(const uint8_t* buffer)
{
    return *(int32_t*) buffer;
}

int16_t get_int16(const uint8_t* buffer)
{
    return *(int16_t*) buffer;
}

uint16_t get_uint16(const uint8_t* buffer)
{
    return *(uint16_t*) buffer;
}

uint32_t get_uint32(const uint8_t* buffer)
{
    return *(uint32_t*) buffer;
}

float get_float(const uint8_t* buffer)
{
    return *(float*) buffer;
}

/*
 * converter.h
 *
 * Created: 4/4/2019 2:25:55 PM
 *  Author: Daniel Buga
 */ 


#ifndef CONVERTER_H_
#define CONVERTER_H_

static int32_t get_int32(const uint8_t* buffer)
{
    uint32_t bytes = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3] << 0;

    return (int32_t) (bytes);
}

static float get_float(const uint8_t* buffer)
{
    uint32_t bytes = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3] << 0;

    return *(float*) &bytes;
}

#endif /* CONVERTER_H_ */
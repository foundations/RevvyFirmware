/*
 * color.h
 *
 * Created: 2019. 04. 18. 14:42:15
 *  Author: Dániel Buga
 */ 


#ifndef COLOR_H_
#define COLOR_H_

#include <stdint.h>

typedef struct
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} rgb_t;

typedef struct
{
    uint16_t h;
    uint8_t s;
    uint8_t v;
} hsv_t;

rgb_t hsv_to_rgb(hsv_t hsv_col);

#endif /* COLOR_H_ */
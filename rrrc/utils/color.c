/*
* color.c
*
* Created: 2019. 04. 18. 14:41:49
*  Author: Dániel Buga
*/

#include "color.h"

rgb_t hsv_to_rgb(hsv_t hsv_col)
{
    uint16_t h = hsv_col.h % 360;
    float s = hsv_col.s / 100.0f;
    float v = hsv_col.v / 100.0f;

    float hh = h / 60.0f;
    uint8_t hue = (uint8_t) hh;
    
    float ff = hh - hue;
    float p = v * (1.0f - s);
    float q = v * (1.0f - (s * ff));
    float t = v * (1.0f - (s * (1.0f - ff)));
    
    uint8_t qq = (uint8_t)(q * 255u);
    uint8_t vv = (uint8_t)(v * 255u);
    uint8_t tt = (uint8_t)(t * 255u);
    uint8_t pp = (uint8_t)(p * 255u);

    switch (hue)
    {
        case 0:
            return (rgb_t) { 
                .R = vv,
                .G = tt,
                .B = pp };
        case 1:
            return (rgb_t) {
                .R = qq,
                .G = vv,
                .B = pp };
        case 2:
            return (rgb_t) {
                .R = pp,
                .G = vv,
                .B = tt };
        case 3:
            return (rgb_t) {
                .R = pp,
                .G = qq,
                .B = vv };
        case 4:
            return (rgb_t) {
                .R = tt,
                .G = pp,
                .B = vv };
        case 5: 
        default:
            return (rgb_t) {
                .R = vv,
                .G = pp,
                .B = qq };
    }
}
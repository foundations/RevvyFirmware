/*
 * InternalTemperatureSensor.c
 *
 * Created: 02/05/2019 18:31:05
 *  Author: D�niel Buga
 */ 

#include "InternalTemperatureSensor.h"

#include "samd51.h"

typedef struct 
{
    uint8_t tli;
    uint8_t tld:4;
    uint8_t thi;
    uint8_t thd:4;
    uint16_t res1;
    uint16_t vpl:12;
    uint16_t vph:12;
    uint16_t vcl:12;
    uint16_t vch:12;
} temp_cal;

void InternalTemperatureSensor_Run_Convert(float tp, float tc, float* temp)
{
    const temp_cal* tempc = (const temp_cal*)NVMCTRL_TEMP_LOG;
    
    const float tl = tempc->tli + tempc->tld * 0.0625f;
    const float th = tempc->thi + tempc->thd * 0.0625f;
    
    float num = tl * tempc->vph * tc - tempc->vpl * th * tc - tl * tempc->vch * tp + th * tempc->vcl * tp;
    float den = tempc->vcl * tp - tempc->vch * tp - tempc->vpl * tc + tempc->vph * tc;

    *temp = num / den;
}

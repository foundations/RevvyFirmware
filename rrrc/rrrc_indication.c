/*
 * rrrc_indication.c
 *
 * Created: 10-Mar-19 01:48:33
 *  Author: vmyakovlev
 */ 

#include "rrrc_hal.h"
#include "rrrc_indication.h"
#include "rrrc_indication_predefined.h"

#include <string.h>
#include <math.h>

//*********************************************************************************************
int32_t IndicationSetRingType(enum INDICATON_RING_TYPE type)
{
    int32_t status = ERR_NONE;
    return status;
}

//*********************************************************************************************
uint32_t IndicationGetRingLedsAmount()
{
    return RING_LEDS_AMOUNT;
}

//*********************************************************************************************
uint32_t IndicationGetStatusLedsAmount()
{
    return STATUS_LEDS_AMOUNT;
}

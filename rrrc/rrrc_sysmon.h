/*
 * rrrc_sysmon.h
 *
 * Created: 23-Mar-19 02:51:10
 *  Author: vmyakovlev
 */ 


#ifndef RRRC_SYSMON_H_
#define RRRC_SYSMON_H_

#include <stdint.h>
#include "rrrc_hal.h"

typedef struct
{
    uint32_t systicks;
    float motor_voltage;
    float motor_current;
    float battery_voltage;
    float temperature;
} rrrc_sysmon_t;

int32_t SysMonGetValues(uint32_t* data);
void SystemMonitor_Update(const rrrc_sysmon_t* value);

#endif /* RRRC_SYSMON_H_ */
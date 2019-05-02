/*
 * rrrc_sysmon.c
 *
 * Created: 23-Mar-19 02:51:29
 *  Author: vmyakovlev
 */ 

#include "rrrc_sysmon.h"

static rrrc_sysmon_t sysmon_val = {0};

int32_t SysMonGetValues(uint32_t* data)
{
    sysmon_val.systicks = get_system_tick();
    const uint32_t sz = sizeof (rrrc_sysmon_t);
    memcpy(data, &sysmon_val, sz );
    return sz;
}

void SystemMonitor_Update(const rrrc_sysmon_t* value)
{
    sysmon_val = *value;
}

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

typedef struct _rrrc_sysmon_t
{
	uint32_t systicks;
	uint32_t temperature1;
	uint32_t temperature2;
	uint32_t motor_voltage;
	uint32_t motor_current;
	uint32_t battery_voltage;	
	uint32_t battery_status;
	
}rrrc_sysmot_t, *p_rrrc_sysmot_t;


int32_t SysMon_Init();
int32_t SysMon_DeInit();
int32_t SysMonGetValues(uint32_t* data);

#endif /* RRRC_SYSMON_H_ */
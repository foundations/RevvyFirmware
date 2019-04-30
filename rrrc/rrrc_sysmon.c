/*
 * rrrc_sysmon.c
 *
 * Created: 23-Mar-19 02:51:29
 *  Author: vmyakovlev
 */ 



#include "rrrc_sysmon.h"
#include "rrrc_motors.h"
#include <math.h>

#define ADC_MAX 4095
#define adc_to_mv(x)     ((3300.0f / ADC_MAX) * x)
static TaskHandle_t      xRRRC_SysMon_xTask;
void RRRC_SysMom_xTask(void* user_data);

static rrrc_sysmot_t sysmon_val = {0};

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

static void SysMon_adc_mot_volt_cb(const uint16_t adc_data, void* user_data)
{
//R1=100K
//R2=30K
	sysmon_val.motor_voltage = (uint32_t) lroundf(adc_to_mv(adc_data) * (130.0f / 30.0f));
}

static void SysMon_adc_bat_volt_cb(const uint16_t adc_data, void* user_data)
{
//R1=100K
//R2=240K
	sysmon_val.battery_voltage = (uint32_t) lroundf(adc_to_mv(adc_data) * (340.0f / 240.0f));
}

static void SysMon_adc_mot_current_cb(const uint16_t adc_data, void* user_data)
{
	sysmon_val.motor_current = (uint32_t) lroundf(adc_to_mv(adc_data));
}

static void SysMon_adc_temperatureC_cb(const uint16_t adc_data, void* user_data)
{
	sysmon_val.temperature1 = adc_to_mv(adc_data); //TODO
}

static void SysMon_adc_temperatureP_cb(const uint16_t adc_data, void* user_data)
{
	sysmon_val.temperature2 = adc_to_mv(adc_data); //TODO
}

float get_temp(float tp, float tc)
{
    const temp_cal* tempc = (temp_cal*)NVMCTRL_TEMP_LOG;
    
    const float tl = tempc->tli + tempc->tld * 0.0625f;
    const float th = tempc->thi + tempc->thd * 0.0625f;

    float t = (tl * tempc->vph * tc - tempc->vpl * th * tc - tl * tempc->vch * tp + th * tempc->vcl * tp) /
        (tempc->vcl * tp - tempc->vch * tp - tempc->vpl * tc + tempc->vph * tc);

    return t;
}

int32_t SysMonGetValues(uint32_t* data)
{
	sysmon_val.systicks = get_system_tick();
	const uint32_t sz = sizeof (rrrc_sysmot_t);
	memcpy(data, &sysmon_val, sz );
	return sz;
}

void RRRC_SysMom_xTask(void* user_data)
{
// #define BAT_EN		PB22 //OUT
// #define BAT_CHG		PB19 //IN
// #define BAT_PG		PB22 //IN
// #define BAT_ISET2	PB18 //OUT
// #define MOT_CURRENT_FAULT	PB05 //IN
	while (1)
	{
		uint32_t val = 0;
		uint32_t bat_status = gpio_get_pin_level(SM_BAT_CHG);
		uint32_t bat_bg = gpio_get_pin_level(SM_BAT_PG);
		uint32_t motor_current_fault = gpio_get_pin_level(SM_MOT_CURRENT_FAULT);

        float temp = get_temp(sysmon_val.temperature1, sysmon_val.temperature2);
		
// 		if (motor_current_fault)
// 			for(uint32_t mot_idx=0; mot_idx<MotorPortGetPortsAmount(); mot_idx++)
// 				MotorPortSetState(mot_idx, 0);

		os_sleep(200*rtos_get_ticks_in_ms());
	}
}

int32_t SysMon_Init()
{
	int32_t result = ERR_NONE;
	RRRC_channel_adc_register_cb(1, 0, SysMon_adc_mot_current_cb, NULL);
	RRRC_channel_adc_register_cb(1, 1, SysMon_adc_bat_volt_cb, NULL);
	RRRC_channel_adc_register_cb(1, 2, SysMon_adc_mot_volt_cb, NULL);
	RRRC_channel_adc_register_cb(1, 3, SysMon_adc_temperatureP_cb, NULL);
	RRRC_channel_adc_register_cb(1, 4, SysMon_adc_temperatureC_cb, NULL);
	
	if (pdPASS != xTaskCreate(RRRC_SysMom_xTask, "RRRC_SysMon", 512 / sizeof(portSTACK_TYPE), NULL, tskIDLE_PRIORITY+2, &xRRRC_SysMon_xTask))
		return ERR_FAILURE;

	return result;
}

int32_t SysMon_DeInit()
{
	int32_t result = ERR_NONE;
	RRRC_channel_adc_unregister_cb(1, 0);
	RRRC_channel_adc_unregister_cb(1, 1);
	RRRC_channel_adc_unregister_cb(1, 2);
	RRRC_channel_adc_unregister_cb(1, 3);
	RRRC_channel_adc_unregister_cb(1, 4);
	
	vTaskDelete(xRRRC_SysMon_xTask);
	
	return result;	
}
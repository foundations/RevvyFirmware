/*
 * rrrc_sysmon.c
 *
 * Created: 23-Mar-19 02:51:29
 *  Author: vmyakovlev
 */ 



#include "rrrc_sysmon.h"
#include "rrrc_motors.h"
#define adc_to_volt(x)    ((uint32_t)((3300.0f/1023) * x))
#define adc_to_celsius(x) ((uint32_t)((3300.0f/1023) * x))
static TaskHandle_t      xRRRC_SysMon_xTask;
void RRRC_SysMom_xTask(void* user_data);

static rrrc_sysmot_t sysmon_val = {0};

static void SysMon_adc_mot_volt_cb(const uint8_t adc_data, void* user_data)
{
	sysmon_val.motor_voltage = adc_data;
	return;
}

static void SysMon_adc_bat_volt_cb(const uint8_t adc_data, void* user_data)
{
	sysmon_val.battery_voltage = adc_to_volt(adc_data);
	return;
}

static void SysMon_adc_mot_current_cb(const uint8_t adc_data, void* user_data)
{
	sysmon_val.motor_current= adc_to_volt(adc_data);
	return;
}

static void SysMon_adc_temperature_cb(const uint8_t adc_data, void* user_data)
{
	sysmon_val.temperature= adc_to_celsius(adc_data);
	return;
}

int32_t SysMonGetValues(uint32_t* data)
{
	sysmon_val.systicks = get_system_tick();
	uint32_t sz = sizeof (rrrc_sysmot_t);
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
	RRRC_channel_adc_register_cb(1, 3, SysMon_adc_temperature_cb, NULL);
	
	if (pdPASS != xTaskCreate(RRRC_SysMom_xTask, "RRRC_SysMon", 256 / sizeof(portSTACK_TYPE), NULL, tskIDLE_PRIORITY+2, &xRRRC_SysMon_xTask))
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
	
	vTaskDelete(xRRRC_SysMon_xTask);
	
	return result;	
}
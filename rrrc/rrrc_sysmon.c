/*
 * rrrc_sysmon.c
 *
 * Created: 23-Mar-19 02:51:29
 *  Author: vmyakovlev
 */ 



#include "rrrc_sysmon.h"

static TaskHandle_t      xRRRC_SysMon_xTask;
void RRRC_SysMom_xTask(void* user_data);

static void SensorPort_adc_mot_volt_cb(const uint8_t adc_data, void* user_data)
{
	
}

static void SensorPort_adc_bat_volt_cb(const uint8_t adc_data, void* user_data)
{
	
}

int32_t SysMon_Init()
{
	int32_t result = ERR_NONE;
	RRRC_channel_adc_register_cb(11, SensorPort_adc_mot_volt_cb, NULL);
	RRRC_channel_adc_register_cb(12, SensorPort_adc_bat_volt_cb, NULL);	
	
	if (pdPASS != xTaskCreate(RRRC_SysMom_xTask, "RRRC_Main", 256 / sizeof(portSTACK_TYPE), NULL, tskIDLE_PRIORITY+2, &xRRRC_SysMon_xTask))
		return ERR_FAILURE;
	
	return result;
}

int32_t SysMon_DeInit()
{
	int32_t result = ERR_NONE;
	RRRC_channel_adc_unregister_cb(11);
	RRRC_channel_adc_unregister_cb(12);
	
	vTaskDelete(xRRRC_SysMon_xTask);
	
	return result;	
}
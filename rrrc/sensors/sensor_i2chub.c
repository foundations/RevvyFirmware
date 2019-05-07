/*
 * sensor_i2chub.c
 *
 * Created: 10-Mar-19 02:54:31
 *  Author: vmyakovlev
 */ 
#include "sensor_i2chub.h"

//*********************************************************************************************
int32_t I2CHUB_Init(void* hw_port)
{
    int32_t result = ERR_NONE;
    p_hw_sensor_port_t sensport = hw_port;
    return result;
}

//*********************************************************************************************
uint32_t I2CHUB_write_data(void* hw_port, void* data, uint32_t size)
{
    int32_t result = ERR_NONE;
    p_hw_sensor_port_t sensport = hw_port;
    return result;
}

//*********************************************************************************************
uint32_t I2CHUB_read_data(void* hw_port, void* data, uint32_t size)
{
    int32_t result = ERR_NONE;
    p_hw_sensor_port_t sensport = hw_port;
    return result;
}

sensor_lib_entry_t sensor_i2chub =
{
    .type_id = SENSOR_I2CHUB,
    .name = "I2CHUB",
    .SensorInit = I2CHUB_Init,
    .SensorDeInit = NULL,
    .sensor_get_values = NULL,
    .write_data = I2CHUB_write_data,
    .read_data = I2CHUB_read_data,

    //callback from sensor port
    .timer_callback = NULL,
    .adc_callback = NULL,
    .gpio0_callback = NULL,
};

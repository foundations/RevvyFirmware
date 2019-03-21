
#ifndef _RRRC_SENSOR_INTERFACE_H_
#define _RRRC_SENSOR_INTERFACE_H_

#include <stdint.h>
#include "err_codes.h"
#include "rrrc_hal.h"


#ifdef __cplusplus
extern "C" {
#endif


#define SENSOR_PORT_LIBDATA 128

typedef enum _sensor_type_t
{
	SENSOR_NOT_SET,
	SENSOR_HC_SR05,
	SENSOR_SWITCH,
	SENSOR_ANALOG_SWITCH,
	SENSOR_SJMCU34725,
	SENSOR_EKPSMD3528,
	SENSOR_KXTJ3_1507,
	SENSOR_POTENTIOMETER,
	SENSOR_I2CHUB,
}sensor_type_t;

// typedef enum _sens_callback_type_t
// {
//     SENS_CB_TASK,
//     SENS_CB_I2C,
//     SENS_CB_ADC,
//     SENS_CB_GPIO0_EXT,
//     SENS_CB_GPIO1_EXT,
// }sens_callback_type_t;
// 
// typedef void (*sensor_callback_t)(void* data);

typedef enum 
{
	SENSOR_VCCIO_3V3,
	SENSOR_VCCIO_5V0,
}sensor_vccio_t;

typedef struct _sensor_values_t
{
    uint32_t amount_vals;
    uint32_t* value32bit;
}sensor_values_t, *p_sensor_values_t;

typedef struct _sensor_lib_entry_t
{
    sensor_type_t type_id;
    char*	name;

    int32_t (*SensorInit)(void* hw_port);
    void (*sensor_thread)(void* hw_port);
	uint32_t (*write_data)(void* hw_port, void* data, uint32_t size);
	uint32_t (*read_data)(void* hw_port, void* data, uint32_t size);

	uint32_t (*sensor_get_values)(void* hw_port, uint32_t* data, uint32_t max_size);

    //callback from sensor port
    void (*timer_callback)(void* hw_port, uint32_t time);
    void (*adc_callback)(void* hw_port, uint32_t time);
    void (*gpio0_callback)(void* hw_port, uint32_t state);
    void (*gpio1_callback)(void* hw_port, uint32_t state);
}sensor_lib_entry_t, *p_sensor_lib_entry_t;



typedef uint8_t gpio_num;

typedef struct _hw_sensor_port_t
{
	uint32_t index;
	p_sensor_lib_entry_t sensor_lib;

	uint8_t lib_data[SENSOR_PORT_LIBDATA];

	TaskHandle_t      xSensorPortTask;

	struct i2c_m_sync_desc		*I2C;
	gpio_num i2c_sda;
	gpio_num i2c_scl;
	
	struct adc_async_descriptor *ADC;
	uint32_t adc_chan_idx;
	gpio_num adc_gpio;

	gpio_num gpio0_num;
	gpio_num gpio1_num;

	gpio_num led0_gpio;
	gpio_num led1_gpio;

	gpio_num vccio_pin;
	
	struct timer_descriptor *TIMER;
}hw_sensor_port_t, *p_hw_sensor_port_t;


#ifdef __cplusplus
}
#endif






#endif //_RRRC_SENSOR_INTERFACE_H_

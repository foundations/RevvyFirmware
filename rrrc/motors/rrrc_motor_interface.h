/*
 * rrrc_motor_interface.h
 *
 * Created: 2/27/2019 8:34:21 PM
 *  Author: User
 */ 


#ifndef RRRC_MOTOR_INTERFACE_H_
#define RRRC_MOTOR_INTERFACE_H_

#include <stdint.h>
#include "err_codes.h"
#include "rrrc_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOTOR_PORT_LIBDATA 128

typedef enum _motor_type_t
{
	MOTOR_NOT_SET,
    MOTOR_OPENLOOP,
    MOTOR_POSITION_CONTROLLED,
    MOTOR_SPEED_CONTROLLED,
	MOTOR_SERVO
}motor_type_t;

typedef struct _hw_motor_port_t;

typedef struct _motor_lib_entry_t
{
    motor_type_t type_id;
    char* name;

    int32_t (*MotorInit)(void* hw_port);
    int32_t (*MotorDeInit)(void* hw_port);

    int32_t (*motor_set_config)(void* hw_port, const uint8_t* data, uint32_t size);
    uint32_t (*motor_get_config)(void* hw_port, uint8_t* data, uint32_t max_size);

    uint32_t (*motor_get_position)(void* hw_port, uint8_t* data, uint32_t max_size);

    uint32_t (*motor_set_control)(void* hw_port, int32_t value);
    uint32_t (*motor_get_control)(void* hw_port, uint8_t* data, uint32_t max_size);

    void (*gpio0_callback)(void* hw_port, uint32_t state);
    void (*gpio1_callback)(void* hw_port, uint32_t state);
}motor_lib_entry_t, *p_motor_lib_entry_t;

typedef struct 
{
    void (*init)(struct _hw_motor_port_t* hw_port);
    void (*set_speed)(struct _hw_motor_port_t* hw_port, int8_t speed);
    int8_t (*get_speed)(struct _hw_motor_port_t* hw_port);
    void (*deinit)(struct _hw_motor_port_t* hw_port);
} motor_driver_lib_entry_t;

typedef uint8_t gpio_num;

typedef struct _hw_motor_port_t
{
	uint32_t index;
	p_motor_lib_entry_t motor_lib;

	uint8_t lib_data[MOTOR_PORT_LIBDATA];

	struct timer_descriptor *enc_timer;
	gpio_num enc0_gpio;
	gpio_num enc1_gpio;

	gpio_num led0_gpio;
	gpio_num led1_gpio;

	gpio_num pwm_pin;

    void* motorDriverConfig;
    const motor_driver_lib_entry_t* motor_driver_lib;
    
}hw_motor_port_t, *p_hw_motor_port_t;

#ifdef __cplusplus
}
#endif

#endif /* RRRC_MOTOR_INTERFACE_H_ */
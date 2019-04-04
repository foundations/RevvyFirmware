/*
 * rrrc_tb6612fng.h
 *
 * Created: 4/3/2019 2:42:27 PM
 *  Author: Daniel Buga
 */ 


#ifndef RRRC_TB6612FNG_H_
#define RRRC_TB6612FNG_H_

#include <stdint.h>
#include "err_codes.h"
#include "rrrc_hal.h"
#include "motors/rrrc_motor_interface.h"

typedef struct {
    gpio_num dir0_gpio;
    gpio_num dir1_gpio;
    
    struct timer_descriptor *pwm;
    uint32_t pwm_ch;

    int8_t speed;
} tb6612fng_t, *p_tb6612fng_t;

void   tb6612fng_one_time_init(hw_motor_port_t* hw_port);
void   tb6612fng_init(hw_motor_port_t* hw_port);
void   tb6612fng_set_speed(hw_motor_port_t* hw_port, int8_t speed);
int8_t tb6612fng_get_speed(hw_motor_port_t* hw_port);
void   tb6612fng_deinit(hw_motor_port_t* hw_port);

extern const motor_driver_lib_entry_t motor_driver_tb6612fng;

#endif /* RRRC_TB6612FNG_H_ */

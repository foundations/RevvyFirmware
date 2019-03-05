/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_adc_sync.h>

#include <hal_adc_sync.h>

#include <hal_ext_irq.h>

#include <hal_timer.h>

#include <hal_i2c_m_async.h>

#include <hal_i2c_m_async.h>

#include <hal_i2c_s_async.h>

#include <hal_i2c_m_async.h>

#include <hal_i2c_m_async.h>

#include <hal_delay.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_timer.h>
#include <hpl_tcc.h>
#include <hal_timer.h>
#include <hpl_tcc.h>

#include "hal_usb_device.h"

#include <hal_wdt.h>

#define SERCOM2_I2CS_BUFFER_SIZE 128

extern struct adc_sync_descriptor ADC_0;

extern struct adc_sync_descriptor ADC_1;
extern struct timer_descriptor    TIMER_0;

extern struct i2c_m_async_desc I2C_4;

extern struct i2c_m_async_desc I2C_1;

extern struct i2c_s_async_descriptor I2C_0;

extern struct i2c_m_async_desc I2C_3;

extern struct i2c_m_async_desc I2C_2;

extern struct timer_descriptor TIMER_1;
extern struct timer_descriptor TIMER_2;
extern struct timer_descriptor TIMER_3;
extern struct timer_descriptor TIMER_4;
extern struct timer_descriptor TIMER_5;
extern struct timer_descriptor TIMER_6;

extern struct timer_descriptor TIMER_7;

extern struct timer_descriptor TIMER_8;

extern struct wdt_descriptor WDT_0;

void ADC_0_PORT_init(void);
void ADC_0_CLOCK_init(void);
void ADC_0_init(void);

void ADC_1_PORT_init(void);
void ADC_1_CLOCK_init(void);
void ADC_1_init(void);

void I2C_4_PORT_init(void);
void I2C_4_CLOCK_init(void);
void I2C_4_init(void);

void I2C_1_PORT_init(void);
void I2C_1_CLOCK_init(void);
void I2C_1_init(void);

void I2C_0_PORT_init(void);
void I2C_0_CLOCK_init(void);
void I2C_0_init(void);

void I2C_3_PORT_init(void);
void I2C_3_CLOCK_init(void);
void I2C_3_init(void);

void I2C_2_PORT_init(void);
void I2C_2_CLOCK_init(void);
void I2C_2_init(void);

void delay_driver_init(void);

void TIMER_7_CLOCK_init(void);
void TIMER_7_init(void);

void TIMER_8_CLOCK_init(void);
void TIMER_8_init(void);

void USB_0_CLOCK_init(void);
void USB_0_init(void);

void WDT_0_CLOCK_init(void);
void WDT_0_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED

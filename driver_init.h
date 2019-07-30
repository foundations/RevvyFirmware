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

#include <hal_adc_async.h>
#include <hal_ext_irq.h>
#include <hal_timer.h>
#include <hpl_tc_base.h>
#include <hal_delay.h>
#include <hal_wdt.h>

#include <hal_i2c_m_async.h>
#include <hal_spi_m_dma.h>
#include <hal_spi_m_sync.h>

#include <hal_evsys.h>
#include <tc_lite.h>

extern struct i2c_m_async_desc I2C_0;
extern struct i2c_m_async_desc I2C_1;
extern struct i2c_m_async_desc I2C_2;
extern struct i2c_m_async_desc I2C_3;

extern struct timer_descriptor TIMER_TC6;

void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED

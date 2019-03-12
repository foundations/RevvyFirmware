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
#include <hpl_tcc.h>
#include <hal_delay.h>
#include <hal_wdt.h>

#include <hal_i2c_m_sync.h>
#include <hal_i2c_m_async.h>
#include <hal_i2c_s_async.h>
#include <hal_spi_m_dma.h>
#include <hal_spi_m_sync.h>

#include <hal_evsys.h>
#include <tc_lite.h>

//#include "hal_usb_device.h"



#define SERCOM2_I2CS_BUFFER_SIZE 128

extern struct adc_async_descriptor ADC_0;
extern struct adc_async_descriptor ADC_1;

extern struct i2c_s_async_descriptor I2C_0;
// extern struct i2c_m_async_desc I2C_1;
// extern struct i2c_m_async_desc I2C_2;
// extern struct i2c_m_async_desc I2C_3;
// extern struct i2c_m_async_desc I2C_4;
extern struct i2c_m_sync_desc		I2C_1;
extern struct i2c_m_sync_desc		I2C_2;
extern struct i2c_m_sync_desc		I2C_3;
extern struct i2c_m_sync_desc		I2C_4;

extern struct timer_descriptor TIMER_RTC;

extern struct timer_descriptor TIMER_TC0;
extern struct timer_descriptor TIMER_TC1;
extern struct timer_descriptor TIMER_TC2;
extern struct timer_descriptor TIMER_TC3;
extern struct timer_descriptor TIMER_TC4;
extern struct timer_descriptor TIMER_TC5;
extern struct timer_descriptor TIMER_TC6;
extern struct timer_descriptor TIMER_TC7;

extern struct timer_descriptor TIMER_TCC0;
extern struct timer_descriptor TIMER_TCC1;
extern struct timer_descriptor TIMER_TCC2;
extern struct timer_descriptor TIMER_TCC3;
extern struct timer_descriptor TIMER_TCC4;

extern struct wdt_descriptor WDT_0;

//extern struct spi_m_sync_descriptor SPI_0;
extern struct spi_m_dma_descriptor  SPI_0;

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED

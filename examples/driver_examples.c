/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

/**
 * Example of using ADC_0 to generate waveform.
 */
void ADC_0_example(void)
{
	uint8_t buffer[2];

	adc_sync_enable_channel(&ADC_0, 0);

	while (1) {
		adc_sync_read_channel(&ADC_0, 0, buffer, 2);
	}
}

/**
 * Example of using ADC_1 to generate waveform.
 */
void ADC_1_example(void)
{
	uint8_t buffer[2];

	adc_sync_enable_channel(&ADC_1, 0);

	while (1) {
		adc_sync_read_channel(&ADC_1, 0, buffer, 2);
	}
}

static void button_on_PB10_pressed(void)
{
}

static void button_on_PB11_pressed(void)
{
}

static void button_on_PB12_pressed(void)
{
}

static void button_on_PB13_pressed(void)
{
}

/**
 * Example of using EXTERNAL_IRQ_0
 */
void EXTERNAL_IRQ_0_example(void)
{

	ext_irq_register(PIN_PB10, button_on_PB10_pressed);
	ext_irq_register(PIN_PB11, button_on_PB11_pressed);
	ext_irq_register(PIN_PB12, button_on_PB12_pressed);
	ext_irq_register(PIN_PB13, button_on_PB13_pressed);
}

static struct timer_task TIMER_0_task1, TIMER_0_task2;
/**
 * Example of using TIMER_0.
 */
static void TIMER_0_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_0_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_0_example(void)
{
	TIMER_0_task1.interval = 100;
	TIMER_0_task1.cb       = TIMER_0_task1_cb;
	TIMER_0_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_0_task2.interval = 200;
	TIMER_0_task2.cb       = TIMER_0_task2_cb;
	TIMER_0_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &TIMER_0_task1);
	timer_add_task(&TIMER_0, &TIMER_0_task2);
	timer_start(&TIMER_0);
}

static uint8_t I2C_4_example_str[12] = "Hello World!";

void I2C_4_tx_complete(struct i2c_m_async_desc *const i2c)
{
}

void I2C_4_example(void)
{
	struct io_descriptor *I2C_4_io;

	i2c_m_async_get_io_descriptor(&I2C_4, &I2C_4_io);
	i2c_m_async_enable(&I2C_4);
	i2c_m_async_register_callback(&I2C_4, I2C_M_ASYNC_TX_COMPLETE, (FUNC_PTR)I2C_4_tx_complete);
	i2c_m_async_set_slaveaddr(&I2C_4, 0x12, I2C_M_SEVEN);

	io_write(I2C_4_io, I2C_4_example_str, 12);
}

static uint8_t I2C_1_example_str[12] = "Hello World!";

void I2C_1_tx_complete(struct i2c_m_async_desc *const i2c)
{
}

void I2C_1_example(void)
{
	struct io_descriptor *I2C_1_io;

	i2c_m_async_get_io_descriptor(&I2C_1, &I2C_1_io);
	i2c_m_async_enable(&I2C_1);
	i2c_m_async_register_callback(&I2C_1, I2C_M_ASYNC_TX_COMPLETE, (FUNC_PTR)I2C_1_tx_complete);
	i2c_m_async_set_slaveaddr(&I2C_1, 0x12, I2C_M_SEVEN);

	io_write(I2C_1_io, I2C_1_example_str, 12);
}

static struct io_descriptor *io;

static void I2C_0_rx_complete(const struct i2c_s_async_descriptor *const descr)
{
	uint8_t c;

	io_read(io, &c, 1);
}

void I2C_0_example(void)
{
	i2c_s_async_get_io_descriptor(&I2C_0, &io);
	i2c_s_async_register_callback(&I2C_0, I2C_S_RX_COMPLETE, I2C_0_rx_complete);
	i2c_s_async_enable(&I2C_0);
}

static uint8_t I2C_3_example_str[12] = "Hello World!";

void I2C_3_tx_complete(struct i2c_m_async_desc *const i2c)
{
}

void I2C_3_example(void)
{
	struct io_descriptor *I2C_3_io;

	i2c_m_async_get_io_descriptor(&I2C_3, &I2C_3_io);
	i2c_m_async_enable(&I2C_3);
	i2c_m_async_register_callback(&I2C_3, I2C_M_ASYNC_TX_COMPLETE, (FUNC_PTR)I2C_3_tx_complete);
	i2c_m_async_set_slaveaddr(&I2C_3, 0x12, I2C_M_SEVEN);

	io_write(I2C_3_io, I2C_3_example_str, 12);
}

static uint8_t I2C_2_example_str[12] = "Hello World!";

void I2C_2_tx_complete(struct i2c_m_async_desc *const i2c)
{
}

void I2C_2_example(void)
{
	struct io_descriptor *I2C_2_io;

	i2c_m_async_get_io_descriptor(&I2C_2, &I2C_2_io);
	i2c_m_async_enable(&I2C_2);
	i2c_m_async_register_callback(&I2C_2, I2C_M_ASYNC_TX_COMPLETE, (FUNC_PTR)I2C_2_tx_complete);
	i2c_m_async_set_slaveaddr(&I2C_2, 0x12, I2C_M_SEVEN);

	io_write(I2C_2_io, I2C_2_example_str, 12);
}

void delay_example(void)
{
	delay_ms(5000);
}

static struct timer_task TIMER_1_task1, TIMER_1_task2;

/**
 * Example of using TIMER_1.
 */
static void TIMER_1_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_1_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_1_example(void)
{
	TIMER_1_task1.interval = 100;
	TIMER_1_task1.cb       = TIMER_1_task1_cb;
	TIMER_1_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_1_task2.interval = 200;
	TIMER_1_task2.cb       = TIMER_1_task2_cb;
	TIMER_1_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_1, &TIMER_1_task1);
	timer_add_task(&TIMER_1, &TIMER_1_task2);
	timer_start(&TIMER_1);
}

static struct timer_task TIMER_2_task1, TIMER_2_task2;

/**
 * Example of using TIMER_2.
 */
static void TIMER_2_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_2_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_2_example(void)
{
	TIMER_2_task1.interval = 100;
	TIMER_2_task1.cb       = TIMER_2_task1_cb;
	TIMER_2_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_2_task2.interval = 200;
	TIMER_2_task2.cb       = TIMER_2_task2_cb;
	TIMER_2_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_2, &TIMER_2_task1);
	timer_add_task(&TIMER_2, &TIMER_2_task2);
	timer_start(&TIMER_2);
}

static struct timer_task TIMER_3_task1, TIMER_3_task2;

/**
 * Example of using TIMER_3.
 */
static void TIMER_3_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_3_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_3_example(void)
{
	TIMER_3_task1.interval = 100;
	TIMER_3_task1.cb       = TIMER_3_task1_cb;
	TIMER_3_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_3_task2.interval = 200;
	TIMER_3_task2.cb       = TIMER_3_task2_cb;
	TIMER_3_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_3, &TIMER_3_task1);
	timer_add_task(&TIMER_3, &TIMER_3_task2);
	timer_start(&TIMER_3);
}

static struct timer_task TIMER_4_task1, TIMER_4_task2;

/**
 * Example of using TIMER_4.
 */
static void TIMER_4_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_4_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_4_example(void)
{
	TIMER_4_task1.interval = 100;
	TIMER_4_task1.cb       = TIMER_4_task1_cb;
	TIMER_4_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_4_task2.interval = 200;
	TIMER_4_task2.cb       = TIMER_4_task2_cb;
	TIMER_4_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_4, &TIMER_4_task1);
	timer_add_task(&TIMER_4, &TIMER_4_task2);
	timer_start(&TIMER_4);
}

static struct timer_task TIMER_5_task1, TIMER_5_task2;

/**
 * Example of using TIMER_5.
 */
static void TIMER_5_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_5_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_5_example(void)
{
	TIMER_5_task1.interval = 100;
	TIMER_5_task1.cb       = TIMER_5_task1_cb;
	TIMER_5_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_5_task2.interval = 200;
	TIMER_5_task2.cb       = TIMER_5_task2_cb;
	TIMER_5_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_5, &TIMER_5_task1);
	timer_add_task(&TIMER_5, &TIMER_5_task2);
	timer_start(&TIMER_5);
}

static struct timer_task TIMER_6_task1, TIMER_6_task2;

/**
 * Example of using TIMER_6.
 */
static void TIMER_6_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_6_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_6_example(void)
{
	TIMER_6_task1.interval = 100;
	TIMER_6_task1.cb       = TIMER_6_task1_cb;
	TIMER_6_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_6_task2.interval = 200;
	TIMER_6_task2.cb       = TIMER_6_task2_cb;
	TIMER_6_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_6, &TIMER_6_task1);
	timer_add_task(&TIMER_6, &TIMER_6_task2);
	timer_start(&TIMER_6);
}

/**
 * Example of using TIMER_7.
 */
struct timer_task TIMER_7_task1, TIMER_7_task2;

static void TIMER_7_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_7_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_7_example(void)
{
	TIMER_7_task1.interval = 100;
	TIMER_7_task1.cb       = TIMER_7_task1_cb;
	TIMER_7_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_7_task2.interval = 200;
	TIMER_7_task2.cb       = TIMER_7_task2_cb;
	TIMER_7_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_7, &TIMER_7_task1);
	timer_add_task(&TIMER_7, &TIMER_7_task2);
	timer_start(&TIMER_7);
}

/**
 * Example of using TIMER_8.
 */
struct timer_task TIMER_8_task1, TIMER_8_task2;

static void TIMER_8_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_8_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_8_example(void)
{
	TIMER_8_task1.interval = 100;
	TIMER_8_task1.cb       = TIMER_8_task1_cb;
	TIMER_8_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_8_task2.interval = 200;
	TIMER_8_task2.cb       = TIMER_8_task2_cb;
	TIMER_8_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_8, &TIMER_8_task1);
	timer_add_task(&TIMER_8, &TIMER_8_task2);
	timer_start(&TIMER_8);
}

/**
 * Example of using WDT_0.
 */
void WDT_0_example(void)
{
	uint32_t clk_rate;
	uint16_t timeout_period;

	clk_rate       = 1000;
	timeout_period = 4096;
	wdt_set_timeout_period(&WDT_0, clk_rate, timeout_period);
	wdt_enable(&WDT_0);
}

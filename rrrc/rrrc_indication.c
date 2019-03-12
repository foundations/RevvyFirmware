/*
 * rrrc_indication.c
 *
 * Created: 10-Mar-19 01:48:33
 *  Author: vmyakovlev
 */ 

#include "rrrc_hal.h"
#include "rrrc_indication.h"

led_val_t status_leds[STATUS_LEDS_AMOUNT];
led_val_t ring_leds[RING_LEDS_AMOUNT];

//led_val_t frame_leds[(STATUS_LEDS_AMOUNT+RING_LEDS_AMOUNT)];
uint8_t frame_leds[(sizeof(led_val_t)*(STATUS_LEDS_AMOUNT+RING_LEDS_AMOUNT)*4)];

static struct timer_task indication_thread;

static void indication_thread_tick_cb(const struct timer_task *const timer_task)
{

	return;
}


#define SPI_BUFF_SIZE (16*3*8/2)
static uint8_t example_SPI_0[SPI_BUFF_SIZE];
static void tx_complete_cb_SPI_0(struct _dma_resource *resource)
{
	/* Transfer completed */

	while (1)
	{
		__NOP();
	}
}

uint32_t IndicationInit(){
	uint32_t result = ERR_NONE;

// 	struct io_descriptor *io;
// 	spi_m_sync_get_io_descriptor(&SPI_0, &io);
// 	
// 	spi_m_sync_enable(&SPI_0);
// 	
// 	memset(example_SPI_0, 0xE1, SPI_BUFF_SIZE);
// 	/* Replace with your application code */
// 	while (1) {
// 		io_write(io, example_SPI_0, SPI_BUFF_SIZE);
// 

	struct io_descriptor *io;
	spi_m_dma_get_io_descriptor(&SPI_0, &io);

	spi_m_dma_register_callback(&SPI_0, SPI_M_DMA_CB_TX_DONE, tx_complete_cb_SPI_0);
	spi_m_dma_enable(&SPI_0);
	io_write(io, example_SPI_0, SPI_BUFF_SIZE);
	io_write(io, example_SPI_0, SPI_BUFF_SIZE);
	io_write(io, example_SPI_0, SPI_BUFF_SIZE);
	io_write(io, example_SPI_0, SPI_BUFF_SIZE);
	while (1) {
	}
	result = RRRC_add_task(&indication_thread, &indication_thread_tick_cb, 33/*ms as fps*/, NULL, false);
	return result;}

uint32_t IndicationDeInit()
{
	uint32_t result = ERR_NONE;
	
	RRRC_remove_task(&indication_thread);

	return result;	
}
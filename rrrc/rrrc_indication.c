/*
 * rrrc_indication.c
 *
 * Created: 10-Mar-19 01:48:33
 *  Author: vmyakovlev
 */ 

#include "rrrc_hal.h"
#include "rrrc_indication.h"
#define RED 0x00,0x00,0x00
#define GREEN 0x55,0x55,0x55
#define BLUE 0x55,0x55,0x55


led_val_t status_leds[STATUS_LEDS_AMOUNT] = 
{
	{RED},
	{GREEN},
	{BLUE},
	{0x03,0x03,0x03}
};

led_val_t ring_leds[RING_LEDS_AMOUNT] = 
{
	{0x55,0x00,0x00},
	{0x00,0x55,0x00},
	{0x00,0x00,0x55},

	{0x55,0x00,0x00},
	{0x00,0x55,0x00},
	{0x00,0x00,0x55},

	{0x55,0x00,0x00},
	{0x00,0x55,0x00},
	{0x00,0x00,0x55},

	{0x55,0x00,0x00},
	{0x00,0x55,0x00},
	{0x55,0x55,0x55},
};

#define LED_RESET_SIZE 6 
uint8_t frame_leds[LED_RESET_SIZE+(sizeof(led_val_t)*(STATUS_LEDS_AMOUNT+RING_LEDS_AMOUNT)*8)];

static struct timer_task indication_thread;

static void tx_complete_cb_SPI_0(struct _dma_resource *resource)
{
	return;
}

#define LED_VAL_ZERO 0xC0
#define LED_VAL_ONE 0x3C
#define LED_VAL_RES 0x00

static void MakeLedBuffer()
{
	uint32_t frame_idx = 0;
	for (int32_t idx=0; idx<LED_RESET_SIZE; idx++)
		frame_leds[frame_idx++] = LED_VAL_RES;
	
	for(uint32_t idx=0; idx<STATUS_LEDS_AMOUNT; idx++)
	{
		for(int32_t bit=7; bit>=0; bit--)
		{
			uint8_t bit_val = status_leds[idx].G>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
		for(int32_t bit=7; bit>=0; bit--)
		{
			uint8_t bit_val = status_leds[idx].R>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
		for(int32_t bit=7; bit>=0; bit--)
		{
			uint8_t bit_val = status_leds[idx].B>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
	}

	for(uint32_t idx=0; idx<RING_LEDS_AMOUNT; idx++)
	{
		for(int32_t bit=7; bit>=0; bit--)
		{
			uint8_t bit_val = ring_leds[idx].G>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
		for(int32_t bit=7; bit>=0; bit--)
		{
			uint8_t bit_val = ring_leds[idx].R>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
		for(int32_t bit=7; bit>=0; bit--)
		{
			uint8_t bit_val = ring_leds[idx].B>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
	}
}


static void indication_thread_tick_cb(const struct timer_task *const timer_task)
{

	MakeLedBuffer();
	struct io_descriptor *io;
	spi_m_dma_get_io_descriptor(&SPI_0, &io);

	spi_m_dma_register_callback(&SPI_0, SPI_M_DMA_CB_TX_DONE, tx_complete_cb_SPI_0);
	spi_m_dma_enable(&SPI_0);
	io_write(io, frame_leds, ARRAY_SIZE(frame_leds));
	return;
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

	result = RRRC_add_task(&indication_thread, &indication_thread_tick_cb, 33/*ms as fps*/, NULL, false);
	return result;}

uint32_t IndicationDeInit()
{
	uint32_t result = ERR_NONE;
	
	RRRC_remove_task(&indication_thread);

	return result;	
}
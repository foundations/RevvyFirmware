/*
 * rrrc_indication.c
 *
 * Created: 10-Mar-19 01:48:33
 *  Author: vmyakovlev
 */ 

#include "rrrc_hal.h"
#include "rrrc_indication.h"
#include "rrrc_indication_predefined.h"



led_status_t status_leds = 
{
	{LED_RED},
	{LED_GREEN},
	{LED_BLUE},
	{LED_YELLOW}
};

uint32_t frame_max = 0;
uint32_t frame_curr = 0;
enum INDICATON_RING_TYPE led_ring_mode = RING_LED_OFF;

static led_ring_frame_t led_ring_userframes[LEDS_USER_MAX_FRAMES] = {0x00};

static led_ring_frame_t* Led_ring_curr_buff = NULL;

#define LED_RESET_SIZE 50 
uint8_t frame_leds[LED_RESET_SIZE+(sizeof(led_val_t)*(STATUS_LEDS_AMOUNT+RING_LEDS_AMOUNT)*8)];

static TaskHandle_t      xIndicationTask;
//*********************************************************************************************
static void tx_complete_cb_SPI_0(struct _dma_resource *resource)
{
	return;
}

//*********************************************************************************************
static void MakeLedBuffer()
{
#define LED_VAL_ZERO 0xC0
#define LED_VAL_ONE 0x3C
#define LED_VAL_RES 0x00

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
			uint8_t bit_val = Led_ring_curr_buff[frame_curr][idx].G>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
		for(int32_t bit=7; bit>=0; bit--)
		{
			uint8_t bit_val = Led_ring_curr_buff[frame_curr][idx].R>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
		for(int32_t bit=7; bit>=0; bit--)
		{
			uint8_t bit_val = Led_ring_curr_buff[frame_curr][idx].B>>bit;
			uint8_t byte_val = LED_VAL_ZERO;
			if (bit_val)
				byte_val |= LED_VAL_ONE;
			frame_leds[frame_idx++] = byte_val;
		}
	}
	frame_curr++;
	if (frame_curr>=frame_max)
		frame_curr = 0;
}

//*********************************************************************************************
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

static void Indication_xTask(const void* user_data)
{

	for(;;)
	{
		MakeLedBuffer();
		struct io_descriptor *io;
		spi_m_dma_get_io_descriptor(&SPI_0, &io);

		spi_m_dma_register_callback(&SPI_0, SPI_M_DMA_CB_TX_DONE, tx_complete_cb_SPI_0);
		spi_m_dma_enable(&SPI_0);
		io_write(io, frame_leds, ARRAY_SIZE(frame_leds));
		os_sleep(100);
	}
	return;
}

//*********************************************************************************************
int32_t IndicationUpdateUserFrame(uint32_t frame_id, led_ring_frame_t* frame)
{
	if (frame_id<LEDS_USER_MAX_FRAMES && frame && led_ring_mode==RING_LED_USER)
	{
		memcpy(&led_ring_userframes[frame_id], frame, sizeof(led_ring_frame_t));
		if (frame_id>=frame_max)
			frame_max = frame_id+1;
		return ERR_NONE;
	}
	return ERR_INVALID_ARG;
}

//*********************************************************************************************
int32_t IndicationSetType(enum INDICATON_RING_TYPE type)
{
	int32_t status = ERR_NONE;
	switch (type)
	{
	case RING_LED_OFF:
		Led_ring_curr_buff = led_ring_userframes;
		memset(Led_ring_curr_buff, 0, ARRAY_SIZE(led_ring_userframes));
		frame_curr = 0;
		frame_max = 1;
		break;
	case RING_LED_USER:
		Led_ring_curr_buff = led_ring_userframes;
		memset(Led_ring_curr_buff, 0, ARRAY_SIZE(led_ring_userframes));
		frame_curr = 0;
		frame_max = 1;
		break;
	case RING_LED_PREDEF_1:
		Led_ring_curr_buff = ring_leds_round_red;
		frame_curr = 0;
		frame_max = ARRAY_SIZE(ring_leds_round_red);
		break;
	case RING_LED_PREDEF_2:
		Led_ring_curr_buff = ring_leds_round_green;
		frame_curr = 0;
		frame_max = ARRAY_SIZE(ring_leds_round_green);
		break;
	case RING_LED_PREDEF_3:
			Led_ring_curr_buff = ring_leds_round_blue;
			frame_curr = 0;
			frame_max = ARRAY_SIZE(ring_leds_round_blue);
			break;
	case RING_LED_PREDEF_4:
		Led_ring_curr_buff = ring_leds_running_fire_blue;
		frame_curr = 0;
		frame_max = ARRAY_SIZE(ring_leds_running_fire_blue);
		break;
	default:
		status = ERR_INVALID_DATA;
		break;
	}
	return status;
}

//*********************************************************************************************
uint32_t IndicationInit(){
	uint32_t result = ERR_NONE;
	IndicationSetType(RING_LED_PREDEF_4);

	char task_name[configMAX_TASK_NAME_LEN+1] = "Indication";
	if (xTaskCreate(Indication_xTask, task_name, 256 / sizeof(portSTACK_TYPE), NULL, tskIDLE_PRIORITY+1, &xIndicationTask) != pdPASS) 
		return ERR_FAILURE;

	return result;}
//*********************************************************************************************
uint32_t IndicationDeInit()
{
	uint32_t result = ERR_NONE;
	
	vTaskDelete(xIndicationTask);

	return result;	
}
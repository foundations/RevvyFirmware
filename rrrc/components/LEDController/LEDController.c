/*
 * LEDController.c
 *
 * Created: 03/05/2019 10:31:19
 *  Author: Dániel Buga
 */

#include "LEDController.h"
#include "rrrc_hal.h"
#include "rrrc_indication.h"

#include <stdint.h>

#define LED_VAL_ZERO    ((uint8_t) 0x3Fu)
#define LED_VAL_ONE     ((uint8_t) 0x03u)
#define LED_VAL_RES     ((uint8_t) 0xFFu)
#define LED_RESET_SIZE  ((size_t)    50u)

#define LED_BYTE_SIZE   8                   /* one LED control bit is coded as 8 MCU bits, so 1 byte -> 8 bytes */
#define LED_COLOR_SIZE  3 * LED_BYTE_SIZE   /* 24 LED bits in total */
#define LED_FRAME_SIZE  (LED_RESET_SIZE + (LED_COLOR_SIZE * (STATUS_LEDS_AMOUNT + RING_LEDS_AMOUNT)))

static bool ledsUpdating;
static uint8_t frame_leds[LED_FRAME_SIZE];

static struct spi_m_dma_descriptor SPI_0;
static struct io_descriptor *io_descr;

//*********************************************************************************************
static void SPI_0_Init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_SLOW, CONF_GCLK_SERCOM4_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBDMASK_SERCOM4_bit(MCLK);

    spi_m_dma_init(&SPI_0, SERCOM4);

    gpio_set_pin_level(WS2812pin, false);
    gpio_set_pin_direction(WS2812pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(WS2812pin, GPIO_PULL_OFF);
    gpio_set_pin_function(WS2812pin, WS2812pin_function);
    //gpio_set_pin_drive(WS2812pin, GPIO_DRIVE_STRONG);
}

static inline uint8_t getLedBitPattern(uint8_t bitValue)
{
    return (bitValue == 0u) ? LED_VAL_ZERO : LED_VAL_ONE;
}

static inline void write_led_byte(uint32_t led_idx, uint32_t byte_idx, uint8_t byte_value)
{
    const uint32_t startIdx = 8u * (3u * led_idx + byte_idx); // TODO optimizer handles this?
    for (uint32_t i = 0u; i < 8u; i++)
    {
        frame_leds[startIdx + i] = getLedBitPattern(byte_value & (0x80u >> i));
    }
}

static inline void write_led_color(uint32_t led_idx, rgb_t color)
{
    write_led_byte(led_idx, 0u, color.G);
    write_led_byte(led_idx, 1u, color.R);
    write_led_byte(led_idx, 2u, color.B);
}

static void update_frame(void)
{
    for (uint32_t i = 0u; i < STATUS_LEDS_AMOUNT; i++)
    {
        write_led_color(i, LEDController_Read_StatusLED(i));
    }

    for (uint32_t i = 0u; i < RING_LEDS_AMOUNT; i++)
    {
        write_led_color(4u + i, LEDController_Read_RingLED(i));
    }
}

static void send_frame(void)
{
    ledsUpdating = true;
    io_write(io_descr, frame_leds, ARRAY_SIZE(frame_leds));
}

static void tx_complete_cb_SPI_0(struct _dma_resource *resource)
{
    ledsUpdating = false;
}

void LEDController_Run_OnInit(void)
{
    ledsUpdating = false;

    SPI_0_Init();

    spi_m_dma_get_io_descriptor(&SPI_0, &io_descr);
    spi_m_dma_register_callback(&SPI_0, SPI_M_DMA_CB_TX_DONE, &tx_complete_cb_SPI_0);
    spi_m_dma_enable(&SPI_0);
}

void LEDController_Run_Update(void)
{
    if (!ledsUpdating)
    {
        update_frame();
        send_frame();
    }
}

__WEAK rgb_t LEDController_Read_StatusLED(uint32_t led_idx)
{
    return (rgb_t){0, 0, 0};
}

__WEAK rgb_t LEDController_Read_RingLED(uint32_t led_idx)
{
    return (rgb_t){0, 0, 0};
}

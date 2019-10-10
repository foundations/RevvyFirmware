#include "LEDController.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include "rrrc_hal.h"
#include "rrrc_indication.h"
#include "utils/functions.h"

#include <stdint.h>
#include <string.h>

#define LEDS_AMOUNT  ((uint8_t) 16u)

#define LED_VAL_ZERO    ((uint8_t) 0xC0u)
#define LED_VAL_ONE     ((uint8_t) 0xFCu)
#define LED_VAL_RES     ((uint8_t) 0x00u)
#define LED_RESET_SIZE  ((size_t)    50u)

#define LED_BYTE_SIZE   8                   /* one LED control bit is coded as 8 MCU bits, so 1 byte -> 8 bytes */
#define LED_COLOR_SIZE  3 * LED_BYTE_SIZE   /* 24 LED bits in total */
#define LED_FRAME_SIZE  (LED_RESET_SIZE + (LED_COLOR_SIZE * (LEDS_AMOUNT + 1)))

static bool ledsUpdating;

static uint8_t led_one;
static uint8_t led_zero;

static uint8_t frame_leds[LED_FRAME_SIZE];

static struct spi_m_dma_descriptor SPI_0;
static struct io_descriptor *io_descr;

static void SPI_0_Init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM4_GCLK_ID_SLOW, CONF_GCLK_SERCOM4_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBDMASK_SERCOM4_bit(MCLK);

    spi_m_dma_init(&SPI_0, WS2812spi);

    gpio_set_pin_level(WS2812pin, false);
    gpio_set_pin_direction(WS2812pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(WS2812pin, GPIO_PULL_OFF);
    gpio_set_pin_function(WS2812pin, WS2812pin_function);
    //gpio_set_pin_drive(WS2812pin, GPIO_DRIVE_STRONG);
}

static inline uint8_t getLedBitPattern(uint8_t bitValue)
{
    return (bitValue == 0u) ? led_zero : led_one;
}

static inline void write_led_byte(uint32_t led_idx, uint32_t byte_idx, uint8_t byte_value)
{
    const uint32_t startIdx = LED_RESET_SIZE + 8u * (3u * led_idx + byte_idx); // TODO optimizer handles this?
    for (uint32_t i = 0u; i < 8u; i++)
    {
        frame_leds[startIdx + i] = getLedBitPattern(byte_value & (0x80u >> i));
    }
}

static inline void write_led_color(uint32_t led_idx, rgb_t color)
{
    /* brightness scaling */
    uint8_t max_brightness = LEDController_Read_MaxBrightness();
    rgb_t rgb_dimmed = rgb_change_brightness(color, max_brightness / 255.0f);

    write_led_byte(led_idx, 0u, rgb_dimmed.G);
    write_led_byte(led_idx, 1u, rgb_dimmed.R);
    write_led_byte(led_idx, 2u, rgb_dimmed.B);
}

static void update_frame(void)
{
    for (uint32_t i = 0u; i < LEDS_AMOUNT; i++)
    {
        write_led_color(i, LEDController_Read_Colors(i));
    }

    /* dummy data to prevent last LED not receive its color properly */
    write_led_color(LEDS_AMOUNT, (rgb_t) LED_OFF);
}

static void send_frame(void)
{
    ledsUpdating = true;

    io_write(io_descr, frame_leds, ARRAY_SIZE(frame_leds));
    gpio_set_pin_function(WS2812pin, WS2812pin_function);
}

static void tx_complete_cb_SPI_0(struct _dma_resource *resource)
{
    (void) resource;
    ledsUpdating = false;
    bool inverted = (FLASH_HEADER->hw_version != 0u);
    gpio_set_pin_level(WS2812pin, inverted ? true : false);
    gpio_set_pin_function(WS2812pin, GPIO_PIN_FUNCTION_OFF);
}
/* End User Code Section: Declarations */

void LEDController_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    ledsUpdating = false;

    /* invert here so the constants are easier to understand */
    led_one = (uint8_t) ~LED_VAL_ONE;
    led_zero = (uint8_t) ~LED_VAL_ZERO;
    uint8_t led_reset = (uint8_t) ~LED_VAL_RES;

    memset(frame_leds, led_reset, sizeof(frame_leds));

    SPI_0_Init();

    spi_m_dma_get_io_descriptor(&SPI_0, &io_descr);
    spi_m_dma_register_callback(&SPI_0, SPI_M_DMA_CB_TX_DONE, &tx_complete_cb_SPI_0);
    spi_m_dma_enable(&SPI_0);
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void LEDController_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    if (!ledsUpdating)
    {
        update_frame();
        send_frame();
    }
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
rgb_t LEDController_Read_Colors(uint32_t index)
{
    ASSERT(index < 16);
    /* Begin User Code Section: Colors Start */

    /* End User Code Section: Colors Start */
    /* Begin User Code Section: Colors End */

    /* End User Code Section: Colors End */
    return (rgb_t){0, 0, 0};
}

__attribute__((weak))
uint8_t LEDController_Read_MaxBrightness(void)
{
    /* Begin User Code Section: MaxBrightness Start */

    /* End User Code Section: MaxBrightness Start */
    /* Begin User Code Section: MaxBrightness End */

    /* End User Code Section: MaxBrightness End */
    return 24u;
}

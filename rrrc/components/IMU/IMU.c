/*
 * IMU.c
 *
 * Created: 2019. 07. 25. 16:31:24
 *  Author: Dániel Buga
 */ 

#include "IMU.h"
#include "rrrc_hal.h"
#include <hal_spi_m_dma.h>

#define IMU_REGISTER_WHOAMI ((uint8_t) 0x0Fu)

struct spi_m_sync_descriptor spi;

void IMU_Run_OnInit(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM5_GCLK_ID_CORE, CONF_GCLK_SERCOM5_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM5_GCLK_ID_SLOW, CONF_GCLK_SERCOM5_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBDMASK_SERCOM5_bit(MCLK);

    gpio_set_pin_level(IMU_MOSI_pin, false);
    gpio_set_pin_direction(IMU_MOSI_pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(IMU_MOSI_pin, IMU_MOSI_pin_function);

    gpio_set_pin_level(IMU_SCLK_pin, false);
    gpio_set_pin_direction(IMU_SCLK_pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(IMU_SCLK_pin, IMU_SCLK_pin_function);

    gpio_set_pin_direction(IMU_MISO_pin, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(IMU_MISO_pin, GPIO_PULL_OFF);
    gpio_set_pin_function(IMU_MISO_pin, IMU_MISO_pin_function);

    /* CS permanently tied to ground */
    gpio_set_pin_level(IMU_CS_pin, false);
    gpio_set_pin_direction(IMU_CS_pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(IMU_CS_pin, IMU_CS_pin_function);

    spi_m_sync_init(&spi, IMU_SERCOM);
    spi_m_sync_enable(&spi);
}

static inline void _imu_send_write_address(uint8_t addr)
{
    uint8_t address = addr & 0x7Fu;

    struct spi_xfer xfer;

    xfer.txbuf = &address;
    xfer.rxbuf = NULL;
    xfer.size = 1u;
    
    spi_m_sync_transfer(&spi, &xfer);
}

static inline void _imu_send_read_address(uint8_t addr)
{
    uint8_t address = 0x80u | (addr & 0x7Fu);

    struct spi_xfer xfer;

    xfer.txbuf = &address;
    xfer.rxbuf = NULL;
    xfer.size = 1u;
    
    spi_m_sync_transfer(&spi, &xfer);
}

static void _imu_write_registers(uint8_t reg, const uint8_t* pData, size_t data_count)
{
    struct spi_xfer xfer;

    xfer.txbuf = pData;
    xfer.rxbuf = NULL;
    xfer.size = data_count;
    
    _imu_send_write_address(reg);
    
    spi_m_sync_transfer(&spi, &xfer);
}

static void _imu_read_registers(uint8_t reg, uint8_t* pData, size_t data_count)
{
    struct spi_xfer xfer;

    xfer.txbuf = NULL;
    xfer.rxbuf = pData;
    xfer.size = data_count;

    _imu_send_read_address(reg);
    
    spi_m_sync_transfer(&spi, &xfer);
}

static void _imu_write_register(uint8_t reg, uint8_t data)
{
    uint8_t rx;

    _imu_write_registers(reg, &data, 1u);

    return rx;
}

static uint8_t _imu_read_register(uint8_t reg)
{
    uint8_t rx;

    _imu_read_registers(reg, &rx, 1u);

    return rx;
}

void IMU_Run_OnUpdate(void)
{
    uint8_t whoami = _imu_read_register(IMU_REGISTER_WHOAMI);
    ASSERT(whoami == 0x6Au);
}

__attribute__((weak))
void IMU_Write_Sample(const IMU_Sample_t* sample)
{
    /* nothing to do here */
}

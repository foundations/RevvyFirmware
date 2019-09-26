/*
 * i2cHal.c
 *
 * Created: 07/05/2019 11:42:39
 *  Author: Dániel Buga
 */ 

#include "i2cHal.h"
#include "utils_assert.h"

#define I2C_DIR_MASTER_TX  0u
#define I2C_DIR_MASTER_RX  1u

static const uint8_t empty_tx_buffer[] = { 0xFFu };

typedef struct
{
    struct _i2c_s_async_device device;

    const uint8_t* txBuffer;
    size_t         txBufferLast;
    size_t         txBufferIndex;

    const uint8_t*  nextTxBuffer;
    volatile size_t nextTxBufferLast;

    uint8_t*        rxBuffer;
    volatile size_t rxBufferSize;
    size_t          rxBufferCount;
} i2c_hal_descriptor;

static void i2c_hal_on_address_matched(const uint8_t dir);
static void i2c_hal_on_error(void);
static void i2c_hal_on_rx_done(const uint8_t data);
static void i2c_hal_on_tx(void);
static void i2c_hal_on_stop(const uint8_t dir);

static i2c_hal_descriptor descriptor;

int32_t i2c_hal_init(void* hw)
{
    int32_t result = _i2c_s_async_init(&descriptor.device, hw);

    if (result == ERR_NONE)
    {
        descriptor.nextTxBuffer = empty_tx_buffer;
        descriptor.nextTxBufferLast = ARRAY_SIZE(empty_tx_buffer) - 1u;
        
        descriptor.txBuffer = empty_tx_buffer;
        descriptor.txBufferLast = ARRAY_SIZE(empty_tx_buffer) - 1u;
        descriptor.txBufferIndex = 0u;

        descriptor.rxBuffer = NULL;
        descriptor.rxBufferSize = 0u;
        descriptor.rxBufferCount = 0u;
        
        descriptor.device.cb.addrm_cb   = &i2c_hal_on_address_matched;
        descriptor.device.cb.error_cb   = &i2c_hal_on_error;
        descriptor.device.cb.rx_done_cb = &i2c_hal_on_rx_done;
        descriptor.device.cb.tx_cb      = &i2c_hal_on_tx;
        descriptor.device.cb.stop_cb    = &i2c_hal_on_stop;

        hri_sercomi2cs_set_INTEN_ERROR_bit(descriptor.device.hw);
        hri_sercomi2cs_set_INTEN_AMATCH_bit(descriptor.device.hw);
        hri_sercomi2cs_set_INTEN_PREC_bit(descriptor.device.hw);
        hri_sercomi2cs_set_INTEN_DRDY_bit(descriptor.device.hw);

        result = _i2c_s_async_enable(&descriptor.device);
    }

    return result;
}

void i2c_hal_receive(uint8_t* buffer, size_t bufferSize)
{
    /* set size to 0 to avoid needing a critical section */
    descriptor.rxBufferSize = 0u;
    descriptor.rxBuffer = buffer;
    descriptor.rxBufferCount = 0u;
    descriptor.rxBufferSize = bufferSize;
}

void i2c_hal_set_tx_buffer(const uint8_t* buffer, size_t bufferSize)
{
    if (bufferSize == 0u)
    {
        buffer = empty_tx_buffer;
        bufferSize = ARRAY_SIZE(empty_tx_buffer);
    }

    __disable_irq();
    descriptor.nextTxBuffer = buffer;
    descriptor.nextTxBufferLast = bufferSize - 1u;
    __enable_irq();
}

/* interrupt handlers */
static void i2c_hal_on_address_matched(const uint8_t dir)
{
    if (dir == I2C_DIR_MASTER_TX)
    {
        descriptor.rxBufferCount = 0u;
        i2c_hal_rx_started();
    }
    else
    {
        descriptor.txBuffer = descriptor.nextTxBuffer;
        descriptor.txBufferLast = descriptor.nextTxBufferLast;
        descriptor.txBufferIndex = 0u;
    }
}

static void i2c_hal_on_error(void)
{
    /* ignore for now */
}

static void i2c_hal_on_rx_done(const uint8_t data)
{
    if (descriptor.rxBufferCount < descriptor.rxBufferSize)
    {
        descriptor.rxBuffer[descriptor.rxBufferCount] = data;
    }
    
    ++descriptor.rxBufferCount;
}

static void i2c_hal_on_tx(void)
{
    uint8_t byte = descriptor.txBuffer[descriptor.txBufferIndex];
    _i2c_s_async_write_byte(&descriptor.device, byte);

    if (descriptor.txBufferIndex == descriptor.txBufferLast)
    {
        descriptor.txBufferIndex = 0u;
    }
    else
    {
        ++descriptor.txBufferIndex;
    }
}

static void i2c_hal_on_stop(const uint8_t dir)
{
    if (dir == I2C_DIR_MASTER_TX)
    {
        i2c_hal_rx_complete(descriptor.rxBuffer, descriptor.rxBufferSize, descriptor.rxBufferCount);
    }
    else
    {
        /* nothing to do (yet) */
    }
}

__attribute__((weak))
void i2c_hal_rx_started(void)
{
}

__attribute__((weak))
void i2c_hal_rx_complete(const uint8_t* buffer, size_t bufferSize, size_t bytesReceived)
{
    (void) buffer;
    (void) bufferSize;
    (void) bytesReceived;
}

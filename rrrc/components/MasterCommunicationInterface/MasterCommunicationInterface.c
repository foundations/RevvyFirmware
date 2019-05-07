/*
 * MasterCommunicationInterface.c
 *
 * Created: 07/05/2019 10:34:21
 *  Author: Dániel Buga
 */ 

#include "MasterCommunicationInterface.h"

#include "i2cHal.h"

#include "rrrc_hal.h"
#include "rrrc_i2c_protocol.h"
#include "driver_init.h"
#include <peripheral_clk_config.h>

extern void rrrc_i2c_s_async_tx(struct _i2c_s_async_device *const device);
extern void rrrc_i2c_s_async_byte_received(struct _i2c_s_async_device *const device, const uint8_t data);
extern void rrrc_i2c_s_async_error(struct _i2c_s_async_device *const device);
extern void rrrc_i2c_s_async_stop(struct _i2c_s_async_device *const device, const uint8_t dir);
extern void rrrc_i2c_s_async_addr_match(struct _i2c_s_async_device *const device, const uint8_t dir);

static TaskHandle_t communicationTaskHandle;
struct i2c_s_async_descriptor I2C_0;

static uint8_t* nextTransmittedBuffer;
static size_t nextTransmittedBufferSize;

//*********************************************************************************************
static void I2C_0_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_SLOW, CONF_GCLK_SERCOM2_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBBMASK_SERCOM2_bit(MCLK);

    gpio_set_pin_pull_mode(I2C0_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C0_SDApin, I2C0_SDApin_function);
    gpio_set_pin_pull_mode(I2C0_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C0_SCLpin, I2C0_SCLpin_function);

    i2c_s_async_init(&I2C_0, I2C0_SERCOM);

    I2C_0.device.cb.addrm_cb   = rrrc_i2c_s_async_addr_match;
    I2C_0.device.cb.tx_cb      = rrrc_i2c_s_async_tx;
    I2C_0.device.cb.rx_done_cb = rrrc_i2c_s_async_byte_received;
    I2C_0.device.cb.stop_cb    = rrrc_i2c_s_async_stop;
    I2C_0.device.cb.error_cb   = rrrc_i2c_s_async_error;
}

static void CommunicationTask(void* user_data)
{
    const MasterCommunicationInterface_Config_t* config = user_data;
    i2c_s_async_enable(&I2C_0);
    for(;;)
    {
        if (ulTaskNotifyTake(pdTRUE, rtos_ms_to_ticks(100u)) == 0u)
        {
            // TODO handle communication fault
        }
        else
        {
            MasterCommunicationInterface_Run_SetResponse(config->defaultResponseBuffer, config->defaultResponseLength);
            MasterCommunicationInterface_Call_OnMessageReceived(rx_buffer.buff, rx_buffer.size);
        }
    }
}

void MasterCommunicationInterface_Run_OnInit(const MasterCommunicationInterface_Config_t* config)
{
    I2C_0_init();
    
    if (xTaskCreate(&CommunicationTask, "RPiComm", 1024, config, taskPriority_Communication, &communicationTaskHandle) != pdPASS)
    {
        ASSERT(0);
    }
}

__attribute__((weak))
void MasterCommunicationInterface_Call_OnMessageReceived(const uint8_t* buffer, size_t bufferSize)
{
    /* nothing to do */
}

void MasterCommunicationInterface_Run_SetResponse(const uint8_t* buffer, size_t bufferSize)
{
    portENTER_CRITICAL();
    nextTransmittedBuffer = buffer;
    nextTransmittedBufferSize = bufferSize;
    portEXIT_CRITICAL();
}

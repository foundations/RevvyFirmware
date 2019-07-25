/*
 * MasterCommunicationInterface.c
 *
 * Created: 07/05/2019 10:34:21
 *  Author: Dániel Buga
 */

#include "MasterCommunicationInterface.h"

#include "i2cHal.h"
#include "rrrc_hal.h"

#include "driver_init.h"
#include "utils_assert.h"
#include "FreeRTOS.h"
#include "task.h"
#include <peripheral_clk_config.h>
#include <limits.h>

static TaskHandle_t communicationTaskHandle;

static uint8_t rxBuffer[255 + 6];

static i2c_hal_descriptor I2C_4;
const MasterCommunicationInterface_Config_t* config;

//*********************************************************************************************
static int32_t I2C_4_init(i2c_hal_descriptor* descriptor)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_SLOW, CONF_GCLK_SERCOM2_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBBMASK_SERCOM2_bit(MCLK);

    gpio_set_pin_pull_mode(I2C4_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C4_SDApin, I2C4_SDApin_function);
    gpio_set_pin_pull_mode(I2C4_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C4_SCLpin, I2C4_SCLpin_function);

    return i2c_hal_init(descriptor, I2C4_SERCOM);
}

static void CommunicationTask(void* user_data)
{
    config = (const MasterCommunicationInterface_Config_t*) user_data;

    (void) I2C_4_init(&I2C_4);
    
    i2c_hal_receive(&I2C_4, &rxBuffer[0], sizeof(rxBuffer));
    for(;;)
    {
        uint32_t bytesReceived;
        BaseType_t notified = xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &bytesReceived, rtos_ms_to_ticks(config->rxTimeout));

        if (!notified)
        {
            MasterCommunicationInterface_Call_RxTimeout();
        }
        else
        {
            if (bytesReceived <= sizeof(rxBuffer))
            {
                MasterCommunicationInterface_Call_OnMessageReceived(&rxBuffer[0], bytesReceived);
            }
            else
            {
                MasterCommunicationInterface_Run_SetResponse(config->longRxErrorResponseBuffer, config->longRxErrorResponseLength);
            }
        }
    }
}

void i2c_hal_rx_started(i2c_hal_descriptor* descr)
{
    /* setup a default response in case processing is slow */
    i2c_hal_set_tx_buffer(descr, config->defaultResponseBuffer, config->defaultResponseLength);
}

void i2c_hal_rx_complete(i2c_hal_descriptor* descr, const uint8_t* buffer, size_t bufferSize, size_t bytesReceived)
{
    (void) descr;
    (void) buffer;
    (void) bufferSize;

    ASSERT(communicationTaskHandle);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(communicationTaskHandle, bytesReceived, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void MasterCommunicationInterface_Run_OnInit(MasterCommunicationInterface_Config_t* config)
{
    BaseType_t success = xTaskCreate(&CommunicationTask, "RPiComm", 1024u, config, taskPriority_Communication, &communicationTaskHandle);
    ASSERT(success == pdPASS);
}

__attribute__((weak))
void MasterCommunicationInterface_Call_OnMessageReceived(const uint8_t* buffer, size_t bufferSize)
{
    (void) buffer;
    (void) bufferSize;
    /* nothing to do */
}

__attribute__((weak))
void MasterCommunicationInterface_Call_RxTimeout(void)
{
    /* nothing to do */
}

void MasterCommunicationInterface_Run_SetResponse(const uint8_t* buffer, size_t bufferSize)
{
    i2c_hal_set_tx_buffer(&I2C_4, buffer, bufferSize);
}

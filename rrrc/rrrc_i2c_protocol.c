/*
 * rrrc_i2c_protocol.c
 *
 * Created: 2/13/2019 7:54:51 PM
 *  Author: User
 */ 
#include "rrrc_hal.h"
#include "rrrc_i2c_protocol.h"
#include <utils_assert.h>

static TaskHandle_t xCommunicationTask;
extern trans_buffer_t rx_buffer;
extern trans_buffer_t tx_buffer;

extern void rrrc_i2c_transmit(size_t size);

void RRRC_Communication_xTask(void* user_data)
{
    for(;;)
    {
        if (ulTaskNotifyTake(pdTRUE, rtos_ms_to_ticks(100u)) == 0u)
        {
            // TODO handle communication fault
        }
        else
        {
            size_t size = CommandHandler_Handle((const commandBuffer_t*) rx_buffer.buff, (responseBuffer_t*) tx_buffer.buff);
        
            rrrc_i2c_transmit(size);
            rx_buffer.size = 0u;
        }
    }
}

int32_t RRRC_Communication_Init(void)
{
    int32_t ret = ERR_NONE;
    if (xTaskCreate(&RRRC_Communication_xTask, "RPiComm", 1024 / sizeof(portSTACK_TYPE), NULL, tskIDLE_PRIORITY + 2, &xCommunicationTask) != pdPASS)
    {
        ret = ERR_FAILURE;
    }
    else
    {
        i2c_s_async_enable(&I2C_0);
    }

    return ret;
}

int32_t RRRC_Communication_DeInit(void)
{
    int32_t ret = ERR_NONE;
    i2c_s_async_disable(&I2C_0);

    vTaskDelete(xCommunicationTask);
    return ret;
}

void CommunicationTask_NotifyRxCompleteFromISR(void)
{
    if (xCommunicationTask != NULL)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(xCommunicationTask, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

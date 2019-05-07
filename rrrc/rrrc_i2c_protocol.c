/*
 * rrrc_i2c_protocol.c
 *
 * Created: 2/13/2019 7:54:51 PM
 *  Author: User
 */ 
#include "rrrc_hal.h"
#include "rrrc_i2c_protocol.h"
#include <utils_assert.h>

extern trans_buffer_t rx_buffer;
extern trans_buffer_t tx_buffer;

extern void rrrc_i2c_transmit(size_t size);

int32_t RRRC_Communication_Init(void)
{
    int32_t ret = ERR_NONE;

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

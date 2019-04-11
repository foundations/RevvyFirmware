/*
 * rrrc_i2c_protocol.c
 *
 * Created: 2/13/2019 7:54:51 PM
 *  Author: User
 */ 
#include "rrrc_hal.h"
#include "rrrc_i2c_protocol.h"
#include <utils_assert.h>

TaskHandle_t xCommunicationTask;
extern trans_buffer_t rx_buffer;
extern trans_buffer_t tx_buffer;
void RRRC_Comunication_xTask(void* user_data)
{
	uint32_t ulNotifiedValue;

	for(;;)
	{
        (void) ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        size_t size = CommandHandler_Handle(rx_buffer.buff, rx_buffer.size, tx_buffer.buff, MAX_TRANSACTION_SIZE);
        tx_buffer.size = size;
        rx_buffer.size = 0u;
	}
}

int32_t RRRC_Comminicationc_Init()
{
	int32_t ret = ERR_NONE;
	if (xTaskCreate(RRRC_Comunication_xTask, "RPiComm", 1024 / sizeof(portSTACK_TYPE), NULL, tskIDLE_PRIORITY, &xCommunicationTask) != pdPASS)
		ret = ERR_FAILURE;
	else
		i2c_s_async_enable(&I2C_0);
	return ret;
}

int32_t RRRC_Comminicationc_DeInit()
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

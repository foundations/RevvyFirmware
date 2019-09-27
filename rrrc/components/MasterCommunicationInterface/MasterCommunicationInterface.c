#include "MasterCommunicationInterface.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include "i2cHal.h"
#include "rrrc_hal.h"

#include "driver_init.h"
#include "FreeRTOS.h"
#include "task.h"
#include <peripheral_clk_config.h>
#include <limits.h>

static TaskHandle_t communicationTaskHandle;

static uint8_t rxBuffer[255 + 6];

static MasterCommunicationInterface_Config_t config;

static int32_t I2C_4_init(void)
{
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_SLOW, CONF_GCLK_SERCOM2_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
    hri_mclk_set_APBBMASK_SERCOM2_bit(MCLK);

    gpio_set_pin_pull_mode(I2C4_SDApin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C4_SDApin, I2C4_SDApin_function);
    gpio_set_pin_pull_mode(I2C4_SCLpin, GPIO_PULL_OFF);
    gpio_set_pin_function(I2C4_SCLpin, I2C4_SCLpin_function);

    return i2c_hal_init(I2C4_SERCOM);
}

static void CommunicationTask(void* user_data)
{
    (void) user_data;

    i2c_hal_receive(&rxBuffer[0], sizeof(rxBuffer));
    for(;;)
    {
        uint32_t bytesReceived;
        BaseType_t notified = xTaskNotifyWait(ULONG_MAX, ULONG_MAX, &bytesReceived, config.rx_timeout);

        if (!notified)
        {
            MasterCommunicationInterface_Call_RxTimeout();
        }
        else
        {
            if (bytesReceived <= sizeof(rxBuffer))
            {
                MasterMessage_t message = { .payload = rxBuffer, .size = bytesReceived };
                MasterCommunicationInterface_Call_OnMessageReceived(message);
            }
            else
            {
                MasterCommunicationInterface_Run_SetResponse(config.rx_overflow_response);
            }
        }
    }
}

void i2c_hal_rx_started(void)
{
    /* setup a default response in case processing is slow */
    MasterCommunicationInterface_Run_SetResponse(config.default_response);
}

void i2c_hal_rx_complete(const uint8_t* buffer, size_t bufferSize, size_t bytesReceived)
{
    (void) buffer;
    (void) bufferSize;

    ASSERT(communicationTaskHandle);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(communicationTaskHandle, bytesReceived, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void i2c_hal_error_occurred(void)
{
    MasterCommunicationInterface_Call_LogError();
}
/* End User Code Section: Declarations */

void MasterCommunicationInterface_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    MasterCommunicationInterface_Read_Configuration(&config);

    int32_t result = I2C_4_init();
    ASSERT(result == ERR_NONE);

    BaseType_t success = xTaskCreate(&CommunicationTask, "RPiComm", 1024u, NULL, taskPriority_Communication, &communicationTaskHandle);
    ASSERT(success == pdPASS);
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void MasterCommunicationInterface_Run_SetResponse(MasterMessage_t response)
{
    /* Begin User Code Section: SetResponse Start */
    i2c_hal_set_tx_buffer(response.payload, response.size);
    /* End User Code Section: SetResponse Start */
    /* Begin User Code Section: SetResponse End */

    /* End User Code Section: SetResponse End */
}

__attribute__((weak))
void MasterCommunicationInterface_Call_RxTimeout(void)
{
    /* Begin User Code Section: RxTimeout Start */

    /* End User Code Section: RxTimeout Start */
    /* Begin User Code Section: RxTimeout End */

    /* End User Code Section: RxTimeout End */
}

__attribute__((weak))
void MasterCommunicationInterface_Call_OnMessageReceived(MasterMessage_t message)
{
    /* Begin User Code Section: OnMessageReceived Start */

    /* End User Code Section: OnMessageReceived Start */
    /* Begin User Code Section: OnMessageReceived End */

    /* End User Code Section: OnMessageReceived End */
}

__attribute__((weak))
void MasterCommunicationInterface_Call_LogError(void)
{
    /* Begin User Code Section: LogError Start */

    /* End User Code Section: LogError Start */
    /* Begin User Code Section: LogError End */

    /* End User Code Section: LogError End */
}

__attribute__((weak))
void MasterCommunicationInterface_Read_Configuration(MasterCommunicationInterface_Config_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: Configuration Start */

    /* End User Code Section: Configuration Start */
    *value = (MasterCommunicationInterface_Config_t) { .default_response = (MasterMessage_t) { .payload = NULL, .size = 0u }, .rx_overflow_response = (MasterMessage_t) { .payload = NULL, .size = 0u }, .rx_timeout = 0u };
    /* Begin User Code Section: Configuration End */

    /* End User Code Section: Configuration End */
}

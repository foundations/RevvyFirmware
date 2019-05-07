/*
 * worklogic.c
 *
 * Created: 1/4/2019 6:56:55 PM
 *  Author: User
 */ 

 /*
//=======================================================================================================
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    //DBG_printf(DBG_LEVEL_DEBUG, "HAL_I2C_SlaveTxCpltCallback. %d bytes trasmitted\n", hi2c->XferCount);
    //buff_i2c_to_tx_bytes = 0;
}

//=======================================================================================================
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    //DBG_printf(DBG_LEVEL_DEBUG, "HAL_I2C_SlaveRxCpltCallback, %d bytes received \n", hi2c->XferCount);
    //uint8_t ret = CommandHandler ( (ptransaction) hi2c->pBuffPtr, hi2c->XferCount);
    //buff_i2c_to_tx_bytes = MakeResponse(ret, (ptransaction) &buff_i2c_tx);

}*/

#include "rrrc_hal.h"
#include "rrrc_sensors.h"
#include "rrrc_motors.h"

//*****************************************************************************************************
//*****************************************************************************************************
//*****************************************************************************************************

static struct timer_descriptor* high_res_timer;
void high_res_timer_init(struct timer_descriptor* timer)
{
    high_res_timer = timer;

    timer_start(timer);
}

uint32_t high_res_timer_get_count(void)
{
    hri_tc_set_CTRLB_CMD_bf(high_res_timer->device.hw, TC_CTRLBSET_CMD_READSYNC_Val);
    while (hri_tc_read_CTRLB_CMD_bf(high_res_timer->device.hw) != 0);

    return hri_tccount16_get_COUNT_COUNT_bf(high_res_timer->device.hw, 0xFFFF);
}

uint32_t high_res_timer_ticks_per_ms(void)
{
    return 24;
}

/*
 * MasterCommunication.c
 *
 * Created: 2019. 05. 06. 20:16:07
 *  Author: Dániel Buga
 */ 

#include "MasterCommunication.h"

#include "driver_init.h"
#include <peripheral_clk_config.h>

extern void rrrc_i2c_s_async_tx(struct _i2c_s_async_device *const device);
extern void rrrc_i2c_s_async_byte_received(struct _i2c_s_async_device *const device, const uint8_t data);
extern void rrrc_i2c_s_async_error(struct _i2c_s_async_device *const device);
extern void rrrc_i2c_s_async_stop(struct _i2c_s_async_device *const device, const uint8_t dir);
extern void rrrc_i2c_s_async_addr_match(struct _i2c_s_async_device *const device, const uint8_t dir);

struct i2c_s_async_descriptor I2C_0;

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

void MasterCommunication_Run_OnInit(const Comm_CommandHandler_t* commandTable, size_t commandTableSize)
{
    I2C_0_init();
    Comm_Init(commandTable, commandTableSize);
}

/*
 * i2cHal.h
 *
 * Created: 07/05/2019 11:43:01
 *  Author: Dániel Buga
 */ 


#ifndef I2C_HAL_H_
#define I2C_HAL_H_

#include <hpl_i2c_s_async.h>
#include <stdio.h>

int32_t i2c_hal_init(void* hw);
void i2c_hal_receive(uint8_t* buffer, size_t bufferSize);
void i2c_hal_set_tx_buffer(const uint8_t* buffer, size_t bufferSize);

void i2c_hal_rx_started(void);
void i2c_hal_rx_complete(const uint8_t* buffer, size_t bufferSize, size_t bytesReceived);

#endif /* I2CHAL_H_ */

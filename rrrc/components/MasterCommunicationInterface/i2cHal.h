#ifndef I2C_HAL_H_
#define I2C_HAL_H_

#include <hpl_i2c_s_async.h>
#include <stdio.h>

int32_t i2c_hal_init(void* hw);
void i2c_hal_receive(uint8_t* buffer, size_t bufferSize);
void i2c_hal_set_tx_buffer(const uint8_t* buffer, size_t bufferSize);

void i2c_hal_rx_started(void);
void i2c_hal_rx_complete(const uint8_t* buffer, size_t bufferSize, size_t bytesReceived);
void i2c_hal_error_occurred(void);

#endif /* I2CHAL_H_ */

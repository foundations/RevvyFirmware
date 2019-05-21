/*
 * SensorPort_I2C.h
 *
 * Created: 2019. 05. 21. 11:18:09
 *  Author: Dániel Buga
 */ 


#ifndef SENSOR_PORT_I2C_H_
#define SENSOR_PORT_I2C_H_

#include "../SensorPortHandlerInternal.h"
#include "hal_i2c_m_async.h"

typedef struct {
    i2c_error_cb_t error;
    i2c_complete_cb_t tx_complete;
    i2c_complete_cb_t rx_complete;
} i2c_callbacks_t;

typedef enum {
    SensorPort_I2C_Success,
    SensorPort_I2C_Error
} SensorPort_I2C_Status_t;

void SensorPort_I2C_Enable(SensorPort_t* port, uint32_t baudrate, const i2c_callbacks_t* callbacks);
void SensorPort_I2C_Disable(SensorPort_t* port);
SensorPort_I2C_Status_t SensorPort_I2C_StartWrite(SensorPort_t* port, uint16_t address, uint8_t* pData, size_t dataLength);
SensorPort_I2C_Status_t SensorPort_I2C_StartRead(SensorPort_t* port, uint16_t address, uint8_t* pData, size_t dataLength);
SensorPort_I2C_Status_t SensorPort_I2C_StartRegWrite(SensorPort_t* port, uint16_t address, uint8_t reg, uint8_t* pData, size_t dataLength);
SensorPort_I2C_Status_t SensorPort_I2C_StartRegRead(SensorPort_t* port, uint16_t address, uint8_t reg, uint8_t* pData, size_t dataLength);

#endif /* SENSOR_PORT_I2C_H_ */

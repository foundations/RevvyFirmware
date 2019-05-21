/*
 * SensorPort_I2C.c
 *
 * Created: 2019. 05. 21. 11:18:40
 *  Author: Dániel Buga
 */ 

#include "SensorPort_I2C.h"

void SensorPort_I2C_Enable(SensorPort_t* port, uint32_t baudrate, const i2c_callbacks_t* callbacks)
{
    i2c_m_async_init(&port->i2c, port->i2c_hw);
    i2c_m_async_set_baudrate(&port->i2c, 24000, baudrate);
    i2c_m_async_register_callback(&port->i2c, I2C_M_ASYNC_ERROR, (FUNC_PTR)callbacks->error);
    i2c_m_async_register_callback(&port->i2c, I2C_M_ASYNC_TX_COMPLETE, (FUNC_PTR)callbacks->tx_complete);
    i2c_m_async_register_callback(&port->i2c, I2C_M_ASYNC_RX_COMPLETE, (FUNC_PTR)callbacks->rx_complete);
    i2c_m_async_enable(&port->i2c);
}

void SensorPort_I2C_Disable(SensorPort_t* port)
{
    i2c_m_async_disable(&port->i2c);
    i2c_m_async_deinit(&port->i2c);
}

SensorPort_I2C_Status_t SensorPort_I2C_StartWrite(SensorPort_t* port, uint16_t address, uint8_t* pData, size_t dataLength)
{
    i2c_m_async_set_slaveaddr(&port->i2c, address, I2C_M_SEVEN);

    struct _i2c_m_msg msg = {
        .addr   = port->i2c.slave_addr,
        .len    = dataLength,
        .flags  = I2C_M_STOP,
        .buffer = pData
    };
    
    if (_i2c_m_async_transfer(&port->i2c.device, &msg) == ERR_NONE)
    {
        return SensorPort_I2C_Success;
    }
    else
    {
        return SensorPort_I2C_Error;
    }
}

SensorPort_I2C_Status_t SensorPort_I2C_StartRead(SensorPort_t* port, uint16_t address, uint8_t* pData, size_t dataLength)
{
    i2c_m_async_set_slaveaddr(&port->i2c, address, I2C_M_SEVEN);

    struct _i2c_m_msg msg = {
        .addr   = port->i2c.slave_addr,
        .len    = dataLength,
        .flags  = I2C_M_STOP | I2C_M_RD,
        .buffer = pData
    };
    
    if (_i2c_m_async_transfer(&port->i2c.device, &msg) == ERR_NONE)
    {
        return SensorPort_I2C_Success;
    }
    else
    {
        return SensorPort_I2C_Error;
    }
}

SensorPort_I2C_Status_t SensorPort_I2C_StartRegWrite(SensorPort_t* port, uint16_t address, uint8_t reg, uint8_t* pData, size_t dataLength)
{
    uint8_t buf[dataLength + 1];
    buf[0] = reg;
    memcpy(&buf[1], pData, dataLength);

    return SensorPort_I2C_StartWrite(port, address, buf, sizeof(buf));
}

SensorPort_I2C_Status_t SensorPort_I2C_StartRegRead(SensorPort_t* port, uint16_t address, uint8_t reg, uint8_t* pData, size_t dataLength)
{
    i2c_m_async_set_slaveaddr(&port->i2c, address, I2C_M_SEVEN);

    struct _i2c_m_msg msg = {
        .addr   = port->i2c.slave_addr,
        .len    = 1,
        .flags  = 0,
        .buffer = &reg
    };

    _i2c_complete_cb_t tx_complete = port->i2c.device.cb.tx_complete;
    port->i2c.device.cb.tx_complete = NULL;

    int32_t ret = _i2c_m_async_transfer(&port->i2c.device, &msg);

    if (ret != 0) {
        /* error occurred */
        /* re-register to enable notify user callback */
        port->i2c.device.cb.tx_complete = tx_complete;
        return;
    }

    /* we polling busy flag wait for send finish here */
    while (port->i2c.device.service.msg.flags & I2C_M_BUSY) {
        ;
    }
    
    port->i2c.device.cb.tx_complete = tx_complete;
    if (port->i2c.device.service.msg.flags & I2C_M_FAIL)
    {
        return SensorPort_I2C_Error;
    }

    msg.flags  = I2C_M_STOP | I2C_M_RD;
    msg.buffer = pData;
    msg.len = dataLength;
    
    if (_i2c_m_async_transfer(&port->i2c.device, &msg) == ERR_NONE)
    {
        return SensorPort_I2C_Success;
    }
    else
    {
        return SensorPort_I2C_Error;
    }
}

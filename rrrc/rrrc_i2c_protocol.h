/*
 * i2c_protocol.h
 *
 * Created: 1/4/2019 7:04:22 PM
 *  Author: User
 */ 


#ifndef RRRC_I2C_PROTOCOL_H_
#define RRRC_I2C_PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TRANSACTION_SIZE 127 /* TODO: do not define twice */

typedef struct
{
    uint8_t command;
    uint8_t payloadLength;
    uint8_t crc;
} commandHeader_t;

typedef union
{
    uint8_t buffer[MAX_TRANSACTION_SIZE];
    struct 
    {
        commandHeader_t header;
        uint8_t payload[MAX_TRANSACTION_SIZE - sizeof(commandHeader_t)];
    };
} commandBuffer_t;

typedef struct 
{
    uint8_t status;
    uint8_t payloadLength;
    uint8_t crc;
} responseHeader_t;

typedef union
{
    uint8_t buffer[MAX_TRANSACTION_SIZE];
    struct 
    {
        responseHeader_t header;
        uint8_t payload[MAX_TRANSACTION_SIZE - sizeof(responseHeader_t)];
    };
} responseBuffer_t;

typedef struct _defice_info_t{
    uint8_t dev_type;
    uint8_t fw_version;
    uint8_t sensor_amount;
    uint8_t motor_amount;
} dev_info, *pdev_info;

//=============================================================================================
//STATUS & PING
typedef enum
{
    RRRC_I2C_CMD_ECHO                = 0x07,        //Use for debug
    RRRC_I2C_CMD_DUMMY               = 0x09,
    RRRC_I2C_CMD_RESET               = 0x0A,

    //about device
    RRRC_I2C_CMD_SENSOR_GET_PORT_AMOUNT = 0x10,
    RRRC_I2C_CMD_MOTOR_GET_PORT_AMOUNT  = 0x11,
    RRRC_I2C_CMD_SENSOR_GET_TYPES       = 0x12,
    RRRC_I2C_CMD_MOTOR_GET_TYPES        = 0x13,
    
    // long command control
    RRRC_I2C_CMD_LONG_COMMAND_START      = 0x20u,
    RRRC_I2C_CMD_LONG_COMMAND_GET_RESULT = 0x21u,
    RRRC_I2C_CMD_LONG_COMMAND_CANCEL     = 0x22u,

    //need testing...
    RRRC_I2C_CMD_SENSOR_GET_TYPES_AMOUNT = 0x14,
    RRRC_I2C_CMD_SENSOR_GET_TYPE_INFO    = 0x15,
    RRRC_I2C_CMD_MOTOR_GET_TYPES_AMOUNT  = 0x16,
    RRRC_I2C_CMD_MOTOR_GET_TYPE_INFO     = 0x17,

    //for sensor ports
    RRRC_I2C_CMD_SENSOR_SET_TYPE        = 0x30,
    RRRC_I2C_CMD_SENSOR_GET_TYPE        = 0x31,
    RRRC_I2C_CMD_SENSOR_GET_VALUE       = 0x32,
    RRRC_I2C_CMD_SENSOR_WRITE_I2CHUB    = 0x34,
    RRRC_I2C_CMD_SENSOR_READ_I2CHUB     = 0x35,
    //for motor ports
    RRRC_I2C_CMD_MOTOR_SET_TYPE         = 0x60,
    RRRC_I2C_CMD_MOTOR_GET_TYPE         = 0x61,
    RRRC_I2C_CMD_MOTOR_SET_CONTROL      = 0x63,
    RRRC_I2C_CMD_MOTOR_GET_STATE        = 0x64,
    RRRC_I2C_CMD_MOTOR_SET_CONFIG       = 0x65,
    RRRC_I2C_CMD_MOTOR_GET_POSITION     = 0x66,
    //for indication
    RRRC_I2C_CMD_INDICATION_GET_STATUS_LEDS_AMOUNT = 0x90,
    RRRC_I2C_CMD_INDICATION_GET_RING_LEDS_AMOUNT   = 0x91,
    RRRC_I2C_CMD_INDICATION_SET_STATUS_LED         = 0x92,
    RRRC_I2C_CMD_INDICATION_SET_RING_SCENARIO      = 0x93,
    RRRC_I2C_CMD_INDICATION_SET_RING_USER_FRAME    = 0x94,

    //system monitor
    RRRC_I2C_CMD_SYSMON_GET_STAT        = 0x95,

    RRRC_I2C_COMMAND_COUNT
} RRRC_I2C_Command_t;

typedef enum {
    RRRC_I2C_LONG_CMD_ECHO = 0x00,

    RRRC_I2C_LONG_COMMAND_COUNT
} RRRC_I2C_LongCommand_t;

typedef enum
{
    RRRC_I2C_STATUS_UNKNOWN                 = 0xFFu,
    RRRC_I2C_STATUS_OK                      = 0x00u,
    RRRC_I2C_STATUS_ERROR                   = 0x01u,
    RRRC_I2C_STATUS_BUSY                    = 0x02u,
    RRRC_I2C_STATUS_READY                   = 0x03u,
    RRRC_I2C_STATUS_PENDING                 = 0x04u, // new
    //RRRC_I2C_STATUS_ERROR_OTHER             = 0x80u,
    //RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND   = 0x81u,
    //RRRC_I2C_STATUS_ERROR_CHECKSUM          = 0x82u,
    //RRRC_I2C_STATUS_ERROR_PAYLOAD_LENGTH    = 0x83u,
    //RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT  = 0x84u,
    RRRC_I2C_STATUS_ERROR_OTHER             = RRRC_I2C_STATUS_ERROR,
    RRRC_I2C_STATUS_ERROR_UNKNOWN_COMMAND   = RRRC_I2C_STATUS_ERROR,
    RRRC_I2C_STATUS_ERROR_CHECKSUM          = RRRC_I2C_STATUS_ERROR,
    RRRC_I2C_STATUS_ERROR_PAYLOAD_LENGTH    = RRRC_I2C_STATUS_ERROR,
    RRRC_I2C_STATUS_ERROR_INVALID_ARGUMENT  = RRRC_I2C_STATUS_ERROR
} RRRC_I2C_Status_t;

// typedef union _port_config_t
// {
//     struct sensor
//     {
//         uint8_t port_num;
//         uint8_t dev_type;
//     }
//     struct motor
//     {
//         uint8_t port_num;
//         uint8_t dev_type;
//     }
//     uint16_t config;
// }sensport_config_t, *psensport_config_t, motorport_config_t, *pmotorport_config_t;

static uint8_t calcCRC(uint8_t* buff, uint8_t size)
{
    if ( (buff==NULL) )
        return 0;

    return 0;

//     uint8_t crc = 0;
//     for (uint8_t idx=0;idx<size;idx++)
//     {
//         crc += buff[idx];
//     }
// 
//     crc ^= 0x55;
// 
//     return crc;
}

//=============================================================================================
static uint8_t checkCRC(uint8_t* buff)
{
    if (buff==NULL)
        return 0;

    return 1;
//     if ( calcCRC(buff->data, buff->data_length) ==  buff->data_crc)
//     return 1; //OK
//     else
//     return 0; //BAD
}




#ifdef __cplusplus

#include <iostream>
#include <iomanip>

static  std::ostream& operator<<(std::ostream& os, ptransaction_t data)
{
    os << std::endl << "  command 0x"  << std::setfill('0') << std::setw(2) << std::hex <<(size_t)data.command;
    os << std::endl << "  data size "  << std::dec <<(size_t)data.data_length << "bytes";
    os << std::endl << "  data crc 0x" << std::setfill('0') << std::setw(2) << std::hex <<(size_t)data.data_crc;
    if (data.data_length && (data.data_length<(MAX_TRANSACTION_SIZE - MIN_TRANSACTION_SIZE)) && checkCRC(&data))
    {
        for (size_t high_idx=0; high_idx<data.data_length; high_idx+=8)
        {
            os << std::endl << "data: ";
            for (size_t low_idx=0; low_idx<8 && ((low_idx+high_idx)<data.data_length); low_idx++)
            os <<" 0x" << std::setfill('0') << std::setw(2) << std::hex << (size_t)data.data[low_idx+high_idx];
        }
    }
    return os;
}
#endif

int32_t RRRC_Communication_Init();
int32_t RRRC_Communication_DeInit();
void CommunicationTask_NotifyRxCompleteFromISR(void);

#ifdef __cplusplus
}
#endif

#endif /* RRRC_I2C_PROTOCOL_H_ */
/*
 * rrrc_i2c_protocol.c
 *
 * Created: 2/13/2019 7:54:51 PM
 *  Author: User
 */ 
#include "rrrc_hal.h"
#include "rrrc_i2c_protocol.h"
#include "rrrc_sensors.h"
#include "rrrc_motors.h"
#include "rrrc_indication.h"
#include "converter.h"
#include <utils_assert.h>

static uint32_t request_port = 0;

//*********************************************************************************************
uint8_t CommandHandler (ptransaction_t buff, uint8_t size)
 {
	 uint8_t ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;

	 if ( (buff == NULL) || (size < MIN_TRANSACTION_SIZE) )
		return ret_cmd;

	 if( !checkCRC(buff) )
		return ret_cmd;
	 
	switch (buff->command)
	{
		case RRRC_I2C_CMD_SENSOR_GET_PORT_AMOUNT:
		case RRRC_I2C_CMD_MOTOR_GET_PORT_AMOUNT:
		case RRRC_I2C_CMD_INDICATION_GET_RING_LEDS_AMOUNT:
		case RRRC_I2C_CMD_INDICATION_GET_STATUS_LEDS_AMOUNT:
		case RRRC_I2C_CMD_SENSOR_GET_TYPES:
		case RRRC_I2C_CMD_MOTOR_GET_TYPES:
		case RRRC_I2C_CMD_SENSOR_GET_TYPE:
		case RRRC_I2C_CMD_SENSOR_GET_VALUE:
		case RRRC_I2C_CMD_MOTOR_GET_TYPE:
		case RRRC_I2C_CMD_MOTOR_GET_STATE:
		case RRRC_I2C_CMD_MOTOR_GET_COUNT:
			if (buff->data_length)
				request_port = buff->data[0];
		case RRRC_I2C_CMD_DUMMY:
			ret_cmd = buff->command;
			break;
		{//*****
		//fix VS bug of show 
		}//*****
//sensors setter
		case RRRC_I2C_CMD_SENSOR_SET_TYPE:
		{
			if (buff->data_length != 2)
				ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			else
			{
				uint8_t port = buff->data[0];
				uint8_t type = buff->data[1];
				uint32_t status = SensorPortSetType(port, type);
				if (status == 0)
					ret_cmd = RRRC_I2C_CMD_STATUS_OK;
				else
					ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			}
			break;
		}
//motors setter
		case RRRC_I2C_CMD_MOTOR_SET_TYPE:
		{
			if (buff->data_length != 2)
				ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			else
			{
				uint8_t port = buff->data[0];
				uint8_t type = buff->data[1];
				uint32_t status = MotorPortSetType(port, type);
				if (status == 0)
					ret_cmd = RRRC_I2C_CMD_STATUS_OK;
				else
					ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			}
			break;
		}
		case RRRC_I2C_CMD_MOTOR_SET_CONTROL:
		{
			if (buff->data_length != 5)
				ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			else
			{
				uint8_t port = buff->data[0];
				int32_t state = get_int32(&buff->data[1]);
				uint32_t status = MotorPortSetControl(port, state);
				if (status == ERR_NONE)
					ret_cmd = RRRC_I2C_CMD_STATUS_OK;
				else
					ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			}
			break;
		}
		case RRRC_I2C_CMD_MOTOR_SET_CONFIG:
		{
    		uint8_t port = buff->data[0];
    		uint32_t status = MotorPortSetConfig(port, &buff->data[1], buff->data_length - 1);
    		if (status == ERR_NONE)
    		{
                ret_cmd = RRRC_I2C_CMD_STATUS_OK;
            }
    		else
    		{
                ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
            }
			break;
		}
		case RRRC_I2C_CMD_INDICATION_SET_RING_SCENARIO:
		{
			if (buff->data_length != 1)
				ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			else
			{
				uint32_t scenario_idx = buff->data[0];
				uint32_t status = IndicationSetRingType(scenario_idx);
				if (status == ERR_NONE)
					ret_cmd = RRRC_I2C_CMD_STATUS_OK;
				else
					ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			}
			break;
		}
		case RRRC_I2C_CMD_INDICATION_SET_RING_USER_FRAME:
		{
			if ( buff->data_length != (1+sizeof(led_ring_frame_t)))
				ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			else
			{
				uint32_t frame_idx = buff->data[0];
				led_ring_frame_t* led_frame = &buff->data[1];
				uint32_t status = IndicationUpdateUserFrame(frame_idx, led_frame);
				if (status == ERR_NONE)
					ret_cmd = RRRC_I2C_CMD_STATUS_OK;
				else
					ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			}
			break;
		}
		case RRRC_I2C_CMD_INDICATION_SET_STATUS_LEDS:
		{
			if (buff->data_length != (1+sizeof(led_val_t)))
				ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			else
			{
				uint32_t led_idx = buff->data[0];
				p_led_val_t led_rgb = &buff->data[1];
				uint32_t status = IndicationSetStatusLed(led_idx, led_rgb);
				if (status == ERR_NONE)
					ret_cmd = RRRC_I2C_CMD_STATUS_OK;
				else
					ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
			}
			break;
		}
		
// int32_t IndicationUpdateUserFrame(uint32_t frame_id, led_ring_frame_t* frame);
// int32_t IndicationSetStatusLed(uint32_t stled_idx, p_led_val_t led_val);
// int32_t IndicationSetRingType(enum INDICATON_RING_TYPE type);
		default:
		{
			break;
		};
	}

	return ret_cmd;
}

//*********************************************************************************************
uint8_t MakeResponse(enum RRRC_I2C_CMD cmd, ptransaction_t respose)
{
	uint8_t size = 0;
	uint32_t max_buff_size = MAX_TRANSACTION_SIZE-MIN_TRANSACTION_SIZE;

	if (respose==NULL)
		return size;

	respose->command = cmd;
	respose->data_length = 0;
	
	switch (cmd)
	{
		case RRRC_I2C_CMD_DUMMY:
		case RRRC_I2C_CMD_STATUS_OK:
		case RRRC_I2C_CMD_STATUS_ERROR:
		case RRRC_I2C_CMD_STATUS_UNKNOWN:
		case RRRC_I2C_CMD_STATUS_BUSY:
		case RRRC_I2C_CMD_STATUS_READY:
			break;
		{//*****
		//fix VS bug of show 
		}//*****
		case RRRC_I2C_CMD_SENSOR_GET_PORT_AMOUNT:
		{
			respose->data[0] = SensorPortGetPortsAmount();
			respose->data_length = 1;
			break;
		};
		case RRRC_I2C_CMD_MOTOR_GET_PORT_AMOUNT:
		{
			respose->data[0] = MotorPortGetPortsAmount();
			respose->data_length = 1;
			break;
		}
		case RRRC_I2C_CMD_SENSOR_GET_TYPES:
		{
			respose->data_length = SensorPortGetTypes(respose->data, max_buff_size);
			break;
		};
		case RRRC_I2C_CMD_MOTOR_GET_TYPES:
		{	
			respose->data_length = MotorPortGetTypes(respose->data, max_buff_size);
			break;
		};
		//sensors
		case RRRC_I2C_CMD_SENSOR_GET_TYPE:
		{
			respose->data[0] = SensorPortGetType(request_port);
			respose->data_length = 1;
			break;
		};
		case RRRC_I2C_CMD_SENSOR_GET_VALUE:
		{
			respose->data_length = SensorPortGetValues(request_port, respose->data) * 4;
			break;
		};
		//motors
		case RRRC_I2C_CMD_MOTOR_GET_TYPE:
		{
			respose->data[0] = MotorPortGetType(request_port);
			respose->data_length = 1;
			break;
		};
		case RRRC_I2C_CMD_MOTOR_GET_STATE:
		{
			//respose->data[0] = GetMotorState(port);
			respose->data[0] = MotorPortGetState(request_port);
			respose->data_length = 1;
			break;
		};
		case RRRC_I2C_CMD_MOTOR_GET_COUNT:
		{
			respose->data_length = MotorPortGetCount(request_port, respose->data) * 4;
			break;
		};
		case RRRC_I2C_CMD_INDICATION_GET_RING_LEDS_AMOUNT:
		{
			respose->data[0] = IndicationGetRingLedsAmount();
			respose->data_length = 1;
			break;
		}
		case RRRC_I2C_CMD_INDICATION_GET_STATUS_LEDS_AMOUNT:
		{
			respose->data[0] = IndicationGetStatusLedsAmount();
			respose->data_length = 1;
			break;
		}
		case RRRC_I2C_CMD_SYSMON_GET_STAT:
		{
			respose->data_length = SysMonGetValues(respose->data);
			break;
		}
		case RRRC_I2C_CMD_SENSOR_SET_TYPE:
		case RRRC_I2C_CMD_MOTOR_SET_TYPE:
		case RRRC_I2C_CMD_MOTOR_SET_CONTROL:
		case RRRC_I2C_CMD_MOTOR_SET_CONFIG:
		default:
		{
			respose->command = RRRC_I2C_CMD_STATUS_ERROR;
			break;
		}
	}
	respose->data_crc = calcCRC(respose->data, respose->data_length);
	size = MIN_TRANSACTION_SIZE+respose->data_length;
	return size;
}

TaskHandle_t    xCommunicationTask;
extern trans_buffer_t rx_buffer;
extern trans_buffer_t tx_buffer;
void RRRC_Comunication_xTask(void* user_data)
{
	BaseType_t xResult;
	uint32_t ulNotifiedValue;


	for(;;)
	{
		xResult = xTaskNotifyWait(pdFALSE, UINT32_MAX, &ulNotifiedValue, portMAX_DELAY);
		
		if (xResult == pdPASS)
		{
			uint8_t cmd = CommandHandler(rx_buffer.buff, rx_buffer.size);
			tx_buffer.size = MakeResponse(cmd, tx_buffer.buff);
			rx_buffer.size = 0;
		}
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
/*
 * command_handler.c
 *
 * Created: 2019. 04. 11. 10:20:36
 *  Author: Dániel Buga
 */ 

#include "command_handler.h"

#include "../rrrc_hal.h"
#include "../rrrc_i2c_protocol.h"

#include "../rrrc_sensors.h"
#include "../rrrc_motors.h"
#include "../rrrc_indication.h"
#include "../converter.h"

static uint32_t request_port = 0;

static uint8_t echoData[MAX_TRANSACTION_SIZE - MIN_TRANSACTION_SIZE];
static uint8_t echoLength;

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
		case RRRC_I2C_CMD_MOTOR_GET_POSITION:
			if (buff->data_length)
				request_port = buff->data[0];
		case RRRC_I2C_CMD_DUMMY:
		case RRRC_I2C_CMD_ECHO_RD:
			ret_cmd = buff->command;
			break;
		{//*****
		//fix VS bug of show 
		}//*****

        case RRRC_I2C_CMD_ECHO_WR:
        {
            if (buff->data_length > MAX_TRANSACTION_SIZE - MIN_TRANSACTION_SIZE)
            {
                ret_cmd = RRRC_I2C_CMD_STATUS_ERROR;
            }
            else
            {
                echoLength = buff->data_length;
                memcpy(echoData, buff->data, echoLength);
                ret_cmd = RRRC_I2C_CMD_STATUS_OK;
            }
            break;
        }

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
uint8_t MakeResponse(enum RRRC_I2C_CMD cmd, ptransaction_t response)
{
	uint8_t size = 0;
	uint32_t max_buff_size = MAX_TRANSACTION_SIZE-MIN_TRANSACTION_SIZE;

	if (response==NULL)
		return size;

	response->command = cmd;
	response->data_length = 0;
	
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
        case RRRC_I2C_CMD_ECHO_RD:
            response->data_length = echoLength;
            memcpy(response->data, echoData, echoLength);
            break;

		case RRRC_I2C_CMD_SENSOR_GET_PORT_AMOUNT:
		{
			response->data[0] = SensorPortGetPortsAmount();
			response->data_length = 1;
			break;
		};
		case RRRC_I2C_CMD_MOTOR_GET_PORT_AMOUNT:
		{
			response->data[0] = MotorPortGetPortsAmount();
			response->data_length = 1;
			break;
		}
		case RRRC_I2C_CMD_SENSOR_GET_TYPES:
		{
			response->data_length = SensorPortGetTypes(response->data, max_buff_size);
			break;
		};
		case RRRC_I2C_CMD_MOTOR_GET_TYPES:
		{	
			response->data_length = MotorPortGetTypes(response->data, max_buff_size);
			break;
		};
		//sensors
		case RRRC_I2C_CMD_SENSOR_GET_TYPE:
		{
			response->data[0] = SensorPortGetType(request_port);
			response->data_length = 1;
			break;
		};
		case RRRC_I2C_CMD_SENSOR_GET_VALUE:
		{
			response->data_length = SensorPortGetValues(request_port, response->data) * 4;
			break;
		};
		//motors
		case RRRC_I2C_CMD_MOTOR_GET_TYPE:
		{
			response->data[0] = MotorPortGetType(request_port);
			response->data_length = 1;
			break;
		};
		case RRRC_I2C_CMD_MOTOR_GET_STATE:
		{
			//response->data[0] = GetMotorState(port);
			response->data[0] = MotorPortGetState(request_port);
			response->data_length = 1;
			break;
		};
		case RRRC_I2C_CMD_MOTOR_GET_POSITION:
		{
			response->data_length = MotorPortGetPosition(request_port, response->data);
			break;
		};
		case RRRC_I2C_CMD_INDICATION_GET_RING_LEDS_AMOUNT:
		{
			response->data[0] = IndicationGetRingLedsAmount();
			response->data_length = 1;
			break;
		}
		case RRRC_I2C_CMD_INDICATION_GET_STATUS_LEDS_AMOUNT:
		{
			response->data[0] = IndicationGetStatusLedsAmount();
			response->data_length = 1;
			break;
		}
		case RRRC_I2C_CMD_SYSMON_GET_STAT:
		{
			response->data_length = SysMonGetValues(response->data);
			break;
		}
		case RRRC_I2C_CMD_SENSOR_SET_TYPE:
		case RRRC_I2C_CMD_MOTOR_SET_TYPE:
		case RRRC_I2C_CMD_MOTOR_SET_CONTROL:
		case RRRC_I2C_CMD_MOTOR_SET_CONFIG:
		default:
		{
			response->command = RRRC_I2C_CMD_STATUS_ERROR;
			break;
		}
	}
	response->data_crc = calcCRC(response->data, response->data_length);
	size = MIN_TRANSACTION_SIZE+response->data_length;
	return size;
}

static CommandHandler_t commandHandlers[RRRC_I2C_COMMAND_COUNT] = 
{
    
};

size_t CommandHandler_Handle(const uint8_t* command, size_t commandSize, uint8_t* pData, size_t bufSize)
{    
    uint8_t response = CommandHandler(command, commandSize);
    return MakeResponse(response, pData);
}

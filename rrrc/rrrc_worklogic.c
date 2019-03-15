/*
 * rrrc_worklogic.c
 *
 * Created: 2/14/2019 11:38:22 AM
 *  Author: User
 */ 

#include "rrrc_hal.h"
#include "rrrc_worklogic.h"
#include "rrrc_i2c_protocol.h"
#include "rrrc_sensors.h"
#include "rrrc_motors.h"

#include "driver_examples.h"


//*********************************************************************************************
void RRRC_ProcessLogic(void)
{

	for (uint32_t idx=0; idx<SENSOR_PORT_AMOUNT; idx++ )
		SensorPortInit(idx);

	for (uint32_t idx=0; idx<MOTOR_PORT_AMOUNT; idx++ )
		MotorPortInit(idx);

	//IndicationInit();

	while (1)
	{
		 delay_ms(200);
	}

	//This code will not never executed
	//This code for DeInit example
	for (uint32_t idx=0; idx<SENSOR_PORT_AMOUNT; idx++ )
		SensorPortDeInit(idx);
	for (uint32_t idx=0; idx<MOTOR_PORT_AMOUNT; idx++ )
		MotorPortInit(idx);
}
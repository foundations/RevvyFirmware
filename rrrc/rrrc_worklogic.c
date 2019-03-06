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




void RRRC_ProcessLogic(void)
{

	for (uint32_t idx=0; idx<SENSOR_PORT_AMOUNT; idx++ )
		SensorPortInit(idx);
	SensorPortSetType(0, SENSOR_ANALOG_SWITCH);
	SensorPortSetType(1, SENSOR_SWITCH);
	SensorPortSetType(2, SENSOR_HC_SR05);
	SensorPortSetType(3, SENSOR_NOT_SET);

	for (uint32_t idx=0; idx<MOTOR_PORT_AMOUNT; idx++ )
		MotorPortInit(idx);
	MotorPortSetType(0, MOTOR_DC);
	MotorPortSetType(1, MOTOR_DC);
	MotorPortSetType(2, MOTOR_DC);
	MotorPortSetType(3, MOTOR_DC);
	MotorPortSetType(4, MOTOR_DC);
	MotorPortSetType(5, MOTOR_NOT_SET);


	MotorPortSetState(0, 10);
	MotorPortSetState(0, -10);

	MotorPortSetState(0, 50);
	MotorPortSetState(0, -50);

	MotorPortSetState(0, 127);
	MotorPortSetState(0, -127);


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
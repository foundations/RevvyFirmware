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


//FOR TEST ONLY
//  	SensorPortSetType(0, SENSOR_HC_SR05);
//  	SensorPortSetType(1, SENSOR_NOT_SET);
//  	SensorPortSetType(2, SENSOR_NOT_SET);
//  	SensorPortSetType(3, SENSOR_HC_SR05);

	


	//pwm_1khz_example();

// 	TIMER_0_example();
// 	TIMER_1_example();
// 	TIMER_2_example();
// 	TIMER_3_example();
// 	TIMER_4_example();
// 	TIMER_5_example();
// 	TIMER_6_example();
// 	TIMER_7_example();

	for (int idx=0; idx<MOTOR_PORT_AMOUNT; idx++)
	{
		MotorPortSetType(idx, MOTOR_DC);
		MotorPortSetState(idx, 20+10*idx);
	}
// 		MotorPortSetType(0, MOTOR_DC);
// 		MotorPortSetType(1, MOTOR_DC);
// 		MotorPortSetType(2, MOTOR_DC);
// 		MotorPortSetType(3, MOTOR_DC);
// 		MotorPortSetType(4, MOTOR_DC);
// 		MotorPortSetType(5, MOTOR_DC);
// 		MotorPortSetState(0, 20);
// 		MotorPortSetState(1, -20);
// 		MotorPortSetState(2, 20);
// 		MotorPortSetState(3, -20);
// 		MotorPortSetState(4, 80);
// 		MotorPortSetState(5, -20);

// 
// 	MotorPortSetState(0, 0);
// 	MotorPortSetState(0, 10);
// 	MotorPortSetState(0, 0);
// 	MotorPortSetState(0, -10);
// 
// 	MotorPortSetState(0, 0);
// 	MotorPortSetState(0, 50);
// 	MotorPortSetState(0, 0);
// 	MotorPortSetState(0, -50);
// 
// 	MotorPortSetState(0, 0);
// 	MotorPortSetState(0, 100);
// 	MotorPortSetState(0, 0);
// 	MotorPortSetState(0, -100);
// 
// 	MotorPortSetState(0, 0);
// 
// 	MotorPortSetState(0, 5);
// 	MotorPortSetState(0, 20);
// 	MotorPortSetState(0, 50);
// 	MotorPortSetState(0, 100);
// 	MotorPortSetState(0, 50);
// 	MotorPortSetState(0, 20);
// 	MotorPortSetState(0, 5);
	
	//MotorPortSetState(0, 0);




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
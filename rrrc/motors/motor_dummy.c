/*
 * motor_dummy.c
 *
 * Created: 2/27/2019 9:10:42 PM
 *  Author: User
 */ 

#include "motor_dummy.h"

//*********************************************************************************************
static int32_t MOTOR_DUMMY_Init(void* hw_port)
{
    return ERR_NONE;
}

static void MOTOR_DUMMY_Update(void* hw_port)
{

}

motor_lib_entry_t motor_dummy =
{
    .type_id = MOTOR_NOT_SET,
    .name = "MOTOR_NO_SET",
    .MotorInit = MOTOR_DUMMY_Init,
    .update = &MOTOR_DUMMY_Update,
    .MotorDeInit = NULL,
};

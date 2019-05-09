/*
 * MotorPortHandler.c
 *
 * Created: 09/05/2019 14:03:32
 *  Author: Dániel Buga
 */ 

#include "MotorPortHandlerInternal.h"
#include "utils.h"
#include <string.h>

#include "MotorPortLibraries/Dummy/Dummy.h"
#include "MotorPortLibraries/SpeedControlled/SpeedControlled.h"
#include "MotorPortLibraries/PositionControlled/PositionControlled.h"
#include "MotorPortLibraries/OpenLoop/OpenLoop.h"

static const MotorLibrary_t* libraries[] = 
{
    &motor_library_dummy,
    &motor_library_open_loop,
    &motor_library_speed_controlled,
    &motor_library_position_controlled,
};

Comm_Status_t MotorPortHandler_GetMotorPortAmount_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    response[0] = 6;
    *responseCount = 1u;

    return Comm_Status_Ok;
}

Comm_Status_t MotorPortHandler_GetMotorPortTypes_Start(const uint8_t* commandPayload, uint8_t commandSize, uint8_t* response, uint8_t responseBufferSize, uint8_t* responseCount)
{
    uint8_t len = 0u;
    for (uint32_t i = 0u; i < ARRAY_SIZE(libraries); i++)
    {
        const MotorLibrary_t* lib = libraries[i];
        size_t name_length = strlen(lib->name);
        if (len + name_length + 2u > responseBufferSize)
        {
            *responseCount = 0u;
            return Comm_Status_Error_InternalError;
        }
        response[len] = i;
        response[len + 1] = name_length;
        memcpy(&response[len + 2], lib->name, name_length);
        len = len + 2 + name_length;
    }
    *responseCount = len;

    return Comm_Status_Ok;
}

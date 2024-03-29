/*
 * MotorPortLibrary.h
 *
 * Created: 09/05/2019 16:07:46
 *  Author: D�niel Buga
 */ 


#ifndef MOTOR_PORT_LIBRARY_H_
#define MOTOR_PORT_LIBRARY_H_

struct _MotorLibrary_t;

#include "../MotorPortHandlerInternal.h"

typedef enum {
    MotorLibraryStatus_Ok,
    MotorLibraryStatus_InputError
} MotorLibraryStatus_t; 

typedef struct _MotorLibrary_t
{
    const char* name;
    MotorLibraryStatus_t (*Init)(struct _MotorPort_t* motorPort);
    MotorLibraryStatus_t (*DeInit)(struct _MotorPort_t* motorPort);
    MotorLibraryStatus_t (*Update)(struct _MotorPort_t* motorPort);

    MotorLibraryStatus_t (*Gpio0Callback)(struct _MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state);
    MotorLibraryStatus_t (*Gpio1Callback)(struct _MotorPort_t* motorPort, uint32_t pin0state, uint32_t pin1state);

    MotorLibraryStatus_t (*SetConfig)(struct _MotorPort_t* motorPort, const uint8_t* data, uint8_t size);
    MotorLibraryStatus_t (*UpdateConfiguration)(struct _MotorPort_t* motorPort);

    MotorLibraryStatus_t (*GetStatus)(struct _MotorPort_t* motorPort, uint8_t* data, uint8_t* dataSize);

    MotorLibraryStatus_t (*SetControlReference)(struct _MotorPort_t* motorPort, const uint8_t* data, uint8_t size);
} MotorLibrary_t;

#endif /* MOTOR_PORT_LIBRARY_H_ */

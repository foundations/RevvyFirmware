/*
 * MotorPortLibrary.h
 *
 * Created: 09/05/2019 16:07:46
 *  Author: Dániel Buga
 */ 


#ifndef MOTOR_PORT_LIBRARY_H_
#define MOTOR_PORT_LIBRARY_H_

struct _MotorLibrary_t;

#include "../MotorPortHandlerInternal.h"

typedef enum {
    MotorLibraryStatus_Ok
} MotorLibraryStatus_t; 

typedef struct _MotorLibrary_t
{
    const char* name;
    MotorLibraryStatus_t (*Init)(struct _MotorPort_t* motorPort);
    MotorLibraryStatus_t (*DeInit)(struct _MotorPort_t* motorPort);
    MotorLibraryStatus_t (*Update)(struct _MotorPort_t* motorPort);

    MotorLibraryStatus_t (*Gpio0Callback)(struct _MotorPort_t* motorPort, uint32_t state);
    MotorLibraryStatus_t (*Gpio1Callback)(struct _MotorPort_t* motorPort, uint32_t state);

    MotorLibraryStatus_t (*SetConfig)(struct _MotorPort_t* motorPort, const uint8_t* data, uint8_t size);
    MotorLibraryStatus_t (*UpdateConfiguration)(struct _MotorPort_t* motorPort);
    MotorLibraryStatus_t (*GetConfig)(struct _MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size);

    MotorLibraryStatus_t (*GetPosition)(struct _MotorPort_t* motorPort, int32_t* data);

    MotorLibraryStatus_t (*SetControlReference)(struct _MotorPort_t* motorPort, uint8_t* data, uint8_t size);
    MotorLibraryStatus_t (*GetControlReference)(struct _MotorPort_t* motorPort, uint8_t* data, uint8_t* size, uint8_t max_size);
} MotorLibrary_t;

#endif /* MOTOR_PORT_LIBRARY_H_ */

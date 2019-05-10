/*
 * SensorPortLibrary.h
 *
 * Created: 09/05/2019 16:07:46
 *  Author: Dániel Buga
 */ 


#ifndef SENSOR_PORT_LIBRARY_H_
#define SENSOR_PORT_LIBRARY_H_

struct _SensorLibrary_t;

#include "../SensorPortHandlerInternal.h"

typedef enum {
    SensorLibraryStatus_Ok,
    SensorLibraryStatus_Pending,
} SensorLibraryStatus_t; 

typedef struct _SensorLibrary_t
{
    const char* name;
    SensorLibraryStatus_t (*Init)(struct _SensorPort_t* sensorPort);
    SensorLibraryStatus_t (*DeInit)(struct _SensorPort_t* sensorPort);
    SensorLibraryStatus_t (*Update)(struct _SensorPort_t* sensorPort);

    SensorLibraryStatus_t (*SetConfig)(struct _SensorPort_t* sensorPort, const uint8_t* data, uint8_t size);
    SensorLibraryStatus_t (*UpdateConfiguration)(struct _SensorPort_t* sensorPort);

    SensorLibraryStatus_t (*PrepareGetValue)(struct _SensorPort_t* sensorPort, const uint8_t* parameter, uint8_t parameterLength);
    SensorLibraryStatus_t (*GetValue)(struct _SensorPort_t* sensorPort, uint8_t* value, uint8_t maxSize, uint8_t* valueSize);
    
    SensorLibraryStatus_t (*InterruptHandler)(struct _SensorPort_t* sensorPort, bool state);
    SensorLibraryStatus_t (*UpdateAnalogData)(struct _SensorPort_t* sensorPort, uint8_t rawValue);
} SensorLibrary_t;

#endif /* SENSOR_PORT_LIBRARY_H_ */

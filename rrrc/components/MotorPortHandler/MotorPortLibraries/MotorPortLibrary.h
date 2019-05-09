/*
 * MotorPortLibrary.h
 *
 * Created: 09/05/2019 16:07:46
 *  Author: Dániel Buga
 */ 


#ifndef MOTOR_PORT_LIBRARY_H_
#define MOTOR_PORT_LIBRARY_H_


/*
typedef struct _motor_lib_entry_t
{
    motor_type_t type_id;
    char* name;

    int32_t (*MotorInit)(void* hw_port);
    int32_t (*MotorDeInit)(void* hw_port);
    void (*update)(void* hw_port);

    int32_t (*motor_set_config)(void* hw_port, const uint8_t* data, uint32_t size);
    uint32_t (*motor_get_config)(void* hw_port, uint8_t* data, uint32_t max_size);

    uint32_t (*motor_get_position)(void* hw_port, uint8_t* data, uint32_t max_size);

    uint32_t (*motor_set_control)(void* hw_port, int32_t value);
    uint32_t (*motor_get_control)(void* hw_port, uint8_t* data, uint32_t max_size);

    void (*gpio0_callback)(void* hw_port, uint32_t state);
    void (*gpio1_callback)(void* hw_port, uint32_t state);
}motor_lib_entry_t, *p_motor_lib_entry_t;

typedef enum {
    MotorLibraryStatus_Ok
} MotorLibraryStatus_t; */

typedef struct _MotorLibrary_t
{
    const char* name;
    // MotorLibraryStatus_t (*Init)(void* motorPort);
} MotorLibrary_t;

#endif /* MOTOR_PORT_LIBRARY_H_ */

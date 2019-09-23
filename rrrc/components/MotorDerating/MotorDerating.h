#ifndef COMPONENT_MOTOR_DERATING_H_
#define COMPONENT_MOTOR_DERATING_H_

#ifndef COMPONENT_TYPES_MOTOR_DERATING_H_
#define COMPONENT_TYPES_MOTOR_DERATING_H_

#include <float.h>
#include <stdint.h>

typedef float Temperature_t;

typedef struct {
    Temperature_t MaxSafeTemperature;
    Temperature_t MaxAllowedTemperature;
} MotorDeratingParameters_t;

#endif /* COMPONENT_TYPES_MOTOR_DERATING_H_ */

void MotorDerating_Run_OnUpdate(void);
void MotorDerating_Write_DeratedControlValue(uint32_t index, const int8_t value);
int8_t MotorDerating_Read_ControlValue(uint32_t index);
Temperature_t MotorDerating_Read_MotorTemperature(uint32_t index);
void MotorDerating_Read_Parameters(MotorDeratingParameters_t* value);

#endif /* COMPONENT_MOTOR_DERATING_H_ */

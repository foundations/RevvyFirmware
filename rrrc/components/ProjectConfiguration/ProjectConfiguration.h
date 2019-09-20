#ifndef COMPONENT_PROJECT_CONFIGURATION_H_
#define COMPONENT_PROJECT_CONFIGURATION_H_

#ifndef COMPONENT_TYPES_PROJECT_CONFIGURATION_H_
#define COMPONENT_TYPES_PROJECT_CONFIGURATION_H_

#include <float.h>
#include <stdint.h>

typedef float Voltage_t;

typedef struct {
    Voltage_t detectionVoltage;
    Voltage_t minVoltage;
    Voltage_t maxVoltage;
} BatteryConfiguration_t;

#endif /* COMPONENT_TYPES_PROJECT_CONFIGURATION_H_ */

void ProjectConfiguration_Constant_FooParameters(uint32_t index, BatteryConfiguration_t* value);
void ProjectConfiguration_Constant_MainBatteryParameters(BatteryConfiguration_t* value);
void ProjectConfiguration_Constant_MotorBatteryParameters(BatteryConfiguration_t* value);

#endif /* COMPONENT_PROJECT_CONFIGURATION_H_ */

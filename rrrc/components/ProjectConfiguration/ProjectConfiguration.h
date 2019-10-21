#ifndef COMPONENT_PROJECT_CONFIGURATION_H_
#define COMPONENT_PROJECT_CONFIGURATION_H_

#ifndef COMPONENT_TYPES_PROJECT_CONFIGURATION_H_
#define COMPONENT_TYPES_PROJECT_CONFIGURATION_H_

#include <float.h>
#include <stdint.h>

typedef float Voltage_t;
typedef float Temperature_t;

typedef struct {
    Voltage_t detectionVoltage;
    Voltage_t minVoltage;
    Voltage_t maxVoltage;
} BatteryConfiguration_t;

typedef struct {
    float resistance;
    float coeff_cooling;
    float coeff_heating;
} MotorThermalParameters_t;

typedef struct {
    Temperature_t MaxSafeTemperature;
    Temperature_t MaxAllowedTemperature;
} MotorDeratingParameters_t;

#endif /* COMPONENT_TYPES_PROJECT_CONFIGURATION_H_ */

uint8_t ProjectConfiguration_Constant_FirmwareOperationMode(void);
uint32_t ProjectConfiguration_Constant_ExpectedStartupTime(void);
void ProjectConfiguration_Constant_MainBatteryParameters(BatteryConfiguration_t* value);
void ProjectConfiguration_Constant_MotorBatteryParameters(BatteryConfiguration_t* value);
void ProjectConfiguration_Constant_MotorDeratingParameters(MotorDeratingParameters_t* value);
void ProjectConfiguration_Constant_MotorThermalParameters(MotorThermalParameters_t* value);

#endif /* COMPONENT_PROJECT_CONFIGURATION_H_ */

#include "ProjectConfiguration.h"
#include "utils.h"
#include "utils_assert.h"


void ProjectConfiguration_Constant_FooParameters(uint32_t index, BatteryConfiguration_t* value)
{
    ASSERT(index < 2);
    ASSERT(value != NULL);
    static const BatteryConfiguration_t data[2] = { {.detectionVoltage = 4000.0f, .minVoltage = 5400.0f, .maxVoltage = 7000.0f}, {.detectionVoltage = 4000.0f, .minVoltage = 5400.0f, .maxVoltage = 7000.0f} };
    *value = data[index];
}

void ProjectConfiguration_Constant_MainBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    *value = (BatteryConfiguration_t) {.detectionVoltage = 2000.0f, .minVoltage = 3400.0f, .maxVoltage = 4100.0f};
}

void ProjectConfiguration_Constant_MotorBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    *value = (BatteryConfiguration_t) {.detectionVoltage = 4000.0f, .minVoltage = 5400.0f, .maxVoltage = 7000.0f};
}

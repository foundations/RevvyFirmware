#include "ProjectConfiguration.h"
#include "utils.h"
#include "utils_assert.h"


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

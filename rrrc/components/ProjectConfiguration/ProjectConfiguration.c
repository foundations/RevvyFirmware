#include "ProjectConfiguration.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */

/* End User Code Section: Declarations */

uint8_t ProjectConfiguration_Constant_FirmwareOperationMode(void)
{
    /* Begin User Code Section: FirmwareOperationMode Start */

    /* End User Code Section: FirmwareOperationMode Start */
    /* Begin User Code Section: FirmwareOperationMode End */

    /* End User Code Section: FirmwareOperationMode End */
    return 0xAA;
}

uint32_t ProjectConfiguration_Constant_ExpectedStartupTime(void)
{
    /* Begin User Code Section: ExpectedStartupTime Start */

    /* End User Code Section: ExpectedStartupTime Start */
    /* Begin User Code Section: ExpectedStartupTime End */

    /* End User Code Section: ExpectedStartupTime End */
    return 32000;
}

void ProjectConfiguration_Constant_MainBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: MainBatteryParameters Start */

    /* End User Code Section: MainBatteryParameters Start */
    *value = (BatteryConfiguration_t) { .detectionVoltage = 2000.0f, .minVoltage = 3400.0f, .maxVoltage = 4100.0f };
    /* Begin User Code Section: MainBatteryParameters End */

    /* End User Code Section: MainBatteryParameters End */
}

void ProjectConfiguration_Constant_MotorBatteryParameters(BatteryConfiguration_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: MotorBatteryParameters Start */

    /* End User Code Section: MotorBatteryParameters Start */
    *value = (BatteryConfiguration_t) { .detectionVoltage = 4000.0f, .minVoltage = 5400.0f, .maxVoltage = 7000.0f };
    /* Begin User Code Section: MotorBatteryParameters End */

    /* End User Code Section: MotorBatteryParameters End */
}

void ProjectConfiguration_Constant_MotorDeratingParameters(MotorDeratingParameters_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: MotorDeratingParameters Start */

    /* End User Code Section: MotorDeratingParameters Start */
    *value = (MotorDeratingParameters_t) { .MaxSafeTemperature = 70.0f, .MaxAllowedTemperature = 130.0f };
    /* Begin User Code Section: MotorDeratingParameters End */

    /* End User Code Section: MotorDeratingParameters End */
}

void ProjectConfiguration_Constant_MotorThermalParameters(MotorThermalParameters_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: MotorThermalParameters Start */

    /* End User Code Section: MotorThermalParameters Start */
    *value = (MotorThermalParameters_t) { .resistance = 3.5f, .coeff_cooling = 0.02f, .coeff_heating = 0.2f };
    /* Begin User Code Section: MotorThermalParameters End */

    /* End User Code Section: MotorThermalParameters End */
}

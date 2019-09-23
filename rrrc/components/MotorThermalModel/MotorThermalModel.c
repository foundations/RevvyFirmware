#include "MotorThermalModel.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
typedef struct {
    float temperature;
} MotorThermalModel_t;

static MotorThermalModel_t models[6] = {};
/* End User Code Section: Declarations */

void MotorThermalModel_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    for (uint32_t i = 0u; i < ARRAY_SIZE(models); i++)
    {
        models[i].temperature = MotorThermalModel_Read_AmbientTemperature();
    }
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void MotorThermalModel_Run_OnUpdate(void)
{
    /* Begin User Code Section: OnUpdate Start */
    MotorThermalParameters_t params;
    MotorThermalModel_Read_ThermalParameters(&params);

    Temperature_t ambient = MotorThermalModel_Read_AmbientTemperature();

    for (uint32_t i = 0u; i < ARRAY_SIZE(models); i++)
    {
        float current = MotorThermalModel_Read_MotorCurrent(i);
        float power = current * current * params.resistance;

        float cooling = params.coeff_cooling * (models[i].temperature - ambient);
        float heating = params.coeff_heating * power;

        models[i].temperature += heating - cooling;
        MotorThermalModel_Write_Temperature(i, models[i].temperature);
    }
    /* End User Code Section: OnUpdate Start */
    /* Begin User Code Section: OnUpdate End */

    /* End User Code Section: OnUpdate End */
}

__attribute__((weak))
void MotorThermalModel_Write_Temperature(uint32_t index, const Temperature_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: Temperature Start */

    /* End User Code Section: Temperature Start */
    /* Begin User Code Section: Temperature End */

    /* End User Code Section: Temperature End */
}

__attribute__((weak))
Temperature_t MotorThermalModel_Read_AmbientTemperature(void)
{
    /* Begin User Code Section: AmbientTemperature Start */

    /* End User Code Section: AmbientTemperature Start */
    /* Begin User Code Section: AmbientTemperature End */

    /* End User Code Section: AmbientTemperature End */
    return (Temperature_t) 20.0f;
}

__attribute__((weak))
Current_t MotorThermalModel_Read_MotorCurrent(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorCurrent Start */

    /* End User Code Section: MotorCurrent Start */
    /* Begin User Code Section: MotorCurrent End */

    /* End User Code Section: MotorCurrent End */
    return 0.0f;
}

__attribute__((weak))
void MotorThermalModel_Read_ThermalParameters(MotorThermalParameters_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: ThermalParameters Start */

    /* End User Code Section: ThermalParameters Start */
    /* Begin User Code Section: ThermalParameters End */

    /* End User Code Section: ThermalParameters End */
}

#include "MotorThermalModel.h"
#include "utils.h"
#include "utils_assert.h"

typedef struct {
    float temperature;
} MotorThermalModel_t;

static MotorThermalModel_t models[6] = {};

void MotorThermalModel_Run_OnInit(void)
{
    for (uint32_t i = 0u; i < ARRAY_SIZE(models); i++)
    {
        models[i].temperature = MotorThermalModel_Read_AmbientTemperature();
    }
}

void MotorThermalModel_Run_OnUpdate(void)
{
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
}

__attribute__((weak))
void MotorThermalModel_Write_Temperature(uint32_t index, const Temperature_t value)
{
    ASSERT(index < 6);
}

__attribute__((weak))
Temperature_t MotorThermalModel_Read_AmbientTemperature(void)
{
    return (Temperature_t) 20.0f;
}

__attribute__((weak))
Current_t MotorThermalModel_Read_MotorCurrent(uint32_t index)
{
    ASSERT(index < 6);
}

__attribute__((weak))
void MotorThermalModel_Read_ThermalParameters(MotorThermalParameters_t* value)
{
    ASSERT(value != NULL);
}

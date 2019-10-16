#include "MotorDerating.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include <math.h>
#include "utils/functions.h"
/* End User Code Section: Declarations */

void MotorDerating_Run_OnUpdate(void)
{
    /* Begin User Code Section: OnUpdate Start */
    MotorDeratingParameters_t params;
    MotorDerating_Read_Parameters(&params);

    for (uint32_t motor_idx = 0u; motor_idx < 6u; motor_idx++)
    {
        Temperature_t temp = MotorDerating_Read_MotorTemperature(motor_idx);
        int8_t control = MotorDerating_Read_ControlValue(motor_idx);

        float derated = map_constrained(temp, params.MaxSafeTemperature, params.MaxAllowedTemperature, control, 0);

        MotorDerating_Write_DeratedControlValue(motor_idx, (int8_t)lroundf(derated));
    }
    /* End User Code Section: OnUpdate Start */
    /* Begin User Code Section: OnUpdate End */

    /* End User Code Section: OnUpdate End */
}

__attribute__((weak))
void MotorDerating_Write_DeratedControlValue(uint32_t index, const int8_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 6);
    /* Begin User Code Section: DeratedControlValue Start */

    /* End User Code Section: DeratedControlValue Start */
    /* Begin User Code Section: DeratedControlValue End */

    /* End User Code Section: DeratedControlValue End */
}

__attribute__((weak))
int8_t MotorDerating_Read_ControlValue(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: ControlValue Start */

    /* End User Code Section: ControlValue Start */
    /* Begin User Code Section: ControlValue End */

    /* End User Code Section: ControlValue End */
    return 0;
}

__attribute__((weak))
Temperature_t MotorDerating_Read_MotorTemperature(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: MotorTemperature Start */

    /* End User Code Section: MotorTemperature Start */
    /* Begin User Code Section: MotorTemperature End */

    /* End User Code Section: MotorTemperature End */
    return (Temperature_t) 20.0f;
}

__attribute__((weak))
void MotorDerating_Read_Parameters(MotorDeratingParameters_t* value)
{
    ASSERT(value != NULL);
    /* Begin User Code Section: Parameters Start */

    /* End User Code Section: Parameters Start */
    *value = (MotorDeratingParameters_t) { .MaxSafeTemperature = 0.0f, .MaxAllowedTemperature = 0.0f };
    /* Begin User Code Section: Parameters End */

    /* End User Code Section: Parameters End */
}

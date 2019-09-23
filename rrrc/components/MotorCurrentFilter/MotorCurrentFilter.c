#include "MotorCurrentFilter.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
static Current_t motorCurrents[6];
/* End User Code Section: Declarations */

void MotorCurrentFilter_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    for (uint32_t i = 0u; i < ARRAY_SIZE(motorCurrents); i++)
    {
        motorCurrents[i] = 0.0f;
    }
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void MotorCurrentFilter_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    for (uint32_t i = 0u; i < ARRAY_SIZE(motorCurrents); i++)
    {
        Current_t prev_current = motorCurrents[i];
        Current_t current = MotorCurrentFilter_Read_RawCurrent(i);

        current = current * 0.05f + prev_current * 0.95f;
        motorCurrents[i] = current;

        MotorCurrentFilter_Write_FilteredCurrent(i, current);
    }
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void MotorCurrentFilter_Write_FilteredCurrent(uint32_t index, const Current_t value)
{
    ASSERT(index < 6);
    /* Begin User Code Section: FilteredCurrent Start */

    /* End User Code Section: FilteredCurrent Start */
    /* Begin User Code Section: FilteredCurrent End */

    /* End User Code Section: FilteredCurrent End */
}

__attribute__((weak))
Current_t MotorCurrentFilter_Read_RawCurrent(uint32_t index)
{
    ASSERT(index < 6);
    /* Begin User Code Section: RawCurrent Start */

    /* End User Code Section: RawCurrent Start */
    /* Begin User Code Section: RawCurrent End */

    /* End User Code Section: RawCurrent End */
    return 0.0f;
}

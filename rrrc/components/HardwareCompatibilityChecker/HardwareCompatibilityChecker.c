#include "HardwareCompatibilityChecker.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include "rrrc_hal.h"
/* End User Code Section: Declarations */

void HardwareCompatibilityChecker_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    {
        const uint32_t compatible_hw[] = { COMPATIBLE_HW_VERSIONS };
        const uint32_t hw = FLASH_HEADER->hw_version;
        bool on_compatible_hw = false;
        for (size_t i = 0u; i < ARRAY_SIZE(compatible_hw); i++)
        {
            if (hw == compatible_hw[i])
            {
                on_compatible_hw = true;
                break;
            }
        }

        if (!on_compatible_hw)
        {
            HardwareCompatibilityChecker_Call_OnIncompatibleHardware();
        }
    }
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

__attribute__((weak))
void HardwareCompatibilityChecker_Call_OnIncompatibleHardware(void)
{
    /* Begin User Code Section: OnIncompatibleHardware Start */

    /* End User Code Section: OnIncompatibleHardware Start */
    /* Begin User Code Section: OnIncompatibleHardware End */

    /* End User Code Section: OnIncompatibleHardware End */
}

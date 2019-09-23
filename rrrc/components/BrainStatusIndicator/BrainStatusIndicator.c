#include "BrainStatusIndicator.h"
#include "utils.h"

/* Begin User Code Section: Declarations */
#include "rrrc_indication.h"
/* End User Code Section: Declarations */

void BrainStatusIndicator_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    switch (BrainStatusIndicator_Read_SystemState())
    {
        case SystemState_Startup:
            BrainStatusIndicator_Write_LedColor((rgb_t) LED_RED);
            break;

        case SystemState_Operational:
            if (BrainStatusIndicator_Read_BluetoothControllerPresent())
            {
                BrainStatusIndicator_Write_LedColor((rgb_t) LED_GREEN);
            }
            else
            {
                BrainStatusIndicator_Write_LedColor((rgb_t) LED_ORANGE);
            }
            break;

        case SystemState_Error:
        default:
            BrainStatusIndicator_Write_LedColor((rgb_t) LED_RED);
            break;
    }
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

__attribute__((weak))
void BrainStatusIndicator_Write_LedColor(const rgb_t value)
{
    /* Begin User Code Section: LedColor Start */

    /* End User Code Section: LedColor Start */
    /* Begin User Code Section: LedColor End */

    /* End User Code Section: LedColor End */
}

__attribute__((weak))
bool BrainStatusIndicator_Read_BluetoothControllerPresent(void)
{
    /* Begin User Code Section: BluetoothControllerPresent Start */

    /* End User Code Section: BluetoothControllerPresent Start */
    /* Begin User Code Section: BluetoothControllerPresent End */

    /* End User Code Section: BluetoothControllerPresent End */
    return false;
}

__attribute__((weak))
SystemState_t BrainStatusIndicator_Read_SystemState(void)
{
    /* Begin User Code Section: SystemState Start */

    /* End User Code Section: SystemState Start */
    /* Begin User Code Section: SystemState End */

    /* End User Code Section: SystemState End */
    return SystemState_Operational;
}

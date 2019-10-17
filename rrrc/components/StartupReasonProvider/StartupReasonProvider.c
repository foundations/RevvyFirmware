#include "StartupReasonProvider.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include "hri_rstc_d51.h"
/* End User Code Section: Declarations */

void StartupReasonProvider_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    if (hri_rstc_get_RCAUSE_POR_bit(RSTC))
    {
        StartupReasonProvider_Write_IsColdStart(true);
    }
    else
    {
        StartupReasonProvider_Write_IsColdStart(false);
    }
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

__attribute__((weak))
void StartupReasonProvider_Write_IsColdStart(const bool value)
{
    (void) value;
    /* Begin User Code Section: IsColdStart Start */

    /* End User Code Section: IsColdStart Start */
    /* Begin User Code Section: IsColdStart End */

    /* End User Code Section: IsColdStart End */
}

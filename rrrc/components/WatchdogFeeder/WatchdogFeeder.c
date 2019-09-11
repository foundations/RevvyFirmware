#include "WatchdogFeeder.h"

#include <compiler.h>
#include "hri_wdt_d51.h"

void WatchdogFeeder_Run_Feed(void)
{
    hri_wdt_write_CLEAR_reg(WDT, WDT_CLEAR_CLEAR_KEY);
}

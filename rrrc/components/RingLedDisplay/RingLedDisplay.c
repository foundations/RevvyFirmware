#include "RingLedDisplay.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include "RingLedDisplay_private.h"

#include <string.h>

static RingLedScenario_t current_scenario;
static const indication_handler_t* current_scenario_handler;
static uint32_t time_since_startup;

static uint8_t copy_ring_led_scenario_name(const char* name, ByteArray_t destination)
{
    size_t length = strlen(name);

    if (length > destination.count)
    {
        return 0u;
    }

    memcpy(destination.bytes, name, length);
    return length;
}

/* End User Code Section: Declarations */

void RingLedDisplay_Run_OnInit(void)
{
    /* Begin User Code Section: OnInit Start */
    bool display_default_animation = RingLedDisplay_Read_WaitForMasterStartup()
                                     && !RingLedDisplay_Read_MasterReady();

    time_since_startup = 0u;
    if (display_default_animation)
    {
        /* current_scenario will be set to a "to-be-changed" value if default animation ends */
        current_scenario_handler = &startup_indicator_scenario;
    }
    else
    {
        current_scenario = RingLedDisplay_Read_Scenario();
        current_scenario_handler = &public_scenario_handlers[current_scenario];
    }

    if (current_scenario_handler->init)
    {
        current_scenario_handler->init(current_scenario_handler->userData);
    }
    /* End User Code Section: OnInit Start */
    /* Begin User Code Section: OnInit End */

    /* End User Code Section: OnInit End */
}

void RingLedDisplay_Run_Update(void)
{
    /* Begin User Code Section: Update Start */
    RingLedScenario_t requested_scenario = RingLedDisplay_Read_Scenario();

    bool display_default_animation = RingLedDisplay_Read_WaitForMasterStartup()
                                     && !RingLedDisplay_Read_MasterReady()
                                     && (time_since_startup < RingLedDisplay_Read_ExpectedStartupTime());

    if (display_default_animation)
    {
        time_since_startup += 20u;

        if (time_since_startup >= RingLedDisplay_Read_ExpectedStartupTime())
        {
            /* force switch to requested scenario */
            current_scenario = requested_scenario + 1u;
            display_default_animation = false;
        }
    }

    if (!display_default_animation)
    {
        if (current_scenario != requested_scenario)
        {
            if (current_scenario_handler->DeInit)
            {
                current_scenario_handler->DeInit(current_scenario_handler->userData);
            }

            current_scenario = requested_scenario;
            current_scenario_handler = &public_scenario_handlers[current_scenario];

            ASSERT(current_scenario_handler);
            ASSERT(current_scenario_handler->handler);

            if (current_scenario_handler->init)
            {
                current_scenario_handler->init(current_scenario_handler->userData);
            }
        }
    }

    current_scenario_handler->handler(current_scenario_handler->userData);
    /* End User Code Section: Update Start */
    /* Begin User Code Section: Update End */

    /* End User Code Section: Update End */
}

uint8_t RingLedDisplay_Run_ReadScenarioName(RingLedScenario_t scenario, ByteArray_t destination)
{
    /* Begin User Code Section: ReadScenarioName Start */
    switch (scenario)
    {
        case RingLedScenario_Off: return copy_ring_led_scenario_name("RingLedOff", destination);
        case RingLedScenario_UserFrame: return copy_ring_led_scenario_name("UserFrame", destination);
        case RingLedScenario_ColorWheel: return copy_ring_led_scenario_name("ColorWheel", destination);
        case RingLedScenario_RainbowFade: return copy_ring_led_scenario_name("RainbowFade", destination);
        case RingLedScenario_BusyIndicator: return copy_ring_led_scenario_name("BusyRing", destination);
        case RingLedScenario_BreathingGreen: return copy_ring_led_scenario_name("BreathingGreen", destination);
        default: return 0u;
    }
    /* End User Code Section: ReadScenarioName Start */
    /* Begin User Code Section: ReadScenarioName End */

    /* End User Code Section: ReadScenarioName End */
}

__attribute__((weak))
void RingLedDisplay_Write_LedColor(uint32_t index, const rgb_t value)
{
    (void) value;
    (void) index;
    ASSERT(index < 12);
    /* Begin User Code Section: LedColor Start */

    /* End User Code Section: LedColor Start */
    /* Begin User Code Section: LedColor End */

    /* End User Code Section: LedColor End */
}

__attribute__((weak))
uint32_t RingLedDisplay_Read_ExpectedStartupTime(void)
{
    /* Begin User Code Section: ExpectedStartupTime Start */

    /* End User Code Section: ExpectedStartupTime Start */
    /* Begin User Code Section: ExpectedStartupTime End */

    /* End User Code Section: ExpectedStartupTime End */
    return 0u;
}

__attribute__((weak))
bool RingLedDisplay_Read_MasterReady(void)
{
    /* Begin User Code Section: MasterReady Start */

    /* End User Code Section: MasterReady Start */
    /* Begin User Code Section: MasterReady End */

    /* End User Code Section: MasterReady End */
    return false;
}

__attribute__((weak))
RingLedScenario_t RingLedDisplay_Read_Scenario(void)
{
    /* Begin User Code Section: Scenario Start */

    /* End User Code Section: Scenario Start */
    /* Begin User Code Section: Scenario End */

    /* End User Code Section: Scenario End */
    return RingLedScenario_Off;
}

__attribute__((weak))
rgb_t RingLedDisplay_Read_UserColors(uint32_t index)
{
    ASSERT(index < 12);
    /* Begin User Code Section: UserColors Start */

    /* End User Code Section: UserColors Start */
    /* Begin User Code Section: UserColors End */

    /* End User Code Section: UserColors End */
    return (rgb_t){0, 0, 0};
}

__attribute__((weak))
bool RingLedDisplay_Read_WaitForMasterStartup(void)
{
    /* Begin User Code Section: WaitForMasterStartup Start */

    /* End User Code Section: WaitForMasterStartup Start */
    /* Begin User Code Section: WaitForMasterStartup End */

    /* End User Code Section: WaitForMasterStartup End */
    return false;
}

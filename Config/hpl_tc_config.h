/* Auto-generated config file hpl_tc_config.h */
#ifndef HPL_TC_CONFIG_H
#define HPL_TC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef CONF_TC0_ENABLE
#define CONF_TC0_ENABLE 1
#endif

#include "peripheral_clk_config.h"

// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC0_PRESCALER
#define CONF_TC0_PRESCALER 0x4
#endif

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC0_TIMER_TICK
#define CONF_TC0_TIMER_TICK 1000
#endif
// </h>

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC0__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC0__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC0_PRESCSYNC
#define CONF_TC0_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC0_RUNSTDBY
#define CONF_TC0_RUNSTDBY 0
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC0_DBGRUN
#define CONF_TC0_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC0_ONDEMAND
#define CONF_TC0_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC0_EVENT_CONTROL_ENABLE
#define CONF_TC0_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC0_MCEO0
#define CONF_TC0_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC0_MCEO1
#define CONF_TC0_MCEO1 1
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC0_OVFEO
#define CONF_TC0_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC0_TCEI
#define CONF_TC0_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC0_TCINV
#define CONF_TC0_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC0_EVACT
#define CONF_TC0_EVACT 4
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC0_MODE
#define CONF_TC0_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

// Calculating correct top value based on requested tick interval.
#define CONF_TC0_PRESCALE (1 << CONF_TC0_PRESCALER)

// Prescaler set to 64
#if CONF_TC0_PRESCALER > 0x4
#undef CONF_TC0_PRESCALE
#define CONF_TC0_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC0_PRESCALER > 0x5
#undef CONF_TC0_PRESCALE
#define CONF_TC0_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC0_PRESCALER > 0x6
#undef CONF_TC0_PRESCALE
#define CONF_TC0_PRESCALE 1024
#endif

// CC 1 register set to 0
#ifndef CONF_TC0_CC1
#define CONF_TC0_CC1                                                                                                   \
	0//(uint32_t)(((float)CONF_TC0_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC0_FREQUENCY / CONF_TC0_PRESCALE)))
#endif

#ifndef CONF_TC0_ALOCK
#define CONF_TC0_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC0_PER 0

#ifndef CONF_TC0_CC0
#define CONF_TC0_CC0                                                                                                   \
	0//(uint32_t)(((float)CONF_TC0_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC0_FREQUENCY / CONF_TC0_PRESCALE)))
#endif


#ifndef CONF_TC0_COPEN0
#define CONF_TC0_COPEN0 1
#endif

#ifndef CONF_TC0_COPEN1
#define CONF_TC0_COPEN1 1
#endif

#ifndef CONF_TC0_CAPTEN0
#define CONF_TC0_CAPTEN0 1
#endif

#ifndef CONF_TC0_CAPTEN1
#define CONF_TC0_CAPTEN1 1
#endif

#ifndef CONF_TC0_DRVCTRLEN0
#define CONF_TC0_DRVCTRLEN0 1
#endif

#ifndef CONF_TC0_DRVCTRLEN1
#define CONF_TC0_DRVCTRLEN1 0
#endif




#ifndef CONF_TC1_ENABLE
#define CONF_TC1_ENABLE 1
#endif

#include "peripheral_clk_config.h"

// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC1_PRESCALER
#define CONF_TC1_PRESCALER 0x4
#endif

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC1_TIMER_TICK
#define CONF_TC1_TIMER_TICK 1000
#endif
// </h>

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC1__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC1__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC1_PRESCSYNC
#define CONF_TC1_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC1_RUNSTDBY
#define CONF_TC1_RUNSTDBY 0
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC1_DBGRUN
#define CONF_TC1_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC1_ONDEMAND
#define CONF_TC1_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC1_EVENT_CONTROL_ENABLE
#define CONF_TC1_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC1_MCEO0
#define CONF_TC1_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC1_MCEO1
#define CONF_TC1_MCEO1 1
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC1_OVFEO
#define CONF_TC1_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC1_TCEI
#define CONF_TC1_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC1_TCINV
#define CONF_TC1_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC1_EVACT
#define CONF_TC1_EVACT 4
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC1_MODE
#define CONF_TC1_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

// CC 1 register set to 0
#ifndef CONF_TC1_CC1
#define CONF_TC1_CC1                                                                                                   \
	0//(uint32_t)(((float)CONF_TC1_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC1_FREQUENCY / CONF_TC1_PRESCALE)))
#endif

#ifndef CONF_TC1_ALOCK
#define CONF_TC1_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC1_PER 0

// Calculating correct top value based on requested tick interval.
#define CONF_TC1_PRESCALE (1 << CONF_TC1_PRESCALER)

// Prescaler set to 64
#if CONF_TC1_PRESCALER > 0x4
#undef CONF_TC1_PRESCALE
#define CONF_TC1_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC1_PRESCALER > 0x5
#undef CONF_TC1_PRESCALE
#define CONF_TC1_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC1_PRESCALER > 0x6
#undef CONF_TC1_PRESCALE
#define CONF_TC1_PRESCALE 1024
#endif

#ifndef CONF_TC1_CC0
#define CONF_TC1_CC0                                                                                                   \
	0//(uint32_t)(((float)CONF_TC1_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC1_FREQUENCY / CONF_TC1_PRESCALE)))
#endif

#ifndef CONF_TC1_COPEN0
#define CONF_TC1_COPEN0 1
#endif

#ifndef CONF_TC1_COPEN1
#define CONF_TC1_COPEN1 1
#endif

#ifndef CONF_TC1_CAPTEN0
#define CONF_TC1_CAPTEN0 1
#endif

#ifndef CONF_TC1_CAPTEN1
#define CONF_TC1_CAPTEN1 1
#endif

#ifndef CONF_TC1_DRVCTRLEN0
#define CONF_TC1_DRVCTRLEN0 1
#endif

#ifndef CONF_TC1_DRVCTRLEN1
#define CONF_TC1_DRVCTRLEN1 0
#endif


#ifndef CONF_TC2_ENABLE
#define CONF_TC2_ENABLE 1
#endif

#include "peripheral_clk_config.h"

// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC2_PRESCALER
#define CONF_TC2_PRESCALER 0x07
#endif

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC2__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC2__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC2_PRESCSYNC
#define CONF_TC2_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC2_RUNSTDBY
#define CONF_TC2_RUNSTDBY 1
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC2_DBGRUN
#define CONF_TC2_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC2_ONDEMAND
#define CONF_TC2_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC2_EVENT_CONTROL_ENABLE
#define CONF_TC2_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC2_MCEO0
#define CONF_TC2_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC2_MCEO1
#define CONF_TC2_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC2_OVFEO
#define CONF_TC2_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC2_TCEI
#define CONF_TC2_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC2_TCINV
#define CONF_TC2_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC2_EVACT
#define CONF_TC2_EVACT 3
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC2_MODE
#define CONF_TC2_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

// CC 1 register set to 0
#ifndef CONF_TC2_CC1
#define CONF_TC2_CC1 0xFFFFFFFF
#endif

#ifndef CONF_TC2_ALOCK
#define CONF_TC2_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC2_PER 0

// Calculating correct top value based on requested tick interval.
#define CONF_TC2_PRESCALE (1 << CONF_TC2_PRESCALER)

// Prescaler set to 64
#if CONF_TC2_PRESCALER > 0x4
#undef CONF_TC2_PRESCALE
#define CONF_TC2_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC2_PRESCALER > 0x5
#undef CONF_TC2_PRESCALE
#define CONF_TC2_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC2_PRESCALER > 0x6
#undef CONF_TC2_PRESCALE
#define CONF_TC2_PRESCALE 1024
#endif

#ifndef CONF_TC2_CC0
#define CONF_TC2_CC0 0xFFFFFFFF
#endif


#ifndef CONF_TC2_COPEN0
#define CONF_TC2_COPEN0 1
#endif

#ifndef CONF_TC2_COPEN1
#define CONF_TC2_COPEN1 0
#endif

#ifndef CONF_TC2_CAPTEN0
#define CONF_TC2_CAPTEN0 0
#endif

#ifndef CONF_TC2_CAPTEN1
#define CONF_TC2_CAPTEN1 0
#endif

#ifndef CONF_TC2_DRVCTRLEN0
#define CONF_TC2_DRVCTRLEN0 0
#endif

#ifndef CONF_TC2_DRVCTRLEN1
#define CONF_TC2_DRVCTRLEN1 0
#endif


#ifndef CONF_TC3_ENABLE
#define CONF_TC3_ENABLE 1
#endif

#include "peripheral_clk_config.h"

// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC3_PRESCALER
#define CONF_TC3_PRESCALER 0x4
#endif

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC3_TIMER_TICK
#define CONF_TC3_TIMER_TICK 1000
#endif
// </h>

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC3__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC3__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC3_PRESCSYNC
#define CONF_TC3_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC3_RUNSTDBY
#define CONF_TC3_RUNSTDBY 0
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC3_DBGRUN
#define CONF_TC3_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC3_ONDEMAND
#define CONF_TC3_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC3_EVENT_CONTROL_ENABLE
#define CONF_TC3_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC3_MCEO0
#define CONF_TC3_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC3_MCEO1
#define CONF_TC3_MCEO1 1
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC3_OVFEO
#define CONF_TC3_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC3_TCEI
#define CONF_TC3_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC3_TCINV
#define CONF_TC3_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC3_EVACT
#define CONF_TC3_EVACT 4
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC3_MODE
#define CONF_TC3_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

// CC 1 register set to 0
#ifndef CONF_TC3_CC1
#define CONF_TC3_CC1                                                                                                   \
	0//(uint32_t)(((float)CONF_TC3_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC3_FREQUENCY / CONF_TC3_PRESCALE)))
#endif

#ifndef CONF_TC3_ALOCK
#define CONF_TC3_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC3_PER 0

// Calculating correct top value based on requested tick interval.
#define CONF_TC3_PRESCALE (1 << CONF_TC3_PRESCALER)

// Prescaler set to 64
#if CONF_TC3_PRESCALER > 0x4
#undef CONF_TC3_PRESCALE
#define CONF_TC3_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC3_PRESCALER > 0x5
#undef CONF_TC3_PRESCALE
#define CONF_TC3_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC3_PRESCALER > 0x6
#undef CONF_TC3_PRESCALE
#define CONF_TC3_PRESCALE 1024
#endif

#ifndef CONF_TC3_CC0
#define CONF_TC3_CC0                                                                                                   \
	0//(uint32_t)(((float)CONF_TC3_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC3_FREQUENCY / CONF_TC3_PRESCALE)))
#endif


#ifndef CONF_TC3_COPEN0
#define CONF_TC3_COPEN0 1
#endif

#ifndef CONF_TC3_COPEN1
#define CONF_TC3_COPEN1 1
#endif

#ifndef CONF_TC3_CAPTEN0
#define CONF_TC3_CAPTEN0 1
#endif

#ifndef CONF_TC3_CAPTEN1
#define CONF_TC3_CAPTEN1 1
#endif

#ifndef CONF_TC3_DRVCTRLEN0
#define CONF_TC3_DRVCTRLEN0 1
#endif

#ifndef CONF_TC3_DRVCTRLEN1
#define CONF_TC3_DRVCTRLEN1 0
#endif


#ifndef CONF_TC4_ENABLE
#define CONF_TC4_ENABLE 1
#endif

#include "peripheral_clk_config.h"

// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC4_PRESCALER
#define CONF_TC4_PRESCALER 0x4
#endif

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC4_TIMER_TICK
#define CONF_TC4_TIMER_TICK 1000
#endif
// </h>

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC4__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC4__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC4_PRESCSYNC
#define CONF_TC4_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC4_RUNSTDBY
#define CONF_TC4_RUNSTDBY 0
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC4_DBGRUN
#define CONF_TC4_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC4_ONDEMAND
#define CONF_TC4_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC4_EVENT_CONTROL_ENABLE
#define CONF_TC4_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC4_MCEO0
#define CONF_TC4_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC4_MCEO1
#define CONF_TC4_MCEO1 1
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC4_OVFEO
#define CONF_TC4_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC4_TCEI
#define CONF_TC4_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC4_TCINV
#define CONF_TC4_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC4_EVACT
#define CONF_TC4_EVACT 4
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC4_MODE
#define CONF_TC4_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

// CC 1 register set to 0
#ifndef CONF_TC4_CC1
#define CONF_TC4_CC1                                                                                                   \
	0//(uint32_t)(((float)CONF_TC4_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC4_FREQUENCY / CONF_TC4_PRESCALE)))
#endif

#ifndef CONF_TC4_ALOCK
#define CONF_TC4_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC4_PER 0

// Calculating correct top value based on requested tick interval.
#define CONF_TC4_PRESCALE (1 << CONF_TC4_PRESCALER)

// Prescaler set to 64
#if CONF_TC4_PRESCALER > 0x4
#undef CONF_TC4_PRESCALE
#define CONF_TC4_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC4_PRESCALER > 0x5
#undef CONF_TC4_PRESCALE
#define CONF_TC4_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC4_PRESCALER > 0x6
#undef CONF_TC4_PRESCALE
#define CONF_TC4_PRESCALE 1024
#endif

#ifndef CONF_TC4_CC0
#define CONF_TC4_CC0                                                                                                   \
	0//(uint32_t)(((float)CONF_TC4_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC4_FREQUENCY / CONF_TC4_PRESCALE)))
#endif


#ifndef CONF_TC4_COPEN0
#define CONF_TC4_COPEN0 1
#endif

#ifndef CONF_TC4_COPEN1
#define CONF_TC4_COPEN1 1
#endif

#ifndef CONF_TC4_CAPTEN0
#define CONF_TC4_CAPTEN0 1
#endif

#ifndef CONF_TC4_CAPTEN1
#define CONF_TC4_CAPTEN1 1
#endif

#ifndef CONF_TC4_DRVCTRLEN0
#define CONF_TC4_DRVCTRLEN0 1
#endif

#ifndef CONF_TC4_DRVCTRLEN1
#define CONF_TC4_DRVCTRLEN1 0
#endif


#ifndef CONF_TC5_ENABLE
#define CONF_TC5_ENABLE 1
#endif

#include "peripheral_clk_config.h"

// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC5_PRESCALER
#define CONF_TC5_PRESCALER 0x4
#endif

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC5_TIMER_TICK
#define CONF_TC5_TIMER_TICK 1000
#endif
// </h>

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC5__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC5__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC5_PRESCSYNC
#define CONF_TC5_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC5_RUNSTDBY
#define CONF_TC5_RUNSTDBY 0
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC5_DBGRUN
#define CONF_TC5_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC5_ONDEMAND
#define CONF_TC5_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC5_EVENT_CONTROL_ENABLE
#define CONF_TC5_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC5_MCEO0
#define CONF_TC5_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC5_MCEO1
#define CONF_TC5_MCEO1 1
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC5_OVFEO
#define CONF_TC5_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC5_TCEI
#define CONF_TC5_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC5_TCINV
#define CONF_TC5_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC5_EVACT
#define CONF_TC5_EVACT 4
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC5_MODE
#define CONF_TC5_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

// CC 1 register set to 0
#ifndef CONF_TC5_CC1
#define CONF_TC5_CC1                                                                                                   \
	0//(uint32_t)(((float)CONF_TC5_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC5_FREQUENCY / CONF_TC5_PRESCALE)))
#endif

#ifndef CONF_TC5_ALOCK
#define CONF_TC5_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC5_PER 0

// Calculating correct top value based on requested tick interval.
#define CONF_TC5_PRESCALE (1 << CONF_TC5_PRESCALER)

// Prescaler set to 64
#if CONF_TC5_PRESCALER > 0x4
#undef CONF_TC5_PRESCALE
#define CONF_TC5_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC5_PRESCALER > 0x5
#undef CONF_TC5_PRESCALE
#define CONF_TC5_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC5_PRESCALER > 0x6
#undef CONF_TC5_PRESCALE
#define CONF_TC5_PRESCALE 1024
#endif

#ifndef CONF_TC5_CC0
#define CONF_TC5_CC0                                                                                                   \
	0//(uint32_t)(((float)CONF_TC5_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC5_FREQUENCY / CONF_TC5_PRESCALE)))
#endif


#ifndef CONF_TC5_COPEN0
#define CONF_TC5_COPEN0 1
#endif

#ifndef CONF_TC5_COPEN1
#define CONF_TC5_COPEN1 1
#endif

#ifndef CONF_TC5_CAPTEN0
#define CONF_TC5_CAPTEN0 1
#endif

#ifndef CONF_TC5_CAPTEN1
#define CONF_TC5_CAPTEN1 1
#endif

#ifndef CONF_TC5_DRVCTRLEN0
#define CONF_TC5_DRVCTRLEN0 1
#endif

#ifndef CONF_TC5_DRVCTRLEN1
#define CONF_TC5_DRVCTRLEN1 0
#endif


#ifndef CONF_TC6_ENABLE
#define CONF_TC6_ENABLE 1
#endif



#include "peripheral_clk_config.h"

// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC6_PRESCALER
#define CONF_TC6_PRESCALER 0x4
#endif

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC6_TIMER_TICK
#define CONF_TC6_TIMER_TICK 1000
#endif
// </h>

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC6__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC6__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC6_PRESCSYNC
#define CONF_TC6_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC6_RUNSTDBY
#define CONF_TC6_RUNSTDBY 0
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC6_DBGRUN
#define CONF_TC6_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC6_ONDEMAND
#define CONF_TC6_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC6_EVENT_CONTROL_ENABLE
#define CONF_TC6_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC6_MCEO0
#define CONF_TC6_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC6_MCEO1
#define CONF_TC6_MCEO1 1
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC6_OVFEO
#define CONF_TC6_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC6_TCEI
#define CONF_TC6_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC6_TCINV
#define CONF_TC6_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC6_EVACT
#define CONF_TC6_EVACT 4
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC6_MODE
#define CONF_TC6_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

// CC 1 register set to 0
#ifndef CONF_TC6_CC1
#define CONF_TC6_CC1 0
#endif

#ifndef CONF_TC6_ALOCK
#define CONF_TC6_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC6_PER 0

// Calculating correct top value based on requested tick interval.
#define CONF_TC6_PRESCALE (1 << CONF_TC6_PRESCALER)

// Prescaler set to 64
#if CONF_TC6_PRESCALER > 0x4
#undef CONF_TC6_PRESCALE
#define CONF_TC6_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC6_PRESCALER > 0x5
#undef CONF_TC6_PRESCALE
#define CONF_TC6_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC6_PRESCALER > 0x6
#undef CONF_TC6_PRESCALE
#define CONF_TC6_PRESCALE 1024
#endif

#ifndef CONF_TC6_CC0
#define CONF_TC6_CC0                                                                                                   \
	(uint32_t)(((float)CONF_TC6_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC6_FREQUENCY / CONF_TC6_PRESCALE)))
#endif


#ifndef CONF_TC6_COPEN0
#define CONF_TC6_COPEN0 1
#endif

#ifndef CONF_TC6_COPEN1
#define CONF_TC6_COPEN1 1
#endif

#ifndef CONF_TC6_CAPTEN0
#define CONF_TC6_CAPTEN0 1
#endif

#ifndef CONF_TC6_CAPTEN1
#define CONF_TC6_CAPTEN1 1
#endif

#ifndef CONF_TC6_DRVCTRLEN0
#define CONF_TC6_DRVCTRLEN0 1
#endif

#ifndef CONF_TC6_DRVCTRLEN1
#define CONF_TC6_DRVCTRLEN1 0
#endif


#ifndef CONF_TC7_ENABLE
#define CONF_TC7_ENABLE 1
#endif

#include "peripheral_clk_config.h"

// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC7_PRESCALER
#define CONF_TC7_PRESCALER 0x4
#endif

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC7_TIMER_TICK
#define CONF_TC7_TIMER_TICK 1000
#endif
// </h>

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC7__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC7__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC7_PRESCSYNC
#define CONF_TC7_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC7_RUNSTDBY
#define CONF_TC7_RUNSTDBY 0
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC7_DBGRUN
#define CONF_TC7_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC7_ONDEMAND
#define CONF_TC7_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC7_EVENT_CONTROL_ENABLE
#define CONF_TC7_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC7_MCEO0
#define CONF_TC7_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC7_MCEO1
#define CONF_TC7_MCEO1 1
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC7_OVFEO
#define CONF_TC7_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC7_TCEI
#define CONF_TC7_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC7_TCINV
#define CONF_TC7_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC7_EVACT
#define CONF_TC7_EVACT 4
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC7_MODE
#define CONF_TC7_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

// CC 1 register set to 0
#ifndef CONF_TC7_CC1
#define CONF_TC7_CC1 0
#endif

#ifndef CONF_TC7_ALOCK
#define CONF_TC7_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC7_PER 0

// Calculating correct top value based on requested tick interval.
#define CONF_TC7_PRESCALE (1 << CONF_TC7_PRESCALER)

// Prescaler set to 64
#if CONF_TC7_PRESCALER > 0x4
#undef CONF_TC7_PRESCALE
#define CONF_TC7_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC7_PRESCALER > 0x5
#undef CONF_TC7_PRESCALE
#define CONF_TC7_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC7_PRESCALER > 0x6
#undef CONF_TC7_PRESCALE
#define CONF_TC7_PRESCALE 1024
#endif

#ifndef CONF_TC7_CC0
#define CONF_TC7_CC0                                                                                                   \
	(uint32_t)(((float)CONF_TC7_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC7_FREQUENCY / CONF_TC7_PRESCALE)))
#endif

#ifndef CONF_TC7_COPEN0
#define CONF_TC7_COPEN0 1
#endif

#ifndef CONF_TC7_COPEN1
#define CONF_TC7_COPEN1 1
#endif

#ifndef CONF_TC7_CAPTEN0
#define CONF_TC7_CAPTEN0 1
#endif

#ifndef CONF_TC7_CAPTEN1
#define CONF_TC7_CAPTEN1 1
#endif

#ifndef CONF_TC7_DRVCTRLEN0
#define CONF_TC7_DRVCTRLEN0 1
#endif

#ifndef CONF_TC7_DRVCTRLEN1
#define CONF_TC7_DRVCTRLEN1 0
#endif

// <<< end of configuration section >>>


#ifndef CONF_TC0_CAPTMODE0
#define CONF_TC0_CAPTMODE0 2
#endif

#ifndef CONF_TC0_CAPTMODE1
#define CONF_TC0_CAPTMODE1 2
#endif


#ifndef CONF_TC1_CAPTMODE0
#define CONF_TC1_CAPTMODE0 2
#endif

#ifndef CONF_TC1_CAPTMODE1
#define CONF_TC1_CAPTMODE1 2
#endif


#ifndef CONF_TC2_CAPTMODE0
#define CONF_TC2_CAPTMODE0 2
#endif

#ifndef CONF_TC2_CAPTMODE1
#define CONF_TC2_CAPTMODE1 2
#endif


#ifndef CONF_TC3_CAPTMODE0
#define CONF_TC3_CAPTMODE0 2
#endif

#ifndef CONF_TC3_CAPTMODE1
#define CONF_TC3_CAPTMODE1 2
#endif


#ifndef CONF_TC4_CAPTMODE0
#define CONF_TC4_CAPTMODE0 2
#endif

#ifndef CONF_TC4_CAPTMODE1
#define CONF_TC4_CAPTMODE1 2
#endif


#ifndef CONF_TC5_CAPTMODE0
#define CONF_TC5_CAPTMODE0 2
#endif

#ifndef CONF_TC5_CAPTMODE1
#define CONF_TC5_CAPTMODE1 2
#endif


#ifndef CONF_TC6_CAPTMODE0
#define CONF_TC6_CAPTMODE0 2
#endif

#ifndef CONF_TC6_CAPTMODE1
#define CONF_TC6_CAPTMODE1 2
#endif


#ifndef CONF_TC7_CAPTMODE0
#define CONF_TC7_CAPTMODE0 2
#endif

#ifndef CONF_TC7_CAPTMODE1
#define CONF_TC7_CAPTMODE1 2
#endif




#endif // HPL_TC_CONFIG_H

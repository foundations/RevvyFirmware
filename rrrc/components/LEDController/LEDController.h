/*
 * LEDController.h
 *
 * Created: 03/05/2019 10:30:59
 *  Author: Dániel Buga
 */ 


#ifndef COMPONENT_LEDCONTROLLER_H_
#define COMPONENT_LEDCONTROLLER_H_

#include "utils/color.h"

#include <stdbool.h>

void LEDController_Run_OnInit(void);
void LEDController_Run_Update(void);
uint8_t LEDController_Read_MaxBrightness(void);

rgb565_t LEDController_Read_StatusLED(uint32_t led_idx);
rgb565_t LEDController_Read_RingLED(uint32_t led_idx);

#endif /* LEDCONTROLLER_H_ */


/**
 * \file
 *
 * \brief RTC Driver
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include <hpl_calendar.h>
#include <hpl_timer.h>
#include <utils_assert.h>
#include <hpl_rtc_config.h>

/*!< Pointer to hpl device */
static struct _timer_device *_rtc_dev = NULL;

/**
 * \brief Initialize Timer
 */
int32_t _rtc_timer_init(struct _timer_device *const dev, void *const hw)
{
	ASSERT(dev);

	dev->hw = hw;

	hri_rtcmode0_write_CTRLA_reg(dev->hw, RTC_MODE0_CTRLA_SWRST);
	hri_rtcmode0_wait_for_sync(dev->hw, RTC_MODE0_SYNCBUSY_SWRST);

#if CONF_RTC_EVENT_CONTROL_ENABLE == 1
	hri_rtcmode0_write_EVCTRL_reg(
	    dev->hw,
	    (CONF_RTC_PEREO0 << RTC_MODE0_EVCTRL_PEREO0_Pos) | (CONF_RTC_PEREO1 << RTC_MODE0_EVCTRL_PEREO1_Pos)
	        | (CONF_RTC_PEREO2 << RTC_MODE0_EVCTRL_PEREO2_Pos) | (CONF_RTC_PEREO3 << RTC_MODE0_EVCTRL_PEREO3_Pos)
	        | (CONF_RTC_PEREO4 << RTC_MODE0_EVCTRL_PEREO4_Pos) | (CONF_RTC_PEREO5 << RTC_MODE0_EVCTRL_PEREO5_Pos)
	        | (CONF_RTC_PEREO6 << RTC_MODE0_EVCTRL_PEREO6_Pos) | (CONF_RTC_PEREO7 << RTC_MODE0_EVCTRL_PEREO7_Pos)
	        | (CONF_RTC_COMPE0 << RTC_MODE0_EVCTRL_CMPEO_Pos) | (CONF_RTC_COMPE1 << RTC_MODE0_EVCTRL_CMPEO1_Pos)
	        | (CONF_RTC_TAMPEREO << RTC_MODE0_EVCTRL_TAMPEREO_Pos)
	        | (CONF_RTC_TAMPEVEI << RTC_MODE0_EVCTRL_TAMPEVEI_Pos) | (CONF_RTC_OVFEO << RTC_MODE0_EVCTRL_OVFEO_Pos));
#endif

	hri_rtcmode0_write_CTRLA_reg(
	    dev->hw, RTC_MODE0_CTRLA_PRESCALER(CONF_RTC_PRESCALER) | RTC_MODE0_CTRLA_COUNTSYNC | RTC_MODE0_CTRLA_MATCHCLR);
	hri_rtcmode0_write_COMP_reg(dev->hw, 0, CONF_RTC_COMP_VAL);
	hri_rtcmode0_set_INTEN_CMP0_bit(dev->hw);

	_rtc_dev = dev;

	return ERR_NONE;
}

/**
 * \brief De-initialize Timer
 */
void _rtc_timer_deinit(struct _timer_device *const dev)
{
	ASSERT(dev && dev->hw);

	NVIC_DisableIRQ(RTC_IRQn);

	hri_rtcmode0_write_CTRLA_reg(dev->hw, RTC_MODE0_CTRLA_SWRST);
}

/**
 * \brief Start hardware timer
 */
void _rtc_timer_start(struct _timer_device *const dev)
{
	ASSERT(dev && dev->hw);

	NVIC_EnableIRQ(RTC_IRQn);
	hri_rtcmode0_write_COUNT_reg(dev->hw, 0);
	hri_rtcmode0_wait_for_sync(dev->hw, RTC_MODE0_SYNCBUSY_COUNT);
	hri_rtcmode0_set_CTRLA_ENABLE_bit(dev->hw);
}

/**
 * \brief Stop hardware timer
 */
void _rtc_timer_stop(struct _timer_device *const dev)
{
	ASSERT(dev && dev->hw);

	hri_rtcmode0_clear_CTRLA_ENABLE_bit(dev->hw);
}

/**
 * \brief Set timer period
 */
void _rtc_timer_set_period(struct _timer_device *const dev, const uint32_t clock_cycles)
{
	hri_rtcmode0_write_COMP_reg(dev->hw, 0, clock_cycles);
}

/**
 * \brief Retrieve timer period
 */
uint32_t _rtc_timer_get_period(const struct _timer_device *const dev)
{
	return hri_rtcmode0_read_COMP_reg(dev->hw, 0);
}

/**
 * \brief Check if timer is running
 */
bool _rtc_timer_is_started(const struct _timer_device *const dev)
{
	return hri_rtcmode0_get_CTRLA_ENABLE_bit(dev->hw);
}

/**
 * \brief Set timer IRQ
 */
void _rtc_timer_set_irq(struct _timer_device *const dev)
{
	(void)dev;
}

/**
 * \brief RTC Timer interrupt handler
 *
 * \param[in] p The pointer to calendar device struct
 */
static void _rtc_timer_interrupt_handler(struct _timer_device *dev)
{
	/* Read and mask interrupt flag register */
	uint16_t flag = hri_rtcmode0_read_INTFLAG_reg(dev->hw);

	if (flag & RTC_MODE0_INTFLAG_CMP0) {
		if (dev->timer_cb.period_expired) {
			dev->timer_cb.period_expired(dev);
		}
		/* Clear interrupt flag */
		hri_rtcmode0_clear_interrupt_CMP0_bit(dev->hw);
	}
}
/**
 * \brief Set of pointer to hal_timer helper functions
 */
static struct _timer_hpl_interface _rtc_timer_functions = {&_rtc_timer_init,
                                                           &_rtc_timer_deinit,
                                                           &_rtc_timer_start,
                                                           &_rtc_timer_stop,
                                                           &_rtc_timer_set_period,
                                                           &_rtc_timer_get_period,
                                                           &_rtc_timer_is_started,
                                                           &_rtc_timer_set_irq,
                                                           NULL};

/**
 * \brief Retrieve timer helper functions
 */
struct _timer_hpl_interface *_rtc_get_timer(void)
{
	return &_rtc_timer_functions;
}

/**
 * \brief Rtc interrupt handler
 */
void RTC_Handler(void)
{
	_rtc_timer_interrupt_handler(_rtc_dev);
}

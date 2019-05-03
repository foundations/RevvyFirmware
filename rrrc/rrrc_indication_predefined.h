/*
 * rrrc_indication_predefined.h
 *
 * Created: 3/14/2019 4:12:30 PM
 *  Author: User
 */ 


#ifndef RRRC_INDICATION_PREDEFINED_H_
#define RRRC_INDICATION_PREDEFINED_H_

#include "rrrc_indication.h"
#include "components/RingLedDisplay/RingLedDisplay.h"

static led_ring_frame_t ring_leds_round_red[] =
{
    {//frame 0
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
    },
    {//frame 1
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
    },
    {//frame 2
        LED_RED,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
        LED_RED,
    },
        {//frame 3
        LED_RED,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_RED,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_RED,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_OFF,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_OFF,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
    },
    {//frame 0
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
        LED_RED,
    },
        {//frame 0
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
        {//frame 0
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
};

static led_ring_frame_t ring_leds_round_green[] =
{
    {//frame 0
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
    },
    {//frame 1
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
    },
    {//frame 2
        LED_GREEN,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 3
        LED_GREEN,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_OFF,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
    {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
        LED_GREEN,
    },
        {//frame 0
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
        {//frame 0
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
};

static led_ring_frame_t ring_leds_round_blue[] =
{
    {//frame 0
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
    },
    {//frame 1
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
    },
    {//frame 2
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 3
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 4
        LED_BLUE,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 5
        LED_BLUE,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 6
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 7
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 8
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 9
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 10
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 11
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_OFF,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 12
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 13
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
        LED_BLUE,
    },
    {//frame 14
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 15
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
};


static led_ring_frame_t ring_leds_running_fire_blue[] =
{
    {//frame 0
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
    },
    {//frame 1
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
    },
    {//frame 2
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
    },
    {//frame 3
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
    },
    {//frame 4
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
    },
    {//frame 5
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 6
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 7
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 8
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 9
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 10
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 11
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 12
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 13
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 14
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 15
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 15
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 15
        LED_OFF,
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 15
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 15
        LED_OFF,
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },

    {//frame 15
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
    {//frame 15
        LED_BLUE,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
        LED_OFF,
    },
};


#endif /* RRRC_INDICATION_PREDEFINED_H_ */
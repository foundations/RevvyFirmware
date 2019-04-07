/*
 * jscope.c
 *
 * Created: 4/5/2019 11:39:57 AM
 *  Author: Daniel Buga
 */ 

#include "jscope.h"
#include "third_party/segger/SEGGER_RTT.h"
#include "utils.h"
#include "utils_assert.h"
 
static uint8_t JS_RTT_UpBuffer[7200];    // J-Scope RTT Buffer
static uint32_t JS_RTT_Channel = 1;       // J-Scope RTT Channel

static int32_t jscope_sample[2 * 6];

void jscope_init(void)
{
    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_I4I4I4I4I4I4I4I4I4I4I4I4", &JS_RTT_UpBuffer[0], sizeof(JS_RTT_UpBuffer), SEGGER_RTT_MODE_NO_BLOCK_SKIP);
}

void jscope_update(uint32_t channel, int32_t v)
{
    ASSERT(channel < ARRAY_SIZE(jscope_sample));

    jscope_sample[channel] = v;
    SEGGER_RTT_Write(JS_RTT_Channel, &jscope_sample[0], sizeof(jscope_sample));
}

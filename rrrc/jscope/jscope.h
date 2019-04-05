/*
 * jscope.h
 *
 * Created: 4/5/2019 11:40:08 AM
 *  Author: Daniel Buga
 */ 


#ifndef JSCOPE_H_
#define JSCOPE_H_

#include <stdint.h>
#include <float.h>

void jscope_init(void);
void jscope_update(uint32_t channel, void* v);

#endif /* JSCOPE_H_ */
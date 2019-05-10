/*
 * MemoryAllocator.h
 *
 * Created: 10/05/2019 11:08:54
 *  Author: Dániel Buga
 */ 

#ifndef MEMORY_ALLOCATOR_H_
#define MEMORY_ALLOCATOR_H_

#include <stdio.h>

void* MemoryAllocator_Run_Allocate(size_t size);
void MemoryAllocator_Run_Free(void** ptr);

#endif /* MEMORY_ALLOCATOR_H_ */

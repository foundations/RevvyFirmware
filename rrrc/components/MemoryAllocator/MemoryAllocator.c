/*
 * MemoryAllocator.c
 *
 * Created: 10/05/2019 11:09:53
 *  Author: bugad
 */ 
#include "MemoryAllocator.h"
#include "FreeRTOS.h"

void* MemoryAllocator_Run_Allocate(size_t size)
{
    return pvPortMalloc(size);
}

void MemoryAllocator_Run_Free(void** ptr)
{
    vPortFree(*ptr);
    *ptr = NULL;
}

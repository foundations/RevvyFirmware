#include "MemoryAllocator.h"
#include "utils.h"

/* Begin User Code Section: Declarations */
#include "FreeRTOS.h"
/* End User Code Section: Declarations */

void* MemoryAllocator_Run_Allocate(size_t size)
{
    /* Begin User Code Section: Allocate Start */

    /* End User Code Section: Allocate Start */
    /* Begin User Code Section: Allocate End */
    return pvPortMalloc(size);
    /* End User Code Section: Allocate End */
}

void MemoryAllocator_Run_Free(void** ptr)
{
    /* Begin User Code Section: Free Start */
    vPortFree(*ptr);
    *ptr = NULL;
    /* End User Code Section: Free Start */
    /* Begin User Code Section: Free End */

    /* End User Code Section: Free End */
}

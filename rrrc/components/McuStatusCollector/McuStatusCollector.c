#include "McuStatusCollector.h"
#include "utils.h"
#include "utils_assert.h"

/* Begin User Code Section: Declarations */
#include <string.h>
#include <stdbool.h>

static uint32_t slots = 0u;
static uint8_t versions[14];
static uint8_t start_at_slot = 0u;

static bool _read_slot(uint8_t index, uint8_t* pData, uint8_t bufferSize, uint8_t* slotSize)
{
    static uint8_t buffer[64];

    __disable_irq();
    SlotData_t slot = McuStatusCollector_Read_SlotData(index);

    if (slot.version == versions[index] || ((slot.version & 0x80u) != 0u)) // if highest bit is 1, there is no data
    {
        // data did not change since last read
        __enable_irq();
        return true;
    }

    // copy bytes in critical section to avoid corruption - TODO do this after the size checks
    memcpy(buffer, slot.data.bytes, slot.data.count);
    __enable_irq();

    bool slot_fits = true;
    *slotSize = 0u;

    if (slot.data.count != 0u) /* < does this slot have any new data? */
    {
        if (2u + slot.data.count <= bufferSize) /* < enough space for slot data? */
        {
            pData[0u] = index;
            pData[1u] = slot.data.count;

            memcpy(&pData[2u], buffer, slot.data.count);

            *slotSize = 2u + slot.data.count;

            // store last successfully read version
            versions[index] = slot.version;
        }
        else
        {
            slot_fits = false;
        }
    }

    return slot_fits;
}

/* End User Code Section: Declarations */

void McuStatusCollector_Run_Reset(void)
{
    /* Begin User Code Section: Reset Start */
    slots = 0u;
    start_at_slot = 0u;
    memset(versions, 0xFFu, sizeof(versions));
    /* End User Code Section: Reset Start */
    /* Begin User Code Section: Reset End */

    /* End User Code Section: Reset End */
}

uint8_t McuStatusCollector_Run_Read(ByteArray_t destination)
{
    /* Begin User Code Section: Read Start */
    uint8_t idx = 0u;

    uint8_t start_at = start_at_slot;
    bool all_read = true;

    for (uint32_t i = start_at; i < 32u; i++)
    {
        if ((slots & (1u << i)) != 0u) /* < slot enabled? */
        {
            uint8_t slotSize = 0u;
            if (_read_slot(i, &destination.bytes[idx], destination.count - idx, &slotSize))
            {
                idx += slotSize;
            }
            else
            {
                start_at_slot = i;
                all_read = false;
                break;
            }
        }
    }

    if (all_read)
    {
        for (uint32_t i = 0u; i < start_at; i++)
        {
            if ((slots & (1u << i)) != 0u) /* < slot enabled? */
            {
                uint8_t slotSize = 0u;
                if (_read_slot(i, &destination.bytes[idx], destination.count - idx, &slotSize))
                {
                    idx += slotSize;
                }
                else
                {
                    start_at_slot = i;
                    break;
                }
            }
        }
    }

    /* End User Code Section: Read Start */
    /* Begin User Code Section: Read End */
    return idx;
    /* End User Code Section: Read End */
}

void McuStatusCollector_Run_EnableSlot(uint8_t slot)
{
    /* Begin User Code Section: EnableSlot Start */
    ASSERT(slot < 14u);

    slots |= (1u << slot);
    versions[slot] = 0xFFu;
    /* End User Code Section: EnableSlot Start */
    /* Begin User Code Section: EnableSlot End */

    /* End User Code Section: EnableSlot End */
}

void McuStatusCollector_Run_DisableSlot(uint8_t slot)
{
    /* Begin User Code Section: DisableSlot Start */
    ASSERT(slot < 14u);

    slots &= ~(1u << slot);
    /* End User Code Section: DisableSlot Start */
    /* Begin User Code Section: DisableSlot End */

    /* End User Code Section: DisableSlot End */
}

__attribute__((weak))
SlotData_t McuStatusCollector_Read_SlotData(uint32_t index)
{
    ASSERT(index < 14);
    /* Begin User Code Section: SlotData Start */

    /* End User Code Section: SlotData Start */
    /* Begin User Code Section: SlotData End */

    /* End User Code Section: SlotData End */
    return (SlotData_t) { .data = { .bytes = NULL, .count = 0u }, .version = 0xFFu };
}

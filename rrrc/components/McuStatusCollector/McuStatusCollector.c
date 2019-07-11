/*
 * McuStatusCollector.c
 *
 * Created: 2019. 07. 12. 11:28:23
 *  Author: Dániel Buga
 */

#include "McuStatusCollector.h"

#include <string.h>
#include <stdbool.h>
#include "utils_assert.h"

static uint32_t slots = 0u;
static uint8_t start_at_slot = 0u;

static bool _read_slot(uint8_t slot, uint8_t* pData, uint8_t bufferSize, uint8_t* slotSize)
{
    static uint8_t buffer[64];
    uint8_t size = 0u;
    McuStatusCollector_Read_SlotData(slot, buffer, sizeof(buffer), &size);

    bool slot_fits = true;
    *slotSize = 0u;

    if (size != 0u) /* < does this slot have any new data? */
    {
        if (size < bufferSize) /* < enough space for slot data? */
        {
            McuStatusCollector_Call_ClearSlotData(slot);

            pData[0] = slot;
            pData[1u] = size;

            memcpy(&pData[2u], buffer, size);

            *slotSize = 2u + size;
        }
        else
        {
            slot_fits = false;
        }
    }

    return slot_fits;
}

void McuStatusCollector_Run_ResetSlots(void)
{
    slots = 0u;
    start_at_slot = 0u;
}

void McuStatusCollector_Run_EnableSlot(uint8_t slot)
{
    ASSERT(slot < 32u);
    
    McuStatusCollector_Call_ClearSlotData(slot);
    slots |= (1u << slot);
}

void McuStatusCollector_Run_DisableSlot(uint8_t slot)
{
    ASSERT(slot < 32u);

    slots &= ~(1u << slot);
    McuStatusCollector_Call_ClearSlotData(slot);
}

void McuStatusCollector_Run_ReadData(uint8_t* pData, uint8_t bufferSize, uint8_t* dataSize)
{
    uint8_t idx = 0u;

    uint8_t start_at = start_at_slot;
    bool all_read = true;
    
    for (uint32_t i = start_at; i < 32u; i++)
    {
        if ((slots & (1u << i)) != 0u) /* < slot enabled? */
        {
            uint8_t slotSize = 0u;
            if (_read_slot(i, &pData[idx], bufferSize - idx, &slotSize))
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
                if (_read_slot(i, &pData[idx], bufferSize - idx, &slotSize))
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

    *dataSize = idx;
}

__attribute__((weak))
void McuStatusCollector_Read_SlotData(uint8_t slot, uint8_t* pData, uint8_t bufferSize, uint8_t* slotDataSize)
{
    *slotDataSize = 0u;
}

__attribute__((weak))
void McuStatusCollector_Call_ClearSlotData(uint8_t slot)
{

}

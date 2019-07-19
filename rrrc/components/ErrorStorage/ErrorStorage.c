/*
 * ErrorStorage.c
 *
 * Created: 2019. 07. 18. 9:19:06
 *  Author: Dániel Buga
 */ 

#include "ErrorStorage.h"
#include <string.h>

#include <peripheral_clk_config.h>
#include <hal_flash.h>

#define NVM_LAYOUT_VERSION   ((uint8_t) 0x00u)

#define BLOCK_SIZE          (8192u)
#define HEADER_OBJECT_SIZE  (32u)
#define DATA_OBJECT_SIZE    (32u)
#define OBJECTS_PER_BLOCK   ((BLOCK_SIZE - HEADER_OBJECT_SIZE) / DATA_OBJECT_SIZE)

typedef struct {
    uint8_t layout_version;
} FlashHeader_t;

typedef union {
    FlashHeader_t header;
    uint8_t raw[32];
} FlashHeaderObject_t;

typedef struct {
    struct {
        uint8_t reserved:5;
        uint8_t deleted:1;
        uint8_t valid:1;
        uint8_t allocated:1;
    } status;
    uint8_t data[31];
} FlashData_t;

/* this 32 bytes size is set in stone */
_Static_assert(sizeof(FlashHeaderObject_t) == HEADER_OBJECT_SIZE, "Incorrect flash header size");

/* this is not set in stone, depends on layout version */
_Static_assert(sizeof(FlashData_t) == DATA_OBJECT_SIZE, "Incorrect flash data object size");
_Static_assert(sizeof(ErrorInfo_t) <= DATA_OBJECT_SIZE - 1, "Incorrect error object size");

static struct flash_descriptor FLASH_0;
static BlockInfo_t* esBlocks;
static size_t esBlockCount;
static uint32_t esActiveBlock;

static inline const FlashHeader_t* _block_header(BlockInfo_t* block)
{
    return &((const FlashHeaderObject_t*) block->base_address)->header;
}

static inline bool _block_header_valid(const FlashHeader_t* header)
{
    return header->layout_version == NVM_LAYOUT_VERSION;
}

static inline FlashData_t* _get_object(const BlockInfo_t* block, uint8_t idx)
{
    uint32_t start_addr = block->base_address + HEADER_OBJECT_SIZE;
    FlashData_t* ptrs = (FlashData_t*) start_addr;
    return &ptrs[idx];
}

static inline FlashData_t _read_data_obj(BlockInfo_t* block, uint8_t idx)
{
    if (idx >= OBJECTS_PER_BLOCK)
    {
        while (1);
    }

    FlashData_t* ptr = _get_object(block, idx);
    return *ptr;
}

static void _delete_object(BlockInfo_t* block, uint8_t idx)
{
    if (idx >= OBJECTS_PER_BLOCK)
    {
        while (1);
    }

    FlashData_t* ptr = _get_object(block, idx);

    if (ptr->status.allocated == 0u && ptr->status.deleted == 1u)
    {
        /* set the deleted bit on a copy */
        FlashData_t data = *ptr;
        data.status.deleted = 0u;

        /* write the first 4 bytes back */
        flash_append(&FLASH_0, (uint32_t) ptr, (uint8_t*) &data, 4u);

        ++block->deleted;
    }
}

static void _write_block_header(BlockInfo_t* block, const FlashHeader_t* header)
{
    flash_append(&FLASH_0, block->base_address, (const uint8_t*) header, sizeof(FlashHeader_t));
}

static void _store_object(BlockInfo_t* block, const void* data, size_t size)
{
    if (block->allocated == OBJECTS_PER_BLOCK)
    {
        /* storage full */
        return;
    }

    if (block->allocated == 0u)
    {
        const FlashHeader_t* header = _block_header(block);
        if (header->layout_version == 0xFFu)
        {
            FlashHeader_t new_header = {};
            _write_block_header(block, &new_header);
        }
    }

    uint32_t idx = block->allocated;
    ++block->allocated;

    FlashData_t* ptr = _get_object(block, idx);

    /* create object with default data */
    FlashData_t object;
    memset(&object, 0xFFu, sizeof(object));
    
    /* allocate flash */
    object.status.allocated = 0u;
    flash_append(&FLASH_0, (uint32_t) ptr, (uint8_t*) &object, 4u);
    
    /* write data */
    memcpy(&object.data[0], data, size);
    flash_append(&FLASH_0, (uint32_t) ptr, (uint8_t*) &object, sizeof(object));
    
    /* finalize flash */
    object.status.valid = 0u;
    flash_append(&FLASH_0, (uint32_t) ptr, (uint8_t*) &object, 4u);
}

static void _update_number_of_stored_errors(void)
{
    uint32_t errors = 0u;
    for (size_t i = 0u; i < esBlockCount; i++)
    {
        errors += esBlocks[i].allocated - esBlocks[i].deleted;
    }
    ErrorStorage_Write_NumberOfStoredErrors(errors);
}

static void _erase_block(BlockInfo_t* block)
{
    flash_erase(&FLASH_0, block->base_address, NVMCTRL_BLOCK_SIZE / NVMCTRL_PAGE_SIZE);
    block->allocated = 0u;
    block->deleted = 0u;
}

static void _cleanup_invalid_and_full_blocks(void)
{
    for (size_t i = 0u; i < esBlockCount; i++)
    {
        esBlocks[i].allocated = 0u;
        esBlocks[i].deleted = 0u;
        const FlashHeader_t* header = _block_header(&esBlocks[i]);
        
        if (header->layout_version != 0xFFu)
        {
            /* if a block is not empty and has an invalid layout version, it shall be erased */
            if (!_block_header_valid(header))
            {
                _erase_block(&esBlocks[i]);
            }
            else
            {
                /* walk through objects in valid used blocks */
                for (size_t j = 0u; j < OBJECTS_PER_BLOCK; j++)
                {
                    FlashData_t obj = _read_data_obj(&esBlocks[i], j);
                    
                    /* track available (actually, allocated) space in each block */
                    if (obj.status.allocated == 0u)
                    {
                        esBlocks[i].allocated++;
                    
                        /* if object has dirty bit set, but valid bit is not set, mark as deleted */
                        if (obj.status.valid == 1u)
                        {
                            /* no need to actually delete the object here */
                            obj.status.deleted = 0u;
                        }
                    }

                    /* track objects mark for deletion */
                    if (obj.status.deleted == 0u)
                    {
                        if (obj.status.allocated == 1u)
                        {
                            /* this is an invalid block */
                            esBlocks[i].allocated++;
                        }
                        esBlocks[i].deleted++;
                    }
                }

                /* if all objects in the old block are marked as deleted, the block shall be erased */
                if (esBlocks[i].deleted == OBJECTS_PER_BLOCK)
                {
                    _erase_block(&esBlocks[i]);
                }
            }
        }
    }
}

static void _select_active_block(void)
{
    /* pick the block that has the most data but is not full - this will pick the currently used block or the first free one */
    uint32_t max_allocated = esBlocks[0].allocated;

    for (size_t i = 1u; i < esBlockCount; i++)
    {
        if (esBlocks[i].allocated > max_allocated)
        {
            if (esBlocks[i].allocated != OBJECTS_PER_BLOCK)
            {
                esActiveBlock = i;
                max_allocated = esBlocks[i].allocated;
            }
        }
    }
}

void ErrorStorage_Run_OnInit(BlockInfo_t* blocks, size_t num_blocks)
{
    hri_mclk_set_AHBMASK_NVMCTRL_bit(MCLK);
    flash_init(&FLASH_0, NVMCTRL);

    esBlocks = blocks;
    esBlockCount = num_blocks;

    _cleanup_invalid_and_full_blocks();
    _select_active_block();

    _update_number_of_stored_errors();
}

void ErrorStorage_Run_Clear(void)
{
    /* delete every allocated object */
    for (size_t i = 0u; i < esBlockCount; i++)
    {
        for (uint32_t j = 0u; j < esBlocks[i].allocated; j++)
        {
            _delete_object(&esBlocks[i], j);
        }
    }
    _update_number_of_stored_errors();
}

void ErrorStorage_Run_Store(const ErrorInfo_t* data)
{
    __disable_irq();
    if (esBlocks[esActiveBlock].allocated == OBJECTS_PER_BLOCK)
    {
        _cleanup_invalid_and_full_blocks();
        _select_active_block();
    }

    _store_object(&esBlocks[esActiveBlock], data, sizeof(ErrorInfo_t));
    _update_number_of_stored_errors();
    __enable_irq();
}

bool ErrorStorage_Run_Read(uint32_t index, ErrorInfo_t* pDst)
{
    uint32_t distance = index;
    bool found = false;
    for (size_t i = 0u; i < esBlockCount; i++)
    {
        uint32_t errors_in_block = esBlocks[i].allocated - esBlocks[i].deleted;
        if (errors_in_block <= distance)
        {
            distance -= errors_in_block;
            /* check next block */
        }
        else
        {
            /* assume linear deletion */
            found = true;
            FlashData_t data = _read_data_obj(&esBlocks[i], esBlocks[i].deleted + distance);

            if (data.status.valid == 0u && data.status.deleted == 1u)
            {
                memcpy(pDst, &data.data[0], sizeof(ErrorInfo_t));
            }
        }
    }

    return found;
}

__attribute__((weak))
void ErrorStorage_Write_NumberOfStoredErrors(uint32_t number)
{
    /* nothing to do */
}

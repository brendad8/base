
#ifndef BASE_VIRTUAL_MEMORY_H
#define BASE_VIRTUAL_MEMORY_H

#include "base_core.h"

typedef struct
{
    uint64_t page_size;
    uint64_t allocation_granularity;

} VirtualMemoryInfo;

static VirtualMemoryInfo vm_get_info(void);
static void*    vm_reserve(uint64_t size);
static bool32_t vm_commit(void* ptr, uint64_t size);
static bool32_t vm_decommit(void* ptr, uint64_t size);
static void     vm_release(void* ptr, uint64_t size);

#endif // BASE_VIRTUAL_MEMORY_H

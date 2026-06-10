
#ifndef BASE_VIRTUAL_MEMORY_H
#define BASE_VIRTUAL_MEMORY_H

#include "base_core.h"

typedef struct
{
    uint64 page_size;
    uint64 allocation_granularity;

} VirtualMemoryInfo;

static VirtualMemoryInfo VM_GetInfo(void);
static void*  VM_Reserve(uint64 size);
static bool32 VM_Commit(void* ptr, uint64 size);
static bool32 VM_Decommit(void* ptr, uint64 size);
static void   VM_Release(void* ptr, uint64 size);

#endif // BASE_VIRTUAL_MEMORY_H

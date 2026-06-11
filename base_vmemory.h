
#ifndef BASE_VIRTUAL_MEMORY_H
#define BASE_VIRTUAL_MEMORY_H

//***************************************************************************
//          INCLUDE_FILES
//***************************************************************************

#include "base_core.h"

//***************************************************************************
//          TYPES
//***************************************************************************

typedef struct
{
    uint64_t page_size;
    uint64_t allocation_granularity;

} VirtualMemoryInfo;

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

// TODO(bcall): make static and include .c or use public api

static VirtualMemoryInfo vm_get_info   (void);
static void*             vm_reserve    (uint64_t size);
static bool32            vm_commit     (void* ptr, uint64_t size);
static bool32            vm_decommit   (void* ptr, uint64_t size);
static void              vm_release    (void* ptr, uint64_t size);

#endif // BASE_VIRTUAL_MEMORY_H

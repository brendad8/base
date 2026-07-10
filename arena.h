
#ifndef _ARENA_H
#define _ARENA_H

//***************************************************************************
//          CONFIGURATION OPTIONS
//***************************************************************************

#ifndef ARENA_EXPORT
#define ARENA_EXPORT
#endif

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include <stddef.h> // for NULL
#include <stdint.h> // for uint8_t, uint64_t

//***************************************************************************
//          TYPES
//***************************************************************************

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
    uint8_t* backing_memory; // backing memory to use for arena instead of committing mem from os
    uint64_t commit_size;    // size of memory chunks when committing memory from os
    uint64_t reserve_size;   // size of vmemory address space when reserving memory from os or capacity of backing memory
};

typedef struct Arena Arena;
struct Arena
{
    uint8_t* base;          // arena base position
    uint64_t pos;           // arena alloc position
    uint64_t committed;     // total memory commited
    uint64_t reserved;      // total memory reserved
    uint64_t commit_size;   // min size in which more memory is commited
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp
{
    Arena* arena;   // underlying arena
    uint64_t pos;   // base position when created
};

//***************************************************************************
//          MACROS
//***************************************************************************

#define ARENA_PUSH_ARRAY(arena, type, count)         (type*)arena_push((arena), sizeof(type)*(count))
#define ARENA_PUSH_ARRAY_NO_ZERO(arena, type, count) (type*)arena_push_no_zero((arena), sizeof(type)*(count))

#define ARENA_PUSH_STRUCT(arena, type)               (type*)arena_push((arena), sizeof(type))
#define ARENA_PUSH_STRUCT_NO_ZERO(arena, type)       (type*)arena_push_no_zero((arena), sizeof(type))

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

ARENA_EXPORT Arena*    arena_alloc              (ArenaParams);
ARENA_EXPORT void      arena_release            (Arena* arena);

ARENA_EXPORT uint64_t  arena_position           (Arena* arena);
ARENA_EXPORT uint64_t  arena_commited           (Arena* arena);
ARENA_EXPORT uint64_t  arena_reserved           (Arena* arena);

ARENA_EXPORT void*     arena_push               (Arena* arena, uint64_t size);
ARENA_EXPORT void*     arena_push_no_zero       (Arena* arena, uint64_t size);

ARENA_EXPORT void*     arena_push_align         (Arena* arena, uint64_t size, uint64_t align);
ARENA_EXPORT void*     arena_push_align_no_zero (Arena* arena, uint64_t size, uint64_t align);

ARENA_EXPORT void      arena_pop_to             (Arena* arena, uint64_t pos);
ARENA_EXPORT void      arena_pop                (Arena* arena, uint64_t size);
ARENA_EXPORT void      arena_clear              (Arena* arena);

ARENA_EXPORT ArenaTemp arena_temp_begin         (Arena* arena);
ARENA_EXPORT void      arena_temp_end           (ArenaTemp temp);

#endif // _ARENA_H

//***************************************************************************
//          IMPLEMENTATION
//***************************************************************************

#ifdef ARENA_IMPLEMENTATION

static const uint64_t arena_default_alignment    = sizeof(void*);
static const uint64_t arena_default_commit_size  = (((uint64_t)64) << 10); // 64 KB
static const uint64_t arena_default_reserve_size = (((uint64_t)64) << 20); // 64 MB

#define __ARENA_ALIGN_UP_POW2(x,b) \
    (((x) + (b) - 1)&(~((b) - 1)))

#define __ARENA_CLAMP(min,val,max) \
    (((val)<(min))?(min):((val)>(max))?(max):(val))

typedef struct
{
    uint64_t page_size;
    uint64_t allocation_granularity;

} VirtualMemoryInfo;

static  VirtualMemoryInfo  vm_get_info   (void);
static  void*              vm_reserve    (uint64_t size);
static  bool               vm_commit     (void* ptr, uint64_t size);
static  bool               vm_decommit   (void* ptr, uint64_t size);
static  void               vm_release    (void* ptr, uint64_t size);

/*********************************************************************************/
ARENA_EXPORT Arena* arena_alloc(ArenaParams params)
{
    void* base;

    if (params.backing_memory)
    {
        if (params.reserve_size == 0)
            return NULL;

        base = params.backing_memory;
    }
    else
    {
        if (params.commit_size <= 0)
            params.commit_size = arena_default_commit_size;

        if (params.reserve_size <= 0)
            params.reserve_size = arena_default_reserve_size;

        VirtualMemoryInfo info = vm_get_info();

        params.commit_size  = __ARENA_ALIGN_UP_POW2(params.commit_size, info.page_size);
        params.reserve_size = __ARENA_ALIGN_UP_POW2(params.reserve_size, info.allocation_granularity);

        base = vm_reserve(params.reserve_size);
        if (!vm_commit(base, params.commit_size)) return NULL;
    }

    Arena* arena = (Arena*)base;
    arena->base = (uint8_t*)arena;
    arena->pos = sizeof(Arena);
    arena->reserved = params.reserve_size;

    if (params.backing_memory)
    {
        arena->committed = params.reserve_size;
        arena->commit_size = 0;
    }
    else
    {
        arena->committed = params.commit_size;
        arena->commit_size = params.commit_size;
    }

    return arena;
}

/*********************************************************************************/
ARENA_EXPORT void arena_release(Arena* arena)
{
    // NOTE(bcall): only release memory when not given a backing buffer
    // if given a backing buffer, user responsible for freeing
    // if (arena->commit_size != 0)
    vm_release(arena, arena->reserved);
}


/*********************************************************************************/
ARENA_EXPORT static void* arena_push_impl(Arena* arena, uint64_t size, uint64_t align, bool zero)
{
    uint64_t new_pos = __ARENA_ALIGN_UP_POW2(arena->pos, align);
    uint64_t new_pos_end = new_pos + size;

    if (new_pos_end > arena->committed)
    {
        // NOTE(bcall): backing buffer case where arena cannot grow
        if (arena->commit_size == 0)
            return NULL;
        
        // NOTE(bcall): commit enough memory to fit new allocation 
        // and be aligned with os commit size
        uint64_t commit_size = __ARENA_ALIGN_UP_POW2(new_pos_end, arena->commit_size) - arena->committed;

        if (arena->committed + commit_size <= arena->reserved)
        {
            // NOTE(bcall): commit new memory starting from end of 
            // initially commited region
            uint8_t* commit_end_ptr = arena->base + arena->committed;
            if (!vm_commit(commit_end_ptr, commit_size)) return NULL;
            arena->committed += commit_size;
        }
        else
        {
            return NULL;
        }
    }

    arena->pos = new_pos_end;
    void* result = arena->base + new_pos;
    if (zero) memset(result, 0, size);

    return result;
}

/*********************************************************************************/
ARENA_EXPORT void* arena_push(Arena* arena, uint64_t size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 1);
}

/*********************************************************************************/
ARENA_EXPORT void* arena_push_no_zero(Arena* arena, uint64_t size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 0);
}

/*********************************************************************************/
ARENA_EXPORT void* arena_push_align(Arena* arena, uint64_t size, uint64_t align)
{
    return arena_push_impl(arena, size, align, 1);
}

/*********************************************************************************/
ARENA_EXPORT void* arena_push_align_no_zero(Arena* arena, uint64_t size, uint64_t align)
{
    return arena_push_impl(arena, size, align, 0);
}

/*********************************************************************************/
ARENA_EXPORT void arena_pop_to(Arena* arena, uint64_t new_pos)
{
    uint64_t pos = arena->pos;
    new_pos = __ARENA_CLAMP(sizeof(Arena), new_pos, pos);
    arena->pos = new_pos;
}

/*********************************************************************************/
ARENA_EXPORT void arena_pop(Arena* arena, uint64_t size)
{
    uint64_t pos = arena->pos;
    uint64_t pos_new = (size < pos) ? pos - size : sizeof(Arena);
    arena_pop_to(arena, pos_new);
}

/*********************************************************************************/
ARENA_EXPORT void arena_clear(Arena* arena)
{
    arena_pop_to(arena, 0);
}

/*********************************************************************************/
ARENA_EXPORT ArenaTemp arena_temp_begin(Arena* arena)
{
    ArenaTemp temp = {arena, arena->pos};
    return temp;
}

/*********************************************************************************/
ARENA_EXPORT void arena_temp_end(ArenaTemp temp)
{
    arena_pop_to(temp.arena, temp.pos);
}

/*********************************************************************************/
#if defined(_WIN32)
#include <windows.h>

static VirtualMemoryInfo vm_get_info(void)
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    VirtualMemoryInfo info = {0};
    info.page_size = (uint64_t)sysinfo.dwPageSize;
    info.allocation_granularity = (uint64_t)sysinfo.dwAllocationGranularity;
    return info;
}

static void* vm_reserve(uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = __ARENA_ALIGN_UP_POW2(size, info.allocation_granularity);
    void* ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
    return ptr;
}

static bool vm_commit(void* ptr, uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = __ARENA_ALIGN_UP_POW2(size, info.page_size);

    void* result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);

    return (result != NULL);
}

bool vm_decommit(void* ptr, uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = __ARENA_ALIGN_UP_POW2(size, info.page_size);
 
    BOOL result = VirtualFree(ptr, size, MEM_DECOMMIT);

    return (result != 0);
}

void vm_release(void* ptr, uint64_t size)
{
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
}

#elif defined(__linux__)

#include <sys/mman.h>
#include <unistd.h>

VirtualMemoryInfo vm_get_info(void)
{
    uint64_t page_size = (uint64_t)sysconf(_SC_PAGESIZE);
    VirtualMemoryInfo info = {0};
    info.page_size = page_size;
    info.allocation_granularity = page_size;

    return info;
}

void* vm_reserve(uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = __ARENA_ALIGN_UP_POW2(size, info.allocation_granularity);

    void* ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED)
        return NULL;

    return ptr;
}

bool vm_commit(void* ptr, uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = __ARENA_ALIGN_UP_POW2(size, info.page_size);

    int result = mprotect(ptr, size, PROT_READ | PROT_WRITE);

    return (result == 0);
}

bool vm_decommit(void* ptr, uint64_t size)
{
    VirtualMemoryInfo info = vm_get_info();

    size = __ARENA_ALIGN_UP_POW2(size, info.page_size);

    // Return pages to OS. Memory remains mapped.
    int result = madvise(ptr, size, MADV_DONTNEED);

    // Make inaccessible again to emulate reserve state.
    mprotect(ptr, size, PROT_NONE);

    return (result == 0);
}

vm_release(void* ptr, uint64_t size)
{
    munmap(ptr, size);
}

#endif // virtual memory implementation

#endif // ARENA_IMPLEMENTATION


/* arena.h - virtual memory backed arena allocator

   To use this library, do this in *one* C file:
      #define ARENA_IMPLEMENTATION
      #include "base/arena.h"

*/

#ifndef ARENA_H
#define ARENA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define ARENA_HEADER_SIZE 128

typedef int ArenaFlags;
enum
{
    ARENA_NO_CHAIN    = 1 << 0;
}

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
    uint64_t commit_size;    // size of memory chunks when committing memory from os
    uint64_t reserve_size;   // size of vmemory address space when reserving memory from os or capacity of backing memory
    uint8_t* backing_memory; // backing memory to use for arena instead of committing mem from os
    ArenaFlags flags;
};

typedef struct Arena Arena;
struct Arena
{
    Arena* prev;
    Arena* current;
    uint64_t base_pos;
    uint64_t commit_size;
    uint64_t reserve_size;
    uint64_t pos;
    uint64_t committed;
    uint64_t reserved;
    uint64_t commit_size;
    ArenaFlags flags;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp
{
    Arena* arena;   // underlying arena
    uint64_t pos;   // position when created
};

Arena*    arena_alloc              (ArenaParams);
void      arena_release            (Arena* arena);

uint64_t  arena_position           (Arena* arena);
uint64_t  arena_commited           (Arena* arena);
uint64_t  arena_reserved           (Arena* arena);

void*     arena_push               (Arena* arena, uint64_t size);
void*     arena_push_no_zero       (Arena* arena, uint64_t size);

void*     arena_push_align         (Arena* arena, uint64_t size, uint64_t align);
void*     arena_push_align_no_zero (Arena* arena, uint64_t size, uint64_t align);

void      arena_pop_to             (Arena* arena, uint64_t pos);
void      arena_pop                (Arena* arena, uint64_t size);
void      arena_clear              (Arena* arena);

ArenaTemp arena_temp_begin         (Arena* arena);
void      arena_temp_end           (ArenaTemp temp);

#define ARENA_PUSH_ARRAY(arena, type, count)         (type*)arena_push((arena), sizeof(type)*(count))
#define ARENA_PUSH_ARRAY_NO_ZERO(arena, type, count) (type*)arena_push_no_zero((arena), sizeof(type)*(count))

#define ARENA_PUSH_STRUCT(arena, type)               (type*)arena_push((arena), sizeof(type))
#define ARENA_PUSH_STRUCT_NO_ZERO(arena, type)       (type*)arena_push_no_zero((arena), sizeof(type))

#ifdef __cplusplus
}
#endif

#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION

#include <stdbool.h>
#include <string.h>

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

Arena* arena_alloc(ArenaParams* params)
{
    Arena* arena = NULL;

    char* base = params.backing_memory;
    uint64_t commit_size = params->commit_size;
    uint64_t reserve_size = params->reserve_size;

    if (base == NULL)
    {
        // if (params.commit_size <= 0)  
        //     params.commit_size  = arena_default_commit_size;
        //
        // if (params.reserve_size <= 0) 
        //     params.reserve_size = arena_default_reserve_size;

        VirtualMemoryInfo info = vm_get_info();

        commit_size  = __ARENA_ALIGN_UP_POW2(commit_size, info.page_size);
        reserve_size = __ARENA_ALIGN_UP_POW2(reserve_size, info.page_size);

        base = vm_reserve(reserve_size);
        vm_commit(base, commit_size)) 
    }

    if (base != NULL)
    {
        arena = (Arena*)base;
        arena->current = arena;
        arena->prev = NULL;
        arena->commit_size = commit_size;
        arena->reserve_size = reserve_size;
        arena->base_pos = 0;
        arena->pos = ARENA_HEADER_SIZE;
        arena->committed = commit_size;
        arena->reserved = reserve_size;
        arena->flags = params->flags;
    }

    return arena;
}

void arena_release(Arena* arena)
{
    Arena* a;
    Arena* tmp = NULL;
    for (a = arena->current; a != NULL; a = tmp)
    {
        prev = a->prev;
        vm_release(n, n->reserved);
    }
}

static void* arena_push_impl(Arena* arena, uint64_t size, uint64_t align, bool zero)
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
            if (!vm_commit(commit_end_ptr, commit_size)) 
                return NULL;

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

void* arena_push(Arena* arena, uint64_t size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 1);
}

void* arena_push_no_zero(Arena* arena, uint64_t size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 0);
}

void* arena_push_align(Arena* arena, uint64_t size, uint64_t align)
{
    return arena_push_impl(arena, size, align, 1);
}

void* arena_push_align_no_zero(Arena* arena, uint64_t size, uint64_t align)
{
    return arena_push_impl(arena, size, align, 0);
}

void arena_pop_to(Arena* arena, uint64_t new_pos)
{
    uint64_t pos = arena->pos;
    new_pos = __ARENA_CLAMP(sizeof(Arena), new_pos, pos);
    arena->pos = new_pos;
}

void arena_pop(Arena* arena, uint64_t size)
{
    uint64_t pos = arena->pos;
    uint64_t pos_new = (size < pos) ? pos - size : sizeof(Arena);
    arena_pop_to(arena, pos_new);
}

void arena_clear(Arena* arena)
{
    arena_pop_to(arena, 0);
}

ArenaTemp arena_temp_begin(Arena* arena)
{
    ArenaTemp temp = {arena, arena->pos};
    return temp;
}

void arena_temp_end(ArenaTemp temp)
{
    arena_pop_to(temp.arena, temp.pos);
}

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
    int result = madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);

    return (result == 0);
}

void vm_release(void* ptr, uint64_t size)
{
    munmap(ptr, size);
}

#endif

#undef __ARENA_ALIGN_UP_POW2
#undef __ARENA_CLAMP

#endif // ARENA_IMPLEMENTATION

#ifdef ARENA_UNIT_TESTS

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static bool is_aligned(void* ptr, uint64_t align)
{
    return (((uintptr_t)ptr) & (align - 1)) == 0;
}

typedef struct
{
    int x;
    double y;

} TestStruct;

void arena_unit_tests(void)
{
    ArenaParams params = {0};
    Arena* arena = arena_alloc(params);
    assert(arena->committed == arena_default_commit_size);
    assert(arena->reserved == arena_default_reserve_size);
    uint64_t start = arena->pos;
    void* p1 = arena_push(arena, 64);
    assert(p1 != NULL);
    assert(arena->pos >= start + 64);
    arena_release(arena);


    ArenaParams params = { .commit_size = KB(4), .reserve_size = KB(64) };
    arena = arena_alloc(params);
    char* ptr = arena_push(arena, KB(5));
    assert(ptr != NULL);
    assert(arena->committed == KB(8));
    arena_release(arena);
    

    arena = arena_alloc((ArenaParams){0});
    uint64_t aligns[] = {1,2,4,8,16,32,64};
    for(uint64_t i = 0; i < ARRAY_COUNT(aligns); i++)
    {
        uint64_t align = aligns[i];
        void* ptr = arena_push_align(arena, 13, align);
        assert(is_aligned(ptr, align));
    }
    arena_release(arena);
  

    arena = arena_alloc((ArenaParams){0});
    uint8_t* mem = ARENA_PUSH_ARRAY(arena, uint8_t, 128);
    for(uint64_t i = 0; i < 128; i++)
        assert(mem[i] == 0);
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    uint64_t start = arena->pos;
    arena_push(arena, 128);
    arena_pop(arena, 128);
    assert(arena->pos == start);
    arena_release(arena);
    

    arena = arena_alloc((ArenaParams){0});
    arena_push(arena, 64);
    uint64_t mark = arena->pos;
    arena_push(arena, 256);
    arena_pop_to(arena, mark);
    assert(arena->pos == mark);
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    arena_push(arena, 1024);
    arena_clear(arena);
    assert(arena->pos == sizeof(Arena));
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    uint64_t start = arena->pos;
    ArenaTemp temp = arena_temp_begin(arena);
    arena_push(arena, 512);
    assert(arena->pos > start);
    arena_temp_end(temp);
    assert(arena->pos == start);
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    uint64_t start = arena->pos;
    ArenaTemp t1 = arena_temp_begin(arena);
    arena_push(arena, 64);
    uint64_t p1 = arena->pos;
    ArenaTemp t2 = arena_temp_begin(arena);
    arena_push(arena, 128);
    arena_temp_end(t2);
    assert(arena->pos == p1);
    arena_temp_end(t1);
    assert(arena->pos == start);
    arena_release(arena);


    ArenaParams params = {0};
    params.commit_size = KB(4);
    arena = arena_alloc(params);
    uint64_t initial_commit = arena->committed;
    assert(initial_commit = KB(4));
    arena_push(arena, KB(5));
    assert(arena->committed > initial_commit);
    arena_release(arena);


    ArenaParams params = {0};
    params.reserve_size = MB(1);
    arena = arena_alloc(params);
    void* ptr = arena_push(arena, MB(2));
    assert(ptr == 0);
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    TestStruct* s = ARENA_PUSH_STRUCT(arena, TestStruct);
    assert(s != 0);
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    for(uint64_t i = 0; i < 100; i++)
    {
        uint64_t size  = (i % 256) + 1;
        uint64_t align = 1ULL << (i % 6);
        void* ptr = arena_push_align(arena, size, align);
        assert(ptr != 0);
        assert(is_aligned(ptr, align));
    }
    arena_release(arena);


    arena = arena_alloc((ArenaParams){.commit_size = KB(64), .reserve_size = KB(64)});
    char* ptr = arena_push(arena, GB(1));
    assert(ptr == NULL);
    arena_release(arena);
}

#endif // ARENA_UNIT_TESTS

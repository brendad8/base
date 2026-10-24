
/* arena.h - virtual memory backed arena allocator

   To use this library, do this in *one* C file:
      #define ARENA_IMPLEMENTATION
      #include "base/arena.h"

   ACKNOWLEDGMENTS
   
      Adapted from base_arena.h 
      The RAD Debugger Project - Ryan Fleury
      https://github.com/EpicGames/raddebugger
*/

#ifndef ARENA_H
#define ARENA_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include "base.h"

/***************************************************************************
 *          TYPES
 ***************************************************************************/

typedef int ArenaFlags;
enum
{
    ARENA_NO_CHAIN    = 1 << 0
};

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
    uint64 commit_size;    // size of memory chunks when committing memory from os
    uint64 reserve_size;   // size of vmemory address space when reserving memory from os or capacity of backing memory
    uint8* backing_memory; // backing memory to use for arena instead of committing mem from os
    ArenaFlags flags;
};

typedef struct Arena Arena;
struct Arena
{
    Arena* prev;
    Arena* current;
    uint64 base_pos;
    uint64 commit_size;
    uint64 reserve_size;
    uint64 pos;
    uint64 committed;
    uint64 reserved;
    ArenaFlags flags;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp
{
    Arena* arena;   // underlying arena
    uint64 pos;   // position when created
};

/***************************************************************************
 *          PROTOTYPES
 ***************************************************************************/

Arena*    arena_alloc              (ArenaParams);
void      arena_release            (Arena* arena);

uint64    arena_position           (Arena* arena);
uint64    arena_commited           (Arena* arena);
uint64    arena_reserved           (Arena* arena);

void*     arena_push               (Arena* arena, uint64 size);
void*     arena_push_no_zero       (Arena* arena, uint64 size);

void*     arena_push_align         (Arena* arena, uint64 size, uint64 align);
void*     arena_push_align_no_zero (Arena* arena, uint64 size, uint64 align);

void      arena_pop_to             (Arena* arena, uint64 pos);
void      arena_pop                (Arena* arena, uint64 size);
void      arena_clear              (Arena* arena);

ArenaTemp arena_temp_begin         (Arena* arena);
void      arena_temp_end           (ArenaTemp temp);

#define arena_push_array(arena, type, count)         (type*)arena_push((arena), sizeof(type)*(count))
#define arena_push_array_no_zero(arena, type, count) (type*)arena_push_no_zero((arena), sizeof(type)*(count))

#define arena_push_struct(arena, type)               (type*)arena_push((arena), sizeof(type))
#define arena_push_struct_no_zero(arena, type)       (type*)arena_push_no_zero((arena), sizeof(type))

#ifdef __cplusplus
}
#endif

#endif // ARENA_H

/***************************************************************************
 *          IMPLEMENTATION
 ***************************************************************************/

#ifdef ARENA_IMPLEMENTATION

#define ARENA_HEADER_SIZE 128

#include <string.h>

static const uint64 arena_default_alignment = sizeof(void*);

#define ARENA_MAX(a,b) \
    (((a) > (b)) ? (a) : (b))

#define ARENA_ALIGN_UP_POW2(x,b) \
    (((x) + (b) - 1)&(~((b) - 1)))

#define ARENA_CLAMP(min,val,max) \
    (((val)<(min))?(min):((val)>(max))?(max):(val))

#define ARENA_STACK_PUSH_N(first, node, next) \
    ((node)->next = (first), (first) = (node))

typedef struct
{
    uint64 page_size;
    uint64 allocation_granularity;

} VirtualMemoryInfo;

static VirtualMemoryInfo  vm_get_info   (void);
static void*              vm_reserve    (uint64 size);
static bool               vm_commit     (void* ptr, uint64 size);
static bool               vm_decommit   (void* ptr, uint64 size);
static void               vm_release    (void* ptr, uint64 size);

Arena* arena_alloc(ArenaParams* params)
{
    Arena* arena = NULL;

    char* base = params.backing_memory;
    uint64 commit_size = params->commit_size;
    uint64 reserve_size = params->reserve_size;

    if (base == NULL)
    {
        // if (params.commit_size <= 0)  
        //     params.commit_size  = arena_default_commit_size;
        //
        // if (params.reserve_size <= 0) 
        //     params.reserve_size = arena_default_reserve_size;

        VirtualMemoryInfo info = vm_get_info();

        commit_size  = ARENA_ALIGN_UP_POW2(commit_size, info.page_size);
        reserve_size = ARENA_ALIGN_UP_POW2(reserve_size, info.page_size);

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

static void* arena_push_impl(Arena* arena, uint64 size, uint64 align, bool zero)
{
    uint64 size_to_zero = size;
    Arena* current = arena->current;

    uint64 new_pos = ARENA_ALIGN_UP_POW2(current->pos, align);
    uint64 new_pos_end = new_pos + size;

    if (new_pos_end > current->reserved && !(arena->flags & ARENA_NO_CHAIN))
    {
        Arena* new_arena = NULL;
        ArenaParams new_params = {0};

        new_params.commit_size = arena->commit_size;
        new_params.reserve_size = arena->reserve_size;

        if (size > new_params.reserve_size - ARENA_HEADER_SIZE)
        {
            new_params.reserve_size = ARENA_ALIGN_UP_POW2(size + ARENA_HEADER_SIZE, align);
            new_params.commit_size = reserve_size;
        }

        new_params->flags = arena->flags
        new_arena = arena_alloc(new_params);

        new_arena->base_pos = current->base_pos + current->reserved;
        ARENA_STACK_PUSH_N(arena->current, new_arena, prev);

        current = new_arena;
        pos_new = ARENA_ALIGN_UP_POW2(current->pos, align);
        pos_new_end = pos_new + size;
    }
    else if (arena->committed < new_pos_end)
    {
        uint64 commit_size = ARENA_ALIGN_UP_POW2(pos_new_end, current->commit_size) - current->committed;
        char* commit_start = (char *)current + current->committed;
        vm_commit(commit_start, commit_size);
        current->committed += commit_size;
        sizeo_zero = 0;
    }

    void* result = NULL;
    if(current->committed >= pos_new_end)
    {
        result = (char*)current + new_pos;
        current->pos = new_pos_end;

        if (zero)
            memset(result, 0, size_to_zero);
    }
  
  return result;
}

void* arena_push(Arena* arena, uint64 size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 1);
}

void* arena_push_no_zero(Arena* arena, uint64 size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 0);
}

void* arena_push_align(Arena* arena, uint64 size, uint64 align)
{
    return arena_push_impl(arena, size, align, 1);
}

void* arena_push_align_no_zero(Arena* arena, uint64 size, uint64 align)
{
    return arena_push_impl(arena, size, align, 0);
}

uint64 arena_position(Arena *arena)
{
  Arena* current = arena->current;
  return current->base_pos + current->pos;
}

void arena_pop_to(Arena *arena, uint64 pos)
{
    pos = ARENA_MAX(ARENA_HEADER_SIZE, pos);
    Arena* current = arena->current;

    for (Arena* prev = NULL; current->base_pos >= pos; current = prev)
    {
        prev = current->prev;
        vm_release(current, current->reserved);
    }

    arena->current = current;
    uint64 new_pos = pos - current->base_pos;
    assert(new_pos <= current->pos);
    current->pos = new_pos;
}

void arena_clear(Arena* arena)
{
    arena_pop_to(arena, 0);
}

void arena_pop(Arena *arena, uint64 amount)
{
    uint64 pos_old = arena_pos(arena);
    uint64 pos_new = pos_old;
    if (amount < pos_old)
    {
        pos_new = pos_old - amount;
    }
    arena_pop_to(arena, pos_new);
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
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif


static VirtualMemoryInfo vm_get_info(void)
{
    VirtualMemoryInfo info = {0};
#if defined(_WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    info.page_size = (uint64)sysinfo.dwPageSize;
    info.allocation_granularity = (uint64)sysinfo.dwAllocationGranularity;
#else
    uint64 page_size = (uint64)sysconf(_SC_PAGESIZE);
    info.page_size = page_size;
    info.allocation_granularity = page_size;
#endif
    return info;
}

static void* vm_reserve(uint64 size)
{
    void* result = NULL;
    VirtualMemoryInfo info = vm_get_info();
    size = ARENA_ALIGN_UP_POW2(size, info.allocation_granularity);
#if defined(_WIN32)
    ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
#else
    ptr = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
    return ptr;
}

static bool vm_commit(void* ptr, uint64 size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = ARENA_ALIGN_UP_POW2(size, info.page_size);
#if defined(_WIN32)
    void* result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
    return (result != NULL);
#else
    int result = mprotect(ptr, size, PROT_READ | PROT_WRITE);
    return (result == 0);
#endif
}

bool vm_decommit(void* ptr, uint64 size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = ARENA_ALIGN_UP_POW2(size, info.page_size);
#if defined(_WIN32)
    BOOL result = VirtualFree(ptr, size, MEM_DECOMMIT);
    return (result != 0);
#else
    int result = madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);
    return (result == 0);
#endif
}

void vm_release(void* ptr, uint64 size)
{
#if defined(_WIN32)
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

#endif

#undef ARENA_MAX
#undef ARENA_STACK_PUSH_N
#undef ARENA_ALIGN_UP_POW2
#undef ARENA_CLAMP

#endif // ARENA_IMPLEMENTATION

#ifdef ARENA_UNIT_TESTS

#include <assert.h>
#include <stdlib.h>

static bool is_aligned(void* ptr, uint64 align)
{
    return (((uintptr)ptr) & (align - 1)) == 0;
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
    uint64 start = arena->pos;
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
    uint64 aligns[] = {1,2,4,8,16,32,64};
    for(uint64 i = 0; i < sizeof(aligns)/sizeof(aligns[0]); i++)
    {
        uint64 align = aligns[i];
        void* ptr = arena_push_align(arena, 13, align);
        assert(is_aligned(ptr, align));
    }
    arena_release(arena);
  

    arena = arena_alloc((ArenaParams){0});
    uint8* mem = ARENA_PUSH_ARRAY(arena, uint8, 128);
    for(uint64 i = 0; i < 128; i++)
        assert(mem[i] == 0);
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    uint64 start = arena->pos;
    arena_push(arena, 128);
    arena_pop(arena, 128);
    assert(arena->pos == start);
    arena_release(arena);
    

    arena = arena_alloc((ArenaParams){0});
    arena_push(arena, 64);
    uint64 mark = arena->pos;
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
    uint64 start = arena->pos;
    ArenaTemp temp = arena_temp_begin(arena);
    arena_push(arena, 512);
    assert(arena->pos > start);
    arena_temp_end(temp);
    assert(arena->pos == start);
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    uint64 start = arena->pos;
    ArenaTemp t1 = arena_temp_begin(arena);
    arena_push(arena, 64);
    uint64 p1 = arena->pos;
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
    uint64 initial_commit = arena->committed;
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
    TestStruct* s = arena_push_struct(arena, TestStruct);
    assert(s != 0);
    arena_release(arena);


    arena = arena_alloc((ArenaParams){0});
    for(uint64 i = 0; i < 100; i++)
    {
        uint64 size  = (i % 256) + 1;
        uint64 align = 1ULL << (i % 6);
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

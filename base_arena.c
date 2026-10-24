
#if OS_WINDOWS
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif

#include "base_arena.h"
#include "base_linklist.h"

#define ARENA_HEADER_SIZE 128

const u64 arena_default_alignment = sizeof(void*);
const u64 arena_default_commit_size = KB(4);
const u64 arena_default_reserve_size = KB(64);


typedef struct
{
    u64 page_size;
    u64 allocation_granularity;

} VirtualMemoryInfo;

internal VirtualMemoryInfo
vm_get_info(void)
{
    VirtualMemoryInfo info = {0};
#if OS_WINDOWS
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    info.page_size = (u64)sysinfo.dwPageSize;
    info.allocation_granularity = (u64)sysinfo.dwAllocationGranularity;
#else
    u64 page_size = (u64)sysconf(_SC_PAGESIZE);
    info.page_size = page_size;
    info.allocation_granularity = page_size;
#endif
    return info;
}

internal void* 
vm_reserve(u64 size)
{
    void* result = NULL;
    VirtualMemoryInfo info = vm_get_info();
    size = mem_align_up_pow2(size, info.allocation_granularity);
#if OS_WINDOWS
    result = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
#else
    result = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
    return result;
}

internal b32
vm_commit(void* ptr, u64 size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = mem_align_up_pow2(size, info.page_size);
#if OS_WINDOWS
    void* result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
    return (result != NULL);
#else
    int result = mprotect(ptr, size, PROT_READ | PROT_WRITE);
    return (result == 0);
#endif
}

internal b32
vm_decommit(void* ptr, u64 size)
{
    VirtualMemoryInfo info = vm_get_info();
    size = mem_align_up_pow2(size, info.page_size);
#if OS_WINDOWS
    BOOL result = VirtualFree(ptr, size, MEM_DECOMMIT);
    return (result != 0);
#else
    int result = madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);
    return (result == 0);
#endif
}

internal void
vm_release(void* ptr, u64 size)
{
#if OS_WINDOWS
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    munmap(ptr, size);
#endif
}

Arena* arena_alloc(ArenaParams params)
{
    Arena* arena = NULL;

    u64 commit_size = params.commit_size;
    u64 reserve_size = params.reserve_size;

    VirtualMemoryInfo info = vm_get_info();

    if (commit_size == 0) commit_size = arena_default_commit_size;
    if (reserve_size == 0) reserve_size = arena_default_commit_size;

    commit_size  = mem_align_up_pow2(commit_size, info.page_size);
    reserve_size = mem_align_up_pow2(reserve_size, info.page_size);

    void* base = vm_reserve(reserve_size);
    vm_commit(base, commit_size);

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
        arena->flags = params.flags;
    }

    return arena;
}

void arena_release(Arena* arena)
{
    Arena* a;
    Arena* tmp = NULL;
    for (a = arena->current; a != NULL; a = tmp)
    {
        tmp = a->prev;
        vm_release(a, a->reserved);
    }
}

static void* arena_push_impl(Arena* arena, u64 size, u64 align, bool zero)
{
    u64 size_to_zero = size;
    Arena* current = arena->current;

    u64 pos_new = mem_align_up_pow2(current->pos, align);
    u64 pos_new_end = pos_new + size;

    if (pos_new_end > current->reserved && !(arena->flags & ArenaFlag_NoChain))
    {
        Arena* new_arena = NULL;
        ArenaParams new_params = {0};

        new_params.commit_size = arena->commit_size;
        new_params.reserve_size = arena->reserve_size;

        if (size > new_params.reserve_size - ARENA_HEADER_SIZE)
        {
            new_params.reserve_size = mem_align_up_pow2(size + ARENA_HEADER_SIZE, align);
            new_params.commit_size = new_params.reserve_size;
        }

        new_params.flags = arena->flags;
        new_arena = arena_alloc(new_params);

        new_arena->base_pos = current->base_pos + current->reserved;
        stack_push_n(arena->current, new_arena, prev);

        current = new_arena;
        pos_new = mem_align_up_pow2(current->pos, align);
        pos_new_end = pos_new + size;
    }
    else if (arena->committed < pos_new_end)
    {
        u64 commit_size = mem_align_up_pow2(pos_new_end, current->commit_size) - current->committed;
        char* commit_start = (char *)current + current->committed;
        vm_commit(commit_start, commit_size);
        current->committed += commit_size;
        size_to_zero = 0;
    }

    void* result = NULL;
    if(current->committed >= pos_new_end)
    {
        result = (char*)current + pos_new;
        current->pos = pos_new_end;

        if (zero) mem_zero(result, size_to_zero);
    }
  
  return result;
}

void* arena_push(Arena* arena, u64 size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 1);
}

void* arena_push_no_zero(Arena* arena, u64 size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 0);
}

void* arena_push_align(Arena* arena, u64 size, u64 align)
{
    return arena_push_impl(arena, size, align, 1);
}

void* arena_push_align_no_zero(Arena* arena, u64 size, u64 align)
{
    return arena_push_impl(arena, size, align, 0);
}

u64 arena_position(Arena *arena)
{
  Arena* current = arena->current;
  return current->base_pos + current->pos;
}

void arena_pop_to(Arena *arena, u64 pos)
{
    pos = max(ARENA_HEADER_SIZE, pos);
    Arena* current = arena->current;

    for (Arena* prev = NULL; current->base_pos >= pos; current = prev)
    {
        prev = current->prev;
        vm_release(current, current->reserved);
    }

    arena->current = current;
    u64 new_pos = pos - current->base_pos;
    assert(new_pos <= current->pos);
    current->pos = new_pos;
}

void arena_clear(Arena* arena)
{
    arena_pop_to(arena, 0);
}

void arena_pop(Arena *arena, u64 amount)
{
    u64 pos_old = arena_position(arena);
    u64 pos_new = pos_old;
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


internal void* 
arena_alloc_alloc(void* ctx, u64 size, u64 alignment) 
{
    Arena* arena = (Arena*)ctx;
    return arena_push_align(arena, size, alignment);
}

internal void* 
arena_alloc_realloc(void* ctx, void* ptr, u64 old_size, u64 new_size, u64 alignment) 
{
    return arena_alloc_alloc(ptr, new_size, alignment);
}

internal void 
arena_alloc_free(void* ctx, void* ptr, u64 size) 
{
    (void)ctx; (void)ptr; (void)size;
}

internal const AllocatorVTable arena_alloc_vtable = { arena_alloc_alloc, arena_alloc_realloc, arena_alloc_free};

Allocator 
arena_make_allocator(Arena* arena)
{
    const Allocator arena_allocator = { &arena_alloc_vtable, arena };
    return arena_allocator;
}


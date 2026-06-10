
#include "base_core.h"
#include "base_arena.h"
#include "base_vmemory.c"

/*********************************************************************************/
Arena* arena_alloc(ArenaParams params)
{
    VirtualMemoryInfo info = VM_GetInfo();

    if (params.commit_size <= 0)
        params.commit_size = arena_default_commit_size;

    if (params.reserve_size <= 0)
        params.reserve_size = arena_default_reserve_size;

    params.commit_size  = ALIGN_UP_POW2(params.commit_size, info.page_size);
    params.reserve_size = ALIGN_UP_POW2(params.reserve_size, info.allocation_granularity);

    void* base = VM_Reserve(params.reserve_size);
    VM_Commit(base, params.commit_size);

    Arena* next = NULL;
    Arena* arena = (Arena*)base;
    arena->base = (uint8*)arena;
    arena->pos = sizeof(Arena);
    arena->reserved = params.reserve_size;
    arena->committed = params.commit_size;
    arena->params = params;

    return arena;
}

/*********************************************************************************/
void arena_release(Arena* arena)
{
    while (arena)
    {
        Arena* next = arena->next;
        VM_Release(arena, arena->reserved);
        arena = next;
    }
}

/*********************************************************************************/
// static uint64 ArenaGetPos(Arena* arena)
// {
//     // needed if we make getting pos more complex with chaining...
//     return arena->pos;
// }

/*********************************************************************************/
static void* arena_push_impl(Arena* arena, uint64 size, uint64 align, bool32 zero)
{
    uint64 new_pos = ALIGN_UP_POW2(arena->pos, align);
    uint64 new_pos_end = new_pos + size;

    if (new_pos_end > arena->committed)
    {
        // commit enough memory to fit new allocation and be aligned with os commit size
        uint64 commit_size = ALIGN_UP_POW2(new_pos_end, arena->params.commit_size) - arena->committed;
        
        if (arena->committed + commit_size > arena->reserved)
        {
            // TODO(bcall): if free_list commit remaining memmory and add to free list...
            // if (arena->params.growable)
            // {
            //     if (size < arena->params.reserve_size)
            //     {
            //         ArenaParams next_params = arena->params;
            //         uint64 next_pos_end = ALIGN_UP_POW2(sizeof(Arena), sizeof(void*)) + size;
            //         uint64 next_commit_size = ALIGN_UP_POW2(new_pos_end, arena->params.commit_size) - arena->committed;
            //         next_params.commit_size = Max(arena->params.commit_size, next_commit_size);
            //         Arena* next = ArenaAlloc();
            //     }
            //     else
            //     {
            //
            //     }
            // }
            // else
            // {
            //     return NULL;
            // }
            return NULL;
        }

        // commit new memory starting from end of initially commited region
        uint8* commit_end_ptr = arena->base + arena->committed;
        VM_Commit(commit_end_ptr, commit_size); // WARN(bcall): VM_Commit failure not handled. 
        arena->committed += commit_size;
    }
    
    arena->pos = new_pos_end;
    void* result = arena->base + new_pos;
    if (zero) memset(result, 0, size);

    return result;
}
/*********************************************************************************/
void* arena_push(Arena* arena, uint64 size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 1);
}

/*********************************************************************************/
void* arena_push_no_zero(Arena* arena, uint64 size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 0);
}

/*********************************************************************************/
void* arena_push_align(Arena* arena, uint64 size, uint64 align)
{
    return arena_push_impl(arena, size, align, 1);
}

/*********************************************************************************/
void* arena_push_align_no_zero(Arena* arena, uint64 size, uint64 align)
{
    return arena_push_impl(arena, size, align, 0);
}

/*********************************************************************************/
void arena_pop_to(Arena* arena, uint64 new_pos)
{
    uint64 pos = arena->pos;
    ASSERT_ALWAYS(new_pos <= pos);
    arena->pos = new_pos;
}

/*********************************************************************************/
void arena_pop(Arena* arena, uint64 size)
{
    uint64 pos = arena->pos;
    uint64 pos_new = (size < pos) ? pos - size : pos;
    arena_pop_to(arena, pos_new);
}

/*********************************************************************************/
void arena_clear(Arena* arena)
{
    arena_pop_to(arena, 0);
}

/*********************************************************************************/
ArenaTemp arena_temp_begin(Arena* arena)
{
    ArenaTemp temp = {arena, arena->pos};
    return temp;
}

/*********************************************************************************/
void arena_temp_end(ArenaTemp temp)
{
    arena_pop_to(temp.arena, temp.pos);
}




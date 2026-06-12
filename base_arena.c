
// #include <stdio.h>

#include "base_core.h"
#include "base_arena.h"
#include "base_vmemory.c"

/*********************************************************************************/
Arena* arena_alloc(ArenaParams params)
{
    VirtualMemoryInfo info = vm_get_info();

    if (params.commit_size <= 0)
        params.commit_size = arena_default_commit_size;

    if (params.reserve_size <= 0)
        params.reserve_size = arena_default_reserve_size;

    params.commit_size  = ALIGN_UP_POW2(params.commit_size, info.page_size);
    params.reserve_size = ALIGN_UP_POW2(params.reserve_size, info.allocation_granularity);

    void* base = vm_reserve(params.reserve_size);
    vm_commit(base, params.commit_size);

    Arena* arena = (Arena*)base;
    arena->base = (uint8_t*)arena;
    arena->pos = sizeof(Arena);
    arena->reserved = params.reserve_size;
    arena->committed = params.commit_size;
    arena->params = params;
    arena->next = NULL;

    return arena;
}

/*********************************************************************************/
void arena_release(Arena* arena)
{
    while (arena)
    {
        Arena* next = arena->next;
        vm_release(arena, arena->reserved);
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
static void* arena_push_impl(Arena* arena, uint64_t size, uint64_t align, bool32 zero)
{
    while (arena->next != NULL)
        arena = arena->next;

    uint64_t new_pos = ALIGN_UP_POW2(arena->pos, align);
    uint64_t new_pos_end = new_pos + size;

    if (new_pos_end > arena->committed)
    {
        // commit enough memory to fit new allocation and be aligned with os commit size
        uint64_t commit_size = ALIGN_UP_POW2(new_pos_end, arena->params.commit_size) - arena->committed;

        // TODO(bcall): check free list..
        
        if (arena->committed + commit_size > arena->reserved)
        {

            // TODO(bcall): commit remaining memmory
            // TODO(bcall): if free_list add to free list
            
            if (arena->params.growable)
            {
                ArenaParams new_arena_params = arena->params;
                new_pos = ALIGN_UP_POW2(sizeof(Arena), align);
                new_pos_end = new_pos + size;

                // NOTE(bcall): adjust next arena params to make sure allocation fits in single arena
                if (new_pos_end > new_arena_params.commit_size)
                {
                    new_arena_params.commit_size = ALIGN_UP_POW2(new_pos_end, arena->params.commit_size);
                    if (new_pos_end > new_arena_params.reserve_size)
                    {
                        new_arena_params.reserve_size = ALIGN_UP_POW2(new_pos_end, arena->params.reserve_size);
                    }
                }
                // printf("commit_size = %llu, reserve_size = %llu\n", arena->params.commit_size, arena->params.reserve_size);
                // printf("commit_size = %llu, reserve_size = %llu\n", new_arena_params.commit_size, new_arena_params.reserve_size);
                
                Arena* new_arena = arena_alloc(new_arena_params);
                arena->next = new_arena;

                new_arena->pos = new_pos_end;
                void* result = new_arena->base + new_pos;
                if (zero) memset(result, 0, size);

                return result;
            }
            else
            {
                // WARN(bcall): out of memory
                return NULL;
            }
        }
        else
        {
            // commit new memory starting from end of initially commited region
            uint8_t* commit_end_ptr = arena->base + arena->committed;
            vm_commit(commit_end_ptr, commit_size); // WARN(bcall): vm_commit failure not handled. 
            arena->committed += commit_size;
        }
    }
    
    arena->pos = new_pos_end;
    void* result = arena->base + new_pos;
    if (zero) memset(result, 0, size);

    return result;
}
/*********************************************************************************/
void* arena_push(Arena* arena, uint64_t size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 1);
}

/*********************************************************************************/
void* arena_push_no_zero(Arena* arena, uint64_t size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 0);
}

/*********************************************************************************/
void* arena_push_align(Arena* arena, uint64_t size, uint64_t align)
{
    return arena_push_impl(arena, size, align, 1);
}

/*********************************************************************************/
void* arena_push_align_no_zero(Arena* arena, uint64_t size, uint64_t align)
{
    return arena_push_impl(arena, size, align, 0);
}

/*********************************************************************************/
void arena_pop_to(Arena* arena, uint64_t new_pos)
{
    uint64_t pos = arena->pos;
    ASSERT_ALWAYS(new_pos <= pos);
    arena->pos = new_pos;
}

/*********************************************************************************/
void arena_pop(Arena* arena, uint64_t size)
{
    uint64_t pos = arena->pos;
    uint64_t pos_new = (size < pos) ? pos - size : pos;
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




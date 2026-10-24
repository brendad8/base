
#include "base_ctx.h"

thread_local ThreadContext* thread_ctx;

void
ctx_alloc(void)
{
    ArenaParams params = {0};
    Arena* arena_0 = arena_alloc(params);
    Arena* arena_1 = arena_alloc(params);

    thread_ctx = arena_push_struct(arena_0, ThreadContext);
    thread_ctx->arenas[0] = arena_0;
    thread_ctx->arenas[1] = arena_1;
}

void
ctx_release(ThreadContext *thread_ctx)
{
  arena_release(thread_ctx->arenas[1]);
  arena_release(thread_ctx->arenas[0]);
}

Arena*
ctx_get_scratch(Arena **conflicts, u64 count)
{
    Arena *result = 0;
    Arena **arena_ptr = thread_ctx->arenas;
    for(u64 i = 0; i < 2; i += 1, arena_ptr += 1)
    {
        Arena **conflict_ptr = conflicts;
        b32 has_conflict = 0;
        for (u64 j = 0; j < count; j += 1, conflict_ptr += 1)
        {
            if (*arena_ptr == *conflict_ptr)
            {
                has_conflict = 1;
                break;
            }
        }

        if (!has_conflict)
        {
            result = *arena_ptr;
            break;
        }
    }
    return result;
}

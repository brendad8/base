
#pragma once

#include "base_core.h"
#include "base_arena.h"

typedef struct
{
    Arena* arenas[2];

} ThreadContext;

extern thread_local ThreadContext* thread_ctx;

void ctx_alloc(void);
void ctx_release(ThreadContext* ctx);

Arena* ctx_get_scratch(Arena **conflicts, u64 count);

#define ctx_scratch_begin(conflicts, count) arena_temp_begin(ctx_get_scratch((conflicts), (count)))
#define ctx_scratch_end(scratch) arena_temp_end(scratch)

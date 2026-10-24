
#pragma once

#include "base_core.h"
#include "base_arena.h"

typedef struct
{
    Arena* arenas[2];

} ThreadContext;

extern thread_local ThreadContext* thread_ctx;

void tctx_alloc(void);
void tctx_release(ThreadContext* ctx);

Arena* tctx_get_scratch(Arena **conflicts, u64 count);

#define tctx_scratch_begin(conflicts, count) arena_temp_begin(tctx_get_scratch((conflicts), (count)))
#define tctx_scratch_end(scratch) arena_temp_end(scratch)

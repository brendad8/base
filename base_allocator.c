
#include "base_allocator.h"

internal void* 
c_alloc_alloc(void* ctx, u64 size, u64 alignment) 
{
    (void)ctx; (void)alignment;
    return malloc(size);
}

internal void* 
c_alloc_realloc(void* ctx, void* ptr, u64 old_size, u64 new_size, u64 alignment) 
{
    (void)ctx; (void)old_size; (void)alignment;
    return realloc(ptr, new_size);
}

internal void 
c_alloc_free(void* ctx, void* ptr, u64 size) 
{
    (void)ctx; (void)size;
    free(ptr);
}

internal const AllocatorVTable c_alloc_vtable = { c_alloc_alloc, c_alloc_realloc, c_alloc_free};
const Allocator c_allocator = { &c_alloc_vtable, .ctx = NULL };


// TODO(bcall):
// 1. create debug allocator that tracks allocations
// 2. create pool allocator that allocates fixed size blocks, maintains free list 


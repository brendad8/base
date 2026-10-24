
#pragma once

#include "base_core.h"

#define mem_alloc_array(a, type, count)                                                     \
    (type*)mem_alloc(a, sizeof(type) * (count), alignof(type))

#define mem_alloc_struct(a, type)                                                           \
    (type*)mem_alloc(a, sizeof(type), alignof(type))

typedef struct 
{
    void* (*alloc)(void* ctx, u64 size, u64 alignment);
    void* (*realloc)(void* ctx, void* ptr, u64 old_size, u64 new_size, u64 alignment);
    void  (*free)(void* ctx, void* ptr, u64 size);

} AllocatorVTable;

typedef struct 
{
    const AllocatorVTable* vtable;
    void* ctx;

} Allocator;

internal inline void* 
mem_alloc(Allocator a, u64 size, u64 alignment) 
{
    return a.vtable->alloc(a.ctx, size, alignment);
}

internal inline void* 
mem_realloc(Allocator a, void* ptr, u64 old_size, u64 new_size, u64 alignment) 
{
    return a.vtable->realloc(a.ctx, ptr, old_size, new_size, alignment);
}

internal inline void 
mem_free(Allocator a, void* ptr, u64 size) 
{
    if (ptr) a.vtable->free(a.ctx, ptr, size);
}

extern const Allocator c_allocator;

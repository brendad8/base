
#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#include "base_core.h"

static const uint64 arena_default_alignment = sizeof(void*);
static const uint64 arena_default_reserve_size = MB(64);
static const uint64 arena_default_commit_size  = KB(64);

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
    uint64 commit_size;     // size of memory chunks when committing memory from os
    uint64 reserve_size;    // size of vmemory address space when reserving memory from os
    
    // bool32 growable;        // whether arena will grow when all reserved space is used
    // bool32 free_list;       // whether arena can free allocations in middle of arena
};

typedef struct Arena Arena;
struct Arena
{
    Arena* next;           // next arena if growable
    uint8* base;           // arena base position
    uint64 pos;            // arena alloc position
    uint64 committed;      // total memory committed
    uint64 reserved;       // total memory reserved
    ArenaParams params;    // configurable options for arena
};

Arena* arena_alloc(ArenaParams);
void   arena_release(Arena* arena);

void* arena_push(Arena* arena, uint64 size);
void* arena_push_no_zero(Arena* arena, uint64 size);

void* arena_push_align(Arena* arena, uint64 size, uint64 align);
void* arena_push_align_no_zero(Arena* arena, uint64 size, uint64 align);

void arena_pop_to(Arena* arena, uint64 pos);
void arena_pop(Arena* arena, uint64 size);
void arena_clear(Arena* arena);

#define arena_push_array(arena, type, count)         (type*)arena_push((arena), sizeof(type)*(count))
#define arena_push_array_no_zero(arena, type, count) (type*)arena_push_no_zero((arena), sizeof(type)*(count))

#define arena_push_struct(arena, type)         (type*)arena_push((arena), sizeof(type))
#define arena_push_struct_no_zero(arena, type) (type*)arena_push_no_zero((arena), sizeof(type))


typedef struct 
{
    Arena* arena;
    uint64 pos;

} ArenaTemp;

ArenaTemp arena_temp_begin(Arena* arena);
void arena_temp_end(ArenaTemp temp);

#endif // BASE_ARENA_H

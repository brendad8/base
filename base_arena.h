
/* arena.h - virtual memory backed arena allocator

   To use this library, do this in *one* C file:
      #define ARENA_IMPLEMENTATION
      #include "base/arena.h"

   ACKNOWLEDGMENTS
   
      Adapted from base_arena.h 
      The RAD Debugger Project - Ryan Fleury
      https://github.com/EpicGames/raddebugger
*/

#pragma once

#include "base_core.h"
#include "base_allocator.h"

typedef int ArenaFlags;
enum
{
    ArenaFlag_NoChain    = 1 << 0
};

typedef struct
{
    u64 commit_size;    // size of memory chunks when committing memory from os
    u64 reserve_size;   // size of vmemory address space when reserving memory from os or capacity of backing memory
    ArenaFlags flags;

} ArenaParams;

typedef struct Arena Arena;
struct Arena
{
    Arena* prev;
    Arena* current;
    u64 base_pos;
    u64 commit_size;
    u64 reserve_size;
    u64 pos;
    u64 committed;
    u64 reserved;
    ArenaFlags flags;
};

typedef struct ArenaTemp ArenaTemp;
struct ArenaTemp
{
    Arena* arena;   // underlying arena
    u64 pos;        // position when created
};


Arena*    arena_alloc              (ArenaParams);
void      arena_release            (Arena* arena);

u64       arena_position           (Arena* arena);
u64       arena_commited           (Arena* arena);
u64       arena_reserved           (Arena* arena);

void*     arena_push               (Arena* arena, u64 size);
void*     arena_push_no_zero       (Arena* arena, u64 size);

void*     arena_push_align         (Arena* arena, u64 size, u64 align);
void*     arena_push_align_no_zero (Arena* arena, u64 size, u64 align);

void      arena_pop_to             (Arena* arena, u64 pos);
void      arena_pop                (Arena* arena, u64 size);
void      arena_clear              (Arena* arena);

ArenaTemp arena_temp_begin         (Arena* arena);
void      arena_temp_end           (ArenaTemp temp);

#define arena_push_array(arena, type, count)         (type*)arena_push((arena), sizeof(type)*(count))
#define arena_push_array_no_zero(arena, type, count) (type*)arena_push_no_zero((arena), sizeof(type)*(count))

#define arena_push_struct(arena, type)               (type*)arena_push((arena), sizeof(type))
#define arena_push_struct_no_zero(arena, type)       (type*)arena_push_no_zero((arena), sizeof(type))

Allocator arena_make_allocator(Arena* arena);


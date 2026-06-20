
#ifndef BASE_ARENA_H
#define BASE_ARENA_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include "base_core.h"

//***************************************************************************
//          GLOBAL CONSTANTS
//***************************************************************************

static const uint64_t arena_default_alignment    = sizeof(void*);
static const uint64_t arena_default_reserve_size = MB(64);
static const uint64_t arena_default_commit_size  = KB(64);

//***************************************************************************
//          TYPES
//***************************************************************************

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
    uint64_t commit_size;   // size of memory chunks when committing memory from os
    uint64_t reserve_size;  // size of vmemory address space when reserving memory from os
   
    bool     growable;      // whether arena will grow when all reserved space is used
    // bool   free_list;    // whether arena can free allocations in middle of arena
};

typedef struct Arena Arena;
struct Arena
{
    uint8_t* base;          // arena base position
    uint64_t pos;           // arena alloc position
    uint64_t committed;     // total memory committed
    uint64_t reserved;      // total memory reserved
    ArenaParams params;     // configurable options for arena
    
    Arena* next;            // next arena if growable
};

typedef struct 
{
    Arena* arena;   // underlying arena
    uint64_t pos;   // base position when created

} ArenaTemp;

//***************************************************************************
//          MACROS
//***************************************************************************

#define ARENA_PUSH_ARRAY(arena, type, count)         (type*)arena_push((arena), sizeof(type)*(count))
#define ARENA_PUSH_ARRAY_NO_ZERO(arena, type, count) (type*)arena_push_no_zero((arena), sizeof(type)*(count))

#define ARENA_PUSH_STRUCT(arena, type)         (type*)arena_push((arena), sizeof(type))
#define ARENA_PUSH_STRUCT_NO_ZERO(arena, type) (type*)arena_push_no_zero((arena), sizeof(type))

//***************************************************************************
//          Function Prototypes
//***************************************************************************

Arena*    arena_alloc              (ArenaParams);
void      arena_release            (Arena* arena);

// TODO(bcall): figure whether to do accumulated value or just last
uint64_t  arena_position           (Arena* arena);
uint64_t  arena_commited           (Arena* arena);
uint64_t  arena_reserved           (Arena* arena);

void*     arena_push               (Arena* arena, uint64_t size);
void*     arena_push_no_zero       (Arena* arena, uint64_t size);

void*     arena_push_align         (Arena* arena, uint64_t size, uint64_t align);
void*     arena_push_align_no_zero (Arena* arena, uint64_t size, uint64_t align);

void      arena_pop_to             (Arena* arena, uint64_t pos);
void      arena_pop                (Arena* arena, uint64_t size);
void      arena_clear              (Arena* arena);

ArenaTemp arena_temp_begin         (Arena* arena);
void      arena_temp_end           (ArenaTemp temp);


#endif // BASE_ARENA_H

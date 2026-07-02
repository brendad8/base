

#ifndef ARENA_H
#define ARENA_H

#ifndef ARENA_EXPORT
#define ARENA_EXPORT
#endif

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include <stddef.h>
#include <stdint.h>

//***************************************************************************
//          TYPES
//***************************************************************************

typedef struct
{
    uint64_t pos;      // arena alloc position
    uint64_t cap;      // arena alloc position
    uint8_t* base;     // arena base position
                       //
} Arena;

typedef struct 
{
    Arena* arena;   // underlying arena
    uint64_t pos;   // base position when created

} ArenaTemp;

//***************************************************************************
//          MACROS
//***************************************************************************

#define KB(n)  (((uint64_t)(n)) << 10)
#define MB(n)  (((uint64_t)(n)) << 20)
#define GB(n)  (((uint64_t)(n)) << 30)
#define TB(n)  (((uint64_t)(n)) << 40)

#define ARENA_PUSH_ARRAY(arena, type, count)         (type*)arena_push((arena), sizeof(type)*(count))
#define ARENA_PUSH_ARRAY_NO_ZERO(arena, type, count) (type*)arena_push_no_zero((arena), sizeof(type)*(count))

#define ARENA_PUSH_STRUCT(arena, type)         (type*)arena_push((arena), sizeof(type))
#define ARENA_PUSH_STRUCT_NO_ZERO(arena, type) (type*)arena_push_no_zero((arena), sizeof(type))

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

ARENA_EXPORT  Arena*    arena_alloc              (uint64_t capacity);
ARENA_EXPORT  void      arena_release            (Arena* arena);

ARENA_EXPORT  void*     arena_push               (Arena* arena, uint64_t size);
ARENA_EXPORT  void*     arena_push_no_zero       (Arena* arena, uint64_t size);

ARENA_EXPORT  void*     arena_push_align         (Arena* arena, uint64_t size, uint64_t align);
ARENA_EXPORT  void*     arena_push_align_no_zero (Arena* arena, uint64_t size, uint64_t align);

ARENA_EXPORT  void      arena_pop_to             (Arena* arena, uint64_t pos);
ARENA_EXPORT  void      arena_pop                (Arena* arena, uint64_t size);
ARENA_EXPORT  void      arena_clear              (Arena* arena);

ARENA_EXPORT  ArenaTemp arena_temp_begin         (Arena* arena);
ARENA_EXPORT  void      arena_temp_end           (ArenaTemp temp);

//***************************************************************************
//          FUNCTION IMPLEMENTATIONS
//***************************************************************************

#ifdef ARENA_IMPLEMENTATION
#define ARENA_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

#define ARENA_ALIGN_UP_POW2(x,b) \
    (((x) + (b) - 1)&(~((b) - 1)))

#define ARENA_CLAMP(min,val,max) \
    (((val)<(min))?(min):((val)>(max))?(max):(val))

static const uint64_t arena_default_alignment = sizeof(void*);

/*********************************************************************************/
ARENA_EXPORT Arena* arena_alloc(uint64_t capacity)
{
    void* mem = malloc(sizeof(Arena) + capacity);
    if (mem == NULL) return NULL;

    Arena* arena = (Arena*)mem;
    arena->base = (uint8_t*)arena + sizeof(Arena);
    arena->pos = 0;
    arena->cap = capacity;

    return arena;
}

/*********************************************************************************/
ARENA_EXPORT void arena_release(Arena* arena)
{
    if (arena)
        free(arena);
}

/*********************************************************************************/
static void* arena_push_impl(Arena* arena, uint64_t size, uint64_t align, bool zero)
{
    uint64_t new_pos = ARENA_ALIGN_UP_POW2(arena->pos, align);
    uint64_t new_pos_end = new_pos + size;

    if (new_pos_end > arena->cap)
        return NULL;
    
    arena->pos = new_pos_end;
    void* result = arena->base + new_pos;
    if (zero) memset(result, 0, size);

    return result;
}

/*********************************************************************************/
ARENA_EXPORT void* arena_push(Arena* arena, uint64_t size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 1);
}

/*********************************************************************************/
ARENA_EXPORT void* arena_push_no_zero(Arena* arena, uint64_t size)
{
    return arena_push_impl(arena, size, arena_default_alignment, 0);
}

/*********************************************************************************/
ARENA_EXPORT void* arena_push_align(Arena* arena, uint64_t size, uint64_t align)
{
    return arena_push_impl(arena, size, align, 1);
}

/*********************************************************************************/
ARENA_EXPORT void* arena_push_align_no_zero(Arena* arena, uint64_t size, uint64_t align)
{
    return arena_push_impl(arena, size, align, 0);
}

/*********************************************************************************/
ARENA_EXPORT void arena_pop_to(Arena* arena, uint64_t new_pos)
{
    uint64_t pos = arena->pos;
    new_pos = ARENA_CLAMP(0, new_pos, pos);
    arena->pos = new_pos;
}

/*********************************************************************************/
ARENA_EXPORT void arena_pop(Arena* arena, uint64_t size)
{
    uint64_t pos = arena->pos;
    uint64_t new_pos = pos - size;
    arena_pop_to(arena, new_pos);
}

/*********************************************************************************/
ARENA_EXPORT void arena_clear(Arena* arena)
{
    arena_pop_to(arena, 0);
}

/*********************************************************************************/
ARENA_EXPORT ArenaTemp arena_temp_begin(Arena* arena)
{
    ArenaTemp temp = {arena, arena->pos};
    return temp;
}

/*********************************************************************************/
ARENA_EXPORT void arena_temp_end(ArenaTemp temp)
{
    arena_pop_to(temp.arena, temp.pos);
}

// cleanup
#undef ARENA_ALIGN_UP_POW2
#undef ARENA_CLAMP

#endif // ARENA_IMPLEMENTATION

#endif // ARENA_H

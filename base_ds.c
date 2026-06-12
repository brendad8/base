
#include <stdlib.h>

#include "base_ds.h"
#include "base_arena.h"

/*********************************************************************************/
void* array_grow_arena(Arena* arena, void* items, uint64_t item_size, uint64_t count)
{
    void* new_ptr;
    uint64_t new_len;
    uint64_t new_cap;

    uint64_t len = ARRAY_LEN(items);
    uint64_t cap = ARRAY_CAP(items);

    new_len = len + count;
    if (new_len < cap)
        return items;

    if (new_len < 2 * cap)
        new_cap = 2 * cap;
    else if (new_len < 4)
        new_cap = 4; // minimum capacity of 4
    else // NOTE(bcall): if new_len > 2 * capacity, then set cap to 1.5*new_len
        new_cap = (uint64_t)(3 * new_len / 2);

    if (items == NULL)
    {
        void* ptr = arena_push(arena, sizeof(ArrayHeader) + new_cap * item_size);
        if (ptr)
        {
            ArrayHeader* hdr = (ArrayHeader*)ptr;
            hdr->cap = new_cap;
            hdr->len = 0;
            return (void*)(hdr + 1);
        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items; 
        }
    }
    // NOTE(bcall): Array has room to grow in arena without relocating it...
    else if ((uint64_t)(arena->base + arena->pos) == (uint64_t)items + cap*item_size)
    {
        // NOTE(bcall): since array is not moving we allocate space for diff in new vs old capacity
        void* ptr = arena_push(arena, (new_cap - cap) * item_size);
        if (ptr)
        {
            ARRAY_HEADER_CAST(items)->cap = new_cap;
            return items;
        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items; 
        }
    }
    else
    {
        // NOTE(bcall): push room for entire new cap and move to new location...
        void* ptr = arena_push(arena, sizeof(ArrayHeader) + new_cap * item_size);
        if (ptr)
        {
            ArrayHeader* hdr = (ArrayHeader*)ptr;
            hdr->cap = new_cap;

            ptr = (void*)(hdr + 1);
            memmove(ptr, items, len * item_size);
            
            // TODO(bcall): free prev items pointer if arena has free list...
            return ptr;

        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items;
        }
    }
}


void* array_grow_heap(void* items, uint64_t item_size, uint64_t count)
{
    void* new_ptr;
    uint64_t new_len;
    uint64_t new_cap;

    uint64_t len = ARRAY_LEN(items);
    uint64_t cap = ARRAY_CAP(items);

    new_len = len + count;
    if (new_len < cap)
        return items;

    if (new_len < 2 * cap)
        new_cap = 2 * cap;
    else if (new_len < 4)
        new_cap = 4;
    else
        new_cap = (uint64_t)(3 * new_len / 2);

    if (items == NULL)
    {
        void* ptr = malloc(sizeof(ArrayHeader) + new_cap * item_size);
        if (ptr)
        {
            ArrayHeader* hdr = (ArrayHeader*)ptr;
            hdr->cap = new_cap;
            hdr->len = 0;
            return (void*)(hdr + 1);
        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items; 
        }
    }
    else
    {
        void* ptr = realloc(ARRAY_HEADER_CAST(items), sizeof(ArrayHeader) + new_cap * item_size);
        if (ptr)
        {
            ArrayHeader* hdr = (ArrayHeader*)ptr;
            hdr->cap = new_cap;
            hdr->len = len;

            ptr = (void*)(hdr + 1);
            memmove(ptr, items, len * item_size);
            
            return ptr;

        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items;
        }
    }
}



// #include <stdio.h>

#include "base_ds.h"
#include "base_arena.h"

/*********************************************************************************/
void* array_grow(Arena* arena, ArrayHeader* header, 
                void* items, uint64_t item_size, uint64_t count)
{
    void* new_ptr;
    uint64_t new_len;
    uint64_t new_capacity;

    new_len = header->len + count;
    if (new_len < header->capacity)
        return items;

    if (new_len < 2 * header->capacity)
        new_capacity = 2 * header->capacity;
    else if (new_len < 4)
        new_capacity = 4; // minimum capacity of 4
    else // NOTE(bcall): if new_len > 2 * capacity, then set cap to 1.5*new_len
        new_capacity = (uint64_t)(3 * new_len / 2);

    if (items == NULL)
    {
        void* ptr = arena_push(arena, new_capacity * item_size);
        if (ptr)
        {
            header->capacity = new_capacity;
            return ptr;
        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items; 
        }
    }
    // NOTE(bcall): Array has room to grow in arena without relocating it...
    else if ((uint64_t)(arena->base + arena->pos) == (uint64_t)items + header->capacity*item_size)
    {
        // NOTE(bcall): since array is not moving we allocate space for diff in new vs old capacity
        void* ptr = arena_push(arena, (new_capacity - header->capacity) * item_size);
        if (ptr)
        {
            header->capacity = new_capacity;
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
        void* ptr = arena_push(arena, new_capacity * item_size);
        if (ptr)
        {
            header->capacity = new_capacity;
            memmove(ptr, items, header->len * item_size);
            
            // TODO(bcall): free prev items pointer if arena has free list...
            return ptr;

        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items;
        }
    }
    return items;
}

/*********************************************************************************/
void array_shift_down(ArrayHeader* header, void* items, uint64_t item_size, uint64_t from_idx)
{
    uint8_t* src = (uint8_t*)items + from_idx * item_size;
    uint8_t* dst = src - item_size;
    uint64_t size = (header->len - from_idx) * item_size;
    memmove(dst, src, size);
}

/*********************************************************************************/
void array_shift_up(ArrayHeader* header, void* items, uint64_t item_size, uint64_t from_idx)
{
    uint8_t* src = (uint8_t*)items + from_idx * item_size;
    uint8_t* dst = src + item_size;
    uint64_t size = (header->len - from_idx) * item_size;
    memmove(dst, src, size);
}


/*********************************************************************************/

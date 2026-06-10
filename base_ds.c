
// #include <stdio.h>

#include "base_ds.h"
#include "base_arena.h"

/*********************************************************************************/
void* arr_grow(Arena* arena, ArrayHeader* header, 
                void* items, uint64 item_size, uint64 count)
{
    void* new_ptr;
    uint64 new_len;
    uint64 new_capacity;

    new_len = header->len + count;
    if (new_len < header->capacity)
        return items;

    if (new_len < 2 * header->capacity)
        new_capacity = 2 * header->capacity;
    else if (new_len < 4)
        new_capacity = 4; // minimum capacity of 4
    else // NOTE(bcall): if new_len > 2 * capacity, then set cap to 1.5*new_len
        new_capacity = (uint64)(3 * new_len / 2);

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
    else if ((uint64)(arena->base + arena->pos) == (uint64)items + header->capacity*item_size)
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
void arr_shift_down(ArrayHeader* header, void* items, uint64 item_size, uint64 from_idx)
{
    uint8* src = (uint8*)items + from_idx * item_size;
    uint8* dst = src - item_size;
    uint64 size = (header->len - from_idx) * item_size;
    memmove(dst, src, size);
}

/*********************************************************************************/
void arr_shift_up(ArrayHeader* header, void* items, uint64 item_size, uint64 from_idx)
{
    uint8* src = (uint8*)items + from_idx * item_size;
    uint8* dst = src + item_size;
    uint64 size = (header->len - from_idx) * item_size;
    memmove(dst, src, size);
}


/*********************************************************************************/

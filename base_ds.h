
#include "base_core.h"
#include "base_arena.h"

//************************
// Stack
//************************

//  struct Node
//  {
//       // Node Data Here
//       Node* next
//  };
//
//  // Example Stack
//  Node* first;

#define stack_pushn(first, node, next) \
    ((node)->next = (first), (first) = (node))

#define stack_popn(first, next) \
    ((first) = (first)->next)

#define stack_push(first, node) stack_pushn(first, node, next)

#define stack_pop(first) stack_popn(first, next)

//************************
// Queue
//************************

//  struct Node
//  {
//       // Node Data Here
//       Node* next
//  };
// 
//  // Example Queue
//  Node* first;
//  Node* last;

#define queue_push_n(first, last, node, next)                       \
    ((first) == NULL ?                                              \
    ((first) = (last) = (node), (node)->next = NULL) :              \
    ((last)->next = (node), (last) = (node), (node)->next = NULL))

#define queue_pop_n(first, last, next)                              \
    ((first) == (last) ?                                            \
    (first) = (last) = NULL :                                       \
    ((first) = (first)->next))

#define queue_push(first, last, node) queue_push_n(first, last, node, next)
#define queue_pop(first, last) queue_pop_n(first, last, next) 


//************************
// Doubly-Linked List
//************************

// struct Node
// {
//     // Node Data Here
//     Node* next;
//     Node* prev;
// }
//
// Node* first = {0};
// Node* last  = {0};

#define dll_push_back_np(first, last, node, next, prev)                                   \
    ((first) == NULL ?                                                                    \
    ((first) = (last) = (node), (node)->next = (node)->prev = NULL) :                     \
    ((last)->next = (node), (node)->prev = (last), (last) = (node), (node)->next = NULL))

#define dll_push_front_np(first, last, node, next, prev) \
    dll_push_back_np(last, first, node, prev, next)

#define dll_insert_after_np(first, last, ref_node, node, next, prev)   \
    (((last) == (ref_node)) ?                                          \
    dll_push_back_np(first, last, node, next, prev) :                  \
    ((node)->prev = (ref_node), (node)->next = (ref_node)->next, (ref_node)->next->prev = (node), (ref_node)->next = (node)))

#define dll_insert_before_np(first, last, ref_node, node, next, prev) \
    dll_insert_after_np(last, first, ref_node, node, prev, next)

#define dll_remove_first_np(first, last, next, prev)    \
    (((first) == (last)) ?                              \
    (first) = (last) = NULL :                           \
    ((first) = (first)->next, (first)->prev = NULL))

#define dll_remove_last_np(first, last, next, prev) \
    dll_remove_first_np(last, first, prev, next)

#define dll_remove_np(first, last, node, next, prev)                           \
    (((first) == (node)) ?                                                     \
    dll_remove_first_np(first, last, next, prev) :                             \
    ((last) == (node)) ?                                                       \
    dll_remove_last_np(first, last, next, prev) :                              \
    ((node)->next->prev = (node)->prev, (node)->prev->next = (node)->next))

#define dll_push_back(first, last, node)                dll_push_back_np(first, last, node, next, prev)
#define dll_push_front(first, last, node)               dll_push_front_np(first, last, node, next, prev)
#define dll_insert_after(first, last, ref_node, node)   dll_insert_after_np(first, last, ref_node, node, next, prev)
#define dll_insert_before(first, last, ref_node, node)  dll_insert_before_np(first, last, ref_node, node, next, prev)
#define dll_remove(first, last, node)                   dll_remove_np(first, last, node, next, prev)
#define dll_remove_first(first, last)                   dll_remove_first_np(first, last, next, prev)
#define dll_remove_last(first, last)                    dll_remove_last_np(first, last, next, prev)


//************************
// Dynamic Array
//************************

//  struct FloatArray 
//  {
//      ArrayHeader header;
//      float* items;
//  };
// 
//  FloatArray floats = {0};
//  arr_reserve(arena, floats, 128);   // reserve space for 128 items
//  arr_push(arena, floats, 3.1415f);  // push to back
//  ...

typedef struct ArrayHeader ArrayHeader;
struct ArrayHeader
{
    uint64 len;
    uint64 capacity;
};

void* arr_grow(Arena* arena, ArrayHeader* header, void* items, uint64 item_size, uint64 count);
void  arr_shift_down(ArrayHeader* header, void* items, uint64 item_size, uint64 from_idx);
void  arr_shift_up(ArrayHeader* header, void* items, uint64 item_size, uint64 from_idx);


#define arr_header_cast(a) (&(a).header)
#define arr_item_size(a) (sizeof(*(a).items))
#define arr_len(a) (a.header.len)
#define arr_cap(a) (a.header.capacity)

#define arr_push(arena, a, item)                                                                      \
    (*((void**)&(a).items) = arr_grow((arena), arr_header_cast(a), (a).items, arr_item_size(a), 1),   \
    (a).items[(a).header.len++] = (item))

#define arr_add(arena, a, n)                                                                          \
    (*((void**)&(a).items) = arr_grow((arena), arr_header_cast(a), (a).items, arr_item_size(a), (n)), \
    (a).header.len += (n),                                                                            \
    &(a).items[(a).header.len - (n)])

#define arr_reserve(arena, a, n) \
    (*((void**)&(a).items) = arr_grow((arena), arr_header_cast(a), (a).items, arr_item_size(a), (n)))

#define arr_clear(a) ((a).header.len = 0)
#define arr_clear_zero(a) (memset((a).items, 0, arr_item_size(a)*arr_len(a)), (a).header.len = 0)

#define arr_insert(arena, a, i, item)                                                 \
    do {                                                                              \
        if ((uint64)(i) <= (a).header.len)                                            \
        {                                                                             \
            *((void**)&(a).items) = arr_grow((arena), arr_header_cast(a), (a).items, arr_item_size(a), 1); \
            arr_shift_up(arr_header_cast(a), (a).items, arr_item_size(a), (i));       \
            (a).items[(i)] = (item);                                                  \
            (a).header.len++;                                                         \
        }                                                                             \
    } while (0)


#define arr_remove(a, i)                                                              \
    do {                                                                              \
        if ((uint64)(i) < (a).header.len)                                             \
        {                                                                             \
            arr_shift_down(arr_header_cast(a), (a).items, arr_item_size(a), (i) + 1); \
            (a).header.len--;                                                         \
        }                                                                             \
    } while (0)


#define arr_remove_swap(a, i)                                  \
    do {                                                       \
        if ((uint64)(i) < (a).header.len)                      \
            (a).items[(i)] = (a).items[--(a).header.len];      \
    } while (0)


// #define arr_sort(a, cmp_func) (qsort((a).items, arr_len(a), arr_item_size(a), cmp_func))


#define stbds_arrmaybegrow(a,n)  ((!(a) || stbds_header(a)->length + (n) > stbds_header(a)->capacity) \
                                  ? (stbds_arrgrow(a,n,0),0) : 0)

#define stbds_arrgrow(a,b,c)   ((a) = stbds_arrgrowf_wrapper((a), sizeof *(a), (b), (c)))


//************************
// Hash Map
//************************

typedef struct HashMapEntry HashMapEntry;
struct HashMapEntry 
{
    uint64 hash;
    uint64 idx;
};

typedef struct HashMapHeader HashMapHeader;
struct HashMapHeader
{
    uint64 len;
    uint64 capacity;
    uint64 insertIndex;
    uint64 usedCountThreshold;
    uint64 deletedCount;
    uint64 deletedCountThreshold;
    HashMapEntry* table;
};

// #define hmap_reserve(arena, m, n)
// #define hmap_default(arena, m, val)
// #define hmap_defaults(arena, m, s)
// #define hmap_clear(m)
// #define hmap_clone(m)

// #define hmap_put(arena, m, k, val)
// #define hmap_index(arena, m, k)
// #define hmap_get(m, type, k)
// #define hmap_try_get(m, k, out_val)
// #define hmap_gets(m, type, k)
// #define hmap_get_ptr(m, k)
// #define hmap_get_ptr_null(m, k)
// #define hmap_del(scratch, m, k)


//************************
// String Hash Map
//************************

// #define smap_reserve(arena, m, n)
// #define smap_default(arena, m, val)
// #define smap_defaults(arena, m, s)
// #define smap_clear(m)
// #define smap_clone(m)

// #define smap_put(arena, m, k, val)
// #define smap_index(arena, m, k)
// #define smap_get(m, type, k)
// #define smap_try_get(m, k, out_val)
// #define smap_gets(m, type, k)
// #define smap_get_ptr(m, k)
// #define smap_get_ptr_null(m, k)
// #define smap_del(scratch, m, k)


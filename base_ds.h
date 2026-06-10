
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

#define STACK_PUSH_N(first, node, next) \
    ((node)->next = (first), (first) = (node))

#define STACK_POP_N(first, next) \
    ((first) = (first)->next)

#define STACK_PUSH(first, node) STACK_PUSH_N(first, node, next)

#define STACK_POP(first) STACK_POP_N(first, next)

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

#define QUEUE_PUSH_N(first, last, node, next)                       \
    ((first) == NULL ?                                              \
    ((first) = (last) = (node), (node)->next = NULL) :              \
    ((last)->next = (node), (last) = (node), (node)->next = NULL))

#define QUEUE_POP_N(first, last, next)                              \
    ((first) == (last) ?                                            \
    (first) = (last) = NULL :                                       \
    ((first) = (first)->next))

#define QUEUE_PUSH(first, last, node) QUEUE_PUSH_N(first, last, node, next)
#define QUEUE_POP(first, last) QUEUE_POP_N(first, last, next) 


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

#define DLL_PUSH_BACK_NP(first, last, node, next, prev)                                   \
    ((first) == NULL ?                                                                    \
    ((first) = (last) = (node), (node)->next = (node)->prev = NULL) :                     \
    ((last)->next = (node), (node)->prev = (last), (last) = (node), (node)->next = NULL))

#define DLL_PUSH_FRONT_NP(first, last, node, next, prev) \
    DLL_PUSH_BACK_NP(last, first, node, prev, next)

#define DLL_INSERT_AFTER_NP(first, last, ref_node, node, next, prev)   \
    (((last) == (ref_node)) ?                                          \
    DLL_PUSH_BACK_NP(first, last, node, next, prev) :                  \
    ((node)->prev = (ref_node), (node)->next = (ref_node)->next, (ref_node)->next->prev = (node), (ref_node)->next = (node)))

#define DLL_INSERT_BEFORE_NP(first, last, ref_node, node, next, prev) \
    DLL_INSERT_AFTER_NP(last, first, ref_node, node, prev, next)

#define DLL_REMOVE_FIRST_NP(first, last, next, prev)    \
    (((first) == (last)) ?                              \
    (first) = (last) = NULL :                           \
    ((first) = (first)->next, (first)->prev = NULL))

#define DLL_REMOVE_LAST_NP(first, last, next, prev) \
    DLL_REMOVE_FIRST_NP(last, first, prev, next)

#define DLL_REMOVE_NP(first, last, node, next, prev)                           \
    (((first) == (node)) ?                                                     \
    DLL_REMOVE_FIRST_NP(first, last, next, prev) :                             \
    ((last) == (node)) ?                                                       \
    DLL_REMOVE_LAST_NP(first, last, next, prev) :                              \
    ((node)->next->prev = (node)->prev, (node)->prev->next = (node)->next))

#define DLL_PUSH_BACK(first, last, node)                DLL_PUSH_BACK_NP(first, last, node, next, prev)
#define DLL_PUSH_FRONT(first, last, node)               DLL_PUSH_FRONT_NP(first, last, node, next, prev)
#define DLL_INSERT_AFTER(first, last, ref_node, node)   DLL_INSERT_AFTER_NP(first, last, ref_node, node, next, prev)
#define DLL_INSERT_BEFORE(first, last, ref_node, node)  DLL_INSERT_BEFORE_NP(first, last, ref_node, node, next, prev)
#define DLL_REMOVE(first, last, node)                   DLL_REMOVE_NP(first, last, node, next, prev)
#define DLL_REMOVE_FIRST(first, last)                   DLL_REMOVE_FIRST_NP(first, last, next, prev)
#define DLL_REMOVE_LAST(first, last)                    DLL_REMOVE_LAST_NP(first, last, next, prev)


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
//  ARRAY_RESERVE(arena, floats, 128);   // reserve space for 128 items
//  ARRAY_PUSH(arena, floats, 3.1415f);  // push to back
//  ...

typedef struct ArrayHeader ArrayHeader;
struct ArrayHeader
{
    uint64 len;
    uint64 capacity;
};

void* array_grow(Arena* arena, ArrayHeader* header, void* items, uint64 item_size, uint64 count);
void  array_shift_down(ArrayHeader* header, void* items, uint64 item_size, uint64 from_idx);
void  array_shift_down(ArrayHeader* header, void* items, uint64 item_size, uint64 from_idx);

#define ARRAY_HEADER_CAST(a) (&(a).header)
#define ARRAY_ITEM_SIZE(a) (sizeof(*(a).items))
#define ARRAY_LEN(a) (a.header.len)
#define ARRAY_CAP(a) (a.header.capacity)

#define ARRAY_PUSH(arena, a, item)                                                                         \
    (*((void**)&(a).items) = array_grow((arena), ARRAY_HEADER_CAST(a), (a).items, ARRAY_ITEM_SIZE(a), 1),  \
    (a).items[(a).header.len++] = (item))

#define ARRAY_ADD(arena, a, n)                                                                              \
    (*((void**)&(a).items) = array_grow((arena), ARRAY_HEADER_CAST(a), (a).items, ARRAY_ITEM_SIZE(a), (n)), \
    (a).header.len += (n),                                                                                  \
    &(a).items[(a).header.len - (n)])

#define ARRAY_RESERVE(arena, a, n) \
    (*((void**)&(a).items) = array_grow((arena), ARRAY_HEADER_CAST(a), (a).items, ARRAY_ITEM_SIZE(a), (n)))

#define ARRAY_CLEAR(a) ((a).header.len = 0)
#define ARRAY_CLEAR_ZERO(a) (memset((a).items, 0, ARRAY_ITEM_SIZE(a)*ARRAY_LEN(a)), (a).header.len = 0)

#define ARRAY_INSERT(arena, a, i, item)                                                                          \
    do {                                                                                                         \
        if ((uint64)(i) <= (a).header.len)                                                                       \
        {                                                                                                        \
            *((void**)&(a).items) = array_grow((arena), ARRAY_HEADER_CAST(a), (a).items, ARRAY_ITEM_SIZE(a), 1); \
            array_shift_up(ARRAY_HEADER_CAST(a), (a).items, ARRAY_ITEM_SIZE(a), (i));                          \
            (a).items[(i)] = (item);                                                                             \
            (a).header.len++;                                                                                    \
        }                                                                                                        \
    } while (0)


#define ARRAY_REMOVE(a, i)                                                                  \
    do {                                                                                    \
        if ((uint64)(i) < (a).header.len)                                                   \
        {                                                                                   \
            array_shift_down(ARRAY_HEADER_CAST(a), (a).items, ARRAY_ITEM_SIZE(a), (i) + 1); \
            (a).header.len--;                                                               \
        }                                                                                   \
    } while (0)


#define ARRAY_REMOVE_SWAP(a, i)                                \
    do {                                                       \
        if ((uint64)(i) < (a).header.len)                      \
            (a).items[(i)] = (a).items[--(a).header.len];      \
    } while (0)


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

// #define HMAP_RESERVE(arena, m, n)
// #define HMAP_DEFAULT(arena, m, val)
// #define HMAP_DEFAULTS(arena, m, s)
// #define HMAP_CLEAR(m)
// #define HMAP_CLONE(m)

// #define HMAP_PUT(arena, m, k, val)
// #define HMAP_INDEX(arena, m, k)
// #define HMAP_GET(m, type, k)
// #define HMAP_TRY_GET(m, k, out_val)
// #define HMAP_GETS(m, type, k)
// #define HMAP_GET_PTR(m, k)
// #define HMAP_GET_PTR_NULL(m, k)
// #define HMAP_DEL(scratch, m, k)


//************************
// String Hash Map
//************************

// #define SMAP_RESERVE(arena, m, n)
// #define SMAP_DEFAULT(arena, m, val)
// #define SMAP_DEFAULTS(arena, m, s)
// #define SMAP_CLEAR(m)
// #define SMAP_CLONE(m)

// #define SMAP_PUT(arena, m, k, val)
// #define SMAP_INDEX(arena, m, k)
// #define SMAP_GET(m, type, k)
// #define SMAP_TRY_GET(m, k, out_val)
// #define SMAP_GETS(m, type, k)
// #define SMAP_GET_PTR(m, k)
// #define SMAP_GET_PTR_NULL(m, k)
// #define SMAP_DEL(scratch, m, k)


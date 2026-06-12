
#ifndef BASE_DATA_STRUCTURES_H
#define BASE_DATA_STRUCTURES_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include "base_core.h"
#include "base_arena.h"

//***************************************************************************
//          TYPES
//***************************************************************************

typedef struct ArrayHeader ArrayHeader;
struct ArrayHeader
{
    uint64_t len;
    uint64_t cap;
};

// typedef struct HashMapEntry HashMapEntry;
// struct HashMapEntry 
// {
//     uint64_t hash;
//     uint64_t idx;
// };
//
// typedef struct HashMapHeader HashMapHeader;
// struct HashMapHeader
// {
//     uint64_t len;
//     uint64_t cap;
//     uint64_t insertIndex;
//     uint64_t usedCountThreshold;
//     uint64_t deletedCount;
//     uint64_t deletedCountThreshold;
//     HashMapEntry* table;
// };

//***************************************************************************
//          MACROS
//***************************************************************************

//*******************
// STACK
//*******************

#define STACK_PUSH_N(first, node, next) \
    ((node)->next = (first), (first) = (node))

#define STACK_POP_N(first, next) \
    ((first) = (first)->next)

#define STACK_PUSH(first, node) STACK_PUSH_N(first, node, next)

#define STACK_POP(first) STACK_POP_N(first, next)

//*******************
// QUEUE
//*******************

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

//*******************
// DOUBLY-LINKED LIST
//*******************

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

//*******************
// DYNAMIC ARRAY
//*******************

#define ARRAY_HEADER_CAST(a)                  (((ArrayHeader*)(a))-1)
#define ARRAY_ITEM_SIZE(a)                    (sizeof(*(a)))
#define ARRAY_LEN(a)                          ((a) ? ARRAY_HEADER_CAST((a))->len : 0)
#define ARRAY_CAP(a)                          ((a) ? ARRAY_HEADER_CAST((a))->cap : 0)

#define ARRAY_REMOVE(a, i)                    (ARRAY_REMOVEN((a), (i), 1))
#define ARRAY_REMOVEN(a, i, n)                ((i) + (n) < ARRAY_LEN(a) ? memmove(&(a)[i], &(a)[(i)+(n)], ARRAY_ITEM_SIZE(a) * (ARRAY_LEN(a)-(n)-(i))), ARRAY_HEADER_CAST(a)->len -= (n) : 0)
#define ARRAY_REMOVE_SWAP(a,i)                ((i) < ARRAY_LEN(a) ? (a)[i] = (a)[ARRAY_LEN(a)-1], ARRAY_HEADER_CAST(a)->len-- : 0)

#define ARRAY_CLEAR(a)                        (ARRAY_HEADER_CAST(a)->len = 0)
#define ARRAY_CLEAR_ZERO(a)                   (memset((a), 0, ARRAY_ITEM_SIZE(a)*ARRAY_LEN(a)), ARRAY_HEADER_CAST(a)->len = 0)

//*******************
// HEAP BACKED ARRAY
//*******************

#define ARRAY_RESERVE(a, n)                   ((a) = array_grow_heap((a), ARRAY_ITEM_SIZE(a), (n)))
// #define ARRAY_SETCAP(a, cap)                   void

#define ARRAY_PUSH(a, item)                   ((a) = array_grow_heap((a), ARRAY_ITEM_SIZE(a), 1), (a)[ARRAY_HEADER_CAST(a)->len++] = (item))
#define ARRAY_ADD(a)                          (ARRAY_ADDN((a), 1))
#define ARRAY_ADDN(a, n)                      ((a) = array_grow_heap((a), ARRAY_ITEM_SIZE(a), (n)), ARRAY_HEADER_CAST(a)->len += (n), &(a)[ARRAY_LEN(a) - (n)])

#define ARRAY_INSERTN(a, i, n)                (ARRAY_ADDN(a, n), memmove(&(a)[(i)+(n)], &(a)[i], ARRAY_ITEM_SIZE(a) * (ARRAY_LEN(a) - (n) - (i))))
#define ARRAY_INSERT(a, i, item)              (ARRAY_INSERTN(a, i, 1), (a)[i] = item)

#define ARRAY_FREE(a)                         ((a) ? free(ARRAY_HEADER_CAST(a)), 1 : 0)

//*******************
// ARENA BACKED ARRAY
//*******************

// TODO(bcall): reserve n additional or reserve cap of n...
#define ARRAY_RESERVE_ARENA(arena, a, n)      ((a) = array_grow_arena((arena), (a), ARRAY_ITEM_SIZE(a), (n)))
// #define ARRAY_SETCAP_ARENA(arena, a, cap)     void     

#define ARRAY_PUSH_ARENA(arena, a, item)      ((a) = array_grow_arena((arena), (a), ARRAY_ITEM_SIZE(a), 1), (a)[ARRAY_HEADER_CAST(a)->len++] = (item))
#define ARRAY_ADD_ARENA(arena, a)             ARRAY_ADDN_ARENA((arena), (a), 1) 
#define ARRAY_ADDN_ARENA(arena, a, n)         (((a) = array_grow_arena((arena), (a), ARRAY_ITEM_SIZE(a), (n))), ARRAY_HEADER_CAST(a)->len += (n), &(a)[ARRAY_LEN(a) - (n)])

#define ARRAY_INSERTN_ARENA(arena, a, i, n)   (ARRAY_ADDN_ARENA(arena, a, n), memmove(&(a)[(i)+(n)], &(a)[i], ARRAY_ITEM_SIZE(a) * (ARRAY_LEN(a) - (n) - (i))))
#define ARRAY_INSERT_ARENA(arena, a, i, item) (ARRAY_INSERTN_ARENA(arena, a, i, 1), (a)[i] = item)

//************************
// Hash Map
//************************


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

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

void* array_grow_arena  (Arena* arena, void* items, uint64_t item_size, uint64_t count);
void* array_grow_heap                 (void* items, uint64_t item_size, uint64_t count);

#endif // BASE_DATA_STRUCTURES_H

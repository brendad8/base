
#ifndef BASE_DATA_STRUCTURES_H
#define BASE_DATA_STRUCTURES_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include "base_core.h"
#include "base_arena.h"

#define STBDS_NO_SHORT_NAMES
#include "deps/stb_ds.h"

//***************************************************************************
//          MACROS
//***************************************************************************

//*******************
// STACK
//*******************

/*

void  STACK_PUSH  (T* first, T* node)    Pushes node to front of stack
void  STACK_POP   (T* first)          Pops node from front of stack. Node is not returned.

*/

#define STACK_PUSH(first, node)   STACK_PUSH_N(first, node, next)
#define STACK_POP(first)          STACK_POP_N(first, next)

#define STACK_PUSH_N(first, node, next) \
    ((node)->next = (first), (first) = (node))

#define STACK_POP_N(first, next) \
    ((first) = (first)->next)



//*******************
// QUEUE
//*******************

/*

void  QUEUE_PUSH  (T* first, T* last, T* node)    Pushes node to end of queue
void  QUEUE_POP   (T* first, T* last)          Pops node from start of queue. Node is not returned.

*/

#define QUEUE_PUSH(first, last, node)    QUEUE_PUSH_N(first, last, node, next)
#define QUEUE_POP(first, last)           QUEUE_POP_N(first, last, next) 

#define QUEUE_PUSH_N(first, last, node, next)                       \
    ((first) == NULL ?                                              \
    ((first) = (last) = (node), (node)->next = NULL) :              \
    ((last)->next = (node), (last) = (node), (node)->next = NULL))

#define QUEUE_POP_N(first, last, next)                              \
    ((first) == (last) ?                                            \
    (first) = (last) = NULL :                                       \
    ((first) = (first)->next))


//*******************
// DOUBLY-LINKED LIST
//*******************

/*

void  DLL_PUSH_BACK      (T* first, T* last, T* node)                 Pushes node to end of list
void  DLL_PUSH_FRONT     (T* first, T* last, T* node)                 Pushes node to front of list

void  DLL_INSERT_AFTER   (T* first, T* last, T* ref_node, T* node)    Inserts node after reference node in list
void  DLL_INSERT_BEFORE  (T* first, T* last, T* ref_node, T* node)    Inserts node before reference node in list

void  DLL_REMOVE         (T* first, T* last, T* node)                 Removes node from list
void  DLL_REMOVE_FIRST   (T* first, T* last)                          Removes node from front of list
void  DLL_REMOVE_LAST    (T* first, T* last)                          Removes node from end of list

*/

#define DLL_PUSH_BACK(first, last, node)                  DLL_PUSH_BACK_NP(first, last, node, next, prev)
#define DLL_PUSH_FRONT(first, last, node)                 DLL_PUSH_FRONT_NP(first, last, node, next, prev)
#define DLL_INSERT_AFTER(first, last, ref_node, node)     DLL_INSERT_AFTER_NP(first, last, ref_node, node, next, prev)
#define DLL_INSERT_BEFORE(first, last, ref_node, node)    DLL_INSERT_BEFORE_NP(first, last, ref_node, node, next, prev)
#define DLL_REMOVE(first, last, node)                     DLL_REMOVE_NP(first, last, node, next, prev)
#define DLL_REMOVE_FIRST(first, last)                     DLL_REMOVE_FIRST_NP(first, last, next, prev)
#define DLL_REMOVE_LAST(first, last)                      DLL_REMOVE_LAST_NP(first, last, next, prev)

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


//*******************
// DYNAMIC ARRAY
//*******************

/*  
   
size_t  ARRAY_LEN          (T* array)                        Returns number of elements in array          
size_t  ARRAY_CAP          (T* array)                        Returns number of elements array can hold before resizing
T       ARRAY_LAST         (T* array)                        Returns last item of array as an lvalue

void    ARRAY_SET_LEN      (T* array)                        Sets the length of the array. Leaves slots uninitialized
void    ARRAY_SET_CAP      (T* array)                        Sets the capacity of the array. Cannot shrink array

T       ARRAY_PUSH         (T* array, T item)                Copies item to end of array. Returns item
void    ARRAY_POP          (T* array)                        Removes last item from array and returns the item

T*      ARRAY_ADDN_PTR     (T* array, size_t n)              Adds n unitialized items to array and returns pointer to first unitialized item
size_t  ARRAY_ADDN_INDEX   (T* array, size_t n)              Adds n unitialized items to array and returns index to first unitialized item

T       ARRAY_INSERT       (T* array, size_t i, T item)      Copies item into ith index of array and shifts items to make space. Returns item.
void    ARRAY_INSERT_N     (T* array, size_t i, size_t n)    Creates room for n uninitialized entries starting from index i

void    ARRAY_DELETE       (T* array, size_t i)              Deletes ith item from array and shifts items after down
void    ARRAY_DELETE_N     (T* array, size_t i, size_t n)    Detetes n items starting from index i and shifts items after down  
void    ARRAY_DELETE_SWAP  (T* array, size_t i)              Deletes ith item from array and swaps last item instead of shifting down

void    ARRAY_FREE         (T* array)                        Frees array memory and sets array pointer to NULL

*/

#define ARRAY_LEN          stbds_arrlenu
#define ARRAY_CAP          stbds_arrcap
#define ARRAY_LAST         stbds_arrlast
#define ARRAY_SET_LEN      stbds_arrsetlen
#define ARRAY_SET_CAP      stbds_arrsetcap
#define ARRAY_PUSH         stbds_arrput
#define ARRAY_POP          stbds_arrpop
#define ARRAY_ADDN_PTR     stbds_arraddnptr
#define ARRAY_ADDN_INDEX   stbds_arraddnindex
#define ARRAY_INSERT       stbds_arrins
#define ARRAY_INSERT_N     stbds_arrinsn
#define ARRAY_DELETE       stbds_arrdel
#define ARRAY_DELETE_N     stbds_arrdeln
#define ARRAY_DELETE_SWAP  stbds_arrdelswap
#define ARRAY_FREE         stbds_arrfree

//************************
// HASH MAP
//************************

/*

size_t     HMAP_LEN           (T* map)                           Returns number of items in map. Can be used to iterate over items in map array.

void       HMAP_DEFAULT       (T* map, TV value)                 Sets the default value returned by HM_GET if key is not present
void       HMAP_DEFAULTS      (T* map, T struct)                 Sets the default struct returned by HM_GETS if key is not present

void       HMAP_PUT           (T* map, TK key, TV value)         Update existing key, value or add new item if key is new         
void       HMAP_PUTS          (T* map, T struct)                 Same as HMAP_PUT but takes in struct with key and value fields

TV         HMAP_GET           (T* map, TK key)                   Returns the value of the corresponding key, or default if key is missing
TV         HMAP_GET_TS        (T* map, TK key, ptrdiff_t tmp)    Same as HMAP_GET but thread safe
T          HMAP_GETS          (T* map, TK key)                   Returns the struct of the corresponding key, or default if key is missing

T*         HMAP_GET_PTR       (T* map, TK key)                   Returns pointer to struct corresponding to key or pointer to default if key is missing
T*         HMAP_GET_PTR_TS    (T* map, TK key, ptrdiff_t tmp)    Same at HMAP_GETP but thread safe
T*         HMAP_GET_PTR_NULL  (T* map, TK key)                   Same as HMAP_GETP but retunrs null if key is missing

ptrdiff_t  HMAP_GET_IDX       (T* map, TK key)                   Returns the index of key into map array or -1 if key is missing
ptrdiff_t  HMAP_GET_IDX_TS    (T* map, ptrdiff_t tmp)            Same as HMAP_GETI but thread safe

int        HMAP_DEL           (T* map, TK key)                   Deletes item corresponding to key from hashmap and returns 1. Returns 0 otherwise.

void       HMAP_FREE          (T* map)                           Frees hash map memory and sets pointer to NULL

*/

#define HMAP_LEN           stbds_hmlenu
#define HMAP_DEFAULT       stbds_hmdefault
#define HMAP_DEFAULTS      stbds_hmdefaults
#define HMAP_PUT           stbds_hmput
#define HMAP_PUTS          stbds_hmputs
#define HMAP_GET           stbds_hmget
#define HMAP_GET_TS        stbds_hmget_ts
#define HMAP_GETS          stbds_hmgets
#define HMAP_GET_PTR       stbds_hmgetp
#define HMAP_GET_PTR_TS    stbds_hmgetp_ts
#define HMAP_GET_PTR_NULL  stbds_hmgetp_null
#define HMAP_GET_IDX       stbds_hmgeti
#define HMAP_GET_IDX_TS    stbds_hmgeti_ts
#define HMAP_DEL           stbds_hmdel
#define HMAP_FREE          stbds_hmfree

//************************
// STRING HASH MAP
//************************

// #define SMAP_PUT         stbds_shput
// #define SMAP_PUTI        stbds_shputi
// #define SMAP_PUTS        stbds_shputs
// #define SMAP_GET         stbds_shget
// #define SMAP_GETI        stbds_shgeti
// #define SMAP_GETS        stbds_shgets
// #define SMAP_GETP        stbds_shgetp
// #define SMAP_GETP_NULL   stbds_shgetp_null
// #define SMAP_DEL         stbds_shdel
// #define SMAP_LEN         stbds_shlen
// #define SMAP_LENU        stbds_shlenu
// #define SMAP_FREE        stbds_shfree
// #define SMAP_DEFAULT     stbds_shdefault
// #define SMAP_DEFAULTS    stbds_shdefaults
// #define SMAP_NEW_ARENA   stbds_sh_new_arena
// #define SMAP_NEW_STRDUP  stbds_sh_new_strdup
//
// #define SMAP_STR_ALLOC   stbds_stralloc
// #define SMAP_STR_RESET   stbds_strreset


#endif // BASE_DATA_STRUCTURES_H
      




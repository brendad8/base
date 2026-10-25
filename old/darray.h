
/* darray.h - dynamic array macros

   To use this library, do this in *one* C file:
      #define DARRAY_IMPLEMENTATION
      #include "base/darray.h"

   ACKNOWLEDGMENTS
   
      Adapted from stb_ds.h - v0.67 
      public domain data structures - Sean Barrett 2019
      http://nothings.org/stb_ds 
        
*/

#ifndef DARRAY_H
#define DARRAY_H

/***************************************************************************
 *          INCLUDES
 ***************************************************************************/

#include <string.h> // for memmove

/***************************************************************************
 *          MACROS
 ***************************************************************************/

#define darray_cap(a)             ((a) ? __DARRAY_HEADER(a)->capacity : 0)
#define darray_len(a)             ((a) ? __DARRAY_HEADER(a)->length : 0)

#define darray_set_cap(a,n)       (__DARRAY_GROW(a,0,n))
#define darray_set_len(a,n)       ((darray_cap(a) < (size_t)(n) ? darray_set_cap((a),(size_t)(n)),0 : 0), (a) ? __DARRAY_HEADER(a)->length = (size_t)(n) : 0)

#define darray_push(a,v)          (__DARRAY_MAYBE_GROW(a,1), (a)[__DARRAY_HEADER(a)->length++] = (v))
#define darray_pop(a)             (__DARRAY_HEADER(a)->length--, (a)[__DARRAY_HEADER(a)->length])

#define darray_addn_ptr(a,n)      (__DARRAY_MAYBE_GROW(a,n), (n) ? (__DARRAY_HEADER(a)->length += (n), &(a)[__DARRAY_HEADER(a)->length-(n)]) : (a))
#define darray_addn_idx(a,n)      (__DARRAY_MAYBE_GROW(a,n), (n) ? (__DARRAY_HEADER(a)->length += (n), __DARRAY_HEADER(a)->length-(n)) : darray_len(a))

#define darray_insertn(a,i,n)     (darray_addn_idx((a),(n)), memmove(&(a)[(i)+(n)], &(a)[i], sizeof(*(a))*(__DARRAY_HEADER(a)->length-(n)-(i))))
#define darray_insert(a,i,v)      (darray_insertn((a),(i),1), (a)[i]=(v))

#define darray_deleten(a,i,n)     (memmove(&(a)[i], &(a)[(i)+(n)], sizeof(*(a))*(__DARRAY_HEADER(a)->length-(n)-(i))), __DARRAY_HEADER(a)->length -= (n))
#define darray_delete(a,i)        darray_deleten(a,i,1)
#define darray_delete_swap(a,i)   ((a)[i] = (a)[__DARRAY_HEADER(a)->length-1], __DARRAY_HEADER(a)->length -= 1)

#define darray_free(a)            ((void)((a) ? DARRAY_FREE_FUNC(NULL, __DARRAY_HEADER(a)) : (void)0), (a)=NULL)

/***************************************************************************
 *          HIDDEN
 ***************************************************************************/

typedef struct
{
  size_t length;
  size_t capacity;

} __ArrayHeader;

extern void* __darray_grow(void* arr, size_t item_size, size_t add_len, size_t min_cap);

#ifdef DARRAY_UNIT_TESTS
    static void  darray_unit_tests(void);
#endif

#define __DARRAY_HEADER(a)         (((__ArrayHeader*)(a)) - 1)
#define __DARRAY_GROW(a,add,cap)   ((a) = __darray_grow((a), sizeof *(a), (add), (cap)))
#define __DARRAY_MAYBE_GROW(a,n)   ((!(a) || __DARRAY_HEADER(a)->length + (n) > __DARRAY_HEADER(a)->capacity) ? (__DARRAY_GROW(a,n,0),0) : 0)

#endif // DARRAY_H

/***************************************************************************
 *          IMPLEMENTATIONS
 ***************************************************************************/

#ifdef DARRAY_IMPLEMENTATION

#ifdef DARRAY_STATISTICS
    #define __DARRAY_STATS(x) x
    size_t darray_grow_count;
#else
    #define __DARRAY_STATS(x)
#endif

#if defined(DARRAY_REALLOC_FUNC) && !defined(DARRAY_FREE_FUNC) || !defined(DARRAY_REALLOC_FUNC) && defined(DARRAY_FREE_FUNC)
    #error "You must define both DARRAY_REALLOC_FUNC and DARRAY_FREE_FUNC, or neither."
#endif
#if !defined(DARRAY_REALLOC_FUNC) && !defined(DARRAY_FREE_FUNC)
    #include <stdlib.h>
    #define DARRAY_REALLOC_FUNC(c,p,s) realloc(p,s)
    #define DARRAY_FREE_FUNC(c,p)      free(p)
#endif

void* __darray_grow(void* arr, size_t item_size, size_t add_len, size_t min_cap)
{
    size_t len = darray_len(arr);
    size_t cap = darray_cap(arr);
    size_t min_len = len + add_len;

    if (min_len > min_cap)
        min_cap = min_len;

    if (min_cap <= cap)
        return arr;

    if (min_cap < 2 * cap)
        min_cap = 2 * cap;
    else if (min_cap < 4)
        min_cap = 4;

    void* arr_new = DARRAY_REALLOC_FUNC(NULL, (arr) ? __DARRAY_HEADER(arr) : 0, item_size * min_cap + sizeof(__ArrayHeader));
    arr_new = (char*)arr_new + sizeof(__ArrayHeader);

    if (arr == NULL)
        __DARRAY_HEADER(arr_new)->length = 0;
    else
        __DARRAY_STATS(darray_grow_count++);

    __DARRAY_HEADER(arr_new)->capacity = min_cap;

    return arr_new;
}

#endif // DARRAY_IMPLEMENTATION


#ifdef DARRAY_UNIT_TESTS
#include <stdio.h>

#ifndef DARRAY_ASSERT
    #define DARRAY_ASSERT assert
    #include <assert.h>
#endif

typedef struct { int key,b,c,d; } test_struct;
typedef struct { int key[2],b,c,d; } test_struct2;

static void darray_unit_tests(void)
{
    const int testsize = 100000;
    const int testsize2 = testsize/20;
    int *arr = NULL;
    int i,j;

    DARRAY_ASSERT(darray_len(arr)==0);
    for (i=0; i < 20000; i += 50) 
    {
        for (j=0; j < i; ++j)
          darray_push(arr,j);
        darray_free(arr);
    }

    for (i=0; i < 4; ++i) 
    {
        darray_push(arr,1); darray_push(arr,2); darray_push(arr,3); darray_push(arr,4);
        darray_delete(arr,i);
        darray_free(arr);
        darray_push(arr,1); darray_push(arr,2); darray_push(arr,3); darray_push(arr,4);
        darray_delete(arr,i);
        darray_free(arr);
    }

    for (i=0; i < 5; ++i) 
    {
        darray_push(arr,1); darray_push(arr,2); darray_push(arr,3); darray_push(arr,4);
        darray_insert(arr,i,5);
        DARRAY_ASSERT(arr[i] == 5);
        if (i < 4)
            DARRAY_ASSERT(arr[4] == 4);
        darray_free(arr);
    }
}
#endif // DARRAY_UNIT_TESTS


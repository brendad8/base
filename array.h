
// Adapted from stb_ds.h - v0.67 
//    public domain data structures - Sean Barrett 2019
//    http://nothings.org/stb_ds

#ifndef ARRAY_H
#define ARRAY_H

//***************************************************************************
//          CONFIGURATION OPTIONS
//***************************************************************************

// #define ARRAY_STATISTICS  // will include array_grow_count which tracks
                             // the number of times any dynamic array had
                             // to be resized

// #define __ARRAY_REALLOC(context, ptr, size) user_defined_realloc
// #define __ARRAY_FREE(context, ptr)          user_defined_free   

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include <string.h> // for memmove

//***************************************************************************
//          HIDDEN
//***************************************************************************

typedef struct
{
  size_t length;
  size_t capacity;

} __ArrayHeader;

extern void* __array_grow(void* arr, size_t item_size, size_t add_len, size_t min_cap);
static void  __array_unit_tests(void);

#define __ARRAY_HEADER(a)         (((__ArrayHeader*)(a)) - 1)
#define __ARRAY_GROW(a,add,cap)   ((a) = __array_grow((a), sizeof *(a), (add), (cap)))
#define __ARRAY_MAYBE_GROW(a,n)   ((!(a) || __ARRAY_HEADER(a)->length + (n) > __ARRAY_HEADER(a)->capacity) ? (__ARRAY_GROW(a,n,0),0) : 0)

//***************************************************************************
//          PUBLIC MACROS
//***************************************************************************

#define ARRAY_CAP(a)             ((a) ? __ARRAY_HEADER(a)->capacity : 0)
#define ARRAY_LEN(a)             ((a) ? __ARRAY_HEADER(a)->length : 0)

#define ARRAY_SET_CAP(a,n)       (__ARRAY_GROW(a,0,n))
#define ARRAY_SET_LEN(a,n)       ((ARRAY_CAP(a) < (size_t)(n) ? ARRAY_SET_CAP((a),(size_t)(n)),0 : 0), (a) ? __ARRAY_HEADER(a)->length = (size_t)(n) : 0)

#define ARRAY_PUSH(a,v)          (__ARRAY_MAYBE_GROW(a,1), (a)[__ARRAY_HEADER(a)->length++] = (v))
#define ARRAY_POP(a)             (__ARRAY_HEADER(a)->length--, (a)[__ARRAY_HEADER(a)->length])

#define ARRAY_ADDN_PTR(a,n)      (__ARRAY_MAYBE_GROW(a,n), (n) ? (__ARRAY_HEADER(a)->length += (n), &(a)[__ARRAY_HEADER(a)->length-(n)]) : (a))
#define ARRAY_ADDN_IDX(a,n)      (__ARRAY_MAYBE_GROW(a,n), (n) ? (__ARRAY_HEADER(a)->length += (n), __ARRAY_HEADER(a)->length-(n)) : ARRAY_LEN(a))

#define ARRAY_INSERTN(a,i,n)     (ARRAY_ADDN_IDX((a),(n)), memmove(&(a)[(i)+(n)], &(a)[i], sizeof(*(a))*(__ARRAY_HEADER(a)->length-(n)-(i))))
#define ARRAY_INSERT(a,i,v)      (ARRAY_INSERTN((a),(i),1), (a)[i]=(v))

#define ARRAY_DELETEN(a,i,n)     (memmove(&(a)[i], &(a)[(i)+(n)], sizeof(*(a))*(__ARRAY_HEADER(a)->length-(n)-(i))), __ARRAY_HEADER(a)->length -= (n))
#define ARRAY_DELETE(a,i)        ARRAY_DELETEN(a,i,1)
#define ARRAY_DELETE_SWAP(a,i)   ((a)[i] = (a)[__ARRAY_HEADER(a)->length-1], __ARRAY_HEADER(a)->length -= 1)

#define ARRAY_FREE(a)            ((void)((a) ? __ARRAY_FREE(NULL, __ARRAY_HEADER(a)) : (void)0), (a)=NULL)

#endif // ARRAY_H

//***************************************************************************
//          FUNCTION IMPLEMENTATIONS
//***************************************************************************

#ifdef ARRAY_IMPLEMENTATION

#ifdef ARRAY_STATISTICS
    #define __ARRAY_STATS(x) x
    size_t array_grow_count;
#else
    #define __ARRAY_STATS(x)
#endif

#if defined(__ARRAY_REALLOC) && !defined(__ARRAY_FREE) || !defined(__ARRAY_REALLOC) && defined(__ARRAY_FREE)
    #error "You must define both __ARRAY_REALLOC and __ARRAY_FREE, or neither."
#endif
#if !defined(__ARRAY_REALLOC) && !defined(__ARRAY_FREE)
    #include <stdlib.h>
    #define __ARRAY_REALLOC(c,p,s) realloc(p,s)
    #define __ARRAY_FREE(c,p)      free(p)
#endif

void* __array_grow(void* arr, size_t item_size, size_t add_len, size_t min_cap)
{
    size_t len = ARRAY_LEN(arr);
    size_t cap = ARRAY_CAP(arr);
    size_t min_len = len + add_len;

    if (min_len > min_cap)
        min_cap = min_len;

    if (min_cap <= cap)
        return arr;

    if (min_cap < 2 * cap)
        min_cap = 2 * cap;
    else if (min_cap < 4)
        min_cap = 4;

    void* arr_new = __ARRAY_REALLOC(NULL, (arr) ? __ARRAY_HEADER(arr) : 0, item_size * min_cap + sizeof(__ArrayHeader));
    arr_new = (char*)arr_new + sizeof(__ArrayHeader);

    if (arr == NULL)
        __ARRAY_HEADER(arr_new)->length = 0;
    else
        __ARRAY_STATS(array_grow_count++);

    __ARRAY_HEADER(arr_new)->capacity = min_cap;

    return arr_new;
}

void __array_free(void* a)
{
    __ARRAY_FREE(NULL, a);
}

#endif // ARRAY_IMPLEMENTATION


#ifdef ARRAY_UNIT_TESTS

#include <stdio.h>


#ifndef ARRAY_ASSERT
    #define ARRAY_ASSERT assert
    #include <assert.h>
#endif

typedef struct { int key,b,c,d; } test_struct;
typedef struct { int key[2],b,c,d; } test_struct2;

static char buffer[256];
char* strkey(int n)
{
#if defined(_WIN32) && defined(__STDC_WANT_SECURE_LIB__)
   sprintf_s(buffer, sizeof(buffer), "test_%d", n);
#else
   sprintf(buffer, "test_%d", n);
#endif
   return buffer;
}

static void array_unit_tests(void)
{
    const int testsize = 100000;
    const int testsize2 = testsize/20;
    int *arr=NULL;
    int i,j;

    ARRAY_ASSERT(ARRAY_LEN(arr)==0);
    for (i=0; i < 20000; i += 50) 
    {
        for (j=0; j < i; ++j)
          ARRAY_PUSH(arr,j);
        ARRAY_FREE(arr);
    }

    for (i=0; i < 4; ++i) 
    {
        ARRAY_PUSH(arr,1); ARRAY_PUSH(arr,2); ARRAY_PUSH(arr,3); ARRAY_PUSH(arr,4);
        ARRAY_DELETE(arr,i);
        ARRAY_FREE(arr);
        ARRAY_PUSH(arr,1); ARRAY_PUSH(arr,2); ARRAY_PUSH(arr,3); ARRAY_PUSH(arr,4);
        ARRAY_DELETE(arr,i);
        ARRAY_FREE(arr);
    }

    for (i=0; i < 5; ++i) 
    {
        ARRAY_PUSH(arr,1); ARRAY_PUSH(arr,2); ARRAY_PUSH(arr,3); ARRAY_PUSH(arr,4);
        ARRAY_INSERT(arr,i,5);
        ARRAY_ASSERT(arr[i] == 5);
        if (i < 4)
            ARRAY_ASSERT(arr[4] == 4);
        ARRAY_FREE(arr);
    }
}

#endif // ARRAY_UNIT_TESTS



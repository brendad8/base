
/* array.h - dynamic array macros

   To use this library, do this in *one* C file:
      #define ARRAY_IMPLEMENTATION
      #include "base/array.h"


ACKNOWLEDGMENTS
   
   Adapted from stb_ds.h - v0.67 
   public domain data structures - Sean Barrett 2019
   http://nothings.org/stb_ds 


COMPILE-TIME OPTIONS

  #define ARRAY_REALLOC_FUNC(context,ptr,size) user_defined_realloc
  #define ARRAY_FREE_FUNC(context,ptr)         user_defined_free

     These defines only need to be set in the file containing #define ARRAY_IMPLEMENTATION.

     By default array uses stdlib realloc() and free() for memory management. You can
     substitute your own functions instead by defining these symbols. You must either
     define both, or neither. Note that at the moment, 'context' will always be NULL.
  
  #define ARRAY_STATISTICS

    Will include global array_grow_count which tracks the number of times any dynamic 
    array had to be resized. 

  #define ARRAY_UNIT_TESTS

     Defines a function stbds_unit_tests() that checks the functionality of the data structure.


DOCUMENTATION

  Declare an empty dynamic array of type T
    T* t_arr = NULL;

  Access the i'th item of a dynamic array 't_arr' of type T, T* t_arr:
    t_arr[i]


  size_t  ARRAY_LEN          (T* array)                        - Returns number of elements in array          
  size_t  ARRAY_CAP          (T* array)                        - Returns number of elements array can hold before resizing
  
  void    ARRAY_SET_LEN      (T* array)                        - Sets the length of the array. Leaves slots uninitialized
  void    ARRAY_SET_CAP      (T* array)                        - Sets the capacity of the array. Cannot shrink array
  
  T       ARRAY_PUSH         (T* array, T item)                - Copies item to end of array. Returns item
  void    ARRAY_POP          (T* array)                        - Removes last item from array and returns the item
  
  T*      ARRAY_ADDN_PTR     (T* array, size_t n)              - Adds n unitialized items to array and returns pointer to first unitialized item
  size_t  ARRAY_ADDN_INDEX   (T* array, size_t n)              - Adds n unitialized items to array and returns index to first unitialized item
  
  T       ARRAY_INSERT       (T* array, size_t i, T item)      - Copies item into ith index of array and shifts items to make space. Returns item.
  void    ARRAY_INSERT_N     (T* array, size_t i, size_t n)    - Creates room for n uninitialized entries starting from index i
  
  void    ARRAY_DELETE       (T* array, size_t i)              - Deletes ith item from array and shifts items after down
  void    ARRAY_DELETE_N     (T* array, size_t i, size_t n)    - Detetes n items starting from index i and shifts items after down  
  void    ARRAY_DELETE_SWAP  (T* array, size_t i)              - Deletes ith item from array and swaps last item instead of shifting down
  
  void    ARRAY_FREE         (T* array)                        - Frees array memory and sets array pointer to NULL

*/

#ifndef _ARRAY_H
#define _ARRAY_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include <string.h> // for memmove

//***************************************************************************
//          TYPES
//***************************************************************************

typedef struct
{
  size_t length;
  size_t capacity;

} __ArrayHeader;

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

extern void* __array_grow(void* arr, size_t item_size, size_t add_len, size_t min_cap);

#ifdef ARRAY_UNIT_TESTS
    static void  array_unit_tests(void);
#endif

//***************************************************************************
//          MACROS
//***************************************************************************

#define __ARRAY_HEADER(a)         (((__ArrayHeader*)(a)) - 1)
#define __ARRAY_GROW(a,add,cap)   ((a) = __array_grow((a), sizeof *(a), (add), (cap)))
#define __ARRAY_MAYBE_GROW(a,n)   ((!(a) || __ARRAY_HEADER(a)->length + (n) > __ARRAY_HEADER(a)->capacity) ? (__ARRAY_GROW(a,n,0),0) : 0)

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

#define ARRAY_FREE(a)            ((void)((a) ? ARRAY_FREE_FUNC(NULL, __ARRAY_HEADER(a)) : (void)0), (a)=NULL)

#endif // _ARRAY_H

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

#if defined(ARRAY_REALLOC_FUNC) && !defined(ARRAY_FREE_FUNC) || !defined(ARRAY_REALLOC_FUNC) && defined(ARRAY_FREE_FUNC)
    #error "You must define both ARRAY_REALLOC_FUNC and ARRAY_FREE_FUNC, or neither."
#endif
#if !defined(ARRAY_REALLOC_FUNC) && !defined(ARRAY_FREE_FUNC)
    #include <stdlib.h>
    #define ARRAY_REALLOC_FUNC(c,p,s) realloc(p,s)
    #define ARRAY_FREE_FUNC(c,p)      free(p)
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

    void* arr_new = ARRAY_REALLOC_FUNC(NULL, (arr) ? __ARRAY_HEADER(arr) : 0, item_size * min_cap + sizeof(__ArrayHeader));
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


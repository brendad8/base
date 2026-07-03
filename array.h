
#ifndef ARRAY_H
#define ARRAY_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include <stdlib.h> // for malloc, free
#include <string.h> // for memmove

//***************************************************************************
//          PUBLIC MACROS
//***************************************************************************

#define ARRAY_SET_CAP(a,n)  (__ARRAY_GROW(a,0,n))
#define ARRAY_SET_LEN(a,n)  ((ARRAY_CAP(a) < (size_t)(n) ? ARRAY_SET_CAP((a),(size_t)(n)),0 : 0), (a) ? __ARRAY_HEADER(a)->length = (size_t)(n) : 0)
#define ARRAY_CAP(a)        ((a) ? __ARRAY_HEADER(a)->capacity : 0)
#define ARRAY_LEN(a)        ((a) ? __ARRAY_HEADER(a)->length : 0)
#define ARRAY_PUSH(a,v)     (__ARRAY_MAYBE_GROW(a,1), (a)[__ARRAY_HEADER(a)->length++] = (v))
#define ARRAY_POP(a)        (__ARRAY_HEADER(a)->length--, (a)[__ARRAY_HEADER(a)->length])
#define ARRAY_ADDN_PTR(a,n) (__ARRAY_MAYBE_GROW(a,n), (n) ? (__ARRAY_HEADER(a)->length += (n), &(a)[__ARRAY_HEADER(a)->length-(n)]) : (a))
#define ARRAY_ADDN_IDX(a,n) (__ARRAY_MAYBE_GROW(a,n), (n) ? (__ARRAY_HEADER(a)->length += (n), __ARRAY_HEADER(a)->length-(n)) : ARRAY_LEN(a))
#define ARRAY_FREE(a)       ((void) ((a) ? __array_free(__ARRAY_HEADER(a)) : (void)0), (a)=NULL)
#define ARRAY_DEL(a,i)      ARRAY_ARR_DELN(a,i,1)
#define ARRAY_DELN(a,i,n)   (memmove(&(a)[i], &(a)[(i)+(n)], sizeof(*(a)) * (__ARRAY_HEADER(a)->length-(n)-(i))), __ARRAY_HEADER(a)->length -= (n))
#define ARRAY_DEL_SWAP(a,i) ((a)[i] = (a)[__ARRAY_HEADER(a)->length-1], __ARRAY_HEADER(a)->length -= 1)
#define ARRAY_INSN(a,i,n)   (ARRAY_ADDN((a),(n)), memmove(&(a)[(i)+(n)], &(a)[i], sizeof(*(a)) * (__ARRAY_HEADER(a)->length-(n)-(i))))
#define ARRAY_INS(a,i,v)    (ARRAY_INSN((a),(i),1), (a)[i]=(v))

//***************************************************************************
//          HIDDEN
//***************************************************************************

typedef struct
{
  size_t length;
  size_t capacity;

} ArrayHeader;

#define __ARRAY_HEADER(a)  (((ArrayHeader*)(a)) - 1)
#define __ARRAY_MAYBE_GROW(a,n)   ((!(a) || __ARRAY_HEADER(a)->length + (n) > __ARRAY_HEADER(a)->capacity) ? (ARRAY_GROW(a,n,0),0) : 0)
#define __ARRAY_GROW(a,add,cap)   ((a) = __array_grow((a), sizeof *(a), (add), (cap)))

extern void* __array_grow(void* arr, size_t item_size, size_t add_len, size_t min_cap);

//***************************************************************************
//          FUNCTION IMPLEMENTATIONS
//***************************************************************************

#ifdef ARRAY_IMPLEMENTATION
#define ARRAY_IMPLEMENTATION

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

    void* arr_new = realloc((arr) ? __ARRAY_HEADER(arr) : 0, elem_size * min_cap + sizeof(ArrayHeader));
    arr_new = (char*)arr_new + sizeof(ArrayHeader);

    if (arr == NULL)
        __ARRAY_HEADER(b)->length = 0;

    __ARRAY_HEADER(b)->capacity = min_cap;

    return arr_new;
}

void _array_free(void* a)
{
    if (a) free(a);
}

#endif // ARRAY_IMPLEMENTATION

#endif // ARRAY_H

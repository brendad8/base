
/* map.h - hash table macros

   To use this library, do this in *one* C file:
      #define MAP_IMPLEMENTATION
      #include "base/map.h"
  

ACKNOWLEDGMENTS

   Adapted from stb_ds.h - v0.67
   public domain data structures - Sean Barrett 2019
   http://nothings.org/stb_ds


COMPILE-TIME OPTIONS

  #define MAP_REALLOC_FUNC(context,ptr,size) user_defined_realloc
  #define MAP_FREE_FUNC(context,ptr)         user_defined_free

     These defines only need to be set in the file containing
     #define MAP_IMPLEMENTATION.

     By default map uses stdlib realloc() and free() for memory
     management. You can substitute your own functions instead by
     defining these symbols. You must either define both, or neither.
     Note that at the moment, 'context' will always be NULL.

  #define MAP_UNIT_TESTS

     Defines map_unit_tests() which verifies the functionality of the
     data structure.


DOCUMENTATION

  A hash map entry must contain fields named "key" and "value".

    typedef struct
    {
        KeyType   key;
        ValueType value;
    } Entry;


  Declare an empty hash map:

    Entry* map = NULL;


  Loop over the entries of the hashmap:

    for (size_t i = 0; i < HMAP_LEN(map); i++)
    {
        Entry entry = map[i];
    }


HASH MAP API

  size_t   HMAP_LEN             (T* map)                             - Returns number of entries in map

  void     HMAP_PUT             (T* map, Key key, Value value)       - Inserts or replaces a key/value pair
  void     HMAP_PUT_STRUCT      (T* map, T entry)                    - Inserts or replaces an entire entry

  size_t   HMAP_GET_IDX         (T* map, Key key)                    - Returns index of key
  size_t   HMAP_GET_IDX_TS      (T* map, Key key, size_t temp)       - Thread-safe version of HMAP_GET_IDX

  T*       HMAP_GET_PTR         (T* map, Key key)                    - Returns pointer to entry
  T*       HMAP_GET_PTR_TS      (T* map, Key key, size_t temp)       - Thread-safe version of HMAP_GET_PTR
  T*       HMAP_GET_PTR_NULL    (T* map, Key key)                    - Returns NULL if key is not found

  Value    HMAP_GET             (T* map, Key key)                    - Returns value associated with key
  Value    HMAP_GET_TS          (T* map, Key key, size_t temp)       - Thread-safe version of HMAP_GET
  T        HMAP_GET_STRUCT      (T* map, Key key)                    - Returns entire entry

  bool     HMAP_DELETE          (T* map, Key key)                    - Removes key from map

  void     HMAP_DEFAULT         (T* map, Value value)                - Sets default value returned for missing keys
  void     HMAP_DEFAULT_STRUCT  (T* map, T entry)                    - Sets default entry returned for missing keys

  void     HMAP_FREE            (T* map)                             - Frees map memory and sets map pointer to NULL


STRING MAP API

  String maps use NULL-terminated strings as keys.

  void     SMAP_NEW_ARENA       (T* map)                             - Creates a string map using arena allocated keys
  void     SMAP_NEW_STRDUP      (T* map)                             - Creates a string map using duplicated keys

  void     SMAP_PUT             (T* map, char* key, Value value)     - Inserts or replaces a key/value pair
  size_t   SMAP_PUTI            (T* map, char* key, Value value)     - Inserts or replaces a key/value pair and returns its index
  void     SMAP_PUT_STRUCT      (T* map, T entry)                    - Inserts or replaces an entire entry

  // void     SMAP_PPUT            (T** map, T* entry)                  - Inserts pointer to entry using entry->key

  size_t   SMAP_GET_IDX         (T* map, char* key)                  - Returns index of key
  // size_t   SMAP_PGET_IDX        (T** map, char* key)                 - Returns index of pointer entry

  T*       SMAP_GET_PTR         (T* map, char* key)                  - Returns pointer to entry
  T*       SMAP_GET_PTR_NULL    (T* map, char* key)                  - Returns NULL if key is not found

  Value    SMAP_GET             (T* map, char* key)                  - Returns value associated with key
  T        SMAP_GET_STRUCT      (T* map, char* key)                  - Returns entire entry
  T*       SMAP_PGET            (T** map, char* key)                 - Returns pointer entry

  bool     SMAP_DELETE          (T* map, char* key)                  - Removes key from map
  bool     SMAP_PDELETE         (T** map, char* key)                 - Removes pointer entry

  void     SMAP_DEFAULT         (T* map, Value value)                - Sets default value returned for missing keys
  void     SMAP_DEFAULT_STRUCT  (T* map, T entry)                    - Sets default entry returned for missing keys

  size_t   SMAP_LEN             (T* map)                             - Returns number of entries in map
  void     SMAP_FREE            (T* map)                             - Frees map memory and sets map pointer to NULL

*/

#ifndef MAP_H
#define MAP_H

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include <stddef.h>
#include <string.h>

//***************************************************************************
//         PUBLIC 
//***************************************************************************

typedef struct SMapStringArena SMapStringArena;

extern void     map_rand_seed     (size_t seed);
extern size_t   map_hash_bytes    (void *p, size_t len, size_t seed);
extern size_t   map_hash_string   (char *str, size_t seed);

extern void     map_unit_tests    (void);

extern char*    smap_arena_alloc  (SMapStringArena *a, char *str);
extern void     smap_arena_reset  (SMapStringArena *a);

//***************************************************************************
//          HIDDEN
//***************************************************************************

typedef struct
{
  size_t     length;
  size_t     capacity;
  void*      hash_table;
  ptrdiff_t  temp;

} __MapHeader;

typedef struct SMapStringBlock SMapStringBlock;
struct SMapStringBlock
{
    SMapStringBlock* next;
    char storage[8];
};

struct SMapStringArena
{
    SMapStringBlock *storage;
    size_t remaining;
    unsigned char block;
    unsigned char mode;  // this isn't used by the string arena itself
};

enum
{
   SMAP_NONE,
   SMAP_DEFAULT,
   SMAP_STRDUP,
   SMAP_ARENA
};

extern void*   __map_grow          (void *map, size_t item_size, size_t add_len, size_t min_cap);
extern void    __map_free          (void *p,   size_t item_size);
extern void*   __map_get_key       (void *map, size_t item_size, void *key, size_t key_size, int mode);
extern void*   __map_get_key_ts    (void *map, size_t item_size, void *key, size_t key_size, ptrdiff_t *temp, int mode);
extern void*   __map_put_default   (void *map, size_t item_size);
extern void*   __map_put_key       (void *map, size_t item_size, void *key, size_t key_size, int mode);
extern void*   __map_del_key       (void *map, size_t item_size, void *key, size_t key_size, size_t key_offset, int mode);

extern void*   __smap_mode         (size_t item_size, int mode);


#ifdef _MSC_VER
#define __MAP_NOTUSED(v)  (void)(v)
#else
#define __MAP_NOTUSED(v)  (void)sizeof(v)
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define __MAP_HAS_TYPEOF
    #ifdef __cplusplus
        #define __MAP_HAS_LITERAL_ARRAY  // this is currently broken for clang
    #endif
#endif

#if !defined(__cplusplus)
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
        #define __MAP_HAS_LITERAL_ARRAY
    #endif
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define __MAP_HAS_TYPEOF
    #ifdef __cplusplus
        #define __MAP_HAS_LITERAL_ARRAY  // this is currently broken for clang
    #endif
#endif

#if !defined(__cplusplus)
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
        #define __MAP_HAS_LITERAL_ARRAY
    #endif
#endif

// this macro takes the address of the argument, but on gcc/clang can accept rvalues
#if defined(__MAP_HAS_LITERAL_ARRAY) && defined(__MAP_HAS_TYPEOF)
    #if __clang__
        #define __MAP_ADDRESSOF(typevar, value) ((__typeof__(typevar)[1]){value}) // literal array decays to pointer to value
    #else
        #define __MAP_ADDRESSOF(typevar, value) ((typeof(typevar)[1]){value})     // literal array decays to pointer to value
    #endif
#else
    #define __MAP_ADDRESSOF(typevar, value) &(value)
#endif

#define __MAP_OFFSETOF(var,field) ((char *) &(var)->field - (char *) (var))

#define __MAP_HEADER(m)    ((__MapHeader *) (m) - 1)
#define __MAP_TEMP(m)      __MAP_HEADER(m)->temp
#define __MAP_TEMP_KEY(m)  (*(char **) __MAP_HEADER(m)->hash_table)
#define __MAP_LEN(m)       ((m) ? __MAP_HEADER(m)->length : 0)
#define __MAP_CAP(m)       ((m) ? __MAP_HEADER(m)->capacity : 0)

#define __MAP_HASH_BINARY 0
#define __MAP_HASH_STRING 1

//***************************************************************************
//          PUBLIC MACROS
//***************************************************************************

#define HMAP_LEN(m)                 ((m) ? __MAP_HEADER((m)-1)->length-1 : 0)

#define HMAP_PUT(m, k, v)           ((m) = __map_put_key((m), sizeof *(m), (void*) __MAP_ADDRESSOF((m)->key, (k)), sizeof (m)->key, 0), (m)[__MAP_TEMP((m)-1)].key = (k), (m)[__MAP_TEMP((m)-1)].value = (v))
#define HMAP_PUT_STRUCT(m, s)       ((m) = __map_put_key((m), sizeof *(m), &(s).key, sizeof (s).key, __MAP_HASH_BINARY), (m)[__MAP_TEMP((m)-1)] = (s))

#define HMAP_GET_IDX(m,k)           ((m) = __map_get_key((m), sizeof *(m), (void*) __MAP_ADDRESSOF((m)->key, (k)), sizeof (m)->key, __MAP_HASH_BINARY), __MAP_TEMP((m)-1))
#define HMAP_GET_IDX_TS(m,k,temp)   ((m) = __map_get_key_ts((m), sizeof *(m), (void*) __MAP_ADDRESSOF((m)->key, (k)), sizeof (m)->key, &(temp), __MAP_HASH_BINARY), (temp))

#define HMAP_GET_PTR(m, k)          ((void) HMAP_GET_IDX(m,k), &(m)[__MAP_TEMP((m)-1)])
#define HMAP_GET_PTR_TS(m, k, temp) ((void) HMAP_GET_IDX_TS(m,k,temp), &(m)[temp])

#define HMAP_GET(m, k)              (HMAP_GET_PTR(m,k)->value)
#define HMAP_GET_TS(m, k, temp)     (HMAP_GET_PTR_TS(m,k,temp)->value)
#define HMAP_GET_STRUCT(m, k)       (*HMAP_GET_PTR(m,k))

#define HMAP_DELETE(m,k)            (((m) = __map_del_key((m),sizeof *(m), (void*) __MAP_ADDRESSOF((m)->key, (k)), sizeof (m)->key, __MAP_OFFSETOF((m),key), __MAP_HASH_BINARY)),(m)?__MAP_TEMP((m)-1):0)

#define HMAP_DEFAULT(m,v)           ((m) = __map_put_default((m), sizeof *(m)), (m)[-1].value = (v))
#define HMAP_DEFAULT_STRUCT(m,s)    ((m) = __map_put_default((m), sizeof *(m)), (m)[-1] = (s))

#define HMAP_FREE(p)                ((void) ((p) != NULL ? __map_free((p)-1,sizeof*(p)),0 : 0),(p)=NULL)
#define HMAP_GET_PTR_NULL(m,k)      (HMAP_GET_IDX(m,k) == -1 ? NULL : &(m)[__MAP_TEMP((m)-1)])


//***************************************************************************

#define SMAP_PUT(m, k, v)           ((m) = __map_put_key((m), sizeof *(m), (void*) (k), sizeof (m)->key, __MAP_HASH_STRING), (m)[__MAP_TEMP((m)-1)].value = (v))
#define SMAP_PUTI(m, k, v)          ((m) = __map_put_key((m), sizeof *(m), (void*) (k), sizeof (m)->key, __MAP_HASH_STRING), (m)[__MAP_TEMP((m)-1)].value = (v), __MAP_TEMP((m)-1))
#define SMAP_PUT_STRUCT(m, s)       ((m) = __map_put_key((m), sizeof *(m), (void*) (s).key, sizeof (s).key, __MAP_HASH_STRING), (m)[__MAP_TEMP((m)-1)] = (s), (m)[__MAP_TEMP((m)-1)].key = __MAP_TEMP_KEY((m)-1)) 

#define SMAP_PPUT(m, p)             ((m) = __map_put_key((m), sizeof *(m), (void*) (p)->key, sizeof (p)->key, __MAP_HASH_PTR_TO_STRING), (m)[__MAP_TEMP((m)-1)] = (p))

#define SMAP_GET_IDX(m,k)           ((m) = __map_get_key((m), sizeof *(m), (void*) (k), sizeof (m)->key, __MAP_HASH_STRING), __MAP_TEMP((m)-1))
#define SMAP_PGET_IDX(m,k)          ((m) = __map_get_key((m), sizeof *(m), (void*) (k), sizeof (*(m))->key, __MAP_HASH_PTR_TO_STRING), __MAP_TEMP((m)-1))

#define SMAP_GET_PTR(m, k)          ((void) SMAP_GET_IDX(m,k), &(m)[__MAP_TEMP((m)-1)])

#define SMAP_PGET(m, k)             ((void) SMAP_PGET_IDX(m,k), (m)[__MAP_TEMP((m)-1)])

#define SMAP_DELETE(m,k)            (((m) = __map_del_key((m),sizeof *(m), (void*) (k), sizeof (m)->key, __MAP_OFFSETOF((m),key), __MAP_HASH_STRING)),(m)?__MAP_TEMP((m)-1):0)
#define SMAP_PDELETE(m,k)           (((m) = __map_del_key((m),sizeof *(m), (void*) (k), sizeof (*(m))->key, __MAP_OFFSETOF(*(m),key), __MAP_HASH_PTR_TO_STRING)),(m)?__MAP_TEMP((m)-1):0)

#define SMAP_NEW_ARENA(m)           ((m) = __smap_mode(sizeof *(m), SMAP_ARENA))
#define SMAP_NEW_STRDUP(m)          ((m) = __smap_mode(sizeof *(m), SMAP_STRDUP))

#define SMAP_DEFAULT(m, v)          HMAP_DEFAULT(m,v)
#define SMAP_DEFAULT_STRUCT(m, s)   HMAP_DEFAULT_STRUCT(m,s)

#define SMAP_FREE                   HMAP_FREE
#define SMAP_LEN                    HMAP_LEN

#define SMAP_GET_STRUCT(m, k)       (*SMAP_GET_PTR(m,k))
#define SMAP_GET(m, k)              (SMAP_GET_PTR(m,k)->value)
#define SMAP_GET_PTR_NULL(m,k)      (SMAP_GET_IDX(m,k) == -1 ? NULL : &(m)[__MAP_TEMP((m)-1)])

#endif // HASHMAP_H

//***************************************************************************
//          FUNCTION IMPLEMENTATIONS
//***************************************************************************

#ifdef MAP_IMPLEMENTATION

#include <assert.h>
#include <string.h>

#if defined(MAP_REALLOC) && !defined(MAP_FREE) || !defined(MAP_REALLOC) && defined(MAP_FREE)
    #error "You must define both MAP_REALLOC and MAP_FREE, or neither."
#endif
#if !defined(MAP_REALLOC) && !defined(MAP_FREE)
    #include <stdlib.h>
    #define MAP_REALLOC(c,p,s) realloc(p,s)
    #define MAP_FREE(c,p)      free(p)
#endif

#ifndef MAP_ASSERT
    #define MAP_ASSERT_WAS_UNDEFINED
    #define MAP_ASSERT(x)   ((void) 0)
#endif

#ifdef MAP_STATISTICS
    #define __MAP_STATS(x)   x
    size_t map_grow_count;
    size_t map_shrink_count;
    size_t map_rebuild_count;
    size_t map_probes_count;
    size_t map_alloc_count;
    size_t map_rehash_probes_count;
    size_t map_rehash_items_count;
#else
    #define __MAP_STATS(x)
#endif

void* __map_array_grow(void *a, size_t item_size, size_t add_len, size_t min_cap)
{
  void *b;
  size_t min_len = __MAP_LEN(a) + add_len;

  // compute the minimum capacity needed
  if (min_len > min_cap)
    min_cap = min_len;

  if (min_cap <= __MAP_CAP(a))
    return a;

  // increase needed capacity to guarantee O(1) amortized
  if (min_cap < 2 * __MAP_CAP(a))
    min_cap = 2 * __MAP_CAP(a);
  else if (min_cap < 4)
    min_cap = 4;

  b = MAP_REALLOC(NULL, (a) ? __MAP_HEADER(a) : 0, item_size * min_cap + sizeof(__MapHeader));
  //if (num_prev < 65536) prev_allocs[num_prev++] = (int *) (char *) b;
  b = (char *) b + sizeof(__MapHeader);
  if (a == NULL) {
    __MAP_HEADER(b)->length = 0;
    __MAP_HEADER(b)->hash_table = 0;
    __MAP_HEADER(b)->temp = 0;
  } else {
    __MAP_STATS(map_grow_count++);
  }
  __MAP_HEADER(b)->capacity = min_cap;

  return b;
}


#ifdef MAP_INTERNAL_SMALL_BUCKET
    #define __MAP_BUCKET_LENGTH      4
#else
    #define __MAP_BUCKET_LENGTH      8
#endif

#define __MAP_BUCKET_SHIFT      (__MAP_BUCKET_LENGTH == 8 ? 3 : 2)
#define __MAP_BUCKET_MASK       (__MAP_BUCKET_LENGTH-1)
#define __MAP_CACHE_LINE_SIZE   64

#define __MAP_ALIGN_FWD(n,a)   (((n) + (a) - 1) & ~((a)-1))

typedef struct
{
   size_t    hash [__MAP_BUCKET_LENGTH];
   ptrdiff_t index[__MAP_BUCKET_LENGTH];

} __MapHashBucket; // in 32-bit, this is one 64-byte cache line; in 64-bit, each array is one 64-byte cache line

typedef struct
{
  char * temp_key; // this MUST be the first field of the hash table
  size_t slot_count;
  size_t used_count;
  size_t used_count_threshold;
  size_t used_count_shrink_threshold;
  size_t tombstone_count;
  size_t tombstone_count_threshold;
  size_t seed;
  size_t slot_count_log2;
  SMapStringArena string;
  __MapHashBucket *storage; // not a separate allocation, just 64-byte aligned storage after this struct
                            
} __MapHashIndex;

#define __MAP_INDEX_EMPTY    -1
#define __MAP_INDEX_DELETED  -2
#define __MAP_INDEX_IN_USE(x)  ((x) >= 0)

#define __MAP_HASH_EMPTY      0
#define __MAP_HASH_DELETED    1

static size_t map_hash_seed=0x31415926;

void map_rand_seed(size_t seed)
{
    map_hash_seed = seed;
}

#define __MAP_LOAD_32_OR_64(var, temp, v32, v64_hi, v64_lo)                                          \
  temp = v64_lo ^ v32, temp <<= 16, temp <<= 16, temp >>= 16, temp >>= 16, /* discard if 32-bit */   \
  var = v64_hi, var <<= 16, var <<= 16,                                    /* discard if 32-bit */   \
  var ^= temp ^ v32

#define __MAP_SIZE_T_BITS ((sizeof (size_t)) * 8)

static size_t __map_probe_position(size_t hash, size_t slot_count, size_t slot_log2)
{
    size_t pos;
    __MAP_NOTUSED(slot_log2);
    pos = hash & (slot_count-1);

#ifdef MAP_INTERNAL_BUCKET_START
    pos &= ~__MAP_BUCKET_MASK;
#endif

    return pos;
}

static size_t __map_log2(size_t slot_count)
{
    size_t n=0;
    while (slot_count > 1) 
    {
        slot_count >>= 1;
        ++n;
    }
    return n;
}

static __MapHashIndex* __map_make_hash_index(size_t slot_count, __MapHashIndex *ot)
{
    __MapHashIndex *t;
    t = (__MapHashIndex *) MAP_REALLOC(NULL,0,(slot_count >> __MAP_BUCKET_SHIFT) * sizeof(__MapHashBucket) + sizeof(__MapHashIndex) + __MAP_CACHE_LINE_SIZE-1);
    t->storage = (__MapHashBucket *) __MAP_ALIGN_FWD((size_t) (t+1), __MAP_CACHE_LINE_SIZE);
    t->slot_count = slot_count;
    t->slot_count_log2 = __map_log2(slot_count);
    t->tombstone_count = 0;
    t->used_count = 0;

    // compute without overflowing
    t->used_count_threshold        = slot_count - (slot_count>>2);
    t->tombstone_count_threshold   = (slot_count>>3) + (slot_count>>4);
    t->used_count_shrink_threshold = slot_count >> 2;

    if (slot_count <= __MAP_BUCKET_LENGTH)
        t->used_count_shrink_threshold = 0;

    // to avoid infinite loop, we need to guarantee that at least one slot is empty and will terminate probes
    MAP_ASSERT(t->used_count_threshold + t->tombstone_count_threshold < t->slot_count);
    __MAP_STATS(map_hash_alloc_count++);
    
    if (ot) 
    {
        t->string = ot->string;
        // reuse old seed so we can reuse old hashes so below "copy out old data" doesn't do any hashing
        t->seed = ot->seed;
    } 
    else 
    {
        size_t a,b,temp;
        memset(&t->string, 0, sizeof(t->string));
        t->seed = map_hash_seed;
        __MAP_LOAD_32_OR_64(a,temp, 2147001325, 0x27bb2ee6, 0x87b0b0fd);
        __MAP_LOAD_32_OR_64(b,temp,  715136305,          0, 0xb504f32d);
        map_hash_seed = map_hash_seed * a + b;
    }

    {
        size_t i,j;
        for (i=0; i < slot_count >> __MAP_BUCKET_SHIFT; ++i) 
        {
            __MapHashBucket *b = &t->storage[i];
            for (j=0; j < __MAP_BUCKET_LENGTH; ++j)
                b->hash[j] = __MAP_HASH_EMPTY;
            for (j=0; j < __MAP_BUCKET_LENGTH; ++j)
                b->index[j] = __MAP_INDEX_EMPTY;
        }
    }

    // copy out the old data, if any
    if (ot) 
    {
        size_t i,j;
        t->used_count = ot->used_count;
        for (i=0; i < ot->slot_count >> __MAP_BUCKET_SHIFT; ++i) 
        {
            __MapHashBucket *ob = &ot->storage[i];

            for (j=0; j < __MAP_BUCKET_LENGTH; ++j) 
            {
                if (__MAP_INDEX_IN_USE(ob->index[j])) 
                {
                    size_t hash = ob->hash[j];
                    size_t pos = __map_probe_position(hash, t->slot_count, t->slot_count_log2);
                    size_t step = __MAP_BUCKET_LENGTH;
                    __MAP_STATS(rehash_items_count++);

                    for (;;) 
                    {
                        size_t limit,z;
                        __MapHashBucket *bucket;
                        bucket = &t->storage[pos >> __MAP_BUCKET_SHIFT];
                        __MAP_STATS(rehash_probes_count++);

                        for (z=pos & __MAP_BUCKET_MASK; z < __MAP_BUCKET_LENGTH; ++z) 
                        {
                            if (bucket->hash[z] == 0) 
                            {
                                bucket->hash[z] = hash;
                                bucket->index[z] = ob->index[j];
                                goto done;
                            }
                        }

                        limit = pos & __MAP_BUCKET_MASK;

                        for (z = 0; z < limit; ++z) 
                        {
                            if (bucket->hash[z] == 0) 
                            {
                                bucket->hash[z] = hash;
                                bucket->index[z] = ob->index[j];
                                goto done;
                            }
                        }

                        pos += step;                  // quadratic probing
                        step += __MAP_BUCKET_LENGTH;
                        pos &= (t->slot_count-1);
                    }
                }
                done:
                ;
            }
        }
    }

  return t;
}

#define SIP_ROTATE_LEFT(val, n)   (((val) << (n)) | ((val) >> (__MAP_SIZE_T_BITS - (n))))
#define SIP_ROTATE_RIGHT(val, n)  (((val) >> (n)) | ((val) << (__MAP_SIZE_T_BITS - (n))))

size_t map_hash_string(char *str, size_t seed)
{
    size_t hash = seed;
    while (*str)
        hash = SIP_ROTATE_LEFT(hash, 9) + (unsigned char) *str++;

    // Thomas Wang 64-to-32 bit mix function, hopefully also works in 32 bits
    hash ^= seed;
    hash = (~hash) + (hash << 18);
    hash ^= hash ^ SIP_ROTATE_RIGHT(hash,31);
    hash = hash * 21;
    hash ^= hash ^ SIP_ROTATE_RIGHT(hash,11);
    hash += (hash << 6);
    hash ^= SIP_ROTATE_RIGHT(hash,22);

    return hash+seed;
}

#ifdef MAP_SIPHASH_2_4
    #define MAP_SIPHASH_C_ROUNDS 2
    #define MAP_SIPHASH_D_ROUNDS 4
    typedef int MAP_SIPHASH_2_4_can_only_be_used_in_64_bit_builds[sizeof(size_t) == 8 ? 1 : -1];
#endif

#ifndef MAP_SIPHASH_C_ROUNDS
    #define MAP_SIPHASH_C_ROUNDS 1
#endif
#ifndef MAP_SIPHASH_D_ROUNDS
    #define MAP_SIPHASH_D_ROUNDS 1
#endif

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable:4127) // conditional expression is constant, for do..while(0) and sizeof()==
#endif

static size_t map_siphash_bytes(void *p, size_t len, size_t seed)
{
  unsigned char *d = (unsigned char *) p;
  size_t i,j;
  size_t v0,v1,v2,v3, data;

  // hash that works on 32- or 64-bit registers without knowing which we have
  // (computes different results on 32-bit and 64-bit platform)
  // derived from siphash, but on 32-bit platforms very different as it uses 4 32-bit state not 4 64-bit
  v0 = ((((size_t) 0x736f6d65 << 16) << 16) + 0x70736575) ^  seed;
  v1 = ((((size_t) 0x646f7261 << 16) << 16) + 0x6e646f6d) ^ ~seed;
  v2 = ((((size_t) 0x6c796765 << 16) << 16) + 0x6e657261) ^  seed;
  v3 = ((((size_t) 0x74656462 << 16) << 16) + 0x79746573) ^ ~seed;

  #define SIPROUND() \
    do {                   \
      v0 += v1; v1 = SIP_ROTATE_LEFT(v1, 13);  v1 ^= v0; v0 = SIP_ROTATE_LEFT(v0,__MAP_SIZE_T_BITS/2);   \
      v2 += v3; v3 = SIP_ROTATE_LEFT(v3, 16);  v3 ^= v2;                                                 \
      v2 += v1; v1 = SIP_ROTATE_LEFT(v1, 17);  v1 ^= v2; v2 = SIP_ROTATE_LEFT(v2,__MAP_SIZE_T_BITS/2);   \
      v0 += v3; v3 = SIP_ROTATE_LEFT(v3, 21);  v3 ^= v0;                                                 \
    } while (0)

  for (i=0; i+sizeof(size_t) <= len; i += sizeof(size_t), d += sizeof(size_t)) {
    data = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
    data |= (size_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // discarded if size_t == 4

    v3 ^= data;
    for (j=0; j < MAP_SIPHASH_C_ROUNDS; ++j)
      SIPROUND();
    v0 ^= data;
  }
  data = len << (__MAP_SIZE_T_BITS-8);
  switch (len - i) {
    case 7: data |= ((size_t) d[6] << 24) << 24; // fall through
    case 6: data |= ((size_t) d[5] << 20) << 20; // fall through
    case 5: data |= ((size_t) d[4] << 16) << 16; // fall through
    case 4: data |= (d[3] << 24); // fall through
    case 3: data |= (d[2] << 16); // fall through
    case 2: data |= (d[1] << 8); // fall through
    case 1: data |= d[0]; // fall through
    case 0: break;
  }
  v3 ^= data;
  for (j=0; j < MAP_SIPHASH_C_ROUNDS; ++j)
    SIPROUND();
  v0 ^= data;
  v2 ^= 0xff;
  for (j=0; j < MAP_SIPHASH_D_ROUNDS; ++j)
    SIPROUND();

  return v1^v2^v3; // slightly stronger since v0^v3 in above cancels out final round operation? I tweeted at the authors of SipHash about this but they didn't reply
}

size_t map_hash_bytes(void *p, size_t len, size_t seed)
{
    unsigned char *d = (unsigned char *) p;

    if (len == 4) 
    {
        unsigned int hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);

        // HASH32-BB  Bob Jenkin's presumably-accidental version of Thomas Wang hash with rotates turned into shifts.
        // Note that converting these back to rotates makes it run a lot slower, presumably due to collisions, so I'm
        // not really sure what's going on.

        hash ^= seed;
        hash = (hash ^ 61) ^ (hash >> 16);
        hash = hash + (hash << 3);
        hash = hash ^ (hash >> 4);
        hash = hash * 0x27d4eb2d;
        hash ^= seed;
        hash = hash ^ (hash >> 15);
        return (((size_t) hash << 16 << 16) | hash) ^ seed;
    } 
    else if (len == 8 && sizeof(size_t) == 8) 
    {
        size_t hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
        hash |= (size_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // avoid warning if size_t == 4
        hash ^= seed;
        hash = (~hash) + (hash << 21);
        hash ^= SIP_ROTATE_RIGHT(hash,24);
        hash *= 265;
        hash ^= SIP_ROTATE_RIGHT(hash,14);
        hash ^= seed;
        hash *= 21;
        hash ^= SIP_ROTATE_RIGHT(hash,28);
        hash += (hash << 31);
        hash = (~hash) + (hash << 18);
        return hash;
    } 
    else 
    {
        return map_siphash_bytes(p,len,seed);
    }
}

#ifdef _MSC_VER
    #pragma warning(pop)
#endif

// size_t map_hash_bytes(void *p, size_t len, size_t seed)
// {
//     unsigned char *d = (unsigned char *) p;
//
//     if (len == 4) 
//     {
//         unsigned int hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
//         hash ^= seed;
//         hash = (hash ^ 61) ^ (hash >> 16);
//         hash = hash + (hash << 3);
//         hash = hash ^ (hash >> 4);
//         hash = hash * 0x27d4eb2d;
//         hash ^= seed;
//         hash = hash ^ (hash >> 15);
//
//         return (((size_t) hash << 16 << 16) | hash) ^ seed;
//     } 
//     else if (len == 8 && sizeof(size_t) == 8) 
//     {
//         size_t hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
//         hash |= (size_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // avoid warning if size_t == 4
//         hash ^= seed;
//         hash = (~hash) + (hash << 21);
//         hash ^= SIP_ROTATE_RIGHT(hash,24);
//         hash *= 265;
//         hash ^= SIP_ROTATE_RIGHT(hash,14);
//         hash ^= seed;
//         hash *= 21;
//         hash ^= SIP_ROTATE_RIGHT(hash,28);
//         hash += (hash << 31);
//         hash = (~hash) + (hash << 18);
//         return hash;
//     } 
//     else 
//     {
//         return map_siphash_bytes(p,len,seed);
//     }
// }

#ifdef _MSC_VER
    #pragma warning(pop)
#endif


static int __map_is_key_equal(void *map, size_t item_size, void *key, size_t key_size, size_t key_offset, int mode, size_t i)
{
  if (mode >= __MAP_HASH_STRING)
    return 0==strcmp((char *) key, * (char **) ((char *) map + item_size*i + key_offset));
  else
    return 0==memcmp(key, (char *) map + item_size*i + key_offset, key_size);
}

#define __MAP_HASH_TO_ARR(x,item_size) ((char*) (x) - (item_size))
#define __MAP_ARR_TO_HASH(x,item_size) ((char*) (x) + (item_size))

#define __MAP_HASH_TABLE(m)  ((__MapHashIndex *) __MAP_HEADER(m)->hash_table)

void __map_free(void *map, size_t item_size)
{
    if (map == NULL) return;
    if (__MAP_HASH_TABLE(map) != NULL) 
    {
        if (__MAP_HASH_TABLE(map)->string.mode == SMAP_STRDUP) 
        {
            size_t i;
            // skip 0th element, which is default
            for (i=1; i < __MAP_HEADER(map)->length; ++i)
                MAP_FREE(NULL, *(char**) ((char *) map + item_size*i));
        }
        smap_arena_reset(&__MAP_HASH_TABLE(map)->string);
    }
    MAP_FREE(NULL, __MAP_HEADER(map)->hash_table);
    MAP_FREE(NULL, __MAP_HEADER(map));
}

static ptrdiff_t __map_find_slot(void *map, size_t item_size, void *key, size_t key_size, size_t key_offset, int mode)
{
    void *raw_a = __MAP_HASH_TO_ARR(map,item_size);
    __MapHashIndex *table = __MAP_HASH_TABLE(raw_a);
    size_t hash = mode >= __MAP_HASH_STRING ? map_hash_string((char*)key,table->seed) : map_hash_bytes(key, key_size,table->seed);
    size_t step = __MAP_BUCKET_LENGTH;
    size_t limit,i;
    size_t pos;
    __MapHashBucket *bucket;

    if (hash < 2) hash += 2; // stored hash values are forbidden from being 0, so we can detect empty slots

    pos = __map_probe_position(hash, table->slot_count, table->slot_count_log2);

    for (;;) 
    {
        __MAP_STATS(map_hash_probes_count++);
        bucket = &table->storage[pos >> __MAP_BUCKET_SHIFT];

        // start searching from pos to end of bucket, this should help performance on small hash tables that fit in cache
        for (i=pos & __MAP_BUCKET_MASK; i < __MAP_BUCKET_LENGTH; ++i) 
        {
            if (bucket->hash[i] == hash) 
            {
                if (__map_is_key_equal(map, item_size, key, key_size, key_offset, mode, bucket->index[i])) 
                {
                    return (pos & ~__MAP_BUCKET_MASK)+i;
                }
            } 
            else if (bucket->hash[i] == __MAP_HASH_EMPTY) 
            {
                return -1;
            }
        }

        // search from beginning of bucket to pos
        limit = pos & __MAP_BUCKET_MASK;
        for (i = 0; i < limit; ++i) 
        {
            if (bucket->hash[i] == hash) 
            {
                if (__map_is_key_equal(map, item_size, key, key_size, key_offset, mode, bucket->index[i])) 
                {
                    return (pos & ~__MAP_BUCKET_MASK)+i;
                }
            } 
            else if (bucket->hash[i] == __MAP_HASH_EMPTY) 
            {
                return -1;
            }
        }

        // quadratic probing
        pos += step;
        step += __MAP_BUCKET_LENGTH;
        pos &= (table->slot_count-1);
    }
  /* NOTREACHED */
}

void* __map_get_key_ts(void *map, size_t item_size, void *key, size_t key_size, ptrdiff_t *temp, int mode)
{
    size_t key_offset = 0;
    if (map == NULL) 
    {
        // make it non-empty so we can return map temp
        map = __map_array_grow(0, item_size, 0, 1);
        __MAP_HEADER(map)->length += 1;
        memset(map, 0, item_size);
        *temp = __MAP_INDEX_EMPTY;
        // adjust map to point after the default element
        return __MAP_ARR_TO_HASH(map,item_size);
    } 
    else 
    {
        __MapHashIndex *table;
        void *raw_a = __MAP_HASH_TO_ARR(map,item_size);
        // adjust map to point to the default element
        table = (__MapHashIndex *) __MAP_HEADER(raw_a)->hash_table;
        if (table == 0) 
        {
        *temp = -1;
        } 
        else 
        {
            ptrdiff_t slot = __map_find_slot(map, item_size, key, key_size, key_offset, mode);
            if (slot < 0) 
            {
                *temp = __MAP_INDEX_EMPTY;
            } 
            else 
            {
                __MapHashBucket *b = &table->storage[slot >> __MAP_BUCKET_SHIFT];
                *temp = b->index[slot & __MAP_BUCKET_MASK];
            }
        }
        return map;
    }
}

void* __map_get_key(void *map, size_t item_size, void *key, size_t key_size, int mode)
{
    ptrdiff_t temp;
    void *p = __map_get_key_ts(map, item_size, key, key_size, &temp, mode);
    __MAP_TEMP(__MAP_HASH_TO_ARR(p,item_size)) = temp;
    return p;
}

void * __map_put_default(void *map, size_t item_size)
{
    // three cases:
    //   map is NULL <- allocate
    //   map has map hash table but no entries, because of shmode <- grow
    //   map has entries <- do nothing
    if (map == NULL || __MAP_HEADER(__MAP_HASH_TO_ARR(map,item_size))->length == 0) 
    {
        map = __map_array_grow(map ? __MAP_HASH_TO_ARR(map,item_size) : NULL, item_size, 0, 1);
        __MAP_HEADER(map)->length += 1;
        memset(map, 0, item_size);
        map=__MAP_ARR_TO_HASH(map,item_size);
    }
    return map;
}

static char* smap_strdup(char *str);

void* __map_put_key(void *map, size_t item_size, void *key, size_t key_size, int mode)
{
    size_t key_offset=0;
    void *raw_a;
    __MapHashIndex *table;

    if (map == NULL) 
    {
        map = __map_array_grow(0, item_size, 0, 1);
        memset(map, 0, item_size);
        __MAP_HEADER(map)->length += 1;
        // adjust map to point AFTER the default element
        map = __MAP_ARR_TO_HASH(map,item_size);
    }

    // adjust map to point to the default element
    raw_a = map;
    map = __MAP_HASH_TO_ARR(map,item_size);

    table = (__MapHashIndex *) __MAP_HEADER(map)->hash_table;

    if (table == NULL || table->used_count >= table->used_count_threshold) 
    {
        __MapHashIndex *nt;
        size_t slot_count;

        slot_count = (table == NULL) ? __MAP_BUCKET_LENGTH : table->slot_count*2;
        nt = __map_make_hash_index(slot_count, table);
        if (table)
            MAP_FREE(NULL, table);
        else
            nt->string.mode = mode >= __MAP_HASH_STRING ? SMAP_DEFAULT : 0;
        __MAP_HEADER(map)->hash_table = table = nt;
        __MAP_STATS(map_grow_count++);
    }

    // we iterate hash table explicitly because we want to track if we saw map tombstone
    {
        size_t hash = mode >= __MAP_HASH_STRING ? map_hash_string((char*)key,table->seed) : map_hash_bytes(key, key_size,table->seed);
        size_t step = __MAP_BUCKET_LENGTH;
        size_t pos;
        ptrdiff_t tombstone = -1;
        __MapHashBucket* bucket;

        // stored hash values are forbidden from being 0, so we can detect empty slots to early out quickly
        if (hash < 2) hash += 2;

        pos = __map_probe_position(hash, table->slot_count, table->slot_count_log2);

        for (;;) 
        {
            size_t limit, i;
            __MAP_STATS(map_hash_probes_count++);
            bucket = &table->storage[pos >> __MAP_BUCKET_SHIFT];

            // start searching from pos to end of bucket
            for (i=pos & __MAP_BUCKET_MASK; i < __MAP_BUCKET_LENGTH; ++i) 
            {
                if (bucket->hash[i] == hash) 
                {
                    if (__map_is_key_equal(raw_a, item_size, key, key_size, key_offset, mode, bucket->index[i])) 
                    {
                        __MAP_TEMP(map) = bucket->index[i];
                        if (mode >= __MAP_HASH_STRING)
                        __MAP_TEMP_KEY(map) = * (char **) ((char *) raw_a + item_size*bucket->index[i] + key_offset);
                        return __MAP_ARR_TO_HASH(map,item_size);
                    }
                } 
                else if (bucket->hash[i] == 0) 
                {
                    pos = (pos & ~__MAP_BUCKET_MASK) + i;
                    goto found_empty_slot;
                } 
                else if (tombstone < 0) 
                {
                    if (bucket->index[i] == __MAP_INDEX_DELETED)
                    tombstone = (ptrdiff_t) ((pos & ~__MAP_BUCKET_MASK) + i);
                }
            }

            // search from beginning of bucket to pos
            limit = pos & __MAP_BUCKET_MASK;
            for (i = 0; i < limit; ++i) 
            {
                if (bucket->hash[i] == hash) 
                {
                    if (__map_is_key_equal(raw_a, item_size, key, key_size, key_offset, mode, bucket->index[i])) 
                    {
                        __MAP_TEMP(map) = bucket->index[i];
                        return __MAP_ARR_TO_HASH(map,item_size);
                    }
                } 
                else if (bucket->hash[i] == 0) 
                {
                    pos = (pos & ~__MAP_BUCKET_MASK) + i;
                    goto found_empty_slot;
                } 
                else if (tombstone < 0) 
                {
                    if (bucket->index[i] == __MAP_INDEX_DELETED)
                    tombstone = (ptrdiff_t) ((pos & ~__MAP_BUCKET_MASK) + i);
                }
            }

            // quadratic probing
            pos += step;
            step += __MAP_BUCKET_LENGTH;
            pos &= (table->slot_count-1);
        }
        found_empty_slot:
        if (tombstone >= 0) 
        {
            pos = tombstone;
            --table->tombstone_count;
        }
        ++table->used_count;

        {
            ptrdiff_t i = (ptrdiff_t) __MAP_LEN(map);
            if ((size_t) i+1 > __MAP_CAP(map))
                *(void **) &map = __map_array_grow(map, item_size, 1, 0);
            raw_a = __MAP_ARR_TO_HASH(map,item_size);

            MAP_ASSERT((size_t) i+1 <= __MAP_CAP(map));
            __MAP_HEADER(map)->length = i+1;
            bucket = &table->storage[pos >> __MAP_BUCKET_SHIFT];
            bucket->hash[pos & __MAP_BUCKET_MASK] = hash;
            bucket->index[pos & __MAP_BUCKET_MASK] = i-1;
            __MAP_TEMP(map) = i-1;

            switch (table->string.mode) 
            {
                case SMAP_STRDUP:  __MAP_TEMP_KEY(map) = *(char **) ((char *) map + item_size*i) = smap_strdup((char*) key); break;
                case SMAP_ARENA:   __MAP_TEMP_KEY(map) = *(char **) ((char *) map + item_size*i) = smap_arena_alloc(&table->string, (char*)key); break;
                case SMAP_DEFAULT: __MAP_TEMP_KEY(map) = *(char **) ((char *) map + item_size*i) = (char *) key; break;
                default:                memcpy((char *) map + item_size*i, key, key_size); break;
            }
        }
        return __MAP_ARR_TO_HASH(map,item_size);
    }
}

void* __smap_mode(size_t item_size, int mode)
{
    void *map = __map_array_grow(0, item_size, 0, 1);
    __MapHashIndex *h;
    memset(map, 0, item_size);
    __MAP_HEADER(map)->length = 1;
    __MAP_HEADER(map)->hash_table = h = (__MapHashIndex *) __map_make_hash_index(__MAP_BUCKET_LENGTH, NULL);
    h->string.mode = (unsigned char) mode;
    return __MAP_ARR_TO_HASH(map,item_size);
}

void * __map_del_key(void *map, size_t item_size, void *key, size_t key_size, size_t key_offset, int mode)
{
    if (map == NULL) 
    {
        return 0;
    } 
    else 
    {
        __MapHashIndex *table;
        void *raw_a = __MAP_HASH_TO_ARR(map,item_size);
        table = (__MapHashIndex *) __MAP_HEADER(raw_a)->hash_table;
        __MAP_TEMP(raw_a) = 0;

        if (table == 0) 
        {
            return map;
        } 
        else 
        {
            ptrdiff_t slot;
            slot = __map_find_slot(map, item_size, key, key_size, key_offset, mode);
            if (slot < 0)
                return map;
            else 
            {
                __MapHashBucket *b = &table->storage[slot >> __MAP_BUCKET_SHIFT];
                int i = slot & __MAP_BUCKET_MASK;
                ptrdiff_t old_index = b->index[i];
                ptrdiff_t final_index = (ptrdiff_t) __MAP_LEN(raw_a)-1-1; // minus one for the raw_a vs map, and minus one for 'last'
                MAP_ASSERT(slot < (ptrdiff_t) table->slot_count);
                --table->used_count;
                ++table->tombstone_count;
                __MAP_TEMP(raw_a) = 1;
                MAP_ASSERT(table->used_count >= 0);
                //MAP_ASSERT(table->tombstone_count < table->slot_count/4);
                b->hash[i] = __MAP_HASH_DELETED;
                b->index[i] = __MAP_INDEX_DELETED;

                if (mode == __MAP_HASH_STRING && table->string.mode == SMAP_STRDUP)
                    MAP_FREE(NULL, *(char**) ((char *) map+item_size*old_index));

                // if indices are the same, memcpy is map no-op, but back-pointer-fixup will fail, so skip
                if (old_index != final_index) 
                {
                    // swap delete
                    memmove((char*) map + item_size*old_index, (char*) map + item_size*final_index, item_size);

                    // now find the slot for the last element
                    if (mode == __MAP_HASH_STRING)
                    slot = __map_find_slot(map, item_size, *(char**) ((char *) map+item_size*old_index + key_offset), key_size, key_offset, mode);
                    else
                    slot = __map_find_slot(map, item_size,  (char* ) map+item_size*old_index + key_offset, key_size, key_offset, mode);
                    MAP_ASSERT(slot >= 0);
                    b = &table->storage[slot >> __MAP_BUCKET_SHIFT];
                    i = slot & __MAP_BUCKET_MASK;
                    MAP_ASSERT(b->index[i] == final_index);
                    b->index[i] = old_index;
                }

                __MAP_HEADER(raw_a)->length -= 1;

                if (table->used_count < table->used_count_shrink_threshold && table->slot_count > __MAP_BUCKET_LENGTH) 
                {
                    __MAP_HEADER(raw_a)->hash_table = __map_make_hash_index(table->slot_count>>1, table);
                    MAP_FREE(NULL, table);
                    __MAP_STATS(++map_hash_shrink_count++);
                } 
                else if (table->tombstone_count > table->tombstone_count_threshold) 
                {
                    __MAP_HEADER(raw_a)->hash_table = __map_make_hash_index(table->slot_count   , table);
                    MAP_FREE(NULL, table);
                    __MAP_STATS(++map_hash_rebuild_count);
                }

                return map;
            }
        }
    }
  /* NOTREACHED */
}

static char* smap_strdup(char *str)
{
  // to keep replaceable allocator simple, we don't want to use strdup.
  // rolling our own also avoids problem of strdup vs _strdup
  size_t len = strlen(str)+1;
  char *p = (char*) MAP_REALLOC(NULL, 0, len);
  memmove(p, str, len);
  return p;
}

#ifndef SMAP_STRING_ARENA_BLOCKSIZE_MIN
#define SMAP_STRING_ARENA_BLOCKSIZE_MIN  512u
#endif
#ifndef SMAP_STRING_ARENA_BLOCKSIZE_MAX
#define SMAP_STRING_ARENA_BLOCKSIZE_MAX  (1u<<20)
#endif

char* smap_arena_alloc(SMapStringArena *map, char *str)
{
    char *p;
    size_t len = strlen(str)+1;
    if (len > map->remaining) 
    {
        // compute the next blocksize
        size_t blocksize = map->block;

        // size is 512, 512, 1024, 1024, 2048, 2048, 4096, 4096, etc., so that
        // there are log(SIZE) allocations to free when we destroy the table
        blocksize = (size_t) (SMAP_STRING_ARENA_BLOCKSIZE_MIN) << (blocksize>>1);

        // if size is under 1M, advance to next blocktype
        if (blocksize < (size_t)(SMAP_STRING_ARENA_BLOCKSIZE_MAX))
        ++map->block;

        if (len > blocksize) 
        {
            // if string is larger than blocksize, then just allocate the full size.
            // note that we still advance string_block so block size will continue
            // increasing, so e.g. if somebody only calls this with 1000-long strings,
            // eventually the arena will start doubling and handling those as well
            SMapStringBlock *sb = (SMapStringBlock *) MAP_REALLOC(NULL, 0, sizeof(*sb)-8 + len);
            memmove(sb->storage, str, len);
            if (map->storage) 
            {
                // insert it after the first element, so that we don't waste the space there
                sb->next = map->storage->next;
                map->storage->next = sb;
            } 
            else 
            {
                sb->next = 0;
                map->storage = sb;
                map->remaining = 0; // this is redundant, but good for clarity
            }
            return sb->storage;
        } 
        else 
        {
            SMapStringBlock *sb = (SMapStringBlock *) MAP_REALLOC(NULL, 0, sizeof(*sb)-8 + blocksize);
            sb->next = map->storage;
            map->storage = sb;
            map->remaining = blocksize;
        }
    }

    MAP_ASSERT(len <= map->remaining);
    p = map->storage->storage + map->remaining - len;
    map->remaining -= len;
    memmove(p, str, len);
    return p;
}

void smap_arena_reset(SMapStringArena *map)
{
    SMapStringBlock *x,*y;
    x = map->storage;
    while (x) 
    {
        y = x->next;
        MAP_FREE(NULL, x);
        x = y;
    }
    memset(map, 0, sizeof(*map));
}

#endif


#ifdef MAP_UNIT_TESTS

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#ifndef MAP_ASSERT
    #define MAP_ASSERT assert
    #include <assert.h>
#endif

typedef struct { int key,b,c,d; } test_struct;
typedef struct { int key[2],b,c,d; } test_struct2;

static char buffer[256];
char *strkey(int n)
{
#if defined(_WIN32) && defined(__STDC_WANT_SECURE_LIB__)
    sprintf_s(buffer, sizeof(buffer), "test_%d", n);
#else
    sprintf(buffer, "test_%d", n);
#endif
    return buffer;
}

void map_unit_tests(void)
{
    const int testsize = 100000;
    const int testsize2 = testsize/20;
    struct { int   key;        int value; }  *intmap  = NULL;
    struct { char *key;        int value; }  *strmap  = NULL, s;
    struct { test_struct key;  int value; }  *map     = NULL;
    test_struct                              *map2    = NULL;
    test_struct2                             *map3    = NULL;
    SMapStringArena                           sa      = { 0 };
    int key3[2] = { 1,2 };
    ptrdiff_t temp;

    int i,j;

    i = 1;
    MAP_ASSERT(HMAP_GET_IDX(intmap,i) == -1);
    HMAP_DEFAULT(intmap, -2);
    MAP_ASSERT(HMAP_GET_IDX(intmap, i) == -1);
    MAP_ASSERT(HMAP_GET(intmap, i) == -2);

    for (i=0; i < testsize; i+=2)
        HMAP_PUT(intmap, i, i*5);

    for (i=0; i < testsize; i+=1) 
    {
        if (i & 1) MAP_ASSERT(HMAP_GET(intmap, i) == -2 );
        else       MAP_ASSERT(HMAP_GET(intmap, i) == i*5);

        if (i & 1) MAP_ASSERT(HMAP_GET_TS(intmap, i, temp) == -2 );
        else       MAP_ASSERT(HMAP_GET_TS(intmap, i, temp) == i*5);
    }

    for (i=0; i < testsize; i+=2)
        HMAP_PUT(intmap, i, i*3);

    for (i=0; i < testsize; i+=1)
        if (i & 1) MAP_ASSERT(HMAP_GET(intmap, i) == -2 );
        else       MAP_ASSERT(HMAP_GET(intmap, i) == i*3);
    
    for (i=2; i < testsize; i+=4)
        HMAP_DELETE(intmap, i); // delete half the entries
    
    for (i=0; i < testsize; i+=1)
        if (i & 3) MAP_ASSERT(HMAP_GET(intmap, i) == -2 );
        else       MAP_ASSERT(HMAP_GET(intmap, i) == i*3);

    for (i=0; i < testsize; i+=1)
        HMAP_DELETE(intmap, i); // delete the rest of the entries
                          //
    for (i=0; i < testsize; i+=1)
        MAP_ASSERT(HMAP_GET(intmap, i) == -2 );

    HMAP_FREE(intmap);

    for (i=0; i < testsize; i+=2)
        HMAP_PUT(intmap, i, i*3);
    HMAP_FREE(intmap);

#if defined(__clang__) || defined(__GNUC__)
#ifndef __cplusplus
    intmap = NULL;
    HMAP_PUT(intmap, 15, 7);
    HMAP_PUT(intmap, 11, 3);
    HMAP_PUT(intmap,  9, 5);
    MAP_ASSERT(HMAP_GET(intmap, 9) == 5);
    MAP_ASSERT(HMAP_GET(intmap, 11) == 3);
    MAP_ASSERT(HMAP_GET(intmap, 15) == 7);
#endif
#endif

    for (i=0; i < testsize; ++i)
        smap_arena_alloc(&sa, strkey(i));
    smap_arena_reset(&sa);

    {
        s.key = "a", s.value = 1;
        SMAP_PUT_STRUCT(strmap, s);
        MAP_ASSERT(*strmap[0].key == 'a');
        MAP_ASSERT(strmap[0].key == s.key);
        MAP_ASSERT(strmap[0].value == s.value);
        SMAP_FREE(strmap);
    }

    {
        s.key = "a", s.value = 1;
        SMAP_NEW_STRDUP(strmap);
        SMAP_PUT_STRUCT(strmap, s);
        MAP_ASSERT(*strmap[0].key == 'a');
        MAP_ASSERT(strmap[0].key != s.key);
        MAP_ASSERT(strmap[0].value == s.value);
        SMAP_FREE(strmap);
    }

    {
        s.key = "a", s.value = 1;
        SMAP_NEW_ARENA(strmap);
        SMAP_PUT_STRUCT(strmap, s);
        MAP_ASSERT(*strmap[0].key == 'a');
        MAP_ASSERT(strmap[0].key != s.key);
        MAP_ASSERT(strmap[0].value == s.value);
        SMAP_FREE(strmap);
    }

    for (j=0; j < 2; ++j) 
    {
        MAP_ASSERT(SMAP_GET_IDX(strmap,"foo") == -1);
        if (j == 0)
            SMAP_NEW_STRDUP(strmap);
        else
            SMAP_NEW_ARENA(strmap);

        MAP_ASSERT(SMAP_GET_IDX(strmap,"foo") == -1);
        SMAP_DEFAULT(strmap, -2);
        MAP_ASSERT(SMAP_GET_IDX(strmap,"foo") == -1);

        for (i=0; i < testsize; i+=2)
            SMAP_PUT(strmap, strkey(i), i*3);

        for (i=0; i < testsize; i+=1)
            if (i & 1) MAP_ASSERT(SMAP_GET(strmap, strkey(i)) == -2 );
            else       MAP_ASSERT(SMAP_GET(strmap, strkey(i)) == i*3);

        for (i=2; i < testsize; i+=4)
            SMAP_DELETE(strmap, strkey(i)); // delete half the entries
                                      
        for (i=0; i < testsize; i+=1)
            if (i & 3) MAP_ASSERT(SMAP_GET(strmap, strkey(i)) == -2 );
            else       MAP_ASSERT(SMAP_GET(strmap, strkey(i)) == i*3);

        for (i=0; i < testsize; i+=1)
            SMAP_DELETE(strmap, strkey(i)); // delete the rest of the entries

        for (i=0; i < testsize; i+=1)
            MAP_ASSERT(SMAP_GET(strmap, strkey(i)) == -2 );

        SMAP_FREE(strmap);
    }

    {
        struct { char *key; char value; } *hash = NULL;
        char name[4] = "jen";
        SMAP_PUT(hash, "bob"   , 'h');
        SMAP_PUT(hash, "sally" , 'e');
        SMAP_PUT(hash, "fred"  , 'l');
        SMAP_PUT(hash, "jen"   , 'x');
        SMAP_PUT(hash, "doug"  , 'o');

        SMAP_PUT(hash, name    , 'l');
        SMAP_FREE(hash);
    }

    for (i=0; i < testsize; i += 2) 
    {
        test_struct s = { i,i*2,i*3,i*4 };
        HMAP_PUT(map, s, i*5);
    }

    for (i=0; i < testsize; i += 1) 
    {
        test_struct s = { i,i*2,i*3  ,i*4 };
        test_struct t = { i,i*2,i*3+1,i*4 };
        if (i & 1) MAP_ASSERT(HMAP_GET(map, s) == 0);
        else       MAP_ASSERT(HMAP_GET(map, s) == i*5);

        if (i & 1) MAP_ASSERT(HMAP_GET_TS(map, s, temp) == 0);
        else       MAP_ASSERT(HMAP_GET_TS(map, s, temp) == i*5);

        //MAP_ASSERT(HMAP_GET(map, t.key) == 0);
    }

    for (i=0; i < testsize; i += 2) 
    {
        test_struct s = { i,i*2,i*3,i*4 };
        HMAP_PUT_STRUCT(map2, s);
    }

    HMAP_FREE(map);

    for (i=0; i < testsize; i += 1) 
    {
        test_struct s = { i,i*2,i*3,i*4 };
        test_struct t = { i,i*2,i*3+1,i*4 };

        if (i & 1) MAP_ASSERT(HMAP_GET_STRUCT(map2, s.key).d == 0);
        else       MAP_ASSERT(HMAP_GET_STRUCT(map2, s.key).d == i*4);

        //MAP_ASSERT(HMAP_GET_PTR(map2, t.key) == 0);
    }
    HMAP_FREE(map2);

    // for (i=0; i < testsize; i += 2) 
    // {
    //     test_struct2 s = { { i,i*2 }, i*3,i*4, i*5 };
    //     HMAP_PUT_STRUCT(map3, s);
    // }
    //
    // for (i=0; i < testsize; i += 1) 
    // {
    //     test_struct2 s = { { i,i*2}, i*3, i*4, i*5 };
    //     test_struct2 t = { { i,i*2}, i*3+1, i*4, i*5 };
    //
    //     if (i & 1) MAP_ASSERT(HMAP_GET_STRUCT(map3, s.key).d == 0);
    //     else       MAP_ASSERT(HMAP_GET_STRUCT(map3, s.key).d == i*5);
    //     //MAP_ASSERT(hmgetp(map3, t.key) == 0);
    // }
}
#endif



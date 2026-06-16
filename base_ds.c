
#include <stdlib.h>

#include "base_ds.h"
#include "base_arena.h"


static size_t hash_string(char *str, size_t seed);
static size_t siphash_bytes(void *p, size_t len, size_t seed);
static size_t hash_bytes(void *p, size_t len, size_t seed);

/*********************************************************************************/
void* array_grow_arena(Arena* arena, void* items, uint64_t item_size, uint64_t count)
{
    void* new_ptr;
    uint64_t new_len;
    uint64_t new_cap;

    uint64_t len = ARRAY_LEN(items);
    uint64_t cap = ARRAY_CAP(items);

    new_len = len + count;
    if (new_len < cap)
        return items;

    if (new_len < 2 * cap)
        new_cap = 2 * cap;
    else if (new_len < 4)
        new_cap = 4; // minimum capacity of 4
    else // NOTE(bcall): if new_len > 2 * capacity, then set cap to 1.5*new_len
        new_cap = (uint64_t)(3 * new_len / 2);

    if (items == NULL)
    {
        void* ptr = arena_push(arena, sizeof(ArrayHeader) + new_cap * item_size);
        if (ptr)
        {
            ArrayHeader* hdr = (ArrayHeader*)ptr;
            hdr->cap = new_cap;
            hdr->len = 0;
            return (void*)(hdr + 1);
        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items; 
        }
    }
    // NOTE(bcall): Array has room to grow in arena without relocating it...
    else if ((uint64_t)(arena->base + arena->pos) == (uint64_t)items + cap*item_size)
    {
        // NOTE(bcall): since array is not moving we allocate space for diff in new vs old capacity
        void* ptr = arena_push(arena, (new_cap - cap) * item_size);
        if (ptr)
        {
            ARRAY_HEADER_CAST(items)->cap = new_cap;
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
        void* ptr = arena_push(arena, sizeof(ArrayHeader) + new_cap * item_size);
        if (ptr)
        {
            ArrayHeader* hdr = (ArrayHeader*)ptr;
            hdr->cap = new_cap;

            ptr = (void*)(hdr + 1);
            memmove(ptr, items, len * item_size);
            
            // TODO(bcall): free prev items pointer if arena has free list...
            return ptr;

        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items;
        }
    }
}

/*********************************************************************************/
void* array_grow_heap(void* items, uint64_t item_size, uint64_t count)
{
    void* new_ptr;
    uint64_t new_len;
    uint64_t new_cap;

    uint64_t len = ARRAY_LEN(items);
    uint64_t cap = ARRAY_CAP(items);

    new_len = len + count;
    if (new_len < cap)
        return items;

    if (new_len < 2 * cap)
        new_cap = 2 * cap;
    else if (new_len < 4)
        new_cap = 4;
    else
        new_cap = (uint64_t)(3 * new_len / 2);

    if (items == NULL)
    {
        void* ptr = malloc(sizeof(ArrayHeader) + new_cap * item_size);
        if (ptr)
        {
            ArrayHeader* hdr = (ArrayHeader*)ptr;
            hdr->cap = new_cap;
            hdr->len = 0;
            return (void*)(hdr + 1);
        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items; 
        }
    }
    else
    {
        void* ptr = realloc(ARRAY_HEADER_CAST(items), sizeof(ArrayHeader) + new_cap * item_size);
        if (ptr)
        {
            ArrayHeader* hdr = (ArrayHeader*)ptr;
            hdr->cap = new_cap;
            hdr->len = len;

            ptr = (void*)(hdr + 1);
            memmove(ptr, items, len * item_size);
            
            return ptr;

        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return items;
        }
    }
}

/*********************************************************************************/
size_t hmap_insert(void* map, void* key_ptr, size_t key_size, int32_t seed)
{
    size_t len = HMAP_LEN(map);
    size_t cap = HMAP_CAP(map);
    MapEntry* entries = HMAP_ENTRIES_CAST(map);
   
    size_t hash = hash_bytes(key_ptr, key_size, seed);

    size_t entry_idx  = hash % cap;
    size_t entry_hash = entries[entry_idx].hash;
    MapEntryState entry_state = entries[entry_idx].state;

    while (entry_state == MAP_ENTRY_TAKEN && entry_hash != hash)
    {
        entry_idx = (entry_idx + 1) % cap; // linear probing...
        entry_hash = entries[entry_idx].hash;
        entry_state = entries[entry_idx].state;
    }

    if (entry_state != MAP_ENTRY_TAKEN)
    {
        MapEntry* new_entry = &entries[entry_idx];
        new_entry->hash  = hash;
        new_entry->idx   = len;
        new_entry->state = MAP_ENTRY_TAKEN;
        HMAP_HEADER_CAST(map)->len++;
    }
    return entry_idx;
}

/*********************************************************************************/
void hmap_rehash_entries(void* map, void* new_map, size_t new_cap, size_t item_size, size_t key_size)
{
    size_t old_len = HMAP_LEN(map);
    MapEntry* new_entries = HMAP_ENTRIES_CAST(new_map);

    for (size_t i = 0; i < old_len; i++)
    {
        uint8_t* key_ptr = ((uint8_t*)map + item_size * i);
        hmap_insert(new_map, key_ptr, key_size, HMAP_SEED);
    }
}


/*********************************************************************************/
void* hmap_grow_heap(void* map, size_t item_size, size_t key_size, size_t count)
{
    void* new_ptr;
    size_t new_len;
    size_t new_cap;

    size_t len = HMAP_LEN(map);
    size_t cap = HMAP_CAP(map);

    new_len = len + count;
    if (new_len < map_grow_threshold * cap)
        return map;

    if (new_len < 2 * cap)
        new_cap = 2 * cap;
    else if (new_len < 4)
        new_cap = 4;
    else
        new_cap = (uint64_t)((3*new_len)/2);

    if (map == NULL)
    {
        void* new_map = malloc(sizeof(MapHeader) + ((new_cap + 1) * item_size) + (new_cap * sizeof(MapEntry)));
        if (new_map)
        {
            MapHeader* hdr = (MapHeader*)new_map;
            hdr->cap = new_cap;
            hdr->len = 0;
            hdr->ts_count = 0;

            uint8_t* new_map_start = (uint8_t*)(new_map + sizeof(MapHeader) + item_size); 
            return new_map_start; // leave space for map[-1] to store default value...
        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return map; 
        }
    }
    else
    {
        uint8_t* new_map = malloc(sizeof(MapHeader) + ((new_cap + 1) * item_size) + (new_cap * sizeof(MapEntry)));
        if (new_map)
        {
            MapHeader* hdr = (MapHeader*)new_map;
            hdr->cap = new_cap;
            hdr->len = 0; // will be incremented in rehash
            hdr->ts_count = 0;

            uint8_t* new_map_ptr = new_map + sizeof(MapHeader) + item_size;
            hmap_rehash_entries(map, new_map_ptr, new_cap, item_size, key_size);

            uint8_t* src = (((uint8_t*)map)-item_size);
            uint8_t* dst = (uint8_t*)new_map + sizeof(MapHeader);
            memmove(dst, src, (len+1)*item_size);

            free(HMAP_HEADER_CAST(map));
            return new_map_ptr;

        }
        else // WARN(bcall): memory allocation fails hiddenly... 
        {
            return map;
        }
    }
}

#define SIZE_T_BITS           ((sizeof (size_t)) * 8)

#define ROTATE_LEFT(val, n)   (((val) << (n)) | ((val) >> (SIZE_T_BITS - (n))))
#define ROTATE_RIGHT(val, n)  (((val) >> (n)) | ((val) << (SIZE_T_BITS - (n))))

#define SIPHASH_C_ROUNDS 1
#define SIPHASH_D_ROUNDS 1

/*********************************************************************************/
static size_t hash_string(char *str, size_t seed)
{
    size_t hash = seed;
    while (*str)
        hash = ROTATE_LEFT(hash, 9) + (unsigned char) *str++;

    // Thomas Wang 64-to-32 bit mix function, hopefully also works in 32 bits
    hash ^= seed;
    hash = (~hash) + (hash << 18);
    hash ^= hash ^ ROTATE_RIGHT(hash,31);
    hash = hash * 21;
    hash ^= hash ^ ROTATE_RIGHT(hash,11);
    hash += (hash << 6);
    hash ^= ROTATE_RIGHT(hash,22);
    return hash+seed;
}

/*********************************************************************************/
static size_t siphash_bytes(void *p, size_t len, size_t seed)
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
        do { \
            v0 += v1; v1 = ROTATE_LEFT(v1, 13);  v1 ^= v0; v0 = ROTATE_LEFT(v0,SIZE_T_BITS/2); \
            v2 += v3; v3 = ROTATE_LEFT(v3, 16);  v3 ^= v2;                                     \
            v2 += v1; v1 = ROTATE_LEFT(v1, 17);  v1 ^= v2; v2 = ROTATE_LEFT(v2,SIZE_T_BITS/2); \
            v0 += v3; v3 = ROTATE_LEFT(v3, 21);  v3 ^= v0;                                     \
        } while (0)

    for (i=0; i+sizeof(size_t) <= len; i += sizeof(size_t), d += sizeof(size_t)) {
        data = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
        data |= (size_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // discarded if size_t == 4

        v3 ^= data;
        for (j=0; j < SIPHASH_C_ROUNDS; ++j)
            SIPROUND();
        v0 ^= data;
    }
    data = len << (SIZE_T_BITS-8);
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
    for (j=0; j < SIPHASH_C_ROUNDS; ++j)
        SIPROUND();
    v0 ^= data;
    v2 ^= 0xff;
    for (j=0; j < SIPHASH_D_ROUNDS; ++j)
        SIPROUND();

    return v1^v2^v3;
}

/*********************************************************************************/
static size_t hash_bytes(void *p, size_t len, size_t seed)
{
  unsigned char *d = (unsigned char *) p;

  if (len == 4) {
    unsigned int hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
    #if 0
    // HASH32-A  Bob Jenkin's hash function w/o large constants
    hash ^= seed;
    hash -= (hash<<6);
    hash ^= (hash>>17);
    hash -= (hash<<9);
    hash ^= seed;
    hash ^= (hash<<4);
    hash -= (hash<<3);
    hash ^= (hash<<10);
    hash ^= (hash>>15);
    #elif 1
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
    #else  // HASH32-C   -  Murmur3
    hash ^= seed;
    hash *= 0xcc9e2d51;
    hash = (hash << 17) | (hash >> 15);
    hash *= 0x1b873593;
    hash ^= seed;
    hash = (hash << 19) | (hash >> 13);
    hash = hash*5 + 0xe6546b64;
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= seed;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    #endif
    // Following statistics were measured on a Core i7-6700 @ 4.00Ghz, compiled with clang 7.0.1 -O2
    // Note that the larger tables have high variance as they were run fewer times
    //  HASH32-A   //  HASH32-BB  //  HASH32-C
    //    0.10ms   //    0.10ms   //    0.10ms :      2,000 inserts creating 2K table
    //    0.96ms   //    0.95ms   //    0.99ms :     20,000 inserts creating 20K table
    //   14.69ms   //   14.43ms   //   14.97ms :    200,000 inserts creating 200K table
    //  199.99ms   //  195.36ms   //  202.05ms :  2,000,000 inserts creating 2M table
    // 2234.84ms   // 2187.74ms   // 2240.38ms : 20,000,000 inserts creating 20M table
    //   55.68ms   //   53.72ms   //   57.31ms : 500,000 inserts & deletes in 2K table
    //   63.43ms   //   61.99ms   //   65.73ms : 500,000 inserts & deletes in 20K table
    //   80.04ms   //   77.96ms   //   81.83ms : 500,000 inserts & deletes in 200K table
    //  100.42ms   //   97.40ms   //  102.39ms : 500,000 inserts & deletes in 2M table
    //  119.71ms   //  120.59ms   //  121.63ms : 500,000 inserts & deletes in 20M table
    //  185.28ms   //  195.15ms   //  187.74ms : 500,000 inserts & deletes in 200M table
    //   15.58ms   //   14.79ms   //   15.52ms : 200,000 inserts creating 200K table with varying key spacing

    return (((size_t) hash << 16 << 16) | hash) ^ seed;
  } else if (len == 8 && sizeof(size_t) == 8) {
    size_t hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
    hash |= (size_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // avoid warning if size_t == 4
    hash ^= seed;
    hash = (~hash) + (hash << 21);
    hash ^= ROTATE_RIGHT(hash,24);
    hash *= 265;
    hash ^= ROTATE_RIGHT(hash,14);
    hash ^= seed;
    hash *= 21;
    hash ^= ROTATE_RIGHT(hash,28);
    hash += (hash << 31);
    hash = (~hash) + (hash << 18);
    return hash;
  } else {
    return siphash_bytes(p,len,seed);
  }
}




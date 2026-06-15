

## HashMap

### Structure:

\[--------------------MEMORY---------------------\]
\[HashMapHeader] \[type * n] \[HashMapEntry * n]
                   ^ map object pointer of type*

### Needed Functions:

\[X] rand_int()                         // for seeding hash
\[X] hash_bytes(void* ptr, size_t len)  //
\[X] hash_string(char* string)

\[ ] hmap_rehash(map, new_map, new_cap)
\[ ] hmap_get_idx(map, key_ptr, key_len)

\[ ] hmap_shrink_heap()
\[ ] hmap_grow_heap()

### API:

HMAP_DEFAULT(map, default)
    - sets default as hashmap struct->value return when key is missing

HMAP_DEFAULTS(map, default)
    - sets default as hashmap struct return when key is missing

HMAP_PUT(map, key, value)
    - puts key, value in hashmap
    - what to do if exists

HMAP_PUTS(map, struct)
    - puts struct in hashmap
    - what to do if exists

HMAP_GET_IDX(map, key)
    - returns idx of item with given key in the map
    - can use key to get item with map\[idx\]
    - return -1 if key not found

HMAP_GET(map, key)
    - return struct->value with given key from map
    - what to do if not found and no default
    - how to return item

HMAP_GETS(map, key)
    - return struct with given key from map

HMAP_GETP(map, key)
    - return struct* with given key from map

HMAP_GETP_NULL(map, key)
    - return struct with given key from map

HMAP_REMOVE(map, key)
    - if key is in map, removes entry
    - returns 1 if removed and 0 otherwise


### Other

use swap delete to minimize rehashing when preserving arrray

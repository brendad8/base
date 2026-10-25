
#ifndef POOL_H
#define POOL_H

//***************************************************************************
//          CONFIGURATION OPTIONS
//***************************************************************************

#ifndef POOL_EXPORT
#define POOL_EXPORT
#endif

//***************************************************************************
//          INCLUDE FILES
//***************************************************************************

#include <stddef.h> // for size_t, NULL
#include <stdint.h> // for uint8_t

//***************************************************************************
//          TYPES
//***************************************************************************

typedef struct PoolNode PoolNode;
struct PoolNode
{
    PoolNode* next;
};

typedef struct
{
    PoolNode* free_list;
    size_t num_used;
    size_t num_free;
    size_t block_size;

} Pool;

//***************************************************************************
//          FUNCTION PROTOTYPES
//***************************************************************************

POOL_EXPORT  Pool*   pool_alloc         (size_t block_size, size_t count);
POOL_EXPORT  void    pool_release       (Pool* pool);
POOL_EXPORT  void    pool_clear         (Pool* pool);
POOL_EXPORT  void*   pool_get_block     (Pool* pool);
POOL_EXPORT  void*   pool_free_block    (Pool* pool, void* block_ptr);

//***************************************************************************
//          FUNCTION IMPLEMENTATIONS
//***************************************************************************

#ifdef POOL_IMPLEMENTATION

#include <stdlib.h> // for malloc, free

/*********************************************************************************/
POOL_EXPORT Pool* pool_alloc(size_t block_size, size_t count)
{
    if (count == 0)
        return NULL;

    // NOTE(bcall): every block must be large enough to hold a PoolNode
    if (block_size < sizeof(PoolNode))
        block_size = sizeof(PoolNode);

    Pool* pool = (Pool*)malloc(sizeof(Pool) + (block_size * count));
    if (pool == NULL) return NULL;

    pool->free_list = NULL;
    pool->num_used   = 0;
    pool->num_free   = count;
    pool->block_size = block_size;

    uint8_t* block = (uint8_t*)(pool + 1);

    for (size_t i = 0; i < count; i++)
    {
        PoolNode* node = (PoolNode*)block;
        node->next = pool->free_list;
        pool->free_list = node;

        block += block_size;
    }

    return pool;
}

/*********************************************************************************/
POOL_EXPORT void pool_release(Pool* pool)
{
    if (pool)
        free(pool);
}

/*********************************************************************************/
POOL_EXPORT void pool_clear(Pool* pool)
{
    if (pool == NULL) return;

    size_t total_blocks = pool->num_used + pool->num_free;

    pool->free_list = NULL;
    pool->num_used = 0;
    pool->num_free = total_blocks;

    unsigned char* block = (unsigned char*)(pool + 1);

    for (size_t i = 0; i < total_blocks; ++i)
    {
        PoolNode* node = (PoolNode*)block;
        node->next = pool->free_list;
        pool->free_list = node;

        block += pool->block_size;
    }
}

/*********************************************************************************/
POOL_EXPORT void* pool_get_block(Pool* pool)
{
    if (pool == NULL || pool->free_list == NULL)
        return NULL;

    PoolNode* node = pool->free_list;
    pool->free_list = node->next;

    pool->num_used++;
    pool->num_free--;

    return node;
}

/*********************************************************************************/
POOL_EXPORT void* pool_free_block(Pool* pool, void* block_ptr)
{
    if (pool == NULL || block_ptr == NULL)
        return NULL;

    PoolNode* node = (PoolNode*)block_ptr;

    node->next = pool->free_list;
    pool->free_list = node;

    pool->num_used--;
    pool->num_free++;

    return block_ptr;
}

#endif // POOL_IMPLEMENTATION

#endif // POOL_H

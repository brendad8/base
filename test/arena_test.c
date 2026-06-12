
#include "../base_arena.c"
#include "../base_test.h"

static void test_arena_basic_alloc(void)
{
    ArenaParams params = {0};
    Arena* arena = arena_alloc(params);
    TEST_ASSERT(arena->committed == arena_default_commit_size);
    TEST_ASSERT(arena->reserved == arena_default_reserve_size);
    uint64_t start = arena->pos;
    void* p1 = arena_push(arena, 64);
    TEST_ASSERT(p1 != NULL);
    TEST_ASSERT(arena->pos >= start + 64);
    arena_release(arena);
}

static void test_arena_push(void)
{
    ArenaParams params = {KB(4), KB(64), 0};
    Arena* arena = arena_alloc(params);
    char* ptr = arena_push(arena, KB(5));
    TEST_ASSERT(ptr != NULL);
    TEST_ASSERT(arena->committed == KB(8));
}

static bool32 is_aligned(void* ptr, uint64_t align)
{
    return (((uintptr_t)ptr) & (align - 1)) == 0;
}

void test_arena_alignment(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    uint64_t aligns[] = {1,2,4,8,16,32,64};
    for(uint64_t i = 0; i < ARRAY_COUNT(aligns); i++)
    {
        uint64_t align = aligns[i];
        void* ptr = arena_push_align(arena, 13, align);
        TEST_ASSERT(is_aligned(ptr, align));
    }

    arena_release(arena);
}
static void test_arena_push_zeroes_memory(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    uint8_t* mem = ARENA_PUSH_ARRAY(arena, uint8_t, 128);
    for(uint64_t i = 0; i < 128; i++)
        TEST_ASSERT(mem[i] == 0);
    arena_release(arena);
}

static void test_arena_pop(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    uint64_t start = arena->pos;
    arena_push(arena, 128);
    arena_pop(arena, 128);
    TEST_ASSERT(arena->pos == start);
    arena_release(arena);
}

static void test_arena_pop_to(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    arena_push(arena, 64);
    uint64_t mark = arena->pos;
    arena_push(arena, 256);
    arena_pop_to(arena, mark);
    TEST_ASSERT(arena->pos == mark);
    arena_release(arena);
}


static void test_arena_clear(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    arena_push(arena, 1024);
    arena_clear(arena);
    TEST_ASSERT(arena->pos == 0);
    arena_release(arena);
}


static void test_arena_temp(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    uint64_t start = arena->pos;
    ArenaTemp temp = arena_temp_begin(arena);
    arena_push(arena, 512);
    TEST_ASSERT(arena->pos > start);
    arena_temp_end(temp);
    TEST_ASSERT(arena->pos == start);
    arena_release(arena);
}


static void test_arena_nested_temps(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    uint64_t start = arena->pos;
    ArenaTemp t1 = arena_temp_begin(arena);
    arena_push(arena, 64);
    uint64_t p1 = arena->pos;
    ArenaTemp t2 = arena_temp_begin(arena);
    arena_push(arena, 128);
    arena_temp_end(t2);
    TEST_ASSERT(arena->pos == p1);
    arena_temp_end(t1);
    TEST_ASSERT(arena->pos == start);
    arena_release(arena);
}


static void test_arena_commit_growth(void)
{
    ArenaParams params = {0};
    params.commit_size = KB(4);
    Arena* arena = arena_alloc(params);
    uint64_t initial_commit = arena->committed;
    TEST_ASSERT(initial_commit = KB(4));
    arena_push(arena, KB(5));
    TEST_ASSERT(arena->committed > initial_commit);
    arena_release(arena);
}


static void test_arena_reserve_limit(void)
{
    ArenaParams params = {0};
    params.reserve_size = MB(1);
    Arena* arena = arena_alloc(params);
    void* ptr = arena_push(arena, MB(2));
    TEST_ASSERT(ptr == 0);
    arena_release(arena);
}

typedef struct
{
    int x;
    double y;

} TestStruct;

static void test_push_struct(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    TestStruct* s = ARENA_PUSH_STRUCT(arena, TestStruct);
    TEST_ASSERT(s != 0);
    arena_release(arena);
}


static void test_arena_stress(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    for(uint64_t i = 0; i < 100; i++)
    {
        uint64_t size  = (i % 256) + 1;
        uint64_t align = 1ULL << (i % 6);
        void* ptr = arena_push_align(arena, size, align);
        TEST_ASSERT(ptr != 0);
        TEST_ASSERT(is_aligned(ptr, align));
    }
    arena_release(arena);
}

static void test_arena_out_of_memory(void)
{
    Arena* arena = arena_alloc((ArenaParams){KB(64), KB(64), 0});
    char* ptr = arena_push(arena, GB(1));
    TEST_ASSERT(ptr == NULL);
    arena_release(arena);
}


static void test_arena_growable(void)
{
    Arena* arena = arena_alloc((ArenaParams){KB(64), KB(64), 1});
    char* ptr = arena_push(arena, KB(64) + 20);
    
    TEST_ASSERT(ptr != NULL);
    TEST_ASSERT(arena->next != NULL);
    TEST_ASSERT(arena->next->params.commit_size == KB(128));
    TEST_ASSERT(arena->next->params.commit_size == KB(128));
    arena_release(arena);
}




int main(void)
{
    test_arena_basic_alloc();
    test_arena_push();
    test_arena_push_zeroes_memory();
    test_arena_pop();
    test_arena_pop_to();
    test_arena_clear();
    test_arena_temp();
    test_arena_nested_temps();
    test_arena_commit_growth();
    test_push_struct();
    test_arena_stress();
    test_arena_out_of_memory();
    test_arena_growable();

    test_print_results("Arena");

    return 0;
}

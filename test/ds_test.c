
#include "../base_arena.c"
#include "../base_ds.c"
#include "../base_test.h"

typedef struct Node Node;
struct Node 
{
    int value;
    Node* next;

};

static void test_stack_push_pop(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    Node* first = {0};

    for (int i = 0; i < 10; i++)
    {
        Node* nodep = arena_push_struct(arena, Node);
        nodep->value = i;
        STACK_PUSH(first, nodep);
        TEST_ASSERT(first->value == i);
    }

    int i = 9;
    while (first)
    {
        TEST_ASSERT(first->value == i);
        STACK_POP(first);
        i--;
    }
    TEST_ASSERT(first == NULL);
    arena_release(arena);
}

static void test_queue_push_pop(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    Node* first = {0};
    Node* last = {0};

    for (int i = 0; i < 10; i++)
    {
        Node* nodep = arena_push_struct(arena, Node);
        nodep->value = i;
        QUEUE_PUSH(first, last, nodep);
        TEST_ASSERT(first->value == 0);
        TEST_ASSERT(last->value == i);
    }

    int i = 0;
    while (first)
    {
        TEST_ASSERT(first->value == i);
        QUEUE_POP(first, last);
        i++;
    }
    TEST_ASSERT(first == NULL);
    TEST_ASSERT(last == NULL);
    arena_release(arena);
}

typedef struct DLLNode DLLNode;
struct DLLNode
{
    int value;
    DLLNode* next;
    DLLNode* prev;
};

static void test_doubly_linked_list(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    DLLNode* first = {0};
    DLLNode* last  = {0};

    for (int i = 0; i < 10; i++)
    {
        DLLNode* node = arena_push_struct(arena, DLLNode);
        node->value = i;
        DLL_PUSH_BACK(first, last, node);
        TEST_ASSERT(last->value == i);
    }
    TEST_ASSERT(first->value == 0);

    DLLNode* third = first->next->next;
    DLL_REMOVE(first, last, third);
    DLLNode* node = first;
    while (node)
    {
        TEST_ASSERT(node->value != third->value);
        node = node->next;
    }

    TEST_ASSERT(first->value == 0);
    DLL_REMOVE_FIRST(first, last);
    TEST_ASSERT(first->value == 1);
    DLL_REMOVE_FIRST(first, last);
    TEST_ASSERT(first->value == 3); // third (idx 2) removed

    TEST_ASSERT(last->value == 9);
    DLL_REMOVE_LAST(first, last);
    TEST_ASSERT(last->value == 8);
    DLL_REMOVE_LAST(first, last);
    TEST_ASSERT(last->value == 7);

    for (int i = 10; i < 20; i++)
    {
        DLLNode* node = arena_push_struct(arena, DLLNode);
        node->value = i;
        DLL_PUSH_FRONT(first, last, node);
        TEST_ASSERT(first->value == i);
    }
}

typedef struct FloatArray FloatArray;
struct FloatArray 
{
    ArrayHeader header;
    float* items;
};

static void test_dynamic_array(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    FloatArray floats = {0};

    ARRAY_RESERVE(arena, floats, 3);
    TEST_ASSERT(ARRAY_LEN(floats) == 0);
    TEST_ASSERT(ARRAY_CAP(floats) == 4);

    ARRAY_PUSH(arena, floats, 1.222);
    TEST_ASSERT(ARRAY_LEN(floats) == 1);
    TEST_ASSERT(ARRAY_CAP(floats) == 4);

    ARRAY_ADD(arena, floats, 4);
    TEST_ASSERT(ARRAY_LEN(floats) == 5);
    TEST_ASSERT(ARRAY_CAP(floats) == 8);

    void* old_array_ptr = floats.items;
    
    arena_push(arena, 400);
    ARRAY_ADD(arena, floats, 4);
    TEST_ASSERT(ARRAY_LEN(floats));
    TEST_ASSERT(ARRAY_CAP(floats) == 16);

    void* new_array_ptr = floats.items;
    TEST_ASSERT(new_array_ptr != old_array_ptr);

    ARRAY_CLEAR_ZERO(floats);
    TEST_ASSERT(ARRAY_LEN(floats) == 0);

    for (int i = 0; i < 10; i++)
    {
        float item = (float)i;
        ARRAY_PUSH(arena, floats, item);
        TEST_ASSERT(floats.items[i] == item);
    }

    ARRAY_INSERT(arena, floats, 5, 99.0f);
    TEST_ASSERT(floats.items[5] == 99.0f);

    // for (int i = 0; i < 10; i++)
    // {
    //     printf("%d: %f\n", i, floats.items[i]);
    // }

    TEST_ASSERT(floats.items[4] == 4.0f);
    TEST_ASSERT(floats.items[6] == 5.0f);
    TEST_ASSERT(floats.items[10] == 9.0f);
    
    ARRAY_REMOVE(floats, 5);
    for (int i = 0; i < 10; i++)
    {
        float item = (float)i;
        // printf("%d: %f\n", i, item);
        // TEST_ASSERT(((int)(floats.items[i])) == i);
    }

    ARRAY_REMOVE_SWAP(floats, 5);
    TEST_ASSERT((int)(floats.items[5]) == 9);

    arena_release(arena);
}

int main(void)
{
    test_stack_push_pop();
    test_queue_push_pop();
    test_doubly_linked_list();
    test_dynamic_array();

    test_print_results("Data Structure");

    return 0;
}

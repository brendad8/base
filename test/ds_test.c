
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
        stack_push(first, nodep);
        TEST_ASSERT(first->value == i);
    }

    int i = 9;
    while (first)
    {
        TEST_ASSERT(first->value == i);
        stack_pop(first);
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
        queue_push(first, last, nodep);
        TEST_ASSERT(first->value == 0);
        TEST_ASSERT(last->value == i);
    }

    int i = 0;
    while (first)
    {
        TEST_ASSERT(first->value == i);
        queue_pop(first, last);
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
        dll_push_back(first, last, node);
        TEST_ASSERT(last->value == i);
    }
    TEST_ASSERT(first->value == 0);

    DLLNode* third = first->next->next;
    dll_remove(first, last, third);
    DLLNode* node = first;
    while (node)
    {
        TEST_ASSERT(node->value != third->value);
        node = node->next;
    }

    TEST_ASSERT(first->value == 0);
    dll_remove_first(first, last);
    TEST_ASSERT(first->value == 1);
    dll_remove_first(first, last);
    TEST_ASSERT(first->value == 3); // third (idx 2) removed

    TEST_ASSERT(last->value == 9);
    dll_remove_last(first, last);
    TEST_ASSERT(last->value == 8);
    dll_remove_last(first, last);
    TEST_ASSERT(last->value == 7);

    for (int i = 10; i < 20; i++)
    {
        DLLNode* node = arena_push_struct(arena, DLLNode);
        node->value = i;
        dll_push_front(first, last, node);
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

    arr_reserve(arena, floats, 3);
    TEST_ASSERT(arr_len(floats) == 0);
    TEST_ASSERT(arr_cap(floats) == 4);

    arr_push(arena, floats, 1.222);
    TEST_ASSERT(arr_len(floats) == 1);
    TEST_ASSERT(arr_cap(floats) == 4);

    arr_add(arena, floats, 4);
    TEST_ASSERT(arr_len(floats) == 5);
    TEST_ASSERT(arr_cap(floats) == 8);

    void* old_array_ptr = floats.items;
    
    arena_push(arena, 400);
    arr_add(arena, floats, 4);
    TEST_ASSERT(arr_len(floats));
    TEST_ASSERT(arr_cap(floats) == 16);

    void* new_array_ptr = floats.items;
    TEST_ASSERT(new_array_ptr != old_array_ptr);

    arr_clear_zero(floats);
    TEST_ASSERT(arr_len(floats) == 0);

    for (int i = 0; i < 10; i++)
    {
        float item = (float)i;
        arr_push(arena, floats, item);
    }

    arr_insert(arena, floats, 5, 99.0);
    TEST_ASSERT((int)(floats.items[5]) == 99);
    TEST_ASSERT((int)(floats.items[4]) == 4);
    TEST_ASSERT((int)(floats.items[6]) == 5);
    TEST_ASSERT((int)(floats.items[10]) == 9);
    
    arr_remove(floats, 5);
    for (int i = 0; i < 10; i++)
    {
        float item = (float)i;
        TEST_ASSERT((int)(floats.items[i]) == i);
    }
    arr_remove_swap(floats, 5);
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

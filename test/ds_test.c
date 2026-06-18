
#include "../base_arena.c"
#include "../base_core.h"
#include "../base_ds.h"
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
        Node* nodep = ARENA_PUSH_STRUCT(arena, Node);
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
        Node* nodep = ARENA_PUSH_STRUCT(arena, Node);
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
        DLLNode* node = ARENA_PUSH_STRUCT(arena, DLLNode);
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
        DLLNode* node = ARENA_PUSH_STRUCT(arena, DLLNode);
        node->value = i;
        DLL_PUSH_FRONT(first, last, node);
        TEST_ASSERT(first->value == i);
    }
}


int main(void)
{
    test_stack_push_pop();
    test_queue_push_pop();
    test_doubly_linked_list();

    test_print_results("Data Structure");

    return 0;
}

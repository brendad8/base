
#include "../base_arena.c"
#include "../base_string.c"
#include "../base_test.h"

void test_string_construction(void)
{
    string hello = str_lit("hi mom!");
    TEST_ASSERT(hello.len == 7);
    
    char* c_string = "hi mom!";
    string hello2 = str_from_cstr(c_string);
    TEST_ASSERT(hello2.len == 7);
}

void test_string_compare(void)
{
}

void test_string_split(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    string to_split = str_lit(",hi,mom,,");
    stringList splits = str_split(arena, to_split, str_lit(","));
    TEST_ASSERT(splits.node_count == 5);

    stringNode* node = splits.first;
    TEST_ASSERT(str_equal(node->s, str_new("\0", 0)));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_lit("hi")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_lit("mom")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_new("\0", 0)));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_new("\0", 0)));

    arena_release(arena);
}

void test_string_split_skip_empty(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    string to_split = str_lit(",hi,mom,,");
    stringList splits = str_split_skip_empty(arena, to_split, str_lit(","));
    TEST_ASSERT(splits.node_count == 2);
    stringNode* node = splits.first;
    TEST_ASSERT(str_equal(node->s, str_lit("hi")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_lit("mom")));
    
    to_split = str_lit(",hi,mom,,last");
    splits = str_split_skip_empty(arena, to_split, str_lit(","));
    TEST_ASSERT(splits.node_count == 3);
    node = splits.first;
    TEST_ASSERT(str_equal(node->s, str_lit("hi")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_lit("mom")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_lit("last")));

    arena_release(arena);
}


int main(void)
{
    test_string_construction();
    test_string_split();
    test_string_split_skip_empty();

    test_print_results("String");
}

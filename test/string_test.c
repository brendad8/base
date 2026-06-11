
#include "../base_arena.c"
#include "../base_string.c"
#include "../base_test.h"

string empty = {"", 0};

void test_string_construction(void)
{
    string hello = STR_LIT("hi mom!");
    TEST_ASSERT(hello.len == 7);
    
    char* c_string = "hi mom!";
    string hello2 = str_from_cstr(c_string);
    TEST_ASSERT(hello2.len == 7);
}

void test_string_equal(void)
{
    char* cstr = "hi mom";
    string a = str_new(cstr, 6);
    string b = str_new(cstr, 6);
    string c = str_new(cstr, 5);

    TEST_ASSERT(str_equal(a, b) != 0);
    TEST_ASSERT(str_equal(a, c) == 0);

    cstr = "";
    a = str_new(cstr, 0);
    b = empty;
    TEST_ASSERT(str_equal(a, b) != 0);
}

void test_string_compare(void)
{
    string a1 = STR_LIT("aaaa");
    string a2 = STR_LIT("aaaa");
    string a3 = STR_LIT("aaaaa");

    TEST_ASSERT(str_compare(a1, a2) == 0);
    TEST_ASSERT(str_compare(a1, a3) < 0);
    TEST_ASSERT(str_compare(a3, a2) > 0);

    // TODO(bcall): what should this be???
    TEST_ASSERT(str_compare(a1, empty) != 0);
}

void test_string_contains(void)
{
    string a = STR_LIT("hi mom!");
    TEST_ASSERT(str_contains(a, STR_LIT("hi")) != 0);
    TEST_ASSERT(str_contains(a, STR_LIT("mom")) != 0);
    TEST_ASSERT(str_contains(a, STR_LIT("mom?")) == 0);
    TEST_ASSERT(str_contains(a, STR_LIT("hi mom?")) == 0);
    TEST_ASSERT(str_contains(a, STR_LIT(" hi")) == 0);
    TEST_ASSERT(str_contains(a, STR_LIT("!")) != 0);
    TEST_ASSERT(str_contains(a, empty) == 0);
    TEST_ASSERT(str_contains(a, STR_LIT("longer than a")) == 0);
}

void test_string_starts_ends_with(void)
{
    string a = STR_LIT("hi mom!");
    TEST_ASSERT(str_starts_with(a, STR_LIT("hi")) != 0);
    TEST_ASSERT(str_ends_with(a, STR_LIT("mom!")) != 0);
    TEST_ASSERT(str_ends_with(a, STR_LIT("mom")) == 0);

    TEST_ASSERT(str_starts_with(a, empty) == 0);
    TEST_ASSERT(str_ends_with(a, empty) == 0);
    
    TEST_ASSERT(str_starts_with(a, empty) == 0);
    TEST_ASSERT(str_ends_with(a, empty) == 0);
}


void test_string_split(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    string to_split = STR_LIT(",hi,mom,,");
    StringList splits = str_split(arena, to_split, STR_LIT(","));
    TEST_ASSERT(splits.node_count == 5);

    StringNode* node = splits.first;
    TEST_ASSERT(str_equal(node->s, str_new("\0", 0)));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, STR_LIT("hi")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, STR_LIT("mom")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_new("\0", 0)));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, str_new("\0", 0)));

    arena_release(arena);
}

void test_string_split_skip_empty(void)
{
    Arena* arena = arena_alloc((ArenaParams){0});
    string to_split = STR_LIT(",hi,mom,,");
    StringList splits = str_split_skip_empty(arena, to_split, STR_LIT(","));
    TEST_ASSERT(splits.node_count == 2);
    StringNode* node = splits.first;
    TEST_ASSERT(str_equal(node->s, STR_LIT("hi")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, STR_LIT("mom")));
    
    to_split = STR_LIT(",hi,mom,,last");
    splits = str_split_skip_empty(arena, to_split, STR_LIT(","));
    TEST_ASSERT(splits.node_count == 3);
    node = splits.first;
    TEST_ASSERT(str_equal(node->s, STR_LIT("hi")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, STR_LIT("mom")));
    node = node->next;
    TEST_ASSERT(str_equal(node->s, STR_LIT("last")));

    arena_release(arena);
}


int main(void)
{
    test_string_construction();
    test_string_equal();
    test_string_compare();
    test_string_contains();
    test_string_starts_ends_with();
    test_string_split();
    test_string_split_skip_empty();

    test_print_results("String");
}



// #include "../base_arena.c"
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


int main(void)
{
    test_string_construction();

    test_print_results("String");
}

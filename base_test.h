
#ifndef BASE_TEST_H
#define BASE_TEST_H

#include <stdio.h>

#define RED   "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

static int tests_failed = 0;
#define TEST_ASSERT(cond)                                                   \
    do {                                                                    \
        if (!(cond)) {                                                      \
            fprintf(stderr,                                                 \
                    RED "[FAIL]" RESET " %s:%d: Assertion failed: %s\n",    \
                    __FILE__, __LINE__, #cond);                             \
            tests_failed++;                                                 \
        }                                                                   \
    } while (0)

static void test_print_results(char* module_name)
{
    if (tests_failed == 0)
        printf(GREEN "Passed %s tests" RESET "\n", module_name);
    else
        printf(RED "Failed %s tests" RESET "\n", module_name);
}

#endif // BASE_TEST_H

#ifndef TEST_UTILITIES_H_
#define TEST_UTILITIES_H_

#include <stdbool.h>
#include <stdio.h>

/**
 * These macros are here to help you create tests more easily and keep them
 * clear
 *
 * The basic idea with unit-testing is create a test function for every real
 * function and inside the test function declare some variables and execute the
 * function under test.
 *
 * Use the ASSERT_TEST to verify correctness of values.
 */

/**
 * Evaluates b and continues if b is true.
 * If b is false, ends the test by returning false and prints a detailed
 * message about the failure.
 */
#define ASSERT_TEST(b) do { \
        if (!(b)) { \
                printf("\nAssertion failed at %s:%d %s ",__FILE__,__LINE__,#b); \
                return false; \
        } \
} while (0)

/**
 * Macro used for running a test from the main function
 */
#define RUN_TEST(test) do { \
        printf("Running "#test"... "); \
        if (test()) { \
            printf("[OK]\n");\
        } else { \
                printf("[Failed]\n"); \
        } \
} while(0)

#endif /* TEST_UTILITIES_H_ */

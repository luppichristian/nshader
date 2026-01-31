#include "unity.h"
#include <nshader.h>

void setUp(void) {
    // Called before each test
}

void tearDown(void) {
    // Called after each test
}

void test_nshader_test_should_return_true(void) {
    TEST_ASSERT_TRUE(nshader_test());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_nshader_test_should_return_true);
    return UNITY_END();
}
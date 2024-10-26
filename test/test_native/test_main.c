#include "unity.h"

void setUp(void)
{
    // Set up before each test
}

void tearDown(void)
{
    // Tear down after each test
}

void test_add(void)
{
    TEST_ASSERT_EQUAL(1, 1);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_add);
    return UNITY_END();
}
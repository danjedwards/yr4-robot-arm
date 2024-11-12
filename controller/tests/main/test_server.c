#include <limits.h>
#include "server.h"
#include "unity.h"

TEST_CASE("Test nothing", "[nothing]")
{
}

TEST_CASE("Test server add", "[add]")
{
    const int a = 1, b = 2;
    TEST_ASSERT_EQUAL(3, add(a, b));
}

TEST_CASE("Test server add fail!", "[add][fails]")
{
    const int a = 1, b = 2;
    TEST_ASSERT_EQUAL(0, add(a, b));
}
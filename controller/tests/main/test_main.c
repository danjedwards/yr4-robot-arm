#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "server.h"

// Kind of ugly, but the only way I could get the test cases to be found :/
#include "test_server.c"

static void print_banner(const char *text);

void app_main(void)
{
    print_banner("Running tests without [add] tag");
    UNITY_BEGIN();
    unity_run_tests_by_tag("[fails]", true);
    UNITY_END();

    add(1, 2);

    print_banner("Running tests with [add] tag");
    UNITY_BEGIN();
    unity_run_tests_by_tag("[add]", false);
    UNITY_END();

    print_banner("Running tests with [fails] tag");
    UNITY_BEGIN();
    unity_run_tests_by_tag("[fails]", false);
    UNITY_END();

    print_banner("Running all the registered tests");
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();

    print_banner("Starting interactive test menu");
    unity_run_menu();
}

static void print_banner(const char *text)
{
    printf("\n#### %s #####\n\n", text);
}

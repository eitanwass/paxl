#include "unity.h"
#include "../src/core/helpers.h"

void setUp(void) {}
void tearDown(void) {}


// skip_ws

void test_skip_ws_leading_spaces(void) {
    char str[] = "   hello";
    char* result = skip_ws(str);
    TEST_ASSERT_EQUAL_STRING("hello", result);
}

void test_skip_ws_suffix_spaces(void) {
    char str[] = "hello   ";
    char* result = skip_ws(str);
    TEST_ASSERT_EQUAL_STRING("hello   ", result);
}

void test_skip_ws_no_spaces(void) {
    char str[] = "hello";
    char* result = skip_ws(str);
    TEST_ASSERT_EQUAL_STRING("hello", result);
}

void test_skip_ws_all_spaces(void) {
    char str[] = "   ";
    char* result = skip_ws(str);
    TEST_ASSERT_EQUAL_STRING("", result);
}

void test_skip_ws_empty_string(void) {
    char str[] = "";
    char* result = skip_ws(str);
    TEST_ASSERT_EQUAL_STRING("", result);
}

// skip_to_tag_end

void test_skip_to_tag_end_with_closing(void) {
    char str[] = "<tag>";
    char* result = skip_to_tag_end(str);
    TEST_ASSERT_EQUAL_STRING("", result);
}

void test_skip_to_tag_end_without_closing(void) {
    char str[] = "<tag";
    char* result = skip_to_tag_end(str);
    TEST_ASSERT_EQUAL_STRING("", result);
}

void test_skip_to_tag_end_empty(void) {
    char str[] = "";
    char* result = skip_to_tag_end(str);
    TEST_ASSERT_EQUAL_STRING("", result);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_skip_ws_leading_spaces);
    RUN_TEST(test_skip_ws_no_spaces);
    RUN_TEST(test_skip_ws_all_spaces);
    RUN_TEST(test_skip_ws_empty_string);
    RUN_TEST(test_skip_to_tag_end_with_closing);
    RUN_TEST(test_skip_to_tag_end_without_closing);
    RUN_TEST(test_skip_to_tag_end_empty);
    return UNITY_END();
}
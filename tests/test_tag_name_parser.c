#include "unity.h"
#include "../src/parsers/tag_name_parser.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse_tag_name_normal(void) {
    char str[] = "div";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(3, len);
    TEST_ASSERT_EQUAL_STRING("", end);
}

void test_parse_tag_name_with_numbers(void) {
    char str[] = "div123";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(6, len);
    TEST_ASSERT_EQUAL_STRING("", end);
}

void test_parse_tag_name_ends_with_close_bracket(void) {
    char str[] = "div>";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(3, len);
    TEST_ASSERT_EQUAL_STRING(">", end);
}

void test_parse_tag_name_ends_with_backslash(void) {
    char str[] = "div/";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(3, len);
    TEST_ASSERT_EQUAL_STRING("/", end);
}

void test_parse_tag_name_ends_with_space(void) {
    char str[] = "div ";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(3, len);
    TEST_ASSERT_EQUAL_STRING(" ", end);
}

void test_parse_tag_name_empty_string(void) {
    char str[] = "";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(0, len);
    TEST_ASSERT_EQUAL_STRING("", end);
}

void test_parse_tag_name_starts_with_invalid(void) {
    char str[] = ">div";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(0, len);
    TEST_ASSERT_EQUAL_STRING(">div", end);
}

void test_parse_tag_name_with_null_terminator(void) {
    char str[] = "div\0other";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(3, len);
    TEST_ASSERT_EQUAL_STRING("\0other", end);
}

void test_parse_tag_name_single_char(void) {
    char str[] = "a";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(1, len);
    TEST_ASSERT_EQUAL_STRING("", end);
}

void test_parse_tag_name_mixed_case(void) {
    char str[] = "DiV";
    size_t len;
    char* end = parse_tag_name(str, &len);
    TEST_ASSERT_EQUAL(3, len);
    TEST_ASSERT_EQUAL_STRING("", end);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_tag_name_normal);
    RUN_TEST(test_parse_tag_name_with_numbers);
    RUN_TEST(test_parse_tag_name_ends_with_close_bracket);
    RUN_TEST(test_parse_tag_name_ends_with_backslash);
    RUN_TEST(test_parse_tag_name_ends_with_space);
    RUN_TEST(test_parse_tag_name_empty_string);
    RUN_TEST(test_parse_tag_name_starts_with_invalid);
    RUN_TEST(test_parse_tag_name_with_null_terminator);
    RUN_TEST(test_parse_tag_name_single_char);
    RUN_TEST(test_parse_tag_name_mixed_case);
    return UNITY_END();
}
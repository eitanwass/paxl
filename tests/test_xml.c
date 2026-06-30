#include "unity.h"
#include "../src/core/parser.h"
#include "../src/xml.h"
#include "../libs/yyjson/yyjson.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Default case: a single root element, single_root=1, so the result is
// the root element itself, not wrapped in a top-level "children" array.

void test_simple_element(void) {
    char* xml = "<root></root>";
    parse_res* json = parse(xml, 1);

    TEST_ASSERT_EQUAL_STRING(json->json_ptr, "{\"tagName\":\"root\"}");

    free(json->json_ptr);
}

void test_self_closing_element(void) {
    char* xml = "<root/>";
    parse_res* json = parse(xml, 1);

    TEST_ASSERT_EQUAL_STRING(json->json_ptr, "{\"tagName\":\"root\"}");

    free(json->json_ptr);
}

void test_element_with_text(void) {
    char* xml = "<root>hello</root>";
    parse_res* json = parse(xml, 1);

    TEST_ASSERT_EQUAL_STRING(json->json_ptr, "{\"tagName\":\"root\",\"children\":[\"hello\"]}");

    free(json->json_ptr);
}

void test_element_with_attribute(void) {
    char* xml = "<root id=\"test\"></root>";
    parse_res* json = parse(xml, 1);

    TEST_ASSERT_EQUAL_STRING(json->json_ptr, "{\"tagName\":\"root\",\"id\":\"test\"}");

    free(json->json_ptr);
}

void test_nested_elements(void) {
    char* xml = "<root><child></child></root>";
    parse_res* json = parse(xml, 1);

    TEST_ASSERT_EQUAL_STRING(json->json_ptr, "{\"tagName\":\"root\",\"children\":[{\"tagName\":\"child\"}]}");

    free(json->json_ptr);
}

void test_multiple_children(void) {
    char* xml = "<root><child1></child1><child2></child2></root>";
    parse_res* json = parse(xml, 1);

    TEST_ASSERT_EQUAL_STRING(json->json_ptr, "{\"tagName\":\"root\",\"children\":[{\"tagName\":\"child1\"},{\"tagName\":\"child2\"}]}");

    free(json->json_ptr);
}

// Special case: multiple root elements, where single_root has no effect
// and the top-level "children" wrapper is kept.

void test_multiple_root_elements_keep_wrapper(void) {
    char* xml = "<root1><child></child></root1><root2></root2>";
    parse_res* json = parse(xml, 1);

    TEST_ASSERT_EQUAL_STRING(json->json_ptr, "{\"children\":[{\"tagName\":\"root1\",\"children\":[{\"tagName\":\"child\"}]},{\"tagName\":\"root2\"}]}");

    free(json->json_ptr);
}

// Special case: single_root explicitly disabled keeps the children wrapper
// even for a single root element.

void test_single_root_disabled_keeps_children_wrapper(void) {
    char* xml = "<root><child></child></root>";
    parse_res* json = parse(xml, 0);

    TEST_ASSERT_EQUAL_STRING(json->json_ptr, "{\"children\":[{\"tagName\":\"root\",\"children\":[{\"tagName\":\"child\"}]}]}");

    free(json->json_ptr);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_element);
    RUN_TEST(test_element_with_text);
    RUN_TEST(test_element_with_attribute);
    RUN_TEST(test_self_closing_element);
    RUN_TEST(test_nested_elements);
    RUN_TEST(test_multiple_children);
    RUN_TEST(test_multiple_root_elements_keep_wrapper);
    RUN_TEST(test_single_root_disabled_keeps_children_wrapper);
    return UNITY_END();
}

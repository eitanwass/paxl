#include "parser_new.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define STACK_SIZE 256

// Helper: Skip whitespace
static inline char* skip_ws(char* p) {
    while (*p && isspace(*p)) p++;
    return p;
}

// Helper: Parse tag name, return pointer after tag name
static char* parse_tag_name(char* start, char** name_out, size_t* len_out) {
    char* p = start;
    while (*p && *p != '>' && !isspace(*p)) p++;
    *len_out = p - start;
    *name_out = start;
    return p;
}

void _parse_new(JSON_Object* root, char* xml) {
    if (!xml || !*xml || !root) return;

    // Stack for tracking open nodes during parsing
    JSON_Object* stack[STACK_SIZE] = {NULL};
    size_t stack_depth = 0;
    
    JSON_Object* current = root;

    char* cur = xml;

    while (cur && *cur) {
        cur = skip_ws(cur);
        if (!*cur) break;

        if (*cur == OPEN_BRACKET) {
            cur++;

            // Check for closing tag
            if (*cur == '/') {
                cur++;                
                // Skip to '>'
                while (*cur && *cur != '>') cur++;
                if (*cur == '>') cur++;

                // Pop from stack
                if (stack_depth > 0) {
                    stack_depth--;
                    current = stack[stack_depth];
                }
            } else if (*cur == '?' || *cur == '!') {
                // Check for self-closing or processing instruction
                // Skip processing instructions and comments
                while (*cur && *cur != '>') cur++;
                if (*cur == '>') cur++;
            } else {
                // Opening tag
                JSON_Value* arr_val = json_object_get_value(current, CHILDREN_FIELD_NAME);
                if (!arr_val) {
                    arr_val = json_value_init_array();
                    json_object_set_value(current, CHILDREN_FIELD_NAME, arr_val);
                }
                JSON_Array* children_arr = json_value_get_array(arr_val);

                JSON_Value* new_node_val = json_value_init_object();
                JSON_Object* new_node = json_value_get_object(new_node_val);

                char* node_name = NULL;
                size_t node_name_len = 0;

                cur = parse_tag_name(cur, &node_name, &node_name_len);

                JSON_Value* new_node_name = json_value_init_string_with_len(node_name, node_name_len);
                json_object_set_value(new_node, "tagName", new_node_name);

                // TODO: Parse attributes

                // Skip attributes (simple approach: skip to '>' or '/')
                bool self_closing = false;
                while (*cur && *cur != '>' && *cur != '/') cur++;
                if (*cur == '/') {
                    self_closing = true;
                    cur++;
                }
                if (*cur == '>') cur++;

                // Add to current
                json_array_append_value(children_arr, new_node_val);

                // Push to stack if not self-closing
                if (!self_closing) {
                    if (stack_depth < STACK_SIZE) {
                        stack[stack_depth] = current;
                        stack_depth++;
                        current = new_node;
                    }
                }
            }
        } else {
            // Parse content
            char* start = cur;
            while (*cur && *cur != OPEN_BRACKET) cur++;
            size_t len = cur - start;
            if (len > 0) {
                JSON_Value* arr_val = json_object_get_value(current, CHILDREN_FIELD_NAME);
                if (!arr_val) {
                    arr_val = json_value_init_array();
                    json_object_set_value(current, CHILDREN_FIELD_NAME, arr_val);
                }
                JSON_Array* children_arr = json_value_get_array(arr_val);

                JSON_Value* content_text = json_value_init_string_with_len(start, len);

                json_array_append_value(children_arr, content_text);
            }
        }
    }
}

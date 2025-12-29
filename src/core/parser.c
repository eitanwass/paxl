#include "parser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

// Helper: Parse tag name, return pointer after tag name
static char* parse_tag_name(char* start, char** name_out, size_t* len_out) {
    char* p = start;
    while (*p && *p != '>' && *p != '/' && !isspace(*p)) {
        p++;
    }
    *len_out = p - start;
    *name_out = start;
    return p;
}

// Helper: Get or create children array
static yyjson_mut_val* get_children_array(yyjson_mut_doc* doc, yyjson_mut_val* obj) {
    yyjson_mut_val* arr = yyjson_mut_obj_get(obj, CHILDREN_FIELD_NAME);
    if (!arr) {
        arr = yyjson_mut_obj_add_arr(doc, obj, CHILDREN_FIELD_NAME);
    }
    return arr;
}

// void _parse_xml(JSON_Object* root, char* xml) {
void _parse_xml(yyjson_mut_doc* doc, yyjson_mut_val* root, char* xml) {
    if (!xml || !*xml || !root) {
        return;
    }

    // Stack for tracking open nodes during parsing
    yyjson_mut_val* stack[XML_DEPTH] = {NULL};
    size_t stack_depth = 0;

    yyjson_mut_val* current = root;

    char* cur = xml;

    while (cur && *cur) {
        cur = skip_ws(cur);
        if (!*cur) {
            break;
        }

        if (*cur == OPEN_BRACKET) {
            cur++;

            // Check for closing tag
            if (*cur == '/') {
                cur++;
                // Skip to '>'
                while (*cur && *cur != CLOSE_BRACKET) {
                    cur++;
                }
                if (*cur == CLOSE_BRACKET) {
                    cur++;
                }

                // Pop from stack
                if (stack_depth > 0) {
                    stack_depth--;
                    current = stack[stack_depth];
                }
            } else if (*cur == '?' || *cur == '!') {
                // Check for self-closing or processing instruction
                // Skip processing instructions and comments
                while (*cur && *cur != CLOSE_BRACKET) {
                    cur++;
                }
                if (*cur == CLOSE_BRACKET) {
                    cur++;
                }
            } else {
                // Opening tag
                yyjson_mut_val* children_arr = get_children_array(doc, current);

                yyjson_mut_val* new_node = yyjson_mut_obj(doc);

                char* node_name = NULL;
                size_t node_name_len = 0;

                cur = parse_tag_name(cur, &node_name, &node_name_len);

                yyjson_mut_obj_add_strn(doc, new_node, "tagName", node_name, node_name_len);

                // Parse attributes
                while (*cur && *cur != CLOSE_BRACKET && *cur != '/') {
                    cur = skip_ws(cur);
                    if (*cur == CLOSE_BRACKET || *cur == '/') break;

                    // parse attribute name
                    char* attr_name_start = cur;
                    while (*cur && *cur != '=' && !isspace(*cur)) cur++;
                    size_t attr_name_len = cur - attr_name_start;

                    // skip ws and =
                    cur = skip_ws(cur);
                    if (*cur == '=') {
                        cur++;
                        cur = skip_ws(cur);
                    }

                    // parse value
                    if (*cur == '"' || *cur == '\'') {
                        char quote = *cur;
                        cur++;
                        char* attr_value_start = cur;
                        while (*cur && *cur != quote) cur++;
                        size_t attr_value_len = cur - attr_value_start;
                        if (*cur == quote) cur++;
                        // add attribute
                        yyjson_mut_val* attr_key = yyjson_mut_strn(doc, attr_name_start, attr_name_len);
                        yyjson_mut_val* attr_val = yyjson_mut_strn(doc, attr_value_start, attr_value_len);
                        yyjson_mut_obj_add(new_node, attr_key, attr_val);
                    } else {
                        // unquoted value
                        char* attr_value_start = cur;
                        while (*cur && *cur != CLOSE_BRACKET && *cur != '/' && !isspace(*cur)) cur++;
                        size_t attr_value_len = cur - attr_value_start;
                        yyjson_mut_val* attr_key = yyjson_mut_strn(doc, attr_name_start, attr_name_len);
                        yyjson_mut_val* attr_val = yyjson_mut_strn(doc, attr_value_start, attr_value_len);
                        yyjson_mut_obj_add(new_node, attr_key, attr_val);
                    }
                }

                bool self_closing = false;
                if (*cur == '/') {
                    self_closing = true;
                    cur++;
                }
                if (*cur == '>') {
                    cur++;
                }

                // Add to current
                yyjson_mut_arr_append(children_arr, new_node);

                // Push to stack if not self-closing
                if (!self_closing) {
                    if (stack_depth < sizeof(stack)) {
                        stack[stack_depth] = current;
                        stack_depth++;
                        current = new_node;
                    }
                }
            }
        } else {
            // Parse content
            char* start = cur;
            while (*cur && *cur != OPEN_BRACKET) {
                cur++;
            }
            size_t len = cur - start;
            if (len > 0) {
                yyjson_mut_val* children_arr = get_children_array(doc, current);

                yyjson_mut_arr_add_strn(doc, children_arr, start, len);
            }
        }
    }
}

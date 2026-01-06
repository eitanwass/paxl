#include "parser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "parsers/comment_parser.h"
#include "parsers/tag_name_parser.h"

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

    yyjson_mut_val* children_arr = NULL;
    yyjson_mut_val* new_node = NULL;

    // Stack for tracking open nodes during parsing
    yyjson_mut_val* stack[XML_DEPTH] = {NULL};
    size_t stack_depth = 0;

    yyjson_mut_val* current = root;

    char* cur = xml;

    char* node_name = NULL;
    size_t node_name_len = 0;

    while (cur && *cur) {
        cur = skip_ws(cur);
        if (!*cur) {
            break;
        }

        if (*cur == OPEN_BRACKET) {
            bool is_self_closing = false;

            cur++;

            switch(*cur) {
                case '/':
                    // Closing Tag
                    cur++;
                    skip_to_tag_end(cur);

                    // Pop from stack
                    if (stack_depth > 0) {
                        stack_depth--;
                        current = stack[stack_depth];
                    }
                    break;
                case '?':
                    // Processing instruction - ignore for now
                    skip_to_tag_end(cur);
                    break;
                case '!':
                    // Comment
                    parse_comment(cur);
                    skip_to_tag_end(cur);
                    break;
                default:
                    // Opening tag
                    children_arr = get_children_array(doc, current);
                    new_node = yyjson_mut_obj(doc);
                    is_self_closing = false;

                    node_name = cur;
                    cur = parse_tag_name(cur, &node_name_len);
                    yyjson_mut_obj_add_strn(doc, new_node, "tagName", node_name, node_name_len);
    
                    // Add to child list
                    yyjson_mut_arr_append(children_arr, new_node);

                    if (*cur == '/') {
                       is_self_closing = true;
                        cur++;
                    }
                    if (*cur == '>') {
                        cur++;
                    }
                    // Push to stack if not self-closing
                    if (!is_self_closing) {
                        if (stack_depth < sizeof(stack)) {
                            stack[stack_depth] = current;
                            stack_depth++;
                            current = new_node;
                        }
                    }

                    break;
            }
        } else {
            // Parse content
            char* start = cur;
            while (*cur && *cur != OPEN_BRACKET) {
                cur++;
            }
            size_t len = cur - start;
            if (len > 0) {
                children_arr = get_children_array(doc, current);

                yyjson_mut_arr_add_strn(doc, children_arr, start, len);
            }
        }
    }
}

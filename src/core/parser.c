#include "parser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "parsers/comment_parser.h"
#include "parsers/tag_name_parser.h"

// Helper: get an object's cached children array, creating it on first use.
// `cached` mirrors whatever's already on `obj` (NULL means "not created
// yet", not "known empty") so this never has to ask yyjson to look it up -
// yyjson_mut_obj_get() would otherwise rescan every existing key (tagName +
// each attribute) on *every* child/text node appended, not just the first.
static inline yyjson_mut_val* ensure_children_array(yyjson_mut_doc* doc, yyjson_mut_val* obj,
                                                      yyjson_mut_val** cached) {
    if (!*cached) {
        *cached = yyjson_mut_obj_add_arr(doc, obj, CHILDREN_FIELD_NAME);
    }
    return *cached;
}

// void _parse_xml(JSON_Object* root, char* xml) {
void _parse_xml(yyjson_mut_doc* doc, yyjson_mut_val* root, char* xml) {
    if (!xml || !*xml || !root) {
        return;
    }

    yyjson_mut_val* new_node = NULL;

    // Parallel stacks: stack_nodes tracks open elements, stack_children caches
    // each one's children array (see ensure_children_array) across pushes/pops.
    yyjson_mut_val* stack_nodes[XML_DEPTH] = {NULL};
    yyjson_mut_val* stack_children[XML_DEPTH] = {NULL};
    size_t stack_depth = 0;

    yyjson_mut_val* current = root;
    yyjson_mut_val* current_children = NULL;

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
                    cur = skip_to_tag_end(cur);

                    // Pop from stack
                    if (stack_depth > 0) {
                        stack_depth--;
                        current = stack_nodes[stack_depth];
                        current_children = stack_children[stack_depth];
                    }
                    break;
                case '?':
                    // Processing instruction - ignore for now
                    cur = skip_to_tag_end(cur);
                    break;
                case '!':
                    // Comment
                    parse_comment(cur);
                    cur = skip_to_tag_end(cur);
                    break;
                default:
                    // Opening tag
                    ensure_children_array(doc, current, &current_children);
                    new_node = yyjson_mut_obj(doc);
                    is_self_closing = false;

                    node_name = cur;
                    cur = parse_tag_name(cur, &node_name_len);
                    yyjson_mut_obj_add_strn(doc, new_node, "tagName", node_name, node_name_len);

                    // Add to child list
                    yyjson_mut_arr_append(current_children, new_node);

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

                    if (*cur == '/') {
                       is_self_closing = true;
                        cur++;
                    }
                    if (*cur == '>') {
                        cur++;
                    }
                    // Push to stack if not self-closing
                    if (!is_self_closing) {
                        if (stack_depth < sizeof(stack_nodes) / sizeof(stack_nodes[0])) {
                            stack_nodes[stack_depth] = current;
                            stack_children[stack_depth] = current_children;
                            stack_depth++;
                            current = new_node;
                            current_children = NULL;
                        }
                    }

                    break;
            }
        } else {
            // Parse content
            char* start = cur;
            char* end = cur;
            size_t len = 0;
            while (*cur && *cur != OPEN_BRACKET) {
                cur++;
            }
            end = cur;
            while (end > start && isspace(*(end-1))) {
                end--;
            }
            len = end - start;
            if (len > 0) {
                ensure_children_array(doc, current, &current_children);

                yyjson_mut_arr_add_strn(doc, current_children, start, len);
            }
        }
    }
}

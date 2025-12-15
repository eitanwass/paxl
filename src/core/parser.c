#include "parser.h"
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

void _parse(XmlNode* root, char* xml) {
    if (!xml || !*xml || !root) return;

    // Stack for tracking open nodes during parsing
    XmlNode* stack[STACK_SIZE] = {NULL};
    size_t stack_depth = 0;
    
    XmlNode* current = root;

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
            }
            // Check for self-closing or processing instruction
            else if (*cur == '?' || *cur == '!') {
                // Skip processing instructions and comments
                while (*cur && *cur != '>') cur++;
                if (*cur == '>') cur++;
            }
            else {
                // Opening tag
                XmlNode* new_node = (XmlNode*)malloc(sizeof(XmlNode));
                memset(new_node, 0, sizeof(XmlNode));

                cur = parse_tag_name(cur, &(new_node->name), &(new_node->name_len));

                // TODO: Parse attributes

                // Skip attributes (simple approach: skip to '>' or '/')
                bool self_closing = false;
                while (*cur && *cur != '>' && *cur != '/') cur++;
                if (*cur == '/') {
                    self_closing = true;
                    cur++;
                }
                if (*cur == '>') cur++;

                // Null-terminate the tag name now that cur is past it
                char* tag_end = new_node->name + new_node->name_len;
                *tag_end = '\0';

                // Add to current
                current->children = (XmlNode**)realloc(
                    current->children,
                    (current->child_count + 1) * sizeof(XmlNode*)
                );
                current->children[current->child_count] = new_node;
                current->child_count++;

                // Push to stack if not self-closing
                if (!self_closing) {
                    if (stack_depth < STACK_SIZE) {
                        stack[stack_depth] = current;
                        stack_depth++;
                        current = new_node;
                    }
                }
            }
        }
        else {
            // Parse content
            char* start = cur;
            while (*cur && *cur != OPEN_BRACKET) cur++;
            size_t len = cur - start;
            if (len > 0) {
                current->text = malloc(len + 1);
                memcpy(current->text, start, len);
                current->text[len] = '\0';
                current->text_len = len;
            }
        }
    }
}

void free_node(XmlNode* node) {
    if (!node) return;
    
    for (size_t i = 0; i < node->child_count; i++) {
        free_node(node->children[i]);
    }
    free(node->children);
    // Don't free node->name, it's part of the xml string
    free(node->text);
    free(node);
}

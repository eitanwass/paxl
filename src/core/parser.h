#ifndef __PARSER_H__
#define __PARSER_H__

#include <stddef.h>

#define OPEN_BRACKET '<'

typedef struct xml_node_t XmlNode;

typedef struct xml_node_t {
    char* name;
    size_t name_len;
    char* text;
    size_t text_len;
    struct xml_node_t** children;
    size_t child_count;
} XmlNode;

void _parse(XmlNode* root, char* xml);

void free_node(XmlNode* node);

#endif  // __PARSER_H__

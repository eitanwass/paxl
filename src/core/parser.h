#ifndef __PARSER_H__
#define __PARSER_H__

#include <stddef.h>
// #include "parson.h"
#include "yyjson.h"

#ifndef XML_DEPTH
#define XML_DEPTH (256)
#endif  // XML_DEPTH

#define OPEN_BRACKET '<'
#define CLOSE_BRACKET '>'

#define CHILDREN_FIELD_NAME "children"

// void _parse_xml(JSON_Object* root, char* xml);
void _parse_xml(yyjson_mut_doc* doc, yyjson_mut_val* root, char* xml);

#endif  // __PARSER_H__

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stddef.h>
#include "parson.h"

#define OPEN_BRACKET '<'

#define CHILDREN_FIELD_NAME "children"

void _parse_new(JSON_Object* root, char* xml);

#endif  // __PARSER_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parson.h"
#include <emscripten/emscripten.h>

#include "parser_new.h"


// Parse XML string and return JS object handle
char* parse_new(char* xml) {
    char* json_str = NULL;
    JSON_Value* root = NULL;
    JSON_Object* root_obj = NULL;

    if (!xml || *xml == '\0') return NULL;

    root = json_value_init_object();
    root_obj = json_value_get_object(root);

    // Parse XML into tree structure
    _parse_new(root_obj, xml);

    json_str = json_serialize_to_string(root);

    json_value_free(root);
    
    return json_str;
}

void free_json(char *ptr) {
    json_free_serialized_string(ptr);
}

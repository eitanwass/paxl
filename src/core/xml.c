#ifdef STANDALONE
#include <stdio.h>
#endif  // STANDALONE

#include <stdlib.h>
#include "parson.h"
#include <emscripten/emscripten.h>

#include "parser.h"


EMSCRIPTEN_KEEPALIVE
char* parse(char* xml) {
    char* json_str = NULL;
    JSON_Value* root = NULL;
    JSON_Object* root_obj = NULL;

    if (!xml || *xml == '\0') return NULL;

    root = json_value_init_object();
    root_obj = json_value_get_object(root);

    // Parse XML into tree structure
    _parse_xml(root_obj, xml);

    json_str = json_serialize_to_string(root);

    json_value_free(root);
    
    return json_str;
}

EMSCRIPTEN_KEEPALIVE
void free_json(char *ptr) {
    json_free_serialized_string(ptr);
}

#ifdef STANDALONE
int main() {
    char* ptr = parse("<xml><child1>content1</child1><child2>content2</child2><emptyChild/><childNoContent></childNoContent></xml>");
    printf("%s\n", ptr);
    free_json(ptr);
}
#endif  // STANDALONE

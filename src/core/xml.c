#ifdef STANDALONE
#include <stdio.h>
#endif  // STANDALONE

#include <stdlib.h>
#include <emscripten/emscripten.h>

#include "parser.h"
// #include "parson.h"
#include "yyjson.h"


EMSCRIPTEN_KEEPALIVE
char* parse(char* xml) {
    char* json_str = NULL;
    yyjson_mut_doc* doc = NULL;
    yyjson_mut_val* root = NULL;

    // JSON_Value* root = NULL;
    // JSON_Object* root_obj = NULL;

    if (!xml || *xml == '\0') return NULL;

    doc = yyjson_mut_doc_new(NULL);
    root = yyjson_mut_obj(doc);

    yyjson_mut_doc_set_root(doc, root);

    // root = json_value_init_object();
    // root_obj = json_value_get_object(root);

    // Parse XML into tree structure
    _parse_xml(doc, root, xml);

    json_str = yyjson_mut_write(doc, 0, NULL);

    // json_str = json_serialize_to_string(root);

    // json_value_free(root);
    
    return json_str;
}

EMSCRIPTEN_KEEPALIVE
void free_json(const char* ptr) {
    free((void*) ptr);
    // json_free_serialized_string(ptr);
}

#ifdef STANDALONE
int main() {
    char* ptr = parse("<xml><child1>content1</child1><child2>content2</child2><emptyChild/><childNoContent></childNoContent></xml>");
    printf("%s\n", ptr);
    free_json(ptr);
}
#endif  // STANDALONE
